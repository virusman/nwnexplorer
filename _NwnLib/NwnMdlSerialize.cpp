//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnMdlSerialize.cpp - Serialize an absolute model to data buffers
//
// This module contains the Nwn model compiler.
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
// $History: Cnf.cpp $
//      
//-----------------------------------------------------------------------------

#include "NwnMdlGeometry.h"
#include "NwnMdlNodes.h"

//-----------------------------------------------------------------------------
//
// @func Rewrite model data
//
// @parm CNwnStream * | pStream | Destination stream
//
// @parm void * | pData | Pointer to the data
//
// @parm size_t | nSize | Length of the data
//
// @parm size_t | nPos | Write position
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void SerializeRewrite (CNwnStream *pStream, 
                       void *pData, size_t nSize, size_t nPos)
{
    size_t nCurPos = pStream ->GetPosition ();
    pStream ->SeekFromBegining (nPos);
    pStream ->Write (pData, nSize);
    pStream ->SeekFromBegining (nCurPos);
    return;
}

//-----------------------------------------------------------------------------
//
// @func Serialize the model
//
// @parm CNwnStream * | pModelData | Model data stream
//
// @parm CNwnStream * | pRawData | Raw data stream
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlModel::Serialize (CNwnStream *pModelData, CNwnStream *pRawData)
{

    //
    // Create a copy of the object.
    //
    // NOTE: We DON'T want the normal destructors to execute
    //

    UINT8 auchData [sizeof (CNwnMdlModel)];
    CNwnMdlModel *pOutModel = (CNwnMdlModel *) auchData;
    memcpy (pOutModel, this, sizeof (*pOutModel));

    //
    // Initialize data that shouldn't be written
    //

    pOutModel ->m_pSuperModel .Initialize ();
    pOutModel ->m_aRtArray1 .Initialize ();
    pOutModel ->m_aRtArray2 .Initialize ();

    //
    // Write the header
    //

    size_t nPos = pModelData ->GetPosition ();
    pModelData ->Write (pOutModel, sizeof (*pOutModel));

    //
    // If we have animations
    //

    pOutModel ->m_apAnimations .RelativePrepare ();
    if (m_apAnimations .GetCount () > 0)
    {

        //
        // Create a dummy list in the file
        //

        UINT32 *pulAnimations = new UINT32 [m_apAnimations .GetCount ()];
        pOutModel ->m_apAnimations .SetOffset (
            (UINT32) pModelData ->GetPosition ());
        pModelData ->Write (pulAnimations, sizeof (UINT32) * 
                            m_apAnimations .GetCount ());

        //
        // Serialize the animations
        //

        for (int i = 0; i < (int) m_apAnimations .GetCount (); i++)
        {
            CNwnMdlAnimation *pAnimation = m_apAnimations [i] .GetData ();
            pulAnimations [i] = (UINT32) pModelData ->GetPosition ();
            pAnimation ->Serialize (pModelData, pRawData);
        }

        //
        // Rewrite the data
        //

        SerializeRewrite (pModelData, pulAnimations, 
                          sizeof (UINT32) * m_apAnimations .GetCount (),
                          pOutModel ->m_apAnimations .GetOffset ());

        //
        // Free the scratch memory
        //

        delete [] pulAnimations;
    }

    //
    // Write the geometry
    //

    if (m_pGeometry .GetData () != NULL)
    {
        pOutModel ->m_pGeometry .SetOffset ((UINT32) 
                                            pModelData ->GetPosition ());
        m_pGeometry .GetData () ->Serialize (pModelData, pRawData);
    }

    //
    // Rewrite the model header
    //

    SerializeRewrite (pModelData, pOutModel, sizeof (*pOutModel), nPos);
    return;
}

//-----------------------------------------------------------------------------
//
// @func Serialize an animation
//
// @parm CNwnStream * | pModelData | Model data stream
//
// @parm CNwnStream * | pRawData | Raw data stream
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlAnimation::Serialize (CNwnStream *pModelData, CNwnStream *pRawData)
{

    //
    // Create a copy of the object.
    //
    // NOTE: We DON'T want the normal destructors to execute
    //

    UINT8 auchData [sizeof (CNwnMdlAnimation)];
    CNwnMdlAnimation *pOutAnimation = (CNwnMdlAnimation *) auchData;
    memcpy (pOutAnimation, this, sizeof (*pOutAnimation));

    //
    // Initialize data that shouldn't be written
    //

    pOutAnimation ->m_aRtArray1 .Initialize ();
    pOutAnimation ->m_aRtArray2 .Initialize ();

    //
    // Prepare arrays and pointers for relative offsets
    //

    pOutAnimation ->m_asEvents .RelativePrepare ();
    pOutAnimation ->m_pGeometry .Initialize ();

    //
    // Compute the start of the geometry and the events
    //

    size_t nPos = pModelData ->GetPosition () + sizeof (*pOutAnimation);
    if (m_asEvents .GetCount () > 0)
    {
        pOutAnimation ->m_asEvents .SetOffset ((UINT32) nPos);
        nPos += m_asEvents .GetCount () * sizeof (NwnMdlAnimationEvent);
    }
    if (m_pGeometry .GetData () != NULL)
    {
        pOutAnimation ->m_pGeometry .SetOffset ((UINT32) nPos);
    }

    //
    // Write the header
    //

    pModelData ->Write (pOutAnimation, sizeof (*pOutAnimation));

    //
    // Write the events
    //

    if (m_asEvents .GetCount () > 0)
    {
        pModelData ->Write (m_asEvents .GetData (),
                            m_asEvents .GetCount () * sizeof (NwnMdlAnimationEvent));
    }

    //
    // Write the geometry
    //

    if (m_pGeometry .GetData () != NULL)
    {
        m_pGeometry .GetData () ->Serialize (pModelData, pRawData);
    }
    return;
}

