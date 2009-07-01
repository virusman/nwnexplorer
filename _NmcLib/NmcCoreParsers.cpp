//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NmcCoreParsers.cpp - Model compiler core value parsers |
//
// This module contains the Nwn model compiler core value parsers.
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

#include <errno.h>
#include "Nmc.h"
#include "NmcCoreParsers.h"

//-----------------------------------------------------------------------------
//
// @func Parse ten values from the input
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm NmcTenValues * | pValues | Structure to populate
//
// @parm unsigned long | ulFlags | Parsing flags
//
// @rdesc Status
//
//              @flag false | The routine failed to parse at least 2 values.
//              @flag true | 2 or more values were parsed.
//
//-----------------------------------------------------------------------------

bool NmcGetTenValues (CNmcContext *pCtx, NmcTenValues *pValues, 
                      unsigned long ulFlags)
{
    pValues ->nColumns = 0;

    //
    // Remove flags we don't want to propagate
    //

    ulFlags &= ~(NmcGet_Optional);

    //
    // Get the first token.  The Bioware code allowed the first token
    // to be optional even on keys... "destruct" is an example of this
    //

    pValues ->afValues [0] = 0.0f;
    if ((ulFlags & NmcGet_NoControllerKey) == 0)
    {
        if (pCtx ->GetNextToken () == NULL)
            return true;
        if (!NmcGetFloatValue (pCtx, &pValues ->afValues [0], 
                               0.0f, ulFlags | NmcGet_CurrentToken))
            return false;
    }
    pValues ->nColumns++;

    //
    // Get up to 9 other values
    //

    for (int i = 1; i < 10; i++)
    {
        if (pCtx ->GetNextToken () == NULL)
            break;
        if (!NmcGetFloatValue (pCtx, &pValues ->afValues [i], 
                               0.0f, ulFlags | NmcGet_CurrentToken))
            break;
        pValues ->nColumns++;
    }

    //
    // If we have 5 columns, then we have a quaternion, 
    // convert from axis rotation to a real quaternion.
    //

    if (pValues ->nColumns == 5)
    {
        CVector3 v (
            pValues ->afValues [1], 
            pValues ->afValues [2], 
            pValues ->afValues [3]); 
        CQuaternion q (v, pValues ->afValues [4]);
        pValues ->afValues [1] = q .m_v [0];
        pValues ->afValues [2] = q .m_v [1];
        pValues ->afValues [3] = q .m_v [2];
        pValues ->afValues [4] = q .m_v [3];
    }
    return true;
}

//-----------------------------------------------------------------------------
//
// @func Parse a floating point value
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm float * | pfValue | Destination
//
// @parm float | fDefault | Default value if not found
//
// @parm unsigned long | ulFlags | Parsing flags
//
// @rdesc Status
//
//              @flag false | The routine failed to parse a floating point value
//              @flag true | The routine parsed a value
//
//-----------------------------------------------------------------------------

bool NmcGetFloatValue (CNmcContext *pCtx, float *pfValue,
                       float fDefault, unsigned long ulFlags)
{

    //
    // Get the token pointer
    //

    char *pszToken;
    if ((ulFlags & NmcGet_CurrentToken) == 0)
        pszToken = pCtx ->GetNextToken ();
    else
        pszToken = pCtx ->GetCurrentToken ();

    //
    // If no token was found
    //

    if (pszToken == NULL)
    {
        *pfValue = fDefault;
        if ((ulFlags & NmcGet_Optional) == 0)
        {
            if ((ulFlags & NmcGet_NoErrors) == 0)
                pCtx ->GenerateError ("Expected floating-point value missing");
            return false;
        }
        else
            return true;
    }

    //
    // Parse the string
    //

    errno = 0;
    char *pszEnd = NULL;
    double dReturn = strtod (pszToken, &pszEnd);

    //
    // Make sure that there isn't any trash at the end
    //

    if (pszEnd && *pszEnd != 0 && !isspace (*pszEnd))
    {
        if ((ulFlags & NmcGet_NoErrors) == 0)
            pCtx ->GenerateError ("Invalid characters is floating-point value");
        *pfValue = fDefault;
        return false;
    }

    //
    // Check for overflow
    //

    if (errno == ERANGE)
    {
        if ((ulFlags & NmcGet_NoErrors) == 0)
        {
            if (dReturn == 0.0)
                pCtx ->GenerateError ("Floating-point value underflow during conversion");
            else
                pCtx ->GenerateError ("Floating-point value overflow during conversion");
        }
        *pfValue = fDefault;
        return false;
    }
    *pfValue = (float) dReturn;
    return true;
}

