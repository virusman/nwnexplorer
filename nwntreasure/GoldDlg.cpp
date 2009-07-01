//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      GoldDlg.cpp - Gold treasure dialog |
//
// This module contains the definition gold treasure dialog
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
// $History: GoldDlg.h $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"
#include "GoldDlg.h"
#include "GoldObject.h"

//
// Global variables
//

extern TCHAR g_szAppName [];

//-----------------------------------------------------------------------------
//
// @mfunc <c CGoldDlg> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CGoldDlg::CGoldDlg ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CGoldDlg> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CGoldDlg::~CGoldDlg ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc Create the dialog
//
// @param CGoldObject * | pObject | Object being edited
//
// @parm HWND | hWndParent | Parent object
//
// @parm RECT & | rc | Creation rectangle
//
// @rdesc Window handle.
//
//-----------------------------------------------------------------------------

HWND CGoldDlg::Create (CGoldObject *pObject, HWND hWndParent, RECT &rc)
{
    m_pObject = pObject;
    return CDialogImpl <CGoldDlg>::Create (hWndParent, rc);
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

LRESULT CGoldDlg::OnInitDialog (UINT uMsg, 
                                WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{

    //
    // Get the controls
    //

    m_editDieCount = GetDlgItem (IDC_DIE_COUNT);
    m_spinDieCount = GetDlgItem (IDC_DIE_COUNT_SPIN);
    m_editDie = GetDlgItem (IDC_DIE);
    m_spinDie = GetDlgItem (IDC_DIE_SPIN);
    m_editMultiplier = GetDlgItem (IDC_MULTIPLIER);

    //
    // Initialize the controls
    //

    m_editDieCount .LimitText (4);
    m_editDie .LimitText (4);
    m_editMultiplier .LimitText (8);
    m_spinDieCount .SetRange (1, 9999);
    m_spinDie .SetRange (2, 9999);

    //
    // Populate the initial values
    //

    CString str;
    str .Format ("%d", m_pObject ->GetCount ());
    m_editDieCount .SetWindowText (str);
    str .Format ("%d", m_pObject ->GetDie ());
    m_editDie .SetWindowText (str);
    str .Format ("%.2f", m_pObject ->GetMultiplier ());
    m_editMultiplier .SetWindowText (str);
    return FALSE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Save changes
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

bool CGoldDlg::SaveChanges ()
{
    CString str;

    //
    // Get the count
    //

    m_editDieCount .GetWindowText (str);
    int nCount = _ttol (str);
    if (nCount < 1 || nCount > 9999)
    {
        str .Format (IDS_ERR_DIE_COUNT);
        ::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
        return false;
    }

    //
    // Get the die
    //

    m_editDie .GetWindowText (str);
    int nDie = _ttol (str);
    if (nDie < 2 || nCount > 9999)
    {
        str .Format (IDS_ERR_DIE);
        ::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
        return false;
    }

    //
    // Get the multiplier
    //

    m_editMultiplier .GetWindowText (str);
    float fMultiplier = _tstof (str);
    if (fMultiplier <= 0.0f)
    {
        str .Format (IDS_ERR_MULTIPLIER);
        ::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
        return false;
    }

    //
    // Save the new settings
    //

    m_pObject ->SetCount (nCount);
    m_pObject ->SetDie (nDie);
    m_pObject ->SetMultiplier (fMultiplier);
    return true;
}


/* vim: set sw=4: */