//-----------------------------------------------------------------------------
//
// @func Serialize a node header
//
// @parm CNwnMdlNodeHeader * | pOutNode | Output copy of the node
//              being serialized
//
// @parm CNwnStream * | pModelData | Model data stream
//
// @parm CNwnStream * | pRawData | Raw data stream
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlNodeHeader::NodeHeaderSerialize (CNwnMdlNodeHeader *pOutNode, 
                                             CNwnStream *pModelData, CNwnStream *pRawData)
{

    //
    // Prepare the data
    //

    pOutNode ->m_pGeometryHeader .Initialize ();
    pOutNode ->m_pParentNode .Initialize ();
    pOutNode ->m_apChildren .RelativePrepare ();
    pOutNode ->m_aControllerKey .RelativePrepare ();
    pOutNode ->m_aControllerData .RelativePrepare ();

    //
    // If we have children
    //

    if (m_apChildren .GetCount () > 0)
    {

        //
        // Create a dummy list in the file
        //

        UINT32 *pulChildren = new UINT32 [m_apChildren .GetCount ()];
        pOutNode ->m_apChildren .SetOffset ((UINT32) pModelData ->GetPosition ());
        pModelData ->Write (pulChildren, 
                            sizeof (UINT32) * m_apChildren .GetCount ());

        //
        // Serialize the animations
        //

        for (int i = 0; i < (int) m_apChildren .GetCount (); i++)
        {
            CNwnMdlNodeHeader *pChild = m_apChildren [i] .GetData ();
            pulChildren [i] = (UINT32) pModelData ->GetPosition ();
            pChild ->Serialize (pModelData, pRawData);
        }

        //
        // Rewrite the data
        //

        SerializeRewrite (pModelData, pulChildren, 
                          sizeof (UINT32) * m_apChildren .GetCount (),
                          pOutNode ->m_apChildren .GetOffset ());

        //
        // Free the scratch memory
        //

        delete [] pulChildren;
    }

    //
    // Write the controller keys
    //

    if (m_aControllerKey .GetCount () > 0)
    {
        pOutNode ->m_aControllerKey .SetOffset (
            (UINT32) pModelData ->GetPosition ());
        pModelData ->Write (m_aControllerKey .GetData (),
                            m_aControllerKey .GetCount () * 
                            sizeof (NwnMdlControllerKey));
    }

    //
    // Write the controller data
    //

    if (m_aControllerData .GetCount () > 0)
    {
        pOutNode ->m_aControllerData .SetOffset (
            (UINT32) pModelData ->GetPosition ());
        pModelData ->Write (m_aControllerData .GetData (),
                            m_aControllerData .GetCount () * 
                            sizeof (float));
    }
    return;
}

//-----------------------------------------------------------------------------
//
// @func Serialize a dummy node 
//
// @parm CNwnStream * | pModelData | Model data stream
//
// @parm CNwnStream * | pRawData | Raw data stream
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlDummyNode::_Serialize (
    CNwnStream *pModelData, CNwnStream *pRawData)
{

    //
    // Create a copy of the object.
    //
    // NOTE: We DON'T want the normal destructors to execute
    //

    UINT8 auchData [sizeof (CNwnMdlDummyNode)];
    CNwnMdlDummyNode *pOutNode = (CNwnMdlDummyNode *) auchData;
    memcpy (pOutNode, this, sizeof (*pOutNode));

    //
    // Write the header
    //

    size_t nPos = pModelData ->GetPosition ();
    pModelData ->Write (pOutNode, sizeof (*pOutNode));

    //
    // Serialize the node header
    //

    NodeHeaderSerialize (pOutNode, pModelData, pRawData);

    //
    // Rewrite the header
    //

    SerializeRewrite (pModelData, pOutNode, sizeof (*pOutNode), nPos);
    return;
}

