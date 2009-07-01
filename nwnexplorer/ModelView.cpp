//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ModelView.cpp - Model view window |
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
// $History: ModelView.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "ModelView.h"
#include "ModelWnd.h"
#include "../_MathLib/Vector2.h"
#include "../_NmcLib/Nmc.h"

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

extern BOOL g_fViewModelsShowAABB;
extern BOOL g_fViewModelsOrthographic;
extern DWORD g_nViewModelsViewAngleY;
extern DWORD g_nViewModelsViewAngleP;
extern DWORD g_nViewModelsViewAngleR;
extern DWORD g_nViewModelsViewDistance;
extern D3DCOLOR *g_apPalettes [NwnPalette__Count];
extern CNwnKeyFile *g_sKeyFiles;
extern CDataCache g_sResourceCache;

//-----------------------------------------------------------------------------
//
// @mfunc <c CModelView> constructor.
//
// @parm CData * | pFile | Memory file
//
// @parm NwnResType | nResType | Resource type
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CModelView::CModelView (CData *pFile, NwnResType nResType) : 
    m_wndImage (pFile, nResType, &m_sModelContext)
{

    //
    // Initialize variables
    //

    m_sRes .Set (pFile);
    m_nResType = nResType;
    for (int i = 0; i < _countof (m_sModelContext .anPalettes); i++)
        m_sModelContext .anPalettes [i] = 0;
    m_sModelContext .mView .MakeIdentity ();
    m_sModelContext .vView .m_x = 0.0f;
    m_sModelContext .vView .m_y = 0.0f;
    m_sModelContext .vView .m_z = 10.0f;
    m_dlgModelColor .m_Child .m_pModelContext = &m_sModelContext;
    m_dlgModelPos .m_Child .m_pModelContext = &m_sModelContext;
    m_pRtNodeRoot = NULL;

    //
    // Compile the model if need be
    //

    if (m_sRes .m_p != NULL && (nResType == NwnResType_MDL || nResType == NwnResType_WOK || nResType == NwnResType_PWK || nResType == NwnResType_DWK))
    {
        unsigned char *pauchData = (unsigned char *) m_sRes .GetData ();
        if (pauchData != NULL && pauchData [0] != 0)
        {

            //
            // Compile
            //

            size_t nSize = m_sRes .GetSize();

            if (nResType != NwnResType_MDL) {
                int nExtra = 0;
                char szName[64];
                char *p, *pauchCopy = (char *) calloc (1, m_sRes .GetSize() + 2048);

                if (nResType == NwnResType_WOK) {
                    memcpy(pauchCopy, pauchData, nSize);
                    pauchCopy[nSize] = 0;

                    if ((p = strstr(pauchCopy, "beginwalkmeshgeom ")) != NULL && (p == pauchCopy || isspace(*(p - 1)))) {
                        p += 18;
                        while (isspace(*p))
                            p++;

                        strncpy(szName, p, sizeof(szName));
                        szName[sizeof(szName) - 1] = 0;

                        if ((p = strpbrk(szName, " \t\r\n")) != NULL)
                            *p = 0;

                        nExtra = sprintf(pauchCopy, "newmodel %s\r\n"
                                                    "setsupermodel %s NULL\r\n"
                                                    "classification TILE\r\n"
                                                    "beginmodelgeom %s\r\n"
                                                    "node dummy %s\r\nparent NULL\r\nendnode\r\n",
                                                    szName, szName, szName, szName);
                        memcpy(pauchCopy + nExtra, pauchData, nSize);
                        pauchCopy[nExtra + nSize] = 0;

                        if ((p = strstr(pauchCopy, "endwalkmeshgeom")) != NULL) {
                            nSize = p - pauchCopy;
                            nSize += sprintf(p, "endmodelgeom %s\r\ndonemodel %s\r\n", szName, szName);
                        } else
                            memcpy(pauchCopy, pauchData, nSize);
                    }
                } else {
                    memcpy(pauchCopy, pauchData, nSize);
                    pauchCopy[nSize] = 0;

                    if ((p = strstr(pauchCopy, "parent ")) != NULL && isspace(*(p - 1))) {
                        p += 7;
                        while (isspace(*p))
                            p++;

                        strncpy(szName, p, sizeof(szName));
                        szName[sizeof(szName) - 1] = 0;

                        if ((p = strpbrk(szName, " \t\r\n")) != NULL)
                            *p = 0;

                        nExtra = sprintf(pauchCopy, "newmodel ArtificialModel\r\n"
                                                    "setsupermodel ArtificialModel NULL\r\n"
                                                    "classification %s\r\n"
                                                    "setanimationscale 1\r\n"
                                                    "beginmodelgeom ArtificialModel\r\n"
                                                    "node dummy ArtificialModel\r\nparent NULL\r\nendnode\r\n"
                                                    "node dummy %s\r\nparent ArtificialModel\r\nendnode\r\n",
                                                    (nResType == NwnResType_DWK ? "DOOR" : "CHARACTER"),
                                                    szName);
                        memcpy(pauchCopy + nExtra, pauchData, nSize);
                        nExtra += sprintf(pauchCopy + nExtra + nSize, "endmodelgeom ArtificialModel\r\ndonemodel ArtificialModel\r\n");
                        nSize  += nExtra;
                    }
                }

                pauchData = (unsigned char *)pauchCopy;
            }

            CNmcCache sCache;
            CNmcContext sCtx;

            for (int i = IDS_TOTAL_KEY_FILES - 1; i >= 0; i--)
                sCtx .AddKeyFile (&g_sKeyFiles[i]);

            sCtx .SetCache (&sCache);
            sCtx .SetPurgeNullFaces (true);
            CNwnMemoryStream *pStream = new CNwnMemoryStream (
                pFile ->m_strResName, pauchData, nSize, false);
            sCtx .AddStream (pStream);
            NmcParseModelFile (&sCtx);

            //
            // If we have an error, return
            //

            if (sCtx .GetErrors () > 0)
            {
                //FIXME
                //printf ("Compilation aborted with errors\n");
            }

            //
            // Make sure we have a model
            //

            else if (sCtx .GetCurrentModel () == NULL)
            {
                //FIXME
                //printf ("Error: File \"%s\" doesn't contain a valid model\n", 
                //      pszInFile);
            }

            //
            // Otherwise, we have a good model
            //

            else
            {

                //
                // Create the output
                //

                CNwnMemoryStream sModelStream;
                CNwnMemoryStream sRawStream;
                sCtx .GetCurrentModel () ->Serialize (
                    &sModelStream, &sRawStream);

                //
                // Create the new data
                //

                UINT32 nModelDataSize = (UINT32) sModelStream .GetLength ();
                UINT32 nRawDataSize = (UINT32) sRawStream .GetLength ();
                UINT32 ulSize = sizeof (CNwnModel::Header) + nModelDataSize + 
                    nRawDataSize;
                unsigned char *pauchData = (unsigned char *) calloc (1, ulSize);
                CNwnModel::Header *phdr = (CNwnModel::Header *) pauchData;
                phdr ->ulZero = 0;
                phdr ->ulRawDataOffset = nModelDataSize;
                phdr ->ulRawDataSize = nRawDataSize;
                memcpy (&pauchData [sizeof (CNwnModel::Header)],
                        sModelStream .GetData (), nModelDataSize);
                memcpy (&pauchData [sizeof (CNwnModel::Header) + nModelDataSize],
                        sRawStream .GetData (), nRawDataSize);

                //
                // Create the model object
                //

                m_sResModel .Set (new CData (&g_sResourceCache, 
                                             pFile ->m_pKeyFile, pFile ->m_pModuleFile, 
                                             pFile ->m_pBifFile, pFile ->m_strResName, NwnResType_MDL,
                                             CDataKey::Type_BinaryModel, NULL, 0, pauchData, ulSize, 
                                             true));
            }

            //
            // Free the model
            //

            if (sCtx .GetCurrentModel ())
                delete sCtx .GetCurrentModel ();

            if (nResType != NwnResType_MDL)
                delete pauchData;
        }
        else
            m_sResModel .Set (m_sRes .m_p);
    }

    //
    // Compute the bmin/bmax/average for the vertices
    //

    CVector3 vSum (0, 0, 0);
    CVector3 vMin (FLT_MAX, FLT_MAX, FLT_MAX);
    CVector3 vMax (FLT_MIN, FLT_MIN, FLT_MIN);
    int nVerts = 0;
    BOOL bTile = false;

    if (m_sResModel .m_p != NULL)
    {
        CNwnMdlModel *pHeader = (CNwnMdlModel *) m_sResModel .GetModelData  ();

        if (pHeader ->m_ucClassification == NwnMdlCls_Tile)
            bTile = true;
        else if (pHeader ->m_pGeometry .GetOffset () != 0)
        {
            CMatrix m;
            m .MakeIdentity ();
            nVerts = GetModelExtent (m,
                                     pHeader ->m_pGeometry .GetOffset (),
                                     vSum, vMin, vMax);
        }
    }

    if (bTile) {
        m_sModelContext .vCenterOfRotation = vSum;

        if (g_fViewModelsOrthographic)
            m_sModelContext .vView .m_z = 50.0f;
        else if (g_nViewModelsViewDistance > 0)
            m_sModelContext .vView .m_z = g_nViewModelsViewDistance * 1.0f;
        else
            m_sModelContext .vView .m_z = 20.0f;

        m_sModelContext .mView .m_m [0] [0] = -1.0f;
        m_sModelContext .mView .m_m [0] [1] =  0.0f;
        m_sModelContext .mView .m_m [0] [2] =  0.0f;
        m_sModelContext .mView .m_m [0] [3] =  0.0f;
        m_sModelContext .mView .m_m [1] [0] =  0.0f;
        m_sModelContext .mView .m_m [1] [1] =  0.0f;
        m_sModelContext .mView .m_m [1] [2] =  1.0f;
        m_sModelContext .mView .m_m [1] [3] =  0.0f;
        m_sModelContext .mView .m_m [2] [0] =  0.0f;
        m_sModelContext .mView .m_m [2] [1] =  1.0f;
        m_sModelContext .mView .m_m [2] [2] =  0.0f;
        m_sModelContext .mView .m_m [2] [3] =  0.0f;
        m_sModelContext .mView .m_m [3] [0] =  0.0f;
        m_sModelContext .mView .m_m [3] [1] =  0.0f;
        m_sModelContext .mView .m_m [3] [2] =  0.0f;
        m_sModelContext .mView .m_m [3] [3] =  1.0f; 
    } else {
        //
        // Apply the initial viewing angle if one was specified
        //

        int nYaw   = (g_nViewModelsViewAngleY + 360) % 360;
        int nPitch = (g_nViewModelsViewAngleP + 360) % 360;
        int nRoll  = (g_nViewModelsViewAngleR + 360) % 360;
        CVector3 vYPR (nYaw, nRoll, nPitch);
        
        if (vYPR .LengthSqr () > 0.0) {
            CMatrix m, mView (m_sModelContext .mView);

            m .MakeHPR (vYPR);
            m_sModelContext .mView = m * mView;
        }


        if (nVerts != 0)
            vSum /= (float) nVerts;

        //
        // Set the center of rotation
        //

        m_sModelContext .vCenterOfRotation = (vMin + vMax) / 2.0f;

        if (g_nViewModelsViewDistance > 0) {
            m_sModelContext .vView .m_z = g_nViewModelsViewDistance * 1.0f;
        } else {
            //
            // Based on the size, set the model center (ok, this is trash,
            // but at the moment, no real need to make it any better)
            //

            float fSize = (vMax - vMin) .Length ();
            if (fSize > 6)
            {
                m_sModelContext .vView .m_z = 20.0f;
            }
            else if (fSize > 3)
            {
                m_sModelContext .vView .m_z = 10.0f;
            }
            else if (fSize > 1)
            {
                m_sModelContext .vView .m_z = 5.0f;
            }
            else if (fSize > .5f)
            {
                m_sModelContext .vView .m_z = 3.0f;
            }
            else
            {
                m_sModelContext .vView .m_z = 1.0f;
            }
        }
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CModelView> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CModelView::~CModelView ()
{
    if (m_pRtNodeRoot)
    {
        g_wndModel .UnloadTextures (m_pRtNodeRoot);
        delete m_pRtNodeRoot;
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

LRESULT CModelView::OnCreate (UINT uMsg, 
                              WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{

    //
    // Load the palettes
    //

    if (g_apPalettes [0] == NULL)
    {
        g_apPalettes [NwnPalette_Metal1] = TgaLoad (IDR_PAL_ARMOR01, NULL, NULL);
        g_apPalettes [NwnPalette_Metal2] = TgaLoad (IDR_PAL_ARMOR02, NULL, NULL);
        g_apPalettes [NwnPalette_Leather1] = TgaLoad (IDR_PAL_LEATHER01, NULL, NULL);
        g_apPalettes [NwnPalette_Leather2] = TgaLoad (IDR_PAL_LEATHER01, NULL, NULL);
        g_apPalettes [NwnPalette_Cloth1] = TgaLoad (IDR_PAL_CLOTH01, NULL, NULL);
        g_apPalettes [NwnPalette_Cloth2] = TgaLoad (IDR_PAL_CLOTH01, NULL, NULL);
        g_apPalettes [NwnPalette_Skin] = TgaLoad (IDR_PAL_SKIN01, NULL, NULL);
        g_apPalettes [NwnPalette_Hair] = TgaLoad (IDR_PAL_HAIR01, NULL, NULL);
        g_apPalettes [NwnPalette_Tattoo1] = TgaLoad (IDR_PAL_TATTOO01, NULL, NULL);
        g_apPalettes [NwnPalette_Tattoo2] = TgaLoad (IDR_PAL_TATTOO01, NULL, NULL);
    }

    //
    // Create our windows
    //

    bool bModel = (m_nResType == NwnResType_MDL ||
                   m_nResType == NwnResType_DWK ||
                   m_nResType == NwnResType_PWK ||
                   m_nResType == NwnResType_WOK);

    if (m_nResType == NwnResType_PLT || bModel)
    {
        m_RolloutContainer .Create (m_hWnd, _T (""), 
                                    WS_CLIPCHILDREN | WS_CHILD | WS_VISIBLE,
                                    0, 0, 0);
    }
    if (bModel)
    {
        m_dlgModelPos .Create (m_RolloutContainer, _T ("Camera"), 
                               WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE);
        m_RolloutContainer .AddRollout (m_dlgModelPos);
    }
    if (m_nResType == NwnResType_PLT || m_nResType == NwnResType_MDL)
    {
        m_dlgModelColor .Create (m_RolloutContainer, _T ("Palette"), 
                                 WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE);
        m_RolloutContainer .AddRollout (m_dlgModelColor);
        if (bModel)
            m_RolloutContainer .ExpandRollout (m_dlgModelColor, false);
    }
    if (bModel)
    {
        m_dlgModelNodes .Create (m_RolloutContainer, _T ("Nodes"), 
                                 WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE);
        m_RolloutContainer .AddRollout (m_dlgModelNodes);
        m_RolloutContainer .ExpandRollout (m_dlgModelNodes, false);

        if (m_nResType == NwnResType_MDL)
        {
            m_dlgModelAnims .Create (m_RolloutContainer, _T ("Animations"), 
                                     WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE);
            m_RolloutContainer .AddRollout (m_dlgModelAnims);
            m_RolloutContainer .ExpandRollout (m_dlgModelAnims, false);
        }
    }
    if (m_nResType == NwnResType_PLT || m_nResType == NwnResType_BMP ||
        m_nResType == NwnResType_TGA || m_nResType == NwnResType_DDS)
    {
        m_wndImage .Create (m_hWnd, CWindow::rcDefault, _T (""), 
                            WS_CHILD |  WS_CLIPSIBLINGS | WS_VISIBLE, WS_EX_CLIENTEDGE, 
                            (HMENU) 0, NULL);
    }
    else if (bModel)
    {
        g_wndModel .ShowModel (m_hWnd, m_sResModel, m_nResType, 
                               &m_pRtNodeRoot, &m_sModelContext);
    }

    //
    // Update the image
    //

    m_wndImage .OnModelContextChanged ();
    g_wndModel .OnModelContextChanged (m_hWnd, TRUE);

    //
    // Return FALSE
    //

    return false;
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the extent of the model
//
// @parm CMatrix & | m | Current translation matrix
//
// @parm int | nOffset | Offset of the node
//
// @parm CVector3 & | vSum | Sum of the verticies
//
// @parm CVector3 & | vMin | Min vertex
//
// @parm CVector3 & | vMax | Max vertex
//
// @rdesc Number of vertices.
//
//-----------------------------------------------------------------------------

int CModelView::GetModelExtent (CMatrix &m, int nOffset, 
                                CVector3 &vSum, CVector3 &vMin, CVector3 &vMax)
{

    //
    // Get the node
    //

    CNwnMdlNodeHeader *pNode = (CNwnMdlNodeHeader *) 
        m_sResModel .GetModelData (nOffset);

    //
    // Process the controllers
    //

    CVector3 vPosition (0, 0, 0);
    CQuaternion qOrientation (0, 0, 0, 1);
    float fScale = 1.0f;
    NwnMdlControllerKey *paControllerKey = pNode ->m_aControllerKey 
        .GetData (m_sResModel .GetModelData ());
    int nControllerKeyCount = (int) pNode ->m_aControllerKey .GetCount ();
    float *pafControllerData = pNode ->m_aControllerData 
        .GetData (m_sResModel .GetModelData ());
    for (int i = 0; i < nControllerKeyCount; i++)
    {
        if (paControllerKey [i] .lType == NwnMdlCT_Position)
        {
            float *pafData = &pafControllerData [
                paControllerKey [i] .sDataOffset];
            vPosition .m_x = pafData [0];
            vPosition .m_y = pafData [1];
            vPosition .m_z = pafData [2];
        }
        else if (paControllerKey [i] .lType == NwnMdlCT_Orientation)
        {
            float *pafData = &pafControllerData [
                paControllerKey [i] .sDataOffset];
            qOrientation .m_x = pafData [0];
            qOrientation .m_y = pafData [1];
            qOrientation .m_z = pafData [2];
            qOrientation .m_w = pafData [3];
        }
        else if (paControllerKey [i] .lType == NwnMdlCT_Scale)
        {
            float *pafData = &pafControllerData [
                paControllerKey [i] .sDataOffset];
            fScale = pafData [0];
        }
    }

    //
    // Compute the new matrix
    //

    CMatrix mnew;
    CMatrix m2;
    m2 .SetQuaternion (qOrientation);
    m2 .SetTranslate (vPosition);
    if (fScale != 0)
    {
        CMatrix m3;
        m3 .MakeIdentity ();
        m3 .m_m [0] [0] = fScale;
        m3 .m_m [1] [1] = fScale;
        m3 .m_m [2] [2] = fScale;
        CMatrix m4;
        m2 .Multiply4 (m3, m4);//FIXME do it better
        m .Multiply4 (m4, mnew);
    }
    else
    {
        m .Multiply4 (m2, mnew);
    }

    //
    // If this is a mesh
    //

    int nVerts = 0;
    if ((pNode ->m_ulFlags & NwnMdlNF_HasMesh) != 0 &&
        (g_fViewModelsShowAABB || pNode ->m_ulFlags != NwnMdlNT_AABBMesh))
    {
        CNwnMdlMeshHeader *pMesh = static_cast <CNwnMdlMeshHeader *> (pNode);
        if (pMesh ->m_usVertexCount != 0 && 
            pMesh ->m_pavVerts .GetOffset () != 0xFFFFFFFF)
        {
            CVector3 *pVerts = (CVector3 *) m_sResModel .GetRawData (
                pMesh ->m_pavVerts .GetOffset ());
            for (size_t nVert = 0; nVert < pMesh ->m_usVertexCount; nVert++)
            {
                CVector3 v;
                mnew .PreTranslate (pVerts [nVert], v);
                if (vMin .m_x > v .m_x)
                    vMin .m_x = v .m_x;
                if (vMin .m_y > v .m_y)
                    vMin .m_y = v .m_y;
                if (vMin .m_z > v .m_z)
                    vMin .m_z = v .m_z;
                if (vMax .m_x < v .m_x)
                    vMax .m_x = v .m_x;
                if (vMax .m_y < v .m_y)
                    vMax .m_y = v .m_y;
                if (vMax .m_z < v .m_z)
                    vMax .m_z = v .m_z;
                vSum += v;
            }
            nVerts += pMesh ->m_usVertexCount;
        }
    }

    //
    // Create the children
    //

    if (pNode ->m_apChildren .GetCount () > 0)
    {
        CNwnPointer <CNwnMdlNodeHeader> *pulOffsets = pNode ->
            m_apChildren .GetData (m_sResModel .GetModelData ());
        for (int i = 0; i < pNode ->m_apChildren .GetCount (); i++)
        {
            nVerts += GetModelExtent (mnew, pulOffsets [i] .GetOffset (), 
                                      vSum, vMin, vMax);
        }
    }
    return nVerts;

#ifdef XXX
    m_sResModel ->

        //
        // Compute the radius
        //

        for (size_t nVert = 0; nVert < pMeshTemp ->aVerts .GetCount (); nVert++)
        {
            CVector3 v (pVerts [nVert] - pMesh ->m_vBAverage);
            float fLength = v .Length ();
            if (pMesh ->m_fRadius < fLength)
                pMesh ->m_fRadius = fLength;
        }
}
#endif
}


/* vim: set sw=4: */