//-----------------------------------------------------------------------------
//
// @func Parse an int point value
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm int * | pnValue | Destination
//
// @parm int | nDefault | Default value if not found
//
// @parm unsigned long | ulFlags | Parsing flags
//
// @rdesc Status
//
//              @flag false | The routine failed to parse a value
//              @flag true | The routine parsed a value
//
//-----------------------------------------------------------------------------

bool NmcGetIntegerValue (CNmcContext *pCtx, int *pnValue,
                         int nDefault, unsigned long ulFlags)
{

    //
    // Get the token pointer
    //

    char *pszToken;
    if ((ulFlags & NmcGet_CurrentToken) == 0)
        pszToken = pCtx ->GetNextToken ();
    else
        pszToken = pCtx ->GetCurrentToken ();

    //
    // If no token was found
    //

    if (pszToken == NULL)
    {
        *pnValue = nDefault;
        if ((ulFlags & NmcGet_Optional) == 0)
        {
            if ((ulFlags & NmcGet_NoErrors) == 0)
                pCtx ->GenerateError ("Expected integer value missing");
            return false;
        }
        else
            return true;
    }

    //
    // Parse the string
    //

    errno = 0;
    char *pszEnd = NULL;
    long lResult = strtol (pszToken, &pszEnd, 10);

    //
    // Make sure that there isn't any trash at the end
    //

    if (pszEnd && *pszEnd != 0 && !isspace (*pszEnd))
    {
        if (stricmp (pszToken, "true") == 0)
        {
            *pnValue = 1;
            return true;
        }
        else if (stricmp (pszToken, "false") == 0)
        {
            *pnValue = 0;
            return true;
        }
        else
        {
            if ((ulFlags & NmcGet_NoErrors) == 0)
                pCtx ->GenerateError ("Invalid characters is integer value");
            *pnValue = nDefault;
            return false;
        }
    }

    //
    // Check for overflow
    //

    if (errno == ERANGE)
    {
        if ((ulFlags & NmcGet_NoErrors) == 0)
        {
            if (lResult < 0)
                pCtx ->GenerateError ("Integer value underflow during conversion");
            else
                pCtx ->GenerateError ("Integer value overflow during conversion");
        }
        *pnValue = nDefault;
        return false;
    }
    *pnValue = (int) lResult;
    return true;
}

//-----------------------------------------------------------------------------
//
// @func Parse a string value
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm char * | pszValue | Destination
//
// @parm int | cchMaxText | Length of the destination
//
// @parm char * | pszDefault | Default value
//
// @parm unsigned long | ulFlags | Parsing flags
//
// @rdesc Status
//
//              @flag false | The routine failed to parse a value
//              @flag true | The routine parsed a value
//
//-----------------------------------------------------------------------------

bool NmcGetStringValue (CNmcContext *pCtx, char *pszValue,
                        int cchMaxText, char *pszDefault, unsigned long ulFlags)
{
    assert (cchMaxText >= 1);

    //
    // Get the token pointer
    //

    const char *pszToken;
    if ((ulFlags & NmcGet_CurrentToken) == 0)
        pszToken = pCtx ->GetNextToken ();
    else
        pszToken = pCtx ->GetCurrentToken ();

    //
    // If no token was found
    //

    if (pszToken == NULL)
    {
        if ((ulFlags & NmcGet_Optional) == 0)
        {
            if ((ulFlags & NmcGet_NoErrors) == 0)
                pCtx ->GenerateError ("Expected string value missing");
            return false;
        }
        pszToken = pszDefault;
        if (pszToken == NULL)
            pszToken = "";
    }

    //
    // Copy the string
    //

    size_t nLength = strlen (pszToken);
    if ((int) nLength > cchMaxText - 1)
    {
        if ((ulFlags & NmcGet_NoWarnings) == 0)
            pCtx ->GenerateWarning ("String value truncated");
        nLength = cchMaxText - 1;
    }
    memcpy (pszValue, pszToken, nLength);
    pszValue [nLength] = 0;
    return true;
}

