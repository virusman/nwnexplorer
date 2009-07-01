//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ImageWnd.cpp - Image view window |
//
// This module contains the definition of the image view window.
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
// $History: ImageWnd.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "ImageWnd.h"
#include "nwnexplorer.h"

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

extern D3DCOLOR *g_apPalettes [NwnPalette__Count];

//-----------------------------------------------------------------------------
//
// @mfunc <c CImageWnd> constructor.
//
// @parm CData * | pFile | Memory file
//
// @parm NwnResType | nResType | Resource type
//
// @parm ModelContext * | pModelContext | Model context
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CImageWnd::CImageWnd (CData *pFile, NwnResType nResType, 
                      ModelContext *pModelContext)
{

    //
    // Initialize variables
    //

    m_sRes .Set (pFile);
    m_sPlt .Set (pFile);
    m_nResType = nResType;
    m_pModelContext = pModelContext;
    m_pclrPltData = NULL;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CImageWnd> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CImageWnd::~CImageWnd ()
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

LRESULT CImageWnd::OnCreate (UINT uMsg, 
                             WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{

    //
    // OK, this is CRAP CODE.  It doesn't do any error checking
    //

    unsigned char *pauchData = (unsigned char *) m_sRes .GetData ();

    //
    // If we are a bitmap
    //

    if (pauchData [0] == 'B' && pauchData [1] == 'M')
    {
        _ASSERTE (m_nResType == NwnResType_BMP);
        BITMAPFILEHEADER *pfh = (BITMAPFILEHEADER *) pauchData;
        unsigned char *pauchBits = &pauchData [pfh ->bfOffBits];
        BITMAPINFOHEADER *pih = (BITMAPINFOHEADER *) &pfh [1];
        CClientDC dc (m_hWnd);
        BYTE *pDstBits;
        m_bm = ::CreateDIBSection (dc, (BITMAPINFO *) pih, 
                                   DIB_RGB_COLORS, (void **) &pDstBits, 0, 0l);
        if (m_bm .m_hBitmap != NULL)
        {
            memmove (pDstBits, pauchBits, 
                     pih ->biBitCount * pih ->biHeight * pih ->biWidth / 8);
        }
    }

    //
    // If this is a palette
    //

    else if (pauchData [0] == 'P' && pauchData [1] == 'L' &&
             pauchData [2] == 'T' && pauchData [3] == ' ')
    {
        _ASSERTE (m_nResType == NwnResType_PLT);

        //
        // Get the size
        //

        int nWidth = m_sPlt .GetWidth ();
        int nHeight = m_sPlt .GetHeight ();

        //
        // Initialize a DIB
        //

        BITMAPINFOHEADER BMI;
        BMI .biSize = sizeof (BITMAPINFOHEADER);
        BMI .biWidth = nWidth;
        BMI .biHeight = nHeight;
        BMI .biPlanes = 1;
        BMI .biBitCount = 32;
        BMI .biCompression = BI_RGB;
        BMI .biSizeImage = 0;
        BMI .biXPelsPerMeter = 0;
        BMI .biYPelsPerMeter = 0;
        BMI .biClrUsed = 0;
        BMI .biClrImportant = 0;

        //
        // Create DIB section in shared memory
        //

        CClientDC dc (::GetDesktopWindow ());
        m_bm = ::CreateDIBSection (dc, (BITMAPINFO *)&BMI,
                                   DIB_RGB_COLORS, (void **) &m_pclrPltData, 0, 0l);
        m_sizeTotal .cx = nWidth;
        m_sizeTotal .cy = nHeight;
    }

    //
    // Otherwise, we are a tga (We hope - crap code alert)
    //

    else if (m_nResType == NwnResType_DDS)
    {
        m_bm .m_hBitmap = DdsLoad (pauchData);
    }
    else 
    {
        _ASSERTE (m_nResType == NwnResType_TGA);
        m_bm .m_hBitmap = TgaLoad (pauchData);
    }

    //
    // Update size
    //

    if (m_bm .IsNull ())
    {
        m_sizeTotal .cx = 1;
        m_sizeTotal .cy = 1;
    }
    else
    {
        m_sizeTotal = GetBitmapSize (m_bm);
    }
    SetScrollOffset (0, 0, FALSE);
    SetScrollSize (m_sizeTotal);

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

void CImageWnd::DoPaint (CDCHandle dc)
{

    //
    // If we have a bitmap, then draw
    //

    if (!m_bm .IsNull ())
    {
        CDC dcMem;
        dcMem .CreateCompatibleDC (dc);
        HBITMAP hBmpOld = dcMem .SelectBitmap (m_bm);
        dc .BitBlt (0, 0, m_sizeTotal .cx, m_sizeTotal .cy, 
                    dcMem, 0, 0, SRCCOPY);
        dcMem .SelectBitmap (hBmpOld);
    }

    //
    // Get the text color
    //

    COLORREF clrText;
    clrText = ::GetSysColor (COLOR_WINDOWTEXT);
    COLORREF clrOld = dc .SetTextColor (clrText);

    //
    // Get the offset rect of the window
    //

    CRect rect;
    GetClientRect (&rect);
    rect += m_ptOffset;

    //
    // Draw the background if needed
    //

    if (rect .right > m_sizeTotal .cx)
    {
        CRect rect2 (m_sizeTotal .cx, rect .top, rect .right, rect .bottom);
        dc .ExtTextOut (0, 0, ETO_OPAQUE, &rect2, NULL, 0, NULL);
    }
    if (rect .bottom > m_sizeTotal .cy)
    {
        CRect rect2 (rect .left, m_sizeTotal. cy, m_sizeTotal .cx, rect .bottom);
        dc .ExtTextOut (0, 0, ETO_OPAQUE, &rect2, NULL, 0, NULL);
    }

    //
    // Draw some text on error
    //

    if (m_bm .m_hBitmap == NULL)
    {
        CString str;
        str .LoadString (IDS_ERR_IMAGE_LOAD);
        dc .ExtTextOut (0, 0, ETO_OPAQUE, NULL, 
                        str, (UINT) _tcslen (str), NULL);
    }

    //
    // Restore the DC
    //

    dc .SetTextColor (clrOld);
}

//-----------------------------------------------------------------------------
//
// @mfunc Update image
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CImageWnd::OnModelContextChanged ()
{

    //
    // If we have no bitmap, return
    //

    if (m_bm .IsNull ())
        return;

    //
    // If we have not PLT data, then this isn't a plt
    //

    if (m_pclrPltData == NULL)
        return;

    //
    // Loop through the image
    //

    int nCount = m_sizeTotal .cx * m_sizeTotal .cy;
    const CResTexturePltRef::Pixel *p = m_sPlt .GetPixelData ();
    for (int i = 0; i < nCount; i++)
    {
        D3DCOLOR *pData = g_apPalettes [p [i] .ucPalette];
        if (pData == NULL)
        {
            m_pclrPltData [i] = RGB (0, 0, 0);
        }
        else
        {
            pData += (255 - m_pModelContext ->anPalettes [p [i] .ucPalette]) * 256;
            D3DCOLOR clr = pData [p [i] .ucIndex];
            m_pclrPltData [i] = RGB (
                RGBA_GETRED (clr),
                RGBA_GETGREEN (clr),
                RGBA_GETBLUE (clr));
        }
    }

    //
    // If we have a window, repaint
    //

    if (m_hWnd)
        InvalidateRect (NULL);
}


/* vim: set sw=4: */
