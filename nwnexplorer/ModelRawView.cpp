//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ModelRawView.cpp - Model view window |
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
// $History: ModelRawView.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "ModelRawView.h"
#include "../_NwnLib/NwnStreams.h"
#include "ModelRawFormatter.h"

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

extern FILE *g_fpDebug;

//-----------------------------------------------------------------------------
//
// @mfunc <c CModelRawView> constructor.
//
// @parm CData * | pFile | Model file
//
// @parm bool | fFormatted | If true, decode binary models
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CModelRawView::CModelRawView (CData *pFile, bool fFormatted) : 
    CTextViewImpl (fFormatted ? IDS_MODEL_ASCII : IDS_MODEL_RAW)
{
    m_fFormatted = fFormatted;
    m_sRes .Set (pFile);
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CModelRawView> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CModelRawView::~CModelRawView ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc Mark a block as being used
//
// @parm CAtlList <Block> & | aBlocks | List of used blocks
//
// @parm BlockType | nBlockType | Block type
//
// @parm bool | fIsAnim | If true, this is part of an animation tree
//
// @parm int | nOffset | Offset of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawView::BlockUsed (CAtlList <Block> &aBlocks, 
                               BlockType nBlockType, bool fIsAnim, int nOffset, size_t nSize)
{
    if (nSize == 0)
        return;
    int n = 0;
    POSITION pos = aBlocks .GetHeadPosition ();
    while (pos)
    {
        Block &c = aBlocks .GetAt (pos);
        if (nOffset >= n && nOffset < n + c .nSize)
        {
            if (c .nBlockType != BlockType_Unknown)
            {
                if (g_fpDebug)
                    fprintf (g_fpDebug, "**************************************RETYPE!!!!!!");
                _ASSERTE (FALSE); //already claimed data
            }
            else if (nOffset + nSize > n + c .nSize)
            {
                if (g_fpDebug)
                    fprintf (g_fpDebug, "**************************************OVERAGE!!!!!!");
                int nOverage = (int) ((nOffset + nSize) - (n + c .nSize));
                _ASSERTE (FALSE); //already claimed data
            }
            else if (nOffset == n && nSize == c .nSize)
            {
                c .nBlockType = nBlockType;
                c .fIsAnim = fIsAnim;
            }
            else if (nOffset == n)
            {
                Block b;
                b .nBlockType = nBlockType;
                b .nSize = (int) nSize;
                b .fIsAnim = fIsAnim;
                c .nSize -= (int) nSize;
                aBlocks .InsertBefore (pos, b);
            }
            else if (nOffset + nSize == n + c .nSize)
            {
                Block b;
                b .nBlockType = nBlockType;
                b .nSize = (int) nSize;
                b .fIsAnim = fIsAnim;
                c .nSize -= (int) nSize;
                aBlocks .InsertAfter (pos, b);
            }
            else
            {
                Block b;
                int nTotal = c .nSize;
                c .nSize = nOffset - n;
                nTotal -= c .nSize + (int) nSize;
                b .nBlockType = BlockType_Unknown;
                b .nSize = nTotal;
                b .fIsAnim = false;
                aBlocks .InsertAfter (pos, b);
                b .nBlockType = nBlockType;
                b .nSize = (int) nSize;
                b .fIsAnim = fIsAnim;
                aBlocks .InsertAfter (pos, b);
            }
            return;
        }
        else
        {
            n += c .nSize;
        }
        aBlocks .GetNext (pos);
    }
    if (g_fpDebug)
        fprintf (g_fpDebug, "**************************************OUTOFRANGE!!!!!!");
    _ASSERTE (FALSE); // data not in file
}