//-----------------------------------------------------------------------------
//
// @func Get a vector
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CVector3 * | pvValue | Destination
// 
// @parm const CVector3 & | vDefault | Default value
//
// @parm unsigned long | ulFlags | Parsing flags
//
// @rdesc Status
//
//              @flag false | The routine failed to parse the vector.
//              @flag true | The routine parsed the vector.
//
//-----------------------------------------------------------------------------

bool NmcGetVector3Value (CNmcContext *pCtx, CVector3 *pvValue,
                         const CVector3 vDefault, unsigned long ulFlags)
{

    //
    // Remove flags we don't want to propagate
    //

    ulFlags &= ~(NmcGet_Optional);

    //
    // Get the first value
    //

    float x;
    if (!NmcGetFloatValue (pCtx, &x, vDefault .m_x, ulFlags))
    {
        *pvValue = vDefault;
        return false;
    }

    //
    // Remove flags we don't want to propagate
    //

    ulFlags &= ~(NmcGet_CurrentToken);

    //
    // Get the y,z values
    //

    float y, z;
    if (!NmcGetFloatValue (pCtx, &y, vDefault .m_y, ulFlags) ||
        !NmcGetFloatValue (pCtx, &z, vDefault .m_z, ulFlags))
    {
        *pvValue = vDefault;
        return false;
    }

    //
    // Save the results
    //

    pvValue ->m_x = x;
    pvValue ->m_y = y;
    pvValue ->m_z = z;
    return true;
}

//-----------------------------------------------------------------------------
//
// @func Get a quaternion
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CQuaternion * | pqValue | Destination
// 
// @parm const CQuaternion & | qDefault | Default value
//
// @parm unsigned long | ulFlags | Parsing flags
//
// @rdesc Status
//
//              @flag false | The routine failed to parse the quaternion.
//              @flag true | The routine parsed the quaternion.
//
//-----------------------------------------------------------------------------

bool NmcGetQuaternionValue (CNmcContext *pCtx, CQuaternion *pqValue,
                            const CQuaternion &qDefault, unsigned long ulFlags)
{

    //
    // Remove flags we don't want to propagate
    //

    ulFlags &= ~(NmcGet_Optional);

    //
    // Get the first value
    //

    CVector3 v;
    if (!NmcGetFloatValue (pCtx, &v .m_x, 0, ulFlags))
    {
        *pqValue = qDefault;
        return false;
    }

    //
    // Remove flags we don't want to propagate
    //

    ulFlags &= ~(NmcGet_CurrentToken);

    //
    // Get the other
    //

    float fAngle;
    if (!NmcGetFloatValue (pCtx, &v .m_y, 0, ulFlags) ||
        !NmcGetFloatValue (pCtx, &v .m_z, 0, ulFlags) ||
        !NmcGetFloatValue (pCtx, &fAngle, 0, ulFlags))
    {
        *pqValue = qDefault;
        return false;
    }

    //
    // Save the results
    //

    pqValue ->SetAxisRotation (v, fAngle);
    return true;
}

//-----------------------------------------------------------------------------
//
// @func Get a face
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm NmcMeshFace * | psValue | Destination
// 
// @parm unsigned long | ulFlags | Parsing flags
//
// @rdesc Status
//
//              @flag false | The routine failed to parse the face.
//              @flag true | The routine parsed the face.
//
//-----------------------------------------------------------------------------

