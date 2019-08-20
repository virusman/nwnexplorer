//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnOptionsPage.cpp - Nwn options page |
//
// This module contains the definition of the Nwn options page
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
// $History: NwnOptionsPage.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"
#include "NwnOptionsPage.h"
#include "nwnexplorer.h"

#if defined (_DEBUG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern TCHAR g_szAppName [];

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnOptionsPage> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnOptionsPage::CNwnOptionsPage ()
{

    //
    // Set the icon and enable help
    //

#if !defined (_WIN32_WCE)
    //  m_psp .dwFlags |= PSP_HASHELP | PSP_USEICONID;
    //  m_psp .pszIcon = MAKEINTRESOURCE (IDR_IO_MONITOR);
#endif
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnOptionsPage> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnOptionsPage::~CNwnOptionsPage ()
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

LRESULT CNwnOptionsPage::OnInitDialog (UINT uMsg, 
                                       WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{

    //
    // Center the sheet
    //

    CenterPropertySheet (GetPropertySheet ());

    //
    // Subclass the controls
    //

    m_btnLoadNwnFrom                = GetDlgItem (IDC_LOAD_NWN_FROM);
    m_btnNwnDirectory               = GetDlgItem (IDC_NWN_DIRECTORY_BROWSE);
    m_btnResourceOpenNewTab         = GetDlgItem (IDC_RESOURCE_OPEN_NEW_TAB);
    m_btnResourceOpenSelected       = GetDlgItem (IDC_RESOURCE_OPEN_SELECTED);
    m_btnViewImagesExportColoredPlt = GetDlgItem (IDC_VIEW_IMAGES_EXPORT_COL_PLT);
    m_btnViewModelsBackfaceCull     = GetDlgItem (IDC_VIEW_MODELS_BACKFACE_CULL);
    m_btnViewModelsDrawAxes         = GetDlgItem (IDC_VIEW_MODELS_DRAW_AXES);
    m_btnViewModelsFullAmbient      = GetDlgItem (IDC_VIEW_MODELS_FULL_AMBIENT);
    m_btnViewModelsHideFade1        = GetDlgItem (IDC_VIEW_MODELS_HIDE_FADE1);
    m_btnViewModelsHideFade2        = GetDlgItem (IDC_VIEW_MODELS_HIDE_FADE2);
    m_btnViewModelsHideFade4        = GetDlgItem (IDC_VIEW_MODELS_HIDE_FADE4);
    m_btnViewModelsMdlNameTex       = GetDlgItem (IDC_VIEW_MODELS_MODEL_NAME_TEX);
    m_btnViewModelsOrthographic     = GetDlgItem (IDC_VIEW_MODELS_ORTHOGRAPHIC);
    m_btnViewModelsOutlinePolys     = GetDlgItem (IDC_VIEW_MODELS_OUTLINE_POLYS);
    m_btnViewModelsPreferDds        = GetDlgItem (IDC_VIEW_MODELS_PREFER_DDS);
    m_btnViewModelsPreventDds       = GetDlgItem (IDC_VIEW_MODELS_PREVENT_DDS);
    m_btnViewModelsPreventTga       = GetDlgItem (IDC_VIEW_MODELS_PREVENT_TGA);
    m_btnViewModelsRunEmit          = GetDlgItem (IDC_VIEW_MODELS_RUN_EMIT);
    m_btnViewModelsShowAABB         = GetDlgItem (IDC_VIEW_MODELS_SHOW_AABB);
    m_btnViewModelsShowDummy        = GetDlgItem (IDC_VIEW_MODELS_SHOW_DUMMY);
    m_btnViewModelsShowEmit         = GetDlgItem (IDC_VIEW_MODELS_SHOW_EMIT);
    m_btnViewModelsShowLight        = GetDlgItem (IDC_VIEW_MODELS_SHOW_LIGHT);
    m_btnViewModelsShowRoot         = GetDlgItem (IDC_VIEW_MODELS_SHOW_ROOT);

    m_btnViewImagesAlphaNone        = GetDlgItem (IDC_VIEW_IMAGES_ALPHA_NONE);
    m_btnViewImagesAlphaTint        = GetDlgItem (IDC_VIEW_IMAGES_ALPHA_TINT);
    m_btnViewImagesAlphaOverride    = GetDlgItem (IDC_VIEW_IMAGES_ALPHA_OVERRIDE);
    m_btnViewModelsAlphaNormal      = GetDlgItem (IDC_VIEW_MODELS_ALPHA_NORMAL);
    m_btnViewModelsAlphaOneBit      = GetDlgItem (IDC_VIEW_MODELS_ALPHA_ONEBIT);
    m_btnViewModelsAlphaIgnore      = GetDlgItem (IDC_VIEW_MODELS_ALPHA_IGNORE);

    m_editNwnDirectory              = GetDlgItem (IDC_NWN_DIRECTORY);
    m_editTexSearchPath             = GetDlgItem (IDC_USER_HAK_MASK);

    m_editViewImagesAlphaR          = GetDlgItem (IDC_VIEW_IMAGES_ALPHA_R);
    m_editViewImagesAlphaG          = GetDlgItem (IDC_VIEW_IMAGES_ALPHA_G);
    m_editViewImagesAlphaB          = GetDlgItem (IDC_VIEW_IMAGES_ALPHA_B);
    m_editViewModelsBackR           = GetDlgItem (IDC_VIEW_MODELS_BACKGROUND_R);
    m_editViewModelsBackG           = GetDlgItem (IDC_VIEW_MODELS_BACKGROUND_G);
    m_editViewModelsBackB           = GetDlgItem (IDC_VIEW_MODELS_BACKGROUND_B);
    m_editViewModelsViewDistance    = GetDlgItem (IDC_VIEW_MODELS_VIEW_DISTANCE);
    m_editViewModelsClipDistance    = GetDlgItem (IDC_VIEW_MODELS_CLIP_DISTANCE);
    m_editViewModelsViewX           = GetDlgItem (IDC_VIEW_MODELS_VIEWPORT_X);
    m_editViewModelsViewY           = GetDlgItem (IDC_VIEW_MODELS_VIEWPORT_Y);
    m_editViewModelsAngleY          = GetDlgItem (IDC_VIEW_MODELS_ANGLE_Y);
    m_editViewModelsAngleP          = GetDlgItem (IDC_VIEW_MODELS_ANGLE_P);
    m_editViewModelsAngleR          = GetDlgItem (IDC_VIEW_MODELS_ANGLE_R);
    m_editViewModelsOrthoV          = GetDlgItem (IDC_VIEW_MODELS_ORTHOGRAPHIC_V);

    m_editNwnDirectory           .LimitText (_MAX_PATH);
    m_editTexSearchPath          .LimitText (_MAX_PATH * 2);

    m_editViewModelsBackR        .LimitText (3);
    m_editViewModelsBackG        .LimitText (3);
    m_editViewModelsBackB        .LimitText (3);
    m_editViewImagesAlphaR       .LimitText (3);
    m_editViewImagesAlphaG       .LimitText (3);
    m_editViewImagesAlphaB       .LimitText (3);
    m_editViewModelsViewDistance .LimitText (5);
    m_editViewModelsClipDistance .LimitText (5);
    m_editViewModelsViewX        .LimitText (5);
    m_editViewModelsViewY        .LimitText (5);
    m_editViewModelsAngleY       .LimitText (4);
    m_editViewModelsAngleP       .LimitText (4);
    m_editViewModelsAngleR       .LimitText (4);
    m_editViewModelsOrthoV       .LimitText (5);

    return 0;
}

//-----------------------------------------------------------------------------
//
// @mfunc Set the page active
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

BOOL CNwnOptionsPage::OnSetActive ()
{

    //
    // Set the fields
    //

    if (m_strNwnDirectory .IsEmpty ())
    {
        m_btnLoadNwnFrom .SetCheck (0);
    }
    else
    {
        m_btnLoadNwnFrom .SetCheck (1);
        m_editNwnDirectory .SetWindowText (m_strNwnDirectory);
    }

    m_editTexSearchPath             .SetWindowText (m_strTexSearchPath);

    m_editViewImagesAlphaR          .SetWindowText (m_strViewImagesAlphaR);
    m_editViewImagesAlphaG          .SetWindowText (m_strViewImagesAlphaG);
    m_editViewImagesAlphaB          .SetWindowText (m_strViewImagesAlphaB);
    m_editViewModelsBackR           .SetWindowText (m_strViewModelsBackR);
    m_editViewModelsBackG           .SetWindowText (m_strViewModelsBackG);
    m_editViewModelsBackB           .SetWindowText (m_strViewModelsBackB);
    m_editViewModelsClipDistance    .SetWindowText (m_strViewModelsClipDistance);
    m_editViewModelsViewDistance    .SetWindowText (m_strViewModelsViewDistance);
    m_editViewModelsViewX           .SetWindowText (m_strViewModelsViewX);
    m_editViewModelsViewY           .SetWindowText (m_strViewModelsViewY);
    m_editViewModelsAngleY          .SetWindowText (m_strViewModelsAngleY);
    m_editViewModelsAngleP          .SetWindowText (m_strViewModelsAngleP);
    m_editViewModelsAngleR          .SetWindowText (m_strViewModelsAngleR);
    m_editViewModelsOrthoV          .SetWindowText (m_strViewModelsOrthoV);

    m_btnResourceOpenNewTab         .SetCheck (!!m_fResourceOpenNewTab);
    m_btnResourceOpenSelected       .SetCheck (!!m_fResourceOpenSelected);

    m_btnViewImagesExportColoredPlt .SetCheck (!!m_fViewImagesExportColoredPlt);
    m_btnViewModelsBackfaceCull     .SetCheck (!!m_fViewModelsBackfaceCull);
    m_btnViewModelsDrawAxes         .SetCheck (!!m_fViewModelsDrawAxes);
    m_btnViewModelsFullAmbient      .SetCheck (!!m_fViewModelsFullAmbient);
    m_btnViewModelsHideFade1        .SetCheck (!!m_fViewModelsHideFade1);
    m_btnViewModelsHideFade2        .SetCheck (!!m_fViewModelsHideFade2);
    m_btnViewModelsHideFade4        .SetCheck (!!m_fViewModelsHideFade4);
    m_btnViewModelsMdlNameTex       .SetCheck (!!m_fViewModelsMdlNameTex);
    m_btnViewModelsOrthographic     .SetCheck (!!m_fViewModelsOrthographic);
    m_btnViewModelsOutlinePolys     .SetCheck (!!m_fViewModelsOutlinePolys);
    m_btnViewModelsPreferDds        .SetCheck (!!m_fViewModelsPreferDds);
    m_btnViewModelsPreventDds       .SetCheck (!!m_fViewModelsPreventDds);
    m_btnViewModelsPreventTga       .SetCheck (!!m_fViewModelsPreventTga);
    m_btnViewModelsRunEmit          .SetCheck (!!m_fViewModelsRunEmit);
    m_btnViewModelsShowAABB         .SetCheck (!!m_fViewModelsShowAABB);
    m_btnViewModelsShowDummy        .SetCheck (!!m_fViewModelsShowDummy);
    m_btnViewModelsShowEmit         .SetCheck (!!m_fViewModelsShowEmit);
    m_btnViewModelsShowLight        .SetCheck (!!m_fViewModelsShowLight);
    m_btnViewModelsShowRoot         .SetCheck (!!m_fViewModelsShowRoot);

    m_btnViewImagesExportColoredPlt .EnableWindow (FALSE);
    m_btnViewModelsRunEmit .EnableWindow (FALSE);

    switch (m_nViewImagesAlphaBehavior) {
        case  1: m_btnViewImagesAlphaTint     .SetCheck (1); break;
        case  2: m_btnViewImagesAlphaOverride .SetCheck (1); break;
        default: m_btnViewImagesAlphaNone     .SetCheck (1); break;
    }

    switch (m_nViewModelsAlphaBehavior) {
        case  1: m_btnViewModelsAlphaOneBit   .SetCheck (1); break;
        case  2: m_btnViewModelsAlphaIgnore   .SetCheck (1); break;
        default: m_btnViewModelsAlphaNormal   .SetCheck (1); break;
    }

    //
    // Update the windows
    //

    UpdateWindows ();
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Kill the page active
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

BOOL CNwnOptionsPage::OnKillActive ()
{
    CString str;

    m_editTexSearchPath          .GetWindowText (m_strTexSearchPath);
    m_editViewImagesAlphaR       .GetWindowText (m_strViewImagesAlphaR);
    m_editViewImagesAlphaG       .GetWindowText (m_strViewImagesAlphaG);
    m_editViewImagesAlphaB       .GetWindowText (m_strViewImagesAlphaB);
    m_editViewModelsBackR        .GetWindowText (m_strViewModelsBackR);
    m_editViewModelsBackG        .GetWindowText (m_strViewModelsBackG);
    m_editViewModelsBackB        .GetWindowText (m_strViewModelsBackB);
    m_editViewModelsClipDistance .GetWindowText (m_strViewModelsClipDistance);
    m_editViewModelsViewDistance .GetWindowText (m_strViewModelsViewDistance);
    m_editViewModelsViewX        .GetWindowText (m_strViewModelsViewX);
    m_editViewModelsViewY        .GetWindowText (m_strViewModelsViewY);
    m_editViewModelsAngleY       .GetWindowText (m_strViewModelsAngleY);
    m_editViewModelsAngleP       .GetWindowText (m_strViewModelsAngleP);
    m_editViewModelsAngleR       .GetWindowText (m_strViewModelsAngleR);
    m_editViewModelsOrthoV       .GetWindowText (m_strViewModelsOrthoV);

    //
    // If we are using default directories
    //

    if (m_btnLoadNwnFrom .GetCheck () == 0)
    {
        m_strNwnDirectory .Empty ();
    }
    else
    {
        m_editNwnDirectory .GetWindowText (m_strNwnDirectory);
        m_strNwnDirectory .TrimRight ();
        m_strNwnDirectory .TrimLeft ();
        if (m_strNwnDirectory .IsEmpty ())
        {
            str .Format (IDS_ERR_NO_DIRECTORY);
            ::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
            return FALSE;
        }
        if (m_strNwnDirectory [m_strNwnDirectory .GetLength () - 1] != '\\' &&
            m_strNwnDirectory [m_strNwnDirectory .GetLength () - 1] != '/' &&
            m_strNwnDirectory [m_strNwnDirectory .GetLength () - 1] != ':')
            m_strNwnDirectory += _T ("\\");
    }

    //
    // Validate
    //

    str = m_strNwnDirectory;
    if (str .IsEmpty ())
        str = GetNwnDirectory ();

    //
    // Try to open the dialog file
    //

    //str += "dialog.tlk";
    FILE *fp = _tfopen (str + "dialog.tlk", "rb");
    if (fp == NULL)
        fp = _tfopen (str + "data\\dialog.tlk", "rb");
    if (fp == NULL)
    {
        str .Format (IDS_ERR_INV_DIRECTORY);
        ::MessageBox (m_hWnd, str, g_szAppName, MB_OK);
        return TRUE;
    }
    else
        fclose (fp);
    return FALSE;
}


/* vim: set sw=4: */
