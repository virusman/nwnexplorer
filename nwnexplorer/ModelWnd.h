#ifndef ETS_MODELWND_H
#define ETS_MODELWND_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ModelWnd.h - Model window |
//
// This module contains the definition of the model window.
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
#include "../_MathLib/Frustum.h"
#include "../_MathLib/Matrix.h"
#include "MdlRtNode.h"
#include <gl/gl.h>

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

class CModelWnd :
    public CWindowImpl <CModelWnd>
{

    // @access Construction and destruction
    public:

        // @cmember General constructor

        CModelWnd ();

        // @cmember General destructor

        ~CModelWnd ();

        // @access Public methods
    public:

        // @cmember Update the image with the given palette

        void OnModelContextChanged (HWND hParent, BOOL bLoadTextures);

        // @cmember Attach the window to this parent

        void ShowModel (HWND hParent, CData *pFile, NwnResType nResType, 
                        CMdlRtNode **ppRtNodeRoot, ModelContext *pModelContext);

        // @cmember unattach the window

        void UnShowModel (HWND hParent);

        // @cmember Unload textures in a model

        void UnloadTextures (CMdlRtNode *pRtNodeRoot);

        // @cmember Get a screenshot of the model window

        D3DCOLOR *GetImage (int *pnWidth, int *pnHeight);

        // @access ATL window support
    public:

        BEGIN_MSG_MAP (CModelWnd)
            MESSAGE_HANDLER (WM_CREATE, OnCreate)
            MESSAGE_HANDLER (WM_PAINT, OnPaint)
            MESSAGE_HANDLER (WM_ERASEBKGND, OnEraseBackground)
            MESSAGE_HANDLER (WM_SIZE, OnSize)
            MESSAGE_HANDLER (WM_TIMER, OnTimer)
            MESSAGE_HANDLER (WM_DESTROY, OnDestroy);
        MESSAGE_HANDLER (WM_LBUTTONDOWN, OnLRButtonDown)
            MESSAGE_HANDLER (WM_LBUTTONUP, OnLRButtonUp)
            MESSAGE_HANDLER (WM_RBUTTONDOWN, OnLRButtonDown)
            MESSAGE_HANDLER (WM_RBUTTONUP, OnLRButtonUp)
            MESSAGE_HANDLER (WM_MOUSEMOVE, OnMouseMove)
            MESSAGE_HANDLER (WM_MOUSEWHEEL, OnMouseWheel)
            //MESSAGE_HANDLER (WM_CANCELMODE, OnCancelMode)
            END_MSG_MAP ()

            // @access ATL Message handlers
    protected:

            // @cmember Handle the creation of the window

            LRESULT OnCreate (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // @cmember Handle the destruction of the window

            LRESULT OnDestroy (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // @cmember Handle the painting of the window

            LRESULT OnPaint (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // @cmember Handle timer

            LRESULT OnTimer (UINT uMsg, 
                             WPARAM wParam, LPARAM lParam, BOOL &bHandled)
            {
                ::SendMessage (GetParent (), 
                               WM_APP_MDLPOSCHANGED, 0, 0);
                return TRUE;
            }

            // @cmember Don't erase the background

            LRESULT OnEraseBackground (UINT uMsg, WPARAM wParam, 
                                       LPARAM lParam, BOOL &bHandled)
            {
                return 1;
            }

            // @cmember Handle the size

            LRESULT OnSize (UINT uMsg, WPARAM wParam, 
                            LPARAM lParam, BOOL &bHandled);

            // @cmember Handle left or right mouse button down

            LRESULT OnLRButtonDown (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
            {
                if (::GetCapture () != m_hWnd )
                {
                    ::SetCapture (m_hWnd);
                    m_ptMouse = CPoint (lParam);
                    m_sModelMouse = m_pModelContext ->mView;
                }
                return 0;
            }

            // @cmember Handle left or right mouse button up

            LRESULT OnLRButtonUp (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
            {
                if (::GetCapture () == m_hWnd)
                {
                    ::ReleaseCapture ();
                }
                return 0;
            }

            // @cmember Handle a mouse move

            LRESULT OnMouseMove (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
            {
                if (::GetCapture () == m_hWnd)
                {
                    CPoint pt (lParam);
                    if ((wParam & MK_LBUTTON) != 0)
                    {
                        CRect rect;
                        GetClientRect (&rect);
                        float fScale = m_pModelContext ->vView .m_z;
                        if (fScale <= 0.5f)
                            fScale = 0.5f;
                        fScale *= 1.4f;
                        m_pModelContext ->vView .m_x += ((pt .x - m_ptMouse .x) / 
                                                         float (rect .Width ())) * fScale;
                        m_pModelContext ->vView .m_y += ((pt .y - m_ptMouse .y) / 
                                                         float (rect .Height ())) * fScale;
                        InvalidateRect (NULL);
                        m_ptMouse = pt;
                    }
                    else if ((wParam & MK_RBUTTON) != 0)
                    {
                        pt -= m_ptMouse;
                        if (pt .x > 16)
                            pt .x -= 16;
                        else if (pt .x < -16)
                            pt .x += 16;
                        else
                            pt .x = 0;
                        if (pt .y > 16)
                            pt .y -= 16;
                        else if (pt .y < -16)
                            pt .y += 16;
                        else
                            pt .y = 0;
                        CMatrix m;
                        m .MakeHPR ((float) pt .x, 0.0f, (float) - pt .y);
                        m_pModelContext ->mView = m * m_sModelMouse;
                        InvalidateRect (NULL);
                    }
                }
                return 0;
            }

            // @cmember Handle a mouse wheel

            LRESULT OnMouseWheel (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
            {
                float fDelta = (float) ((short) HIWORD (wParam)) / (float (WHEEL_DELTA) * 2.0f);
                if ((wParam & MK_MBUTTON) != 0)
                    fDelta *= 2.0f;
                m_pModelContext ->vView .m_z -= fDelta;
                if (m_pModelContext ->vView .m_z < 0.0f)
                    m_pModelContext ->vView .m_z = 0.0f;
                InvalidateRect (NULL);
                return 0;
            }

            // @access Protected members
    protected:

            // @cmember X and Y size of the viewport

            int                     m_nViewX, m_nViewY;

            // @cmember Pointer to the file

            CResModelRef                        m_sRes;

            // @cmember Resource type

            NwnResType                          m_nResType;

            // @cmember Model context

            ModelContext                        *m_pModelContext;

            // @cmember DC

            CDC                                         m_dc;

            // @cmember OpenGL context

            HGLRC                                       m_hgrc;

            // @cmember Drawing frustum

            CFrustum                            m_sFrustum;

            // @cmember Last mouse position

            CPoint                                      m_ptMouse;

            // @cmember Rotation matrix at mouse move start

            CMatrix                                     m_sModelMouse;

            // @cmember Root node

            CMdlRtNode                          *m_pRtNodeRoot;
};

extern CModelWnd g_wndModel;

#endif // ETS_MODELWND_H

/* vim: set sw=4: */
