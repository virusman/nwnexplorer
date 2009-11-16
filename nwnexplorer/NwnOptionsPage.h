#ifndef ETS_NWNOPTIONSPAGE_H
#define ETS_NWNOPTIONSPAGE_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnOptionsPage.h - NWN options page |
//
// This module contains the definition of the NWN options page.
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
// $History: NwnOptionsPage.h $
//      
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

//
// Class definitions
//

class CNwnOptionsPage : 
    public CPropertyPageImpl <CNwnOptionsPage>
{
    // @access Enumerations and types
    public:

        enum { IDD = IDD_OPTIONS_NWN };

        // @access Constructors
    public:

        // @cmember General constructor

        CNwnOptionsPage ();

        // @cmember General destructor

        ~CNwnOptionsPage ();

        // @access Public virtual methods
    public:

        // @cmember Set the active page

        BOOL OnSetActive ();

        // @cmember Kill the active page

        BOOL OnKillActive ();

        // @access ATL dialog support
    public:
        BEGIN_MSG_MAP (CTopicGeneralPage)
            MESSAGE_HANDLER (WM_INITDIALOG, OnInitDialog)

            COMMAND_HANDLER (IDC_LOAD_NWN_FROM,                  BN_CLICKED, OnLoadNwnFrom)
            COMMAND_HANDLER (IDC_NWN_DIRECTORY_BROWSE,           BN_CLICKED, OnDirectoryBrowse)
            COMMAND_HANDLER (IDC_RESOURCE_OPEN_NEW_TAB,          BN_CLICKED, OnViewModelsButtonToggle)
            COMMAND_HANDLER (IDC_RESOURCE_OPEN_SELECTED,         BN_CLICKED, OnViewModelsButtonToggle)
            COMMAND_HANDLER (IDC_VIEW_IMAGES_ALPHA_NONE,         BN_CLICKED, OnViewModelsButtonToggle)
            COMMAND_HANDLER (IDC_VIEW_IMAGES_ALPHA_OVERRIDE,     BN_CLICKED, OnViewModelsButtonToggle)
            COMMAND_HANDLER (IDC_VIEW_IMAGES_ALPHA_TINT,         BN_CLICKED, OnViewModelsButtonToggle)
            COMMAND_HANDLER (IDC_VIEW_IMAGES_EXPORT_COL_PLT,     BN_CLICKED, OnViewModelsButtonToggle)
            COMMAND_HANDLER (IDC_VIEW_MODELS_ALPHA_IGNORE,       BN_CLICKED, OnViewModelsButtonToggle)
            COMMAND_HANDLER (IDC_VIEW_MODELS_ALPHA_NORMAL,       BN_CLICKED, OnViewModelsButtonToggle)
            COMMAND_HANDLER (IDC_VIEW_MODELS_ALPHA_ONEBIT,       BN_CLICKED, OnViewModelsButtonToggle)
            COMMAND_HANDLER (IDC_VIEW_MODELS_BACKFACE_CULL,      BN_CLICKED, OnViewModelsButtonToggle)
            COMMAND_HANDLER (IDC_VIEW_MODELS_DRAW_AXES,          BN_CLICKED, OnViewModelsButtonToggle)
            COMMAND_HANDLER (IDC_VIEW_MODELS_FULL_AMBIENT,       BN_CLICKED, OnViewModelsButtonToggle)
            COMMAND_HANDLER (IDC_VIEW_MODELS_HIDE_FADE1,         BN_CLICKED, OnViewModelsButtonToggle)
            COMMAND_HANDLER (IDC_VIEW_MODELS_HIDE_FADE2,         BN_CLICKED, OnViewModelsButtonToggle)
            COMMAND_HANDLER (IDC_VIEW_MODELS_HIDE_FADE4,         BN_CLICKED, OnViewModelsButtonToggle)
            COMMAND_HANDLER (IDC_VIEW_MODELS_MODEL_NAME_TEX,     BN_CLICKED, OnViewModelsButtonToggle)
            COMMAND_HANDLER (IDC_VIEW_MODELS_ORTHOGRAPHIC,       BN_CLICKED, OnViewModelsButtonToggle)
            COMMAND_HANDLER (IDC_VIEW_MODELS_OUTLINE_POLYS,      BN_CLICKED, OnViewModelsButtonToggle)
            COMMAND_HANDLER (IDC_VIEW_MODELS_PREFER_DDS,         BN_CLICKED, OnViewModelsButtonToggle)
            COMMAND_HANDLER (IDC_VIEW_MODELS_PREVENT_DDS,        BN_CLICKED, OnViewModelsButtonToggle)
            COMMAND_HANDLER (IDC_VIEW_MODELS_PREVENT_TGA,        BN_CLICKED, OnViewModelsButtonToggle)
            COMMAND_HANDLER (IDC_VIEW_MODELS_RUN_EMIT,           BN_CLICKED, OnViewModelsButtonToggle)
            COMMAND_HANDLER (IDC_VIEW_MODELS_SHOW_AABB,          BN_CLICKED, OnViewModelsButtonToggle)
            COMMAND_HANDLER (IDC_VIEW_MODELS_SHOW_DUMMY,         BN_CLICKED, OnViewModelsButtonToggle)
            COMMAND_HANDLER (IDC_VIEW_MODELS_SHOW_EMIT,          BN_CLICKED, OnViewModelsButtonToggle)
            COMMAND_HANDLER (IDC_VIEW_MODELS_SHOW_LIGHT,         BN_CLICKED, OnViewModelsButtonToggle)
            COMMAND_HANDLER (IDC_VIEW_MODELS_SHOW_ROOT,          BN_CLICKED, OnViewModelsButtonToggle)

            CHAIN_MSG_MAP (CPropertyPageImpl <CNwnOptionsPage>);

        REFLECT_NOTIFICATIONS ();
        END_MSG_MAP ()

            // @access Message handlers
    protected:

            // @cmember Initialize the dialog

            LRESULT OnInitDialog (UINT uMsg, WPARAM wParam, 
                                  LPARAM lParam, BOOL &bHandled);

            // @cmember Change the button

            LRESULT OnLoadNwnFrom (WORD wNotifyCode, 
                                   WORD wID, HWND hWndCtl, BOOL& bHandled)
            {
                UpdateWindows ();
                return TRUE;
            }

            // @cmember Change the button

            LRESULT OnViewModelsButtonToggle (WORD wNotifyCode, 
                                              WORD wID, HWND hWndCtl, BOOL& bHandled)
            {
                m_fResourceOpenNewTab         = m_btnResourceOpenNewTab         .GetCheck () != 0;
                m_fResourceOpenSelected       = m_btnResourceOpenSelected       .GetCheck () != 0;
                m_fViewImagesExportColoredPlt = m_btnViewImagesExportColoredPlt .GetCheck () != 0;
                m_fViewModelsBackfaceCull     = m_btnViewModelsBackfaceCull     .GetCheck () != 0;
                m_fViewModelsDrawAxes         = m_btnViewModelsDrawAxes         .GetCheck () != 0;
                m_fViewModelsFullAmbient      = m_btnViewModelsFullAmbient      .GetCheck () != 0;
                m_fViewModelsHideFade1        = m_btnViewModelsHideFade1        .GetCheck () != 0;
                m_fViewModelsHideFade2        = m_btnViewModelsHideFade2        .GetCheck () != 0;
                m_fViewModelsHideFade4        = m_btnViewModelsHideFade4        .GetCheck () != 0;
                m_fViewModelsMdlNameTex       = m_btnViewModelsMdlNameTex       .GetCheck () != 0;
                m_fViewModelsOrthographic     = m_btnViewModelsOrthographic     .GetCheck () != 0;
                m_fViewModelsOutlinePolys     = m_btnViewModelsOutlinePolys     .GetCheck () != 0;
                m_fViewModelsPreferDds        = m_btnViewModelsPreferDds        .GetCheck () != 0;
                m_fViewModelsPreventDds       = m_btnViewModelsPreventDds       .GetCheck () != 0;
                m_fViewModelsPreventTga       = m_btnViewModelsPreventTga       .GetCheck () != 0;
                m_fViewModelsRunEmit          = m_btnViewModelsRunEmit          .GetCheck () != 0;
                m_fViewModelsShowAABB         = m_btnViewModelsShowAABB         .GetCheck () != 0;
                m_fViewModelsShowDummy        = m_btnViewModelsShowDummy        .GetCheck () != 0;
                m_fViewModelsShowEmit         = m_btnViewModelsShowEmit         .GetCheck () != 0;
                m_fViewModelsShowLight        = m_btnViewModelsShowLight        .GetCheck () != 0;
                m_fViewModelsShowRoot         = m_btnViewModelsShowRoot         .GetCheck () != 0;

                if (m_btnViewImagesAlphaOverride .GetCheck ())
                    m_nViewImagesAlphaBehavior = 2;
                else if (m_btnViewImagesAlphaTint .GetCheck ())
                    m_nViewImagesAlphaBehavior = 1;
                else
                    m_nViewImagesAlphaBehavior = 0;

                if (m_btnViewModelsAlphaIgnore .GetCheck ())
                    m_nViewModelsAlphaBehavior = 2;
                else if (m_btnViewModelsAlphaOneBit .GetCheck ())
                    m_nViewModelsAlphaBehavior = 1;
                else
                    m_nViewModelsAlphaBehavior = 0;

                return TRUE;
            }

            // @cmember Browse for a directory

            LRESULT OnDirectoryBrowse (WORD wNotifyCode, 
                                       WORD wID, HWND hWndCtl, BOOL& bHandled)
            {
                CString str;
                m_editNwnDirectory .GetWindowText (str);
                CFolderDialog dlg (m_hWnd);
                if (dlg .DoModal () == IDOK)
                {
                    m_editNwnDirectory .SetWindowText (
                        dlg .m_szFolderPath);
                }
                return TRUE;
            }

            // @access Protected methods
    protected:

            // @cmember Update the windows

            void UpdateWindows ()
            {
                bool fEnable = m_btnLoadNwnFrom .GetCheck () != 0;
                m_editNwnDirectory .EnableWindow (fEnable);
                m_btnNwnDirectory .EnableWindow (fEnable);
            }

            // @access Public variables
    public:

            CString                     m_strNwnDirectory;
            CString                     m_strTexSearchPath;

            BOOL                        m_fResourceOpenNewTab;
            BOOL                        m_fResourceOpenSelected;

            BOOL                        m_fViewImagesExportColoredPlt;
            BOOL                        m_fViewModelsBackfaceCull;
            BOOL                        m_fViewModelsDrawAxes;
            BOOL                        m_fViewModelsFullAmbient;
            BOOL                        m_fViewModelsHideFade1;
            BOOL                        m_fViewModelsHideFade2;
            BOOL                        m_fViewModelsHideFade4;
            BOOL                        m_fViewModelsMdlNameTex;
            BOOL                        m_fViewModelsOrthographic;
            BOOL                        m_fViewModelsOutlinePolys;
            BOOL                        m_fViewModelsPreferDds;
            BOOL                        m_fViewModelsPreventDds;
            BOOL                        m_fViewModelsPreventTga;
            BOOL                        m_fViewModelsRunEmit;
            BOOL                        m_fViewModelsShowAABB;
            BOOL                        m_fViewModelsShowDummy;
            BOOL                        m_fViewModelsShowEmit;
            BOOL                        m_fViewModelsShowLight;
            BOOL                        m_fViewModelsShowRoot;

            DWORD                       m_nViewImagesAlphaBehavior;
            DWORD                       m_nViewModelsAlphaBehavior;

            CString                     m_strViewImagesAlphaR;
            CString                     m_strViewImagesAlphaG;
            CString                     m_strViewImagesAlphaB;

            CString                     m_strViewModelsBackR;
            CString                     m_strViewModelsBackG;
            CString                     m_strViewModelsBackB;
            CString                     m_strViewModelsClipDistance;
            CString                     m_strViewModelsViewDistance;
            CString                     m_strViewModelsViewX;
            CString                     m_strViewModelsViewY;
            CString                     m_strViewModelsAngleY;
            CString                     m_strViewModelsAngleP;
            CString                     m_strViewModelsAngleR;
            CString                     m_strViewModelsOrthoV;

            // @access Protected members
    protected:

            // @cmember Beauty over speed button

            CButton                     m_btnNwnDirectory;
            CButton                     m_btnLoadNwnFrom;

            CButton                     m_btnResourceOpenNewTab;
            CButton                     m_btnResourceOpenSelected;

            CButton                     m_btnViewImagesExportColoredPlt;
            CButton                     m_btnViewModelsBackfaceCull;
            CButton                     m_btnViewModelsDrawAxes;
            CButton                     m_btnViewModelsFullAmbient;
            CButton                     m_btnViewModelsHideFade1;
            CButton                     m_btnViewModelsHideFade2;
            CButton                     m_btnViewModelsHideFade4;
            CButton                     m_btnViewModelsMdlNameTex;
            CButton                     m_btnViewModelsOrthographic;
            CButton                     m_btnViewModelsOutlinePolys;
            CButton                     m_btnViewModelsPreferDds;
            CButton                     m_btnViewModelsPreventDds;
            CButton                     m_btnViewModelsPreventTga;
            CButton                     m_btnViewModelsRunEmit;
            CButton                     m_btnViewModelsShowAABB;
            CButton                     m_btnViewModelsShowDummy;
            CButton                     m_btnViewModelsShowEmit;
            CButton                     m_btnViewModelsShowLight;
            CButton                     m_btnViewModelsShowRoot;

            CButton                     m_btnViewImagesAlphaNone;
            CButton                     m_btnViewImagesAlphaTint;
            CButton                     m_btnViewImagesAlphaOverride;

            CButton                     m_btnViewModelsAlphaNormal;
            CButton                     m_btnViewModelsAlphaOneBit;
            CButton                     m_btnViewModelsAlphaIgnore;

            // @cmember NWN directory edit

            CEdit                       m_editNwnDirectory;

            // @cmember User hak mask edit

            CEdit                       m_editTexSearchPath;

            // @cmember Other edit boxes

            CEdit                       m_editViewImagesAlphaR;
            CEdit                       m_editViewImagesAlphaG;
            CEdit                       m_editViewImagesAlphaB;

            CEdit                       m_editViewModelsBackR;
            CEdit                       m_editViewModelsBackG;
            CEdit                       m_editViewModelsBackB;
            CEdit                       m_editViewModelsClipDistance;
            CEdit                       m_editViewModelsViewDistance;
            CEdit                       m_editViewModelsViewX;
            CEdit                       m_editViewModelsViewY;
            CEdit                       m_editViewModelsAngleY;
            CEdit                       m_editViewModelsAngleP;
            CEdit                       m_editViewModelsAngleR;
            CEdit                       m_editViewModelsOrthoV;
};

#endif // ETS_NWNOPTIONSPAGE_H

/* vim: set sw=4: */
