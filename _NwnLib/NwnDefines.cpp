//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnDefiles.cpp - General helper routines |
//
// This module contains the general NWN helper routines.
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

#include "NwnDefines.h"

//-----------------------------------------------------------------------------
//
// @func Return the extension for the given type
//
// @parm NwnResType | nResType | Type of the resource
//
// @rdesc Pointer to the extension or NULL if unknown.
//
//-----------------------------------------------------------------------------

const char *NwnGetResTypeExtension (NwnResType nType)
{
    switch (nType)
    {
        case NwnResType_RES: return ".res";
        case NwnResType_BMP: return ".bmp";
        case NwnResType_MVE: return ".mve";
        case NwnResType_TGA: return ".tga";
        case NwnResType_WAV: return ".wav";
        case NwnResType_PLT: return ".plt";
        case NwnResType_INI: return ".ini";
        case NwnResType_BMU: return ".bmu";
        case NwnResType_MPG: return ".mpg";
        case NwnResType_TXT: return ".txt";
        case NwnResType_PLH: return ".plh";
        case NwnResType_TEX: return ".tex";
        case NwnResType_MDL: return ".mdl";
        case NwnResType_THG: return ".thg";
        case NwnResType_FNT: return ".fnt";
        case NwnResType_LUA: return ".lua";
        case NwnResType_SLT: return ".slt";
        case NwnResType_NSS: return ".nss";
        case NwnResType_NCS: return ".ncs";
        case NwnResType_MOD: return ".mod";
        case NwnResType_ARE: return ".are";
        case NwnResType_SET: return ".set";
        case NwnResType_IFO: return ".ifo";
        case NwnResType_BIC: return ".bic";
        case NwnResType_WOK: return ".wok";
        case NwnResType_2DA: return ".2da";
        case NwnResType_TLK: return ".tlk";
        case NwnResType_TXI: return ".txi";
        case NwnResType_GIT: return ".git";
        case NwnResType_BTI: return ".bti";
        case NwnResType_UTI: return ".uti";
        case NwnResType_BTC: return ".btc";
        case NwnResType_UTC: return ".utc";
        case NwnResType_DLG: return ".dlg";
        case NwnResType_ITP: return ".itp";
        case NwnResType_BTT: return ".btt";
        case NwnResType_UTT: return ".utt";
        case NwnResType_DDS: return ".dds";
        case NwnResType_UTS: return ".uts";
        case NwnResType_LTR: return ".ltr";
        case NwnResType_GFF: return ".gff";
        case NwnResType_FAC: return ".fac";
        case NwnResType_BTE: return ".bte";
        case NwnResType_UTE: return ".ute";
        case NwnResType_BTD: return ".btd";
        case NwnResType_UTD: return ".utd";
        case NwnResType_BTP: return ".btp";
        case NwnResType_UTP: return ".utp";
        case NwnResType_DTF: return ".dtf";
        case NwnResType_GIC: return ".gic";
        case NwnResType_GUI: return ".gui";
        case NwnResType_CSS: return ".css";
        case NwnResType_CCS: return ".ccs";
        case NwnResType_BTM: return ".btm";
        case NwnResType_UTM: return ".utm";
        case NwnResType_DWK: return ".dwk";
        case NwnResType_PWK: return ".pwk";
        case NwnResType_BTG: return ".btg";
        case NwnResType_UTG: return ".utg";
        case NwnResType_JRL: return ".jrl";
        case NwnResType_SAV: return ".sav";
        case NwnResType_UTW: return ".utw";
        case NwnResType_4PC: return ".4pc";
        case NwnResType_SSF: return ".ssf";
        case NwnResType_HAK: return ".hak";
        case NwnResType_NWM: return ".nwm";
        case NwnResType_BIK: return ".bik";
        case NwnResType_ERF: return ".erf";
        case NwnResType_BIF: return ".bif";
        case NwnResType_KEY: return ".key";
        case NwnResType_NDB: return ".ndb";
        case NwnResType_PTM: return ".ptm";
        case NwnResType_PTT: return ".ptt";
        case NwnResType_BAK: return ".bak";
        case NwnResType_DAT: return ".dat";
        case NwnResType_SHD: return ".shd";
        case NwnResType_XBC: return ".xbc";
        case NwnResType_WBM: return ".wbm";
        case NwnResType_MTR: return ".mtr";
        case NwnResType_KTX: return ".ktx";
        case NwnResType_TTF: return ".ttf";
        case NwnResType_SQL: return ".sql";
        case NwnResType_TML: return ".tml";
        case NwnResType_SQ3: return ".sq3";
        case NwnResType_LOD: return ".lod";
        case NwnResType_GIF: return ".gif";
        case NwnResType_PNG: return ".png";
        case NwnResType_JPG: return ".jpg";
        case NwnResType_CAF: return ".caf";
        case NwnResType_JUI: return ".jui";
        case NwnResType_CDB: return ".cdb";
        default: return NULL;
    }
}

