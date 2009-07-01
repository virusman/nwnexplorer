//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      TableItemObject.cpp - Table item object |
//
// This module contains the table item object.
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
// $History: Cnf.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"
#include "TableItemObject.h"
#include "TableObject.h"

//
// Debug NEW
//

#if defined (_DEBUG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//-----------------------------------------------------------------------------
//
// @mfunc <c CTableItemObject> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CTableItemObject::CTableItemObject ()
{
    m_nChance = 0;
    m_pTable = NULL;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CTableItemObject> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CTableItemObject::~CTableItemObject ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the name of the object
//
// @parm CMainWnd * | pMainWnd | Main window
// 
// @parm LPTSTR | pszName | Name of the object
//
// @parm int | cchMaxText | Size of the buffer
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CTableItemObject::GetDescription (CMainWnd *pMainWnd,
                                       LPTSTR pszName, int cchMaxText)
{

    //
    // Get the full name
    //

    CString str = GetFullName (pMainWnd);

    //
    // Get the text for the chance
    //

    CString strPrefix;
    if (m_pTable ->GetTableCommand () == Table_Command_Pick_One)
    {
        strPrefix .Format (IDS_CHANCE, m_nChance);
    }
    else if (m_pTable ->GetTableCommand () == Table_Command_Pick_Multiple)
    {
        strPrefix .Format (IDS_CHANCE_PERCENT, m_nChance);
    }
    else if (m_pTable ->GetTableCommand () == Table_Command_Level_Range)
    {
        strPrefix .Format (IDS_LEVEL_RANGE, m_nChance);
    }
    else if (m_pTable ->GetTableCommand () == Table_Command_Specific)
    {
        strPrefix .LoadString (m_nChance == 0 ? IDS_GENERIC : IDS_SPECIFIC);
    }
    else if (m_pTable ->GetTableCommand () == Table_Command_Racial_Type)
    {
        TCHAR szText [256];
        int nIndex = m_nChance;
        if (nIndex < -1 || nIndex > Racial_Type__Last)
            nIndex = Racial_Type_Error;
        ::LoadString (_Module .GetResourceInstance (),
                      IDS_RACIAL_TYPE_00 + nIndex, szText, _countof (szText));
        TCHAR *p = _tcschr (szText, '\n');
        if (p) 
            *p = 0;
        strPrefix .Format (IDS_RACE, szText);
    }
    else if (m_pTable ->GetTableCommand () == Table_Command_Random_Class_Type)
    {
        TCHAR szText [256];
        int nIndex = m_nChance;
        if (nIndex < -1 || nIndex > Class_Type__Last)
            nIndex = Class_Type_Error;
        ::LoadString (_Module .GetResourceInstance (),
                      IDS_CLASS_TYPE_00 + m_nChance, szText, _countof (szText));
        TCHAR *p = _tcschr (szText, '\n');
        if (p) 
            *p = 0;
        strPrefix .Format (IDS_CLASS, szText);
    }
    else if (m_pTable ->GetTableCommand () == Table_Command_Class_Type)
    {
        TCHAR szText [256];
        int nIndex = m_nChance;
        if (nIndex < -1 || nIndex > Class_Type__Last)
            nIndex = Class_Type_Error;
        ::LoadString (_Module .GetResourceInstance (),
                      IDS_CLASS_TYPE_00 + m_nChance, szText, _countof (szText));
        TCHAR *p = _tcschr (szText, '\n');
        if (p) 
            *p = 0;
        strPrefix .Format (IDS_CLASS, szText);
    }
    else if (m_pTable ->GetTableCommand () == Table_Command_Weapon_Focus)
    {
        TCHAR szText [256];
        int nIndex = m_nChance;
        if (nIndex < -1 || nIndex > Weapon_Focus__Last)
            nIndex = Weapon_Focus_Error;
        ::LoadString (_Module .GetResourceInstance (),
                      IDS_WEAPON_FOCUS_00 + m_nChance, szText, _countof (szText));
        TCHAR *p = _tcschr (szText, '\n');
        if (p) 
            *p = 0;
        strPrefix .Format (IDS_WEAPON_FOCUS, szText);
    }
    else if (m_pTable ->GetTableCommand () == Table_Command_Armor_Proficiency)
    {
        TCHAR szText [256];
        int nIndex = m_nChance;
        if (nIndex < -1 || nIndex > Armor_Proficiency__Last)
            nIndex = Armor_Proficiency_Error;
        ::LoadString (_Module .GetResourceInstance (),
                      IDS_ARMOR_PROFICIENCY_00 + m_nChance, szText, _countof (szText));
        TCHAR *p = _tcschr (szText, '\n');
        if (p) 
            *p = 0;
        strPrefix .Format (IDS_ARMOR_PROFICIENCY, szText);
    }

    //
    // Make the full name
    //

    CString strFull;
    if (strPrefix .IsEmpty ())
        strFull = str;
    else 
        strFull = strPrefix + " - " + str;
    _tcsncpy (pszName, strFull, cchMaxText);
    pszName [cchMaxText - 1] = 0;
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Delete this object
//
// @parm CMainWnd * | pMainWnd | Main window
// 
// @parm CTreeViewCtrl & | tv | Tree view
//
// @parm HTREEITEM | hItem | Handle of the item being deleted
//
// @parm bool | fPrompt | If true, prompt.  Otherwise, take defaults
//
// @rdesc TRUE if the item was deleted
//
//-----------------------------------------------------------------------------

bool CTableItemObject::Delete (CMainWnd *pMainWnd, 
                               CTreeViewCtrl &tv, HTREEITEM hItem, bool fPrompt)
{
    if (m_pTable) 
        m_pTable ->RemoveItem (this);
    DeleteFromTV (pMainWnd, tv, NULL);
    delete this;
    return true;
}

//-----------------------------------------------------------------------------
//
// @mfunc Set the default chance based on the table command
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CTableItemObject::SetDefaultChance ()
{
    if (m_pTable == NULL)
        return;
    m_nChance = m_pTable ->GetDefaultChance ();
}


/* vim: set sw=4: */
