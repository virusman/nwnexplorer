//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NmcController.cpp - Model compiler controller support |
//
// This module contains the Nwn model compiler controller support.
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

#include "Nmc.h"
#include "NmcCoreParsers.h"
#include "NmcListParser.h"

//
// Controller list
//

struct NmcController
{
    const char         *pszName;
    NwnMdlCT            nType;
    UINT32              ulFlags;
};

NmcController g_sControllers [] =
{

    //
    // Common
    //

    { "position",                               NwnMdlCT_Position,                              NwnMdlNF_HasHeader, },
    { "orientation",                    NwnMdlCT_Orientation,                   NwnMdlNF_HasHeader, },
    { "scale",                                  NwnMdlCT_Scale,                                 NwnMdlNF_HasHeader, },

    //
    // Light
    //

    { "color",                                  NwnMdlCT_Color,                                 NwnMdlNF_HasLight, },
    { "radius",                                 NwnMdlCT_Radius,                                NwnMdlNF_HasLight, },
    { "shadowradius",                   NwnMdlCT_ShadowRadius,                  NwnMdlNF_HasLight, },
    { "verticaldisplacement",   NwnMdlCT_VerticalDisplacement,  NwnMdlNF_HasLight, },
    { "multiplier",                             NwnMdlCT_Multiplier,                    NwnMdlNF_HasLight, },

    // Emitter 

    { "alphaEnd",                               NwnMdlCT_AlphaEnd,                              NwnMdlNF_HasEmitter, },
    { "alphaStart",                             NwnMdlCT_AlphaStart,                    NwnMdlNF_HasEmitter, },
    { "birthrate",                              NwnMdlCT_BirthRate,                             NwnMdlNF_HasEmitter, },
    { "bounce_co",                              NwnMdlCT_Bounce_Co,                             NwnMdlNF_HasEmitter, },
    { "colorEnd",                               NwnMdlCT_ColorEnd,                              NwnMdlNF_HasEmitter, },
    { "colorStart",                             NwnMdlCT_ColorStart,                    NwnMdlNF_HasEmitter, },
    { "combinetime",                    NwnMdlCT_CombineTime,                   NwnMdlNF_HasEmitter, },
    { "drag",                                   NwnMdlCT_Drag,                                  NwnMdlNF_HasEmitter, },
    { "fps",                                    NwnMdlCT_FPS,                                   NwnMdlNF_HasEmitter, },
    { "frameEnd",                               NwnMdlCT_FrameEnd,                              NwnMdlNF_HasEmitter, },
    { "frameStart",                             NwnMdlCT_FrameStart,                    NwnMdlNF_HasEmitter, },
    { "grav",                                   NwnMdlCT_Grav,                                  NwnMdlNF_HasEmitter, },
    { "lifeExp",                                NwnMdlCT_LifeExp,                               NwnMdlNF_HasEmitter, },
    { "mass",                                   NwnMdlCT_Mass,                                  NwnMdlNF_HasEmitter, },
    { "p2p_bezier2",                    NwnMdlCT_P2P_Bezier2,                   NwnMdlNF_HasEmitter, },
    { "p2p_bezier3",                    NwnMdlCT_P2P_Bezier3,                   NwnMdlNF_HasEmitter, },
    { "particleRot",                    NwnMdlCT_ParticleRot,                   NwnMdlNF_HasEmitter, },
    { "randvel",                                NwnMdlCT_RandVel,                               NwnMdlNF_HasEmitter, },
    { "sizeStart",                              NwnMdlCT_SizeStart,                             NwnMdlNF_HasEmitter, },
    { "sizeEnd",                                NwnMdlCT_SizeEnd,                               NwnMdlNF_HasEmitter, },
    { "sizeStart_y",                    NwnMdlCT_SizeStart_Y,                   NwnMdlNF_HasEmitter, },
    { "sizeEnd_y",                              NwnMdlCT_SizeEnd_Y,                             NwnMdlNF_HasEmitter, },
    { "spread",                                 NwnMdlCT_Spread,                                NwnMdlNF_HasEmitter, },
    { "threshold",                              NwnMdlCT_Threshold,                             NwnMdlNF_HasEmitter, },
    { "velocity",                               NwnMdlCT_Velocity,                              NwnMdlNF_HasEmitter, },
    { "xsize",                                  NwnMdlCT_XSize,                                 NwnMdlNF_HasEmitter, },
    { "ysize",                                  NwnMdlCT_YSize,                                 NwnMdlNF_HasEmitter, },
    { "blurlength",                             NwnMdlCT_BlurLength,                    NwnMdlNF_HasEmitter, },
    { "lightningDelay",                 NwnMdlCT_LightningDelay,                NwnMdlNF_HasEmitter, },
    { "lightningRadius",                NwnMdlCT_LightningRadius,               NwnMdlNF_HasEmitter, },
    { "lightningScale",                 NwnMdlCT_LightningScale,                NwnMdlNF_HasEmitter, },
    { "detonate",                               NwnMdlCT_Detonate,                              NwnMdlNF_HasEmitter, },
    { "alphaMid",                               NwnMdlCT_AlphaMid,                              NwnMdlNF_HasEmitter, },
    { "colorMid",                               NwnMdlCT_ColorMid,                              NwnMdlNF_HasEmitter, },
    { "percentStart",                   NwnMdlCT_PercentStart,                  NwnMdlNF_HasEmitter, },
    { "percentMid",                             NwnMdlCT_PercentMid,                    NwnMdlNF_HasEmitter, },
    { "percentEnd",                             NwnMdlCT_PercentEnd,                    NwnMdlNF_HasEmitter, },
    { "sizeMid",                                NwnMdlCT_SizeMid,                               NwnMdlNF_HasEmitter, },
    { "sizeMid_y",                              NwnMdlCT_SizeMid_Y,                             NwnMdlNF_HasEmitter, },

    //
    // Meshes
    //

    { "selfillumcolor",                 NwnMdlCT_SelfIllumColor,                NwnMdlNF_HasMesh, },
    { "alpha",                                  NwnMdlCT_Alpha,                                 NwnMdlNF_HasMesh, },
};