//-----------------------------------------------------------------------------
//
// @func Serialize a light node
//
// @parm CNwnStream * | pModelData | Model data stream
//
// @parm CNwnStream * | pRawData | Raw data stream
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlLightNode::_Serialize (
    CNwnStream *pModelData, CNwnStream *pRawData)
{

    //
    // Create a copy of the object.
    //
    // NOTE: We DON'T want the normal destructors to execute
    //

    UINT8 auchData [sizeof (CNwnMdlLightNode)];
    CNwnMdlLightNode *pOutNode = (CNwnMdlLightNode *) auchData;
    memcpy (pOutNode, this, sizeof (*pOutNode));

    //
    // Write the header
    //

    size_t nPos = pModelData ->GetPosition ();
    pModelData ->Write (pOutNode, sizeof (*pOutNode));

    //
    // Prep the arrays
    //

    pOutNode ->m_aSomething1 .Initialize ();
    pOutNode ->m_afFlareSizes .RelativePrepare ();
    pOutNode ->m_afFlarePositions .RelativePrepare ();
    pOutNode ->m_avFlareColorShifts .RelativePrepare ();
    pOutNode ->m_apszTextureNames .RelativePrepare ();

    //
    // Write the sizes
    //

    if (m_afFlareSizes .GetCount () > 0)
    {
        pOutNode ->m_afFlareSizes .SetOffset (
            (UINT32) pModelData ->GetPosition ());
        pModelData ->Write (m_afFlareSizes .GetData (),
                            m_afFlareSizes .GetCount () * 
                            sizeof (float));
    }

    //
    // Write the positions
    //

    if (m_afFlarePositions .GetCount () > 0)
    {
        pOutNode ->m_afFlarePositions .SetOffset (
            (UINT32) pModelData ->GetPosition ());
        pModelData ->Write (m_afFlarePositions .GetData (),
                            m_afFlarePositions .GetCount () * 
                            sizeof (float));
    }

    //
    // Write the color shifts
    //

    if (m_avFlareColorShifts .GetCount () > 0)
    {
        pOutNode ->m_avFlareColorShifts .SetOffset (
            (UINT32) pModelData ->GetPosition ());
        pModelData ->Write (m_avFlareColorShifts .GetData (),
                            m_avFlareColorShifts .GetCount () * 
                            sizeof (CVector3));
    }

    //
    // Write the texture names
    //

    if (m_apszTextureNames .GetCount () > 0)
    {

        //
        // Initialize the offset to the offset array
        //

        size_t nPos = pModelData ->GetPosition ();
        pOutNode ->m_apszTextureNames .SetOffset ((UINT32) nPos);

        //
        // Compute the offset to the first string
        //

        nPos += m_apszTextureNames .GetCount () * sizeof (UINT32);

        //
        // Create the array of offsets one at a time
        //

        UINT32 ulOffset = (UINT32) nPos;
        for (int i = 0; i < (int) m_apszTextureNames .GetCount (); i++)
        {
            pModelData ->Write (&ulOffset, sizeof (ulOffset));
            ulOffset += (UINT32) (strlen (
                    m_apszTextureNames [i] .GetData ()) + 1);
        }

        //
        // Write the strings
        //

        for (int i = 0; i < (int) m_apszTextureNames .GetCount (); i++)
        {
            char *psz = m_apszTextureNames [i] .GetData ();
            pModelData ->Write (psz, strlen (psz) + 1);
        }
    }

    //
    // Serialize the node header
    //

    NodeHeaderSerialize (pOutNode, pModelData, pRawData);

    //
    // Rewrite the header
    //

    SerializeRewrite (pModelData, pOutNode, sizeof (*pOutNode), nPos);
    return;
}

//-----------------------------------------------------------------------------
//
// @func Serialize an emitter node
//
// @parm CNwnMdlEmitterNode * | pNode | Node being serialized
//
// @parm CNwnStream * | pModelData | Model data stream
//
// @parm CNwnStream * | pRawData | Raw data stream
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlEmitterNode::_Serialize (
    CNwnStream *pModelData, CNwnStream *pRawData)
{

    //
    // Create a copy of the object.
    //
    // NOTE: We DON'T want the normal destructors to execute
    //

    UINT8 auchData [sizeof (CNwnMdlEmitterNode)];
    CNwnMdlEmitterNode *pOutNode = (CNwnMdlEmitterNode *) auchData;
    memcpy (pOutNode, this, sizeof (*pOutNode));

    //
    // Write the header
    //

    size_t nPos = pModelData ->GetPosition ();
    pModelData ->Write (pOutNode, sizeof (*pOutNode));

    //
    // Serialize the node header
    //

    NodeHeaderSerialize (pOutNode, pModelData, pRawData);

    //
    // Rewrite the header
    //

    SerializeRewrite (pModelData, pOutNode, sizeof (*pOutNode), nPos);
    return;
}

//-----------------------------------------------------------------------------
//
// @func Serialize a camera object
//
// @parm CNwnStream * | pModelData | Model data stream
//
// @parm CNwnStream * | pRawData | Raw data stream
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlCameraNode::_Serialize (
    CNwnStream *pModelData, CNwnStream *pRawData)
{

    //
    // Create a copy of the object.
    //
    // NOTE: We DON'T want the normal destructors to execute
    //

    UINT8 auchData [sizeof (CNwnMdlCameraNode)];
    CNwnMdlCameraNode *pOutNode = (CNwnMdlCameraNode *) auchData;
    memcpy (pOutNode, this, sizeof (*pOutNode));

    //
    // Write the header
    //

    size_t nPos = pModelData ->GetPosition ();
    pModelData ->Write (pOutNode, sizeof (*pOutNode));

    //
    // Serialize the node header
    //

    NodeHeaderSerialize (pOutNode, pModelData, pRawData);

    //
    // Rewrite the header
    //

    SerializeRewrite (pModelData, pOutNode, sizeof (*pOutNode), nPos);
    return;
}