//-----------------------------------------------------------------------------
//
// @func Return the res type based on the extension
//
// @parm const char * | pszExtension | File extension
//
// @rdesc Resource type or NwnResType_Unknown if not known.
//
//-----------------------------------------------------------------------------

NwnResType NwnGetResTypeFromExtension (const char *pszExtension)
{
    if (*pszExtension == '.')
        pszExtension++;
    if (stricmp (pszExtension, "res") == 0)
        return NwnResType_RES;
    if (stricmp (pszExtension, "bmp") == 0)
        return NwnResType_BMP;
    if (stricmp (pszExtension, "mve") == 0)
        return NwnResType_MVE;
    if (stricmp (pszExtension, "tga") == 0)
        return NwnResType_TGA;
    if (stricmp (pszExtension, "wav") == 0)
        return NwnResType_WAV;
    if (stricmp (pszExtension, "plt") == 0)
        return NwnResType_PLT;
    if (stricmp (pszExtension, "ini") == 0)
        return NwnResType_INI;
    if (stricmp (pszExtension, "bmu") == 0)
        return NwnResType_BMU;
    if (stricmp (pszExtension, "mpg") == 0)
        return NwnResType_MPG;
    if (stricmp (pszExtension, "txt") == 0)
        return NwnResType_TXT;
    if (stricmp (pszExtension, "plh") == 0)
        return NwnResType_PLH;
    if (stricmp (pszExtension, "tex") == 0)
        return NwnResType_TEX;
    if (stricmp (pszExtension, "mdl") == 0)
        return NwnResType_MDL;
    if (stricmp (pszExtension, "thg") == 0)
        return NwnResType_THG;
    if (stricmp (pszExtension, "fnt") == 0)
        return NwnResType_FNT;
    if (stricmp (pszExtension, "lua") == 0)
        return NwnResType_LUA;
    if (stricmp (pszExtension, "slt") == 0)
        return NwnResType_SLT;
    if (stricmp (pszExtension, "nss") == 0)
        return NwnResType_NSS;
    if (stricmp (pszExtension, "ncs") == 0)
        return NwnResType_NCS;
    if (stricmp (pszExtension, "mod") == 0)
        return NwnResType_MOD;
    if (stricmp (pszExtension, "are") == 0)
        return NwnResType_ARE;
    if (stricmp (pszExtension, "set") == 0)
        return NwnResType_SET;
    if (stricmp (pszExtension, "ifo") == 0)
        return NwnResType_IFO;
    if (stricmp (pszExtension, "bic") == 0)
        return NwnResType_BIC;
    if (stricmp (pszExtension, "wok") == 0)
        return NwnResType_WOK;
    if (stricmp (pszExtension, "2da") == 0)
        return NwnResType_2DA;
    if (stricmp (pszExtension, "tlk") == 0)
        return NwnResType_TLK;
    if (stricmp (pszExtension, "txi") == 0)
        return NwnResType_TXI;
    if (stricmp (pszExtension, "git") == 0)
        return NwnResType_GIT;
    if (stricmp (pszExtension, "bti") == 0)
        return NwnResType_BTI;
    if (stricmp (pszExtension, "uti") == 0)
        return NwnResType_UTI;
    if (stricmp (pszExtension, "btc") == 0)
        return NwnResType_BTC;
    if (stricmp (pszExtension, "utc") == 0)
        return NwnResType_UTC;
    if (stricmp (pszExtension, "dlg") == 0)
        return NwnResType_DLG;
    if (stricmp (pszExtension, "itp") == 0)
        return NwnResType_ITP;
    if (stricmp (pszExtension, "btt") == 0)
        return NwnResType_BTT;
    if (stricmp (pszExtension, "utt") == 0)
        return NwnResType_UTT;
    if (stricmp (pszExtension, "dds") == 0)
        return NwnResType_DDS;
    if (stricmp (pszExtension, "uts") == 0)
        return NwnResType_UTS;
    if (stricmp (pszExtension, "ltr") == 0)
        return NwnResType_LTR;
    if (stricmp (pszExtension, "gff") == 0)
        return NwnResType_GFF;
    if (stricmp (pszExtension, "fac") == 0)
        return NwnResType_FAC;
    if (stricmp (pszExtension, "bte") == 0)
        return NwnResType_BTE;
    if (stricmp (pszExtension, "ute") == 0)
        return NwnResType_UTE;
    if (stricmp (pszExtension, "btd") == 0)
        return NwnResType_BTD;
    if (stricmp (pszExtension, "utd") == 0)
        return NwnResType_UTD;
    if (stricmp (pszExtension, "btp") == 0)
        return NwnResType_BTP;
    if (stricmp (pszExtension, "utp") == 0)
        return NwnResType_UTP;
    if (stricmp (pszExtension, "dtf") == 0)
        return NwnResType_DTF;
    if (stricmp (pszExtension, "gic") == 0)
        return NwnResType_GIC;
    if (stricmp (pszExtension, "gui") == 0)
        return NwnResType_GUI;
    if (stricmp (pszExtension, "css") == 0)
        return NwnResType_CSS;
    if (stricmp (pszExtension, "ccs") == 0)
        return NwnResType_CCS;
    if (stricmp (pszExtension, "btm") == 0)
        return NwnResType_BTM;
    if (stricmp (pszExtension, "utm") == 0)
        return NwnResType_UTM;
    if (stricmp (pszExtension, "dwk") == 0)
        return NwnResType_DWK;
    if (stricmp (pszExtension, "pwk") == 0)
        return NwnResType_PWK;
    if (stricmp (pszExtension, "btg") == 0)
        return NwnResType_BTG;
    if (stricmp (pszExtension, "utg") == 0)
        return NwnResType_UTG;
    if (stricmp (pszExtension, "jrl") == 0)
        return NwnResType_JRL;
    if (stricmp (pszExtension, "sav") == 0)
        return NwnResType_SAV;
    if (stricmp (pszExtension, "utw") == 0)
        return NwnResType_UTW;
    if (stricmp (pszExtension, "4pc") == 0)
        return NwnResType_4PC;
    if (stricmp (pszExtension, "ssf") == 0)
        return NwnResType_SSF;
    if (stricmp (pszExtension, "hak") == 0)
        return NwnResType_HAK;
    if (stricmp (pszExtension, "nwm") == 0)
        return NwnResType_NWM;
    if (stricmp (pszExtension, "bik") == 0)
        return NwnResType_BIK;
    if (stricmp (pszExtension, "erf") == 0)
        return NwnResType_ERF;
    if (stricmp (pszExtension, "bif") == 0)
        return NwnResType_BIF;
    if (stricmp (pszExtension, "key") == 0)
        return NwnResType_KEY;
    if (stricmp (pszExtension, "ptm") == 0)
        return NwnResType_PTM;
    if (stricmp (pszExtension, "ptt") == 0)
        return NwnResType_PTT;
    if (stricmp (pszExtension, "bak") == 0)
        return NwnResType_BAK;
    if (stricmp (pszExtension, "dat") == 0)
        return NwnResType_DAT;
    if (stricmp (pszExtension, "shd") == 0)
        return NwnResType_SHD;
    if (stricmp (pszExtension, "xbc") == 0)
        return NwnResType_XBC;
    if (stricmp (pszExtension, "wbm") == 0)
        return NwnResType_WBM;
    if (stricmp (pszExtension, "mtr") == 0)
        return NwnResType_MTR;
    if (stricmp (pszExtension, "ktx") == 0)
        return NwnResType_KTX;
    if (stricmp (pszExtension, "ttf") == 0)
        return NwnResType_TTF;
    if (stricmp (pszExtension, "sql") == 0)
        return NwnResType_SQL;
    if (stricmp (pszExtension, "tml") == 0)
        return NwnResType_TML;
    if (stricmp (pszExtension, "sq3") == 0)
        return NwnResType_SQ3;
    if (stricmp (pszExtension, "lod") == 0)
        return NwnResType_LOD;
    if (stricmp (pszExtension, "gif") == 0)
        return NwnResType_GIF;
    if (stricmp (pszExtension, "png") == 0)
        return NwnResType_PNG;
    if (stricmp (pszExtension, "jpg") == 0)
        return NwnResType_JPG;
    if (stricmp (pszExtension, "caf") == 0)
        return NwnResType_CAF;
    if (stricmp (pszExtension, "jui") == 0)
        return NwnResType_JUI;
    if (stricmp (pszExtension, "cdb") == 0)
        return NwnResType_CDB;
    return NwnResType_Unknown;
}

