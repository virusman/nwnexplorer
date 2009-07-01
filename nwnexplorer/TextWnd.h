#ifndef ETS_TEXTWND_H
#define ETS_TEXTWND_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      TextWnd.h - Text window |
//
// This module contains the definition of the text window.
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

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

struct TextLine2
{
    const char *pachStart;
    int nLength;
};

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CTextWnd :
    public CScrollWindowImpl <CTextWnd>
{

    // @access Construction and destruction
    public:
        DECLARE_WND_CLASS_EX(NULL, 0, -1)

            // @cmember General constructor

            CTextWnd ();

        // @cmember General destructor

        ~CTextWnd ();

        // @access Public members
    public:

        // @cmember Set the text of the window

        void SetText (const char *pszText);

        // @access ATL window support
    public:

        BEGIN_MSG_MAP (CTextWnd)
            MESSAGE_HANDLER (WM_CREATE, OnCreate)
            MESSAGE_HANDLER (WM_ERASEBKGND, OnEraseBackground)
            MESSAGE_HANDLER (WM_KEYDOWN, OnKeyDown)
            MESSAGE_HANDLER (WM_SIZE, OnSize)
            CHAIN_MSG_MAP (CScrollWindowImpl <CTextWnd>);
        END_MSG_MAP ()

            // @access ATL Message handlers
    protected:

            // @cmember Handle the creation of the window

            LRESULT OnCreate (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // @cmember Handle a keydown

            LRESULT OnKeyDown (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // @cmember Don't erase the background

            LRESULT OnEraseBackground (UINT uMsg, WPARAM wParam, 
                                       LPARAM lParam, BOOL &bHandled)
            {
                return 1;
            }

            // @cmember Handle the size

            LRESULT OnSize (UINT uMsg, WPARAM wParam, 
                            LPARAM lParam, BOOL &bHandled)
            {
                LRESULT lResult = CScrollWindowImpl <CTextWnd>::
                    OnSize (uMsg, wParam, lParam, bHandled);
                SetScrollPage (m_sizeClient);
                return lResult;
            }

            // @access ATL overrides
    public:

            // @cmember Paint the window

            void DoPaint (CDCHandle dc);

            // @access Protected members
    protected:

            // @cmember Total size of the document

            CSize                                       m_sizeTotal;

            // @cmember Array of text

            CAtlArray <TextLine2>       m_asLines;

            // @cmember Max length

            int                                         m_nMaxLength;

            // @cmember Text

            CStringA                            m_strText;
};

#endif // ETS_TEXTWND_H


/* vim: set sw=4: */
