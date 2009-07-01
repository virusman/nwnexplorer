//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ModelPosDlg.cpp - Model position dialog |
//
// This module contains the definition model position dialog.
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
// $History: ModelPosDlg.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"
#include "ModelPosDlg.h"

//-----------------------------------------------------------------------------
//
// @mfunc <c CModelPosDlg> constructor.
//
// @parm ModelContext * | pModelContext | Model context
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CModelPosDlg::CModelPosDlg (ModelContext *pModelContext)
{
    m_pModelContext = pModelContext;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CModelPosDlg> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CModelPosDlg::~CModelPosDlg()
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

LRESULT CModelPosDlg::OnInitDialog (UINT uMsg, 
                                    WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{

    m_btnPitchForward = GetDlgItem (IDC_MODEL_PITCH_FORWARD);
    m_btnPitchBackward = GetDlgItem (IDC_MODEL_PITCH_BACKWARD);
    m_btnYawLeft = GetDlgItem (IDC_MODEL_YAW_LEFT);
    m_btnYawRight = GetDlgItem (IDC_MODEL_YAW_RIGHT);
    m_btnRollLeft = GetDlgItem (IDC_MODEL_ROLL_LEFT);
    m_btnRollRight = GetDlgItem (IDC_MODEL_ROLL_RIGHT);
    m_btnZoomIn = GetDlgItem (IDC_MODEL_ZOOM_IN);
    m_btnZoomOut = GetDlgItem (IDC_MODEL_ZOOM_OUT);
    m_btnMoveLeft = GetDlgItem (IDC_MODEL_MOVE_LEFT);
    m_btnMoveRight = GetDlgItem (IDC_MODEL_MOVE_RIGHT);
    m_btnMoveUp = GetDlgItem (IDC_MODEL_MOVE_UP);
    m_btnMoveDown = GetDlgItem (IDC_MODEL_MOVE_DOWN);

    //
    // Get the button icons
    //

    m_il .Create (32, 32, ILC_COLOR16 | ILC_MASK, 16, 16);
    m_nPitchForward = m_il .AddIcon (
        ::LoadIcon (_Module .GetResourceInstance (),
                    MAKEINTRESOURCE (IDI_PITCH_FORWARD)));
    m_nPitchBackward = m_il .AddIcon (
        ::LoadIcon (_Module .GetResourceInstance (),
                    MAKEINTRESOURCE (IDI_PITCH_BACKWARD)));
    m_nYawLeft = m_il .AddIcon (
        ::LoadIcon (_Module .GetResourceInstance (),
                    MAKEINTRESOURCE (IDI_YAW_LEFT)));
    m_nYawRight = m_il .AddIcon (
        ::LoadIcon (_Module .GetResourceInstance (),
                    MAKEINTRESOURCE (IDI_YAW_RIGHT)));
    m_nRollLeft = m_il .AddIcon (
        ::LoadIcon (_Module .GetResourceInstance (),
                    MAKEINTRESOURCE (IDI_ROLL_LEFT)));
    m_nRollRight = m_il .AddIcon (
        ::LoadIcon (_Module .GetResourceInstance (),
                    MAKEINTRESOURCE (IDI_ROLL_RIGHT)));
    m_nZoomIn = m_il .AddIcon (
        ::LoadIcon (_Module .GetResourceInstance (),
                    MAKEINTRESOURCE (IDI_ZOOM_IN)));
    m_nZoomOut = m_il .AddIcon (
        ::LoadIcon (_Module .GetResourceInstance (),
                    MAKEINTRESOURCE (IDI_ZOOM_OUT)));
    m_nMoveLeft = m_il .AddIcon (
        ::LoadIcon (_Module .GetResourceInstance (),
                    MAKEINTRESOURCE (IDI_MOVE_LEFT)));
    m_nMoveRight = m_il .AddIcon (
        ::LoadIcon (_Module .GetResourceInstance (),
                    MAKEINTRESOURCE (IDI_MOVE_RIGHT)));
    m_nMoveUp = m_il .AddIcon (
        ::LoadIcon (_Module .GetResourceInstance (),
                    MAKEINTRESOURCE (IDI_MOVE_UP)));
    m_nMoveDown = m_il .AddIcon (
        ::LoadIcon (_Module .GetResourceInstance (),
                    MAKEINTRESOURCE (IDI_MOVE_DOWN)));

    //
    // Get the theme data
    //

    OpenThemeData (L"Button");

    //
    // Start the rotation timer
    //

    SetTimer (1, 50, NULL);
    return FALSE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle a draw item request
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

LRESULT CModelPosDlg::OnDrawItem (UINT uMsg, WPARAM wParam, 
                                  LPARAM lParam, BOOL &bHandled) 
{
    UNUSED_ALWAYS (bHandled);
    UNUSED_ALWAYS (wParam);
    UNUSED_ALWAYS (uMsg);

    LPDRAWITEMSTRUCT lpItem = (LPDRAWITEMSTRUCT) lParam;
    CDC dc (lpItem ->hDC);

    //
    // Get data about the request
    //

    UINT uState = lpItem ->itemState;
    CRect rcDraw = lpItem ->rcItem;

    //
    // If we have a theme
    //

    if (m_hTheme != NULL)
    {

        //
        // Draw the outer edge
        //

        UINT uFrameState = 0;
        if ((uState & ODS_SELECTED) != 0)
            uFrameState |= PBS_PRESSED;
        if ((uState & ODS_DISABLED) != 0)
            uFrameState |= PBS_DISABLED;
        if ((uState & ODS_HOTLIGHT) != 0)
            uFrameState |= PBS_HOT;
        DrawThemeBackground (dc, BP_PUSHBUTTON, 
                             uFrameState, &rcDraw, NULL);
        GetThemeBackgroundContentRect (dc, BP_PUSHBUTTON, 
                                       uFrameState, &rcDraw, &rcDraw);
    }

    //
    // Otherwise, we are old school
    //

    else
    {
        //
        // Draw the outer edge
        //

        UINT uFrameState = DFCS_BUTTONPUSH | DFCS_ADJUSTRECT;
        if ((uState & ODS_SELECTED) != 0)
            uFrameState |= DFCS_PUSHED;
        if ((uState & ODS_DISABLED) != 0)
            uFrameState |= DFCS_INACTIVE;
        dc .DrawFrameControl (&rcDraw, DFC_BUTTON, uFrameState);

        //
        // Adjust the position if we are selected (gives a 3d look)
        //

        if ((uState & ODS_SELECTED) != 0)
            rcDraw .OffsetRect (1, 1);
    }

    //
    // Draw focus
    //

    if ((uState & ODS_FOCUS) != 0) 
    {
        CRect rcFocus (rcDraw.left, rcDraw.top, 
                       rcDraw.right - 1, rcDraw.bottom);
        dc .DrawFocusRect(&rcFocus);
    }
    rcDraw .InflateRect (
        - ::GetSystemMetrics(SM_CXEDGE),
        - ::GetSystemMetrics(SM_CYEDGE));

    //
    // Center the icon
    //

    if (rcDraw .Width () > 32)
        rcDraw .left += (rcDraw .Width () - 32) / 2;
    rcDraw .right = rcDraw .left + 32;
    if (rcDraw .Height () > 32)
        rcDraw .top += (rcDraw .Height () - 32) / 2;
    rcDraw .bottom = rcDraw .top + 32;

    //
    // Get the index
    //

    int n;
    if (lpItem ->CtlID == IDC_MODEL_PITCH_FORWARD)
        n = m_nPitchForward;
    else if (lpItem ->CtlID == IDC_MODEL_PITCH_BACKWARD)
        n = m_nPitchBackward;
    else if (lpItem ->CtlID == IDC_MODEL_YAW_LEFT)
        n = m_nYawLeft;
    else if (lpItem ->CtlID == IDC_MODEL_YAW_RIGHT)
        n = m_nYawRight;
    else if (lpItem ->CtlID == IDC_MODEL_ROLL_LEFT)
        n = m_nRollLeft;
    else if (lpItem ->CtlID == IDC_MODEL_ROLL_RIGHT)
        n = m_nRollRight;
    else if (lpItem ->CtlID == IDC_MODEL_ZOOM_IN)
        n = m_nZoomIn;
    else if (lpItem ->CtlID == IDC_MODEL_ZOOM_OUT)
        n = m_nZoomOut;
    else if (lpItem ->CtlID == IDC_MODEL_MOVE_LEFT)
        n = m_nMoveLeft;
    else if (lpItem ->CtlID == IDC_MODEL_MOVE_RIGHT)
        n = m_nMoveRight;
    else if (lpItem ->CtlID == IDC_MODEL_MOVE_UP)
        n = m_nMoveUp;
    else if (lpItem ->CtlID == IDC_MODEL_MOVE_DOWN)
        n = m_nMoveDown;
    else
        n = 0;
    m_il .Draw (dc, n, rcDraw .TopLeft (), ILD_TRANSPARENT);
    return 1;
}


/* vim: set sw=4: */