//-----------------------------------------------------------------------------
//
// @func Serialize a reference object
//
// @parm CNwnStream * | pModelData | Model data stream
//
// @parm CNwnStream * | pRawData | Raw data stream
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlReferenceNode::_Serialize (
    CNwnStream *pModelData, CNwnStream *pRawData)
{

    //
    // Create a copy of the object.
    //
    // NOTE: We DON'T want the normal destructors to execute
    //

    UINT8 auchData [sizeof (CNwnMdlReferenceNode)];
    CNwnMdlReferenceNode *pOutNode = (CNwnMdlReferenceNode *) auchData;
    memcpy (pOutNode, this, sizeof (*pOutNode));

    //
    // Write the header
    //

    size_t nPos = pModelData ->GetPosition ();
    pModelData ->Write (pOutNode, sizeof (*pOutNode));

    //
    // Serialize the node header
    //

    NodeHeaderSerialize (pOutNode, pModelData, pRawData);

    //
    // Rewrite the header
    //

    SerializeRewrite (pModelData, pOutNode, sizeof (*pOutNode), nPos);
    return;
}

//-----------------------------------------------------------------------------
//
// @func Serialize a mesh object
//
// @parm CNwnMdlMeshHeader * | pOutNode | Output copy of the node
//              being serialized
//
// @parm CNwnStream * | pModelData | Model data stream
//
// @parm CNwnStream * | pRawData | Raw data stream
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlMeshHeader::MeshHeaderSerialize (CNwnMdlMeshHeader *pOutNode, 
                                             CNwnStream *pModelData, CNwnStream *pRawData)
{

    //
    // Prepare the data
    //

    pOutNode ->m_sFaces .RelativePrepare ();
    pOutNode ->m_sVertexIndices .Initialize ();
    pOutNode ->m_sLeftOverFaces .RelativePrepare ();
    pOutNode ->m_sVertexIndicesCount .RelativePrepare ();
    pOutNode ->m_sRawVertexIndices .RelativePrepare ();
    pOutNode ->m_pavVerts .Initialize ();
    pOutNode ->m_pavTVerts [0] .Initialize ();
    pOutNode ->m_pavTVerts [1] .Initialize ();
    pOutNode ->m_pavTVerts [2] .Initialize ();
    pOutNode ->m_pavTVerts [3] .Initialize ();
    pOutNode ->m_pavNormals .Initialize ();
    pOutNode ->m_paColors .Initialize ();
    pOutNode ->m_pavBumpmapAnim1 .Initialize ();
    pOutNode ->m_pavBumpmapAnim2 .Initialize ();
    pOutNode ->m_pavBumpmapAnim3 .Initialize ();
    pOutNode ->m_pavBumpmapAnim4 .Initialize ();
    pOutNode ->m_pavBumpmapAnim5 .Initialize ();
    pOutNode ->m_pafBumpmapAnim6 .Initialize ();
    assert (pOutNode ->m_ulSomething3Offset == 0xFFFFFFFF);

    //
    // Write the faces
    //

    if (m_sFaces .GetCount () > 0)
    {
        pOutNode ->m_sFaces .SetOffset (
            (UINT32) pModelData ->GetPosition ());
        pModelData ->Write (m_sFaces .GetData (),
                            m_sFaces .GetCount () * 
                            sizeof (NwnMdlFace));
    }

    //
    // Write the left over faces
    //

    if (m_sLeftOverFaces .GetCount () > 0)
    {
        pOutNode ->m_sLeftOverFaces .SetOffset (
            (UINT32) pModelData ->GetPosition ());
        pModelData ->Write (m_sLeftOverFaces .GetData (),
                            m_sLeftOverFaces .GetCount () * 
                            sizeof (UINT32));
    }

    //
    // Write the vertex indices count
    //

    if (m_sVertexIndicesCount .GetCount () > 0)
    {
        pOutNode ->m_sVertexIndicesCount .SetOffset (
            (UINT32) pModelData ->GetPosition ());
        pModelData ->Write (m_sVertexIndicesCount .GetData (),
                            m_sVertexIndicesCount .GetCount () * 
                            sizeof (UINT32));
    }

    //
    // Write the vertices
    //

    if (m_pavVerts .GetDataNZ (NULL) != NULL)
    {
        pOutNode ->m_pavVerts .SetOffset (
            (UINT32) pRawData ->GetPosition ());
        pRawData ->Write (m_pavVerts .GetData (),
                          m_usVertexCount * sizeof (CVector3));
    }

    //
    // Write the tverts
    //

    for (int i = 0; i < 4; i++)
    {
        if (m_pavTVerts [i] .GetDataNZ (NULL) != NULL)
        {
            pOutNode ->m_pavTVerts [i] .SetOffset (
                (UINT32) pRawData ->GetPosition ());
            pRawData ->Write (m_pavTVerts [i] .GetData (),
                              m_usVertexCount * sizeof (CVector2));
        }
    }

    //
    // Write the normals
    //

    if (m_pavNormals .GetDataNZ (NULL) != NULL)
    {
        pOutNode ->m_pavNormals .SetOffset (
            (UINT32) pRawData ->GetPosition ());
        pRawData ->Write (m_pavNormals .GetData (),
                          m_usVertexCount * sizeof (CVector3));
    }

    //
    // Write the colors
    //

    if (m_paColors .GetDataNZ (NULL) != NULL)
    {
        pOutNode ->m_paColors .SetOffset (
            (UINT32) pRawData ->GetPosition ());
        pRawData ->Write (m_paColors .GetData (),
                          m_usVertexCount * sizeof (UINT32));
    }

    //
    // Write the bumpmap data 1
    //

    if (m_pavBumpmapAnim1 .GetDataNZ (NULL) != NULL)
    {
        pOutNode ->m_pavBumpmapAnim1 .SetOffset (
            (UINT32) pRawData ->GetPosition ());
        pRawData ->Write (m_pavBumpmapAnim1 .GetData (),
                          m_usVertexCount * sizeof (CVector3));
    }

    //
    // Write the bumpmap data 2
    //

    if (m_pavBumpmapAnim2 .GetDataNZ (NULL) != NULL)
    {
        pOutNode ->m_pavBumpmapAnim2 .SetOffset (
            (UINT32) pRawData ->GetPosition ());
        pRawData ->Write (m_pavBumpmapAnim2 .GetData (),
                          m_usVertexCount * sizeof (CVector3));
    }

    //
    // Write the bumpmap data 3
    //

    if (m_pavBumpmapAnim3 .GetDataNZ (NULL) != NULL)
    {
        pOutNode ->m_pavBumpmapAnim3 .SetOffset (
            (UINT32) pRawData ->GetPosition ());
        pRawData ->Write (m_pavBumpmapAnim3 .GetData (),
                          m_usVertexCount * sizeof (CVector3));
    }

    //
    // Write the bumpmap data 4
    //

    if (m_pavBumpmapAnim4 .GetDataNZ (NULL) != NULL)
    {
        pOutNode ->m_pavBumpmapAnim4 .SetOffset (
            (UINT32) pRawData ->GetPosition ());
        pRawData ->Write (m_pavBumpmapAnim4 .GetData (),
                          m_usVertexCount * sizeof (CVector3));
    }

    //
    // Write the bumpmap data 5
    //

    if (m_pavBumpmapAnim5 .GetDataNZ (NULL) != NULL)
    {
        pOutNode ->m_pavBumpmapAnim5 .SetOffset (
            (UINT32) pRawData ->GetPosition ());
        pRawData ->Write (m_pavBumpmapAnim5 .GetData (),
                          m_usVertexCount * sizeof (CVector3));
    }

    //
    // Write the bumpmap data 6
    //

    if (m_pafBumpmapAnim6 .GetDataNZ (NULL) != NULL)
    {
        pOutNode ->m_pafBumpmapAnim6 .SetOffset (
            (UINT32) pRawData ->GetPosition ());
        pRawData ->Write (m_pafBumpmapAnim6 .GetData (),
                          m_usVertexCount * sizeof (float));
    }

    //
    // Write the vertex index list
    //

    if (m_sRawVertexIndices .GetCount () > 0)
    {

        //
        // Initialize the offset to the offset array
        //

        size_t nPos = pModelData ->GetPosition ();
        pOutNode ->m_sRawVertexIndices .SetOffset ((UINT32) nPos);

        //
        // Create the array of offsets one at a time
        //

        UINT32 ulOffset = (UINT32) pRawData ->GetPosition ();
        for (int i = 0; i < (int) m_sRawVertexIndices .GetCount (); i++)
        {
            pModelData ->Write (&ulOffset, sizeof (ulOffset));
            ulOffset += (UINT32) (m_sVertexIndicesCount [i] * sizeof (UINT16));
        }

        //
        // Write the offsets
        //

        for (int i = 0; i < (int) m_sRawVertexIndices .GetCount (); i++)
        {
            UINT16 *pus = m_sRawVertexIndices [i] .GetData ();
            pRawData ->Write (pus, m_sVertexIndicesCount [i] * sizeof (UINT16));
        }
    }

    //
    // Others not done yet
    //

    // m_ulSomething3Offset

    //
    // Serialize the core node
    //

    NodeHeaderSerialize (pOutNode, pModelData, pRawData);
}

