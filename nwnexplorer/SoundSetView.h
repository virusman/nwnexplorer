#ifndef ETS_SOUNDSETVIEW_H
#define ETS_SOUNDSETVIEW_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      SoundSetView.h - Sound set view window |
//
// This module contains the definition of the sound set view window.
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
// $History: HexView.h $
//      
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include "resource.h"
#include "DataRef.h"
#include "TextViewImpl.h"

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

class CSoundSetView :
    public CTextViewImpl
{

    // @access Construction and destruction
    public:

        // @cmember General constructor

        CSoundSetView (CData *pFile);

        // @cmember General destructor

        ~CSoundSetView ();

        // @access Public virtual method
    public:


        // @cmember Get the data

        virtual char *GetData ()
        {
            return (char *) (LPCSTR) m_strText;
        }

        // @cmember Get the size

        virtual unsigned long GetSize ()
        {
            return m_strText .GetLength ();
        }

        // @access Protected members
    protected:

        // @cmember Pointer to the file

        CResSoundSetRef                 m_sRes;

        // @cmember Text of the file

        CStringA                                m_strText;
};

#endif // ETS_SOUNDSETVIEW_H


/* vim: set sw=4: */
