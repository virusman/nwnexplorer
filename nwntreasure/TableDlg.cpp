//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      TableDlg.cpp - None treasure dialog |
//
// This module contains the definition None treasure dialog
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

#include "stdafx.h"
#include "resource.h"
#include "TableDlg.h"
#include "TableObject.h"
#include "MainWnd.h"

//
// Global variables
//

extern TCHAR g_szAppName [];
extern BiowareTables g_asBiowareTables [];
extern int g_nBiowareTables;

//
// Decision array
//

struct Decisions
{
    Table_Command nCmd;
    UINT nID;
};

Decisions g_asTreasureDes [] = 
{
    { Table_Command_Pick_One,                   IDS_TREASURE_COMMAND_01 },
    { Table_Command_Pick_Multiple,              IDS_TREASURE_COMMAND_02 },
    { Table_Command_Racial_Type,                IDS_TREASURE_COMMAND_03 },
    { Table_Command_Level_Range,                IDS_TREASURE_COMMAND_04 },
    { Table_Command_Random_Class_Type,  IDS_TREASURE_COMMAND_05 },
    { Table_Command_Specific,                   IDS_TREASURE_COMMAND_06 },
    { Table_Command_Weapon_Focus,               IDS_TREASURE_COMMAND_07 },
    { Table_Command_Armor_Proficiency,  IDS_TREASURE_COMMAND_08 },
    { Table_Command_Class_Type,                 IDS_TREASURE_COMMAND_09 },
};

Decisions g_asEncounterDes [] = 
{
    { Table_Command_Pick_One,                   IDS_TREASURE_COMMAND_01 },
    { Table_Command_Pick_Multiple,              IDS_TREASURE_COMMAND_02 },
};

Decisions g_asPlaceableDes [] = 
{
    { Table_Command_Pick_One,                   IDS_TREASURE_COMMAND_01 },
    { Table_Command_Pick_Multiple,              IDS_TREASURE_COMMAND_02 },
};