//-----------------------------------------------------------------------------
//
// @func Serialize a trimesh object
//
// @parm CNwnStream * | pModelData | Model data stream
//
// @parm CNwnStream * | pRawData | Raw data stream
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlTriMeshNode::_Serialize (
    CNwnStream *pModelData, CNwnStream *pRawData)
{

    //
    // Create a copy of the object.
    //
    // NOTE: We DON'T want the normal destructors to execute
    //

    UINT8 auchData [sizeof (CNwnMdlTriMeshNode)];
    CNwnMdlTriMeshNode *pOutNode = (CNwnMdlTriMeshNode *) auchData;
    memcpy (pOutNode, this, sizeof (*pOutNode));

    //
    // Write the header
    //

    size_t nPos = pModelData ->GetPosition ();
    pModelData ->Write (pOutNode, sizeof (*pOutNode));

    //
    // Serialize the mesh header
    //

    MeshHeaderSerialize (pOutNode, pModelData, pRawData);

    //
    // Rewrite the header
    //

    SerializeRewrite (pModelData, pOutNode, sizeof (*pOutNode), nPos);
    return;
}

//-----------------------------------------------------------------------------
//
// @func Serialize a new object 
//
// @parm CNwnStream * | pModelData | Model data stream
//
// @parm CNwnStream * | pRawData | Raw data stream
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlSkinMeshNode::_Serialize (
    CNwnStream *pModelData, CNwnStream *pRawData)
{

    //
    // Create a copy of the object.
    //
    // NOTE: We DON'T want the normal destructors to execute
    //

    UINT8 auchData [sizeof (CNwnMdlSkinMeshNode)];
    CNwnMdlSkinMeshNode *pOutNode = (CNwnMdlSkinMeshNode *) auchData;
    memcpy (pOutNode, this, sizeof (*pOutNode));

    //
    // Initialize lists and such
    //

    pOutNode ->m_aWeights .RelativePrepare ();
    pOutNode ->m_aQBoneRefInv .RelativePrepare ();
    pOutNode ->m_aTBoneRefInv .RelativePrepare ();
    pOutNode ->m_aBoneConstantIndices .RelativePrepare ();
    pOutNode ->m_pafSkinWeights .Initialize ();
    pOutNode ->m_pasSkinBoneRefs .Initialize ();
    pOutNode ->m_pasNodeToBoneMap .Initialize ();

    //
    // Write the header
    //

    size_t nPos = pModelData ->GetPosition ();
    pModelData ->Write (pOutNode, sizeof (*pOutNode));

    //
    // Write the weights
    //

    if (m_pafSkinWeights .GetDataNZ (NULL) != NULL)
    {
        pOutNode ->m_pafSkinWeights .SetOffset (
            (UINT32) pRawData ->GetPosition ());
        pRawData ->Write (m_pafSkinWeights .GetData (),
                          m_usVertexCount * sizeof (float) * 4);
    }

    //
    // Write the skin bone refs
    //

    if (m_pasSkinBoneRefs .GetDataNZ (NULL) != NULL)
    {
        pOutNode ->m_pasSkinBoneRefs .SetOffset (
            (UINT32) pRawData ->GetPosition ());
        pRawData ->Write (m_pasSkinBoneRefs .GetData (),
                          m_usVertexCount * sizeof (INT16) * 4);
    }

    //
    // The node to bone map
    //

    if (m_pasNodeToBoneMap .GetDataNZ (NULL) != NULL)
    {
        pOutNode ->m_pasNodeToBoneMap .SetOffset (
            (UINT32) pModelData ->GetPosition ());
        pModelData ->Write (m_pasNodeToBoneMap .GetData (),
                            m_ulNodeToBoneCount * sizeof (INT16));
    }

    //
    // Write the QBoneRefInv
    //

    if (m_aQBoneRefInv .GetCount () > 0)
    {
        pOutNode ->m_aQBoneRefInv .SetOffset (
            (UINT32) pModelData ->GetPosition ());
        pModelData ->Write (m_aQBoneRefInv .GetData (),
                            m_aQBoneRefInv .GetCount () * sizeof (CQuaternion));
    }

    //
    // Write the TBoneRefInv
    //

    if (m_aTBoneRefInv .GetCount () > 0)
    {
        pOutNode ->m_aTBoneRefInv .SetOffset (
            (UINT32) pModelData ->GetPosition ());
        pModelData ->Write (m_aTBoneRefInv .GetData (),
                            m_aTBoneRefInv .GetCount () * 
                            sizeof (CVector3));
    }

    //
    // Write the BoneConstantIndices
    //

    if (m_aBoneConstantIndices .GetCount () > 0)
    {
        pOutNode ->m_aBoneConstantIndices .SetOffset (
            (UINT32) pModelData ->GetPosition ());
        pModelData ->Write (m_aBoneConstantIndices .GetData (),
                            m_aBoneConstantIndices .GetCount () * 
                            sizeof (UINT32));
    }

    //
    // Serialize the mesh header
    //

    MeshHeaderSerialize (pOutNode, pModelData, pRawData);

    //
    // Rewrite the header
    //

    SerializeRewrite (pModelData, pOutNode, sizeof (*pOutNode), nPos);
    return;
}

