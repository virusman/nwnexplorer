//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ScriptRawView.cpp - Script view window |
//
// This module contains the definition of the script view window.
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
// $History: ScriptRawView.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "ScriptRawView.h"
#include "../_NwnLib/NwnStreams.h"
#include "../_NscLib/Nsc.h"

//
// Debug NEW
//

#if defined (_DEBUG) && !defined (_WIN32_WCE)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// Globals
//

extern FILE *g_fpDebug;

//-----------------------------------------------------------------------------
//
// @mfunc <c CScriptRawView> constructor.
//
// @parm CData * | pFile | Hierarchy file
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CScriptRawView::CScriptRawView (CData *pFile) : 
    CTextViewImpl (IDS_SCRIPT_RAW)
{

    //
    // Initialize variables
    //

    m_sRes .Set (pFile);
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CScriptRawView> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CScriptRawView::~CScriptRawView ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the window for this view
//
// @parm HWND | hWnd | Parent window
//
// @rdesc Pointer to the window
//
//-----------------------------------------------------------------------------

CWindow *CScriptRawView::GetWindow (HWND hWnd)
{
    if (m_hWnd == NULL && m_sRes != NULL)
        Write ();
    return CTextViewImpl::GetWindow (hWnd);
}

//-----------------------------------------------------------------------------
//
// @mfunc Set the contents of the file
//
// @parm unsigned char * | pauchData | Pointer to the data
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CScriptRawView::Write ()
{
    //
    // Create the output stream
    //

    CNwnMemoryStream sStream;
    unsigned char *pStart = m_sRes .GetData ();
    unsigned long ulSize = m_sRes .GetSize ();
    unsigned char *pEnd = &pStart [ulSize];
    NscScriptDecompile (sStream, pStart, ulSize);

    //
    // Save the stream
    //

    char szNull = 0;
    sStream .Write (&szNull, 1);
    m_strText = (char *) sStream .GetData ();
}


/* vim: set sw=4: */
