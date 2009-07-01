//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      HexView.cpp - Hex view window |
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
// $History: HexView.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "HexView.h"

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
// @mfunc <c CHexView> constructor.
//
// @parm CData * | pFile | Memory file
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CHexView::CHexView (CData *pFile)
{

    //
    // Initialize variables
    //

    m_sRes .Set (pFile);
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CHexView> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CHexView::~CHexView ()
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

LRESULT CHexView::OnCreate (UINT uMsg, 
                            WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{
    m_sizeTotal .cx = (9 + (16 * 3) + 16);
    m_sizeTotal .cx *= g_sizeFixed .cx;
    m_sizeTotal .cy = (m_sRes .GetSize () + 15) / 16;
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

void CHexView::DoPaint (CDCHandle dc)
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
    const unsigned char *pauchData = m_sRes .GetData ();
    const unsigned char *pauchStart = pauchData;
    const unsigned char *pauchEnd = &pauchStart [m_sRes .GetSize ()];
    int nLine = m_ptOffset .y / g_sizeFixed .cy;
    int nPartial = m_ptOffset .y % g_sizeFixed .cy;
    y -= nPartial;
    pauchStart += 16 * nLine;

    //
    // Display the lines
    //

    while (pauchStart < pauchEnd && y <= rect .bottom)
    {
        TCHAR szBuffer [80 + 1];
        RECT rect2;

        //
        // Format the line of data
        //

        _stprintf (szBuffer, "%08X ", pauchStart - pauchData);
        LPTSTR psz = &szBuffer [_tcslen (szBuffer)];
        for (int i = 0; i < 16; i++)
        {
            if (&pauchStart [i] < pauchEnd)
            {
                _stprintf (psz, "%02X ", pauchStart [i]);
                psz += 3;
            }
            else
            {
                *psz++ = ' ';
                *psz++ = ' ';
                *psz++ = ' ';
            }
        }
        for (int i = 0; i < 16; i++)
        {
            if (&pauchStart [i] < pauchEnd && 
                pauchStart [i] >= ' ' && pauchStart [i] < 127)
                *psz++ = (char) pauchStart [i];
            else
                *psz++ = ' ';
        }
        *psz++ = 0;


        //
        // Draw the dwAddress in the buffer
        //

        rect2 .left = rect .left;
        rect2 .top = y;
        rect2 .right = rect .right;
        rect2 .bottom = rect2 .top + g_sizeFixed .cy;
        dc .ExtTextOut (0, rect2 .top, ETO_OPAQUE, 
                        &rect2, szBuffer, (UINT) _tcslen (szBuffer), NULL);
        y += g_sizeFixed .cy;
        pauchStart += 16;
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

LRESULT CHexView::OnKeyDown (UINT uMsg, WPARAM wParam, 
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