//-----------------------------------------------------------------------------
//
// @func Serialize a new object 
//
// @parm CNwnStream * | pModelData | Model data stream
//
// @parm CNwnStream * | pRawData | Raw data stream
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlAnimMeshNode::_Serialize (
    CNwnStream *pModelData, CNwnStream *pRawData)
{

    //
    // Create a copy of the object.
    //
    // NOTE: We DON'T want the normal destructors to execute
    //

    UINT8 auchData [sizeof (CNwnMdlAnimMeshNode)];
    CNwnMdlAnimMeshNode *pOutNode = (CNwnMdlAnimMeshNode *) auchData;
    memcpy (pOutNode, this, sizeof (*pOutNode));

    //
    // Initialize lists and such
    //

    pOutNode ->m_avAnimVerts .RelativePrepare ();
    pOutNode ->m_avAnimTVerts .RelativePrepare ();
    pOutNode ->m_avAnimNormals .RelativePrepare ();
    pOutNode ->m_pvVertexSets .Initialize ();
    pOutNode ->m_pvTextureSets .Initialize ();

    //
    // Write the header
    //

    size_t nPos = pModelData ->GetPosition ();
    pModelData ->Write (pOutNode, sizeof (*pOutNode));

    //
    // Write the AnimVerts
    //

    if (m_avAnimVerts .GetCount () > 0)
    {
        pOutNode ->m_avAnimVerts .SetOffset (
            (UINT32) pModelData ->GetPosition ());
        pModelData ->Write (m_avAnimVerts .GetData (),
                            m_avAnimVerts .GetCount () * 
                            sizeof (CVector3));
    }

    //
    // Write the AnimTVerts
    //

    if (m_avAnimTVerts .GetCount () > 0)
    {
        pOutNode ->m_avAnimTVerts .SetOffset (
            (UINT32) pModelData ->GetPosition ());
        pModelData ->Write (m_avAnimTVerts .GetData (),
                            m_avAnimTVerts .GetCount () * 
                            sizeof (CVector3));
    }

    //
    // Write the AnimNormals
    //

    if (m_avAnimNormals .GetCount () > 0)
    {
        pOutNode ->m_avAnimNormals .SetOffset (
            (UINT32) pModelData ->GetPosition ());
        pModelData ->Write (m_avAnimNormals .GetData (),
                            m_avAnimNormals .GetCount () * 
                            sizeof (CVector3));
    }

    //
    // Write the vertex array
    //

    if (m_pvVertexSets .GetDataNZ (NULL) != NULL)
    {
        pOutNode ->m_pvVertexSets .SetOffset (
            (UINT32) pModelData ->GetPosition ());
        pModelData ->Write (m_pvVertexSets .GetData (),
                            m_usVertexCount * m_ulVertexSetCount * 
                            sizeof (CVector3));
    }

    //
    // Write the texture array
    //

    if (m_pvTextureSets .GetDataNZ (NULL) != NULL)
    {
        pOutNode ->m_pvTextureSets .SetOffset (
            (UINT32) pModelData ->GetPosition ());
        pModelData ->Write (m_pvTextureSets .GetData (),
                            m_usVertexCount * m_ulTextureSetCount * 
                            sizeof (CVector2));
    }

    //
    // Serialize the mesh header
    //

    MeshHeaderSerialize (pOutNode, pModelData, pRawData);

    //
    // Rewrite the header
    //

    SerializeRewrite (pModelData, pOutNode, sizeof (*pOutNode), nPos);
    return;
}

