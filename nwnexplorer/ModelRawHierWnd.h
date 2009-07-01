#ifndef ETS_MODELRAWHIERWND_H
#define ETS_MODELRAWHIERWND_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ModelRawHierWnd.h - Model raw hierarchy window |
//
// This module contains the definition of the model raw hierarchy window.
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
#include "DataRef.h"

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

class CModelRawHierView;

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CModelRawHierWnd : 
    public CWindowImpl <CModelRawHierWnd>,
    public CCustomDraw <CModelRawHierWnd>
{

    // @access Construction and destruction
    public:

        // @cmember General constructor

        CModelRawHierWnd (CModelRawHierView *pParent, CData *pFile);

        // @cmember General destructor

        ~CModelRawHierWnd ();

        // @access public methods
    public:

        CTreeViewCtrl &GetTreeView ()
        {
            return m_tv;
        }

        // @access ATL window support
    public:

        BEGIN_MSG_MAP (CModelRawHierWnd)
            MESSAGE_HANDLER (WM_CREATE, OnCreate)
            CHAIN_MSG_MAP (CCustomDraw <CModelRawHierWnd>)
            MESSAGE_HANDLER (WM_ERASEBKGND, OnEraseBackground)
            MESSAGE_HANDLER (WM_SIZE, OnSize)
            MESSAGE_HANDLER (WM_SETFOCUS, OnSetFocus)

            NOTIFY_HANDLER (IDC_EXP_HEADER, HDN_ITEMCHANGED, OnHDItemChanged)
            NOTIFY_HANDLER (IDC_EXP_TREEVIEW, TVN_GETDISPINFO, OnTVGetDispInfo)
            NOTIFY_HANDLER (IDC_EXP_TREEVIEW, TVN_ITEMEXPANDING, OnTVItemExpanding)
            NOTIFY_HANDLER (IDC_EXP_TREEVIEW, TVN_SELCHANGED, OnTVSelChanged)
            //NOTIFY_HANDLER (IDC_EXP_TREEVIEW, NM_DBLCLK, OnNMDblClk);
            END_MSG_MAP ()

            // @access ATL Message handlers
    public:

            // @cmember Handle the creation of the window

            LRESULT OnCreate (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // @cmember Handle a size request

            LRESULT OnSize (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
            {
                CRect rectClient;
                GetClientRect (&rectClient);
                WINDOWPOS wps;
                HDLAYOUT hdl;
                hdl .prc = &rectClient;
                hdl .pwpos = &wps;
                m_header .Layout (&hdl);
                m_header .SetWindowPos (wps .hwndInsertAfter, wps .x, wps .y,
                                        wps .cx, wps .cy, wps .flags);
                m_tv .SetWindowPos (NULL, rectClient .left, rectClient .top,
                                    rectClient .Width (), rectClient .Height (), 
                                    SWP_NOZORDER | SWP_NOACTIVATE);
                return TRUE;
            }

            // @cmember Don't erase the background

            LRESULT OnEraseBackground (UINT uMsg, WPARAM wParam, 
                                       LPARAM lParam, BOOL &bHandled)
            {
                return 1;
            }

            // @cmember Reflect focus to the child

            LRESULT OnSetFocus (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
            {
                if (m_tv .m_hWnd)
                    m_tv .SetFocus ();
                else
                    bHandled = FALSE;
                return 0;
            }

            // @cmember Get the display information

            LRESULT OnTVGetDispInfo (int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

            // @cmember Handle item expansion

            LRESULT OnTVItemExpanding (int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

            // @cmember Handle selection change

            LRESULT OnTVSelChanged (int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

            // @cmember Handle header change

            LRESULT OnHDItemChanged (int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
            {
                m_tv .InvalidateRect (NULL);
                return TRUE;
            }

            DWORD OnPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
            {
                if( lpNMCustomDraw->hdr.hwndFrom != m_tv ) return CDRF_DODEFAULT;
                return CDRF_NOTIFYITEMDRAW;   // We need per-item notifications
            }

            DWORD OnItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
            {
                if( lpNMCustomDraw->hdr.hwndFrom != m_tv ) return CDRF_DODEFAULT;

                LPNMTVCUSTOMDRAW pCustomDraw = reinterpret_cast<LPNMTVCUSTOMDRAW>(lpNMCustomDraw);
                pCustomDraw->clrText = GetSysColor (COLOR_WINDOW);
                pCustomDraw->clrTextBk = GetSysColor (COLOR_WINDOW);

                // Reset the focus because it will be drawn by us
                m_iItemState = lpNMCustomDraw->uItemState;
                lpNMCustomDraw->uItemState &= ~(CDIS_FOCUS|CDIS_SELECTED);

                // Remember the drawing rectangle of the item so we can draw it ourselves
                m_tv.GetItemRect( (HTREEITEM)lpNMCustomDraw->dwItemSpec, &m_rcItem, TRUE);
                //      m_rcItem.right = (lpNMCustomDraw->rc.right > m_cxHeader) ? pCustomDraw->nmcd.rc.right : m_cxHeader;

                return CDRF_NOTIFYPOSTPAINT;   // We need more notifications
            }

            DWORD OnItemPostPaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
            {
                if( lpNMCustomDraw->hdr.hwndFrom != m_tv ) return CDRF_DODEFAULT;
                DrawTreeItem (lpNMCustomDraw->hdc, (HTREEITEM)lpNMCustomDraw->dwItemSpec, m_iItemState, m_rcItem);
                return CDRF_DODEFAULT;
            }

            // @access Protected methods
    protected:

            // @cmember Add an item

            HTREEITEM AddToTV (int nOffset, HTREEITEM hParent);

            // @cmember Get resource text

            void GetElementText (int nOffset, LPTSTR pszText, 
                                 int cchMaxText, int nColumn);

            // @cmember Add children

            void AddChildren (int nOffset, HTREEITEM hParent);

            // @cmember Draw an item

            void DrawTreeItem (HDC hDC, HTREEITEM hItem, UINT iState, RECT &rcItem);

            // @access Protected members
    protected:

            // @cmember Pointer to the file

            CResModelRef                        m_sRes;

            // @cmember Treeview control

            CTreeViewCtrl                       m_tv;

            // @cmember Header control

            CHeaderCtrl                         m_header;

            // @cmember Parent view

            CModelRawHierView           *m_pParent;

            // @cmember Scratch TV strings

            char                                        m_szTVText [256];

            // @cmember Scratch item state

            int                                         m_iItemState;

            // @cmember Scratch item rect

            CRect                                       m_rcItem;
};

#endif // ETS_MODELRAWHIERWND_H


/* vim: set sw=4: */