//-----------------------------------------------------------------------------
//
// @mfunc Process a mesh header
//
// @parm CResModelRef * | pFile | Model file
//
// @parm CAtlList <Block> & | aBlocks | List of identified block
//
// @parm bool | fIsAnim | If true, this is an animation tree
//
// @parm CNwnMdlMeshHeader | pMesh | Mesh header
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawView::HandleMeshHeader (CResModelRef *pFile, 
                                      CAtlList <Block> &aBlocks, bool fIsAnim, CNwnMdlMeshHeader *pMesh)
{

    //
    // Mark the data data
    //

    if (pMesh ->m_sFaces .GetCount () != 0)
    {
        BlockUsed (aBlocks, BlockType_FaceData, fIsAnim,
                   pMesh ->m_sFaces .GetOffset (), 
                   pMesh ->m_sFaces .GetCount () * 
                   pMesh ->m_sFaces .GetElementSize ());
    }

    //
    // Mark the face sets
    //

    if (pMesh ->m_sRawVertexIndices .GetCount () != 0 ||
        pMesh ->m_sVertexIndicesCount .GetCount () != 0)
    {
        _ASSERTE (
            pMesh ->m_sRawVertexIndices .GetCount () != 0 &&
            pMesh ->m_sVertexIndicesCount .GetCount () != 0);

        BlockUsed (aBlocks, BlockType_FaceSetOffsets, fIsAnim,
                   pMesh ->m_sRawVertexIndices .GetOffset (),
                   pMesh ->m_sRawVertexIndices .GetCount () * 
                   pMesh ->m_sRawVertexIndices .GetElementSize ());

        BlockUsed (aBlocks, BlockType_FaceSetCounts, fIsAnim,
                   pMesh ->m_sVertexIndicesCount .GetOffset (),
                   pMesh ->m_sVertexIndicesCount .GetCount () * 
                   pMesh ->m_sVertexIndicesCount .GetElementSize ());

        UINT32 *pulCounts = pMesh ->m_sVertexIndicesCount .
            GetData (pFile ->GetModelData ());
        CNwnPointer <UINT16> *pulOffsets = pMesh ->m_sRawVertexIndices .
            GetData (pFile ->GetModelData ());

        for (int i = 0; i < pMesh ->m_sRawVertexIndices .GetCount (); i++)
        {
            BlockUsed (aBlocks, BlockType_FaceSetFaces, fIsAnim,
                       pFile ->GetRawDataOffset () + pulOffsets [i] .GetOffset (),
                       sizeof (UINT16) * pulCounts [i]);
        }
    }

    //
    // Mark the known vertex information
    //

    if (pMesh ->m_usVertexCount > 0)
    {
        if (pMesh ->m_pavVerts .GetOffset () != 0xFFFFFFFF)
        {
            BlockUsed (aBlocks, BlockType_VertexArray, fIsAnim,
                       pFile ->GetRawDataOffset () + 
                       pMesh ->m_pavVerts .GetOffset (),
                       sizeof (float) * 3 * pMesh ->m_usVertexCount);
        }

        for (int i = 0; i < 4; i++)
        {
            if (pMesh ->m_pavTVerts [i] .GetOffset () != 0xFFFFFFFF)
            {
                BlockUsed (aBlocks, BlockType_TextureArray, fIsAnim,
                           pFile ->GetRawDataOffset () + 
                           pMesh ->m_pavTVerts [i] .GetOffset (),
                           sizeof (float) * 2 * pMesh ->m_usVertexCount);
            }
        }

        if (pMesh ->m_pavNormals .GetOffset () != 0xFFFFFFFF)
        {
            BlockUsed (aBlocks, BlockType_NormalArray, fIsAnim,
                       pFile ->GetRawDataOffset () + 
                       pMesh ->m_pavNormals .GetOffset (),
                       sizeof (float) * 3 * pMesh ->m_usVertexCount);
        }

        if (pMesh ->m_paColors .GetOffset () != 0xFFFFFFFF)
        {
            BlockUsed (aBlocks, BlockType_ColorArray, fIsAnim,
                       pFile ->GetRawDataOffset () + 
                       pMesh ->m_paColors .GetOffset (),
                       sizeof (UINT32) * pMesh ->m_usVertexCount);
        }

        if (pMesh ->m_pavBumpmapAnim1 .GetOffset () != 0xFFFFFFFF)
        {
            BlockUsed (aBlocks, BlockType_FaceAnimVertex, fIsAnim,
                       pFile ->GetRawDataOffset () + 
                       pMesh ->m_pavBumpmapAnim1 .GetOffset (),
                       sizeof (CVector3) * pMesh ->m_usVertexCount);
        }

        if (pMesh ->m_pavBumpmapAnim2 .GetOffset () != 0xFFFFFFFF)
        {
            BlockUsed (aBlocks, BlockType_FaceAnimVertex, fIsAnim,
                       pFile ->GetRawDataOffset () + 
                       pMesh ->m_pavBumpmapAnim2 .GetOffset (),
                       sizeof (CVector3) * pMesh ->m_usVertexCount);
        }

        if (pMesh ->m_pavBumpmapAnim3 .GetOffset () != 0xFFFFFFFF)
        {
            BlockUsed (aBlocks, BlockType_FaceAnimVertex, fIsAnim,
                       pFile ->GetRawDataOffset () + 
                       pMesh ->m_pavBumpmapAnim3 .GetOffset (),
                       sizeof (CVector3) * pMesh ->m_usVertexCount);
        }

        if (pMesh ->m_pavBumpmapAnim4 .GetOffset () != 0xFFFFFFFF)
        {
            BlockUsed (aBlocks, BlockType_FaceAnimVertex, fIsAnim,
                       pFile ->GetRawDataOffset () + 
                       pMesh ->m_pavBumpmapAnim4 .GetOffset (),
                       sizeof (CVector3) * pMesh ->m_usVertexCount);
        }

        if (pMesh ->m_pavBumpmapAnim5 .GetOffset () != 0xFFFFFFFF)
        {
            BlockUsed (aBlocks, BlockType_FaceAnimVertex, fIsAnim,
                       pFile ->GetRawDataOffset () + 
                       pMesh ->m_pavBumpmapAnim5 .GetOffset (),
                       sizeof (CVector3) * pMesh ->m_usVertexCount);
        }

        if (pMesh ->m_pafBumpmapAnim6 .GetOffset () != 0xFFFFFFFF)
        {
            BlockUsed (aBlocks, BlockType_FaceAnimValue, fIsAnim,
                       pFile ->GetRawDataOffset () + 
                       pMesh ->m_pafBumpmapAnim6 .GetOffset (),
                       sizeof (float) * pMesh ->m_usVertexCount);
        }
    }
}

