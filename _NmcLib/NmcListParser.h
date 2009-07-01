#ifndef ETS_NMCLISTPARSER_H
#define ETS_NMCLISTPARSER_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NmcListParser.h - List parser template |
//
// This module contains the definition of the NmcListParser.
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

#include "../_NwnLib/NwnStreams.h"
#include "../_NwnLib/NwnArray.h"
#include "NmcContext.h"
#include "NmcCoreParsers.h"

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

template <class TClass>
class CNmcListParserTraits
{
    // @access Public static methods
    public:
        // @cmember Test to see if the class supports binary read

        static bool AllowBinary () 
        {
            return true;
        }

        // @cmember Parse a value

        static bool ParseElement (CNmcContext *pCtx, TClass &sElement)
        {
            assert (false); // must be specialized
            return false;
        }
};

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

template <class TClass, class TTraits = CNmcListParserTraits <TClass> >
class CNmcListParser
{
    // @access Public static methods
    public:

        // @cmember Parse an array

        static bool ParseArray (CNmcContext *pCtx, CNwnArray <TClass> &aElements)
        {
            //
            // If the number of rows was specified
            //

            int nRows;
            if (NmcGetIntegerValue (pCtx, &nRows, 0, NmcGet_NoErrors))
            {

                //
                // Test to see if the use specified the binary flag
                //

                char szBinary [2];
                if (TTraits::AllowBinary ())
                {
                    NmcGetStringValue (pCtx, szBinary, _countof (szBinary), (char *)"", 
                                       NmcGet_NoWarnings | NmcGet_Optional);
                }
                else
                    szBinary [0] = 0;

                //
                // If we don't have binary, read the next n rows
                //

                if (szBinary [0] == 0)
                {
                    for (int i = 0; i < nRows; i++)
                    {
                        if (!pCtx ->ReadNextLine ())
                            return false;
                        TClass sElement;
                        if (!TTraits::ParseElement (pCtx, sElement))
                            return false;
                        aElements .Add (sElement);
                    }
                }

                //
                // Otherwise, read n blocks of binary data
                //

                else
                {
                    for (int i = 0; i < nRows; i++)
                    {
                        TClass sElement;
                        if (!pCtx ->ReadBinary (&sElement, sizeof (sElement)))
                            return false;
                        aElements .Add (sElement);
                    }
                }
            }

            //
            // Otherwise, we have to read the data until we get an "endlist"
            //

            else
            {

                //
                // Loop until we reach the "endlist"
                //

                for (;;)
                {
                    if (!pCtx ->ReadNextLine ())
                        return false;
                    char *pszToken = pCtx ->GetNextToken ();
                    if (pszToken == NULL)
                        return false;
                    if (stricmp (pszToken, "endlist") == 0)
                        break;
                    pCtx ->UnGetToken ();
                    TClass sElement;
                    if (!TTraits::ParseElement (pCtx, sElement))
                        return false;
                    aElements .Add (sElement);
                }
            }
            return true;
        }
};

//-----------------------------------------------------------------------------
//
// Class specialization - NmcTenValues
//
//-----------------------------------------------------------------------------

template <>
class CNmcListParserTraits <NmcTenValues>
{
    // @access Public static methods
    public:
        // @cmember Test to see if the class supports binary read

        static bool AllowBinary () 
        {
            return true;
        }

        // @cmember Parse a value

        static bool ParseElement (CNmcContext *pCtx, NmcTenValues &sElement)
        {
            return NmcGetTenValues (pCtx, &sElement, 0);
        }
};

//-----------------------------------------------------------------------------
//
// Class specialization - UINT32
//
//-----------------------------------------------------------------------------

template <>
class CNmcListParserTraits <UINT32>
{
    // @access Public static methods
    public:
        // @cmember Test to see if the class supports binary read

        static bool AllowBinary () 
        {
            return true;
        }

        // @cmember Parse a value

        static bool ParseElement (CNmcContext *pCtx, UINT32 &sElement)
        {
            int nValue;
            if (!NmcGetIntegerValue (pCtx, &nValue, 0, 0))
                return false;
            sElement = (UINT32) nValue;
            return true;
        }
};

//-----------------------------------------------------------------------------
//
// Class specialization - float
//
//-----------------------------------------------------------------------------

