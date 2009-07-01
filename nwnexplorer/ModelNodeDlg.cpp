//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ModelNodeDlg.cpp - Model position dialog |
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
// $History: ModelNodeDlg.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"
#include "ModelNodeDlg.h"

//-----------------------------------------------------------------------------
//
// @mfunc <c CModelNodeDlg> constructor.
//
// @parm ModelContext * | pModelContext | Model context
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CModelNodeDlg::CModelNodeDlg (ModelContext *pModelContext)
{
    m_pModelContext = pModelContext;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CModelNodeDlg> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CModelNodeDlg::~CModelNodeDlg()
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

LRESULT CModelNodeDlg::OnInitDialog (UINT uMsg, 
                                    WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{

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

LRESULT CModelNodeDlg::OnDrawItem (UINT uMsg, WPARAM wParam, 
                                  LPARAM lParam, BOOL &bHandled) 
{
    return 1;
}


/* vim: set sw=4: */
