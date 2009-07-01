//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      PlaceableDlg.cpp - None treasure dialog |
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
// $History: PlaceableDlg.h $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"
#include "PlaceableDlg.h"
#include "PlaceableObject.h"
#include "ItemBrowserDlg.h"

//
// Global variables
//

extern TCHAR g_szAppName [];

//-----------------------------------------------------------------------------
//
// @mfunc <c CPlaceableDlg> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CPlaceableDlg::CPlaceableDlg ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CPlaceableDlg> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CPlaceableDlg::~CPlaceableDlg ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc Create the dialog
//
// @param CPlaceableObject * | pObject | Object being edited
//
// @parm HWND | hWndParent | Parent object
//
// @parm RECT & | rc | Creation rectangle
//
// @rdesc Window handle.
//
//-----------------------------------------------------------------------------

HWND CPlaceableDlg::Create (CPlaceableObject *pObject, HWND hWndParent, RECT &rc)
{
    m_pObject = pObject;
    return CDialogImpl <CPlaceableDlg>::Create (hWndParent, rc);
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

LRESULT CPlaceableDlg::OnInitDialog (UINT uMsg, 
                                     WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{
    //
    // Get the controls
    //

    m_editBlueprint = GetDlgItem (IDC_ITEM_BLUEPRINT);

    //
    // Initialize
    //

    m_editBlueprint .LimitText (64);
    m_editBlueprint .SetWindowText (m_pObject ->GetName ());
    return FALSE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Save the changes
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

bool CPlaceableDlg::SaveChanges () 
{
    CString str;

    //
    // Get the blueprint
    //

    CString strBlueprint;
    m_editBlueprint .GetWindowText (strBlueprint);
    if (!IsValidBlueprintName (strBlueprint))
    {
        str .Format (IDS_ERR_INVALID_BLUEPRINT_NAME);
        ::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
        return false;
    }

    //
    // Save the new settings
    //

    m_pObject ->SetName (strBlueprint);
    return true;
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

LRESULT CPlaceableDlg::OnBrowseItem (WORD wNotifyCode, 
                                     WORD wID, HWND hWndCtl, BOOL &bHandled) 
{
    CItemBrowserDlg dlg (CItemBrowserDlg::Browse_What_Placeables);
    m_editBlueprint .GetWindowText (dlg .m_strSelection);
    if (dlg .DoModal () == IDOK)
        m_editBlueprint .SetWindowText (dlg .m_strSelection);
    return TRUE;
}


/* vim: set sw=4: */
