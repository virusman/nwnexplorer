//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      TableItemDlg.cpp - None treasure dialog |
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
// $History: TableItemDlg.h $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"
#include "TableItemDlg.h"
#include "TableItemObject.h"
#include "TableObject.h"
#include "GoldObject.h"
#include "ItemObject.h"
#include "NoneObject.h"
#include "TableRefObject.h"
#include "EncounterObject.h"
#include "PlaceableObject.h"

//
// Global variables
//

extern TCHAR g_szAppName [];

//
// Object type to string array
//

struct ObjectToStr
{
    Object_Type nObjectType;
    UINT nID;
};

ObjectToStr g_asTreasureStr [] = 
{
    { Object_Type_None,                 IDS_DROP_NOTHING },
    { Object_Type_Gold,                 IDS_DROP_GOLD },
    { Object_Type_Item,                 IDS_DROP_ITEM },
    { Object_Type_Table_Ref,    IDS_DROP_TABLE },
};

ObjectToStr g_asEncounterStr [] = 
{
    { Object_Type_None,                 IDS_SPAWN_NOTHING },
    { Object_Type_Encounter,    IDS_SPAWN_ENCOUNTER },
    { Object_Type_Table_Ref,    IDS_SPAWN_TABLE },
};

ObjectToStr g_asPlaceableStr [] = 
{
    { Object_Type_None,                 IDS_SPAWN_NOTHING },
    { Object_Type_Placeable,    IDS_SPAWN_PLACEABLE },
    { Object_Type_Table_Ref,    IDS_SPAWN_TABLE },
};

//
// New default type
//

Object_Type g_nNextTreasure = Object_Type_None;
Object_Type g_nNextEncounter = Object_Type_None;
Object_Type g_nNextPlaceable = Object_Type_None;