bool NmcGetFaceValue (CNmcContext *pCtx, NmcMeshFace *psFace, 
                      unsigned long ulFlags)
{
    int nValue [8];

    //
    // Remove flags we don't want to propagate
    //

    ulFlags &= ~(NmcGet_Optional);

    //
    // Get the first value
    //

    if (!NmcGetIntegerValue (pCtx, &nValue [0], 0, ulFlags))
    {
        return false;
    }

    //
    // Remove flags we don't want to propagate
    //

    ulFlags &= ~(NmcGet_CurrentToken);

    //
    // Get the other
    //

    if (!NmcGetIntegerValue (pCtx, &nValue [1], 0, ulFlags) ||
        !NmcGetIntegerValue (pCtx, &nValue [2], 0, ulFlags) ||
        !NmcGetIntegerValue (pCtx, &nValue [3], 0, ulFlags) ||
        !NmcGetIntegerValue (pCtx, &nValue [4], 0, ulFlags) ||
        !NmcGetIntegerValue (pCtx, &nValue [5], 0, ulFlags) ||
        !NmcGetIntegerValue (pCtx, &nValue [6], 0, ulFlags) ||
        !NmcGetIntegerValue (pCtx, &nValue [7], 0, ulFlags | NmcGet_Optional))
    {
        return false;
    }

    //
    // Save the results
    //

    psFace ->anVerts [0] = nValue [0];
    psFace ->anVerts [1] = nValue [1];
    psFace ->anVerts [2] = nValue [2];
    psFace ->nSmoothMask = nValue [3];
    psFace ->sTexIndices .anIndices [0] = nValue [4];
    psFace ->sTexIndices .anIndices  [1] = nValue [5];
    psFace ->sTexIndices .anIndices  [2] = nValue [6];
    psFace ->nSurface = nValue [7];
    return true;
}

//-----------------------------------------------------------------------------
//
// @func Get a tex indices set
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm NmcTexIndices * | psValue | Destination
// 
// @parm unsigned long | ulFlags | Parsing flags
//
// @rdesc Status
//
//              @flag false | The routine failed to parse the value.
//              @flag true | The routine parsed the value.
//
//-----------------------------------------------------------------------------

bool NmcGetTexIndicesValue (CNmcContext *pCtx, NmcTexIndices *psValue,
                            unsigned long ulFlags)
{

    //
    // Remove flags we don't want to propagate
    //

    ulFlags &= ~(NmcGet_Optional);

    //
    // Initialize output
    //

    psValue ->anIndices [0] = 0;
    psValue ->anIndices [1] = 0;
    psValue ->anIndices [2] = 0;

    //
    // Get the first value
    //

    int i0;
    if (!NmcGetIntegerValue (pCtx, &i0, 0, ulFlags))
    {
        return false;
    }

    //
    // Remove flags we don't want to propagate
    //

    ulFlags &= ~(NmcGet_CurrentToken);

    //
    // Get the other values
    //

    int i1, i2;
    if (!NmcGetIntegerValue (pCtx, &i1, 0, ulFlags) ||
        !NmcGetIntegerValue (pCtx, &i2, 0, ulFlags))
    {
        return false;
    }

    //
    // Save the results
    //

    psValue ->anIndices [0] = i0;
    psValue ->anIndices [1] = i1;
    psValue ->anIndices [2] = i2;
    return true;
}

//-----------------------------------------------------------------------------
//
// @func Get skin weight values
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm NwnMdlSkinWeight * | pValue | Destination
// 
// @parm unsigned long | ulFlags | Parsing flags
//
// @rdesc Status
//
//              @flag false | The routine failed to parse the value.
//              @flag true | The routine parsed the value.
//
//-----------------------------------------------------------------------------

bool NmcGetWeightValues (CNmcContext *pCtx, NwnMdlSkinWeight *pValue, 
                         unsigned long ulFlags)
{

    //
    // Remove flags we don't want to propagate
    //

    ulFlags &= ~(NmcGet_Optional);

    //
    // Initialize output
    //

    for (int i = 0; i < 4; i++)
    {
        pValue ->szBones [i] [0] = 0;
        pValue ->fWeights [i] = 0;
    }
    pValue ->nCount = 0;

    //
    // Read the data
    //

    for (int i = 0; i < 4; i++)
    {
        if (!NmcGetStringValue (pCtx, pValue ->szBones [i],
                                _countof (pValue ->szBones [i]), (char *)"", ulFlags | NmcGet_NoErrors))
        {
            if (pValue ->nCount == 0)
                pCtx ->GenerateWarning ("No bones specified");
            return true;
        }
        ulFlags &= ~(NmcGet_CurrentToken);
        if (!NmcGetFloatValue (pCtx, &pValue ->fWeights [i], 0, ulFlags))
            return false;
        pValue ->nCount++;
    }
    return true;
}


/* vim: set sw=4: */
