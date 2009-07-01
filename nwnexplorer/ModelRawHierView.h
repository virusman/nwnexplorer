#ifndef ETS_MODELRAWHIERVIEW_H
#define ETS_MODELRAWHIERVIEW_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ModelRawHierView.h - Hierarchy view window |
//
// This module contains the definition of the hierarchy view window.
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
#include "CommonView.h"
#include "ModelRawHierWnd.h"
#include "TextWnd.h"

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

class CModelRawHierView : 
    public CCommonView,
    public CWindowImpl <CModelRawHierView>,
    public CSplitterImpl <CModelRawHierView, false>,
    public CCustomDraw <CModelRawHierView>
{

    // @access Construction and destruction
    public:

        // @cmember General constructor

        CModelRawHierView (CData *pFile, bool fFormatted, bool fCompiled = false);

        // @cmember General destructor

        ~CModelRawHierView ();

        // @access Public virtual method
    public:

        // @cmember Get the support formats/button text

        virtual bool GetModeText (LPTSTR pszText, int cchMaxText)
        {
            ::LoadString (_Module .GetResourceInstance (), 
                          m_fFormatted ? (m_fCompiled ? IDS_MODEL_COMP_HIER :
                          IDS_MODEL_ASCII_HIER ) : 
                          IDS_MODEL_RAW_HIER, pszText, cchMaxText);
            return true;
        }

        // @cmember Get the given window

        virtual CWindow *GetWindow (HWND hWnd)
        {
            if (m_hWnd == NULL)
            {
                Create (hWnd, CWindow::rcDefault, _T (""), 
                        WS_CHILD |  WS_CLIPSIBLINGS, 0, 
                        (HMENU) 0, NULL);
            }
            return this;
        }

        // @cmember Destroy the window

        virtual void DestroyTheWindow ()
        {
            if (m_hWnd)
                DestroyWindow ();
        }

        // @cmember Handle a change in selection

        void OnNodeChange (int nOffset, HTREEITEM hItem);

        // @access ATL window support
    public:

        typedef CSplitterImpl <CModelRawHierView, false> _SplitterClass;

        BEGIN_MSG_MAP (CModelRawHierView)
            MESSAGE_HANDLER (WM_CREATE, OnCreate)
            MESSAGE_HANDLER (WM_ERASEBKGND, OnEraseBackground)
            MESSAGE_HANDLER (WM_SIZE, OnSize)
            //MESSAGE_HANDLER (WM_SETFOCUS, OnSetFocus)
            CHAIN_MSG_MAP (_SplitterClass);
        END_MSG_MAP ()

            // @access ATL Message handlers
    public:

            // @cmember Handle the creation of the window

            LRESULT OnCreate (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // @cmember Handle a size request

            LRESULT OnSize (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
            {
                if(wParam != SIZE_MINIMIZED)
                    SetSplitterRect();

                bHandled = FALSE;
                return 1;
            }

            // @cmember Don't erase the background

            LRESULT OnEraseBackground (UINT uMsg, WPARAM wParam, 
                                       LPARAM lParam, BOOL &bHandled)
            {
                return 1;
            }

            // @cmember Reflect focus to the child

            LRESULT OnSetFocus (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
            {
                if (m_wnd .m_hWnd)
                    m_wnd .SetFocus ();
                else
                    bHandled = FALSE;
                return 0;
            }

            // @access Protected members
    protected:

            // @cmember Pointer to the file

            CResModelRef                        m_sRes;

            // @cmember Actual hierarchy

            CModelRawHierWnd            m_wnd;

            // @cmember Text

            CTextWnd                            m_wndText;

            // @cmember If true, format the data

            bool                                        m_fFormatted;

            // @cmember If true, the model was compiled

            bool                                        m_fCompiled;
};

#endif // ETS_MODELRAWHIERVIEW_H

/* vim: set sw=4: */
