//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnNssComp.cpp - NWN Script compiler/decompiler |
//
// This module contains the Nwn script compiler/decompiler.
//
// Copyright (c) 2002-2003 - Edward T. Smith
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are 
// met:
// 
// 1. Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
// 2. Neither the name of Edward T. Smith nor the names of its contributors 
//    may be used to endorse or promote products derived from this software 
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// @end
//
// $History: Cnf.cpp $
//      
//-----------------------------------------------------------------------------

#include <math.h>
#include "../_NscLib/Nsc.h"
#include "../_NwnLib/NwnModuleFile.h"
#include "../_NwnLib/NwnStdLoader.h"

#ifdef _WIN32
#include <atlbase.h>
#include <io.h>
#endif
#ifdef HAVE_LIBIBERTY_H
#include <libiberty.h>
#elif HAVE_LIBGEN_H
#include <libgen.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

//
// Compile flags
//

bool g_bQuiet = false;
bool g_fCompile = true;
bool g_fExtract = false;
bool g_fOptimize = false;
bool g_fEnableExtensions = false;
int g_nTest = 0;
int g_nVersion = 999999;

//
// Test flags
//

int g_nFailures = 0;
int g_nMismatches = 0;
int g_nCount = 0;
int g_nOldSize = 0;
int g_nNewSize = 0;

//
// Other globals
//

CNwnStdLoader g_sLoader;

//
// Callbacks
//

typedef void (FN_EBMCALLBACK) (const char *pszName, int nIndex);

//
// List of known scripts that won't compile
//

char *g_apszBadScripts [] = 
{
    "nwscript",
    "nw_c2_wnattack5",
    "nw_c2_wnattack8",
    "nw_c2_wnshout2",
    "nw_c2_wnshout5",
    "nw_c2_wnshout8",
    "nw_c3_blockerd",
    "nw_c3_waypoint1",
    "nw_c3_waypoint4",
    "nw_ch_fighter1",
    "nw_ci_lv203",
    "nw_ci_lv123",
    "nw_cd_lv73",
    "nw_cd_lv23",
    "nw_cd_lv203",
    "nw_cd_lv123",
    "nw_cb_lv203",
    "nw_cb_lv123",
    "nw_c8_lv203",
    "designinclude",
    "nw_s0_sphchaos",
    "nw_s0_shldlaw",
    "nw_s0_shades",
    "nw_s0_shadconj",
    "nw_s0_rmvblddef",
    "nw_s0_prlaw",
    "nw_s0_prchaos",
    "nw_s0_mislead",
    "nw_s0_mirrimage",
    "nw_s0_masdomn",
    "nw_s0_grshconj",
    "nw_s0_clokchaos",
    "nw_e0_default4",
    "nw_e0_default2",
    "nw_d2_racesnh",
    "nw_d2_races",
    "nw_d2_inl9",
    "nw_d2_ing8wig13",
    "nw_d2_ing6",
    "nw_d2_chg13inl9",
    "nw_d2_chg13ing8",
    "nw_d1_fthostile",
    "nw_i0_2q4luskan",
    "nw_i0_generic",
    "nw_i0_henchman",
    "nw_i0_henchman",
    "nw_i0_plot",
    "nw_i0_spells",
    "nw_i0_tool",
    "nw_j_artifact",
    "nw_j_assassin",
    "nw_j_complex",
    "nw_o2_coninclude",
    "nw_o0_itemmaker",
    "nw_j_theft",
    "nw_j_story",
    "nw_j_rescue",
    "nw_j_guard",
    "nw_j_fetch",
    "nw_i0_assoc",

    // NON-FUNCTIONAL
    "nw_s0_1carrion",
    NULL,
};

//
//MAYBEFIX
//
// 1. Make symbol searches type specified
// 2. Generate a warning on NULL control statements
//

//-----------------------------------------------------------------------------
//
// @func Enum the scripts
//
// @parm FN_EBMCALLBACK | pfnCallback | Routine to be invoked
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void EnumScripts (FN_EBMCALLBACK *pfnCallback)
{
    CNwnKeyFile &sKeyFile = *g_sLoader .GetNthKeyFile (0);

    //
    // Loop through the resources
    //

    for (int i = 0; i < sKeyFile .GetResCount (); i++)
        //      for (int i = 178; i <= 178; i++)
    {

        //
        // Skip if not a model
        //

        const CNwnKeyFile::Resource *psRes = sKeyFile .GetRes (i);
        if (g_fCompile && psRes ->usType != NwnResType_NSS)
            continue;

        //
        // Print the name of the model as we process
        //

        char szName [17];
        memmove (szName, psRes ->szName, 16);
        szName [16] = 0;

        //
        // Invoke routine
        //

        pfnCallback (szName, i);
    }
}

//-----------------------------------------------------------------------------
//
// @func Get a 4 byte long from the data
//
// @parm unsigned char * | pData | Pointer to the data
//
// @parm unsigned long * | pul | Pointer to the destination
//
// @rdesc Updated address.
//
//-----------------------------------------------------------------------------

static unsigned char *GetUINT32 (unsigned char *pData, unsigned long *pul)
{
    *pul = CNwnByteOrder<UINT32>::BigEndian (pData);
    return &pData [4];
}

//-----------------------------------------------------------------------------
//
// @func Get a 2 byte long from the data
//
// @parm unsigned char * | pData | Pointer to the data
//
// @parm unsigned long * | pul | Pointer to the destination
//
// @rdesc Updated address.
//
//-----------------------------------------------------------------------------

static unsigned char *GetUINT16 (unsigned char *pData, unsigned long *pul)
{
    *pul = CNwnByteOrder<UINT16>::BigEndian (pData);
    return &pData [2];
}

//-----------------------------------------------------------------------------
//
// @func Get a floating point value from the data
//
// @parm unsigned char * | pData | Pointer to the data
//
// @parm unsigned long * | pul | Pointer to the destination
//
// @rdesc Updated address.
//
//-----------------------------------------------------------------------------

static unsigned char *GetFLOAT (unsigned char *pData, float *pf)
{
    *pf = CNwnByteOrder<float>::BigEndian (pData);
    return &pData [4];
}

//-----------------------------------------------------------------------------
//
// @func Compare two scripts
//
// @parm unsigned char * | pauch1Data | Start of the data for script 1
//
// @parm uunsigned long | ul1Size | Size of script 1
//
// @parm unsigned char * | pauch2Data | Start of the data for script 2
//
// @parm uunsigned long | ul2Size | Size of script 2
//
// @rdesc TRUE if the scripts match
//
//-----------------------------------------------------------------------------

