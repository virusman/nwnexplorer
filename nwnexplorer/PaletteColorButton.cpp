//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      PaletteColorButton.cpp - Color button and popup |
//
// This module contains the definition of color button and popup
//
// Copyright (c) 2000-2002 - Descartes Systems Sciences, Inc.
//
// Based on work by Chris Maunder, Alexander Bischofberger and James White.
//
// http://www.codetools.com/miscctrl/colorbutton.asp
// http://www.codetools.com/miscctrl/colour_picker.asp
//
// Copyright (c) 2000-2002 - Descartes Systems Sciences, Inc.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are 
// met:
// 
// 1. Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
// 2. Neither the name of Descartes Systems Sciences, Inc nor the names of 
//    its contributors may be used to endorse or promote products derived 
//    from this software without specific prior written permission.
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
// --- ORIGINAL COPYRIGHT STATEMENT ---
//
// Written by Chris Maunder (chrismaunder@codeguru.com)
// Extended by Alexander Bischofberger (bischofb@informatik.tu-muenchen.de)
// Copyright (c) 1998.
//
// Updated 30 May 1998 to allow any number of colours, and to
//                     make the appearance closer to Office 97. 
//                     Also added "Default" text area.         (CJM)
//
//         13 June 1998 Fixed change of focus bug (CJM)
//         30 June 1998 Fixed bug caused by focus bug fix (D'oh!!)
//                      Solution suggested by Paul Wilkerson.
//
// ColourPopup is a helper class for the colour picker control
// CColourPicker. Check out the header file or the accompanying 
// HTML doc file for details.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Expect bugs.
// 
// Please use and enjoy. Please let me know of any bugs/mods/improvements 
// that you have found/implemented and I will fix/incorporate them into this
// file. 
//
// @end
//
// $History: PaletteColorButton.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "PaletteColorButton.h"

//
// Debug NEW
//

#if defined (_DEBUG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// Sizing constants
//

const int g_ciArrowSizeX = 4;
const int g_ciArrowSizeY = 2;

//
// Other definitions
//

#define INVALID_COLOR           -1

//
// Sizing definitions
//

static const CSize s_sizeBoxHiBorder (2, 2);
static const CSize s_sizeBoxMargin (0, 0);
static const CSize s_sizeBoxCore (12, 12);
static const int s_nNumColors = 256;
static const int s_nNumColumns = 16;
static const int s_nNumRows = 16;

