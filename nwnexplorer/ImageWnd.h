#ifndef ETS_IMAGEWND_H
#define ETS_IMAGEWND_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ImageWnd.h - Image window |
//
// This module contains the definition of the image window.
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
// $History: HexView.h $
//      
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include "resource.h"
#include "nwnexplorer.h"
#include "DataRef.h"

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

class CImageWnd :
    public CScrollWindowImpl <CImageWnd>
{

    // @access Construction and destruction
    public:
        DECLARE_WND_CLASS_EX(NULL, 0, -1)

            // @cmember General constructor

            CImageWnd (CData *pFile, NwnResType nResType, 
                       ModelContext *pModelContext);

        // @cmember General destructor

        ~CImageWnd ();

        // @access Public methods
    public:

        // @cmember Update the image with the palette

        void OnModelContextChanged ();

        void WriteTga (FILE *fp);

        // @access ATL window support
    public:

        BEGIN_MSG_MAP (CImageWnd)
            MESSAGE_HANDLER (WM_CREATE, OnCreate)
            MESSAGE_HANDLER (WM_ERASEBKGND, OnEraseBackground)
            MESSAGE_HANDLER (WM_SIZE, OnSize)
            CHAIN_MSG_MAP (CScrollWindowImpl <CImageWnd>);
        END_MSG_MAP ()

            // @access ATL Message handlers
    protected:

            // @cmember Handle the creation of the window

            LRESULT OnCreate (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // @cmember Don't erase the background

            LRESULT OnEraseBackground (UINT uMsg, WPARAM wParam, 
                                       LPARAM lParam, BOOL &bHandled)
            {
                return 1;
            }

            // @cmember Handle the size

            LRESULT OnSize (UINT uMsg, WPARAM wParam, 
                            LPARAM lParam, BOOL &bHandled)
            {
                LRESULT lResult = CScrollWindowImpl <CImageWnd>::
                    OnSize (uMsg, wParam, lParam, bHandled);
                SetScrollPage (m_sizeClient);
                return lResult;
            }

            // @access ATL overrides
    public:

            // @cmember Paint the window

            void DoPaint (CDCHandle dc);

            // @access Protected members
    protected:

            // @cmember Pointer to the file

            CDataRef                    m_sRes;

            // @cmember Texture PLT 

            CResTexturePltRef           m_sPlt;

            // @cmember Resource type

            NwnResType                          m_nResType;

            // @cmember Pointer to the context

            ModelContext                        *m_pModelContext;

            // @cmember pointer to the PLT bits

            COLORREF                            *m_pclrPltData;

            // @cmember Total size of the document

            CSize                                       m_sizeTotal;

            // @cmember Bitmap

            CBitmap                                     m_bm;
};

#endif // ETS_IMAGEWND_H

/* vim: set sw=4: */