//-----------------------------------------------------------------------------
//
// @mfunc <c CTableDlg> constructor.
//
// @parm CTableObject * | pObject | Object being edited
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CTableDlg::CTableDlg (CTableObject *pObject, 
                      CMainWnd *pMainWnd, HTREEITEM hItem)
{
    m_pObject = pObject;
    m_pMainWnd = pMainWnd;
    m_hItem = hItem;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CTableDlg> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CTableDlg::~CTableDlg ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc Initialize the dialog
//
// @parm UINT | uMsg | Message
//
// @parm WPARAM | wParam | Message w-parameter
//
// @parm LPARAM | lParam | Message l-parameter
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CTableDlg::OnInitDialog (UINT uMsg, 
                                 WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{
    CString str;
    int i;

    //
    // Center the dialog
    //

    CenterWindow ();

    //
    // Get the controls
    //

    m_editTableName = GetDlgItem (IDC_TABLE_NAME);
    m_cbDecision = GetDlgItem (IDC_DECISION_MODE);
    m_btnGlobal = GetDlgItem (IDC_GLOBAL);
    m_editTableName .LimitText (64);
    m_statNumber = GetDlgItem (IDC_NUMBER_STATIC);
    m_editNumber = GetDlgItem (IDC_NUMBER);
    m_spinNumber = GetDlgItem (IDC_NUMBER_SPIN);
    m_editNumber .LimitText (4);
    m_spinNumber .SetRange (100, 9999);

    //
    // Get the tables to use
    //

    Decisions *pDes = NULL;
    int nDes = 0;
    if (m_pObject ->GetTableType () == Table_Type_Treasure)
    {
        pDes = g_asTreasureDes;
        nDes = _countof (g_asTreasureDes);
    }
    else if (m_pObject ->GetTableType () == Table_Type_Encounter)
    {
        pDes = g_asEncounterDes;
        nDes = _countof (g_asEncounterDes);
    }
    else if (m_pObject ->GetTableType () == Table_Type_Placeable)
    {
        pDes = g_asPlaceableDes;
        nDes = _countof (g_asPlaceableDes);
    }
    else
        _ASSERTE (FALSE);

    //
    // Load the combo
    //

    for (i = 0; i < nDes; i++)
    {
        str .LoadString (pDes [i] .nID);
        m_cbDecision .AddString (str);
    }

    //
    // Initialize
    //

    m_editTableName .SetWindowText (m_pObject ->GetName ());
    m_btnGlobal .SetCheck (m_pObject ->GetGlobal ());
    for (i = 0; i < nDes; i++)
    {
        if (pDes [i] .nCmd == m_pObject ->GetTableCommand ())
            break;
    }
    if (i >= nDes)
        i = 0;
    str .LoadString (pDes [i] .nID);
    m_cbDecision .SelectString (-1, str);

    //
    // Update the number
    //

    int nNumber = m_pObject ->GetNumber ();
    if (nNumber < 100)
        nNumber = 100;
    str .Format ("%d", nNumber);
    m_editNumber .SetWindowText (str);

    //
    // Update the windows
    //

    UpdateWindows ();
    return FALSE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle the user press Ok
//
// @parm WORD | wNotifyCode | Command notification code
// 
// @parm WORD | wID | ID of the control
//
// @parm HWND | hWndCtl | Handle of the control
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CTableDlg::OnOk (WORD wNotifyCode, 
                         WORD wID, HWND hWndCtl, BOOL& bHandled) 
{
    CString str;

    //
    // Get the tables to use
    //

    Decisions *pDes = NULL;
    int nDes = 0;
    if (m_pObject ->GetTableType () == Table_Type_Treasure)
    {
        pDes = g_asTreasureDes;
        nDes = _countof (g_asTreasureDes);
    }
    else if (m_pObject ->GetTableType () == Table_Type_Encounter)
    {
        pDes = g_asEncounterDes;
        nDes = _countof (g_asEncounterDes);
    }
    else if (m_pObject ->GetTableType () == Table_Type_Placeable)
    {
        pDes = g_asPlaceableDes;
        nDes = _countof (g_asPlaceableDes);
    }
    else
        _ASSERTE (FALSE);

    //
    // Get the combo
    //

    Table_Command nCmd = Table_Command_None;
    int nSel = m_cbDecision .GetCurSel ();
    m_cbDecision .GetLBText (nSel, str);
    for (int i = 0; i < nDes; i++)
    {
        CString str2;
        str2 .LoadString (pDes [i] .nID);
        if (str2 == str)
        {
            nCmd = pDes [i] .nCmd;
        }
    }

    //
    // Get the table number
    //

    int nNumber = -1;
    if (m_editNumber .IsWindowEnabled ())
    {
        m_editNumber .GetWindowText (str);
        nNumber = _ttol (str);
        if (nNumber < 100 || nNumber > 9999)
        {
            str .Format (IDS_ERR_TABLE_NUMBER);
            ::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
            return TRUE;
        }
        for (int i = 0; i < m_pMainWnd ->GetTableCount (); i++)
        {
            CTableObject *pTable = m_pMainWnd ->GetTable (i);
            if (pTable != m_pObject)
            {
                if (pTable ->GetGlobal ())
                {
                    if (pTable ->GetNumber () == nNumber)
                    {
                        str .Format (IDS_ERR_DUP_TABLE_NUMBER);
                        ::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
                        return TRUE;
                    }
                }
            }
        }
    }

    //
    // Handle the table name change
    //

    CString strName;
    m_editTableName .GetWindowText (strName);
    if (!IsValidTableName (strName))
    {
        str .Format (IDS_ERR_INVALID_TABLE_NAME);
        ::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
        return TRUE;
    }
    if (!m_pMainWnd ->HandleTableNameChange (
            m_pObject, strName, m_hItem))
        return TRUE;

    //
    // Save the global
    //

    m_pMainWnd ->HandleTableGlobalChange (m_pObject, 
                                          m_btnGlobal .GetCheck () != 0, m_hItem);

    //
    // Save the new settings
    //

    m_pObject ->SetTableCommand (nCmd);
    m_pObject ->SetNumber (nNumber);
    EndDialog (IDOK);
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle the user press Ok
//
// @parm WORD | wNotifyCode | Command notification code
// 
// @parm WORD | wID | ID of the control
//
// @parm HWND | hWndCtl | Handle of the control
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CTableDlg::OnCancel (WORD wNotifyCode, 
                             WORD wID, HWND hWndCtl, BOOL& bHandled) 
{
    EndDialog (IDCANCEL);
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle something important changing
//
// @parm WORD | wNotifyCode | Command notification code
// 
// @parm WORD | wID | ID of the control
//
// @parm HWND | hWndCtl | Handle of the control
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CTableDlg::OnSomethingChanged (WORD wNotifyCode, 
                                       WORD wID, HWND hWndCtl, BOOL& bHandled) 
{
    UpdateWindows ();
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Update the windows
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CTableDlg::UpdateWindows () 
{
    USES_CONVERSION;

    //
    // See if the global flag is enable and if the table name is 
    // a bioware table name
    //

    bool fEnable = m_btnGlobal .GetCheck () != 0;
    if (fEnable)
    {
        CString str;
        m_editTableName .GetWindowText (str);
        LPCSTR psz = T2CA (str);
        for (int i = 0; i < g_nBiowareTables; i++)
        {
            if (strcmp (psz, g_asBiowareTables [i] .pszScript) == 0)
            {
                fEnable = false;
                break;
            }
        }
    }

    //
    // Enable/Disable
    //

    m_statNumber .EnableWindow (fEnable);
    m_editNumber .EnableWindow (fEnable);
    m_spinNumber .EnableWindow (fEnable);
    return;
}


/* vim: set sw=4: */