//-----------------------------------------------------------------------------
//
// @mfunc Read the file into memory
//
// @parm const char * | pszKeyFile | Full path name
//
// @parm UINT32 * | pulSize | Size of the opened file.
//
// @rdesc Pointer to an allocated buffer containing the file.
//
//-----------------------------------------------------------------------------

unsigned char *NwnLoadFile (const char *pszKeyFile, UINT32 *pulSize)
{

    //
    // Try to open the new file
    //

    FILE *fp = fopen (pszKeyFile, "rb");
    if (fp == NULL)
        return NULL;

    //
    // Get the size of the file
    //

    fseek (fp, 0, SEEK_END);
    long lSize = ftell (fp);
    fseek (fp, 0, SEEK_SET);

    //
    // Allocate memory for the data
    //

    unsigned char *pauchData = (unsigned char *) malloc (lSize);
    if (pauchData == NULL)
    {
        fclose (fp);
        return NULL;
    }

    //
    // Read the data
    //

    fread (pauchData, 1, lSize, fp);
    fclose (fp);

    //
    // Return
    //

    if (pulSize)
        *pulSize = (UINT32) lSize;
    return pauchData;
}

//-----------------------------------------------------------------------------
//
// @mfunc BASENAME helper routine
//
// @parm const char * | pszFile | Input file name
//
// @rdesc Pointer to the file name
//
//-----------------------------------------------------------------------------

