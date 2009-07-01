//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ModelWnd.cpp - Model view window |
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
// $History: ModelWnd.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "MainWnd.h"
#include "ModelWnd.h"
#include "nwnexplorer.h"
#include <math.h>
#include "../_MathLib/Constants.h"
#include "../_MathLib/Matrix.h"

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
extern HWND g_hWndMain;
extern BOOL g_fViewModelsBackfaceCull;
extern BOOL g_fViewModelsFullAmbient;
extern BOOL g_fViewModelsMdlNameTex;
extern BOOL g_fViewModelsOrthographic;
extern BOOL g_fViewModelsOutlinePolys;
extern BOOL g_fViewModelsShowAABB;
extern BOOL g_fViewModelsShowDummy;
extern BOOL g_fViewModelsShowEmit;
extern BOOL g_fViewModelsShowLight;
extern BOOL g_fViewModelsShowRoot;
extern DWORD g_nViewModelsBackground;
extern DWORD g_nViewModelsClipDistance;
extern DWORD g_nViewModelsHideFade;
extern DWORD g_nViewModelsOrthoViewSz;
extern DWORD g_nViewModelsViewportX;
extern DWORD g_nViewModelsViewportY;
extern CMainWnd *g_pMainWnd;
CModelWnd g_wndModel;

static const GLfloat s_fNearPlane = 0.5f;
static const GLfloat s_fFOV = 45.0;