bool __cdecl CModelRawView::PartitionCallback (CResModelRef *pFile, 
                                               void *lParam, void *pData, bool fIsGeometryHeader,  
                                               UINT32 ulFlags, int nOffset)
{

    //
    // Get the blocks
    //

    CAtlList <Block> *pBlocks = (CAtlList <Block> *) lParam;
    CAtlList <Block> &aBlocks = *pBlocks;

    //
    // Used only by nodes
    //

    int nSize;
    BlockType nBlockType;

    //
    // Get the animation flag
    //

    bool fIsAnim = (ulFlags & CResModelRef::EnumFlag_Animation) != 0;

    //
    // If this is geometry
    //

    if (fIsGeometryHeader)
    {
        //
        // Get the geometry header
        //

        CNwnMdlGeometryHeader *pGeoHeader = (CNwnMdlGeometryHeader *) pData;

        //
        // Switch based on the type
        //

        switch (pGeoHeader ->m_ucGeometryType)
        {
            //
            // If this is a model header
            //

            case NwnMdlGT_Model:
                {

                    //
                    // Get the header
                    //

                    CNwnMdlModel *pModelHeader = 
                        (CNwnMdlModel *) pData;

                    //
                    // Mark my known data
                    //

                    BlockUsed (aBlocks, BlockType_ModelHeader, fIsAnim,
                               nOffset, sizeof (*pModelHeader));
                    if (pModelHeader ->m_apAnimations .GetCount () > 0)
                    {
                        BlockUsed (aBlocks, BlockType_AnimationOffsets, fIsAnim,
                                   pModelHeader ->m_apAnimations .GetOffset (),
                                   pModelHeader ->m_apAnimations .GetCount () * 
                                   pModelHeader ->m_apAnimations .GetElementSize ());
                    }
                }
                break;

                //
                // If this is a animation header
                //

            case NwnMdlGT_Animation:
                {

                    //
                    // Get the header
                    //

                    CNwnMdlAnimation *pAnimationHeader = 
                        (CNwnMdlAnimation *) pData;

                    //
                    // Mark my known data
                    //

                    BlockUsed (aBlocks, BlockType_AnimationHeader, 
                               fIsAnim, nOffset, sizeof (*pAnimationHeader));

                    //
                    // Mark the events
                    //

                    if (pAnimationHeader ->m_asEvents .GetCount () > 0)
                    {
                        BlockUsed (aBlocks, BlockType_AnimationEvents, fIsAnim,
                                   pAnimationHeader ->m_asEvents .GetOffset (), 
                                   pAnimationHeader ->m_asEvents .GetCount () * 
                                   pAnimationHeader ->m_asEvents .GetElementSize ());
                    }
                }
                break;
        }
    }
    else
    {

        //
        // Get the node header
        //

        CNwnMdlNodeHeader *pNodeHeader = (CNwnMdlNodeHeader *) pData;

        //
        // Switch based on the type
        //

        switch (pNodeHeader ->m_ulFlags)
        {

            //
            // If this is an unknown node
            //

            default:
                {

                    //
                    // Set the size
                    //

                    nSize = sizeof (CNwnMdlNodeHeader);
                    nBlockType = BlockType_NodeHeader;
                }
                break;

                //
                // If this is an dummy node
                //

            case NwnMdlNT_Dummy:
                {

                    //
                    // Set the size
                    //

                    nSize = sizeof (CNwnMdlDummyNode);
                    nBlockType = BlockType_DummyNode;
                }
                break;

                //
                // If this is an tri mesh node
                //

            case NwnMdlNT_TriMesh:
                {

                    //
                    // Set the size
                    //

                    nSize = sizeof (CNwnMdlTriMeshNode);
                    nBlockType = BlockType_TriMeshNode;

                    //
                    // Handle the common mesh elements
                    //

                    CNwnMdlTriMeshNode *pNode = (CNwnMdlTriMeshNode *) pData;
                    HandleMeshHeader (pFile, aBlocks, fIsAnim, pNode);
                }
                break;

                //
                // If this is an anim mesh node
                //

            case NwnMdlNT_AnimMesh:
                {

                    //
                    // Set the size
                    //

                    nSize = sizeof (CNwnMdlAnimMeshNode);
                    nBlockType = BlockType_AnimMeshNode;

                    //
                    // Handle the common mesh elements
                    //

                    CNwnMdlAnimMeshNode *pNode = (CNwnMdlAnimMeshNode *) pData;
                    HandleMeshHeader (pFile, aBlocks, fIsAnim, pNode);

                    //
                    // Add the extra vertex and texture sets
                    //

                    if (pNode ->m_ulVertexSetCount > 0)
                    {
                        size_t nSize = pNode ->m_pvVertexSets .GetElementSize () * pNode ->m_usVertexCount;
                        for (int i = 0; i < pNode ->m_ulVertexSetCount; i++)
                        {
                            BlockUsed (aBlocks, BlockType_VertexArray, fIsAnim,
                                       pNode ->m_pvVertexSets .GetOffset () + i * nSize, nSize);
                        }
                    }
                    if (pNode ->m_ulTextureSetCount > 0)
                    {
                        size_t nSize = pNode ->m_pvTextureSets .GetElementSize () * pNode ->m_usVertexCount;
                        for (int i = 0; i < pNode ->m_ulTextureSetCount; i++)
                        {
                            BlockUsed (aBlocks, BlockType_VertexArray, fIsAnim,
                                       pNode ->m_pvTextureSets .GetOffset () + i * nSize, nSize);
                        }
                    }
                }
                break;

                //
                // If this is an AABB node
                //

            case NwnMdlNT_AABBMesh:
                {

                    //
                    // Set the size
                    //

                    nSize = sizeof (CNwnMdlAABBMeshNode);
                    nBlockType = BlockType_AABBNode;

                    //
                    // Handle the common mesh elements
                    //

                    CNwnMdlAABBMeshNode *pNode = (CNwnMdlAABBMeshNode *) pData;
                    HandleMeshHeader (pFile, aBlocks, fIsAnim, pNode);

                    //
                    // If we have an AABB tree
                    //

                    if (pNode ->m_pAABBTree .GetOffset () > 0)
                    {
                        int nMaxOffset = 0;
                        CModelRawFormatter::GetMaxAABBTreeOffset (pFile, 
                                                                  pNode ->m_pAABBTree .GetOffset (), nMaxOffset);
                        nMaxOffset += sizeof (NwnMdlAABBEntry);
                        BlockUsed (aBlocks, BlockType_AABBTree, fIsAnim,
                                   pNode ->m_pAABBTree .GetOffset (), 
                                   nMaxOffset - pNode ->m_pAABBTree .GetOffset ());
                    }
                }
                break;

                //
                // If this is an dangly mesh node
                //

            case NwnMdlNT_DanglyMesh:
                {

                    //
                    // Set the size
                    //

                    nSize = sizeof (CNwnMdlDanglyMeshNode);
                    nBlockType = BlockType_DanglyMeshNode;

                    //
                    // Handle the common mesh elements
                    //

                    CNwnMdlDanglyMeshNode *pNode = (CNwnMdlDanglyMeshNode *) pData;
                    HandleMeshHeader (pFile, aBlocks, fIsAnim, pNode);

                    //
                    // If we have contraints
                    //

                    if (pNode ->m_afConstraints .GetCount () > 0)
                    {
                        BlockUsed (aBlocks, BlockType_Constraints, fIsAnim,
                                   pNode ->m_afConstraints .GetOffset (),
                                   pNode ->m_afConstraints .GetElementSize () * 
                                   pNode ->m_afConstraints .GetCount ());
                    }
                }
                break;


                //
                // If this is an skin node
                //

            case NwnMdlNT_SkinMesh:
                {

                    //
                    // Set the size
                    //

                    nSize = sizeof (CNwnMdlSkinMeshNode);
                    nBlockType = BlockType_SkinNode;

                    //
                    // Handle the common mesh elements
                    //

                    CNwnMdlSkinMeshNode *pNode = (CNwnMdlSkinMeshNode *) pData;
                    HandleMeshHeader (pFile, aBlocks, fIsAnim, pNode);

                    //
                    // Mark my vertices
                    //

                    if (pNode ->m_usVertexCount > 0)
                    {
                        if (pNode ->m_pafSkinWeights .GetDataNZ (NULL) != NULL)
                        {
                            BlockUsed (aBlocks, BlockType_SkinBoneWeight, fIsAnim,
                                       pFile ->GetRawDataOffset () + 
                                       pNode ->m_pafSkinWeights .GetOffset (),
                                       sizeof (float) * 4 * pNode ->m_usVertexCount);
                        }

                        if (pNode ->m_pasSkinBoneRefs .GetDataNZ (NULL) != NULL)
                        {
                            BlockUsed (aBlocks, BlockType_SkinBoneRef, fIsAnim,
                                       pFile ->GetRawDataOffset () + 
                                       pNode ->m_pasSkinBoneRefs .GetOffset (),
                                       sizeof (UINT16) * 4 * pNode ->m_usVertexCount);
                        }
                    }

                    //
                    // Mark the skin blocks
                    //

                    if (pNode ->m_ulNodeToBoneCount > 0)
                    {
                        BlockUsed (aBlocks, BlockType_SkinUnkA, fIsAnim,
                                   pNode ->m_pasNodeToBoneMap .GetOffset (), 
                                   sizeof (UINT16) * pNode ->m_ulNodeToBoneCount);
                    }

                    if (pNode ->m_aQBoneRefInv .GetCount () > 0)
                    {
                        BlockUsed (aBlocks, BlockType_SkinQBoneRefInv, fIsAnim,
                                   pNode ->m_aQBoneRefInv .GetOffset (), 
                                   sizeof (float) * 4 * pNode ->m_aQBoneRefInv .GetCount ());
                    }

                    if (pNode ->m_aTBoneRefInv .GetCount () > 0)
                    {
                        BlockUsed (aBlocks, BlockType_SkinTBoneRefInv, fIsAnim,
                                   pNode ->m_aTBoneRefInv .GetOffset (), 
                                   sizeof (float) * 3 * pNode ->m_aTBoneRefInv .GetCount ());
                    }

                    if (pNode ->m_aBoneConstantIndices .GetCount () > 0)
                    {
                        BlockUsed (aBlocks, BlockType_SkinUnkD, fIsAnim,
                                   pNode ->m_aBoneConstantIndices .GetOffset (), 
                                   sizeof (float) * pNode ->m_aBoneConstantIndices .GetCount ());
                    }
                }
                break;

                //
                // If this is an emitter node
                //

            case NwnMdlNT_Emitter:
                {

                    //
                    // Set the size
                    //

                    nSize = sizeof (CNwnMdlEmitterNode);
                    nBlockType = BlockType_EmitterNode;
                }
                break;

                //
                // If this is an light node
                //

            case NwnMdlNT_Light:
                {

                    //
                    // Set the size
                    //

                    nSize = sizeof (CNwnMdlLightNode);
                    nBlockType = BlockType_LightNode;

                    //
                    // Get a pointer to the node
                    //

                    CNwnMdlLightNode *pNode = (CNwnMdlLightNode *) pData;

                    //
                    // Add the flare textures
                    //

                    if (pNode ->m_apszTextureNames .GetCount () > 0)
                    {
                        BlockUsed (aBlocks, BlockType_FlareTexOffset, fIsAnim,
                                   pNode ->m_apszTextureNames .GetOffset (), 
                                   sizeof (UINT32) * pNode ->m_apszTextureNames .GetCount ());
                        CNwnPointer <char> *pulOffsets = pNode ->m_apszTextureNames .
                            GetData (pFile ->GetModelData ());
                        for (int i = 0; i < pNode ->m_apszTextureNames .GetCount (); i++)
                        {
                            char *pszTexture = (char *) pFile ->GetModelData (
                                pulOffsets [i] .GetOffset ());
                            int nLength = (int) strlen (pszTexture) + 1;
                            BlockUsed (aBlocks, BlockType_FlareTex, fIsAnim,
                                       pulOffsets [i] .GetOffset (), nLength);
                        }
                    }

                    //
                    // Add the flare data
                    //

                    if (pNode ->m_afFlareSizes .GetCount () > 0)
                    {
                        BlockUsed (aBlocks, BlockType_FlareData, fIsAnim,
                                   pNode ->m_afFlareSizes .GetOffset (), 
                                   sizeof (UINT32) * 1 * pNode ->m_afFlareSizes .GetCount ());
                    }
                    if (pNode ->m_afFlarePositions .GetCount () > 0)
                    {
                        BlockUsed (aBlocks, BlockType_FlareData, fIsAnim,
                                   pNode ->m_afFlareSizes .GetOffset (), 
                                   sizeof (UINT32) * 1 * pNode ->m_afFlarePositions .GetCount ());
                    }
                    if (pNode ->m_avFlareColorShifts .GetCount () > 0)
                    {
                        BlockUsed (aBlocks, BlockType_FlareData, fIsAnim,
                                   pNode ->m_afFlareSizes .GetOffset (), 
                                   sizeof (UINT32) * 3 * pNode ->m_avFlareColorShifts .GetCount ());
                    }
                }
                break;

                //
                // If this is an reference node
                //

            case NwnMdlNT_Reference:
                {

                    //
                    // Set the size
                    //

                    nSize = sizeof (CNwnMdlReferenceNode);
                    nBlockType = BlockType_ReferenceNode;
                }
                break;
        }

        //
        // Mark my known data
        //

        BlockUsed (aBlocks, nBlockType,  fIsAnim, nOffset, nSize);
        if (pNodeHeader ->m_apChildren .GetCount () > 0)
        {
            BlockUsed (aBlocks, BlockType_GenericOffsets, fIsAnim,
                       pNodeHeader ->m_apChildren .GetOffset (),
                       pNodeHeader ->m_apChildren .GetCount () * 
                       pNodeHeader ->m_apChildren .GetElementSize ());
        }

        //
        // Handle the controller data
        //

        if (pNodeHeader ->m_aControllerKey .GetCount () > 0)
        {
            BlockUsed (aBlocks, BlockType_ControllerKey, fIsAnim,
                       pNodeHeader ->m_aControllerKey .GetOffset (),
                       pNodeHeader ->m_aControllerKey .GetCount () * 
                       pNodeHeader ->m_aControllerKey .GetElementSize ());
        }

        //
        // Handle the geometry data
        //

        if (pNodeHeader ->m_aControllerData .GetCount () > 0)
        {
            BlockUsed (aBlocks, BlockType_ControllerData, fIsAnim,
                       pNodeHeader ->m_aControllerData .GetOffset (),
                       pNodeHeader ->m_aControllerData .GetCount () * 
                       pNodeHeader ->m_aControllerData .GetElementSize ());
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a block of data
//
// @parm CModelRawFormatter * | pFormatter | Formatter to use
//
// @parm Block & | b | Block to format
//
// @parm int | nStart | Starting location
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawView::WriteBlock (CModelRawFormatter *pFormatter, 
                                Block &b, int nStart)
{
    //
    // Dump known information
    //

    int nSkip = 0;
    switch (b .nBlockType)
    {
        case BlockType_Unknown:
            pFormatter ->Format ("****** Unknown ****** at %08X length %d\r\n\r\n", nStart, b .nSize);
            break;

        case BlockType_ModelHeader:
            pFormatter ->WriteModelHeader (nStart, b .nSize);
            return;

        case BlockType_AnimationHeader:
            pFormatter ->WriteAnimationHeader (nStart, b .nSize);
            return;

        case BlockType_NodeHeader:
            pFormatter ->WriteNodeHeader (nStart, b .nSize);
            return;

        case BlockType_DummyNode:
            pFormatter ->WriteDummyNode (nStart, b .nSize);
            return;

        case BlockType_TriMeshNode:
            pFormatter ->WriteTriMeshNode (nStart, b .nSize);
            return;

        case BlockType_SkinNode:
            pFormatter ->WriteSkinNode (nStart, b .nSize);
            return;

        case BlockType_AABBNode:
            pFormatter ->WriteAABBNode (nStart, b .nSize);
            return;

        case BlockType_AnimMeshNode:
            pFormatter ->WriteAnimMeshNode (nStart, b .nSize);
            return;

        case BlockType_ReferenceNode:
            pFormatter ->WriteReferenceNode (nStart, b .nSize);
            return;

        case BlockType_EmitterNode:
            pFormatter ->WriteEmitterNode (b .fIsAnim, nStart, b .nSize);
            return;

        case BlockType_LightNode:
            pFormatter ->WriteLightNode (b .fIsAnim, nStart, b .nSize);
            return;

        case BlockType_DanglyMeshNode:
            pFormatter ->WriteDanglyMeshNode (nStart, b .nSize);
            return;

        case BlockType_AABBTree:
            return;

        case BlockType_SkinUnkA:
            return;

        case BlockType_SkinQBoneRefInv:
            return;

        case BlockType_SkinTBoneRefInv:
            return;

        case BlockType_SkinUnkD:
            return;

        case BlockType_AnimationEvents:
            return;

        case BlockType_Constraints:
            return;

        case BlockType_FaceData:
            return;

        case BlockType_ControllerData:
            return;

        case BlockType_ControllerKey:
            return;

        case BlockType_AnimationOffsets:
            return;

        case BlockType_GenericOffsets:
            return;

        case BlockType_FaceSetOffsets:
            return;

        case BlockType_FaceSetFaces:
            return;

        case BlockType_VertexArray:
            return;

        case BlockType_TextureArray:
            return;

        case BlockType_NormalArray:
            return;

        case BlockType_ColorArray:
            return;

        case BlockType_FaceSetCounts:
            return;

        case BlockType_SkinBoneWeight:
            return;

        case BlockType_SkinBoneRef:
            return;

        case BlockType_FaceAnimVertex:
            return;

        case BlockType_FaceAnimValue:
            return;

        case BlockType_FlareTexOffset:
            return;

        case BlockType_FlareTex:
            return;

        case BlockType_FlareData:
            return;
    }

    //
    // Write the hex
    //

    pFormatter ->WriteHex (nStart, b .nSize, nSkip);
    pFormatter ->TwoBlankLines ();
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the window for this view
//
// @parm HWND | hWnd | Parent window
//
// @rdesc Pointer to the window
//
//-----------------------------------------------------------------------------

CWindow *CModelRawView::GetWindow (HWND hWnd)
{
    if (m_hWnd == NULL && m_sRes != NULL)
    {
        //CString str;

        const unsigned char *pauchStart = m_sRes .GetData ();
        const unsigned char *pauchEnd = pauchStart + m_sRes .GetSize ();
        if (pauchStart [0] != '#')
        {
            CNwnMemoryStream sStream;
            if (!m_fFormatted)
            {
                pauchStart += 12;
                const unsigned char *p = pauchStart;
                CAtlList <Block> aBlocks;
                Block a;
                a .nBlockType = BlockType_Unknown;
                a .nSize = pauchEnd - pauchStart;
                a .fIsAnim = false;
                aBlocks .AddHead (a);
                //HandleModelHeader (m_sRes, aBlocks, 0);
                m_sRes .EnumModel (PartitionCallback, &aBlocks);

                CModelRawFormatter sFormatter (m_sRes, &sStream);
                int nStart = 0;
                POSITION pos = aBlocks .GetHeadPosition ();
                while (pos)
                {
                    Block &c = aBlocks .GetNext (pos);
                    WriteBlock (&sFormatter, c, nStart);
                    nStart += c .nSize;
                }

                sFormatter .Format ("Raw Data at %08X length %d\r\n\r\n", 
                                    m_sRes .GetRawDataOffset (), m_sRes .GetRawDataSize ());
                sFormatter .WriteHex (m_sRes .GetRawDataOffset (), 
                                      m_sRes .GetRawDataSize (), 0);
                sFormatter .TwoBlankLines ();
            }
            else
            {
                CNwnMdlModel *pModel = (CNwnMdlModel *) m_sRes .GetModelData (0);
                pModel ->Decompile (&sStream, true, false, true, 
                                    m_sRes .GetModelData (), m_sRes .GetRawData ());
            }

            //
            // Save the stream text
            //

            char szNull = 0;
            sStream .Write (&szNull, 1);
            m_strText = (char *) sStream .GetData ();

#ifdef _DEBUG
            FILE *fp = fopen ("model.txt", "wb");
            fwrite (m_strText .GetString (), m_strText .GetLength (), 1, fp);
            fclose (fp);
#endif
        }
        else
        {
            m_strText = CStringA ((char *) pauchStart, m_sRes .GetSize ());
        }
    }
    return CTextViewImpl::GetWindow (hWnd);
}


/* vim: set sw=4: */
