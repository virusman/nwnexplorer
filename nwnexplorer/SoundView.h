#ifndef ETS_SOUNDVIEW_H
#define ETS_SOUNDVIEW_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      SoundView.h - Sound view window |
//
// This module contains the definition of the sound view window.
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
#include "include\fmod\fmod.h"

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

class CSoundView :
    public CCommonView,
    public CDialogImpl <CSoundView>
{

    // @access Construction and destruction
    public:

        // @cmember General constructor

        CSoundView (CData *pFile);

        // @cmember General destructor

        ~CSoundView ();

        // @access Public virtual method
    public:

        // @cmember Get the support formats/button text

        virtual bool GetModeText (LPTSTR pszText, int cchMaxText)
        {
            ::LoadString (_Module .GetResourceInstance (), 
                          IDS_AUDIO, pszText, cchMaxText);
            return true;
        }

        // @cmember Get the given window

        virtual CWindow *GetWindow (HWND hWnd)
        {
            if (m_hWnd == NULL)
            {
                Create (hWnd, rcDefault);
            }
            BOOL bHandled = TRUE;
            OnTimer (0, 0, 0, bHandled);
            return this;
        }

        // @cmember Destroy the window

        virtual void DestroyTheWindow ()
        {
            if (m_hWnd)
                DestroyWindow ();
        }

        // @access ATL window support
    public:

        enum { IDD = IDD_SOUND };

        BEGIN_MSG_MAP (CSoundView)
            MESSAGE_HANDLER (WM_INITDIALOG, OnInitDialog)
            MESSAGE_HANDLER (WM_ERASEBKGND, OnEraseBackground)
            MESSAGE_HANDLER (WM_TIMER, OnTimer)
            COMMAND_ID_HANDLER (IDC_PLAY, OnPlay)
            END_MSG_MAP ()

            // @access ATL Message handlers
    protected:

            LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL &bHandled)
            {
                ModifyStyleEx (0, WS_EX_CLIENTEDGE);
                SetTimer (1, 250, NULL);
                OnTimer (0, 0, 0, bHandled);
                return FALSE;
            }

            // @cmember Don't erase the background

            LRESULT OnEraseBackground (UINT uMsg, WPARAM wParam, 
                                       LPARAM lParam, BOOL &bHandled)
            {
                CDCHandle dc ((HDC) wParam);
                CBrush br;
                br .CreateSolidBrush (GetSysColor (COLOR_WINDOW));
                CRect rect;
                GetClientRect (&rect);
                dc .FillRect (rect, br);
                return 1;
            }

            // @cmember Handle timer

            LRESULT OnTimer (UINT uMsg, WPARAM wParam, 
                             LPARAM lParam, BOOL &bHandled);

            // @cmember Handle a play request

            LRESULT OnPlay (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled);

            // @access Protected methods
    protected:

            // @cmember Handle stream complete

            static signed char F_CALLBACKAPI StreamCallback (
                FSOUND_STREAM *stream, void *buff, int len, void * param);

            // @access Protected members
    protected:

            // @cmember Pointer to the file

            CDataRef                    m_sRes;

            // @cmember FMOD stream

            FSOUND_STREAM                       *m_pStream;
};

#endif // ETS_SOUNDVIEW_H

/* vim: set sw=4: */
