#ifndef ETS_TABLEITEMDLG_H
#define ETS_TABLEITEMDLG_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      TableItemDlg.h - Edit a gold treasure |
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
// $History: NoneDlg.h $
//      
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include "Object.h"
#include "GoldDlg.h"
#include "ItemDlg.h"
#include "TableRefDlg.h"
#include "EncounterDlg.h"
#include "PlaceableDlg.h"

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

class CTableObject;
class CTableItemObject;
class CGoldObject;
class CItemObject;
class CTableRefObject;
class CEncounterObject;

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CTableItemDlg : 
    public CDialogImpl <CTableItemDlg>
{
    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CTableItemDlg (CMainWnd *pMainWnd, HTREEITEM hItem,
                       CTableObject *pTable, CTableItemObject *pObject);

        // @cmember General destructor

        ~CTableItemDlg ();

        // @access COM interface list
    public:
        enum { IDD = IDD_TABLE_ITEM };
        BEGIN_MSG_MAP (CTableItemDlg)
            MESSAGE_HANDLER (WM_INITDIALOG, OnInitDialog)

            COMMAND_HANDLER (IDC_ACTION, CBN_SELCHANGE, OnActionSelChange) 

            COMMAND_ID_HANDLER (IDOK, OnOk)
            COMMAND_ID_HANDLER (IDCANCEL, OnCancel)
            END_MSG_MAP ()

            // @access Command handlers
    public:

            // @cmember Handle a change in the selection

            LRESULT OnActionSelChange (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled);

            // @cmember Handle the ok button

            LRESULT OnOk (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

            // @cmember Handle the cancel button

            LRESULT OnCancel (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

            // @access Message handlers
    protected:

            // @cmember Initialize the dialog box

            LRESULT OnInitDialog (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // @cmember Show the current dialog

            void ShowDialog ();

            // @access Public variables
    public:

            // @cmember Object being edited

            CTableItemObject    *m_pObject;

            // @access Protected variables
    protected:

            // @cmember Main window

            CMainWnd                    *m_pMainWnd;

            // @cmember Handle of the item being edited

            HTREEITEM                   m_hItem;

            // @cmember Parent table

            CTableObject                *m_pTable;

            // @cmember Action static

            CStatic                             m_statAction;

            // @cmember Action combo box

            CComboBox                   m_cbAction;

            // @cmember Chance static

            CStatic                             m_statChance;

            // @cmember Chance edit

            CEdit                               m_editChance;

            // @cmember Chance spin

            CUpDownCtrl                 m_spinChance;

            // @cmember Chance combo

            CComboBox                   m_cbChance;

            // @cmember Current object type

            Object_Type                 m_nObjectType;

            // @cmember Current window

            HWND                                m_hWndCurrent;

            // @cmember Gold dialog

            CGoldDlg                    m_dlgGold;

            // @cmember Gold object

            CGoldObject                 *m_pGoldObject;

            // @cmember Item dialog

            CItemDlg                    m_dlgItem;

            // @cmember Item object

            CItemObject                 *m_pItemObject;

            // @cmember Tableref dialog

            CTableRefDlg                m_dlgTableRef;

            // @cmember Item object

            CTableRefObject             *m_pTableRefObject;

            // @cember Encounter dialog

            CEncounterDlg               m_dlgEncounter;

            // @cmember Encounter object

            CEncounterObject    *m_pEncounterObject;

            // @cember Placeable dialog

            CPlaceableDlg               m_dlgPlaceable;

            // @cmember Encounter object

            CPlaceableObject    *m_pPlaceableObject;
};

#endif // ETS_TABLEITEMDLG_H

/* vim: set sw=4: */
