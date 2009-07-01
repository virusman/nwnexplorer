#ifndef ETS_MODELPOSDLG_H
#define ETS_MODELPOSDLG_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ModelPosDlg.h - Model position dialog |
//
// This module contains the definition for model position dialog.
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
// $History: ModelPosDlg.h $
//      
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include "nwnexplorer.h"
#include "../_MathLib/Constants.h"

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

class CModelPosDlg : 
    public CDialogImpl <CModelPosDlg>,
    public CThemeImpl <CModelPosDlg>
{
    // @access Enumerations and types
    public:

        // @access Constructors and destructors
    public:

        // @cmember General constructor

        CModelPosDlg (ModelContext *pModelContext = NULL);

        // @cmember General destructor

        ~CModelPosDlg ();

        // @access COM interface list
    public:
        enum { IDD = IDD_MODEL_POS };
        BEGIN_MSG_MAP (CModelPosDlg)
            MESSAGE_HANDLER (WM_INITDIALOG, OnInitDialog)
            MESSAGE_HANDLER (WM_DRAWITEM, OnDrawItem)
            MESSAGE_HANDLER (WM_TIMER, OnTimer)

            REFLECT_NOTIFICATIONS ();
        END_MSG_MAP ()

            // @access Message handlers
    protected:

            // @cmember Initialize the dialog box

            LRESULT OnInitDialog (UINT uMsg, 
                                  WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // @cmember Handle draw item

            LRESULT OnDrawItem (UINT uMsg, 
                                WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // @cmember Handle timer

            LRESULT OnTimer (UINT uMsg, 
                             WPARAM wParam, LPARAM lParam, BOOL &bHandled)
            {
                extern BOOL g_fViewModelsOrthographic;
                float fX, fY, fInc = (g_fViewModelsOrthographic ? 90.0f : 15.0f);
                CVector3 vHPR (0, 0, 0);
                CVector3 vXYZ (0, 0, 0);

                if (g_fViewModelsOrthographic) {
                    fX = 5.0;
                    fY = 5.0;
                } else {
                    float fScale = m_pModelContext ->vView .m_z;
                    if (fScale <= 0.5f)
                        fScale = 0.5f;
                    fScale *= 1.4f;

                    CRect rect;
                    GetClientRect (&rect);

                    fX = fScale / rect .Width ();
                    fY = fScale / rect .Height ();
                }

                if ((m_btnPitchForward .GetState () & BST_PUSHED) != 0)
                    vHPR = CVector3 (0.0, 0.0, fInc);
                else if ((m_btnPitchBackward .GetState () & BST_PUSHED) != 0)
                    vHPR = CVector3 (0.0, 0.0, -fInc);
                else if ((m_btnYawLeft .GetState () & BST_PUSHED) != 0)
                    vHPR = CVector3 (fInc, 0.0, 0.0);
                else if ((m_btnYawRight .GetState () & BST_PUSHED) != 0)
                    vHPR = CVector3 (-fInc, 0.0, 0.0);
                else if ((m_btnRollLeft .GetState () & BST_PUSHED) != 0)
                    vHPR = CVector3 (0.0, fInc, 0.0);
                else if ((m_btnRollRight .GetState () & BST_PUSHED) != 0)
                    vHPR = CVector3 (0.0, -fInc, 0.0);
                else if ((m_btnZoomIn .GetState () & BST_PUSHED) != 0)
                    vXYZ = CVector3 (0.0, 0.0, -1.0);
                else if ((m_btnZoomOut .GetState () & BST_PUSHED) != 0)
                    vXYZ = CVector3 (0.0, 0.0, 1.0);
                else if ((m_btnMoveLeft .GetState () & BST_PUSHED) != 0)
                    vXYZ = CVector3 (-fX, 0.0, 0.0);
                else if ((m_btnMoveRight .GetState () & BST_PUSHED) != 0)
                    vXYZ = CVector3 (fX, 0.0, 0.0);
                else if ((m_btnMoveUp .GetState () & BST_PUSHED) != 0)
                    vXYZ = CVector3 (0.0, -fY, 0.0);
                else if ((m_btnMoveDown .GetState () & BST_PUSHED) != 0)
                    vXYZ = CVector3 (0.0, fY, 0.0);

                if (vHPR .LengthSqr () > 0.0f || vXYZ .LengthSqr () > 0.0f)
                {
                    if (vHPR .LengthSqr () > 0.0f)
                    {
                        CMatrix mView (m_pModelContext ->mView);
                        CMatrix m;
                        m .MakeHPR (vHPR);
                        m_pModelContext ->mView = m * mView;
                    }
                    m_pModelContext ->vView += vXYZ;
                    if (m_pModelContext ->vView .m_z < 0.0)
                        m_pModelContext ->vView .m_z = 0.0;
                    ::SendMessage (GetParent () .GetParent () .GetParent (), 
                                   WM_APP_MDLPOSCHANGED, 0, 0);
                }
                return TRUE;
            }

            // @access Protected members
    public:

            // @cmember Pointer to the model context

            ModelContext                        *m_pModelContext;

            // @cmember Image list

            CImageList                  m_il;

            // @cmember Pitch forward button

            CButton                             m_btnPitchForward;

            // @cmember Pitch forward icon

            int                                 m_nPitchForward;

            // @cmember Pitch backward button

            CButton                             m_btnPitchBackward;

            // @cmember Pitch backward icon

            int                                 m_nPitchBackward;

            // @cmember Yaw left button

            CButton                             m_btnYawLeft;

            // @cmember Yaw left icon

            int                                 m_nYawLeft;

            // @cmember Yaw right button

            CButton                             m_btnYawRight;

            // @cmember Yaw right icon

            int                                 m_nYawRight;    

            // @cmember Roll left button

            CButton                             m_btnRollLeft;

            // @cmember Roll left icon

            int                                 m_nRollLeft;

            // @cmember Roll right button

            CButton                             m_btnRollRight;

            // @cmember Roll right icon

            int                                 m_nRollRight;   

            // @cmember Zoom in button

            CButton                             m_btnZoomIn;

            // @cmember Zoom in icon

            int                                 m_nZoomIn;

            // @cmember Zoom out button

            CButton                             m_btnZoomOut;

            // @cmember Zoom out icon

            int                                 m_nZoomOut;     

            // @cmember Move left button

            CButton                             m_btnMoveLeft;

            // @cmember Move left icon

            int                                 m_nMoveLeft;

            // @cmember Move right button

            CButton                             m_btnMoveRight;

            // @cmember Move right icon

            int                                 m_nMoveRight;   

            // @cmember Move up button

            CButton                             m_btnMoveUp;

            // @cmember Move up icon

            int                                 m_nMoveUp;

            // @cmember Move down button

            CButton                             m_btnMoveDown;

            // @cmember Move down icon

            int                                 m_nMoveDown;    
};

#endif // ETS_MODELPOSDLG_H

/* vim: set sw=4: */
