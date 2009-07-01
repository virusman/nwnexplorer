//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      TableRefDlg.cpp - None treasure dialog |
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
// $History: TableRefDlg.h $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"
#include "TableRefDlg.h"
#include "TableRefObject.h"
#include "MainWnd.h"

//
// Global variables
//

extern TCHAR g_szAppName [];

//-----------------------------------------------------------------------------
//
// @mfunc <c CTableRefDlg> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CTableRefDlg::CTableRefDlg ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CTableRefDlg> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CTableRefDlg::~CTableRefDlg ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc Create the dialog
//
// @parm CMainWnd * | pMainWnd | Main window
//
// @param CTableRefObject * | pObject | Object being edited
//
// @parm HTREEITEM | hItem | Item in the tree view
//
// @parm HWND | hWndParent | Parent object
//
// @parm RECT & | rc | Creation rectangle
//
// @rdesc Window handle.
//
//-----------------------------------------------------------------------------

HWND CTableRefDlg::Create (CMainWnd *pMainWnd, CTableRefObject *pObject, 
                           HTREEITEM hItem, HWND hWndParent, RECT &rc)
{
    m_pMainWnd = pMainWnd;
    m_hItem = hItem;
    m_pObject = pObject;
    return CDialogImpl <CTableRefDlg>::Create (hWndParent, rc);
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

LRESULT CTableRefDlg::OnInitDialog (UINT uMsg, 
                                    WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{
    CString str;

    //
    // Get my table type
    //

    Table_Type nTableType = m_pObject ->GetTable () ->GetTableType ();

    //
    // Get the controls
    //

    m_cbTableName = GetDlgItem (IDC_TABLE_NAME);
    m_editMin = GetDlgItem (IDC_MIN);
    m_spinMin = GetDlgItem (IDC_MIN_SPIN);
    m_editMax = GetDlgItem (IDC_MAX);
    m_spinMax = GetDlgItem (IDC_MAX_SPIN);
    m_cbModifier = GetDlgItem (IDC_MODIFIER);
    m_cbSpecific = GetDlgItem (IDC_SPECIFIC);

    //
    // Initialize
    //

    m_cbTableName .LimitText (64);
    m_editMin .LimitText (4);
    m_editMax .LimitText (4);
    m_spinMin .SetRange (1, 9999);
    m_spinMax .SetRange (1, 9999);

    //
    // Initialize the combos
    //

    str .LoadString (IDS_INHERIT);
    m_cbModifier .AddString (str);
    m_cbSpecific .AddString (str);
    for (int i = 0; i < 21; i++)
    {
        str .Format ("%d", i);
        m_cbModifier .AddString (str);
    }
    str .LoadString (IDS_GENERIC);
    m_cbSpecific .AddString (str);
    str .LoadString (IDS_SPECIFIC);
    m_cbSpecific .AddString (str);

    //
    // Initialize the table ref falgs
    //

    for (int i = 0; i < m_pMainWnd ->GetTableCount (); i++)
    {
        CTableObject *pTable = m_pMainWnd ->GetTable (i);
        pTable ->m_nTableRefCount = 1;
    }
    m_pObject ->GetTable () ->RemoveTableRecusions (m_pMainWnd);

    //
    // Initialize the table names
    //

    for (int i = 0; i < m_pMainWnd ->GetTableCount (); i++)
    {
        CTableObject *pTable = m_pMainWnd ->GetTable (i);
        if (pTable ->GetTableType () == nTableType && 
            pTable ->m_nTableRefCount != 0)
            m_cbTableName .AddString (pTable ->GetName ());
    }

    //
    // Populate Min/Max
    //

    if (m_pObject ->GetTable () ->GetTableType () == Table_Type_Treasure ||
        m_pObject ->GetTable () ->GetTableType () == Table_Type_Encounter)
    {
        int nMin = m_pObject ->GetCount ();
        if (nMin < 1)
            nMin = 1;
        if (m_pObject ->GetRange () >= 2)
        {
            str .Format ("%d", nMin + m_pObject ->GetRange () - 1);
            m_editMax .SetWindowText (str);
            str .Format ("%d", nMin);
            m_editMin .SetWindowText (str);
        }
        else
        {
            str .Format ("%d", nMin);
            m_editMax .SetWindowText (str);
            m_editMin .SetWindowText (str);
        }
    }
    else if (m_pObject ->GetTable () ->GetTableType () == Table_Type_Encounter ||
             m_pObject ->GetTable () ->GetTableType () == Table_Type_Placeable)
    {
        ::EnableWindow (GetDlgItem (IDC_MIN_STATIC), FALSE);
        ::EnableWindow (GetDlgItem (IDC_MAX_STATIC), FALSE);
        m_editMin .EnableWindow (FALSE);
        m_editMax .EnableWindow (FALSE);
        m_spinMin .EnableWindow (FALSE);
        m_spinMax .EnableWindow (FALSE);
    }
    else
        _ASSERTE (FALSE);

    //
    // Populate modifier/specific
    //

    if (m_pObject ->GetTable () ->GetTableType () == Table_Type_Treasure)
    {
        int nModifier = m_pObject ->GetModifier ();
        if (nModifier < -1)
            nModifier = -1;
        else if (nModifier > 20)
            nModifier = 20;
        m_cbModifier .SetCurSel (nModifier + 1);
        int nSpecific = m_pObject ->GetSpecific ();
        if (nSpecific < -1)
            nSpecific = -1;
        else if (nSpecific > 1)
            nSpecific = 1;
        m_cbSpecific .SetCurSel (nSpecific + 1);
    }
    else if (m_pObject ->GetTable () ->GetTableType () == Table_Type_Encounter ||
             m_pObject ->GetTable () ->GetTableType () == Table_Type_Placeable)
    {
        ::EnableWindow (GetDlgItem (IDC_MODIFIER_STATIC), FALSE);
        ::EnableWindow (GetDlgItem (IDC_SPECIFIC_STATIC), FALSE);
        m_cbSpecific .EnableWindow (FALSE);
        m_cbModifier .EnableWindow (FALSE);
    }
    else
        _ASSERTE (FALSE);

    //
    // Set the table name
    //

    m_cbTableName .SetWindowText (m_pObject ->GetName ());
    return FALSE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Save changes
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

bool CTableRefDlg::SaveChanges () 
{
    CString str;
    int nMin = 1;
    int nMax = 1;

    if (m_pObject ->GetTable () ->GetTableType () == Table_Type_Treasure ||
        m_pObject ->GetTable () ->GetTableType () == Table_Type_Encounter)
    {
        //
        // Get the min
        //

        m_editMin .GetWindowText (str);
        nMin = _ttol (str);
        if (nMin < 1 || nMin > 9999)
        {
            str .Format (IDS_ERR_MIN);
            ::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
            return false;
        }

        //
        // Get the max
        //

        m_editMax .GetWindowText (str);
        nMax = _ttol (str);
        if (nMax < 1 || nMax > 9999)
        {
            str .Format (IDS_ERR_MAX);
            ::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
            return false;
        }

        //
        // Validate the range
        //

        if (nMax < nMin)
        {
            //str .Format (IDS_ERR_MIN_MAX);
            //::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
            //return false;
            nMax = nMin;
        }
    }
    else if (m_pObject ->GetTable () ->GetTableType () == Table_Type_Placeable)
    {
    }
    else
        _ASSERTE (FALSE);

    //
    // Get the name
    //

    CString strName;
    m_cbTableName .GetWindowText (strName);
    if (!IsValidTableName (strName))
    {
        str .Format (IDS_ERR_INVALID_TABLE_NAME);
        ::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
        return false;
    }

    //
    // Test for recusions
    //

    CTableObject *pNewTable = m_pMainWnd ->FindTable (
        strName, m_pObject ->GetTable () ->GetTableType ());
    if (pNewTable && pNewTable ->m_nTableRefCount == 0)
    {
        str .Format (IDS_ERR_TABLE_RECUSIONS);
        ::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
        return false;
    }

    //
    // Handle name change
    //

    if (!m_pMainWnd ->HandleTableRefNameChange (m_pObject, strName, m_hItem))
        return false;

    //
    // Save the new settings
    //

    m_pObject ->SetCount (nMin);
    m_pObject ->SetRange (nMax - nMin + 1);
    if (m_pObject ->GetTable () ->GetTableType () == Table_Type_Treasure)
    {
        m_pObject ->SetModifier (m_cbModifier .GetCurSel () - 1);
        m_pObject ->SetSpecific (m_cbSpecific .GetCurSel () - 1);
    }
    else if (m_pObject ->GetTable () ->GetTableType () == Table_Type_Encounter ||
             m_pObject ->GetTable () ->GetTableType () == Table_Type_Placeable)
    {
        m_pObject ->SetModifier (-1);
        m_pObject ->SetSpecific (-1);
    }
    else
        _ASSERTE (FALSE);
    return true;
}

/* vim: set sw=4: */