int g_nControllers = _countof (g_sControllers);

//
// Controller ID to structure mapping
//

NmcController **g_paIndexToController = NULL;
int g_nMaxControllerIndex = 0;

//
// Controller table initializer
//

class CNmcControllerInitialize
{
    public:
        CNmcControllerInitialize ()
        {

            //
            // Sort the controllers (OMG, it is a hand written sort loop!!! Bite me)
            //

            bool fSwapped = true;
            while (fSwapped)
            {
                fSwapped = false;
                for (int i = 0; i < g_nControllers - 1; i++)
                {
                    if (stricmp (g_sControllers [i] .pszName, 
                                 g_sControllers [i + 1] .pszName) > 0)
                    {
                        NmcController temp;
                        temp = g_sControllers [i];
                        g_sControllers [i] = g_sControllers [i + 1];
                        g_sControllers [i + 1] = temp;
                        fSwapped = true;
                    }
                }
            }

            //
            // Get the max controller index
            //

            g_nMaxControllerIndex = 0;
            for (int i = 0; i < g_nControllers; i++)
            {
                if (g_sControllers [i] .nType > g_nMaxControllerIndex) 
                    g_nMaxControllerIndex = g_sControllers [i] .nType;
            }

            //
            // Create the controller map
            //

            int nCount = g_nMaxControllerIndex + 1;
            g_paIndexToController = new NmcController * [nCount * 2];
            memset (g_paIndexToController, 0, nCount * 2 * sizeof (NmcController *));
            for (int i = 0; i < g_nControllers; i++)
            {
                int nIndex = g_sControllers [i] .nType;
                if (g_paIndexToController [nIndex] == NULL)
                    g_paIndexToController [nIndex] = &g_sControllers [i];
                else if (g_paIndexToController [nIndex + nCount] == NULL)
                    g_paIndexToController [nIndex + nCount] = &g_sControllers [i];
                else
                    assert (!"more than 2 controllers share the same name");
            }
        }