//-----------------------------------------------------------------------------
//
// @mfunc <c CModelWnd> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CModelWnd::CModelWnd ()
{

    //
    // Initialize variables
    //

    m_nResType = NwnResType_Unknown;
    m_hgrc = NULL;
    m_pModelContext = NULL;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CModelWnd> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CModelWnd::~CModelWnd ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc Show the window
//
// @parm HWND | hParent | New parent window
//
// @parm CData * | pFile | Memory file
//
// @parm NwnResType | nResType | Resource type
//
// @parm CMdlRtNode ** | ppRtNodeRoot | Root node
//
// @parm ModelContext * | pModelContext | Model context
//
// @parm int * | pasPalette | Texture palette
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelWnd::ShowModel (HWND hParent, CData *pFile, NwnResType nResType, 
                           CMdlRtNode **ppRtNodeRoot, ModelContext *pModelContext)
{

    //
    // Save the model
    //

    m_sRes .Set (pFile);
    m_nResType = nResType;
    m_pRtNodeRoot = *ppRtNodeRoot;

    //
    // Create the window
    //

    m_pModelContext = pModelContext;
    if (m_hWnd == NULL)
    {
        Create (hParent, CWindow::rcDefault, _T (""), 
                WS_CHILD |  WS_CLIPSIBLINGS | WS_VISIBLE, WS_EX_CLIENTEDGE, 
                (HMENU) 0, NULL);
    }
    SetParent (hParent);
    ShowWindow (SW_SHOWNA);

    //
    // Create the root node
    //

    if (m_pRtNodeRoot == NULL && m_sRes .m_p != NULL)
    {
        unsigned char *pauchData = (unsigned char *) m_sRes .GetData ();
        if (pauchData != NULL && pauchData [0] == 0)
        {
            CNwnMdlModel *pHeader = (CNwnMdlModel *) m_sRes .GetModelData  ();
            if (pHeader ->m_pGeometry .GetOffset () != 0)
            {
                m_pRtNodeRoot = CMdlRtNode::CreateNodes (
                    m_sRes, pHeader ->m_pGeometry .GetOffset (), NULL);
                *ppRtNodeRoot = m_pRtNodeRoot;
                wglMakeCurrent (m_dc, m_hgrc);
                m_pRtNodeRoot ->LoadTextures (m_sRes, 
                                              pModelContext ->anPalettes);
                wglMakeCurrent (m_dc, NULL);
            }
        }
    }

    SetTimer (1, 50, NULL);
}

//-----------------------------------------------------------------------------
//
// @mfunc UnShow the window
//
// @parm HWND | hParent | Parent window
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelWnd::UnShowModel (HWND hParent)
{
    if (m_hWnd != NULL && hParent == GetParent ())
    {
        m_pRtNodeRoot = NULL;
        m_sRes .Set (NULL);
        m_nResType = NwnResType_Unknown;
        m_pModelContext = NULL;
        ShowWindow (SW_HIDE);
        SetParent (g_hWndMain);
    }

    KillTimer (1);
}

//-----------------------------------------------------------------------------
//
// @mfunc Unload the textures in a model
//
// @parm CMdlRtNode * | pRtNodeRoot | Root node
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelWnd::UnloadTextures (CMdlRtNode *pRtNodeRoot)
{
    if (m_hWnd) 
    {
        wglMakeCurrent (m_dc, m_hgrc);
        pRtNodeRoot ->UnloadTextures (m_sRes);
        wglMakeCurrent (m_dc, NULL);
    }
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

LRESULT CModelWnd::OnCreate (UINT uMsg, 
                             WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{

    //
    // Get a DC we will use from here on out
    //

    m_dc = GetDC ();

    //
    // Setup the pixel format
    //

    PIXELFORMATDESCRIPTOR pfd;
    memset (&pfd, 0, sizeof (pfd));
    pfd .nSize = sizeof (pfd);
    pfd .nVersion = 1;
    pfd .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd .dwLayerMask = PFD_MAIN_PLANE;
    pfd .iPixelType = PFD_TYPE_RGBA;
    pfd .cColorBits = 16;
    pfd .cDepthBits = 16;
    pfd .cAccumBits = 0;
    pfd .cStencilBits = 0;

    int nPixelFormat = ChoosePixelFormat (m_dc, &pfd);
    if (nPixelFormat != 0)
    {
        if (SetPixelFormat (m_dc, nPixelFormat, &pfd))
        {
            m_hgrc = wglCreateContext (m_dc);
        }
    }

    //
    // Return FALSE
    //

    return false;
}

//-----------------------------------------------------------------------------
//
// @mfunc Destroy the window
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

LRESULT CModelWnd::OnDestroy (UINT uMsg, 
                              WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{

    //
    // Destroy objects
    //

    if (m_hgrc)
        wglDeleteContext (m_hgrc);
    if (m_dc)
        ReleaseDC (m_dc .Detach ());

    //
    // Return FALSE
    //

    bHandled = FALSE;
    return false;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle a paint
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

LRESULT CModelWnd::OnPaint (UINT uMsg, 
                            WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{
    CPaintDC dc (m_hWnd);
    if (m_hgrc)
    {
        CRect rect;
        GetClientRect (&rect);
        wglMakeCurrent (m_dc, m_hgrc);

        //
        // Select the back buffer and clear
        //

        glDrawBuffer (GL_BACK);
        glEnable (GL_DEPTH_TEST);

        if (g_fViewModelsBackfaceCull)
            glEnable (GL_CULL_FACE);
        else
            glDisable (GL_CULL_FACE);

        float fClearR = ((g_nViewModelsBackground >> 16) & 0xFF) / 255.0f;
        float fClearG = ((g_nViewModelsBackground >>  8) & 0xFF) / 255.0f;
        float fClearB = ((g_nViewModelsBackground >>  0) & 0xFF) / 255.0f;

        glClearColor (fClearR, fClearG, fClearB, 1.0);

        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //
        // Set our required states
        //

        glShadeModel (GL_SMOOTH);
        glEnable (GL_LIGHTING);
        glEnable (GL_LIGHT0);
        glEnable (GL_BLEND);
        glEnable (GL_ALPHA_TEST);
        glEnableClientState (GL_VERTEX_ARRAY);
        glEnableClientState (GL_TEXTURE_COORD_ARRAY);
        glDisableClientState (GL_COLOR_ARRAY);
        glDisableClientState (GL_EDGE_FLAG_ARRAY);
        glDisableClientState (GL_INDEX_ARRAY);
        glDisableClientState (GL_NORMAL_ARRAY);
        glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glAlphaFunc (GL_GREATER, 0.1f);


        //
        // Initialize the viewport and projection matrix
        //

        if (g_fViewModelsOrthographic) {
            int nSide = 2, nMax = rect .Width ();

            if (rect .Height() < nMax)
                nMax = rect .Height();

            while (nSide * 2 < 1024 && nSide * 2 < nMax)
                nSide *= 2;

            m_nViewX = nSide;
            m_nViewY = nSide;
            glViewport (0, 0, m_nViewX, m_nViewY);

            glMatrixMode (GL_PROJECTION); 
            glLoadIdentity();

            float fView = g_nViewModelsOrthoViewSz * 0.5;

            if (g_nViewModelsClipDistance < 1)
                g_nViewModelsClipDistance = 128;

            glOrtho(-fView, fView, -fView, fView, s_fNearPlane, g_nViewModelsClipDistance * 1.0f);
        } else {
            if (g_nViewModelsViewportX > 0)
                m_nViewX = g_nViewModelsViewportX;
            else
                m_nViewX = rect .Width ();

            if (g_nViewModelsViewportY > 0)
                m_nViewY = g_nViewModelsViewportY;
            else
                m_nViewY = rect .Height ();

            glViewport (0, 0, m_nViewX, m_nViewY);

            CMatrix m;
            m .MakeIdentity ();
            m .m_m [0] [0] = m_sFrustum .m_vProjection .m_x;
            m .m_m [1] [1] = m_sFrustum .m_vProjection .m_y;
            m .m_m [2] [2] = - m_sFrustum .m_vProjection .m_z;
            m .m_m [3] [3] = 0.0f;
            m .m_m [3] [2] = - m_sFrustum .m_vProjection .m_w;
            m .m_m [2] [3] = -1.0f;
            glMatrixMode (GL_PROJECTION); 
            glLoadMatrixf ((float *) m .m_m);
        }

        //
        // Initialize the basic lighting
        //

        glMatrixMode (GL_MODELVIEW);
        glLoadIdentity ();
        float f [4];

        if (g_fViewModelsFullAmbient) {
            f [0] = 1.0f;
            f [1] = 1.0f;
            f [2] = 1.0f;
            f [3] = 1.0f;
        } else {
            f [0] = 0.0f;
            f [1] = 0.0f;
            f [2] = 0.0f;
            f [3] = 1.0f;
        }

        glLightfv (GL_LIGHT0, GL_AMBIENT, f);
        f [0] = 0.0f;
        f [1] = 0.0f;
        f [2] = 0.0f;
        f [3] = 1.0f;
        glMaterialfv (GL_FRONT, GL_EMISSION, f);
        f [0] = 1.0f;
        f [1] = 1.0f;
        f [2] = 1.0f;
        f [3] = 1.0f;
        glLightfv (GL_LIGHT0, GL_DIFFUSE, f);
        glLightfv (GL_LIGHT0, GL_SPECULAR, f);
        f [0] = -2.0f;
        f [1] = 4.0f;
        f [2] = 10.0f;
        f [3] = 0.0f;
        glLightfv (GL_LIGHT0, GL_POSITION, f);

        //
        // Initialize our model matrix
        //

        CMatrix mWorldToEye;
        mWorldToEye .MakeLookAt (
            CVector3 (0.0, 1.0, 0.0),   // Eye
            CVector3 (0.0, 0.0, 0.0),   // Center
            CVector3 (0.0, 0.0, 1.0));  // Up
        glMatrixMode (GL_MODELVIEW);
        glLoadMatrixf ((float *) mWorldToEye .m_m);
        if (m_pModelContext)
        {
            glTranslatef (
                -m_pModelContext ->vView .m_x, 
                -m_pModelContext ->vView .m_z, 
                -m_pModelContext ->vView .m_y);
            glMultMatrixf ((float *) m_pModelContext ->mView .m_m);
            glTranslatef (
                -m_pModelContext ->vCenterOfRotation .m_x, 
                -m_pModelContext ->vCenterOfRotation .m_y,
                -m_pModelContext ->vCenterOfRotation .m_z); 

            float fYaw, fPitch, fRoll;
            char szText[128];

            if (m_pModelContext ->mView .m_m [1] [0] == 1.0) {
                fYaw   = asin(m_pModelContext ->mView .m_m [1] [0]);
                fRoll  = atan2(m_pModelContext ->mView .m_m [0] [2], m_pModelContext ->mView .m_m [2] [2]);
                fPitch = 0.0;
            } else if (m_pModelContext ->mView .m_m [1] [0] == -1.0) {
                fYaw   = asin(m_pModelContext ->mView .m_m [1] [0]);
                fRoll  = atan2(m_pModelContext ->mView .m_m [0] [2], m_pModelContext ->mView .m_m [2] [2]);
                fRoll  = 0.0;
            } else {
                fYaw   = asin(m_pModelContext ->mView .m_m [1] [0]);
                fPitch = atan2(-(m_pModelContext ->mView .m_m [1] [2]), m_pModelContext ->mView .m_m [1] [1]);
                fRoll  = atan2(-(m_pModelContext ->mView .m_m [2] [0]), m_pModelContext ->mView .m_m [0] [0]);
            }

            fYaw   = (fYaw   * 180.0) / PI;
            fPitch = (fPitch * 180.0) / PI;
            fRoll  = (fRoll  * 180.0) / PI;

            while (fYaw < 0.0)
                fYaw += 360.0;
            fYaw = fmodf(fYaw, 360.0);
            while (fPitch < 0.0)
                fPitch += 360.0;
            fPitch = fmodf(fPitch, 360.0);
            while (fRoll < 0.0)
                fRoll += 360.0;
            fRoll = fmodf(fRoll, 360.0);

            snprintf(szText, _countof(szText), "Render [X %.2f, Y %.2f, Z %.2f] [Y %.1f, P %.1f, R %.1f] %d",
                (m_pModelContext ->vView .m_x == 0.0 ? 0.0 : -m_pModelContext ->vView .m_x), 
                (m_pModelContext ->vView .m_y == 0.0 ? 0.0 : -m_pModelContext ->vView .m_y),
                m_pModelContext ->vView .m_z, 
                fYaw, fPitch, fRoll);

            g_pMainWnd ->SetStatusBarText(szText);
        }

        //
        // Dump the data
        //

        if (m_pRtNodeRoot) {
            DWORD nFlags = g_nViewModelsHideFade & MDL_RENDER_HIDE_FADE;

            if (m_nResType == NwnResType_DWK)
                nFlags |= MDL_RENDER_IS_DWK;
            else if (m_nResType == NwnResType_PWK)
                nFlags |= MDL_RENDER_IS_PWK;
            else if (m_nResType == NwnResType_WOK)
                nFlags |= MDL_RENDER_IS_WOK;

            if (g_fViewModelsOutlinePolys)
                nFlags |= MDL_RENDER_TRANSLUCENT;
            if (g_fViewModelsOrthographic)
                nFlags |= MDL_RENDER_ORTHOGRAPHIC;

            /* change AABB rendering to not be mesh-based later */
            if (g_fViewModelsShowAABB)
                nFlags |= MDL_RENDER_SHOW_AABB;
            
            m_pRtNodeRoot ->Render (m_sRes, 0.0, nFlags | MDL_RENDER_SHOW_MESH);

#if 0
            if (g_fViewModelsShowAABB)
                m_pRtNodeRoot ->Render (m_sRes, 0.0, nFlags | MDL_RENDER_SHOW_AABB);
#endif

            if (g_fViewModelsOutlinePolys)
                m_pRtNodeRoot ->Render (m_sRes, 0.0, MDL_RENDER_SHOW_MESH | MDL_RENDER_OUTLINE_POLYS);

            if (g_fViewModelsShowDummy)
                m_pRtNodeRoot ->Render (m_sRes, 0.0, MDL_RENDER_SHOW_DUMMY);

            if (g_fViewModelsShowEmit)
                m_pRtNodeRoot ->Render (m_sRes, 0.0, MDL_RENDER_SHOW_EMITTER);

            if (g_fViewModelsShowLight)
                m_pRtNodeRoot ->Render (m_sRes, 0.0, MDL_RENDER_SHOW_LIGHT);
        }

        //
        // Swap buffers and end
        //

        SwapBuffers (m_dc);
        wglMakeCurrent (m_dc, NULL);
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle a size
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

LRESULT CModelWnd::OnSize (UINT uMsg, 
                           WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{
    if (m_hgrc != NULL)
    {
        if (g_nViewModelsClipDistance < 1)
            g_nViewModelsClipDistance = 128;

        CSize size (lParam);
        m_sFrustum .SetFrustum (s_fFOV, s_fNearPlane, 
                                g_nViewModelsClipDistance * 1.0,
                                size .cx, size .cy);
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Update image
//
// @parm HWND | hParent | Parent window
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelWnd::OnModelContextChanged (HWND hParent, BOOL bLoadTextures)
{

    //
    // Update the textures
    //

    if (m_pRtNodeRoot != NULL && m_sRes .m_p != NULL)
    {
        wglMakeCurrent (m_dc, m_hgrc);
        if (bLoadTextures)
            m_pRtNodeRoot ->LoadTextures (m_sRes, 
                                          m_pModelContext ->anPalettes);
        wglMakeCurrent (m_dc, NULL);
    }

    //
    // If we have a window, repaint
    //

    if (m_hWnd && GetParent () == hParent)
        InvalidateRect (NULL);
}

//-----------------------------------------------------------------------------
//
// @mfunc Get a screenshot of the current model display
//
// @parm int * | pnWidth | Width of the screenshot
//
// @parm int * | pnHeight | Height of the screenshot
//
// @rdesc The screenshot data
//
//-----------------------------------------------------------------------------

D3DCOLOR *CModelWnd::GetImage (int *pnWidth, int *pnHeight)
{
    BOOL bHandled;
    int x, y, yoff, p;
    CSize sz;
    CRect rect;

    OnPaint(0, 0, 0, bHandled);

    GetClientRect (&rect);
    RedrawWindow (&rect, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ERASENOW | RDW_UPDATENOW | RDW_INTERNALPAINT);

    MSG msg;
    while (::PeekMessage( &msg, NULL, 0, 0, PM_REMOVE))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }

    D3DCOLOR *pData = new D3DCOLOR [m_nViewX * m_nViewY];
    if (pData == NULL)
        return NULL;

    yoff = rect.Height () - m_nViewY - 1;

    if (pnWidth != NULL)
        *pnWidth = m_nViewX;
    if (pnHeight != NULL)
        *pnHeight = m_nViewY;

    for (x = 0; x < m_nViewX; x++) {
        for (y = 1; y <= m_nViewY; y++) {
            p = ((m_nViewY - y) * m_nViewX) + x;

            pData[p] = RGBA_SETALPHA(m_dc .GetPixel (x, y + yoff), 255);
        }
    }

    return pData;
}


/* vim: set sw=4: */
