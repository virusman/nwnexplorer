#ifndef ETS_MODELVIEW_H
#define ETS_MODELVIEW_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ModelView.h - Model view window |
//
// This module contains the definition of the model view window.
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
// $History: ModelView.h $
//      
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include "resource.h"
#include "../_NwnLib/NwnMemoryFile.h"
#include "CommonView.h"
#include "ModelColorDlg.h"
#include "ModelPosDlg.h"
#include "ModelNodeDlg.h"
#include "ModelAnimDlg.h"
#include "ImageWnd.h"
#include "ModelWnd.h"
#include "AtlRollOutCtrl.h"
#include "MdlRtNode.h"

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

class CModelView :
    public CCommonView,
    public CWindowImpl <CModelView>
{

    // @access Construction and destruction
    public:

        // @cmember General constructor

        CModelView (CData *pFile, NwnResType nResType);

        // @cmember General destructor

        ~CModelView ();

        // @access Public virtual method
    public:

        // @cmember Get the support formats/button text

        virtual bool GetModeText (LPTSTR pszText, int cchMaxText)
        {
            bool bModel = (m_nResType == NwnResType_MDL ||
                           m_nResType == NwnResType_DWK ||
                           m_nResType == NwnResType_PWK ||
                           m_nResType == NwnResType_WOK);

            ::LoadString (_Module .GetResourceInstance (), 
                          bModel ? IDS_MODEL : IDS_IMAGE, 
                          pszText, cchMaxText);
            return true;
        }

        // @cmember Get the given window

        virtual CWindow *GetWindow (HWND hWnd)
        {
            if (m_hWnd == NULL)
            {
                Create (hWnd, CWindow::rcDefault, _T (""), 
                        WS_CHILD |  WS_CLIPSIBLINGS, 0, 
                        (HMENU) 0, NULL);
            }
            else if (m_nResType == NwnResType_MDL)
            {
                g_wndModel .ShowModel (m_hWnd, m_sResModel, 
                                       m_nResType, &m_pRtNodeRoot, &m_sModelContext);
            }
            return this;
        }

        // @cmember Destroy the window

        virtual void DestroyTheWindow ()
        {
            if (m_hWnd)
                DestroyWindow ();
        }

        // @cmember Get the compiled model

        CResModelRef *GetCompiledModel ()
        {
            return &m_sResModel;
        }

        // @access ATL window support
    public:

        BEGIN_MSG_MAP (CModelView)
            MESSAGE_HANDLER (WM_CREATE, OnCreate)
            MESSAGE_HANDLER (WM_DESTROY, OnDestroy)
            MESSAGE_HANDLER (WM_ERASEBKGND, OnEraseBackground)
            MESSAGE_HANDLER (WM_SETFOCUS, OnSetFocus)
            MESSAGE_HANDLER (WM_MOUSEACTIVATE, OnMouseActivate)
            MESSAGE_HANDLER (WM_SIZE, OnSize)
            MESSAGE_HANDLER (WM_APP_MDLCTXCHANGED, OnAppModelContextChanged);
            MESSAGE_HANDLER (WM_APP_MDLPOSCHANGED, OnAppModelPositionChanged);
        END_MSG_MAP ()

            // @access ATL Message handlers
    protected:

            // @cmember Handle the creation of the window

            LRESULT OnCreate (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

            // @cmember Handle the destruction of the window

            LRESULT OnDestroy (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
            {
                g_wndModel .UnShowModel (m_hWnd);
                bHandled = FALSE;
                return 0;
            }

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
                int nLeftMargin = 0;
                if (m_RolloutContainer .m_hWnd)
                {
                    CSize size (lParam);
                    CSize sizeClient;
                    m_RolloutContainer .GetClientSize (&sizeClient);
                    m_RolloutContainer .SetWindowPos (NULL,
                                                      0, 0, sizeClient .cx, size .cy,
                                                      SWP_NOACTIVATE | SWP_NOZORDER);
                    nLeftMargin = sizeClient .cx;
                }
                if (m_wndImage .m_hWnd)
                {
                    CSize size (lParam);
                    m_wndImage .SetWindowPos (NULL,
                                              nLeftMargin, 0, size .cx - nLeftMargin,
                                              size .cy, SWP_NOACTIVATE | SWP_NOZORDER);
                }
                if (g_wndModel .m_hWnd && g_wndModel .GetParent () == m_hWnd)
                {
                    CSize size (lParam);
                    g_wndModel .SetWindowPos (NULL,
                                              nLeftMargin, 0, size .cx - nLeftMargin,
                                              size .cy, SWP_NOACTIVATE | SWP_NOZORDER);
                }
                bHandled = FALSE;
                return FALSE;
            }

            // @cmember Reflect focus to the child

            LRESULT OnSetFocus (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
            {
                if (m_wndImage .m_hWnd != NULL)
                    m_wndImage .SetFocus ();
                else if (g_wndModel .m_hWnd != NULL && g_wndModel .GetParent () == m_hWnd)
                    g_wndModel .SetFocus ();
                else
                    bHandled = FALSE;
                return 0;
            }

            // @cmember Handle window activation

            LRESULT OnMouseActivate (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
            {
                LRESULT lRet = DefWindowProc (uMsg, wParam, lParam);
                if (lRet == MA_ACTIVATE || lRet == MA_ACTIVATEANDEAT)
                {
                    if (m_wndImage .m_hWnd != NULL)
                        m_wndImage .SetFocus ();
                    else if (g_wndModel .m_hWnd != NULL && g_wndModel .GetParent () == m_hWnd)
                        g_wndModel .SetFocus ();
                }
                return lRet;
            }

            // @cmember Palette change

            LRESULT OnAppModelContextChanged (UINT uMsg, 
                                              WPARAM wParam, LPARAM lParam, BOOL &bHandled)
            {
                m_wndImage .OnModelContextChanged ();
                g_wndModel .OnModelContextChanged (m_hWnd, TRUE);
                return TRUE;
            }

            // @cmember Position change

            LRESULT OnAppModelPositionChanged (UINT uMsg, 
                                              WPARAM wParam, LPARAM lParam, BOOL &bHandled)
            {
                m_wndImage .OnModelContextChanged ();
                g_wndModel .OnModelContextChanged (m_hWnd, FALSE);
                return TRUE;
            }

            // @cmember Get the model extent

            int GetModelExtent (CMatrix &m, int nOffset, CVector3 &vSum, 
                                CVector3 &vMin, CVector3 &vMax);

            // @access Protected members
    protected:

            // @cmember Pointer to the file

            CDataRef                                    m_sRes;

            // @cmember Pointer to a compiled model file

            CResModelRef                                m_sResModel;

            // @cmember Resource type

            NwnResType                                  m_nResType;

            // @cmember Root node

            CMdlRtNode                                  *m_pRtNodeRoot;

            // @cmember Rollout container

            CRolloutContainerClient             m_RolloutContainer;

            // @cmember Model color dialog

            CRolloutCtrl <CModelColorDlg> m_dlgModelColor;

            // @cmember Model position dialog

            CRolloutCtrl <CModelPosDlg> m_dlgModelPos;

            // @cmember Model node dialog

            CRolloutCtrl <CModelNodeDlg> m_dlgModelNodes;

            // @cmember Model animation dialog

            CRolloutCtrl <CModelAnimDlg> m_dlgModelAnims;

            // @cmember Image view window

            CImageWnd                                   m_wndImage;

            // @cmember Model context

            ModelContext                                m_sModelContext;
};

#endif // ETS_MODELVIEW_H


/* vim: set sw=4: */
