//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ModelColorDlg.cpp - Model color dialog |
//
// This module contains the definition model color dialog.
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
// $History: ModelColorDlg.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"
#include "ModelColorDlg.h"

//-----------------------------------------------------------------------------
//
// @mfunc <c CModelColorDlg> constructor.
//
// @parm ModelContext * | pModelContext | Model context
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CModelColorDlg::CModelColorDlg (ModelContext *pModelContext)
{
    m_pModelContext = pModelContext;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CModelColorDlg> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CModelColorDlg::~CModelColorDlg()
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

LRESULT CModelColorDlg::OnInitDialog (UINT uMsg, 
                                      WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{

    //
    // Subclass the controls
    //

    m_btnArmor1 .SubclassWindow (GetDlgItem (IDC_MODEL_ARMOR1));
    m_btnArmor2 .SubclassWindow (GetDlgItem (IDC_MODEL_ARMOR2));
    m_btnLeather1 .SubclassWindow (GetDlgItem (IDC_MODEL_LEATHER1));
    m_btnLeather2 .SubclassWindow (GetDlgItem (IDC_MODEL_LEATHER2));
    m_btnCloth1 .SubclassWindow (GetDlgItem (IDC_MODEL_CLOTH1));
    m_btnCloth2 .SubclassWindow (GetDlgItem (IDC_MODEL_CLOTH2));
    m_btnHair .SubclassWindow (GetDlgItem (IDC_MODEL_HAIR));
    m_btnSkin .SubclassWindow (GetDlgItem (IDC_MODEL_SKIN));
    m_btnTattoo1 .SubclassWindow (GetDlgItem (IDC_MODEL_TATTOO1));
    m_btnTattoo2 .SubclassWindow (GetDlgItem (IDC_MODEL_TATTOO2));

    //
    // Attach the bitmaps
    //

    m_btnArmor1 .SetBitmap (IDB_COLOR_ARMOR);
    m_btnArmor2 .SetBitmap (IDB_COLOR_ARMOR);
    m_btnLeather1 .SetBitmap (IDB_COLOR_LEATHER);
    m_btnLeather2 .SetBitmap (IDB_COLOR_LEATHER);
    m_btnCloth1 .SetBitmap (IDB_COLOR_CLOTH);
    m_btnCloth2 .SetBitmap (IDB_COLOR_CLOTH);
    m_btnHair .SetBitmap (IDB_COLOR_HAIR);
    m_btnSkin .SetBitmap (IDB_COLOR_SKIN);
    m_btnTattoo1 .SetBitmap (IDB_COLOR_TATTOO);
    m_btnTattoo2 .SetBitmap (IDB_COLOR_TATTOO);

    //
    // Set the colors of all the cotrols
    //

    m_btnArmor1 .SetColorIndex (m_pModelContext ->anPalettes [NwnPalette_Metal1]);
    m_btnArmor2 .SetColorIndex (m_pModelContext ->anPalettes [NwnPalette_Metal2]);
    m_btnLeather1 .SetColorIndex (m_pModelContext ->anPalettes [NwnPalette_Leather1]);
    m_btnLeather2 .SetColorIndex (m_pModelContext ->anPalettes [NwnPalette_Leather2]);
    m_btnCloth1 .SetColorIndex (m_pModelContext ->anPalettes [NwnPalette_Cloth1]);
    m_btnCloth2 .SetColorIndex (m_pModelContext ->anPalettes [NwnPalette_Cloth2]);
    m_btnTattoo1 .SetColorIndex (m_pModelContext ->anPalettes [NwnPalette_Tattoo1]);
    m_btnTattoo2 .SetColorIndex (m_pModelContext ->anPalettes [NwnPalette_Tattoo2]);
    m_btnHair .SetColorIndex (m_pModelContext ->anPalettes [NwnPalette_Hair]);
    m_btnSkin .SetColorIndex (m_pModelContext ->anPalettes [NwnPalette_Skin]);
    return FALSE;
}

/* vim: set sw=4: */