        ~CNmcControllerInitialize ()
        {
            if (g_paIndexToController)
                delete [] g_paIndexToController;
        }
};

static CNmcControllerInitialize g_sControllerInitialize;

//-----------------------------------------------------------------------------
//
// @func Parse a controller
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CNwnMdlNodeHeader * | pNode | Destination node
//
// @rdesc Status
//
//              @flag true | if the controller was recognized
//              @flag false | if it wasn't a controller
//
//-----------------------------------------------------------------------------

bool NmcParseController (CNmcContext *pCtx, CNwnMdlNodeHeader *pNode)
{
    static const char szKey [] = "key";
    static const char szBezierKey [] = "bezierkey";
    static const char szSetfIllumColor [] = "setfillumcolor";
    static const int nKeyLength = _countof (szKey) - 1;
    static const int nBezierKeyLength = _countof (szBezierKey) - 1;
    static const int nSetfIllumColorLength = _countof (szSetfIllumColor) - 1;

    //
    // Get a pointer to the current token
    //

    char *pszToken = pCtx ->GetCurrentToken ();

    //
    // Copy the string to local space
    //

    size_t nLength = strlen (pszToken);
    char *pszLToken = (char *) alloca (nLength + 1);
    strcpy (pszLToken, pszToken);

    //
    // Test to see if the token ends in "key" or "bezierkey".
    // If found, remember this and remove this suffix from the string.
    //
    // Also check for 'setfillumcolor' and translate that to 'selfillumcolor'
    //

    int nKey = 0;
    if (nLength > nBezierKeyLength && stricmp (&pszLToken 
                                               [nLength - nBezierKeyLength], szBezierKey) == 0)
    {
        pszLToken [nLength - nBezierKeyLength] = 0;
        nKey = 2;
    }
    else if (nLength > nKeyLength && stricmp (&pszLToken 
                                              [nLength - nKeyLength], szKey) == 0)
    {
        pszLToken [nLength - nKeyLength] = 0;
        nKey = 1;
    }
    else if (nLength > nSetfIllumColorLength && stricmp(pszLToken, szSetfIllumColor) == 0)
    {
        pszLToken[2] = 'l';
    }
    else
        nKey = 0;

    //
    // Use a binary search to find a match
    //

    int nMatch = 0;
    int nMin = 0;
    int nMax = g_nControllers - 1;
    while (nMin <= nMax)
    {
        nMatch = (nMin + nMax) / 2;
        int nCompare = stricmp (pszLToken, g_sControllers [nMatch] .pszName);
        if (nCompare < 0)
            nMax = nMatch - 1;
        else if (nCompare > 0)
            nMin = nMatch + 1;
        else
            break;
    }

    //
    // If we found nothing, return
    //

    if (nMin > nMax)
        return false;

    //
    // Now, try to locate the controller that matches our flags
    //

    if ((g_sControllers [nMatch] .ulFlags & pNode ->m_ulFlags) == 0)
    {

        //
        // The thing to remember about the binary search is that if we have
        // a series of controllers with the same name, then the point we located
        // a name probably won't be the first one in the list.  Thus we have to
        // search forward and backward
        //

        int nFound = -1;

        //
        // Search backwards for a match
        //

        for (int nTest = nMatch - 1; nTest >= 0; nTest--)
        {
            if (stricmp (g_sControllers [nTest] .pszName, pszLToken) != 0)
                break;
            if ((g_sControllers [nTest] .ulFlags & pNode ->m_ulFlags) != 0)
            {
                nFound = nTest;
                break;
            }
        }

        //
        // If not found backwards, try forwards
        //

        if (nFound == -1)
        {
            for (int nTest = nMatch + 1; nTest < g_nControllers; nTest++)
            {
                if (stricmp (g_sControllers [nTest] .pszName, pszLToken) != 0)
                    break;
                if ((g_sControllers [nTest] .ulFlags & pNode ->m_ulFlags) != 0)
                {
                    nFound = nTest;
                    break;
                }
            }
        }

        //
        // If still not found, then return false
        //

        if (nFound == -1)
            return false;

        //
        // Use the found index as the match index
        //

        nMatch = nFound;
    }

    //
    // Get the number of rows
    //

    CNwnArray <NmcTenValues> aValues;
    if (nKey == 0)
    {

        //
        // Get the next ten values
        //

        NmcTenValues sValues;
        if (!NmcGetTenValues (pCtx, &sValues, NmcGet_NoControllerKey))
            return true;
        aValues .Add (sValues);
    }
    else
    {
        CNmcListParser <NmcTenValues>::ParseArray (pCtx, aValues);
    }

    //
    // At this point, we have an array of values.  If the user specified
    // a "key" but only one row, the we can't tell the difference between
    // a "key" and a normal controller.  Oh well...
    //

    //
    // First, we validate that we have at least 1 entry
    //

    if (aValues .GetCount () < 1)
    {
        //pCtx ->GenerateWarning ("No controller data specified");
        return true;
    }

    //
    // Next, just do a basic check on the number of columns
    // 

    int nColumns = aValues [0] .nColumns;
    for (size_t i = 1; i < aValues .GetCount (); i++)
    {
        if (aValues [i] .nColumns != nColumns)
        {
            pCtx ->GenerateWarning ("Not all controller data rows "
                                    "contain the same number of columns");
            break;
        }
    }

    //
    // Create the structure
    //

    NwnMdlControllerKey sKey;
    sKey .lType = g_sControllers [nMatch] .nType;
    sKey .sRows = (INT16) aValues .GetCount ();
    sKey .bColumns = (INT8) (nColumns - 1);
    sKey .bPad = 0;
    if (nKey == 2)
        sKey .bColumns |= 0x10;

    //
    // Add the key data
    //

    sKey .sKeyOffset = (INT16) pNode ->m_aControllerData .GetCount ();
    for (size_t i = 0; i < aValues .GetCount (); i++)
        pNode ->m_aControllerData .Add (aValues [i] .afValues [0]);

    //
    // Add the controller data
    //

    sKey .sDataOffset = (INT16) pNode ->m_aControllerData .GetCount ();
    if (nColumns > 0)
    {
        for (size_t i = 0; i < aValues .GetCount (); i++)
        {
            pNode ->m_aControllerData .Append (&
                                               aValues [i] .afValues [1],
                                               nColumns - 1);
        }
    }

    //
    // Add the main controller structure
    //

    pNode ->m_aControllerKey .Add (sKey);
    return true;
}

//-----------------------------------------------------------------------------
//
// @func Get the controller name
//
// @parm int | nController | Controller number
//
// @parm UINT32 | ulFlags | Node flags
//
// @rdesc Pointer to the controller name or NULL if unknown
//
//-----------------------------------------------------------------------------

const char *NmcGetControllerName (int nController, UINT32 ulFlags)
{

    //
    // Verify the range
    //

    if (nController < 0 || nController >= g_nMaxControllerIndex)
        return NULL;

    //
    // Verify there is a controller for that index
    //

    if (g_paIndexToController [nController] == NULL)
        return NULL;

    //
    // Return the name
    //

    NmcController *pController;

    pController = g_paIndexToController [nController];
    if ((pController ->ulFlags & ulFlags) != 0)
        return pController ->pszName;

    pController = g_paIndexToController [nController + g_nMaxControllerIndex + 1];
    if ((pController ->ulFlags & ulFlags) != 0)
        return pController ->pszName;

    return NULL;
}

/* vim: set sw=4: */