bool ScriptCompare (
    unsigned char *pauch1Data, unsigned long ul1Size,
    unsigned char *pauch2Data, unsigned long ul2Size)
{

    //
    // Loop through the data
    //

    unsigned char *p1Start = pauch1Data;
    unsigned char *p1End = &pauch1Data [ul1Size];
    unsigned char *p1Data = p1Start;
    p1Data += 8 + 5;
    unsigned char *p2Start = pauch2Data;
    unsigned char *p2End = &pauch2Data [ul2Size];
    unsigned char *p2Data = p2Start;
    p2Data += 8 + 5;
    while (p1Data < p1End && p2Data < p2End)
    {

        //
        // Compare the opcode and type
        //

        size_t nOffset = p1Data - p1Start;
        unsigned char cOp = *p1Data;
        if (*p1Data++ != *p2Data++)
        {
            printf ("MISMATCH - opcodes differ at %08X\n", nOffset);
            return false;
        }
        unsigned char cOpType = *p1Data;
        if (*p1Data++ != *p2Data++)
        {
            printf ("MISMATCH - optypes differ at %08X\n", nOffset);
            return false;
        }

        //
        // Switch based on the opcode
        //

        const char *pszOpRoot;
        switch (cOp)
        {

            case NscCode_CPDOWNSP:
                {
                    unsigned long ul1_1, ul1_2;
                    p1Data = GetUINT32 (p1Data, &ul1_1);
                    p1Data = GetUINT16 (p1Data, &ul1_2);
                    unsigned long ul2_1, ul2_2;
                    p2Data = GetUINT32 (p2Data, &ul2_1);
                    p2Data = GetUINT16 (p2Data, &ul2_2);
                    if (ul1_1 != ul2_1 || ul1_2 != ul2_2)
                    {
                        printf ("MISMATCH - CPDOWNSP parameters differ at %08X\n", nOffset);
                        return false;
                    }
                }
                break;

            case NscCode_CPTOPSP:
                {
                    unsigned long ul1_1, ul1_2;
                    p1Data = GetUINT32 (p1Data, &ul1_1);
                    p1Data = GetUINT16 (p1Data, &ul1_2);
                    unsigned long ul2_1, ul2_2;
                    p2Data = GetUINT32 (p2Data, &ul2_1);
                    p2Data = GetUINT16 (p2Data, &ul2_2);
                    if (ul1_1 != ul2_1 || ul1_2 != ul2_2)
                    {
                        printf ("MISMATCH - CPTOPSP parameters differ at %08X\n", nOffset);
                        return false;
                    }
                }
                break;

            case NscCode_CONST:
                {
                    switch (cOpType)
                    {
                        case 3:
                        case 6:
                            {
                                unsigned long ul1_1;
                                p1Data = GetUINT32 (p1Data, &ul1_1);
                                unsigned long ul2_1;
                                p2Data = GetUINT32 (p2Data, &ul2_1);
                                if (ul1_1 != ul2_1)
                                {
                                    printf ("MISMATCH - CONSTI/CONSTO parameters differ at %08X\n", nOffset);
                                    return false;
                                }
                            }
                            break;

                        case 4:
                            {
                                float f1;
                                p1Data = GetFLOAT (p1Data, &f1);
                                float f2;
                                p2Data = GetFLOAT (p2Data, &f2);
                                if (fabs (f1 - f2) > 0.00001 && 
                                    fabs ((f1 - f2) / (f1 + f2)) > 0.00001)
                                {
                                    printf ("MISMATCH - CONSTF parameters differ at %08X\n", nOffset);
                                    return false;
                                }
                            }
                            break;

                        case 5:
                            {
                                unsigned long ul1_1;
                                p1Data = GetUINT16 (p1Data, &ul1_1);
                                unsigned long ul2_1;
                                p2Data = GetUINT16 (p2Data, &ul2_1);
                                if (ul1_1 != ul2_1 || memcmp (p1Data, p2Data, ul1_1) != 0)
                                {
                                    printf ("MISMATCH - CONSTS parameters differ at %08X\n", nOffset);
                                    return false;
                                }
                                p1Data += ul1_1;
                                p2Data += ul2_1;
                            }
                            break;
                    }
                }
                break;

            case NscCode_ACTION:
                {
                    unsigned long ul1_1, ul1_2;
                    p1Data = GetUINT16 (p1Data, &ul1_1);
                    ul1_2 = *p1Data++;
                    unsigned long ul2_1, ul2_2;
                    p2Data = GetUINT16 (p2Data, &ul2_1);
                    ul2_2 = *p2Data++;
                    if (ul1_1 != ul2_1 || ul1_2 != ul2_2)
                    {
                        printf ("MISMATCH - ACTION parameters differ at %08X\n", nOffset);
                        return false;
                    }
                }
                break;

            case NscCode_LOGAND:
                pszOpRoot = "LOGAND";
do_binary_operator:;
                   {
                       if (cOpType == 0x24)
                       {
                           unsigned long ul1_1;
                           p1Data = GetUINT16 (p1Data, &ul1_1);
                           unsigned long ul2_1;
                           p2Data = GetUINT16 (p2Data, &ul2_1);
                           if (ul1_1 != ul2_1)
                           {
                               printf ("MISMATCH - %s parameters differ at %08X\n", pszOpRoot, nOffset);
                               return false;
                           }
                       }
                   }
                   break;

            case NscCode_LOGOR:
                   pszOpRoot = "LOGOR";
                   goto do_binary_operator;

            case NscCode_INCOR:
                   pszOpRoot = "INCOR";
                   goto do_binary_operator;

            case NscCode_EXCOR:
                   pszOpRoot = "EXCOR";
                   goto do_binary_operator;

            case NscCode_BOOLAND:
                   pszOpRoot = "BOOLAND";
                   goto do_binary_operator;

            case NscCode_EQUAL:
                   pszOpRoot = "EQUAL";
                   goto do_binary_operator;

            case NscCode_NEQUAL:
                   pszOpRoot = "NEQUAL";
                   goto do_binary_operator;

            case NscCode_GEQ:
                   pszOpRoot = "GEQ";
                   goto do_binary_operator;

            case NscCode_GT:
                   pszOpRoot = "GT";
                   goto do_binary_operator;

            case NscCode_LT:
                   pszOpRoot = "LT";
                   goto do_binary_operator;

            case NscCode_LEQ:
                   pszOpRoot = "LEQ";
                   goto do_binary_operator;

            case NscCode_SHLEFT:
                   pszOpRoot = "SHLEFT";
                   goto do_binary_operator;

            case NscCode_SHRIGHT:
                   pszOpRoot = "SHRIGHT";
                   goto do_binary_operator;

            case NscCode_USHRIGHT:
                   pszOpRoot = "USHRIGHT";
                   goto do_binary_operator;

            case NscCode_ADD:
                   pszOpRoot = "ADD";
                   goto do_binary_operator;

            case NscCode_SUB:
                   pszOpRoot = "SUB";
                   goto do_binary_operator;

            case NscCode_MUL:
                   pszOpRoot = "MUL";
                   goto do_binary_operator;

            case NscCode_DIV:
                   pszOpRoot = "DIV";
                   goto do_binary_operator;

            case NscCode_MOD:
                   pszOpRoot = "MOD";
                   goto do_binary_operator;

            case NscCode_MOVSP:
                   {
                       unsigned long ul1_1;
                       p1Data = GetUINT32 (p1Data, &ul1_1);
                       unsigned long ul2_1;
                       p2Data = GetUINT32 (p2Data, &ul2_1);
                       if (ul1_1 != ul2_1)
                       {
                           printf ("MISMATCH - MOVSP parameters differ at %08X\n", nOffset);
                           return false;
                       }
                   }
                   break;

            case NscCode_JMP:
                   {
                       unsigned long ul1_1;
                       p1Data = GetUINT32 (p1Data, &ul1_1);
                       unsigned long ul2_1;
                       p2Data = GetUINT32 (p2Data, &ul2_1);
                       if (ul1_1 != ul2_1)
                       {
                           printf ("MISMATCH - JMP parameters differ at %08X\n", nOffset);
                           return false;
                       }
                   }
                   break;

            case NscCode_JSR:
                   {
                       unsigned long ul1_1;
                       p1Data = GetUINT32 (p1Data, &ul1_1);
                       unsigned long ul2_1;
                       p2Data = GetUINT32 (p2Data, &ul2_1);
                       if (ul1_1 != ul2_1)
                       {
                           printf ("MISMATCH - JSR parameters differ at %08X\n", nOffset);
                           return false;
                       }
                   }
                   break;

            case NscCode_JZ:
                   {
                       unsigned long ul1_1;
                       p1Data = GetUINT32 (p1Data, &ul1_1);
                       unsigned long ul2_1;
                       p2Data = GetUINT32 (p2Data, &ul2_1);
                       if (ul1_1 != ul2_1)
                       {
                           printf ("MISMATCH - JZ parameters differ at %08X\n", nOffset);
                           return false;
                       }
                   }
                   break;

            case NscCode_DESTRUCT:
                   {
                       unsigned long ul1_1, ul1_2, ul1_3;
                       p1Data = GetUINT16 (p1Data, &ul1_1);
                       p1Data = GetUINT16 (p1Data, &ul1_2);
                       p1Data = GetUINT16 (p1Data, &ul1_3);
                       unsigned long ul2_1, ul2_2, ul2_3;
                       p2Data = GetUINT16 (p2Data, &ul2_1);
                       p2Data = GetUINT16 (p2Data, &ul2_2);
                       p2Data = GetUINT16 (p2Data, &ul2_3);
                       if (ul1_1 != ul2_1 || ul1_2 != ul2_2 || ul1_3 != ul2_3)
                       {
                           printf ("MISMATCH - DESTRUCT parameters differ at %08X\n", nOffset);
                           return false;
                       }
                   }
                   break;

            case NscCode_DECISP:
                   {
                       unsigned long ul1_1;
                       p1Data = GetUINT32 (p1Data, &ul1_1);
                       unsigned long ul2_1;
                       p2Data = GetUINT32 (p2Data, &ul2_1);
                       if (ul1_1 != ul2_1)
                       {
                           printf ("MISMATCH - DECISP parameters differ at %08X\n", nOffset);
                           return false;
                       }
                   }
                   break;

            case NscCode_INCISP:
                   {
                       unsigned long ul1_1;
                       p1Data = GetUINT32 (p1Data, &ul1_1);
                       unsigned long ul2_1;
                       p2Data = GetUINT32 (p2Data, &ul2_1);
                       if (ul1_1 != ul2_1)
                       {
                           printf ("MISMATCH - INCISP parameters differ at %08X\n", nOffset);
                           return false;
                       }
                   }
                   break;

            case NscCode_JNZ:
                   {
                       unsigned long ul1_1;
                       p1Data = GetUINT32 (p1Data, &ul1_1);
                       unsigned long ul2_1;
                       p2Data = GetUINT32 (p2Data, &ul2_1);
                       if (ul1_1 != ul2_1)
                       {
                           printf ("MISMATCH - JNZ parameters differ at %08X\n", nOffset);
                           return false;
                       }
                   }
                   break;

            case NscCode_CPDOWNBP:
                   {
                       unsigned long ul1_1, ul1_2;
                       p1Data = GetUINT32 (p1Data, &ul1_1);
                       p1Data = GetUINT16 (p1Data, &ul1_2);
                       unsigned long ul2_1, ul2_2;
                       p2Data = GetUINT32 (p2Data, &ul2_1);
                       p2Data = GetUINT16 (p2Data, &ul2_2);
                       if (ul1_1 != ul2_1 || ul1_2 != ul2_2)
                       {
                           printf ("MISMATCH - CPDOWNBP parameters differ at %08X\n", nOffset);
                           return false;
                       }
                   }
                   break;

            case NscCode_CPTOPBP:
                   {
                       unsigned long ul1_1, ul1_2;
                       p1Data = GetUINT32 (p1Data, &ul1_1);
                       p1Data = GetUINT16 (p1Data, &ul1_2);
                       unsigned long ul2_1, ul2_2;
                       p2Data = GetUINT32 (p2Data, &ul2_1);
                       p2Data = GetUINT16 (p2Data, &ul2_2);
                       if (ul1_1 != ul2_1 || ul1_2 != ul2_2)
                       {
                           printf ("MISMATCH - CPTOPBP parameters differ at %08X\n", nOffset);
                           return false;
                       }
                   }
                   break;

            case NscCode_DECIBP:
                   {
                       unsigned long ul1_1;
                       p1Data = GetUINT32 (p1Data, &ul1_1);
                       unsigned long ul2_1;
                       p2Data = GetUINT32 (p2Data, &ul2_1);
                       if (ul1_1 != ul2_1)
                       {
                           printf ("MISMATCH - DECIBP parameters differ at %08X\n", nOffset);
                           return false;
                       }
                   }
                   break;

            case NscCode_INCIBP:
                   {
                       unsigned long ul1_1;
                       p1Data = GetUINT32 (p1Data, &ul1_1);
                       unsigned long ul2_1;
                       p2Data = GetUINT32 (p2Data, &ul2_1);
                       if (ul1_1 != ul2_1)
                       {
                           printf ("MISMATCH - INCIBP parameters differ at %08X\n", nOffset);
                           return false;
                       }
                   }
                   break;

            case NscCode_STORE_STATE:
                   {
                       unsigned long ul1_1, ul1_2;
                       p1Data = GetUINT32 (p1Data, &ul1_1);
                       p1Data = GetUINT32 (p1Data, &ul1_2);
                       unsigned long ul2_1, ul2_2;
                       p2Data = GetUINT32 (p2Data, &ul2_1);
                       p2Data = GetUINT32 (p2Data, &ul2_2);
                       if (ul1_1 != ul2_1 || ul1_2 != ul2_2)
                       {
                           printf ("MISMATCH - STORE_STATE parameters differ at %08X\n", nOffset);
                           return false;
                       }
                   }
                   break;
        }
    }
    if (p1Data < p1End)
    {
        printf ("MISMATCH - New longer\n");
        return false;
    }
    else if (p2Data < p2End)
    {
        printf ("MISMATCH - Original longer\n");
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
//
// @func Compare two debug files
//
// @parm unsigned char * | pauchData | Start of the data for file
//
// @parm unsigned long | ulSize | Size of file
//
// @parm bool | fAllocated | Is the resource allocated
//
// @parm CNwnMemoryStream & | sDbg | Compare stream
//
// @rdesc TRUE if the files match
//
//-----------------------------------------------------------------------------

bool DebugCompare (unsigned char *pauchData, unsigned long ulSize,
                   bool fAllocated, CNwnMemoryStream &sDbg)
{
    static const int nLineSize = 0x8000;
    bool fSuccess = true;

    //
    // Place the loaded dbg file into a stream
    //

    CNwnMemoryStream sCmp ("", pauchData, ulSize, fAllocated);

    //
    // Reposition the file to the start
    //

    sDbg .SeekFromBegining (0);
    char *pszDebug = (char *) sDbg .GetData ();

    //
    // Create the temp line buffers
    //

    char *pszLine1 = new char [nLineSize];
    char *pszLine2 = new char [nLineSize];
    int nLine = 1;

    //
    // For right now, read the first two lines
    //

    sCmp .ReadLine (pszLine1, nLineSize);
    sCmp .ReadLine (pszLine1, nLineSize);
    sDbg .ReadLine (pszLine2, nLineSize);
    sDbg .ReadLine (pszLine2, nLineSize);
    nLine += 2;

    //
    // Loop through the lines
    //

    for (;;)
    {

        //
        // Read the next two lines
        //

        bool fHaveLine1 = sCmp .ReadLine (pszLine1, nLineSize) != NULL;
        bool fHaveLine2 = sDbg .ReadLine (pszLine2, nLineSize) != NULL;

        //
        // If the first one ran out eary
        //

        if (!fHaveLine1 && fHaveLine2)
        {
            printf ("MISMATCH_DBG - Compare debug file contains too many lines\n");
            fSuccess = false;
            break;
        }

        //
        // If the second one ran out eary
        //

        if (fHaveLine1 && !fHaveLine2)
        {
            printf ("MISMATCH_DBG - Compare debug file contains too few lines\n");
            fSuccess = false;
            break;
        }

        //
        // If the both ran out, break
        //

        if (!fHaveLine1 && !fHaveLine2)
            break;

        //
        // Compare the strings
        //

        if (strcmp (pszLine1, pszLine2) != 0)
        {
            printf ("MISMATCH_DBG - Different text on line %d\n", nLine);
            fSuccess = false;
            break;
        }

        //
        // Next line!!
        //

        nLine++;
    }

    //
    // Free the lines and return
    //

    delete [] pszLine1;
    delete [] pszLine2;
    return fSuccess;
}

//-----------------------------------------------------------------------------
//
// @func Enum the scripts
//
// @parm CNwnModuleFile * | pModule | Module
//
// @parm FN_EBMCALLBACK | pfnCallback | Routine to be invoked
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void EnumScripts (CNwnModuleFile *pModule, FN_EBMCALLBACK *pfnCallback)
{
    //
    // Loop through the resources
    //

    for (int i = 0; i < pModule ->GetResCount (); i++)
        //      for (int i = 178; i <= 178; i++)
    {

        //
        // Skip if not a model
        //

        const CNwnModuleFile::Resource *psRes = pModule ->GetRes (i);
        if (g_fCompile && psRes ->ulType != NwnResType_NSS)
            continue;

        //
        // Print the name of the model as we process
        //

        char szName [17];
        memmove (szName, psRes ->szName, 16);
        szName [16] = 0;

        //
        // Invoke routine
        //

        pfnCallback (szName, i);
    }
}

//-----------------------------------------------------------------------------
//
// @func Callback for test 1
//
// @parm const char * | pszName | Name of the script
//
// @parm int | nIndex | Index
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void Test1Callback (const char *pszName, int nIndex)
{

    //
    // Extract the name
    //

    char szName [32];
    strcpy (szName, pszName);

    //
    // Check for a script we can't compile (Bioware too)
    //

    if (g_nTest == 1)
    {
        for (int i = 0; g_apszBadScripts [i]; i++)
        {
            if (stricmp (szName, g_apszBadScripts [i]) == 0)
                return;
        }
    }

    //
    // Make a full name
    //

    printf ("Script %s (%d) - ", szName, nIndex);
    strcat (szName, ".nss");

    //
    // Initialize context
    //

    g_nCount++;

    //
    // Compile
    //

    CNwnMemoryStream sOut;
    CNwnMemoryStream sDbg;
    NscResult nResult = NscCompileScript (&g_sLoader, 
                                          pszName, g_nVersion, false, true, &sOut, &sDbg);

    //
    // If we have a success
    //

    if (nResult == NscResult_Success)
    {

        //
        // Load the compiled file
        //

        UINT32 ulSize;
        bool fAllocated;
        bool fSuccess = true;
        unsigned char *pauchData = g_sLoader .LoadResource (
            pszName, NwnResType_NCS, &ulSize, &fAllocated);
        if (pauchData != NULL)
        {
            size_t ulSize2 = sOut .GetLength ();
            unsigned char *pauchData2 = sOut .GetData ();
            fSuccess = ScriptCompare (pauchData, (UINT32) ulSize,
                                      pauchData2, (UINT32) ulSize2);
            if (fAllocated)
                free (pauchData);               
        }
        if (fSuccess)
        {
            unsigned char *pauchData = g_sLoader .LoadResource (
                pszName, NwnResType_NDB, &ulSize, &fAllocated);
            if (pauchData != NULL)
            {
                size_t ulSize2 = sDbg .GetLength ();
                unsigned char *pauchData2 = sDbg .GetData ();
                fSuccess = DebugCompare (pauchData, (UINT32) ulSize,
                                         fAllocated, sDbg);
            }
        }
        if (fSuccess)
            printf ("passed\n");
        else
            g_nMismatches++;
    }

    //
    // If we have an include
    //

    else if (nResult == NscResult_Include)
    {
        printf ("include\n");
    }

    //
    // Otherwise, failure
    //

    else
    {
        printf ("failed\n");
        g_nFailures++;
    }

    //
    // Print results
    //

    return;
}

//-----------------------------------------------------------------------------
//
// @func Callback for test 3
//
// @parm const char * | pszName | Name of the script
//
// @parm int | nIndex | Index
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void Test3Callback (const char *pszName, int nIndex)
{

    //
    // Extract the name
    //

    char szName [32];
    strcpy (szName, pszName);

    //
    // Make a full name
    //

    printf ("Script %s (%d) - ", szName, nIndex);
    strcat (szName, ".nss");

    //
    // Initialize context
    //

    g_nCount++;

    //
    // Compile
    //

    CNwnMemoryStream sOut;
    NscResult nResult = NscCompileScript (&g_sLoader, 
                                          pszName, g_nVersion, true, true, &sOut, NULL);

    //
    // If we have a success
    //

    if (nResult == NscResult_Success)
    {

        //
        // Load the compiled file
        //

        UINT32 ulSize;
        bool fAllocated;
        unsigned char *pauchData = g_sLoader .LoadResource (
            pszName, NwnResType_NCS, &ulSize, &fAllocated);
        if (pauchData != NULL)
        {
            g_nOldSize += ulSize;
            g_nNewSize += (int) sOut .GetLength ();
            if (fAllocated)
                free (pauchData);               
        }
        printf ("passed\n");
    }

    //
    // If we have an include
    //

    else if (nResult == NscResult_Include)
    {
        printf ("include\n");
    }

    //
    // Otherwise, failure
    //

    else
    {
        printf ("failed\n");
        g_nFailures++;
    }

    //
    // Print results
    //

    return;
}

//-----------------------------------------------------------------------------
//
// @func Main test routine for test 4
//
// @parm const char * | pszName | Name of the script
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void DoTest4 (const char *pszName)
{

    //
    // Display the name
    //

    printf ("Script %s - ", pszName);

    //
    // Initialize context
    //

    g_nCount++;

    //
    // Load the file
    //

    UINT32 ulSize;
    unsigned char *pauchData = NULL;
    pauchData = NwnLoadFile (pszName, &ulSize);
    if (pauchData == NULL)
    {
        printf ("unable to open file\n");
        g_nFailures++;
    }

    //
    // Extract the default dir
    //

    const char *pszFileName = NwnBasename (pszName);
    size_t nLength = pszFileName - pszName;

    char *pszDefDir = (char *) alloca (sizeof (char) * (nLength + 1));
    memcpy (pszDefDir, pszName, sizeof (char) * (nLength));
    pszDefDir [nLength] = 0;
    g_sLoader .SetDefaultDir (pszDefDir);

    //
    // Compile
    //

    CNwnMemoryStream sOut;
    CNwnMemoryStream sDbg;
    NscResult nResult = NscCompileScript (&g_sLoader, pszName, 
                                          pauchData, ulSize, true, g_nVersion, false, true, 
                                          &sOut, &sDbg);

    //
    // If we have a success
    //

    if (nResult == NscResult_Success)
    {

        //
        // Generate the name of the compiled file
        //

        int nFileLength = (int) strlen (pszName);
        char *pszNCS = (char *) alloca (sizeof (char) * (nFileLength + 5));
        strcpy (pszNCS, pszName);
        char *p = (char *) strrchr (NwnBasename (pszNCS), '.');
        if (p && stricmp (p, ".nss") == 0)
            strcpy (p, ".ncs");
        else
            strcat (pszNCS, ".ncs");

        //
        // Load the compare compiled file
        //

        bool fSuccess = true;
        pauchData = NwnLoadFile (pszNCS, &ulSize);

        //
        // Test the compiled file
        //

        if (pauchData != NULL)
        {
            size_t ulSize2 = sOut .GetLength ();
            unsigned char *pauchData2 = sOut .GetData ();
            fSuccess = ScriptCompare (pauchData, (UINT32) ulSize,
                                      pauchData2, (UINT32) ulSize2);
            free (pauchData);           
        }

        //
        // If we are successful
        //

        if (fSuccess)
        {

            //
            // Generate the name of the debug file
            //

            char *p = strrchr (pszNCS, '.');
            strcpy (p, ".ndb");

            //
            // Load the compare debug file
            //

            fSuccess = true;
            pauchData = NwnLoadFile (pszNCS, &ulSize);

            //
            // Test the compiled file
            //

            if (pauchData != NULL)
            {
                fSuccess = DebugCompare (pauchData, 
                                         (UINT32) ulSize, true, sDbg);
            }
        }

        //
        // Generate results
        //

        if (fSuccess)
            printf ("passed\n");
        else
            g_nMismatches++;
    }

    //
    // If we have an include
    //

    else if (nResult == NscResult_Include)
    {
        printf ("include\n");
    }

    //
    // Otherwise, failure
    //

    else
    {
        printf ("failed\n");
        g_nFailures++;
    }

    //
    // Print results
    //

    return;
}

//-----------------------------------------------------------------------------
//
// @func Do wildcard based test 4
//
// @parm const char * | pszInFile | Input file
//
// @rdesc Number of files found
//
//-----------------------------------------------------------------------------

int WildcardTest4 (const char *pszInFile)
{
#ifdef _WIN32
    struct _finddata_t sFind;

    //
    // Split the input file name up
    //

    char szDrive [_MAX_DRIVE];
    char szDir [_MAX_DIR];
    char szFileName [_MAX_FNAME];
    char szExtension [_MAX_EXT];
    _splitpath (pszInFile, szDrive, szDir, szFileName, szExtension);

    //
    // Make a root name
    //

    char szInFile [_MAX_PATH];
    strcpy (szInFile, szDrive);
    strcat (szInFile, szDir);
    int nRootLength = (int) strlen (szInFile);

    //
    // Initiate the find
    //

    intptr_t id = _findfirst (pszInFile, &sFind); 
    if (id <= 0)
        return 0;

    //
    // Loop through the files
    //

    int nCount = 0;
    do
    {

        //
        // If system, hidden, or subdir, then skip
        //

        if ((sFind .attrib & (_A_HIDDEN | _A_SYSTEM | _A_SUBDIR)) != 0)
            continue;

        //
        // Create the file name
        //

        strcpy (&szInFile [nRootLength], sFind .name);

        //
        // Invoke the test routine
        //

        DoTest4 (szInFile);
        nCount++;
    } while (_findnext (id, &sFind) >= 0);

    //
    // Close the find
    //

    _findclose (id);
    return nCount;
#else

    //
    // Invoke the test routine
    //

    DoTest4 (pszInFile);
    return 1;
#endif
}

//-----------------------------------------------------------------------------
//
// @func Make output file name
//
// @parm const char * | pszInFile | Current input file
//
// @parm const char * | pszOutFile | Default output file
//
// @parm const char * | pszToReplaceExt | Extension to replace
//
// @parm const char * | pszReplaceWithExt | Extension to replace with
//
// @parm const char * | pszDefaultExt | Default extension
//
// @rdesc Pointer to the file name
//
//-----------------------------------------------------------------------------

const char *MakeOutFile (const char *pszInFile, const char *pszOutFile,
                         const char *pszToReplaceExt, const char *pszReplaceWithExt)
{
    static char szOutName [512];

    //
    // If we don't have an output file name
    //
    if (pszOutFile == NULL || pszOutFile [0] == 0)
    {
        //
        // Start with the input file name
        //
        strcpy (szOutName, pszInFile);
        pszOutFile = szOutName;
    }

    //
    // Otherwise, we have an output file name, but it might be a directory
    //

    else
    {
        //
        // If it is just a path
        //
        int nLength = (int) strlen (pszOutFile);
        if (pszOutFile [nLength - 1] == '\\' || pszOutFile [nLength - 1] == '/')
        {
            //
            // Start with the input file name
            //
            strcpy (szOutName, pszOutFile);

            //
            // Append the base name of the input file name
            //
            strcat (szOutName, NwnBasename (pszInFile));
            pszOutFile = szOutName;
        }
        else
        {
            strcpy (szOutName, pszOutFile);
            pszOutFile = szOutName;
        }
    }

    //
    // Make the extension (needs cleaning up)
    // No Kidding - Judd
    //

    // Determine if our extension is already correct
    int nLen = (int) strlen (szOutName);
    int nLen2 = (int)strlen(pszToReplaceExt);
    if (nLen > nLen2 && !stricmp(&szOutName[nLen - nLen2],
                                 pszReplaceWithExt))
        return pszOutFile;

    // Determine if the original extension is even here
    if (nLen > nLen2 && !stricmp(&szOutName[nLen - nLen2], pszToReplaceExt))
        strcpy (&szOutName [nLen - nLen2], pszReplaceWithExt);
    else
    {
        // As a fallback, try the input file

        int nLen3 = (int)strlen(pszInFile);
        if (nLen3 > nLen2 && !stricmp(&pszInFile[nLen3 - nLen2], pszToReplaceExt))
        {
            strcpy (szOutName, pszInFile);
            pszOutFile = szOutName;

            // This should work since the above just copies
            // pszInFile into szOutName
            strcpy (&szOutName[nLen3 - nLen2],
                    pszReplaceWithExt);
        }
        else // Do the best we can
            strcat (szOutName, pszReplaceWithExt);
    }
    return pszOutFile;
}

/*
   const char *MakeOutFile (const char *pszInFile, const char *pszOutFile,
   const char *pszToReplaceExt, const char *pszReplaceWithExt)
   {
   static char szOutName [512];

//
// If we don't have an output file name
//

if (pszOutFile == NULL || pszOutFile [0] == 0)
{

//
// Start with the input file name
//

strcpy (szOutName, pszInFile);
pszOutFile = szOutName;
}

//
// Otherwise, we have an output file name, but it might be a directory
//

else
{

//
// If it is just a path
//

int nLength = (int) strlen (pszOutFile);
if (pszOutFile [nLength - 1] == '\\' ||
pszOutFile [nLength - 1] == '/')
{

//
// Start with the input file name
//

strcpy (szOutName, pszOutFile);

//
// Append the base name of the input file name
//

strcat (szOutName, NwnBasename (pszInFile));
pszOutFile = szOutName;
}
}

//
// Make the extension (needs cleaning up)
//

int nLen = (int) strlen (szOutName);
int nLen2 = (int) strlen (pszToReplaceExt);
if (nLen > nLen2 && stricmp (&szOutName 
[nLen - nLen2], pszToReplaceExt) == 0)
{
strcpy (&szOutName [nLen - nLen2], pszReplaceWithExt);
}
else
strcat (szOutName, pszReplaceWithExt);
return pszOutFile;
}
*/

//-----------------------------------------------------------------------------
//
// @func Compile a file
//
// @parm unsigned char * | pauchData | Model data
//
// @parm UINT32 | ulSize | Size of the file in bytes
//
// @parm const char * | pszInFile | Input file name
//
// @parm const char * | pszOutFile | Output file name (can be NULL)
//
// @rdesc Decompile status
//
//-----------------------------------------------------------------------------

bool Compile (unsigned char *pauchData, UINT32 ulSize, 
              const char *pszInFile, const char *pszOutFile, const bool bDebug)
{
    //
    // Issue message
    //

    if (!g_bQuiet) printf ("Compiling: %s\n", pszInFile);

    //
    // Extract the default dir
    //

    //make a copy, as basename may modify
    char *pszInFileCopy = strdup(pszInFile);
    //should really use dirname here, but whatever
    const char *pszFileName = NwnBasename (pszInFileCopy);
    //cannot rely on result being a pointer in the same string
    size_t nLength = strlen(pszInFile) - strlen(pszFileName);

    char *pszDefDir = (char *) alloca (sizeof (char) * (nLength + 1));
    memcpy (pszDefDir, pszInFile, sizeof (char) * (nLength));
    pszDefDir [nLength] = 0;
    g_sLoader .SetDefaultDir (pszDefDir);

    free(pszInFileCopy);

    //
    // Compile
    //

    CNwnMemoryStream sOut;
    CNwnMemoryStream sDbg;
    NscResult nResult = NscCompileScript (&g_sLoader, pszInFile, 
                                          pauchData, ulSize, true, g_nVersion, g_fOptimize, true, 
                                          &sOut, &sDbg);

    //
    // If we have an error or include, return
    //

    if (nResult == NscResult_Failure)
    {
        printf ("Compilation aborted with errors\n");
        return false;
    }
    else if (nResult == NscResult_Include)
    {
        printf ("File is an include file, ignored\n");
        return true;
    }

    //
    // Create the output file
    //

    pszOutFile = MakeOutFile (pszInFile, pszOutFile, ".nss", ".ncs");
    FILE *pf = fopen (pszOutFile, "wb");
    if (pf == NULL)
    {
        printf ("Error: Unable to open output file \"%s\"\n", pszOutFile);
        return false;
    }
    fwrite (sOut .GetData (), sOut .GetPosition (), 1, pf);
    fclose (pf);

    //
    // Create the debug file
    //
    if (bDebug) {
        pszOutFile = MakeOutFile (pszInFile, pszOutFile, ".ncs", ".ndb");
        pf = fopen (pszOutFile, "wb");
        if (pf == NULL)
        {
            printf ("Error: Unable to open debug file \"%s\"\n", pszOutFile);
            return false;
        }
        fwrite (sDbg .GetData (), sDbg .GetPosition (), 1, pf);
        fclose (pf);
    }

    return true;
}

//-----------------------------------------------------------------------------
//
// @func Decompile a file
//
// @parm unsigned char * | pauchData | Model data
//
// @parm UINT32 | ulSize | Size of the file in bytes
//
// @parm const char * | pszInFile | Input file name
//
// @parm const char * | pszOutFile | Output file name (can be NULL)
//
// @rdesc Decompile status
//
//-----------------------------------------------------------------------------

bool Decompile (unsigned char *pauchData, UINT32 ulSize, 
                const char *pszInFile, const char *pszOutFile)
{

    //
    // Issue message
    //

    printf ("Decompiling: %s\n", pszInFile);

    //
    // Make ASCII
    //

    CNwnMemoryStream sStream;
    NscScriptDecompile (sStream, pauchData, ulSize);

    //
    // Create the output file
    //

    pszOutFile = MakeOutFile (pszInFile, pszOutFile, ".ncs", ".pcode");
    FILE *pf = fopen (pszOutFile, "wb");
    if (pf == NULL)
    {
        printf ("Error: Unable to open output file \"%s\"\n", pszInFile);
        return false;
    }
    fwrite (sStream .GetData (), sStream .GetPosition (), 1, pf);
    fclose (pf);
    return true;
}

//-----------------------------------------------------------------------------
//
// @func Do wildcard based compile/decompile
//
// @parm const char * | pszInFile | Input file
//
// @parm const char * | pszOutFile | Output file
//
// @rdesc Number of files found
//
//-----------------------------------------------------------------------------

int Wildcard (const char *pszInFile, const char *pszOutFile, const bool bDebug)
{
    bool noError = true;

#ifdef _WIN32
    struct _finddata_t sFind;

    //
    // Split the input file name up
    //

    char szDrive [_MAX_DRIVE];
    char szDir [_MAX_DIR];
    char szFileName [_MAX_FNAME];
    char szExtension [_MAX_EXT];
    _splitpath (pszInFile, szDrive, szDir, szFileName, szExtension);

    //
    // Make a root name
    //

    char szInFile [_MAX_PATH];
    strcpy (szInFile, szDrive);
    strcat (szInFile, szDir);
    int nRootLength = (int) strlen (szInFile);

    //
    // Initiate the find
    //

    intptr_t id = _findfirst (pszInFile, &sFind); 
    if (id <= 0)
        return 0;

    //
    // Loop through the files
    //

    int nCount = 0;

    do
    {

        //
        // If system, hidden, or subdir, then skip
        //

        if ((sFind .attrib & (_A_HIDDEN | _A_SYSTEM | _A_SUBDIR)) != 0)
            continue;

        //
        // Create the file name
        //

        strcpy (&szInFile [nRootLength], sFind .name);

        //
        // Load the file
        //

        UINT32 ulSize;
        unsigned char *pauchData = NULL;
        pauchData = NwnLoadFile (szInFile, &ulSize);

        //
        // Compile/decompile
        //

        if (pauchData == NULL)
        {
            printf ("Error: Unable to open file %s\n", sFind .name);
            noError = false;
            continue;
        }

        //
        // Compile/decompile
        //

        if (g_fCompile)
            noError = noError && Compile (pauchData, ulSize, sFind .name, pszOutFile, bDebug);
        else
            noError = noError && Decompile (pauchData, ulSize, sFind .name, pszOutFile);
        nCount++;
    } while (_findnext (id, &sFind) >= 0);

    //
    // Close the find
    //

    _findclose (id);
    return noError; //not returning error currently
#else

    //
    // Load the file
    //

    UINT32 ulSize;
    unsigned char *pauchData = NULL;
    pauchData = NwnLoadFile (pszInFile, &ulSize);
    if (pauchData == NULL)
        return false;

    //
    // Compile
    //

    if (g_fCompile)
        noError = noError && Compile (pauchData, ulSize, pszInFile, pszOutFile);
    else
        noError = noError && Decompile (pauchData, ulSize, pszInFile, pszOutFile);
    return noError;
#endif
}

//-----------------------------------------------------------------------------
//
// @func Match a simple pattern
//
// @parm const char * | pszString | String to test
//
// @parm const char * | pszPattern | Pattern string
//
// @rdesc TRUE if the pattern matches
//
//-----------------------------------------------------------------------------

bool MatchPattern (const char *pszString, const char *pszPattern)
{
    for (;;)
    {
        char p = *pszPattern++;
        p = tolower (p);
        switch (p)
        {


            //
            // If end of pattern, return success if end of string reached
            //

            case 0:
                return *pszString == 0;

                //
                // If matching zero or more characters
                //

            case '*':
                while (*pszString) 
                {
                    if (MatchPattern (pszString++, pszPattern))
                        return true; 
                }
                return MatchPattern (pszString, pszPattern);

                //
                // Match any one character but not end of string
                //

            case '?':
                if (*pszString++ == 0)
                    return false;
                break; 

            default:
                {
                    char c = *pszString++;
                    c = tolower (c);
                    if (c != p)
                        return false;
                }
                break; 
        } 
    } 
}

//-----------------------------------------------------------------------------
//
// @func The main
//
// @parm int | argc | Argument count
//
// @parm char * | argv [] | Argument list
//
// @rdesc Exit status
//
//-----------------------------------------------------------------------------

int main (int argc, char *argv [])
{
    char *pszOutFile = NULL;
    char *pszNWNDir = NULL;
    char **papszInFiles = NULL;
    int nInFileCount = 0;
    bool bDebug = true;
    std::string* strSearchDirs = NULL;

    //
    // Enable leak checking
    //

#ifdef _WIN32
#if defined (_DEBUG)
    _CrtSetDbgFlag (_CrtSetDbgFlag (_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif
#endif

    //
    // Get start time
    //

#ifdef _WIN32
    DWORD dwT1 = GetTickCount ();
#endif



    //
    // Allocate an array that will hold the input file names.  On
    // a WIN32 system, this will always max out at one entry.  However
    // on a Linux system, it can be a list up to the number of arguments
    //

    papszInFiles = new char * [argc];

    //
    // Loop through the arguments
    //

    bool fError = false;
    unsigned int skip = 0;
    for (int i = 1; i < argc && !fError; i++)
    {
        if (skip > 0) {
            --skip;
            continue;
        }

        //
        // If this is a switch
        //

        if (argv [i] [0] == '-')
        {
            char *p = &argv [i] [1];
            while (*p && !fError)
            {
                char c = *p++;
                switch (tolower (c))
                {
                    case 'd':
                        g_fCompile = false;
                        break;
                    case 'c':
                        g_fCompile = true;
                        break;
                    case 'x':
                        g_fExtract = true;
                        break;
                    case 'o':
                        g_fOptimize = true;
                        break;
                    case 'e':
                        g_fEnableExtensions = true;
                        break;
                    case 'g':
                        bDebug = false;
                        break;
                    case 'q':
                        g_bQuiet = true;
                        break;
                    case 'v':
                        {
                            g_nVersion = 0;
                            while (*p)
                            {
                                char c = *p++;
                                if (isdigit (c))
                                    g_nVersion = g_nVersion * 10 + (c - '0');
                                else if (c == '.')
                                    ; // nothing
                                else
                                {
                                    printf ("Error: Invalid digit in version number\n");
                                    fError = true;
                                    break;
                                }
                            }
                        }
                        break;
                    case 't':
                        {
                            char c = *p++;
                            if (c < '1' || c > '4')
                            {
                                printf ("Error: Unrecognized test number\n");
                                fError = true;
                            }
                            else
                                g_nTest = c - '0';
                        }
                        break;
                    case 'i':
                    case 'I':
                        ++skip;

                        if (strSearchDirs == NULL)
                            strSearchDirs = new std::string(argv[i + skip]);
                        else {
                            strSearchDirs->append(";");
                            strSearchDirs->append(argv[i + skip]);
                        }

                        break;
                    default:
                        printf ("Error: Unrecognized option \"%c\"\n", *p);
                        fError = true;
                        break;
                }
            }
        }

        //
        // Otherwise, save the name
        //

#ifdef _WIN32
        else if (nInFileCount == 0)
        {
            papszInFiles [nInFileCount++] = argv [i];
        }
        else if (pszOutFile == NULL)
        {
            pszOutFile = argv [i];
        }
        else
        {
            printf ("Error: Too many arguments\n");
            fError = true;
            break;
        }
#else
        else if (pszNWNDir == NULL)
        {
            pszNWNDir = argv[i];
        }
        else
        {
            papszInFiles [nInFileCount++] = argv [i];
        }
#endif
    }

    //
    // If there is an error, display the help
    //

    if (fError || (nInFileCount == 0 && g_nTest == 0) ||
        ((g_nTest == 2 || g_nTest == 4) && nInFileCount == 0))
    {
        printf ("Usage:\n");
#ifdef _WIN32
        printf ("nwnnsscomp [-cdegoqx] [-t#] [-v#] [[-i pathspec] ...] infile [outfile]\n\n");
        printf ("  pathspec - semicolon separated list of directories to search for files.\n");
#else
        printf ("nwnnsscomp [-cdegoqx] [-t#] [-v#] nwndir infile\n\n");
        printf ("  nwndir - directory where NWN is installed.\n");
#endif
        printf ("  infile - name of the input file.\n");
#ifdef _WIN32
        printf ("  outfile - name of the output file.\n");
#endif
        printf ("  -c - Compile the script (default)\n");
        printf ("  -d - Decompile the script (can't be used with -c)\n");
        printf ("  -e - Enable non-Bioware extensions\n");
        printf ("  -g - Don't produce ndb debug file\n");
        printf ("  -i - Add Search Path\n");
        printf ("  -o - Optimize the compiled source\n");
        printf ("  -q - Silence most messages\n");
        printf ("  -x - Extract script from NWN data files\n");
        printf ("  -vx.xx - Set the version of the compiler\n");
        printf ("  -t1 - Perform a compilation test with BIF scripts\n");
        printf ("  -t2 - Perform a compilation test with the given module\n");
        printf ("  -t3 - Optimization space saving report with the given module\n");
        printf ("  -t4 - Perform a compilation test with the given file or files\n");
        exit (0);
    }

    //
    // Advertise
    //
    if (!g_bQuiet) {
        printf ("NeverWinter Nights Script Compiler/Decompiler\n");
        printf ("Copyright 2002-2003, Edward T. Smith\n");
        printf ("Copyright 2003, The Open Knights Consortium\n\n");
    }

    //
    // We must be able to open the file
    //

    if (!g_sLoader .Initialize (pszNWNDir))
    {
        printf ("Unable to locate or open Neverwinter Night\n");
        exit (1);
    }

    // Add the dir search path for includes
    if (strSearchDirs != NULL) {
        std::string::size_type pos = strSearchDirs->find(';');
        if (pos == -1)
            g_sLoader.AddSearchDir(*strSearchDirs);
        else {
            // more than one
            g_sLoader.AddSearchDir(strSearchDirs->substr(0, pos));
            do {
                ++pos;
                std::string::size_type delim = strSearchDirs->find(';', pos);
                if (delim == -1)
                    g_sLoader.AddSearchDir(strSearchDirs->substr(pos));
                else
                    g_sLoader.AddSearchDir(strSearchDirs->substr(pos, delim - pos));

                pos = delim;
            } while (pos != -1);
        }
    }

    //
    // Initialize the compiler
    //

    if (!NscCompilerInitialize (&g_sLoader, g_nVersion, g_fEnableExtensions))
        exit (1);

    //
    // If we are testing compilation with bif files
    //

    if (g_nTest == 1)
    {

        //
        // Enum the scripts
        //

        g_fCompile = true;
        EnumScripts (Test1Callback);
        printf ("Finished with %d scripts, %d failures, and %d mismatches\n",
                g_nCount, g_nFailures, g_nMismatches);
    }

    //
    // If we are testing compilation with a module
    //

    else if (g_nTest == 2)
    {

        //
        // Open the module
        //

        CNwnModuleFile sModule;
        if (!g_sLoader .OpenModule (&sModule, papszInFiles [0]))
        {
            printf ("Unable to open module \"%s\"", papszInFiles [0]);
            exit (1);
        }
        g_sLoader .SetModule (&sModule);

        //
        // Load any hak files
        //

        g_sLoader .AddModuleHaks ();

        //
        // Enum the scripts
        //

        g_fCompile = true;
        EnumScripts (&sModule, Test1Callback);
        printf ("Finished with %d scripts, %d failures, and %d mismatches\n",
                g_nCount, g_nFailures, g_nMismatches);
        g_sLoader .SetModule (NULL);
    }

    //
    // If we are testing space savings with a module
    //

    else if (g_nTest == 3)
    {

        //
        // Open the module
        //

        CNwnModuleFile sModule;
        if (!g_sLoader .OpenModule (&sModule, papszInFiles [0]))
        {
            printf ("Unable to open module \"%s\"\n", papszInFiles [0]);
            exit (1);
        }
        g_sLoader .SetModule (&sModule);


        //
        // Load any hak files
        //

        g_sLoader .AddModuleHaks ();

        //
        // Enum the scripts
        //

        g_fCompile = true;
        EnumScripts (&sModule, Test3Callback);
        printf ("Finished with %d scripts, %d failures.\n",
                g_nCount, g_nFailures);
        int nSavings = g_nOldSize - g_nNewSize;
        printf ("Old size = %d, New size = %d, Savings = %5.2f%%\n",
                g_nOldSize, g_nNewSize, (float) nSavings / (float) g_nOldSize * 100.0f);
        g_sLoader .SetModule (NULL);
    }

    //
    // If we are doing a compilation test of a directory
    //

    else if (g_nTest == 4)
    {

        //
        // Load any hak files
        //

        g_sLoader .AddModuleHaks ();

        //
        // Loop through the input files
        //

        g_fCompile = true;
        for (int i = 0; i < nInFileCount; i++)
        {

            //
            // Compile the files.  On a WIN32 system, the argument
            // will be tested for wildcards.
            //

            char *pszOrgInFile = papszInFiles [i];
            char szInFile [512];
            strcpy (szInFile, papszInFiles [i]);
            char *p = (char *) strchr (NwnBasename (szInFile), '.');
            if (p) 
                *p = 0;
            strcat (szInFile, ".nss");
            int nCount = WildcardTest4 (szInFile);
            if (nCount == 0)
            {
                printf ("Error: Unable to open input file \"%s\"\n", pszOrgInFile);
            }
        }

        //
        // Print summary
        //

        printf ("Finished with %d scripts, %d failures, and %d mismatches\n",
                g_nCount, g_nFailures, g_nMismatches);
        g_sLoader .SetModule (NULL);
    }

    //
    // If we are to extract
    //

    else if (g_fExtract)
    {

        //
        // Get to the root file name
        //

        char *pszOrgInFile = papszInFiles [0];
        char szPattern [512];
        strcpy (szPattern, papszInFiles [0]);
        char *p = (char *) strchr (NwnBasename (szPattern), '.');
        if (p) 
            *p = 0;

        //
        // Loop through the resources
        //

        CNwnKeyFile &sKeyFile = *g_sLoader .GetNthKeyFile (0);
        int nCount = 0;
        for (int n = 0; n < sKeyFile .GetResCount (); n++)
        {
            CNwnKeyFile::Resource *pRes = sKeyFile .GetRes (n);
            if ((g_fCompile && pRes ->usType != NwnResType_NSS) ||
                (!g_fCompile && pRes ->usType != NwnResType_NCS))
                continue;
            char szName [32];
            memcpy (szName, pRes ->szName, 16);
            szName [16] = 0;
            if (MatchPattern (szName, szPattern))
            {
                UINT32 ulSize;
                unsigned char *pauchData = NULL;
                pauchData = sKeyFile .LoadRes (pRes, &ulSize, NULL);
                if (pauchData == NULL)
                {
                    printf ("Error: Unable to open input file \"%s\"\n", szName);
                }
                else
                {
                    strcat (szName, g_fCompile ? ".nss" : ".ncs");
                    if (g_fCompile)
                        Compile (pauchData, ulSize, szName, pszOutFile, bDebug);
                    else
                        Decompile (pauchData, ulSize, szName, pszOutFile);
                    nCount++;
                }
            }
        }

        //
        // Issue error if nothing found
        //

        if (nCount == 0)
        {
            printf ("Error: Unable to open input file \"%s\"\n", pszOrgInFile);
            exit (1);
        }
    }

    //
    // Otherwise, normal operation
    //

    else
    {

        //
        // Load any hak files
        //

        g_sLoader .AddModuleHaks ();

        //
        // Loop through the input files
        //

        for (int i = 0; i < nInFileCount; i++)
        {

            //
            // Compile the files.  On a WIN32 system, the argument
            // will be tested for wildcards.
            //

            int nCount = 0;
            struct _stat buf;
            int result = _stat(papszInFiles[i], &buf);

            if (result != 0) {
                char szInFile [512];
                const char *tmp;
                strcpy (szInFile, papszInFiles [i]);
                tmp = NwnBasename(szInFile);
                char *p = (char *) strchr (tmp, '.');   

                if (p) {
                    *p = '\0';
                }
                strcpy (szInFile, tmp);
                strcat (szInFile, g_fCompile ? ".nss" : ".ncs");
                //note that on linux the following returns true/false 
                //indicating whether any error occurred
                //on windows it returns the number of files compiled 
                //- errors are not reported in the return code
                nCount = Wildcard (szInFile, pszOutFile, bDebug);
            }
            else
                nCount = Wildcard (papszInFiles[i], pszOutFile, bDebug);
            if (!nCount)
            {
                if (!g_bQuiet) printf ("Errors occurred in compiling \"%s\"\n", papszInFiles[i]);
                exit (1);
            }
        }
    }

    //
    // Delete the input file pointers
    //

    delete [] papszInFiles;

    //
    // Close the loader for good measure
    //

    g_sLoader .Close ();

    // Release the strdup'd memory
    if (strSearchDirs != NULL)
        delete strSearchDirs;

    //
    // Write time
    //

#ifdef _WIN32
    if (!g_bQuiet)
        printf ("Total Execution time = %d ms\n", GetTickCount () - dwT1);
#endif
}


/* vim: set sw=4: */
