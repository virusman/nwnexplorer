//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      TextViewImpl.cpp - Text view window |
//
// This module contains the definition of the text view window.
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
// $History: TextViewImpl.cpp $
//      
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "TextViewImpl.h"

//
// Debug NEW
//

#if defined (_DEBUG) && !defined (_WIN32_WCE)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// Externals
//

extern CFont g_fontFixed;
extern CSize g_sizeFixed;

//-----------------------------------------------------------------------------
//
// @mfunc <c CTextViewImpl> constructor.
//
// @parm UINT | nID | Button string id
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CTextViewImpl::CTextViewImpl (UINT nID)
{

    //
    // Initialize variables
    //

    m_nID = nID;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CTextViewImpl> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CTextViewImpl::~CTextViewImpl ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc Initialize the window
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

LRESULT CTextViewImpl::OnCreate (UINT uMsg, 
                                 WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{

    //
    // Create the line array
    //

    char *pachStart = GetData ();
    char *pachEnd = &pachStart [GetSize ()];
    m_nMaxLength = 0;
    while (pachStart < pachEnd)
    {
        char *pachLine = pachStart;
        while (pachStart < pachEnd)
        {
            if (*pachStart == '\r' || *pachStart == '\n')
                break;
            pachStart++;
        }
        {
            TextLine tl;
            tl .pachStart = pachLine;
            tl .nLength = pachStart - pachLine;
            m_asLines .Add (tl);
            if (tl .nLength > m_nMaxLength)
                m_nMaxLength = tl .nLength;
        }
        if (pachStart < pachEnd)
        {
            if (pachStart + 1 < pachEnd &&
                pachStart [0] == '\r' &&
                pachStart [1] == '\n')
                pachStart += 2;
            else
                pachStart++;
        }
    }

    m_sizeTotal .cx = m_nMaxLength;
    m_sizeTotal .cx *= g_sizeFixed .cx;
    m_sizeTotal .cy = (int) m_asLines .GetCount ();
    m_sizeTotal .cy *= g_sizeFixed .cy;
    SetScrollOffset (0, 0, FALSE);
    SetScrollSize (m_sizeTotal);
    SetScrollLine (g_sizeFixed);

    //
    // Return FALSE
    //

    return false;
}

//-----------------------------------------------------------------------------
//
// @mfunc Paint the window
//
// @parm CDCHandle | dc | Destination DC
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CTextViewImpl::DoPaint (CDCHandle dc)
{

    //
    // Initialize the DC
    //

    HFONT hfontOld = dc .SelectFont (g_fontFixed);
    COLORREF clrText = ::GetSysColor (COLOR_WINDOWTEXT);
    COLORREF clrOld = dc .SetTextColor (clrText);

    //
    // Get the offset rect of the window
    //

    CRect rect;
    GetClientRect (&rect);
    rect += m_ptOffset;

    //
    // Compute the starting and ending position 
    //

    int y = rect .top;
    int nLine = m_ptOffset .y / g_sizeFixed .cy;
    int nPartial = m_ptOffset .y % g_sizeFixed .cy;
    y -= nPartial;

    //
    // Display the lines
    //

    while (nLine < m_asLines .GetCount () && y <= rect .bottom)
    {

        //
        // Calc the destination rect
        //

        CRect rect2;
        rect2 .left = 0;
        rect2 .top = y;
        rect2 .right = rect .right;
        rect2 .bottom = rect2 .top + g_sizeFixed .cy;

        //
        // Use ETO to erase the text
        //

        dc .ExtTextOut (rect2 .left, rect2 .top, ETO_OPAQUE,
                        &rect2, _T (""), 0, NULL);

        //
        // Draw the text with tabs
        //

        DRAWTEXTPARAMS dtp;
        dtp .cbSize = sizeof (dtp);
        dtp .iTabLength = 4;
        dtp .iLeftMargin = 0;
        dtp .iRightMargin = 0;
        dc .DrawTextEx (m_asLines [nLine] .pachStart,
                        m_asLines [nLine] .nLength, &rect2, 
                        DT_NOPREFIX | DT_EXPANDTABS  | DT_TABSTOP| DT_LEFT | 
                        DT_TOP, &dtp);

        //
        // Next line
        //

        y += g_sizeFixed .cy;
        nLine++;
    }

    //
    // Fill in the remaining part
    //

    rect .top = y;
    dc .ExtTextOut (rect .left, rect .top, 
                    ETO_OPAQUE, &rect, _T (""), 0, NULL);

    //
    // Restore the DC
    //

    dc .SetTextColor (clrOld);
    dc .SelectFont (hfontOld);
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle a key down event
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

LRESULT CTextViewImpl::OnKeyDown (UINT uMsg, WPARAM wParam, 
                                  LPARAM lParam, BOOL &bHandled)
{

    //
    // If up key
    //

    if (wParam == VK_UP)
    {
        ScrollLineUp ();
    }
    else if (wParam == VK_DOWN)
    {
        ScrollLineDown ();
    }
    else if (wParam == VK_LEFT)
    {
        ScrollLineLeft ();
    }
    else if (wParam == VK_RIGHT)
    {
        ScrollLineRight ();
    }
    else if (wParam == VK_PRIOR)
    {
        ScrollPageUp ();
    }
    else if (wParam == VK_NEXT)
    {
        ScrollPageDown ();
    }
    else if (wParam == VK_HOME)
    {
        ScrollTop ();
    }
    else if (wParam == VK_END)
    {
        ScrollBottom ();
    }
    bHandled = FALSE;
    return 0;
}

/* vim: set sw=4: */
