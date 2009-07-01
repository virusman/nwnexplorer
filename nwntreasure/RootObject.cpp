//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      RootObject.cpp - Core object |
//
// This module contains the core object.
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
#include "RootObject.h"
#include "MainWnd.h"

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
// @mfunc <c CRootObject> constructor.
//
// @parm Table_Type | nTableType | Type of table root
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CRootObject::CRootObject (Table_Type nTableType)
{
    m_nTableType = nTableType;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CRootObject> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CRootObject::~CRootObject ()
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

void CRootObject::GetDescription (CMainWnd *pMainWnd, 
                                  LPTSTR pszName, int cchMaxText)
{
    CString str;

    if (m_nTableType == Table_Type_Treasure)
        str .LoadString (IDS_TREASURE_TABLES);
    else if (m_nTableType == Table_Type_Encounter)
        str .LoadString (IDS_ENCOUNTER_TABLES);
    else if (m_nTableType == Table_Type_Placeable)
        str .LoadString (IDS_PLACEABLE_TABLES);
    else if (m_nTableType == Table_Type_Profile)
        str .LoadString (IDS_PROFILE_TABLES);
    else
        _ASSERTE (FALSE);
    _tcsncpy (pszName, str, cchMaxText);
    pszName [cchMaxText - 1] = 0;
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the object image
//
// @rdesc Image index.
//
//-----------------------------------------------------------------------------

Object_Image CRootObject::GetImage ()
{
    if (m_nTableType == Table_Type_Treasure)
        return Object_Image_Item;
    else if (m_nTableType == Table_Type_Encounter)
        return Object_Image_Encounter;
    else if (m_nTableType == Table_Type_Placeable)
        return Object_Image_Placeable;
    else if (m_nTableType == Table_Type_Profile)
        return Object_Image_Profile;
    else
    {
        _ASSERTE (FALSE);
        return Object_Image_Encounter;
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc See if the object has children
//
// @parm CMainWnd * | pMainWnd | Main window
//
// @rdesc true if the object has children
//
//-----------------------------------------------------------------------------

bool CRootObject::HasChildren (CMainWnd *pMainWnd)
{
    for (int i = 0; i < pMainWnd ->GetTableCount (); i++)
    {
        CTableObject *pTable = pMainWnd ->GetTable (i);
        if (pTable ->GetTableType () == m_nTableType &&
            pTable ->IsRoot ())
            return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
//
// @mfunc Add children
//
// @parm CMainWnd * | pMainWnd | Main window
//
// @parm CTreeViewCtrl & | tv | Tree view
//
// @parm HTREEITEM | hParent | Parent item
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CRootObject::AddChildren (CMainWnd *pMainWnd, 
                               CTreeViewCtrl &tv, HTREEITEM hParent)
{
    for (int i = 0; i < pMainWnd ->GetTableCount (); i++)
    {
        CTableObject *pTable = pMainWnd ->GetTable (i);
        if (pTable ->GetTableType () == m_nTableType &&
            pTable ->IsRoot ())
            pTable ->AddToTV (tv, hParent, TVI_LAST);
    }
    SortTV (pMainWnd, tv, hParent);
}


/* vim: set sw=4: */