template <>
class CNmcListParserTraits <float>
{
    // @access Public static methods
    public:
        // @cmember Test to see if the class supports binary read

        static bool AllowBinary () 
        {
            return true;
        }

        // @cmember Parse a value

        static bool ParseElement (CNmcContext *pCtx, float &sElement)
        {
            return NmcGetFloatValue (pCtx, &sElement, 0.0f, 0);
        }
};

//-----------------------------------------------------------------------------
//
// Class specialization - CVector3
//
//-----------------------------------------------------------------------------

template <>
class CNmcListParserTraits <CVector3>
{
    // @access Public static methods
    public:
        // @cmember Test to see if the class supports binary read

        static bool AllowBinary () 
        {
            return true;
        }

        // @cmember Parse a value

        static bool ParseElement (CNmcContext *pCtx, CVector3 &sElement)
        {
            return NmcGetVector3Value (pCtx, &sElement, g_vOrigin, 0);
        }
};

//-----------------------------------------------------------------------------
//
// Class specialization - CQuaternion
//
//-----------------------------------------------------------------------------

template <>
class CNmcListParserTraits <CQuaternion>
{
    // @access Public static methods
    public:
        // @cmember Test to see if the class supports binary read

        static bool AllowBinary () 
        {
            return true;
        }

        // @cmember Parse a value

        static bool ParseElement (CNmcContext *pCtx, CQuaternion &sElement)
        {
            CQuaternion qDefault (0, 0, 0, 0);
            return NmcGetQuaternionValue (pCtx, &sElement, qDefault, 0);
        }
};

//-----------------------------------------------------------------------------
//
// Class specialization - NmcMeshFace
//
//-----------------------------------------------------------------------------

template <>
class CNmcListParserTraits <NmcMeshFace>
{
    // @access Public static methods
    public:
        // @cmember Test to see if the class supports binary read

        static bool AllowBinary () 
        {
            return false;
        }

        // @cmember Parse a value

        static bool ParseElement (CNmcContext *pCtx, NmcMeshFace &sElement)
        {
            return NmcGetFaceValue (pCtx, &sElement, 0);
        }
};

//-----------------------------------------------------------------------------
//
// Class specialization - CNwnPointer <char>
//
//-----------------------------------------------------------------------------

template <>
class CNmcListParserTraits <CNwnPointer <char> >
{
    // @access Public static methods
    public:
        // @cmember Test to see if the class supports binary read

        static bool AllowBinary () 
        {
            return false;
        }

        // @cmember Parse a value

        static bool ParseElement (CNmcContext *pCtx, CNwnPointer <char> &sElement)
        {
            char *pszToken = pCtx ->GetNextToken ();
            if (pszToken == NULL)
            {
                pCtx ->GenerateError ("Expected string not found");
                return false;
            }
            else
            {
                size_t nLength = strlen (pszToken) + 1;
                char *psz = new char [nLength];
                strcpy (psz, pszToken);
                sElement .SetData (psz);
                return true;
            }
        }
};

//-----------------------------------------------------------------------------
//
// Class specialization - CVector3
//
//-----------------------------------------------------------------------------

template <>
class CNmcListParserTraits <NmcTexIndices>
{
    // @access Public static methods
    public:
        // @cmember Test to see if the class supports binary read

        static bool AllowBinary () 
        {
            return false;
        }

        // @cmember Parse a value

        static bool ParseElement (CNmcContext *pCtx, NmcTexIndices &sElement)
        {
            return NmcGetTexIndicesValue (pCtx, &sElement, 0);
        }
};

//-----------------------------------------------------------------------------
//
// Class specialization - Weights
//
//-----------------------------------------------------------------------------

template <>
class CNmcListParserTraits <NwnMdlSkinWeight>
{
    // @access Public static methods
    public:
        // @cmember Test to see if the class supports binary read

        static bool AllowBinary () 
        {
            return false;
        }

        // @cmember Parse a value

        static bool ParseElement (CNmcContext *pCtx, NwnMdlSkinWeight &sElement)
        {
            return NmcGetWeightValues (pCtx, &sElement, 0);
        }
};

#endif // ETS_NMCLISTPARSER_H

/* vim: set sw=4: */
