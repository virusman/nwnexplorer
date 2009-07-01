//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      TBCustomizeDlg.cpp - Toolbar customization extra dialog |
//
// This module contains the definition of the toolbar 
//              customization extra dialog.
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
// $History: TBCustomizeDlg.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"
#include "TBCustomizeDlg.h"

//-----------------------------------------------------------------------------
//
// @mfunc <c CTBCustomizeDlg> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CTBCustomizeDlg::CTBCustomizeDlg ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CTBCustomizeDlg> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CTBCustomizeDlg::~CTBCustomizeDlg()
{
    m_nTextStyle = TBTS_NoText;
    m_nIconStyle = TBIS_SmallIcons;
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

LRESULT CTBCustomizeDlg::OnInitDialog (UINT uMsg, 
                                       WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{
    CString str;

    //
    // Get the controls
    //

    m_cbTextStyle = GetDlgItem (IDC_TBCUSTOMIZE_TEXT_OPTIONS);
    m_cbIconStyle = GetDlgItem (IDC_TBCUSTOMIZE_ICON_OPTIONS);

    //
    // Add the text style strings
    //

    str .LoadString (IDS_TBCUSTOMIZE_NO_LABELS);
    m_cbTextStyle .AddString (str);
    str .LoadString (IDS_TBCUSTOMIZE_BOTTOM_LABELS);
    m_cbTextStyle .AddString (str);
    str .LoadString (IDS_TBCUSTOMIZE_RIGHT_LABELS);
    m_cbTextStyle .AddString (str);

    //
    // Add the icon style strings
    //

    str .LoadString (IDS_TBCUSTOMIZE_SMALL_ICONS);
    m_cbIconStyle .AddString (str);
    str .LoadString (IDS_TBCUSTOMIZE_LARGE_ICONS);
    m_cbIconStyle .AddString (str);

    //
    // Set the current styles
    //

    SetTextStyle (m_nTextStyle);
    SetIconStyle (m_nIconStyle);
    return FALSE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Set the current text style
//
// @parm TB_TextStyle | nTextStyle | New text style
//
// @rdesc None
//
//-----------------------------------------------------------------------------

void CTBCustomizeDlg::SetTextStyle (TB_TextStyle nTextStyle)
{

    //
    // Validate range
    //

    if (nTextStyle < TBTS_NoText || nTextStyle > TBTS_RightText)
        nTextStyle = TBTS_NoText;

    //
    // Save the setting
    //

    m_nTextStyle = nTextStyle;
    CString str;
    switch (m_nTextStyle)
    {
        case TBTS_NoText:
            str .LoadString (IDS_TBCUSTOMIZE_NO_LABELS);
            break;
        case TBTS_BottomText:
            str .LoadString (IDS_TBCUSTOMIZE_BOTTOM_LABELS);
            break;
        case TBTS_RightText:
            str .LoadString (IDS_TBCUSTOMIZE_RIGHT_LABELS);
            break;
        default:
            break;
    }
    if (m_hWnd != NULL && m_cbTextStyle .m_hWnd != NULL)
        m_cbTextStyle .SelectString (-1, str);
}

//-----------------------------------------------------------------------------
//
// @mfunc Set the current icon style
//
// @parm TB_IconStyle | nIconStyle | New icon style
//
// @rdesc None
//
//-----------------------------------------------------------------------------

void CTBCustomizeDlg::SetIconStyle (TB_IconStyle nIconStyle)
{

    //
    // Validate range
    //

    if (nIconStyle < TBIS_SmallIcons || nIconStyle > TBIS_LargeIcons)
        nIconStyle = TBIS_SmallIcons;

    //
    // Save the setting
    //

    m_nIconStyle = nIconStyle;
    CString str;
    switch (m_nIconStyle)
    {
        case TBIS_SmallIcons:
            str .LoadString (IDS_TBCUSTOMIZE_SMALL_ICONS);
            break;
        case TBIS_LargeIcons:
            str .LoadString (IDS_TBCUSTOMIZE_LARGE_ICONS);
            break;
        default:
            break;
    }
    if (m_hWnd != NULL && m_cbIconStyle .m_hWnd != NULL)
        m_cbIconStyle .SelectString (-1, str);
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle a change in the text style
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

LRESULT CTBCustomizeDlg::OnTextSelChange (WORD wNotifyCode, 
                                          WORD wID, HWND hWndCtl, BOOL &bHandled)
{
    CString strCB, str;

    int nSel = m_cbTextStyle .GetCurSel ();
    m_cbTextStyle .GetLBText (nSel, strCB);
    str .LoadString (IDS_TBCUSTOMIZE_NO_LABELS);
    if (strCB == str)
    {
        m_nTextStyle = TBTS_NoText;
        return TRUE;
    }
    str .LoadString (IDS_TBCUSTOMIZE_BOTTOM_LABELS);
    if (strCB == str)
    {
        m_nTextStyle = TBTS_BottomText;
        return TRUE;
    }
    str .LoadString (IDS_TBCUSTOMIZE_RIGHT_LABELS);
    if (strCB == str)
    {
        m_nTextStyle = TBTS_RightText;
        return TRUE;
    }
    else
    {
        return TRUE;
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle a change in the icon style
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

LRESULT CTBCustomizeDlg::OnIconSelChange (WORD wNotifyCode, 
                                          WORD wID, HWND hWndCtl, BOOL &bHandled)
{
    CString strCB, str;

    int nSel = m_cbIconStyle .GetCurSel ();
    m_cbIconStyle .GetLBText (nSel, strCB);
    str .LoadString (IDS_TBCUSTOMIZE_SMALL_ICONS);
    if (strCB == str)
    {
        m_nIconStyle = TBIS_SmallIcons;
        return TRUE;
    }
    str .LoadString (IDS_TBCUSTOMIZE_LARGE_ICONS);
    if (strCB == str)
    {
        m_nIconStyle = TBIS_LargeIcons;
        return TRUE;
    }
    else
    {
        return TRUE;
    }
}


/* vim: set sw=4: */
