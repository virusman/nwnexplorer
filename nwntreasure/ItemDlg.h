#ifndef ETS_ITEMDLG_H
#define ETS_ITEMDLG_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ItemDlg.h - Edit a gold treasure |
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
// $History: ItemDlg.h $
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

class CItemObject;

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CItemDlg : 
    public CDialogImpl <CItemDlg>
{
    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CItemDlg ();

        // @cmember General destructor

        ~CItemDlg ();

        // @cmember Create the dialog

        HWND Create (CItemObject *pItem, HWND hWndParent, RECT &rc);

        // @cmember Save changes

        bool SaveChanges ();

        // @access COM interface list
    public:
        enum { IDD = IDD_ITEM };
        BEGIN_MSG_MAP (CItemDlg)
            MESSAGE_HANDLER (WM_INITDIALOG, OnInitDialog)

            COMMAND_ID_HANDLER (IDC_ITEM_BLUEPRINT_BROWSE, OnBrowseItem)

            END_MSG_MAP ()

            // @access Message handlers
    protected:

            // @cmember Initialize the dialog box

            LRESULT OnInitDialog (UINT uMsg, WPARAM wParam,
                                  LPARAM lParam, BOOL &bHandled);

            // @cmember Browse for an item

            LRESULT OnBrowseItem (WORD wNotifyCode, 
                                  WORD wID, HWND hWndCtl, BOOL& bHandled);

            // @access Protected members
    protected:

            // @cmember Object being edited

            CItemObject         *m_pObject;

            // @cmember Item blueprint

            CEdit                       m_editBlueprint;

            // @cmember Min value

            CEdit                       m_editMin;

            // @cmember Min spin

            CUpDownCtrl         m_spinMin;

            // @cmember Max value

            CEdit                       m_editMax;

            // @cmember Max spin

            CUpDownCtrl         m_spinMax;
};

#endif // ETS_ITEMDLG_H

/* vim: set sw=4: */