//-----------------------------------------------------------------------------
//
// @mfunc <c CPaletteColorButton> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CPaletteColorButton::CPaletteColorButton () : m_wndPicker (this, 1)
{
    m_nCurrent = 0;
    m_fPopupActive = FALSE;
    m_fTrackSelection = FALSE;
    m_fMouseOver = FALSE;
    m_nBitmapID = 0;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CPaletteColorButton> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CPaletteColorButton::~CPaletteColorButton ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc Subclass the control
//
// @parm HWND | hWnd | Handle of the window to be subclassed
// 
// @rdesc Return value
//
//              @flag TRUE | Window was subclassed
//              @flag FALSE | Window was not subclassed
//
//-----------------------------------------------------------------------------

BOOL CPaletteColorButton::SubclassWindow (HWND hWnd)
{
    CWindowImpl <CPaletteColorButton>::SubclassWindow (hWnd);
    ModifyStyle (0, BS_OWNERDRAW);
#if !defined (COLORBUTTON_NOTHEMES)
    OpenThemeData (L"Button");
#endif
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle key press
//
// @parm WORD | wNotifyCode | Command notification code
// 
// @parm WORD | wID | ID of the control
//
// @parm HWND | hWndCtl | Handle of the control
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CPaletteColorButton::OnClicked (WORD wNotifyCode, 
                                        WORD wID, HWND hWndCtl, BOOL &bHandled) 
{
    UNUSED_ALWAYS (bHandled);
    UNUSED_ALWAYS (hWndCtl);
    UNUSED_ALWAYS (wID);
    UNUSED_ALWAYS (wNotifyCode);

    //
    // Mark button as active and invalidate button to force a redraw
    //

    m_fPopupActive = TRUE;
    InvalidateRect (NULL);

    //
    // Send the drop down notification to the parent
    //

    SendNotification (CPN_DROPDOWN, m_nCurrent, TRUE); 

    //
    // Save the current color for future reference
    //

    int nOldColor = m_nCurrent;

    //
    // Display the popup
    //

    BOOL fOked = Picker ();

    //
    // Cancel the popup
    //

    m_fPopupActive = FALSE;

    //
    // If the popup was canceled without a selection
    //

    if (!fOked)
    {

        //
        // If we are tracking, restore the old selection
        //

        if (m_fTrackSelection)
        {
            if (nOldColor != m_nCurrent)
            {
                m_nCurrent = nOldColor;
                SendNotification (CPN_SELCHANGE, m_nCurrent, TRUE); 
            }
        }
        SendNotification (CPN_CLOSEUP, m_nCurrent, TRUE); 
        SendNotification (CPN_SELENDCANCEL, m_nCurrent, TRUE); 
    }
    else
    {
        if (nOldColor != m_nCurrent)
        {
            SendNotification (CPN_SELCHANGE, m_nCurrent, TRUE); 
        }
        SendNotification (CPN_CLOSEUP, m_nCurrent, TRUE); 
        SendNotification (CPN_SELENDOK, m_nCurrent, TRUE); 
    }

    //
    // Invalidate button to force repaint
    //

    InvalidateRect (NULL);
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle mouse move
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

LRESULT CPaletteColorButton::OnMouseMove (UINT uMsg, WPARAM wParam, 
                                          LPARAM lParam, BOOL &bHandled) 
{
    UNUSED_ALWAYS (lParam);
    UNUSED_ALWAYS (wParam);
    UNUSED_ALWAYS (uMsg);

    if (!m_fMouseOver)
    {
        m_fMouseOver = TRUE;
        TRACKMOUSEEVENT tme;
        tme .cbSize = sizeof (tme);
        tme .dwFlags = TME_LEAVE;
        tme .hwndTrack = m_hWnd;
        _TrackMouseEvent (&tme);
        InvalidateRect (NULL);
    }
    bHandled = FALSE;
    return FALSE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle mouse leave
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

LRESULT CPaletteColorButton::OnMouseLeave (UINT uMsg, WPARAM wParam, 
                                           LPARAM lParam, BOOL &bHandled) 
{
    UNUSED_ALWAYS (lParam);
    UNUSED_ALWAYS (wParam);
    UNUSED_ALWAYS (uMsg);

    if (m_fMouseOver)
    {
        m_fMouseOver = FALSE;
        InvalidateRect (NULL);
    }
    bHandled = FALSE;
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

LRESULT CPaletteColorButton::OnDrawItem (UINT uMsg, WPARAM wParam, 
                                         LPARAM lParam, BOOL &bHandled) 
{
    UNUSED_ALWAYS (bHandled);
    UNUSED_ALWAYS (wParam);
    UNUSED_ALWAYS (uMsg);

    LPDRAWITEMSTRUCT lpItem = (LPDRAWITEMSTRUCT) lParam;
    CDC dc (lpItem ->hDC);

    //
    // Get data about the request
    //

    UINT uState = lpItem ->itemState;
    CRect rcDraw = lpItem ->rcItem;

    //
    // If we have a theme
    //

    m_fPopupActive = false;
#if !defined (COLORBUTTON_NOTHEMES)
    if (m_hTheme != NULL)
    {

        //
        // Draw the outer edge
        //

        UINT uFrameState = 0;
        if ((uState & ODS_SELECTED) != 0 || m_fPopupActive)
            uFrameState |= PBS_PRESSED;
        if ((uState & ODS_DISABLED) != 0)
            uFrameState |= PBS_DISABLED;
        if ((uState & ODS_HOTLIGHT) != 0 || m_fMouseOver)
            uFrameState |= PBS_HOT;
        DrawThemeBackground (dc, BP_PUSHBUTTON, 
                             uFrameState, &rcDraw, NULL);
        GetThemeBackgroundContentRect (dc, BP_PUSHBUTTON, 
                                       uFrameState, &rcDraw, &rcDraw);
    }

    //
    // Otherwise, we are old school
    //

    else
#endif
    {
        //
        // Draw the outer edge
        //

        UINT uFrameState = DFCS_BUTTONPUSH | DFCS_ADJUSTRECT;
        if ((uState & ODS_SELECTED) != 0 || m_fPopupActive)
            uFrameState |= DFCS_PUSHED;
        if ((uState & ODS_DISABLED) != 0)
            uFrameState |= DFCS_INACTIVE;
        dc .DrawFrameControl (&rcDraw, DFC_BUTTON, uFrameState);

        //
        // Adjust the position if we are selected (gives a 3d look)
        //

        if ((uState & ODS_SELECTED) != 0 || m_fPopupActive)
            rcDraw .OffsetRect (1, 1);
    }

    //
    // Draw focus
    //

    if ((uState & ODS_FOCUS) != 0 || m_fPopupActive) 
    {
        CRect rcFocus (rcDraw.left, rcDraw.top, 
                       rcDraw.right - 1, rcDraw.bottom);
        dc .DrawFocusRect(&rcFocus);
    }
    rcDraw .InflateRect (
        - ::GetSystemMetrics(SM_CXEDGE),
        - ::GetSystemMetrics(SM_CYEDGE));

    //
    // Draw the arrow
    //

    {
        CRect rcArrow;
        rcArrow .left   = rcDraw. right - g_ciArrowSizeX - ::GetSystemMetrics (SM_CXEDGE) / 2;
        rcArrow .top    = (rcDraw.bottom + rcDraw.top)/2 - g_ciArrowSizeY / 2;
        rcArrow .right  = rcArrow.left + g_ciArrowSizeX;
        rcArrow .bottom = (rcDraw .bottom + rcDraw .top) / 2 + g_ciArrowSizeY / 2;

        DrawArrow (dc, rcArrow, 0, 
                   (uState & ODS_DISABLED) ? ::GetSysColor (COLOR_GRAYTEXT) : RGB (0,0,0));

        rcDraw.right = rcArrow.left - ::GetSystemMetrics (SM_CXEDGE) / 2;
    }

    //
    // Draw separator
    //

    dc .DrawEdge (&rcDraw, EDGE_ETCHED, BF_RIGHT);
    rcDraw.right -= (::GetSystemMetrics (SM_CXEDGE) * 2) + 1 ;

    //
    // Get the text
    //

    dc .SetBkMode (TRANSPARENT);
    TCHAR szText [256];
    GetWindowText (szText, _countof (szText));
    if ((uState & ODS_DISABLED) == 0)
    {
        dc .DrawText (szText, (int) _tcslen (szText), &rcDraw, 0);
    }
    else
    {
        dc .GrayString (NULL, NULL, (LPARAM) szText, (int) _tcslen (szText), 
                        rcDraw .left, rcDraw .top, rcDraw .Width (), rcDraw .Height ());
    }

    //
    // Draw color
    //

    if ((uState & ODS_DISABLED) == 0)
    {
        if (!m_bitmap .IsNull () && m_nCurrent != INVALID_COLOR)
        {
            int x = (m_nCurrent % s_nNumColumns) * 16;
            int y = (m_nCurrent / s_nNumColumns) * 16;
            CDC dcMem;
            dcMem .CreateCompatibleDC (dc);
            HBITMAP hBmpOld = dcMem .SelectBitmap (m_bitmap);
            dc .BitBlt (rcDraw .right - 12, 
                        (rcDraw .top + rcDraw .bottom) / 2 - 6,
                        12, 12, dcMem, x + 2, y + 2, SRCCOPY);
            dcMem .SelectBitmap (hBmpOld);
        }
    }
    return 1;
}

//-----------------------------------------------------------------------------
//
// @mfunc Draw the arrow of the button
//
// @parm CDC & | dc | Destination DC
//
// @parm const RECT & | rect | Rectangle of the control
//
// @parm int | iDirection | Direction
//
// @parm COLORREF | clrArrow | Color to draw the arrow.
//
// @rdesc None
//
//-----------------------------------------------------------------------------

void CPaletteColorButton::DrawArrow (CDC &dc, const RECT &rect, 
                                     int iDirection, COLORREF clrArrow)
{
    POINT ptsArrow[3];

    switch (iDirection)
    {
        case 0 : // Down
            {
                ptsArrow [0] .x = rect .left;
                ptsArrow [0] .y = rect .top;
                ptsArrow [1] .x = rect .right;
                ptsArrow [1] .y = rect .top;
                ptsArrow [2] .x = (rect .left + rect .right) / 2;
                ptsArrow [2] .y = rect .bottom;
                break;
            }

        case 1 : // Up
            {
                ptsArrow [0] .x = rect .left;
                ptsArrow [0] .y = rect .bottom;
                ptsArrow [1] .x = rect .right;
                ptsArrow [1] .y = rect .bottom;
                ptsArrow [2] .x = (rect .left + rect .right) / 2;
                ptsArrow [2] .y = rect .top;
                break;
            }

        case 2 : // Left
            {
                ptsArrow [0] .x = rect .right;
                ptsArrow [0] .y = rect .top;
                ptsArrow [1] .x = rect .right;
                ptsArrow [1] .y = rect .bottom;
                ptsArrow [2] .x = rect .left;
                ptsArrow [2] .y = (rect .top + rect .bottom) / 2;
                break;
            }

        case 3 : // Right
            {
                ptsArrow [0] .x = rect .left;
                ptsArrow [0] .y = rect .top;
                ptsArrow [1] .x = rect .left;
                ptsArrow [1] .y = rect .bottom;
                ptsArrow [2] .x = rect .right;
                ptsArrow [2] .y = (rect .top + rect .bottom) / 2;
                break;
            }
    }

    CBrush brArrow;
    brArrow .CreateSolidBrush (clrArrow);
    CPen penArrow;
    penArrow .CreatePen (PS_SOLID, 0, clrArrow);

    HBRUSH hbrOld = dc .SelectBrush (brArrow);
    HPEN hpenOld = dc .SelectPen (penArrow);

    dc .SetPolyFillMode (WINDING);
    dc .Polygon (ptsArrow, 3);

    dc .SelectBrush (hbrOld);
    dc .SelectPen (hpenOld);
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Display the picker popup
//
// @rdesc Return value
//
//              @parm TRUE | A new color was selected
//              @parm FALSE | The user canceled the picket
//
//-----------------------------------------------------------------------------

BOOL CPaletteColorButton::Picker ()
{
    BOOL fOked = FALSE;

    //
    // See what version we are using
    //

    OSVERSIONINFO osvi;
    osvi .dwOSVersionInfoSize = sizeof (osvi);
    ::GetVersionEx (&osvi);
    bool fIsXP = osvi .dwPlatformId == VER_PLATFORM_WIN32_NT &&
        (osvi .dwMajorVersion > 5 || (osvi .dwMajorVersion == 5 &&
                                      osvi .dwMinorVersion >= 1));

    //
    // Get the flat flag
    //

    m_fPickerFlat = FALSE;
    if (fIsXP)
        ::SystemParametersInfo (SPI_GETFLATMENU, 0, &m_fPickerFlat, FALSE);

    //
    // Get all the colors I need
    //

    int nAlpha = 48;
    m_clrBackground = ::GetSysColor (COLOR_MENU);
    m_clrHiLightBorder = ::GetSysColor (COLOR_HIGHLIGHT);
    m_clrHiLight = fIsXP ? ::GetSysColor (COLOR_MENUHILIGHT) : m_clrHiLightBorder;
    m_clrHiLightText = ::GetSysColor (COLOR_HIGHLIGHTTEXT);
    m_clrText = ::GetSysColor (COLOR_MENUTEXT);
    m_clrLoLight = RGB (
        (GetRValue (m_clrBackground) * (255 - nAlpha) + 
         GetRValue (m_clrHiLightBorder) * nAlpha) >> 8,
        (GetGValue (m_clrBackground) * (255 - nAlpha) + 
         GetGValue (m_clrHiLightBorder) * nAlpha) >> 8,
        (GetBValue (m_clrBackground) * (255 - nAlpha) + 
         GetBValue (m_clrHiLightBorder) * nAlpha) >> 8);

    //
    // Get the margins
    //

    m_rectMargins .left = ::GetSystemMetrics (SM_CXEDGE);
    m_rectMargins .top = ::GetSystemMetrics (SM_CYEDGE);
    m_rectMargins .right = ::GetSystemMetrics (SM_CXEDGE);
    m_rectMargins .bottom = ::GetSystemMetrics (SM_CYEDGE);

    //
    // Initialize our state
    // 

    m_nCurrentSel       = INVALID_COLOR;
    m_nChosenColorSel   = INVALID_COLOR;
    m_nPicker                   = m_nCurrent;

    //
    // Register the window class used for the picker
    //

    WNDCLASSEX wc;
    wc .cbSize = sizeof (WNDCLASSEX);
    wc .style  = CS_CLASSDC | CS_SAVEBITS | CS_HREDRAW | CS_VREDRAW;
    wc .lpfnWndProc = CContainedWindow::StartWindowProc;
    wc .cbClsExtra  = 0;
    wc .cbWndExtra = 0;
    wc .hInstance = _Module .GetModuleInstance ();
    wc .hIcon = NULL;
    wc .hCursor = LoadCursor (NULL, IDC_ARROW);
    wc .hbrBackground = (HBRUSH) (COLOR_MENU + 1);
    wc .lpszMenuName = NULL;
    wc .lpszClassName = _T ("ColorPicker");
    wc .hIconSm = NULL;
    if (fIsXP)
    {
        BOOL fDropShadow;
        ::SystemParametersInfo (SPI_GETDROPSHADOW, 0, &fDropShadow, FALSE);
        if (fDropShadow)
            wc .style |= CS_DROPSHADOW;
    }
    ATOM atom = ::RegisterClassEx (&wc);

    //
    // Create the window
    //

    CRect rcButton;
    GetWindowRect (&rcButton);
    _Module .AddCreateWndData (&m_wndPicker .m_thunk .cd, &m_wndPicker);
    m_wndPicker .m_hWnd = ::CreateWindowEx (0, MAKEINTATOM (atom), 
                                            _T (""),  WS_POPUP, rcButton .left, rcButton .bottom, 100, 100,
                                            GetParent (), NULL, _Module .GetModuleInstance(), NULL);

    //
    // If we created the window
    //

    if (m_wndPicker .m_hWnd != NULL)
    {

        //
        // Set the window size
        //

        SetPickerWindowSize ();

        //
        // Find which cell (if any) corresponds to the initial color
        //

        if (m_nCurrent >= 0 || m_nCurrent < s_nNumColumns)
            m_nChosenColorSel = m_nCurrent;
        else
            m_nChosenColorSel = INVALID_COLOR;

        //
        // Make visible
        //

        m_wndPicker .ShowWindow (SW_SHOWNA);

        //
        // Purge the message queue of paints
        //

        MSG msg;
        while (::PeekMessage(&msg, NULL, WM_PAINT, WM_PAINT, PM_NOREMOVE))
        {
            if (!GetMessage(&msg, NULL, WM_PAINT, WM_PAINT))
                return FALSE;
            DispatchMessage(&msg);
        }

        // 
        // Set capture to the window which received this message
        //

        m_wndPicker .SetCapture ();
        _ASSERTE (m_wndPicker .m_hWnd == ::GetCapture ());

        //
        // Get messages until capture lost or cancelled/accepted
        //

        while (m_wndPicker .m_hWnd == ::GetCapture ())
        {
            MSG msg;
            if (!::GetMessage(&msg, NULL, 0, 0))
            {
                ::PostQuitMessage (msg .wParam);
                break;
            }

            switch (msg.message)
            {
                case WM_LBUTTONUP:
                    {
                        BOOL bHandled = TRUE;
                        OnPickerLButtonUp (msg .message, 
                                           msg .wParam, msg .lParam, bHandled);
                    }
                    break;

                case WM_MOUSEMOVE:
                    {
                        BOOL bHandled = TRUE;
                        OnPickerMouseMove (msg .message, 
                                           msg .wParam, msg .lParam, bHandled);
                    }
                    break;

                case WM_KEYUP:
                    break;

                case WM_KEYDOWN:
                    {
                        BOOL bHandled = TRUE;
                        OnPickerKeyDown (msg .message, 
                                         msg .wParam, msg .lParam, bHandled);
                    }
                    break;

                case WM_RBUTTONDOWN:
                    ::ReleaseCapture ();
                    m_fOked = FALSE;
                    break;

                    // just dispatch rest of the messages
                default:
                    DispatchMessage (&msg);
                    break;
            }
        }
        ::ReleaseCapture ();
        fOked = m_fOked;

        //
        // Destroy the window
        //

        m_wndPicker .DestroyWindow ();

        //
        // If needed, show custom
        //

        if (fOked)
        {
            m_nCurrent = m_nPicker;
        }
    }

    //
    // Unregister our class
    //

    ::UnregisterClass (MAKEINTATOM (atom), _Module .GetModuleInstance());
    return fOked;
}

//-----------------------------------------------------------------------------
//
// @mfunc Set the window size of the picker control
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CPaletteColorButton::SetPickerWindowSize ()
{
    CSize szText (0, 0);

    //
    // Initiailize our box size
    //

    _ASSERTE (s_sizeBoxHiBorder .cx == s_sizeBoxHiBorder .cy);
    _ASSERTE (s_sizeBoxMargin .cx == s_sizeBoxMargin .cy);
    m_sizeBox .cx = s_sizeBoxCore .cx + (s_sizeBoxHiBorder .cx + s_sizeBoxMargin .cx) * 2;
    m_sizeBox .cy = s_sizeBoxCore .cy + (s_sizeBoxHiBorder .cy + s_sizeBoxMargin .cy) * 2;

    //
    // Compute the min width
    //

    int nBoxTotalWidth = s_nNumColumns * m_sizeBox .cx;
    int nMinWidth = nBoxTotalWidth;
    if (nMinWidth < szText .cx)
        nMinWidth = szText .cx;

    //
    // Initialize the color box rectangle
    //

    m_rectBoxes = CRect (
        CPoint ((nMinWidth - nBoxTotalWidth) / 2, 0), 
        CSize (nBoxTotalWidth, s_nNumRows * m_sizeBox .cy)
        );

    //
    // Get the current window position, and set the new size
    //

    CRect rectWindow (m_rectBoxes);
    CRect rect;
    m_wndPicker .GetWindowRect (&rect);
    rectWindow .OffsetRect (rect .TopLeft ());

    //
    // Adjust the rects for the border
    //

    rectWindow .right += m_rectMargins .left + m_rectMargins .right;
    rectWindow .bottom += m_rectMargins .top + m_rectMargins .bottom;
    ::OffsetRect (&m_rectBoxes, m_rectMargins .left, m_rectMargins .top);

    //
    // Get the screen rectangle
    //

    CRect rectScreen (CPoint (0, 0), CSize (
            ::GetSystemMetrics (SM_CXSCREEN),
            ::GetSystemMetrics (SM_CYSCREEN)));
    HMODULE hUser32 = ::GetModuleHandleA ("USER32.DLL");
    if (hUser32 != NULL)
    {
        typedef HMONITOR (WINAPI *FN_MonitorFromWindow) (HWND hWnd, DWORD dwFlags);
        typedef BOOL (WINAPI *FN_GetMonitorInfo) (HMONITOR hMonitor, LPMONITORINFO lpmi);
        FN_MonitorFromWindow pfnMonitorFromWindow = (FN_MonitorFromWindow)
            ::GetProcAddress (hUser32, "MonitorFromWindow");
        FN_GetMonitorInfo pfnGetMonitorInfo = (FN_GetMonitorInfo)
            ::GetProcAddress (hUser32, "GetMonitorInfoA");
        if (pfnMonitorFromWindow != NULL && pfnGetMonitorInfo != NULL)
        {
            MONITORINFO mi;
            HMONITOR hMonitor = pfnMonitorFromWindow (m_hWnd, 
                                                      MONITOR_DEFAULTTONEAREST);
            mi .cbSize = sizeof (mi);
            pfnGetMonitorInfo (hMonitor, &mi);
            rectScreen = mi .rcWork;
        }
    }

    //
    // Need to check it'll fit on screen: Too far right?
    //

    if (rectWindow .right > rectScreen .right)
        ::OffsetRect (&rectWindow, rectScreen .right - rectWindow .right, 0);

    //
    // Too far left?
    //

    if (rectWindow .left < rectScreen .left)
        ::OffsetRect (&rectWindow, rectScreen .left - rectWindow .left, 0);

    //
    // Bottom falling out of screen?  If so, the move
    // the whole popup above the parents window
    //

    if (rectWindow .bottom > rectScreen .bottom)
    {
        CRect rcParent;
        GetWindowRect (&rcParent);
        ::OffsetRect (&rectWindow, 0,
                      - ((rcParent .bottom - rcParent .top) + 
                         (rectWindow .bottom - rectWindow .top)));
    }

    //
    // Set the window size and position
    //

    m_wndPicker .MoveWindow (&rectWindow, TRUE);
}

//-----------------------------------------------------------------------------
//
// @mfunc Gets the dimensions of the colour cell given by (row,col)
//
// @parm int | nIndex | Index of the cell
//
// @parm RECT * | pRect | Rectangle of the cell
//
// @rdesc Return value.
//
//              @flag TRUE | If the index is valid
//              @flag FALSE | If the index is not valid
//
//-----------------------------------------------------------------------------

BOOL CPaletteColorButton::GetPickerCellRect (int nIndex, RECT *pRect) const
{

    //
    // Validate the range
    //

    if (nIndex < 0 || nIndex >= s_nNumColors)
        return FALSE;

    //
    // Compute the value of the boxes
    //

    pRect ->left = (nIndex % s_nNumColumns) * m_sizeBox .cx + m_rectBoxes .left;
    pRect ->top  = (nIndex / s_nNumColumns) * m_sizeBox .cy + m_rectBoxes .top;
    pRect ->right = pRect ->left + m_sizeBox .cx;
    pRect ->bottom = pRect ->top + m_sizeBox .cy;
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Change the current selection
//
// @parm int | nIndex | New selection
//
// @rdesc None
//
//-----------------------------------------------------------------------------

void CPaletteColorButton::ChangePickerSelection (int nIndex)
{
    CRect rect;

    //
    // If the current selection is valid, redraw old selection with out
    // it being selected
    //

    if (m_nCurrentSel >= 0 && m_nCurrentSel < s_nNumColors)
    {
        int nOldSel = m_nCurrentSel;
        m_nCurrentSel = INVALID_COLOR;
        GetPickerCellRect (nOldSel, &rect);
        m_wndPicker .InvalidateRect (&rect);
    }

    //
    // Set the current selection as row/col and draw (it will be drawn selected)
    //

    m_nCurrentSel = nIndex;
    m_nPicker = nIndex;
    GetPickerCellRect (m_nCurrentSel, &rect);
    m_wndPicker .InvalidateRect (&rect);

    //
    // Send the message
    //

    if (m_fTrackSelection)
    {
        m_nCurrent = m_nCurrentSel;
        InvalidateRect (NULL);
        SendNotification (CPN_SELCHANGE, m_nCurrentSel, 
                          m_nCurrentSel != INVALID_COLOR); 
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc End the selection
//
// @parm BOOL | fOked | If TRUE, the user has selected a new color.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CPaletteColorButton::EndPickerSelection (BOOL fOked)
{
    ::ReleaseCapture ();
    m_fOked = fOked;
}

//-----------------------------------------------------------------------------
//
// @mfunc Set the chosen color from the given color
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

LRESULT CPaletteColorButton::OnPickerKeyDown (UINT uMsg, 
                                              WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{
    UNUSED_ALWAYS (lParam);
    UNUSED_ALWAYS (uMsg);

    //
    // Get the key data
    //

    UINT nChar = wParam;

    //
    // Get the offset for movement
    //

    int nOffset = 0;
    switch (nChar)
    {
        case VK_DOWN:
            nOffset = s_nNumColumns;
            break;

        case VK_UP:
            nOffset = -s_nNumColumns;
            break;

        case VK_RIGHT:
            nOffset = 1;
            break;

        case VK_LEFT:
            nOffset = -1;
            break;

        case VK_ESCAPE:
            m_nPicker = m_nCurrent;
            EndPickerSelection (FALSE);
            break;

        case VK_RETURN:
        case VK_SPACE:
            if (m_nCurrentSel == INVALID_COLOR)
                m_nPicker = m_nCurrent;
            EndPickerSelection (m_nCurrentSel != INVALID_COLOR);
            break;
    }

    //
    // If we have an offset
    //

    if (nOffset != 0)
    {

        //
        // Based on our current position, compute a new position
        //

        int nNewSel;
        if (m_nCurrentSel == INVALID_COLOR)
            nNewSel = nOffset > 0 ? 0 : s_nNumColors;
        else
        {
            nNewSel = m_nCurrentSel + nOffset;
            if (nNewSel < 0)
                nNewSel += s_nNumColors;
            else if (nNewSel >= s_nNumColors)
                nNewSel -= s_nNumColors;
        }

        //
        // Set the new location
        //

        ChangePickerSelection (nNewSel);
    }
    bHandled = FALSE;
    return FALSE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle a button up event
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

LRESULT CPaletteColorButton::OnPickerLButtonUp (UINT uMsg, 
                                                WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{
    UNUSED_ALWAYS (bHandled);
    UNUSED_ALWAYS (wParam);
    UNUSED_ALWAYS (uMsg);

    //
    // Where did the button come up at?
    //

    CPoint pt (GET_X_LPARAM (lParam), GET_Y_LPARAM (lParam));
    int nNewSelection = PickerHitTest (pt);

    //
    // If valid, then change selection and end
    //

    if (nNewSelection != m_nCurrentSel)
        ChangePickerSelection (nNewSelection);
    EndPickerSelection (nNewSelection != INVALID_COLOR);
    return 0;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle mouse move
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

LRESULT CPaletteColorButton::OnPickerMouseMove (UINT uMsg, WPARAM wParam, 
                                                LPARAM lParam, BOOL &bHandled) 
{
    UNUSED_ALWAYS (bHandled);
    UNUSED_ALWAYS (wParam);
    UNUSED_ALWAYS (uMsg);

    //
    // Do a hit test
    //

    CPoint pt (GET_X_LPARAM (lParam), GET_Y_LPARAM (lParam));
    int nNewSelection = PickerHitTest (pt);

    //
    // Update
    //

    if (nNewSelection != m_nCurrentSel)
        ChangePickerSelection (nNewSelection);
    return 0;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle a paint event
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

LRESULT CPaletteColorButton::OnPickerPaint (UINT uMsg, 
                                            WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{
    UNUSED_ALWAYS (bHandled);
    UNUSED_ALWAYS (lParam);
    UNUSED_ALWAYS (wParam);
    UNUSED_ALWAYS (uMsg);

    CPaintDC dc (m_wndPicker);

    //
    // Draw raised window edge (ex-window style WS_EX_WINDOWEDGE is sposed to do this,
    // but for some reason isn't
    //

    CRect rect;
    m_wndPicker .GetClientRect (&rect);
    if (m_fPickerFlat)
    {
        CPen pen;
        pen .CreatePen (PS_SOLID, 0, ::GetSysColor (COLOR_GRAYTEXT));
        HPEN hpenOld = dc .SelectPen (pen);
        dc .Rectangle (rect .left, rect .top, 
                       rect .Width (), rect .Height ());
        dc .SelectPen (hpenOld);
    }
    else
    {
        dc .DrawEdge (&rect, EDGE_RAISED, BF_RECT);
    }

    //
    // Draw colour cells
    // 

    if (!m_bitmap .IsNull ())
    {
        CDC dcMem;
        dcMem .CreateCompatibleDC (dc);
        HBITMAP hBmpOld = dcMem .SelectBitmap (m_bitmap);
        dc .BitBlt (m_rectBoxes .left, m_rectBoxes .top,
                    m_rectBoxes .Width (), m_rectBoxes .Height (),
                    dcMem, 0, 0, SRCCOPY);
        dcMem .SelectBitmap (hBmpOld);
    }

    //
    // IF we have selections
    //

    if (m_nCurrentSel != INVALID_COLOR)
    {
        CBrush br;
        br .CreateSolidBrush (RGB (0, 255, 0));
        CRect rect;
        GetPickerCellRect (m_nCurrentSel, &rect);
        DrawHighlight (dc, br, rect, 2);
    }
    if (m_nChosenColorSel != INVALID_COLOR &&
        m_nChosenColorSel != m_nCurrentSel)
    {
        CBrush br;
        br .CreateSolidBrush (RGB (255, 0, 0));
        CRect rect;
        GetPickerCellRect (m_nChosenColorSel, &rect);
        DrawHighlight (dc, br, rect, 2);
    }
    return 0;
}

//-----------------------------------------------------------------------------
//
// @mfunc Draw a highlight
//
// @parm CDC & | dc | Device context
//
// @parm CBrush & | br | Brush
//
// @parm CRect & | rect | Rectangle
//
// @parm int | nWidth | Width
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CPaletteColorButton::DrawHighlight (CDC &dc, 
                                         CBrush &br, CRect &rect, int nWidth) 
{
    for (int i = 0; i < nWidth; i++)
    {
        dc .FrameRect (&rect, br);
        rect .InflateRect (-1, -1);
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle palette query for picker
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

LRESULT CPaletteColorButton::OnPickerQueryNewPalette (UINT uMsg, 
                                                      WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{
    UNUSED_ALWAYS (bHandled);
    Invalidate ();
    return DefWindowProc (uMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle palette change for picker
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

LRESULT CPaletteColorButton::OnPickerPaletteChanged (UINT uMsg, 
                                                     WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{
    UNUSED_ALWAYS (bHandled);
    LRESULT lResult = DefWindowProc (uMsg, wParam, lParam);
    if ((HWND) wParam != m_hWnd)
        Invalidate ();
    return lResult;
}

//-----------------------------------------------------------------------------
//
// @mfunc Send notification message
//
// @parm UINT | nCode | Notification code
//
// @parm int | nIndex | Color index
//
// @parm BOOL | fColorValid | If true, the color is a valid color.
//
// @rdesc None
//
//-----------------------------------------------------------------------------

void CPaletteColorButton::SendNotification (
    UINT nCode, int nIndex, BOOL fColorValid)
{
    NMPALETTECOLORBUTTON nmclr;

    nmclr .hdr .code = nCode;
    nmclr .hdr .hwndFrom = m_hWnd;
    nmclr .hdr .idFrom = GetDlgCtrlID ();
    nmclr .fColorValid = fColorValid;
    nmclr .nIndex = nIndex;

    ::SendMessage (GetParent (), WM_NOTIFY, 
                   (WPARAM) GetDlgCtrlID (), (LPARAM) &nmclr);
}

//-----------------------------------------------------------------------------
//
// @mfunc Do a hit test
//
// @parm const POINT & | pt | Point inside the window
//
// @rdesc Index/Item over or INVALID_COLOR
//
//-----------------------------------------------------------------------------

int CPaletteColorButton::PickerHitTest (const POINT &pt)
{

    //
    // If the point isn't in the boxes, return invalid color
    //

    if (!m_rectBoxes .PtInRect (pt))
        return INVALID_COLOR;

    //
    // Convert the point to an index
    //

    int nRow = (pt .y - m_rectBoxes .top) / m_sizeBox .cy;
    int nCol = (pt .x - m_rectBoxes .left) / m_sizeBox .cx;
    if (nRow < 0 || nRow >= s_nNumRows || nCol < 0 || nCol >= s_nNumColumns)
        return INVALID_COLOR;
    int nIndex = nRow * s_nNumColumns + nCol;
    if (nIndex >= s_nNumColors)
        return INVALID_COLOR;
    return nIndex;
}


/* vim: set sw=4: */
