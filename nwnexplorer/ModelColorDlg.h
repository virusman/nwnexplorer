#ifndef ETS_MODELCOLORDLG_H
#define ETS_MODELCOLORDLG_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ModelColorDlg.h - Model color dialog |
//
// This module contains the definition for model color dialog.
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
// $History: ModelColorDlg.h $
//      
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include "nwnexplorer.h"
#include "PaletteColorButton.h"

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

class CModelColorDlg : 
    public CDialogImpl <CModelColorDlg>
{
    // @access Enumerations and types
    public:

        // @access Constructors and destructors
    public:

        // @cmember General constructor

        CModelColorDlg (ModelContext *pModelContext = NULL);

        // @cmember General destructor

        ~CModelColorDlg ();

        // @access COM interface list
    public:
        enum { IDD = IDD_MODEL_COLOR };
        BEGIN_MSG_MAP (CModelColorDlg)
            MESSAGE_HANDLER (WM_INITDIALOG, OnInitDialog)

            NOTIFY_CODE_HANDLER (CPN_SELCHANGE, OnColorSelChange)

            REFLECT_NOTIFICATIONS ();
        END_MSG_MAP ()

            // @access Message handlers
    protected:

            // @cmember Initialize the dialog box

            LRESULT OnInitDialog (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // @cmember Handle color change

            LRESULT OnColorSelChange (int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
            {
                NMPALETTECOLORBUTTON *p = (NMPALETTECOLORBUTTON *) pnmh;
                if (!p ->fColorValid)
                    return TRUE;
                if (idCtrl == IDC_MODEL_ARMOR1)
                    m_pModelContext ->anPalettes [NwnPalette_Metal1] = p ->nIndex;
                else if (idCtrl == IDC_MODEL_ARMOR2)
                    m_pModelContext ->anPalettes [NwnPalette_Metal2] = p ->nIndex;
                else if (idCtrl == IDC_MODEL_LEATHER1)
                    m_pModelContext ->anPalettes [NwnPalette_Leather1] = p ->nIndex;
                else if (idCtrl == IDC_MODEL_LEATHER2)
                    m_pModelContext ->anPalettes [NwnPalette_Leather2] = p ->nIndex;
                else if (idCtrl == IDC_MODEL_CLOTH1)
                    m_pModelContext ->anPalettes [NwnPalette_Cloth1] = p ->nIndex;
                else if (idCtrl == IDC_MODEL_CLOTH2)
                    m_pModelContext ->anPalettes [NwnPalette_Cloth2] = p ->nIndex;
                else if (idCtrl == IDC_MODEL_TATTOO1)
                    m_pModelContext ->anPalettes [NwnPalette_Tattoo1] = p ->nIndex;
                else if (idCtrl == IDC_MODEL_TATTOO2)
                    m_pModelContext ->anPalettes [NwnPalette_Tattoo2] = p ->nIndex;
                else if (idCtrl == IDC_MODEL_SKIN)
                    m_pModelContext ->anPalettes [NwnPalette_Skin] = p ->nIndex;
                else if (idCtrl == IDC_MODEL_HAIR)
                    m_pModelContext ->anPalettes [NwnPalette_Hair] = p ->nIndex;
                ::SendMessage (GetParent () .GetParent () .GetParent (), 
                               WM_APP_MDLCTXCHANGED, 0, 0);
                return TRUE;
            }

            // @access Protected members
    public:

            // @cmember Pointer to the model context

            ModelContext                        *m_pModelContext;

            // @cmember Color button controls

            CPaletteColorButton         m_btnArmor1;
            CPaletteColorButton         m_btnArmor2;
            CPaletteColorButton         m_btnLeather1;
            CPaletteColorButton         m_btnLeather2;
            CPaletteColorButton         m_btnCloth1;
            CPaletteColorButton         m_btnCloth2;
            CPaletteColorButton         m_btnHair;
            CPaletteColorButton         m_btnSkin;
            CPaletteColorButton         m_btnTattoo1;
            CPaletteColorButton         m_btnTattoo2;
};

#endif // ETS_MODELCOLORDLG_H

/* vim: set sw=4: */