//-----------------------------------------------------------------------------
//
// @func Serialize a new object 
//
// @parm CNwnStream * | pModelData | Model data stream
//
// @parm CNwnStream * | pRawData | Raw data stream
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlDanglyMeshNode::_Serialize (
    CNwnStream *pModelData, CNwnStream *pRawData)
{

    //
    // Create a copy of the object.
    //
    // NOTE: We DON'T want the normal destructors to execute
    //

    UINT8 auchData [sizeof (CNwnMdlDanglyMeshNode)];
    CNwnMdlDanglyMeshNode *pOutNode = (CNwnMdlDanglyMeshNode *) auchData;
    memcpy (pOutNode, this, sizeof (*pOutNode));

    //
    // Initialize lists and such
    //

    pOutNode ->m_afConstraints .RelativePrepare ();

    //
    // Write the header
    //

    size_t nPos = pModelData ->GetPosition ();
    pModelData ->Write (pOutNode, sizeof (*pOutNode));

    //
    // Write the Constraints
    //

    if (m_afConstraints .GetCount () > 0)
    {
        pOutNode ->m_afConstraints .SetOffset (
            (UINT32) pModelData ->GetPosition ());
        pModelData ->Write (m_afConstraints .GetData (),
                            m_afConstraints .GetCount () * 
                            sizeof (float));
    }

    //
    // Serialize the mesh header
    //

    MeshHeaderSerialize (pOutNode, pModelData, pRawData);

    //
    // Rewrite the header
    //

    SerializeRewrite (pModelData, pOutNode, sizeof (*pOutNode), nPos);
    return;
}