const char *NwnBasename (const char *pszFile)
{
#if !defined (HAVE_BASENAME)
    if (pszFile == NULL)
        return NULL;
    for (const char *psz = pszFile; *psz; ++psz) 
    {
        if (*psz == '\\' || *psz == '/' || *psz == ':')
            pszFile = psz + 1;
    }
    return pszFile;
#else
    return basename ((char *) pszFile);
#endif
}

//-----------------------------------------------------------------------------
//
// @mfunc STRICMP helper routine
//
// @parm const char * | string1 | First string
//
// @parm const char * | string2 | Second string
//
// @rdesc Results of the compare
//
//-----------------------------------------------------------------------------

#if !defined (HAVE_STRICMP) && !defined (HAVE_STRCASECMP)
int stricmp (const char *string1, const char *string2)
{
    int c1, c2;
    do 
    {
        c1 = tolower ((unsigned char) (*(string1++)));
        c2 = tolower ((unsigned char) (*(string2++)));
    } while (c1 && (c1 == c2));
    return (c1 - c2);
}
#endif

//-----------------------------------------------------------------------------
//
// @mfunc STRNICMP helper routine
//
// @parm const char * | string1 | First string
//
// @parm const char * | string2 | Second string
//
// @parm size_t | count | Length of the string
//
// @rdesc Results of the compare
//
//-----------------------------------------------------------------------------


#if !defined (HAVE_STRNICMP) && !defined (HAVE_STRNCASECMP)
int strnicmp (const char *string1, const char *string2, size_t count)
{
    if (count)
    {
        int c1, c2;
        do 
        {
            c1 = tolower ((unsigned char) (*(string1++)));
            c2 = tolower ((unsigned char) (*(string2++)));
        } while (--count && c1 && (c1 == c2));
        return (c1 - c2);
    }
    else
        return 0;
}
#endif

//-----------------------------------------------------------------------------
//
// @mfunc SNPRINTF helper routine
//
// @parm char * | buffer | Output buffer
//
// @parm size_t | count | Length of the output buffer
//
// @parm const char * | format | Format string
//
// @parm assorted | ... | Arguments
//
// @rdesc Number of bytes formatted
//
//-----------------------------------------------------------------------------

#if !defined (HAVE_SNPRINTF) && !defined (HAVE__SNPRINTF)
int snprintf (char *buffer, size_t count, const char *format, ...)
{
    //FIXME
}
#endif

//-----------------------------------------------------------------------------
//
// @mfunc STRLWR helper routine
//
// @parm char * | string | Input string
//
// @rdesc Pointer to the string
//
//-----------------------------------------------------------------------------

#ifndef HAVE_STRLWR
char *strlwr (char *string)
{
    char *psz = string;
    while (*psz)
    {
        *psz = (char) tolower (*psz);
        ++psz;
    }
    return psz;
}
#endif

/* vim: set sw=4: */
