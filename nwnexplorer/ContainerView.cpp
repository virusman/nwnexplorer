//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ContainerView.cpp - Container view for the views |
//
// This module contains the definition of the container view
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
// $History: ContainerView.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "ContainerView.h"

//
// Debug NEW
//

#if defined (_DEBUG) && !defined (_WIN32_WCE)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//-----------------------------------------------------------------------------
//
// @mfunc <c CContainerView> constructor.
//
// @parm CData * | pFile | Pointer to the file
//
// @parm const CString & | strResName | Resource name
//
// @parm NwnResType | nResType | Resource type
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CContainerView::CContainerView (CData *pFile, const CString &strResName,
                                NwnResType nResType)
{
    m_sRes .Set (pFile);
    m_strResName = strResName;
    m_nResType = nResType;
    m_fViewNewWindow = true;
    m_nCurrentMode = 0;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CContainerView> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CContainerView::~CContainerView ()
{
    //
    // Delete all the views
    //

    for (int i = 0; i < m_apViews .GetCount (); i++)
    {
        m_apViews [i] ->DestroyTheWindow ();
        delete m_apViews [i];
    }
}


/* vim: set sw=4: */
