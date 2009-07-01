#ifndef ETS_NMCCOREPARSERS_H
#define ETS_NMCCOREPARSERS_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NmcCoreParsers.h - Model compiler core parsers |
//
// This module contains the definition of the model compiler's core parsers.
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
// $History: CnfMainWnd.h $
//      
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include "../_MathLib/Vector3.h"
#include "../_MathLib/Quaternion.h"

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

class CNmcContext;

//-----------------------------------------------------------------------------
//
// Flags
//
//-----------------------------------------------------------------------------

enum NmcGet
{
    NmcGet_CurrentToken                 = 0x00000001,
    NmcGet_Optional                             = 0x00000002,
    NmcGet_NoWarnings                   = 0x00000004,
    NmcGet_NoErrors                             = 0x00000008,
    NmcGet_NoControllerKey              = 0x00000010,
};

//-----------------------------------------------------------------------------
//
// Structures
//
//-----------------------------------------------------------------------------

struct NmcTenValues
{
    float afValues [10];
    int nColumns;
};

//-----------------------------------------------------------------------------
//
// Structures
//
//-----------------------------------------------------------------------------

struct NmcTexIndices
{
    int anIndices [3];
};

//-----------------------------------------------------------------------------
//
// Structures
//
//-----------------------------------------------------------------------------

struct NmcMeshFace
{
    int anVerts [3];
    int nSmoothMask;
    NmcTexIndices sTexIndices;
    int nSurface;
    // The following values are not parsed
    int anVertNormals [3];
};

//-----------------------------------------------------------------------------
//
// Routines
//
//-----------------------------------------------------------------------------

bool NmcGetFloatValue (CNmcContext *pCtx, float *pfValue,
                       float fDefault, unsigned long ulFlags);

bool NmcGetIntegerValue (CNmcContext *pCtx, int *pnValue, 
                         int nDefault, unsigned long ulFlags);

bool NmcGetStringValue (CNmcContext *pCtx, char *pszValue,
                        int cchMaxText, char *pszDefault, unsigned long ulFlags);

bool NmcGetTenValues (CNmcContext *pCtx, NmcTenValues *pValues, 
                      unsigned long ulFlags);

bool NmcGetVector3Value (CNmcContext *pCtx, CVector3 *pvValue,
                         const CVector3 vDefault, unsigned long ulFlags);

bool NmcGetQuaternionValue (CNmcContext *pCtx, CQuaternion *pqValue,
                            const CQuaternion &qDefault, unsigned long ulFlags);

bool NmcGetFaceValue (CNmcContext *pCtx, NmcMeshFace *psFace,
                      unsigned long ulFlags);

bool NmcGetTexIndicesValue (CNmcContext *pCtx, 
                            NmcTexIndices *psValue, unsigned long ulFlags);

bool NmcGetWeightValues (CNmcContext *pCtx, NwnMdlSkinWeight *pValue, 
                         unsigned long ulFlags);

#endif // ETS_NMCCOREPARSERS_H

/* vim: set sw=4: */
