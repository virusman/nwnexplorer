//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ProfileDlg.cpp - None treasure dialog |
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
// $History: ProfileDlg.h $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"
#include "ProfileDlg.h"
#include "ProfileObject.h"
#include "MainWnd.h"
#include "TableObject.h"
#include "ItemBrowserDlg.h"

//
// Global variables
//

extern TCHAR g_szAppName [];

//-----------------------------------------------------------------------------
//
// @mfunc <c CProfileDlg> constructor.
//
// @parm CProfileObject * | pObject | Object being edited
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CProfileDlg::CProfileDlg (CProfileObject *pObject, 
                          CMainWnd *pMainWnd, HTREEITEM hItem)
{
    m_pObject = pObject;
    m_pMainWnd = pMainWnd;
    m_hItem = hItem;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CProfileDlg> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CProfileDlg::~CProfileDlg ()
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

LRESULT CProfileDlg::OnInitDialog (UINT uMsg, 
                                   WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{
    CString str;

    //
    // Center the dialog
    //

    CenterWindow ();

    //
    // Get the controls
    //

    m_editName = GetDlgItem (IDC_PROFILE_NAME);
    m_cbRespawnType = GetDlgItem (IDC_RESPAWN_TYPE);
    m_statRespawnTime = GetDlgItem (IDC_RESPAWN_TIME_STATIC);
    m_editRespawnTime = GetDlgItem (IDC_RESPAWN_TIME);
    m_spinRespawnTime = GetDlgItem (IDC_RESPAWN_TIME_SPIN);
    m_statRespawnName = GetDlgItem (IDC_RESPAWN_NAME_STATIC);
    m_cbRespawnName = GetDlgItem (IDC_RESPAWN_NAME);
    m_btnRespawnNameBrowse = GetDlgItem (IDC_RESPAWN_NAME_BROWSE);
    m_cbDeathOpenSpawn = GetDlgItem (IDC_DEATHOPEN_SPAWN);
    m_btnDeathOpenSpawnBrowse = GetDlgItem (IDC_DEATHOPEN_SPAWN_BROWSE);
    m_editDeathOpenSound = GetDlgItem (IDC_DEATHOPEN_SOUND);
    m_editDeathOpenDamage = GetDlgItem (IDC_DEATHOPEN_DAMAGE);
    m_spinDeathOpenDamage = GetDlgItem (IDC_DEATHOPEN_DAMAGE_SPIN);
    m_cbTreasureTable = GetDlgItem (IDC_TREASURE_TABLE);

    //
    // Initialize the name
    //

    m_editName .LimitText (64);
    m_editName .SetWindowText (m_pObject ->GetName ());

    //
    // Initialize the repsawn type
    //

    str .LoadString (IDS_RESPAWN_TYPE_NONE);
    m_cbRespawnType .AddString (str);
    str .LoadString (IDS_RESPAWN_TYPE_RESPAWN);
    m_cbRespawnType .AddString (str);
    m_cbRespawnType .SetCurSel (m_pObject ->GetRespawnType ());

    //
    // Initialize the respawn time
    //

    m_editRespawnTime .LimitText (5);
    m_spinRespawnTime .SetRange (0, 99999);
    str .Format ("%d", m_pObject ->GetRespawnTime ());
    m_editRespawnTime .SetWindowText (str);

    //
    // Initialize the respawn name
    //

    m_cbRespawnName .LimitText (64);
    str .LoadString (IDS_PAREN_DEFAULT);
    m_cbRespawnName .AddString (str);
    for (int i = 0; i < m_pMainWnd ->GetTableCount (); i++)
    {
        CTableObject *pTable = m_pMainWnd ->GetTable (i);
        if (pTable ->GetGlobal () && 
            (pTable ->GetTableType () == Table_Type_Encounter ||
             pTable ->GetTableType () == Table_Type_Placeable))
            m_cbRespawnName .AddString (pTable ->GetName ());
    }
    if (m_pObject ->GetRespawnName () .IsEmpty ())
        m_cbRespawnName .SetWindowText (str);
    else
        m_cbRespawnName .SetWindowText (m_pObject ->GetRespawnName ());

    //
    // Initialize the deathopen spawn combo
    //

    m_cbDeathOpenSpawn .LimitText (64);
    for (int i = 0; i < m_pMainWnd ->GetTableCount (); i++)
    {
        CTableObject *pTable = m_pMainWnd ->GetTable (i);
        if (pTable ->GetGlobal () && pTable ->GetTableType () == Table_Type_Encounter)
            m_cbDeathOpenSpawn .AddString (pTable ->GetName ());
    }
    m_cbDeathOpenSpawn .SetWindowText (m_pObject ->GetDeathOpenSpawn ());

    //
    // Initialize the deathopen sound
    //

    m_editDeathOpenSound .LimitText (64);
    m_editDeathOpenSound .SetWindowText (m_pObject ->GetDeathOpenSound ());

    //
    // Initialize the deathopen damage
    //

    m_editDeathOpenDamage .LimitText (3);
    m_spinDeathOpenDamage .SetRange (0, 999);
    str .Format ("%d", m_pObject ->GetDeathOpenDamage ());
    m_editDeathOpenDamage .SetWindowText (str);

    //
    // Initialize the treasure tables
    //

    str .LoadString (IDS_PAREN_DEFAULT);
    m_cbTreasureTable .AddString (str);
    for (int i = 0; i < m_pMainWnd ->GetTableCount (); i++)
    {
        CTableObject *pTable = m_pMainWnd ->GetTable (i);
        if (pTable ->GetGlobal () && 
            (pTable ->GetTableType () == Table_Type_Treasure))
            m_cbTreasureTable .AddString (pTable ->GetName ());
    }
    if (m_pObject ->GetTreasureTable () .IsEmpty ())
        m_cbTreasureTable .SetCurSel (0);
    else
    {
        int nSel = m_cbTreasureTable .FindStringExact (
            -1, m_pObject ->GetTreasureTable ());
        if (nSel < 0)
            nSel = 0;
        m_cbTreasureTable .SetCurSel (nSel);
    }


    //
    // Update the windows
    //

    UpdateWindows ();
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

LRESULT CProfileDlg::OnOk (WORD wNotifyCode, 
                           WORD wID, HWND hWndCtl, BOOL& bHandled) 
{
    CString str;

    //
    // Get the name
    //

    CString strName;
    m_editName .GetWindowText (strName);
    if (!IsValidProfileName (strName))
    {
        str .Format (IDS_ERR_INVALID_PROFILE_NAME);
        ::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
        return false;
    }

    //
    // Get the respawn information
    //

    CString strRespawnName;
    int nRespawnTime = 5 * 60;
    Respawn_Type nRespawnType = (Respawn_Type) m_cbRespawnType .GetCurSel ();
    if (nRespawnType == Respawn_TypeResourcepawn)
    {

        //
        // Get the time
        //

        m_editRespawnTime .GetWindowText (str);
        nRespawnTime = _ttol (str);
        if (nRespawnTime < 0 || nRespawnTime > 99999)
        {
            str .Format (IDS_ERR_RESPAWN_TIME);
            ::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
            return false;
        }

        //
        // Get the name
        //

        m_cbRespawnName .GetWindowText (strRespawnName);
        str .LoadString (IDS_PAREN_DEFAULT);
        if (str == strRespawnName)
            strRespawnName .Empty ();
        else
        {
            if (!IsValidBlueprintName (strRespawnName))
            {
                str .Format (IDS_ERR_INVALID_BLUEPRINT_NAME);
                ::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
                return false;
            }
        }
    }

    //
    // Get the deathopen spawn
    //

    CString strDeathOpenSpawn;
    m_cbDeathOpenSpawn .GetWindowText (strDeathOpenSpawn);
    if (!strDeathOpenSpawn .IsEmpty ())
    {
        if (!IsValidTableName (strDeathOpenSpawn))
        {
            str .Format (IDS_ERR_INVALID_TABLE_NAME);
            ::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
            return false;
        }
    }

    //
    // Get the deathopen sound
    //

    CString strDeathOpenSound;
    m_editDeathOpenSound .GetWindowText (strDeathOpenSound);
    if (!strDeathOpenSound .IsEmpty ())
    {
        if (!IsValidBlueprintName (strDeathOpenSound))
        {
            str .Format (IDS_ERR_INVALID_BLUEPRINT_NAME);
            ::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
            return false;
        }
    }

    //
    // Get the deathopen damage
    //

    m_editDeathOpenDamage .GetWindowText (str);
    int nDeathOpenDamage = _ttol (str);
    if (nDeathOpenDamage < 0 || nDeathOpenDamage > 999)
    {
        str .Format (IDS_ERR_INVALID_DAMAGE);
        ::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
        return false;
    }

    //
    // Get the treasure table
    //

    CString strTreasureTable;
    m_cbTreasureTable .GetWindowText (strTreasureTable);
    str .LoadString (IDS_PAREN_DEFAULT);
    if (str == strTreasureTable)
        strTreasureTable .Empty ();
    else
    {
        if (!IsValidTableName (strTreasureTable))
        {
            str .Format (IDS_ERR_INVALID_TABLE_NAME);
            ::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
            return false;
        }
    }

    //
    // Save the new settings
    //

    m_pObject ->SetName (strName);
    m_pObject ->SetRespawnType (nRespawnType);
    m_pObject ->SetRespawnTime (nRespawnTime);
    m_pObject ->SetRespawnName (strRespawnName);
    m_pObject ->SetDeathOpenSpawn (strDeathOpenSpawn);
    m_pObject ->SetDeathOpenSound (strDeathOpenSound);
    m_pObject ->SetDeathOpenDamage (nDeathOpenDamage);
    m_pObject ->SetTreasureTable (strTreasureTable);
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

LRESULT CProfileDlg::OnCancel (WORD wNotifyCode, 
                               WORD wID, HWND hWndCtl, BOOL& bHandled) 
{
    EndDialog (IDCANCEL);
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Update the windows
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CProfileDlg::UpdateWindows ()
{

    //
    // Update the respawns
    //

    bool fRespawn = m_cbRespawnType .GetCurSel () != 0;
    m_statRespawnTime .EnableWindow (fRespawn);
    m_editRespawnTime .EnableWindow (fRespawn);
    m_spinRespawnTime .EnableWindow (fRespawn);
    m_cbRespawnName .EnableWindow (fRespawn);
    m_statRespawnName .EnableWindow (fRespawn);
    m_btnRespawnNameBrowse .EnableWindow (fRespawn);
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle the user press about
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

LRESULT CProfileDlg::OnBrowseRespawnName (WORD wNotifyCode, 
                                          WORD wID, HWND hWndCtl, BOOL &bHandled) 
{
    CItemBrowserDlg dlg (CItemBrowserDlg::Browse_What_EncPlace);
    m_cbRespawnName .GetWindowText (dlg .m_strSelection);
    if (dlg .DoModal () == IDOK)
        m_cbRespawnName .SetWindowText (dlg .m_strSelection);
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle the user press about
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

LRESULT CProfileDlg::OnBrowseDeathOpenSpawn (WORD wNotifyCode, 
                                             WORD wID, HWND hWndCtl, BOOL &bHandled) 
{
    CItemBrowserDlg dlg (CItemBrowserDlg::Browse_What_Encounters);
    m_cbDeathOpenSpawn .GetWindowText (dlg .m_strSelection);
    if (dlg .DoModal () == IDOK)
        m_cbDeathOpenSpawn .SetWindowText (dlg .m_strSelection);
    return TRUE;
}


/* vim: set sw=4: */
