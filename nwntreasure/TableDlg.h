#ifndef ETS_TABLEDLG_H
#define ETS_TABLEDLG_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      TableDlg.h - Edit a gold treasure |
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
// $History: TableDlg.h $
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

class CTableObject;
class CMainWnd;

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CTableDlg : 
    public CDialogImpl <CTableDlg>
{
    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CTableDlg (CTableObject *pObject, CMainWnd *pMainWnd, HTREEITEM hItem);

        // @cmember General destructor

        ~CTableDlg ();

        // @access COM interface list
    public:
        enum { IDD = IDD_TABLE };
        BEGIN_MSG_MAP (CTableDlg)
            MESSAGE_HANDLER (WM_INITDIALOG, OnInitDialog)

            COMMAND_HANDLER (IDC_TABLE_NAME, EN_UPDATE, OnSomethingChanged)
            COMMAND_HANDLER (IDC_GLOBAL, BN_CLICKED, OnSomethingChanged)

            COMMAND_ID_HANDLER (IDOK, OnOk)
            COMMAND_ID_HANDLER (IDCANCEL, OnCancel)
            END_MSG_MAP ()

            // @access Command handlers
    public:

            // @cmember Handle the ok button

            LRESULT OnOk (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

            // @cmember Handle the cancel button

            LRESULT OnCancel (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

            // @cmember Handle change in something

            LRESULT OnSomethingChanged (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled);

            // @access Message handlers
    protected:

            // @cmember Initialize the dialog box

            LRESULT OnInitDialog (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // @access Protected methods
    protected:

            // @cmember Update the windows

            void UpdateWindows ();

            // @access Protected members
    protected:

            // @cmember List of tables

            CMainWnd            *m_pMainWnd;

            // @cmember tree item handle

            HTREEITEM           m_hItem;

            // @cmember Object being edited

            CTableObject        *m_pObject;

            // @cmember Table name edit

            CEdit                       m_editTableName;

            // @cmember Decision mode combo

            CComboBox           m_cbDecision;

            // @cmember Global checkbox

            CButton                     m_btnGlobal;

            // @cmember Number static

            CStatic                     m_statNumber;

            // @cmember Number edit

            CEdit                       m_editNumber;

            // @cmember Number spin

            CUpDownCtrl         m_spinNumber;
};

#endif // ETS_TABLEDLG_H

/* vim: set sw=4: */
