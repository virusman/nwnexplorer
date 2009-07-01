//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnMdlR2A.cpp - Model relative to absolute address |
//
// This module contains the general model support.
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
// @mfunc Convert from relative to absolute addressing
//
// @parm unsigned char * | pauchModelData | Pointer to the model data
//
// @parm unsigned char * | pauchRawData | Pointer to the raw data
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlGeometryHeader::RelativeToAbsolute (
    unsigned char *pauchModelData, unsigned char *pauchRawData)
{

    //
    // Set as binary
    //

    m_ucGeometryType |= NwnMdlGF_LoadedBinary;

    //
    // Convert the geometry
    //

    m_pGeometry .ConvertToAbsoluteNZ (pauchModelData);
    if (m_pGeometry .GetData () != NULL)
    {
        m_pGeometry .GetData () ->RelativeToAbsolute (
            pauchModelData, pauchRawData);
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Convert from relative to absolute addressing
//
// @parm unsigned char * | pauchModelData | Pointer to the model data
//
// @parm unsigned char * | pauchRawData | Pointer to the raw data
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlModel::RelativeToAbsolute (
    unsigned char *pauchModelData, unsigned char *pauchRawData)
{

    //
    // Clear the super model pointer.  It isn't valid
    //

    m_pSuperModel .Initialize ();

    //
    // Invoke base class
    //

    CNwnMdlGeometryHeader::RelativeToAbsolute (pauchModelData, pauchRawData);

    //
    // Convert the animations
    //

    m_apAnimations .ConvertToAbsoluteNZ (pauchModelData);
    for (int i = 0; i < (int) m_apAnimations .GetCount (); i++)
    {
        m_apAnimations [i] .ConvertToAbsolute (pauchModelData);
        m_apAnimations [i] .GetData () ->RelativeToAbsolute (
            pauchModelData, pauchRawData);
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Convert from relative to absolute addressing
//
// @parm unsigned char * | pauchModelData | Pointer to the model data
//
// @parm unsigned char * | pauchRawData | Pointer to the raw data
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlAnimation::RelativeToAbsolute (
    unsigned char *pauchModelData, unsigned char *pauchRawData)
{

    //
    // Invoke base class
    //

    CNwnMdlGeometryHeader::RelativeToAbsolute (pauchModelData, pauchRawData);

    //
    // Convert the events
    //

    m_asEvents .ConvertToAbsolute (pauchModelData);
}

//-----------------------------------------------------------------------------
//
// @mfunc Convert from relative to absolute addressing
//
// @parm unsigned char * | pauchModelData | Pointer to the model data
//
// @parm unsigned char * | pauchRawData | Pointer to the raw data
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlNodeHeader::_R2A (unsigned char *pauchModelData, 
                              unsigned char *pauchRawData)
{

    //
    // Initialize reference pointers
    //

    m_pGeometryHeader .ConvertToAbsolute (pauchModelData);
    m_pParentNode .ConvertToAbsoluteNZ (pauchModelData);

    //
    // Initialize the child list
    //

    m_apChildren .ConvertToAbsoluteNZ (pauchModelData);
    for (int i = 0; i < (int) m_apChildren .GetCount (); i++)
    {
        m_apChildren [i] .ConvertToAbsolute (pauchModelData);
        m_apChildren [i] .GetData () ->RelativeToAbsolute (
            pauchModelData, pauchRawData);
    }

    //
    // Initialize controller lists
    //

    m_aControllerKey .ConvertToAbsoluteNZ (pauchModelData);
    m_aControllerData .ConvertToAbsoluteNZ (pauchModelData);
}

//-----------------------------------------------------------------------------
//
// @mfunc Convert from relative to absolute addressing
//
// @parm unsigned char * | pauchModelData | Pointer to the model data
//
// @parm unsigned char * | pauchRawData | Pointer to the raw data
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlDummyNode::_R2A (unsigned char *pauchModelData, 
                             unsigned char *pauchRawData)
{

    //
    // Invoke the base class
    //

    CNwnMdlNodeHeader::_R2A (pauchModelData, pauchRawData);
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Convert from relative to absolute addressing
//
// @parm unsigned char * | pauchModelData | Pointer to the model data
//
// @parm unsigned char * | pauchRawData | Pointer to the raw data
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlLightNode::_R2A (unsigned char *pauchModelData, 
                             unsigned char *pauchRawData)
{

    //
    // Invoke the base class
    //

    CNwnMdlNodeHeader::_R2A (pauchModelData, pauchRawData);

    //
    // Update the flare textures
    //

    m_apszTextureNames .ConvertToAbsoluteNZ (pauchModelData);
    for (int i = 0; i < (int) m_apszTextureNames .GetCount (); i++)
        m_apszTextureNames [i] .ConvertToAbsolute (pauchModelData);

    //
    // Update flare data
    //

    m_afFlareSizes .ConvertToAbsoluteNZ (pauchModelData);
    m_afFlarePositions .ConvertToAbsoluteNZ (pauchModelData);
    m_avFlareColorShifts .ConvertToAbsoluteNZ (pauchModelData);
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Convert from relative to absolute addressing
//
// @parm unsigned char * | pauchModelData | Pointer to the model data
//
// @parm unsigned char * | pauchRawData | Pointer to the raw data
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlEmitterNode::_R2A (unsigned char *pauchModelData, 
                               unsigned char *pauchRawData)
{

    //
    // Invoke the base class
    //

    CNwnMdlNodeHeader::_R2A (pauchModelData, pauchRawData);
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Convert from relative to absolute addressing
//
// @parm unsigned char * | pauchModelData | Pointer to the model data
//
// @parm unsigned char * | pauchRawData | Pointer to the raw data
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlCameraNode::_R2A (unsigned char *pauchModelData, 
                              unsigned char *pauchRawData)
{

    //
    // Invoke the base class
    //

    CNwnMdlNodeHeader::_R2A (pauchModelData, pauchRawData);
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Convert from relative to absolute addressing
//
// @parm unsigned char * | pauchModelData | Pointer to the model data
//
// @parm unsigned char * | pauchRawData | Pointer to the raw data
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlReferenceNode::_R2A (unsigned char *pauchModelData, 
                                 unsigned char *pauchRawData)
{

    //
    // Invoke the base class
    //

    CNwnMdlNodeHeader::_R2A (pauchModelData, pauchRawData);
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Convert from relative to absolute addressing
//
// @parm unsigned char * | pauchModelData | Pointer to the model data
//
// @parm unsigned char * | pauchRawData | Pointer to the raw data
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlMeshHeader::_R2A (unsigned char *pauchModelData, 
                              unsigned char *pauchRawData)
{

    //
    // Invoke the base class
    //

    CNwnMdlNodeHeader::_R2A (pauchModelData, pauchRawData);

    //
    // Convert the addresses
    //

    m_sFaces .ConvertToAbsoluteNZ (pauchModelData);

    //
    // Initialize the faces
    //

    m_sFaces .ConvertToAbsoluteNZ (pauchModelData);
    m_sVertexIndices .ConvertToAbsoluteNZ (pauchModelData);
    m_sVertexIndicesCount .ConvertToAbsoluteNZ (pauchModelData);

    //
    // Initialize the vertex indices offset array
    //

    for (int i = 0; i < (int) m_sVertexIndices .GetCount (); i++)
        m_sVertexIndices [i] .ConvertToAbsolute (pauchModelData);

    //
    // Initialize the left over faces
    //

    m_sLeftOverFaces .ConvertToAbsoluteNZ (pauchModelData);

    //
    // Initialize the raw lists
    //

    m_pavVerts .ConvertToAbsoluteNZ (pauchRawData);
    m_pavTVerts [0] .ConvertToAbsoluteNZ (pauchRawData);
    m_pavTVerts [1] .ConvertToAbsoluteNZ (pauchRawData);
    m_pavTVerts [2] .ConvertToAbsoluteNZ (pauchRawData);
    m_pavTVerts [3] .ConvertToAbsoluteNZ (pauchRawData);
    m_pavNormals .ConvertToAbsoluteNZ (pauchRawData);
    m_paColors .ConvertToAbsoluteNZ (pauchRawData);
    m_pavBumpmapAnim1 .ConvertToAbsoluteNZ (pauchRawData);
    m_pavBumpmapAnim2 .ConvertToAbsoluteNZ (pauchRawData);
    m_pavBumpmapAnim3 .ConvertToAbsoluteNZ (pauchRawData);
    m_pavBumpmapAnim4 .ConvertToAbsoluteNZ (pauchRawData);
    m_pavBumpmapAnim5 .ConvertToAbsoluteNZ (pauchRawData);
    m_pafBumpmapAnim6 .ConvertToAbsoluteNZ (pauchRawData);

    //
    // Initialize the raw vertex indices
    //

    m_sRawVertexIndices .ConvertToAbsoluteNZ (pauchModelData);
    for (int i = 0; i < (int) m_sRawVertexIndices .GetCount (); i++)
        m_sRawVertexIndices [i] .ConvertToAbsoluteNZ (pauchRawData);

    //
    // Initialzie the triagle strip information
    //

    // m_ulSomething3Offset (UINT16!!!!!)
}

//-----------------------------------------------------------------------------
//
// @mfunc Convert from relative to absolute addressing
//
// @parm unsigned char * | pauchModelData | Pointer to the model data
//
// @parm unsigned char * | pauchRawData | Pointer to the raw data
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlTriMeshNode::_R2A (unsigned char *pauchModelData, 
                               unsigned char *pauchRawData)
{

    //
    // Invoke the base class
    //

    CNwnMdlMeshHeader::_R2A (pauchModelData, pauchRawData);

}

//-----------------------------------------------------------------------------
//
// @mfunc Convert from relative to absolute addressing
//
// @parm unsigned char * | pauchModelData | Pointer to the model data
//
// @parm unsigned char * | pauchRawData | Pointer to the raw data
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlSkinMeshNode::_R2A (unsigned char *pauchModelData, 
                                unsigned char *pauchRawData)
{

    //
    // Invoke the base class
    //

    CNwnMdlMeshHeader::_R2A (pauchModelData, pauchRawData);

    //
    // Initialize the arrays
    //

    m_aWeights .ConvertToAbsoluteNZ (pauchModelData);
    m_aQBoneRefInv .ConvertToAbsoluteNZ (pauchModelData);
    m_aTBoneRefInv .ConvertToAbsoluteNZ (pauchModelData);
    m_aBoneConstantIndices .ConvertToAbsoluteNZ (pauchModelData);
    m_pafSkinWeights .ConvertToAbsoluteNZ (pauchRawData);
    m_pasSkinBoneRefs .ConvertToAbsoluteNZ (pauchRawData);
    m_pasNodeToBoneMap .ConvertToAbsoluteNZ (pauchModelData);
}

//-----------------------------------------------------------------------------
//
// @mfunc Convert from relative to absolute addressing
//
// @parm unsigned char * | pauchModelData | Pointer to the model data
//
// @parm unsigned char * | pauchRawData | Pointer to the raw data
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlAnimMeshNode::_R2A (unsigned char *pauchModelData, 
                                unsigned char *pauchRawData)
{

    //
    // Invoke the base class
    //

    CNwnMdlMeshHeader::_R2A (pauchModelData, pauchRawData);

    //
    // Initialize the arrays
    //

    m_avAnimVerts .ConvertToAbsoluteNZ (pauchModelData);
    m_avAnimTVerts .ConvertToAbsoluteNZ (pauchModelData);
    m_avAnimNormals .ConvertToAbsoluteNZ (pauchModelData);
    m_pvVertexSets .ConvertToAbsoluteNZ (pauchRawData);
    m_pvTextureSets .ConvertToAbsoluteNZ (pauchRawData);
}

//-----------------------------------------------------------------------------
//
// @mfunc Convert from relative to absolute addressing
//
// @parm unsigned char * | pauchModelData | Pointer to the model data
//
// @parm unsigned char * | pauchRawData | Pointer to the raw data
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlDanglyMeshNode::_R2A (unsigned char *pauchModelData, 
                                  unsigned char *pauchRawData)
{

    //
    // Invoke the base class
    //

    CNwnMdlMeshHeader::_R2A (pauchModelData, pauchRawData);

    //
    // Initialize the arrays
    //

    m_afConstraints .ConvertToAbsoluteNZ (pauchModelData);
}

//-----------------------------------------------------------------------------
//
// @mfunc Convert from relative to absolute addressing
//
// @parm unsigned char * | pauchModelData | Pointer to the model data
//
// @parm unsigned char * | pauchRawData | Pointer to the raw data
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlAABBMeshNode::_R2A (unsigned char *pauchModelData, 
                                unsigned char *pauchRawData)
{

    //
    // Invoke the base class
    //

    CNwnMdlMeshHeader::_R2A (pauchModelData, pauchRawData);

    //
    // Initialize the arrays
    //

    _R2AAABBEntry (m_pAABBTree, pauchModelData);
}

//-----------------------------------------------------------------------------
//
// @func Free an AABB Entry
//
// @parm CNwnPointer <NwnMdlAABBEntry> & | pAABBEntry | Entry being converted
//
// @parm unsigned char * | pauchModelData | Pointer to the model data
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlAABBMeshNode::_R2AAABBEntry (
    CNwnPointer <NwnMdlAABBEntry> &pAABBEntry,
    unsigned char *pauchModelData)
{

    //
    // Convert
    //

    pAABBEntry .ConvertToAbsoluteNZ (pauchModelData);

    //
    // Invoke on children
    //

    if (pAABBEntry .GetData () != NULL)
    {
        _R2AAABBEntry (pAABBEntry .GetData () ->psLeft, pauchModelData);
        _R2AAABBEntry (pAABBEntry .GetData () ->psRight, pauchModelData);
    }
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Convert from relative to absolute addressing
//
// @parm unsigned char * | pauchModelData | Pointer to the model data
//
// @parm unsigned char * | pauchRawData | Pointer to the raw data
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlNodeHeader::RelativeToAbsolute (
    unsigned char *pauchModelData, unsigned char *pauchRawData)
{

    //
    // Based on the type of the node, invoke the proper routine
    //

    if (m_ulFlags == NwnMdlNT_Light)
        static_cast <CNwnMdlLightNode *> (this) ->_R2A (pauchModelData, pauchRawData);
    else if (m_ulFlags == NwnMdlNT_Emitter)
        static_cast <CNwnMdlEmitterNode *> (this) ->_R2A (pauchModelData, pauchRawData);
    else if (m_ulFlags == NwnMdlNT_Camera)
        static_cast <CNwnMdlCameraNode *> (this) ->_R2A (pauchModelData, pauchRawData);
    else if (m_ulFlags == NwnMdlNT_Reference)
        static_cast <CNwnMdlReferenceNode *> (this) ->_R2A (pauchModelData, pauchRawData);
    else if (m_ulFlags == NwnMdlNT_TriMesh)
        static_cast <CNwnMdlTriMeshNode *> (this) ->_R2A (pauchModelData, pauchRawData);
    else if (m_ulFlags == NwnMdlNT_SkinMesh)
        static_cast <CNwnMdlSkinMeshNode *> (this) ->_R2A (pauchModelData, pauchRawData);
    else if (m_ulFlags == NwnMdlNT_AnimMesh)
        static_cast <CNwnMdlAnimMeshNode *> (this) ->_R2A (pauchModelData, pauchRawData);
    else if (m_ulFlags == NwnMdlNT_DanglyMesh)
        static_cast <CNwnMdlDanglyMeshNode *> (this) ->_R2A (pauchModelData, pauchRawData);
    else if (m_ulFlags == NwnMdlNT_AABBMesh)
        static_cast <CNwnMdlAABBMeshNode *> (this) ->_R2A (pauchModelData, pauchRawData);
    else if (m_ulFlags == NwnMdlNT_Dummy)
        static_cast <CNwnMdlDummyNode *> (this) ->_R2A (pauchModelData, pauchRawData);
}

/* vim: set sw=4: */