//-----------------------------------------------------------------------------
//
// @mfunc <c CTableItemDlg> constructor.
//
// @parm CMainWnd * | pMainWnd | Main window
//
// @parm HTREEITEM | hItem | Tree item
//
// @parm CTableObject * | pTable | Parent table
//
// @parm CTableItemObject * | pObject | Object being edited
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CTableItemDlg::CTableItemDlg (CMainWnd *pMainWnd, HTREEITEM hItem,
                              CTableObject *pTable, CTableItemObject *pObject)
{
    m_pMainWnd = pMainWnd;
    m_hItem = hItem;
    m_pTable = pTable;
    m_pObject = pObject;
    m_pGoldObject = NULL;
    m_pItemObject = NULL;
    m_pTableRefObject = NULL;
    m_pEncounterObject = NULL;
    m_pPlaceableObject = NULL;
    m_hWndCurrent = NULL;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CTableItemDlg> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CTableItemDlg::~CTableItemDlg ()
{
    if (m_pGoldObject != NULL && m_pGoldObject != m_pObject)
        delete m_pGoldObject;
    if (m_pItemObject != NULL && m_pItemObject != m_pObject)
        delete m_pItemObject;
    if (m_pTableRefObject != NULL && m_pTableRefObject != m_pObject)
        delete m_pTableRefObject;
    if (m_pEncounterObject != NULL && m_pEncounterObject != m_pObject)
        delete m_pEncounterObject;
    if (m_pPlaceableObject != NULL && m_pPlaceableObject != m_pObject)
        delete m_pPlaceableObject;
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

LRESULT CTableItemDlg::OnInitDialog (UINT uMsg, 
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

    m_statAction = GetDlgItem (IDC_ACTION_STATIC);
    m_cbAction = GetDlgItem (IDC_ACTION);
    m_statChance = GetDlgItem (IDC_CHANCE_STATIC);
    m_editChance = GetDlgItem (IDC_CHANCE_EDIT);
    m_spinChance = GetDlgItem (IDC_CHANCE_SPIN);
    m_cbChance = GetDlgItem (IDC_CHANCE_COMBO);

    //
    // Get the current chance
    //

    int nChance;
    if (m_pObject)
        nChance = m_pObject ->GetChance ();
    else
        nChance = m_pTable ->GetDefaultChance ();

    //
    // Depending on the type, get the controls or delete them
    //

    int nID;
    int nMax;
    switch (m_pTable ->GetTableCommand ())
    {
        case Table_Command_Pick_One:
            str .LoadString (IDS_CHANCE_CHANCE);
            m_statChance .SetWindowText (str);
            m_cbChance .DestroyWindow ();
            str .Format ("%d", nChance);
            m_editChance .SetWindowText (str);
            m_editChance .LimitText (4);
            m_spinChance .SetRange (0, 9999);
            break;

        case Table_Command_Pick_Multiple:
            str .LoadString (IDS_CHANCE_PERCENT);
            m_statChance .SetWindowText (str);
            m_cbChance .DestroyWindow ();
            str .Format ("%d", nChance);
            m_editChance .SetWindowText (str);
            m_editChance .LimitText (3);
            m_spinChance .SetRange (0, 100);
            break;

        case Table_Command_Level_Range:
            str .LoadString (IDS_CHANCE_LEVEL_RANGE);
            m_statChance .SetWindowText (str);
            m_cbChance .DestroyWindow ();
            str .Format ("%d", nChance);
            m_editChance .SetWindowText (str);
            m_editChance .LimitText (1);
            m_spinChance .SetRange (1, 6);
            break;

        case Table_Command_Specific:
            str .LoadString (IDS_CHANCE_SPECIFIC);
            m_statChance .SetWindowText (str);
            m_editChance .DestroyWindow ();
            m_spinChance .DestroyWindow ();
            str .LoadString (IDS_GENERIC);
            m_cbChance .AddString (str);
            str .LoadString (IDS_SPECIFIC);
            m_cbChance .AddString (str);
            m_cbChance .SetCurSel (nChance == 0 ? 0 : 1);
            break;

        case Table_Command_Racial_Type:
            str .LoadString (IDS_CHANCE_RACIAL_TYPE);
            m_statChance .SetWindowText (str);
            m_editChance .DestroyWindow ();
            m_spinChance .DestroyWindow ();
            nID = IDS_RACIAL_TYPE_00;
            nMax = Racial_Type__Last;
            goto do_type_table;

        case Table_Command_Weapon_Focus:
            str .LoadString (IDS_CHANCE_WEAPON_FOCUS);
            m_statChance .SetWindowText (str);
            m_editChance .DestroyWindow ();
            m_spinChance .DestroyWindow ();
            nID = IDS_WEAPON_FOCUS_00;
            nMax = Weapon_Focus__Last;
            goto do_type_table;

        case Table_Command_Armor_Proficiency:
            str .LoadString (IDS_CHANCE_ARMOR_PROFICIENCY);
            m_statChance .SetWindowText (str);
            m_editChance .DestroyWindow ();
            m_spinChance .DestroyWindow ();
            nID = IDS_ARMOR_PROFICIENCY_00;
            nMax = Armor_Proficiency__Last;
            goto do_type_table;

        case Table_Command_Class_Type:
            str .LoadString (IDS_CHANCE_CLASS_TYPE);
            m_statChance .SetWindowText (str);
            m_editChance .DestroyWindow ();
            m_spinChance .DestroyWindow ();
            nID = IDS_CLASS_TYPE_00;
            nMax = Class_Type__Last;
            goto do_type_table;

        case Table_Command_Random_Class_Type:
            str .LoadString (IDS_CHANCE_RANDOM_CLASS_TYPE);
            m_statChance .SetWindowText (str);
            m_editChance .DestroyWindow ();
            m_spinChance .DestroyWindow ();
            nID = IDS_CLASS_TYPE_00;
            nMax = Class_Type__Last;
do_type_table:;
              {
                  for (int i = nID; i <= nID + nMax; i++)
                  {
                      TCHAR szText [128];
                      ::LoadString (_Module .GetResourceInstance (),
                                    i, szText, _countof (szText));
                      LPTSTR p = _tcschr (szText, '\n');
                      if (p)
                          *p = 0;
                      m_cbChance .AddString (szText);
                  }
                  nID = nChance;
                  if (nID < 0 || nID > nMax)
                      nID = 0;
                  m_cbChance .SetCurSel (nID);
              }
              break;

        default:
              _ASSERTE (FALSE);
              break;
    }

    //
    // Poplate the object types
    //

    if (m_pObject)
        m_nObjectType = m_pObject ->GetObjectType ();
    else if (m_pTable ->GetTableType () == Table_Type_Treasure)
        m_nObjectType = g_nNextTreasure;
    else if (m_pTable ->GetTableType () == Table_Type_Encounter)
        m_nObjectType = g_nNextEncounter;
    else if (m_pTable ->GetTableType () == Table_Type_Placeable)
        m_nObjectType = g_nNextPlaceable;
    else
    {
        _ASSERTE (FALSE);
        m_nObjectType = Object_Type_Unknown;
    }
    int nSel = 0;
    int nStrCount = 0;
    ObjectToStr *pStrList = NULL;
    if (m_pTable ->GetTableType () == Table_Type_Treasure)
    {
        nStrCount = _countof (g_asTreasureStr);
        pStrList = g_asTreasureStr;
    }
    else if (m_pTable ->GetTableType () == Table_Type_Encounter)
    {
        nStrCount = _countof (g_asEncounterStr);
        pStrList = g_asEncounterStr;
    }
    else if (m_pTable ->GetTableType () == Table_Type_Placeable)
    {
        nStrCount = _countof (g_asPlaceableStr);
        pStrList = g_asPlaceableStr;
    }
    else
        _ASSERTE (FALSE);
    for (int i = 0; i < nStrCount; i++)
    {
        str .LoadString (pStrList [i] .nID);
        int nIndex = m_cbAction .AddString (str);
        if (pStrList [i] .nObjectType == m_nObjectType)
            nSel = nIndex;
    }
    m_cbAction .SetCurSel (nSel);
    if (m_pObject)
    {
        m_cbAction .EnableWindow (FALSE);
        m_statAction .EnableWindow (FALSE);
    }

    //
    // Show the dialog
    //

    ShowDialog ();
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

LRESULT CTableItemDlg::OnOk (WORD wNotifyCode, 
                             WORD wID, HWND hWndCtl, BOOL& bHandled) 
{

    //
    // Get the chance
    //

    int nChance = 0;
    CString str;
    switch (m_pTable ->GetTableCommand ())
    {
        case Table_Command_Pick_One:
            m_editChance .GetWindowText (str);
            nChance = _ttol (str);
            if (nChance < 0 || nChance > 9999)
            {
                str .Format (IDS_ERR_CHANCE);
                ::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
                return TRUE;
            }
            break;

        case Table_Command_Pick_Multiple:
            m_editChance .GetWindowText (str);
            nChance = _ttol (str);
            if (nChance < 0 || nChance > 100)
            {
                str .Format (IDS_ERR_CHANCE_PERCENT);
                ::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
                return TRUE;
            }
            break;

        case Table_Command_Level_Range:
            m_editChance .GetWindowText (str);
            nChance = _ttol (str);
            if (nChance < 1 || nChance > 6)
            {
                str .Format (IDS_ERR_RANGE);
                ::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
                return TRUE;
            }
            break;

        case Table_Command_Specific:
        case Table_Command_Racial_Type:
        case Table_Command_Weapon_Focus:
        case Table_Command_Armor_Proficiency:
        case Table_Command_Random_Class_Type:
        case Table_Command_Class_Type:
            nChance = m_cbChance .GetCurSel ();
            break;

        default:
            _ASSERTE (FALSE);
            break;
    }

    //
    // Save the other settings
    //

    if (m_hWndCurrent == NULL)
    {
        if (m_pObject == NULL)
        {
            m_pObject = new CNoneObject ();
            m_pObject ->SetTable (m_pTable);
        }
    }
    else if (m_dlgGold .m_hWnd == m_hWndCurrent)
    {
        if (!m_dlgGold .SaveChanges ())
            return TRUE;
        m_pObject = m_pGoldObject;
    }
    else if (m_dlgItem .m_hWnd == m_hWndCurrent)
    {
        if (!m_dlgItem .SaveChanges ())
            return TRUE;
        m_pObject = m_pItemObject;
    }
    else if (m_dlgTableRef .m_hWnd == m_hWndCurrent)
    {
        if (!m_dlgTableRef .SaveChanges ())
            return TRUE;
        m_pObject = m_pTableRefObject;
    }
    else if (m_dlgEncounter .m_hWnd == m_hWndCurrent)
    {
        if (!m_dlgEncounter .SaveChanges ())
            return TRUE;
        m_pObject = m_pEncounterObject;
    }
    else if (m_dlgPlaceable .m_hWnd == m_hWndCurrent)
    {
        if (!m_dlgPlaceable .SaveChanges ())
            return TRUE;
        m_pObject = m_pPlaceableObject;
    }

    //
    // Save the new settings
    //

    m_pObject ->SetChance (nChance);
    if (m_pTable ->GetTableType () == Table_Type_Treasure)
        g_nNextTreasure = m_pObject ->GetObjectType ();
    else if (m_pTable ->GetTableType () == Table_Type_Encounter)
        g_nNextEncounter = m_pObject ->GetObjectType ();
    else if (m_pTable ->GetTableType () == Table_Type_Placeable)
        g_nNextPlaceable = m_pObject ->GetObjectType ();
    else
        _ASSERTE (FALSE);
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

LRESULT CTableItemDlg::OnCancel (WORD wNotifyCode, 
                                 WORD wID, HWND hWndCtl, BOOL& bHandled) 
{
    EndDialog (IDCANCEL);
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle a change in the action
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

LRESULT CTableItemDlg::OnActionSelChange (WORD wNotifyCode, 
                                          WORD wID, HWND hWndCtl, BOOL &bHandled)
{
    CString strCB, str;

    //
    // Get the current text
    //

    int nSel = m_cbAction .GetCurSel ();
    m_cbAction .GetLBText (nSel, strCB);

    //
    // Get the string lists
    //

    int nStrCount = 0;
    ObjectToStr *pStrList = NULL;
    if (m_pTable ->GetTableType () == Table_Type_Treasure)
    {
        nStrCount = _countof (g_asTreasureStr);
        pStrList = g_asTreasureStr;
    }
    else if (m_pTable ->GetTableType () == Table_Type_Encounter)
    {
        nStrCount = _countof (g_asEncounterStr);
        pStrList = g_asEncounterStr;
    }
    else if (m_pTable ->GetTableType () == Table_Type_Placeable)
    {
        nStrCount = _countof (g_asPlaceableStr);
        pStrList = g_asPlaceableStr;
    }
    else
        _ASSERTE (FALSE);

    //
    // Search
    //

    for (int i = 0; i < nStrCount; i++)
    {
        str .LoadString (pStrList [i] .nID);
        if (str == strCB)
        {
            m_nObjectType = pStrList [i] .nObjectType;
            break;
        }
    }
    ShowDialog ();
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Show the current dialog
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CTableItemDlg::ShowDialog ()
{
    HWND hWndNew = NULL;

    //
    // Get the destination rectangle
    //

    HWND hWndInsert = GetDlgItem (IDC_DIALOG_INSERT);
    CRect rc;
    ::GetWindowRect (hWndInsert, &rc);
    ScreenToClient (rc);

    //
    // Based on the new type, get the window handle of the dialog 
    //

    switch (m_nObjectType)
    {

        case Object_Type_None:
            hWndNew = NULL;
            break;

        case Object_Type_Gold:
            if (m_pGoldObject == NULL)
            {
                if (m_pObject != NULL)
                    m_pGoldObject = static_cast <CGoldObject *> (m_pObject);
                else
                {
                    m_pGoldObject = new CGoldObject;
                    m_pGoldObject ->SetTable (m_pTable);
                }
            }
            if (m_dlgGold .m_hWnd == NULL)
            {
                m_dlgGold .Create (m_pGoldObject, m_hWnd, rcDefault);
                m_dlgGold .SetWindowPos (hWndInsert, rc .left, rc .top, 
                                         0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
            }
            hWndNew = m_dlgGold .m_hWnd;
            break;

        case Object_Type_Item:
            if (m_pItemObject == NULL)
            {
                if (m_pObject != NULL)
                    m_pItemObject = static_cast <CItemObject *> (m_pObject);
                else
                {
                    m_pItemObject = new CItemObject;
                    m_pItemObject ->SetTable (m_pTable);
                }
            }
            if (m_dlgItem .m_hWnd == NULL)
            {
                m_dlgItem .Create (m_pItemObject, m_hWnd, rcDefault);
                m_dlgItem .SetWindowPos (hWndInsert, rc .left, rc .top, 
                                         0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
            }
            hWndNew = m_dlgItem .m_hWnd;
            break;

        case Object_Type_Table_Ref:
            if (m_pTableRefObject == NULL)
            {
                if (m_pObject != NULL)
                    m_pTableRefObject = static_cast <CTableRefObject *> (m_pObject);
                else
                {
                    m_pTableRefObject = new CTableRefObject;
                    m_pTableRefObject ->SetTable (m_pTable);
                }
            }
            if (m_dlgTableRef .m_hWnd == NULL)
            {
                m_dlgTableRef .Create (m_pMainWnd, m_pTableRefObject, 
                                       m_hItem, m_hWnd, rcDefault);
                m_dlgTableRef .SetWindowPos (hWndInsert, rc .left, rc .top, 
                                             0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
            }
            hWndNew = m_dlgTableRef .m_hWnd;
            break;

        case Object_Type_Encounter:
            if (m_pEncounterObject == NULL)
            {
                if (m_pObject != NULL)
                    m_pEncounterObject = static_cast <CEncounterObject *> (m_pObject);
                else
                {
                    m_pEncounterObject = new CEncounterObject;
                    m_pEncounterObject ->SetTable (m_pTable);
                }
            }
            if (m_dlgEncounter .m_hWnd == NULL)
            {
                m_dlgEncounter .Create (m_pEncounterObject, m_hWnd, rcDefault);
                m_dlgEncounter .SetWindowPos (hWndInsert, rc .left, rc .top, 
                                              0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
            }
            hWndNew = m_dlgEncounter .m_hWnd;
            break;

        case Object_Type_Placeable:
            if (m_pPlaceableObject == NULL)
            {
                if (m_pObject != NULL)
                    m_pPlaceableObject = static_cast <CPlaceableObject *> (m_pObject);
                else
                {
                    m_pPlaceableObject = new CPlaceableObject;
                    m_pPlaceableObject ->SetTable (m_pTable);
                }
            }
            if (m_dlgPlaceable .m_hWnd == NULL)
            {
                m_dlgPlaceable .Create (m_pPlaceableObject, m_hWnd, rcDefault);
                m_dlgPlaceable .SetWindowPos (hWndInsert, rc .left, rc .top, 
                                              0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
            }
            hWndNew = m_dlgPlaceable .m_hWnd;
            break;
    }

    //
    // Hide the old window
    //

    if (m_hWndCurrent)
        ::ShowWindow (m_hWndCurrent, SW_HIDE);
    m_hWndCurrent = hWndNew;
    if (m_hWndCurrent)
        ::ShowWindow (m_hWndCurrent, SW_SHOW);
    return;
}

/* vim: set sw=4: */