//-----------------------------------------------------------------------------
//
// @func Serialize a new object 
//
// @parm CNwnStream * | pModelData | Model data stream
//
// @parm CNwnStream * | pRawData | Raw data stream
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlAABBMeshNode::_Serialize (
    CNwnStream *pModelData, CNwnStream *pRawData)
{

    //
    // Create a copy of the object.
    //
    // NOTE: We DON'T want the normal destructors to execute
    //

    UINT8 auchData [sizeof (CNwnMdlAABBMeshNode)];
    CNwnMdlAABBMeshNode *pOutNode = (CNwnMdlAABBMeshNode *) auchData;
    memcpy (pOutNode, this, sizeof (*pOutNode));

    //
    // Write the header
    //

    size_t nPos = pModelData ->GetPosition ();
    pModelData ->Write (pOutNode, sizeof (*pOutNode));

    //
    // Write the AABB tree
    //

    if (m_pAABBTree .GetData () != NULL)
    {
        pOutNode ->m_pAABBTree .SetOffset ((UINT32) (nPos + sizeof (*pOutNode)));
        SerializeAABBEntry (m_pAABBTree .GetData (), pModelData);
    }
    else
        pOutNode ->m_pAABBTree .SetOffset (0);

    //
    // Serialize the mesh header
    //

    MeshHeaderSerialize (pOutNode, pModelData, pRawData);

    //
    // Rewrite the header
    //

    SerializeRewrite (pModelData, pOutNode, sizeof (*pOutNode), nPos);
}

//-----------------------------------------------------------------------------
//
// @func Serialize an AABB Entry
//
// @parm NwnMdlAABBEntry * | pAABBEntry | Entry being serialized
//
// @parm CNwnStream * | pModelData | Model data stream
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlAABBMeshNode::SerializeAABBEntry (
    NwnMdlAABBEntry *pAABBEntry, CNwnStream *pModelData)
{

    //
    // Write the entry
    //

    NwnMdlAABBEntry sEntry;
    memcpy (&sEntry, pAABBEntry, sizeof (sEntry));
    int nPos = (int) pModelData ->GetPosition ();
    pModelData ->Write (&sEntry, sizeof (sEntry));

    //
    // If we have children, write them
    //

    if (pAABBEntry ->ulLeafFace == 0xFFFFFFFF)
    {
        sEntry .psLeft .SetOffset ((UINT32) pModelData ->GetPosition ());
        SerializeAABBEntry (pAABBEntry ->psLeft .GetData (), pModelData);
        sEntry .psRight .SetOffset ((UINT32) pModelData ->GetPosition ());
        SerializeAABBEntry (pAABBEntry ->psRight .GetData (), pModelData);
    }
    else
    {
        sEntry .psLeft .Initialize ();
        sEntry .psRight .Initialize ();
    }

    //
    // Rewrite the myself
    //

    SerializeRewrite (pModelData, &sEntry, sizeof (sEntry), nPos);
    return;
}

//-----------------------------------------------------------------------------
//
// @func Serialize a node
//
// @parm CNwnStream * | pModelData | Model data stream
//
// @parm CNwnStream * | pRawData | Raw data stream
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlNodeHeader::Serialize (
    CNwnStream *pModelData, CNwnStream *pRawData)
{

    //
    // Serialize the data
    //

    if (m_ulFlags == NwnMdlNT_Light)
        static_cast <CNwnMdlLightNode *> (this) ->_Serialize (pModelData, pRawData);
    else if (m_ulFlags == NwnMdlNT_Emitter)
        static_cast <CNwnMdlEmitterNode *> (this) ->_Serialize (pModelData, pRawData);
    else if (m_ulFlags == NwnMdlNT_Camera)
        static_cast <CNwnMdlCameraNode *> (this) ->_Serialize (pModelData, pRawData);
    else if (m_ulFlags == NwnMdlNT_Reference)
        static_cast <CNwnMdlReferenceNode *> (this) ->_Serialize (pModelData, pRawData);
    else if (m_ulFlags == NwnMdlNT_TriMesh)
        static_cast <CNwnMdlTriMeshNode *> (this) ->_Serialize (pModelData, pRawData);
    else if (m_ulFlags == NwnMdlNT_SkinMesh)
        static_cast <CNwnMdlSkinMeshNode *> (this) ->_Serialize (pModelData, pRawData);
    else if (m_ulFlags == NwnMdlNT_AnimMesh)
        static_cast <CNwnMdlAnimMeshNode *> (this) ->_Serialize (pModelData, pRawData);
    else if (m_ulFlags == NwnMdlNT_DanglyMesh)
        static_cast <CNwnMdlDanglyMeshNode *> (this) ->_Serialize (pModelData, pRawData);
    else if (m_ulFlags == NwnMdlNT_AABBMesh)
        static_cast <CNwnMdlAABBMeshNode *> (this) ->_Serialize (pModelData, pRawData);
    else if (m_ulFlags == NwnMdlNT_Dummy)
        static_cast <CNwnMdlDummyNode *> (this) ->_Serialize (pModelData, pRawData);
    else
    {
        static_cast <CNwnMdlDummyNode *> (this) ->_Serialize (pModelData, pRawData);
        assert (false);
    }
}

/* vim: set sw=4: */
