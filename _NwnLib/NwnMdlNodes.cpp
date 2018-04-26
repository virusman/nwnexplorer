//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnMdlNodes.cpp - Model node support |
//
// This module contains the general model node support.
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

#include "NwnMdlNodes.h"

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlNodeHeader> constructor.
//
// @parm const char * | pszName | Name of the node
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnMdlNodeHeader::CNwnMdlNodeHeader (const char *pszName)
{
    //
    // Initialize the flags
    //

    m_ulFlags = NwnMdlNT_Dummy;

    //
    // Copy the name (this is an ultra-clean version)
    //

    memset (m_szName, 0, _countof (m_szName));
    strncpy (m_szName, pszName, _countof (m_szName));
    m_szName [_countof (m_szName) - 1] = 0;

    //
    // Initialize some remaining values
    //

    m_ulInheritColor = 0;
    m_nPartNumber = 0;

    //
    // Initialize the routine pointers
    //

    m_aulNodeRoutines [0] = 0x0046B9E0;
    m_aulNodeRoutines [1] = 0x0046B9F0;
    m_aulNodeRoutines [2] = 0x0046BA04;
    m_aulNodeRoutines [3] = 0x0046BA14;
    m_aulNodeRoutines [4] = 0x0046BA30;
    m_aulNodeRoutines [5] = 0x0046BA40;
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlNodeHeader> denstructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnMdlNodeHeader::~CNwnMdlNodeHeader ()
{

    //
    // Delete all the children
    //

    if (m_apChildren .GetCount () > 0)
    {
        CNwnPointer <CNwnMdlNodeHeader> *ppChildren = 
            m_apChildren .GetData ();
        for (int i = 0; i < (int) m_apChildren .GetCount (); i++)
        {
            CNwnMdlNodeHeader *pChildNode = ppChildren [i] .GetData ();
            pChildNode ->DeleteNode ();
        }
    }
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlDummyNode> constructor.
//
// @parm const char * | pszName | Name of the node
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

    CNwnMdlDummyNode::CNwnMdlDummyNode (const char *pszName) 
: CNwnMdlNodeHeader (pszName)
{
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlDummyNode> denstructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnMdlDummyNode::~CNwnMdlDummyNode ()
{
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlLightNode> constructor.
//
// @parm const char * | pszName | Name of the node
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

    CNwnMdlLightNode::CNwnMdlLightNode (const char *pszName) 
: CNwnMdlNodeHeader (pszName)
{

    //
    // Initialize the flags
    //

    m_ulFlags = NwnMdlNT_Light;

    //
    // Initialize the local data
    //

    m_fFlareRadius = 0.0f;
    m_ulLightPriority = 5;
    m_ulAmbientOnly = 0;
    m_ulDynamicType = 1;
    m_ulAffectDynamic = 1;
    m_ulShadow = 1;
    m_ulGenerateFlare = 0;
    m_ulFadingLight = 1;

    //
    // Initialize the routine pointers
    //

    m_aulNodeRoutines [0] = 0x0046DBB8;
    m_aulNodeRoutines [1] = 0x0046DBC8;
    m_aulNodeRoutines [2] = 0x0046DBDC;
    m_aulNodeRoutines [3] = 0x0046DBEC;
    m_aulNodeRoutines [4] = 0x0046DC08;
    m_aulNodeRoutines [5] = 0x0046DC18;
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlLightNode> denstructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnMdlLightNode::~CNwnMdlLightNode ()
{

    //
    // Free the texture names
    //

    for (int i = 0; i < (int) m_apszTextureNames .GetCount (); i++)
    {
        delete [] m_apszTextureNames [i] .GetData ();
    }
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlEmitterNode> constructor.
//
// @parm const char * | pszName | Name of the node
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

    CNwnMdlEmitterNode::CNwnMdlEmitterNode (const char *pszName) 
: CNwnMdlNodeHeader (pszName)
{

    //
    // Initialize the flags
    //

    m_ulFlags = NwnMdlNT_Emitter;

    //
    // Initialize variables
    //

    m_fDeadSpace = 0.0f;
    m_fBlastRadius = 0.0f;
    m_fBlastLength = 0.0f;
    m_ulSpawnType = 0;
    memset (m_szChunkName, 0, _countof (m_szChunkName));
    m_ulTwoSidedTex = 0;
    m_ulLoop = 0;
    m_usRenderOrder = 0;
    m_ulEmitterFlags = 0;

    //
    // Initialize variables not initialize by Bioware
    //

    m_ulXGrid = 0;
    m_ulYGrid = 0;
    memset (m_szUpdate, 0, _countof (m_szUpdate));
    memset (m_szRender, 0, _countof (m_szRender));
    memset (m_szBlend, 0, _countof (m_szBlend));
    memset (m_szTexture, 0, _countof (m_szTexture));
    m_usPad = 0;

    //
    // Initialize the routine pointers
    //

    m_aulNodeRoutines [0] = 0x0046DBB8;
    m_aulNodeRoutines [1] = 0x0046DA00;
    m_aulNodeRoutines [2] = 0x0046DA14;
    m_aulNodeRoutines [3] = 0x0046DA24;
    m_aulNodeRoutines [4] = 0x0046DA40;
    m_aulNodeRoutines [5] = 0x0046DA50;
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlEmitterNode> denstructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnMdlEmitterNode::~CNwnMdlEmitterNode ()
{
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlCameraNode> constructor.
//
// @parm const char * | pszName | Name of the node
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

    CNwnMdlCameraNode::CNwnMdlCameraNode (const char *pszName) 
: CNwnMdlNodeHeader (pszName)
{

    //
    // Initialize the flags
    //

    m_ulFlags = NwnMdlNT_Camera;

    //
    // Initialize the routine pointers
    //

    m_aulNodeRoutines [1] = 0x0046D260;
    m_aulNodeRoutines [3] = 0x0046D274;
    m_aulNodeRoutines [4] = 0x0046D290;
    m_aulNodeRoutines [5] = 0x0046D2A0;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlCameraNode> denstructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnMdlCameraNode::~CNwnMdlCameraNode ()
{
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlReferenceNode> constructor.
//
// @parm const char * | pszName | Name of the node
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

    CNwnMdlReferenceNode::CNwnMdlReferenceNode (const char *pszName) 
: CNwnMdlNodeHeader (pszName)
{

    //
    // Initialize the flags
    //

    m_ulFlags = NwnMdlNT_Reference;

    //
    // Initialize variables (none of these normally initialized by bioware)
    //

    memset (m_szRefModel, 0, _countof (m_szRefModel));
    m_ulReattachable = 0;

    //
    // Initialize the routine pointers
    //

    m_aulNodeRoutines [1] = 0x0046E178;
    m_aulNodeRoutines [3] = 0x0046E18C;
    m_aulNodeRoutines [4] = 0x0046E1A8;
    m_aulNodeRoutines [5] = 0x0046E1B8;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlReferenceNode> denstructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnMdlReferenceNode::~CNwnMdlReferenceNode ()
{
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlMeshHeader> constructor.
//
// @parm const char * | pszName | Name of the node
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

    CNwnMdlMeshHeader::CNwnMdlMeshHeader (const char *pszName) 
: CNwnMdlNodeHeader (pszName)
{

    //
    // Initialize the flags
    //

    m_ulFlags = NwnMdlNT_TriMesh;

    //
    // Initialize other variables
    //

    m_vBMin = CVector3 (0, 0, 0);
    m_vBMax = CVector3 (0, 0, 0);
    m_fRadius = 0.0f;
    m_vBAverage = CVector3 (0, 0, 0);
    m_vDiffuse = CVector3 (0.8f, 0.8f, 0.8f);
    m_vAmbient = CVector3 (0.2f, 0.2f, 0.2f);
    m_vSpecular = CVector3 (0, 0, 0);
    m_fShininess = 1.0f;
    m_ulShadow = 1;
    m_ulBeaming = 0;
    m_ulRender = 1;
    m_ulTransparencyHint = 0;
    m_ulRenderHint = 0;
    memset (m_szTextures, 0, sizeof (m_szTextures));
    m_ulTileFade = 0;
    m_ulSomething3Offset = 0xFFFFFFFF;
    m_ulSomething3Count = 0;
    m_usVertexCount = 0;
    m_usTextureCount = 0;
    m_ucLightMapped = 0;
    m_ucRotateTexture = 0;
    m_fFaceNormalSumDiv2 = 0.0f;
    m_ulUnknown1 = 0;

    //
    // Initialize variables not initialized by Bioware
    //

    m_ucTriangleMode = 0;
    m_ucPad [0] = 0;
    m_ucPad [1] = 0;
    m_ucPad [2] = 0;
    m_usPad = 0;

    //
    // Create the mesh temp structure
    //

    m_pvTempMeshData = NULL;

    //
    // Initialize the routine pointers
    //

    m_aulNodeRoutines [0] = 0x0046E7B4;
    m_aulNodeRoutines [1] = 0x0046E7C4;
    m_aulNodeRoutines [2] = 0x0046E7D8;
    m_aulNodeRoutines [3] = 0x0046E7E8;
    m_aulNodeRoutines [4] = 0x0046E804;
    m_aulNodeRoutines [5] = 0x0046E814;
    m_aulMeshRoutines [0] = 0x0046E828;
    m_aulMeshRoutines [1] = 0x0046E838;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlMeshHeader> denstructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnMdlMeshHeader::~CNwnMdlMeshHeader ()
{
    //
    // Free the vertex indicies 
    //

    for (int i = 0; i < (int) m_sVertexIndices .GetCount (); i++)
        delete [] m_sVertexIndices [i] .GetData ();
    for (int i = 0; i < (int) m_sRawVertexIndices .GetCount (); i++)
        delete [] m_sRawVertexIndices [i] .GetData ();

    //
    // Free these pointers
    //

    if (m_pavVerts .GetDataNZ (NULL) != 0)
        delete [] m_pavVerts .GetData ();
    if (m_pavTVerts [0] .GetDataNZ (NULL) != 0)
        delete [] m_pavTVerts [0] .GetData ();
    if (m_pavTVerts [1] .GetDataNZ (NULL) != 0)
        delete [] m_pavTVerts [1] .GetData ();
    if (m_pavTVerts [2] .GetDataNZ (NULL) != 0)
        delete [] m_pavTVerts [2] .GetData ();
    if (m_pavTVerts [3] .GetDataNZ (NULL) != 0)
        delete [] m_pavTVerts [3] .GetData ();
    if (m_paColors .GetDataNZ (NULL) != 0)
        delete [] m_paColors .GetData ();
    if (m_pavNormals .GetDataNZ (NULL) != 0)
        delete [] m_pavNormals .GetData ();
    if (m_pavBumpmapAnim1 .GetDataNZ (NULL) != 0)
        delete [] m_pavBumpmapAnim1 .GetData ();
    if (m_pavBumpmapAnim2 .GetDataNZ (NULL) != 0)
        delete [] m_pavBumpmapAnim2 .GetData ();
    if (m_pavBumpmapAnim3 .GetDataNZ (NULL) != 0)
        delete [] m_pavBumpmapAnim3 .GetData ();
    if (m_pavBumpmapAnim4 .GetDataNZ (NULL) != 0)
        delete [] m_pavBumpmapAnim4 .GetData ();
    if (m_pavBumpmapAnim5 .GetDataNZ (NULL) != 0)
        delete [] m_pavBumpmapAnim5 .GetData ();
    if (m_pafBumpmapAnim6 .GetDataNZ (NULL) != 0)
        delete [] m_pafBumpmapAnim6 .GetData ();

    //
    // Free other variables
    //

    m_ulSomething3Offset = 0xFFFFFFFF;
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlTriMeshNode> constructor.
//
// @parm const char * | pszName | Name of the node
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

    CNwnMdlTriMeshNode::CNwnMdlTriMeshNode (const char *pszName) 
: CNwnMdlMeshHeader (pszName)
{
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlTriMeshNode> denstructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnMdlTriMeshNode::~CNwnMdlTriMeshNode ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlSkinMeshNode> constructor.
//
// @parm const char * | pszName | Name of the node
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

    CNwnMdlSkinMeshNode::CNwnMdlSkinMeshNode (const char *pszName) 
: CNwnMdlMeshHeader (pszName)
{

    //
    // Initialize the flags
    //

    m_ulFlags = NwnMdlNT_SkinMesh;

    //
    // Initialize the other variables
    //

    m_pafSkinWeights .Initialize ();
    m_pasSkinBoneRefs .Initialize ();
    m_pasNodeToBoneMap .Initialize ();

    //
    // Initialize variables not initialized by Bioware
    //

    m_ulNodeToBoneCount = 0;
    memset (m_asBoneNodeNumbers, 0, sizeof (m_asBoneNodeNumbers));
    m_sPad = 0;

    //
    // Initialize the routine pointers
    //

    m_aulNodeRoutines [0] = 0x0046D394;
    m_aulNodeRoutines [1] = 0x0046D3A4;
    m_aulNodeRoutines [2] = 0x0046D3B8;
    m_aulNodeRoutines [4] = 0x0046D3C8;
    m_aulNodeRoutines [5] = 0x0046D3D8;
    m_aulMeshRoutines [0] = 0x0046D3EC;
    m_aulMeshRoutines [1] = 0x0046D3FC;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlSkinMeshNode> denstructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnMdlSkinMeshNode::~CNwnMdlSkinMeshNode ()
{
    //
    // Free the other variables
    //

    if (m_pafSkinWeights .GetDataNZ (NULL) != 0)
        delete [] m_pafSkinWeights .GetData ();
    if (m_pasSkinBoneRefs .GetDataNZ (NULL) != 0)
        delete [] m_pasSkinBoneRefs .GetData ();
    if (m_pasNodeToBoneMap .GetDataNZ (NULL) != 0)
        delete [] m_pasNodeToBoneMap .GetData ();
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlAnimMeshNode> constructor.
//
// @parm const char * | pszName | Name of the node
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

    CNwnMdlAnimMeshNode::CNwnMdlAnimMeshNode (const char *pszName) 
: CNwnMdlMeshHeader (pszName)
{

    //
    // Initialize the flags
    //

    m_ulFlags = NwnMdlNT_AnimMesh;

    //
    // Initialize other values
    //

    m_fSamplePeriod = 0.0f;
    m_ulVertexSetCount = 0;
    m_ulTextureSetCount = 0;

    //
    // Initialize the routine pointers
    //

    m_aulNodeRoutines [0] = 0x0046CF88;
    m_aulNodeRoutines [1] = 0x0046CF98;
    m_aulNodeRoutines [2] = 0x0046CFAC;
    m_aulNodeRoutines [3] = 0x0046CFBC;
    m_aulNodeRoutines [4] = 0x0046CFD8;
    m_aulNodeRoutines [5] = 0x0046CFE8;
    m_aulMeshRoutines [0] = 0x0046CFFC;
    m_aulMeshRoutines [1] = 0x0046D00C;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlAnimMeshNode> denstructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnMdlAnimMeshNode::~CNwnMdlAnimMeshNode ()
{
    //
    // Free the other variables
    //

    if (m_pvVertexSets .GetData ())
        delete [] m_pvVertexSets .GetData ();
    if (m_pvTextureSets .GetData ())
        delete [] m_pvTextureSets .GetData ();
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlDanglyMeshNode> constructor.
//
// @parm const char * | pszName | Name of the node
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

    CNwnMdlDanglyMeshNode::CNwnMdlDanglyMeshNode (const char *pszName) 
: CNwnMdlMeshHeader (pszName)
{

    //
    // Initialize the flags
    //

    m_ulFlags = NwnMdlNT_DanglyMesh;

    //
    // Initialize variables not initialized by Bioware
    //

    m_fDisplacement = 0.0f;
    m_fTightness = 0.0f;
    m_fPeriod = 0.0f;

    //
    // Initialize the routine pointers
    //

    m_aulNodeRoutines [0] = 0x0046E2B4;
    m_aulNodeRoutines [1] = 0x0046E2C4;
    m_aulNodeRoutines [2] = 0x0046E2D8;
    m_aulNodeRoutines [5] = 0x0046E2E8;
    m_aulMeshRoutines [0] = 0x0046E2FC;
    m_aulMeshRoutines [1] = 0x0046E30C;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlDanglyMeshNode> denstructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnMdlDanglyMeshNode::~CNwnMdlDanglyMeshNode ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlAABBMeshNode> constructor.
//
// @parm const char * | pszName | Name of the node
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

    CNwnMdlAABBMeshNode::CNwnMdlAABBMeshNode (const char *pszName) 
: CNwnMdlMeshHeader (pszName)
{

    //
    // Initialize the flags
    //

    m_ulFlags = NwnMdlNT_AABBMesh;

    //
    // Initialize other values
    //

    m_ulRender = 0;
    m_ulShadow = 0;

    //
    // Initialize the routine pointers
    //

    m_aulNodeRoutines [0] = 0x0046ED74;
    m_aulNodeRoutines [1] = 0x0046ED84;
    m_aulNodeRoutines [5] = 0x0046ED98;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnMdlAABBMeshNode> denstructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnMdlAABBMeshNode::~CNwnMdlAABBMeshNode ()
{
    //
    // Delete AABB tree
    //

    DeleteAABBEntry (m_pAABBTree .GetData ());
}

//-----------------------------------------------------------------------------
//
// @func Free an AABB Entry
//
// @parm NwnMdlAABBEntry * | pAABBEntry | Entry being freed
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlAABBMeshNode::DeleteAABBEntry (NwnMdlAABBEntry *pAABBEntry)
{

    //
    // Return if NULL
    //

    if (pAABBEntry == NULL)
        return;

    //
    // Free the children
    //

    DeleteAABBEntry (pAABBEntry ->psLeft .GetData ());
    DeleteAABBEntry (pAABBEntry ->psRight .GetData ());

    //
    // Free myself
    //

    delete pAABBEntry;
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Delete myself
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlNodeHeader::DeleteNode ()
{

    //
    // Based on the type of the node, invoke the proper routine
    //

    if (m_ulFlags == NwnMdlNT_Light)
        delete static_cast <CNwnMdlLightNode *> (this);
    else if (m_ulFlags == NwnMdlNT_Emitter)
        delete static_cast <CNwnMdlEmitterNode *> (this);
    else if (m_ulFlags == NwnMdlNT_Camera)
        delete static_cast <CNwnMdlCameraNode *> (this);
    else if (m_ulFlags == NwnMdlNT_Reference)
        delete static_cast <CNwnMdlReferenceNode *> (this);
    else if (m_ulFlags == NwnMdlNT_TriMesh)
        delete static_cast <CNwnMdlTriMeshNode *> (this);
    else if (m_ulFlags == NwnMdlNT_SkinMesh)
        delete static_cast <CNwnMdlSkinMeshNode *> (this);
    else if (m_ulFlags == NwnMdlNT_AnimMesh)
        delete static_cast <CNwnMdlAnimMeshNode *> (this);
    else if (m_ulFlags == NwnMdlNT_DanglyMesh)
        delete static_cast <CNwnMdlDanglyMeshNode *> (this);
    else if (m_ulFlags == NwnMdlNT_AABBMesh)
        delete static_cast <CNwnMdlAABBMeshNode *> (this);
    else if (m_ulFlags == NwnMdlNT_Dummy)
        delete static_cast <CNwnMdlDummyNode *> (this);
    else
        assert (false);
}


/* vim: set sw=4: */
