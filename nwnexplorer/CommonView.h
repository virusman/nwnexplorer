#ifndef ETS_COMMONVIEW_H
#define ETS_COMMONVIEW_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      CommonView.h - Common base class for all views |
//
// This module contains the definition common base class for all views.
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
// $History: CommonView.h $
//      
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

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

class CCommonView
{

    // @access Construction and destruction
    public:

        // @cmember General constructor

        CCommonView ();

        // @cmember General destructor

        virtual ~CCommonView ();

        // @access Public virtual methods
    public:

        // @cmember Get the support formats/button text

        virtual bool GetModeText (LPTSTR pszText, int cchMaxText) = 0;

        // @cmember Get the given window

        virtual CWindow *GetWindow (HWND hWnd) = 0;

        // @cmember Destroy the window

        virtual void DestroyTheWindow () = 0;

        // @cmember Can export text

        virtual bool CanExportText () const 
        {
            return false;
        }

        // @cmember Export the text

        virtual bool ExportText (FILE *fp) 
        {
            return true;
        }

        // @access Protected members
    protected:

};

#endif // ETS_COMMONVIEW_H


/* vim: set sw=4: */
