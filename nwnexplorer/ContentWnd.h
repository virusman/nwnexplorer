#ifndef ETS_CONTENTWND_H
#define ETS_CONTENTWND_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ContentWnd.h - Content window |
//
// This module contains the definition of the content window.
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
// $History: CnfContentWnd.h $
//      
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include "resource.h"
#include "nwnexplorer.h"
#include "CustomTabCtrl.h"
#include "DotNetTabCtrl.h"
#include "BlankWnd.h"
#include "ContainerView.h"
#include "../_NwnLib/NwnKeyFile.h"

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

class CContentWnd :
    public CWindowImpl <CContentWnd>
{
    // @access Enumerations
    public:

        // @access Constructors and destructors
    public:

        // @cmember General constructor

        CContentWnd ();

        // @cmember General destructor

        ~CContentWnd ();

        // @access Public methods
    public:

        // @cmember Open existing file

        bool OpenExisting (LPCSTR pszResName, NwnResType nResType);

        // @cmember Open a resource

        void OpenResource (LPCSTR pszResName, NwnResType nResType, 
                           CData *pFile);

        // @cmember Handle idle processing

        BOOL OnIdle (CUpdateUIBase *pUpdateUI);

        // @cmember Show the given view

        void ShowView (int nIndex, int nMode);

        // @cmember Show the given view

        void ShowView (CContainerView *pView, int nMode);

        // @cmember Close all views

        void CloseAllViews ();

        // @access Public inline methods
    public:

        // @access ATL interface list
    public:

        BEGIN_MSG_MAP (CContentWnd)
            MESSAGE_HANDLER (WM_CREATE, OnCreate)
            MESSAGE_HANDLER (WM_SIZE, OnSize)
            MESSAGE_HANDLER (WM_ERASEBKGND, OnEraseBackground)
            MESSAGE_HANDLER (WM_SETFOCUS, OnSetFocus)
            MESSAGE_HANDLER (WM_MOUSEACTIVATE, OnMouseActivate)
            MESSAGE_HANDLER (WM_CONTEXTMENU, OnContextMenu)

            REFLECTED_COMMAND_ID_HANDLER (ID_RESOURCES_CLOSEVIEW, OnResourcesCloseView);
        REFLECTED_COMMAND_ID_HANDLER (ID_RESOURCES_CLOSEALLVIEWS, OnResourcesCloseAllViews);
        REFLECTED_COMMAND_ID_HANDLER (ID_RESOURCES_EXPORTTEXT, OnResourcesExportText);
        REFLECTED_COMMAND_ID_HANDLER (ID_RESOURCES_MOVELEFT, OnResourcesMoveLeft);
        REFLECTED_COMMAND_ID_HANDLER (ID_RESOURCES_MOVERIGHT, OnResourcesMoveRight);

        NOTIFY_HANDLER (IDC_EXP_TABSTOP, CTCN_SELCHANGE, OnSelChangeTabsTop);
        NOTIFY_HANDLER (IDC_EXP_TABSTOP, CTCN_CLOSE, OnCloseTabsTop);
        NOTIFY_HANDLER (IDC_EXP_TABSBOTTOM, CTCN_SELCHANGE, OnSelChangeTabsBottom);

        NOTIFY_HANDLER (IDC_EXP_TABSTOP, NM_RCLICK, OnTabBarRClick)
            NOTIFY_HANDLER (IDC_EXP_TABSBOTTOM, NM_RCLICK, OnTabBarRClick)

#ifdef XXX
            REFLECTED_NOTIFY_CODE_HANDLER (NM_DBLCLK, OnNMDblClk);
        REFLECTED_NOTIFY_CODE_HANDLER (LVN_DELETEITEM, OnDeleteItem);
        REFLECTED_NOTIFY_CODE_HANDLER (LVN_ITEMCHANGED, OnItemChanged);
        REFLECTED_NOTIFY_CODE_HANDLER (TBN_DROPDOWN, OnTBDropDown)
#endif
            END_MSG_MAP ()

            // @access ATL message handlers
    public:

            // @cmember Handle post creation actions

            LRESULT OnCreate (UINT uMsg, WPARAM wParam, 
                              LPARAM lParam, BOOL &bHandled);

            // @cmember Handle size actions

            LRESULT OnSize (UINT uMsg, WPARAM wParam, 
                            LPARAM lParam, BOOL &bHandled);

            // @cmember Don't erase the background

            LRESULT OnEraseBackground (UINT uMsg, WPARAM wParam, 
                                       LPARAM lParam, BOOL &bHandled)
            {
                return 1;
            }

            // @cmember Reflect focus to the child

            LRESULT OnSetFocus (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
            {
                if (m_pwndCurrent)
                    m_pwndCurrent ->SetFocus ();
                else
                    bHandled = FALSE;
                return 0;
            }

            LRESULT OnMouseActivate (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
            {
                LRESULT lRet = DefWindowProc (uMsg, wParam, lParam);
                if (lRet == MA_ACTIVATE || lRet == MA_ACTIVATEANDEAT)
                {
                    if (m_pwndCurrent)
                        m_pwndCurrent ->SetFocus ();
                }
                return lRet;
            }

            // @cmember Handle context menu

            LRESULT OnContextMenu (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // Notification handlers
    public:

            // @cmember Handle tabs sel change

            LRESULT OnSelChangeTabsTop (int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

            // @cmember Handle tabs sel close

            LRESULT OnCloseTabsTop (int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

            // @cmember Handle tabs sel change

            LRESULT OnSelChangeTabsBottom (int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

            // @cmember Handle right click of top tab bar

            LRESULT OnTabBarRClick (int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

            // Command handlers
    public:

            // @cmember Handle view close

            LRESULT OnResourcesCloseView (WORD wNotifyCode, 
                                          WORD wID, HWND hWndCtl, BOOL& bHandled);

            // @cmember Handle close all views

            LRESULT OnResourcesCloseAllViews (WORD wNotifyCode, 
                                              WORD wID, HWND hWndCtl, BOOL& bHandled);

            // @cmember Handle exporting text

            LRESULT OnResourcesExportText (WORD wNotifyCode, 
                                           WORD wID, HWND hWndCtl, BOOL& bHandled);

            // @cmember Move one tab to the left

            LRESULT OnResourcesMoveLeft (WORD wNotifyCode, 
                                         WORD wID, HWND hWndCtl, BOOL& bHandled);

            // @cmember Move one tab to the right

            LRESULT OnResourcesMoveRight (WORD wNotifyCode, 
                                          WORD wID, HWND hWndCtl, BOOL& bHandled);

            // @access Protected methods
    protected:

            // @cmember Compute the height of a tab

            int CalcTabAreaHeight (CWindow &wndTab);

            // @cmember Close the given item

            void CloseItem (int nItem);

            // @access Protected variables
    protected:

            // @cmember Upper tab control

            CDotNetTabCtrl<>            m_tcTop;

            // @cmember Height of the tab control

            int                                         m_nTopHeight;

            // @cmember Lower button control

            CDotNetButtonTabCtrl<>      m_tcBottom;

            // @cmember Height of the tab control

            int                                         m_nBottomHeight;

            // @cmember Blank window

            CBlankWnd                           m_wndBlank;

            // @cmember Current window

            CWindow                                     *m_pwndCurrent;

            // @cmember Current view

            CContainerView                      *m_pCurrent;

            // @cmember List of windows

            CAtlArray <CContainerView *> m_apViews;
};

#endif // ETS_CONTENTWND_H

/* vim: set sw=4: */
