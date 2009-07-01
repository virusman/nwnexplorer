#ifndef ETS_ITEMBROWSERDLG_H
#define ETS_ITEMBROWSERDLG_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ItemBrowserDlg.h - Edit a gold treasure |
//
// This module contains the definition for the gold dialog.
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
// $History: ItemBrowserDlg.h $
//      
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include "../_NwnLib/NwnHierarchy.h"

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

class CTableObject;
class CMainWnd;

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CItemBrowserDlg : 
    public CDialogImpl <CItemBrowserDlg>
{
    // @access Browse for what
    public:

        enum Browse_What
        {
            Browse_What_Items           = 0,
            Browse_What_Encounters      = 1,
            Browse_What_Placeables      = 2,
            Browse_What_EncPlace        = 3,
        };

        // @access Constructors and destructors
    public:

        // @cmember General constructor

        CItemBrowserDlg (Browse_What nBrowseWhat);

        // @cmember General destructor

        ~CItemBrowserDlg ();

        // @access COM interface list
    public:
        enum { IDD = IDD_ITEM_BROWSER };
        BEGIN_MSG_MAP (CItemBrowserDlg)
            MESSAGE_HANDLER (WM_INITDIALOG, OnInitDialog)

            COMMAND_HANDLER (IDC_SOURCE, CBN_SELCHANGE, OnSourceSelChange) 
            COMMAND_HANDLER (IDC_BROWSE_FOR, CBN_SELCHANGE, OnCbnSelchangeBrowseFor)

            NOTIFY_HANDLER (IDC_ITEMS, TVN_SELCHANGED, OnTVSelChanged)
            NOTIFY_HANDLER (IDC_ITEMS, NM_DBLCLK, OnNMDblClk);

        COMMAND_ID_HANDLER (IDOK, OnOk)
            COMMAND_ID_HANDLER (IDCANCEL, OnCancel)
            END_MSG_MAP ()

            // @access Command handlers
    public:

            // @cmember Handle a change in the selection

            LRESULT OnSourceSelChange (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled);

            // @cmember Handle the ok button

            LRESULT OnOk (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

            // @cmember Handle the cancel button

            LRESULT OnCancel (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

            // @cmember Handle the cancel button

            LRESULT OnCbnSelchangeBrowseFor (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

            // @cmember Handle selection change

            LRESULT OnTVSelChanged (int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

            // @cmember Handle a double click

            LRESULT OnNMDblClk (int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

            // @access Message handlers
    protected:

            // @cmember Initialize the dialog box

            LRESULT OnInitDialog (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // @access Protected methods
    protected:

            // @cmember Add elements to the tree

            void AddTree (CNwnHierarchy &sPal, int nEntry, 
                          HTREEITEM hParent, HTREEITEM &hSel);

            // @cmember Show the current combo box setting

            void ShowCurrent ();

            // @access Public variables
    public:

            // @cmember Selection

            CString                                     m_strSelection;

            // @access Protected members
    protected:

            // @cmember Tree view

            CTreeViewCtrl                       m_tv;

            // @cmember Source combo

            CComboBox                           m_cbSource;

            // @cmember Browse for combo

            CComboBox                           m_cbBrowseFor;

            // @cmember Browse for static

            CStatic                                     m_statBrowseFor;

            // @cmember Browse what

            Browse_What                         m_nBrowseWhat;
};

#endif // ETS_ITEMBROWSERDLG_H

/* vim: set sw=4: */
