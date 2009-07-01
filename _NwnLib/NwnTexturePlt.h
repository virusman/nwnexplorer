#ifndef ETS_NWNTEXTUREPLT_H
#define ETS_NWNTEXTUREPLT_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnTexturePlt.h - Nwn palette texture file support |
//
// This module contains the definition of the NwnTexturePlt.
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

#include "NwnDefines.h"
#include "NwnMemoryFile.h"

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
class CNwnTexturePltImpl
{
    public:

        struct Header
        {
            char        achSig [4];
            char        achVersion [4];
            UINT32      ulUnknown1;
            UINT32      ulUnknown2;
            UINT32      ulWidth;
            UINT32      ulHeight;
        };

        struct Pixel
        {
            UINT8       ucIndex;
            UINT8       ucPalette;
        };

        // @access Public methods
    public:

        // @access Public inline methods
    public:

        // @cmember Get the header

        Header *GetHeader () 
        {
            TClass *pT = static_cast <TClass *> (this);
            return (Header *) pT ->GetData (0);
        }

        // @cmember Get a pointer to the pixel data

        Pixel *GetPixelData () 
        {
            TClass *pT = static_cast <TClass *> (this);
            return (Pixel *) pT ->GetData (sizeof (Header));
        }

        // @cmember Get the width of the texture

        int GetWidth () 
        {
            Header *pHeader = GetHeader ();
            return pHeader ->ulWidth;
        }

        // @cmember Get the height of the texture

        int GetHeight () 
        {
            Header *pHeader = GetHeader ();
            return pHeader ->ulHeight;
        }
};

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CNwnTexturePlt : 
    public CNwnTexturePltImpl <CNwnTexturePlt>,
    public CNwnMemoryFile
{
    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CNwnTexturePlt (unsigned char *pauchData = NULL, 
                        unsigned long ulSize = 0, bool fAllocated = true) : 
            CNwnMemoryFile (pauchData, ulSize, fAllocated)
    {
    }
};

#endif // ETS_NWNTEXTUREPLT_H

/* vim: set sw=4: */
