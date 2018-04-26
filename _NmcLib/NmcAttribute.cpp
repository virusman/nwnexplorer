//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnAttribute.cpp - Nwn model attribute support |
//
// This module contains the Nwn model attribute support.
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

#include "Nmc.h"
#include "NmcCoreParsers.h"
#include "NmcListParser.h"
#include "NmcMesh.h"

//
// Attribute structures
//

struct NmcAttribute;
typedef void (FN_NMCATTRIBUTE) (CNmcContext *pCtx, 
                                CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr);

struct NmcAttribute
{
    const char                 *pszName;
    UINT32                      ulFlags;
    size_t                      ulOffset;
    size_t                      ulSize;
    FN_NMCATTRIBUTE            *pRoutine;
};

CNwnMdlNodeHeader *NmcFindNode (CNwnMdlNodeHeader *pNode, const char *pszName)
{
    if (pNode == NULL)
        return NULL;
    if (stricmp (pNode ->m_szName, pszName) == 0)
        return pNode;
    for (size_t i = 0; i < pNode ->m_apChildren .GetCount (); i++)
    {
        CNwnMdlNodeHeader *pChild = NmcFindNode (
            pNode ->m_apChildren [i] .GetData (), pszName);
        if (pChild != NULL)
            return pChild;
    }
    return NULL;
}

//
// Attribute processing routines
//

void NmcAttrIgnore (CNmcContext *pCtx, CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr);
void NmcAttrInteger (CNmcContext *pCtx, CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr);
void NmcAttrInteger01 (CNmcContext *pCtx, CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr);
void NmcAttrFloat (CNmcContext *pCtx, CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr);
void NmcAttrVector3 (CNmcContext *pCtx, CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr);
void NmcAttrString (CNmcContext *pCtx, CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr);
void NmcAttrUINT32Flag (CNmcContext *pCtx, CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr);

void NmcAttrIntegerArray (CNmcContext *pCtx, CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr);
void NmcAttrFloatArray (CNmcContext *pCtx, CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr);
void NmcAttrVector3Array (CNmcContext *pCtx, CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr);
void NmcAttrQuaternionArray (CNmcContext *pCtx, CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr);
void NmcAttrStringArray (CNmcContext *pCtx, CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr);

void NmcAttrParent (CNmcContext *pCtx, CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr);
void NmcAttrVertexIndices (CNmcContext *pCtx, CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr);
void NmcAttrAABBEntryArray (CNmcContext *pCtx, CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr);
void NmcAttrWeightArray (CNmcContext *pCtx, CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr);

void NmcAttrMeshFaceArray (CNmcContext *pCtx, CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr);
void NmcAttrMeshIntegerArray (CNmcContext *pCtx, CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr);
void NmcAttrMeshVector3Array (CNmcContext *pCtx, CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr);
void NmcAttrMeshTexIndicesArray (CNmcContext *pCtx, CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr);

//
// Nodes attributes
//

NmcAttribute g_sAttributes [] =
{

    //
    // Common node attributes
    //

    {
        "wirecolor",
        NwnMdlNF_HasHeader,
        0,
        0,
        NmcAttrIgnore // wirecolor is really 3 floats
    },

    { 
        "inheritcolor",
        NwnMdlNF_HasHeader,
        offsetof (CNwnMdlNodeHeader, m_ulInheritColor),
        sizeof (UINT32),
        NmcAttrInteger
    },

    { 
        "parent",
        NwnMdlNF_HasHeader,
        0,
        0,
        NmcAttrParent
    },

    //
    // Light node attributes
    //

    { 
        "flareradius",
        NwnMdlNF_HasLight,
        offsetof (CNwnMdlLightNode, m_fFlareRadius),
        sizeof (float),
        NmcAttrFloat
    },

    { 
        "ambientonly",
        NwnMdlNF_HasLight,
        offsetof (CNwnMdlLightNode, m_ulAmbientOnly),
        sizeof (UINT32),
        NmcAttrInteger
    },

    { 
        "shadow",
        NwnMdlNF_HasLight,
        offsetof (CNwnMdlLightNode, m_ulShadow),
        sizeof (UINT32),
        NmcAttrInteger
    },

    { 
        "nDynamicType",
        NwnMdlNF_HasLight,
        offsetof (CNwnMdlLightNode, m_ulDynamicType),
        sizeof (UINT32),
        NmcAttrInteger
    },

    { 
        "affectdynamic",
        NwnMdlNF_HasLight,
        offsetof (CNwnMdlLightNode, m_ulAffectDynamic),
        sizeof (UINT32),
        NmcAttrInteger
    },

    { 
        "lightpriority",
        NwnMdlNF_HasLight,
        offsetof (CNwnMdlLightNode, m_ulLightPriority),
        sizeof (UINT32),
        NmcAttrInteger
    },

    { 
        "generateflare",
        NwnMdlNF_HasLight,
        offsetof (CNwnMdlLightNode, m_ulGenerateFlare),
        sizeof (UINT32),
        NmcAttrInteger
    },

    { 
        "fadingLight",
        NwnMdlNF_HasLight,
        offsetof (CNwnMdlLightNode, m_ulFadingLight),
        sizeof (UINT32),
        NmcAttrInteger
    },

    { 
        "isdynamic",
        NwnMdlNF_HasLight,
        offsetof (CNwnMdlLightNode, m_ulDynamicType),
        sizeof (UINT32),
        NmcAttrInteger
    },

    { 
        "flaresizes",
        NwnMdlNF_HasLight,
        offsetof (CNwnMdlLightNode, m_afFlareSizes),
        0,
        NmcAttrFloatArray
    },

    { 
        "flarepositions",
        NwnMdlNF_HasLight,
        offsetof (CNwnMdlLightNode, m_afFlarePositions),
        0,
        NmcAttrFloatArray
    },

    { 
        "flarecolorshifts",
        NwnMdlNF_HasLight,
        offsetof (CNwnMdlLightNode, m_avFlareColorShifts),
        0,
        NmcAttrVector3Array
    },

    { 
        "texturenames",
        NwnMdlNF_HasLight,
        offsetof (CNwnMdlLightNode, m_apszTextureNames),
        0,
        NmcAttrStringArray
    },

    //
    // Emitter node attributes
    //

    { 
        "p2p",
        NwnMdlNF_HasEmitter,
        offsetof (CNwnMdlEmitterNode, m_ulEmitterFlags),
        NwnMdlEF_P2P,
        NmcAttrUINT32Flag
    },

    { 
        "p2p_sel",
        NwnMdlNF_HasEmitter,
        offsetof (CNwnMdlEmitterNode, m_ulEmitterFlags),
        NwnMdlEF_P2PSel,
        NmcAttrUINT32Flag
    },

    { 
        "affectedByWind",
        NwnMdlNF_HasEmitter,
        offsetof (CNwnMdlEmitterNode, m_ulEmitterFlags),
        NwnMdlEF_AffectedByWind,
        NmcAttrUINT32Flag
    },

    { 
        "m_isTinted",
        NwnMdlNF_HasEmitter,
        offsetof (CNwnMdlEmitterNode, m_ulEmitterFlags),
        NwnMdlEF_IsTinted,
        NmcAttrUINT32Flag
    },

    { 
        "bounce",
        NwnMdlNF_HasEmitter,
        offsetof (CNwnMdlEmitterNode, m_ulEmitterFlags),
        NwnMdlEF_Bounce,
        NmcAttrUINT32Flag
    },

    { 
        "random",
        NwnMdlNF_HasEmitter,
        offsetof (CNwnMdlEmitterNode, m_ulEmitterFlags),
        NwnMdlEF_Random,
        NmcAttrUINT32Flag
    },

    { 
        "inherit",
        NwnMdlNF_HasEmitter,
        offsetof (CNwnMdlEmitterNode, m_ulEmitterFlags),
        NwnMdlEF_Inherit,
        NmcAttrUINT32Flag
    },

    { 
        "inheritvel",
        NwnMdlNF_HasEmitter,
        offsetof (CNwnMdlEmitterNode, m_ulEmitterFlags),
        NwnMdlEF_InheritVel,
        NmcAttrUINT32Flag
    },

    { 
        "inherit_local",
        NwnMdlNF_HasEmitter,
        offsetof (CNwnMdlEmitterNode, m_ulEmitterFlags),
        NwnMdlEF_InheritLocal,
        NmcAttrUINT32Flag
    },

    { 
        "splat",
        NwnMdlNF_HasEmitter,
        offsetof (CNwnMdlEmitterNode, m_ulEmitterFlags),
        NwnMdlEF_Splat,
        NmcAttrUINT32Flag
    },

    { 
        "inherit_part",
        NwnMdlNF_HasEmitter,
        offsetof (CNwnMdlEmitterNode, m_ulEmitterFlags),
        NwnMdlEF_InheritPart,
        NmcAttrUINT32Flag
    },

    { 
        "renderorder",
        NwnMdlNF_HasEmitter,
        offsetof (CNwnMdlEmitterNode, m_usRenderOrder),
        sizeof (UINT16),
        NmcAttrInteger
    },

    { 
        "xgrid",
        NwnMdlNF_HasEmitter,
        offsetof (CNwnMdlEmitterNode, m_ulXGrid),
        sizeof (UINT32),
        NmcAttrInteger
    },

    { 
        "ygrid",
        NwnMdlNF_HasEmitter,
        offsetof (CNwnMdlEmitterNode, m_ulYGrid),
        sizeof (UINT32),
        NmcAttrInteger
    },

    { 
        "loop",
        NwnMdlNF_HasEmitter,
        offsetof (CNwnMdlEmitterNode, m_ulLoop),
        sizeof (UINT32),
        NmcAttrInteger
    },

    { 
        "twosidedtex",
        NwnMdlNF_HasEmitter,
        offsetof (CNwnMdlEmitterNode, m_ulTwoSidedTex),
        sizeof (UINT32),
        NmcAttrInteger
    },

    { 
        "spawntype",
        NwnMdlNF_HasEmitter,
        offsetof (CNwnMdlEmitterNode, m_ulSpawnType),
        sizeof (UINT32),
        NmcAttrInteger01
    },

    { 
        "deadspace",
        NwnMdlNF_HasEmitter,
        offsetof (CNwnMdlEmitterNode, m_fDeadSpace),
        sizeof (float),
        NmcAttrFloat
    },

    { 
        "blastRadius",
        NwnMdlNF_HasEmitter,
        offsetof (CNwnMdlEmitterNode, m_fBlastRadius),
        sizeof (float),
        NmcAttrFloat
    },

    { 
        "blastLength",
        NwnMdlNF_HasEmitter,
        offsetof (CNwnMdlEmitterNode, m_fBlastLength),
        sizeof (float),
        NmcAttrFloat
    },

    { 
        "update",
        NwnMdlNF_HasEmitter,
        offsetof (CNwnMdlEmitterNode, m_szUpdate),
        32,
        NmcAttrString
    },

    { 
        "render",
        NwnMdlNF_HasEmitter,
        offsetof (CNwnMdlEmitterNode, m_szRender),
        32,
        NmcAttrString
    },

    { 
        "blend",
        NwnMdlNF_HasEmitter,
        offsetof (CNwnMdlEmitterNode, m_szBlend),
        32,
        NmcAttrString
    },

    { 
        "texture",
        NwnMdlNF_HasEmitter,
        offsetof (CNwnMdlEmitterNode, m_szTexture),
        64,
        NmcAttrString
    },

    { 
        "chunkName",
        NwnMdlNF_HasEmitter,
        offsetof (CNwnMdlEmitterNode, m_szChunkName),
        16,
        NmcAttrString
    },

    //
    // Reference node attributes
    //

    { 
        "reattachable",
        NwnMdlNF_HasReference,
        offsetof (CNwnMdlReferenceNode, m_ulReattachable),
        sizeof (UINT32),
        NmcAttrInteger
    },

    { 
        "refModel",
        NwnMdlNF_HasReference,
        offsetof (CNwnMdlReferenceNode, m_szRefModel),
        64,
        NmcAttrString
    },

    //
    // Mesh node attributes
    //

    { 
        "bmin",
        NwnMdlNF_HasMesh,
        offsetof (CNwnMdlMeshHeader, m_vBMin),
        0,
        NmcAttrVector3
    },

    { 
        "bmax",
        NwnMdlNF_HasMesh,
        offsetof (CNwnMdlMeshHeader, m_vBMax),
        0,
        NmcAttrVector3
    },

    { 
        "diffuse",
        NwnMdlNF_HasMesh,
        offsetof (CNwnMdlMeshHeader, m_vDiffuse),
        0,
        NmcAttrVector3
    },

    { 
        "ambient",
        NwnMdlNF_HasMesh,
        offsetof (CNwnMdlMeshHeader, m_vAmbient),
        0,
        NmcAttrVector3
    },

    { 
        "specular",
        NwnMdlNF_HasMesh,
        offsetof (CNwnMdlMeshHeader, m_vSpecular),
        0,
        NmcAttrVector3
    },

    { 
        "shininess",
        NwnMdlNF_HasMesh,
        offsetof (CNwnMdlMeshHeader, m_fShininess),
        sizeof (float),
        NmcAttrFloat
    },

    { 
        "shadow",
        NwnMdlNF_HasMesh,
        offsetof (CNwnMdlMeshHeader, m_ulShadow),
        sizeof (UINT32),
        NmcAttrInteger
    },

    { 
        "beaming",
        NwnMdlNF_HasMesh,
        offsetof (CNwnMdlMeshHeader, m_ulBeaming),
        sizeof (UINT32),
        NmcAttrInteger
    },

    { 
        "render",
        NwnMdlNF_HasMesh,
        offsetof (CNwnMdlMeshHeader, m_ulRender),
        sizeof (UINT32),
        NmcAttrInteger
    },

    { 
        "transparencyhint",
        NwnMdlNF_HasMesh,
        offsetof (CNwnMdlMeshHeader, m_ulTransparencyHint),
        sizeof (UINT32),
        NmcAttrInteger
    },

    { 
        "tilefade",
        NwnMdlNF_HasMesh,
        offsetof (CNwnMdlMeshHeader, m_ulTileFade),
        sizeof (UINT32),
        NmcAttrInteger
    },

    { 
        "rotatetexture",
        NwnMdlNF_HasMesh,
        offsetof (CNwnMdlMeshHeader, m_ucRotateTexture),
        sizeof (UINT8),
        NmcAttrInteger
    },

    { 
        "lightmapped",
        NwnMdlNF_HasMesh,
        offsetof (CNwnMdlMeshHeader, m_ucLightMapped),
        sizeof (UINT8),
        NmcAttrInteger
    },

    { 
        "texture0",
        NwnMdlNF_HasMesh,
        offsetof (CNwnMdlMeshHeader, m_szTextures [0]),
        64,
        NmcAttrString
    },

    { 
        "bitmap",
        NwnMdlNF_HasMesh,
        offsetof (CNwnMdlMeshHeader, m_szTextures [0]),
        64,
        NmcAttrString
    },

    { 
        "texture1",
        NwnMdlNF_HasMesh,
        offsetof (CNwnMdlMeshHeader, m_szTextures [1]),
        64,
        NmcAttrString
    },

    { 
        "texture2",
        NwnMdlNF_HasMesh,
        offsetof (CNwnMdlMeshHeader, m_szTextures [2]),
        64,
        NmcAttrString
    },

    { 
        "materialname",
        NwnMdlNF_HasMesh,
        offsetof (CNwnMdlMeshHeader, m_szMaterialName),
        64,
        NmcAttrString
    },

    { 
        "vertexindicescount",
        NwnMdlNF_HasMesh,
        offsetof (CNwnMdlMeshHeader, m_sVertexIndicesCount),
        0,
        NmcAttrIntegerArray
    },

    { 
        "leftoverfaces",
        NwnMdlNF_HasMesh,
        offsetof (CNwnMdlMeshHeader, m_sLeftOverFaces),
        0,
        NmcAttrIntegerArray
    },

    { 
        "vertexindices",
        NwnMdlNF_HasMesh,
        offsetof (CNwnMdlMeshHeader, m_sVertexIndices),
        0,
        NmcAttrVertexIndices
    },

    {
        "verts",
        NwnMdlNF_HasMesh,
        offsetof (NmcMeshTemp, aVerts),
        0,
        NmcAttrMeshVector3Array
    },

    {
        "tverts",
        NwnMdlNF_HasMesh,
        offsetof (NmcMeshTemp, aTVerts [0]),
        0,
        NmcAttrMeshVector3Array
    },

    {
        "tverts1",
        NwnMdlNF_HasMesh,
        offsetof (NmcMeshTemp, aTVerts [1]),
        0,
        NmcAttrMeshVector3Array
    },

    {
        "tverts2",
        NwnMdlNF_HasMesh,
        offsetof (NmcMeshTemp, aTVerts [2]),
        0,
        NmcAttrMeshVector3Array
    },

    {
        "tverts3",
        NwnMdlNF_HasMesh,
        offsetof (NmcMeshTemp, aTVerts [3]),
        0,
        NmcAttrMeshVector3Array
    },

    {
        "colors",
        NwnMdlNF_HasMesh,
        offsetof (NmcMeshTemp, aColors),
        0,
        NmcAttrMeshVector3Array
    },

    {
        "texindices1",
        NwnMdlNF_HasMesh,
        offsetof (NmcMeshTemp, aTexIndices [1]),
        0,
        NmcAttrMeshTexIndicesArray
    },

    {
        "texindices2",
        NwnMdlNF_HasMesh,
        offsetof (NmcMeshTemp, aTexIndices [2]),
        0,
        NmcAttrMeshTexIndicesArray
    },

    {
        "texindices3",
        NwnMdlNF_HasMesh,
        offsetof (NmcMeshTemp, aTexIndices [3]),
        0,
        NmcAttrMeshTexIndicesArray
    },

    {
        "mirrorlist",
        NwnMdlNF_HasMesh,
        offsetof (NmcMeshTemp, aMirrorList),
        0,
        NmcAttrMeshIntegerArray
    },

    {
        "faces",
        NwnMdlNF_HasMesh,
        offsetof (NmcMeshTemp, aFaces),
        0,
        NmcAttrMeshFaceArray
    },

    //
    // Skin mesh node attributes
    //

    { 
        "weights",
        NwnMdlNF_HasSkin,
        offsetof (CNwnMdlSkinMeshNode, m_aWeights),
        0,
        NmcAttrWeightArray
    },

    { 
        "qbone_ref_inv",
        NwnMdlNF_HasSkin,
        offsetof (CNwnMdlSkinMeshNode, m_aQBoneRefInv),
        0,
        NmcAttrQuaternionArray
    },

    { 
        "tbone_ref_inv",
        NwnMdlNF_HasSkin,
        offsetof (CNwnMdlSkinMeshNode, m_aTBoneRefInv),
        0,
        NmcAttrVector3Array
    },

    { 
        "boneconstantindices",
        NwnMdlNF_HasSkin,
        offsetof (CNwnMdlSkinMeshNode, m_aBoneConstantIndices),
        0,
        NmcAttrIntegerArray
    },

    //
    // Animation mesh node attributes
    //

    { 
        "sampleperiod",
        NwnMdlNF_HasAnim,
        offsetof (CNwnMdlAnimMeshNode, m_fSamplePeriod),
        sizeof (float),
        NmcAttrFloat
    },

    { 
        "animverts",
        NwnMdlNF_HasAnim,
        offsetof (CNwnMdlAnimMeshNode, m_avAnimVerts),
        0,
        NmcAttrVector3Array
    },

    { 
        "animtverts",
        NwnMdlNF_HasAnim,
        offsetof (CNwnMdlAnimMeshNode, m_avAnimTVerts),
        0,
        NmcAttrVector3Array
    },

    //
    // Dangly mesh node attributes
    //

    { 
        "displacement",
        NwnMdlNF_HasDangly,
        offsetof (CNwnMdlDanglyMeshNode, m_fDisplacement),
        sizeof (float),
        NmcAttrFloat
    },

    { 
        "tightness",
        NwnMdlNF_HasDangly,
        offsetof (CNwnMdlDanglyMeshNode, m_fTightness),
        sizeof (float),
        NmcAttrFloat
    },

    { 
        "period",
        NwnMdlNF_HasDangly,
        offsetof (CNwnMdlDanglyMeshNode, m_fPeriod),
        sizeof (float),
        NmcAttrFloat
    },

    { 
        "constraints",
        NwnMdlNF_HasDangly,
        offsetof (CNwnMdlDanglyMeshNode, m_afConstraints),
        0,
        NmcAttrFloatArray
    },

    //
    // AABB mesh node attributes
    //

    { 
        "aabb",
        NwnMdlNF_HasAABB,
        offsetof (CNwnMdlAABBMeshNode, m_pAABBTree),
        0,
        NmcAttrAABBEntryArray
    },
};
int g_nAttributes = _countof (g_sAttributes);

//
// Controller table initializer
//

class CNmcAttributeInitialize
{
    public:
        CNmcAttributeInitialize ()
        {

            //
            // Sort the controllers (OMG, it is a hand written sort loop!!! Bite me)
            //

            bool fSwapped = true;
            while (fSwapped)
            {
                fSwapped = false;
                for (int i = 0; i < g_nAttributes - 1; i++)
                {
                    if (stricmp (g_sAttributes [i] .pszName, 
                                 g_sAttributes [i + 1] .pszName) > 0)
                    {
                        NmcAttribute temp;
                        temp = g_sAttributes [i];
                        g_sAttributes [i] = g_sAttributes [i + 1];
                        g_sAttributes [i + 1] = temp;
                        fSwapped = true;
                    }
                }
            }
        }
};

static CNmcAttributeInitialize g_sAttributesInitialize;

//-----------------------------------------------------------------------------
//
// @func Parse for an attribute
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CNwnMdlNodeHeader * | pNode | Node in question
//
// @rdesc TRUE if the attribute was parsed.  FALSE if it wasn't
//              an attribute.
//
//-----------------------------------------------------------------------------

bool NmcParseAttribute (CNmcContext *pCtx, CNwnMdlNodeHeader *pNode)
{
    //
    // Get a pointer to the current token
    //

    char *pszToken = pCtx ->GetCurrentToken ();

    //
    // Use a binary search to find a match
    //

    int nMatch = 0;
    int nMin = 0;
    int nMax = g_nAttributes - 1;
    while (nMin <= nMax)
    {
        nMatch = (nMin + nMax) / 2;
        int nCompare = stricmp (pszToken, g_sAttributes [nMatch] .pszName);
        if (nCompare < 0)
            nMax = nMatch - 1;
        else if (nCompare > 0)
            nMin = nMatch + 1;
        else
            break;
    }

    //
    // If we found nothing, return
    //

    if (nMin > nMax)
        return false;

    //
    // Now, try to locate the controller that matches our flags
    //

    if ((g_sAttributes [nMatch] .ulFlags & pNode ->m_ulFlags) == 0)
    {

        //
        // The thing to remember about the binary search is that if we have
        // a series of controllers with the same name, then the point we located
        // a name probably won't be the first one in the list.  Thus we have to
        // search forward and backward
        //

        int nFound = -1;

        //
        // Search backwards for a match
        //

        for (int nTest = nMatch - 1; nTest >= 0; nTest--)
        {
            if (stricmp (g_sAttributes [nTest] .pszName, pszToken) != 0)
                break;
            if ((g_sAttributes [nTest] .ulFlags & pNode ->m_ulFlags) != 0)
            {
                nFound = nTest;
                break;
            }
        }

        //
        // If not found backwards, try forwards
        //

        if (nFound == -1)
        {
            for (int nTest = nMatch + 1; nTest < g_nAttributes; nTest++)
            {
                if (stricmp (g_sAttributes [nTest] .pszName, pszToken) != 0)
                    break;
                if ((g_sAttributes [nTest] .ulFlags & pNode ->m_ulFlags) != 0)
                {
                    nFound = nTest;
                    break;
                }
            }
        }

        //
        // If still not found, then return false
        //

        if (nFound == -1)
            return false;

        //
        // Use the found index as the match index
        //

        nMatch = nFound;
    }

    //
    // Invoke them method
    //

    g_sAttributes [nMatch] .pRoutine (pCtx, pNode, &g_sAttributes [nMatch]);
    return true;
}

//-----------------------------------------------------------------------------
//
// @func Ignore the data of the attribute
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CNwnMdlNodeHeader * | pNode | Node in question
//
// @parm NmcAttribute * | pAttr | Attribute
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcAttrIgnore (CNmcContext *pCtx, 
                    CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr)
{
    pCtx; pNode; pAttr; //4100
}

//-----------------------------------------------------------------------------
//
// @func Get an integer attribute
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CNwnMdlNodeHeader * | pNode | Node in question
//
// @parm NmcAttribute * | pAttr | Attribute
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcAttrInteger (CNmcContext *pCtx, 
                     CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr)
{

    //
    // Get the integer value 
    //

    int nValue;
    if (!NmcGetIntegerValue (pCtx, &nValue, 0, 0))
        return;

    //
    // Store the value
    //

    switch (pAttr ->ulSize)
    {
        case 1:
            *((INT8 *) (((char *) pNode) + pAttr ->ulOffset)) = (INT8) nValue;
            break;
        case 2:
            *((INT16 *) (((char *) pNode) + pAttr ->ulOffset)) = (INT16) nValue;
            break;
        case 4:
            *((INT32 *) (((char *) pNode) + pAttr ->ulOffset)) = (INT32) nValue;
            break;
        default:
            assert (0);
            break;
    }
    return;
}

//-----------------------------------------------------------------------------
//
// @func Get an integer attribute
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CNwnMdlNodeHeader * | pNode | Node in question
//
// @parm NmcAttribute * | pAttr | Attribute
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcAttrInteger01 (CNmcContext *pCtx, 
                       CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr)
{

    //
    // Get the integer value 
    //

    int nValue;
    if (!NmcGetIntegerValue (pCtx, &nValue, 0, 0))
        return;

    //
    // Validate
    //

    if (nValue != 0 && nValue != 1)
    {
        pCtx ->GenerateError ("Attribute only allows a value of 0 or 1");
        return;
    }

    //
    // Store the value
    //

    switch (pAttr ->ulSize)
    {
        case 1:
            *((INT8 *) (((char *) pNode) + pAttr ->ulOffset)) = (INT8) nValue;
            break;
        case 2:
            *((INT16 *) (((char *) pNode) + pAttr ->ulOffset)) = (INT16) nValue;
            break;
        case 4:
            *((INT32 *) (((char *) pNode) + pAttr ->ulOffset)) = (INT32) nValue;
            break;
        default:
            assert (0);
            break;
    }
    return;
}

//-----------------------------------------------------------------------------
//
// @func Get an float attribute
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CNwnMdlNodeHeader * | pNode | Node in question
//
// @parm NmcAttribute * | pAttr | Attribute
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcAttrFloat (CNmcContext *pCtx, 
                   CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr)
{

    //
    // Get the float value 
    //

    float fValue;
    if (!NmcGetFloatValue (pCtx, &fValue, 0, 0))
        return;

    //
    // Store the value
    //

    switch (pAttr ->ulSize)
    {
        case 4:
            *((float *) (((char *) pNode) + pAttr ->ulOffset)) = (float) fValue;
            break;
        default:
            assert (0);
            break;
    }
    return;
}

//-----------------------------------------------------------------------------
//
// @func Get an vector3 attribute
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CNwnMdlNodeHeader * | pNode | Node in question
//
// @parm NmcAttribute * | pAttr | Attribute
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcAttrVector3 (CNmcContext *pCtx, 
                     CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr)
{

    //
    // Get the float value 
    //

    CVector3 v;
    if (!NmcGetVector3Value (pCtx, &v, g_vOrigin, 0))
        return;

    //
    // Store the value
    //

    *((CVector3 *) (((char *) pNode) + pAttr ->ulOffset)) = v;
    return;
}

//-----------------------------------------------------------------------------
//
// @func Get an string attribute
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CNwnMdlNodeHeader * | pNode | Node in question
//
// @parm NmcAttribute * | pAttr | Attribute
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcAttrString (CNmcContext *pCtx, 
                    CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr)
{

    //
    // Get a pointer to the value
    //

    char *psz = (((char *) pNode) + pAttr ->ulOffset);

    //
    // Get the string value 
    //

    NmcGetStringValue (pCtx, psz, (int) pAttr ->ulSize, NULL, 0);
    return;
}

//-----------------------------------------------------------------------------
//
// @func Get a parent attribute
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CNwnMdlNodeHeader * | pNode | Node in question
//
// @parm NmcAttribute * | pAttr | Attribute
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcAttrParent (CNmcContext *pCtx, 
                    CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr)
{
    pAttr;//4100

    //
    // Get the parent name
    //

    char szParent [32];
    if (!NmcGetStringValue (pCtx, szParent, _countof (szParent), NULL, 0))
        return;

    //
    // Get the current geomerty
    //

    CNwnMdlGeometryHeader *pGeometry = pCtx ->GetCurrentGeometry ();

    //
    // If the user specified NULL
    //

    if (stricmp (szParent, "NULL") == 0)
    {

        //
        // Issue an error if we have two root nodes
        //

        if (pGeometry ->m_pGeometry .GetData () != NULL)
        {
            pCtx ->GenerateError ("More than one node in "
                                  "geometry with parent set to NULL");
        }

        //
        // Otherwise, set the parent
        //

        else
        {
            pGeometry ->m_pGeometry .SetData (pNode);
        }
    }

    //
    // Otherwise, existing node
    //

    else
    {

        //
        // Invoke the routine to find the node
        //

        CNwnMdlNodeHeader *pParent = NmcFindNode (
            pGeometry ->m_pGeometry .GetData (), szParent);

        //
        // If not found, issue an error
        //

        if (pParent == NULL)
        {
            pCtx ->GenerateError ("Unknown parent node");
        }

        //
        // If found, attach
        //

        else
        {
            pNode ->m_pParentNode .SetData (pParent);
            pParent ->m_apChildren .Add (pNode);
        }
    }
    return;
}

//-----------------------------------------------------------------------------
//
// @func Get a integer array attribute
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CNwnMdlNodeHeader * | pNode | Node in question
//
// @parm NmcAttribute * | pAttr | Attribute
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcAttrIntegerArray (CNmcContext *pCtx, 
                          CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr)
{
    CNwnArray <UINT32> *pArray = (CNwnArray <UINT32> *) 
        (((char *) pNode) + pAttr ->ulOffset);
    CNmcListParser <UINT32>::ParseArray (pCtx, *pArray);
}

//-----------------------------------------------------------------------------
//
// @func Get a float array attribute
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CNwnMdlNodeHeader * | pNode | Node in question
//
// @parm NmcAttribute * | pAttr | Attribute
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcAttrFloatArray (CNmcContext *pCtx, 
                        CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr)
{
    CNwnArray <float> *pArray = (CNwnArray <float> *) 
        (((char *) pNode) + pAttr ->ulOffset);
    CNmcListParser <float>::ParseArray (pCtx, *pArray);
}

//-----------------------------------------------------------------------------
//
// @func Get a Vector3 array attribute
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CNwnMdlNodeHeader * | pNode | Node in question
//
// @parm NmcAttribute * | pAttr | Attribute
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcAttrVector3Array (CNmcContext *pCtx, 
                          CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr)
{
    CNwnArray <CVector3> *pArray = (CNwnArray <CVector3> *) 
        (((char *) pNode) + pAttr ->ulOffset);
    CNmcListParser <CVector3>::ParseArray (pCtx, *pArray);
}

//-----------------------------------------------------------------------------
//
// @func Get a Quaternion array attribute
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CNwnMdlNodeHeader * | pNode | Node in question
//
// @parm NmcAttribute * | pAttr | Attribute
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcAttrQuaternionArray (CNmcContext *pCtx, 
                             CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr)
{
    CNwnArray <CQuaternion> *pArray = (CNwnArray <CQuaternion> *) 
        (((char *) pNode) + pAttr ->ulOffset);
    CNmcListParser <CQuaternion>::ParseArray (pCtx, *pArray);
}

//-----------------------------------------------------------------------------
//
// @func Get a weight array attribute
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CNwnMdlNodeHeader * | pNode | Node in question
//
// @parm NmcAttribute * | pAttr | Attribute
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcAttrWeightArray (CNmcContext *pCtx, 
                         CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr)
{
    CNwnArray <NwnMdlSkinWeight> *pArray = (CNwnArray <NwnMdlSkinWeight> *) 
        (((char *) pNode) + pAttr ->ulOffset);
    CNmcListParser <NwnMdlSkinWeight>::ParseArray (pCtx, *pArray);
}

//-----------------------------------------------------------------------------
//
// @func Get a String array attribute
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CNwnMdlNodeHeader * | pNode | Node in question
//
// @parm NmcAttribute * | pAttr | Attribute
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcAttrStringArray (CNmcContext *pCtx, 
                         CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr)
{
    CNwnArray <CNwnPointer <char> > *pArray = (CNwnArray <CNwnPointer <char> > *) 
        (((char *) pNode) + pAttr ->ulOffset);
    CNmcListParser <CNwnPointer <char> >::ParseArray (pCtx, *pArray);
}

//-----------------------------------------------------------------------------
//
// @func Get an UINT32 flag
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CNwnMdlNodeHeader * | pNode | Node in question
//
// @parm NmcAttribute * | pAttr | Attribute
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcAttrUINT32Flag (CNmcContext *pCtx, 
                        CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr)
{

    //
    // Get the integer value 
    //

    int nValue;
    if (!NmcGetIntegerValue (pCtx, &nValue, 0, 0))
        return;

    //
    // Get a pointer to the data
    //

    UINT32 *pul = (UINT32 *) (((char *) pNode) + pAttr ->ulOffset);

    //
    // Set or clear
    //

    if (nValue)
        *pul |= pAttr ->ulSize;
    else
        *pul &= ~pAttr ->ulSize;
    return;
}

//-----------------------------------------------------------------------------
//
// @func Get a list of vertex indices
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CNwnMdlNodeHeader * | pNode | Node in question
//
// @parm NmcAttribute * | pAttr | Attribute
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcAttrVertexIndices (CNmcContext *pCtx, CNwnMdlNodeHeader *pNode, 
                           NmcAttribute *pAttr)
{

    //
    // Get a pointer to the array
    //

    CNwnArray <CNwnPointer <UINT32> > *pArray = 
        (CNwnArray <CNwnPointer <UINT32> > *) 
        (((char *) pNode) + pAttr ->ulOffset);

    //
    // Get the number of vertex lists
    //

    int nCount;
    if (!NmcGetIntegerValue (pCtx, &nCount, 0, 0))
        return;

    //
    // Get each block
    //

    for (int i = 0; i < nCount; i++)
    {

        //
        // Get the number of entries in this block
        //

        if (!pCtx ->ReadNextLine ())
        {
            pCtx ->GenerateError ("Unexpected end of file while processing attribute");
            return;
        }

        //
        // Get the count
        //

        int nIndices;
        if (!NmcGetIntegerValue (pCtx, &nIndices, 0, 0))
            return;

        //
        // Allocate an array
        //

        UINT32 *pul = new UINT32 [nIndices];
        pArray ->Add (pul);

        //
        // Get the indices
        //

        for (int j = 0; j < nIndices; j++)
        {
            if (!pCtx ->ReadNextLine ())
            {
                pCtx ->GenerateError ("Unexpected end of file while processing attribute");
                return;
            }
            int nIndex;
            if (!NmcGetIntegerValue (pCtx, &nIndex, 0, 0))
                return;
            pul [j] = (UINT32) nIndex;
        }
    }
}

//-----------------------------------------------------------------------------
//
// @func Read the AABB values
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CNwnPointer <NwnMdlAABBEntry> * | pEntryPtr | Entry 
//
// @parm bool | fReadLine | If true, read the next line
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

bool NmcAttrAABBEntry (CNmcContext *pCtx, 
                       CNwnPointer <NwnMdlAABBEntry> *pEntryPtr, bool fReadLine)
{

    //
    // Move to the next line
    //

    if (fReadLine  && !pCtx ->ReadNextLine ())
    {
        pCtx ->GenerateError ("Unexpected end of file while processing an \"aabb\" tree");
        return false;
    }

    //
    // Read the root entry data
    //

    int nValue;
    NwnMdlAABBEntry sEntry;
    if (!NmcGetVector3Value (pCtx, &sEntry .vBMin, g_vOrigin, 0) ||
        !NmcGetVector3Value (pCtx, &sEntry .vBMax, g_vOrigin, 0) ||
        !NmcGetIntegerValue (pCtx, &nValue, 0, 0))
        return false;
    sEntry .ulLeafFace = (UINT32) nValue;
    sEntry .ulPlane = 0;

    //
    // Save the data in a new structure
    //

    NwnMdlAABBEntry *pEntry = new NwnMdlAABBEntry;
    memcpy (pEntry, &sEntry, sizeof (sEntry));
    pEntryPtr ->SetData (pEntry);


    //
    // Get the plane
    //

    bool fHadPlane = NmcGetIntegerValue (pCtx, &nValue, 0, NmcGet_NoErrors);

    //
    // If we have children
    //

    if (pEntry ->ulLeafFace == 0xFFFFFFFF)
    {

        //
        // Read the children
        ///

        if (!NmcAttrAABBEntry (pCtx, &pEntry ->psLeft, true) ||
            !NmcAttrAABBEntry (pCtx, &pEntry ->psRight, true))
            return false;

        //
        // If we didn't have the plane
        //

        if (!fHadPlane)
        {
            NwnMdlAABBEntry *pLeft = pEntry ->psLeft .GetData ();
            NwnMdlAABBEntry *pRight = pEntry ->psRight .GetData ();
            CVector3 vLeft = pLeft ->vBMin + pLeft ->vBMax;
            CVector3 vRight = pRight ->vBMin + pRight ->vBMax;
            CVector3 vDelta = vRight - vLeft;
            if (fabs (vDelta .m_x) > fabs (vDelta .m_y) &&
                fabs (vDelta .m_x) > fabs (vDelta .m_z))
                pEntry ->ulPlane = vDelta .m_x > 0.0f ? 0x01 : 0x08;
            else if (fabs (vDelta .m_y) > fabs (vDelta .m_z)) 
                pEntry ->ulPlane = vDelta .m_y > 0.0f ? 0x02 : 0x10;
            else
                pEntry ->ulPlane = vDelta .m_z > 0.0f ? 0x04 : 0x20;
        }

        //
        // Otherwise, save it
        //

        else
        {
            pEntry ->ulPlane = (UINT32) nValue;
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
//
// @func Read the AABB values
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CNwnMdlNodeHeader * | pNode | Node in question
//
// @parm NmcAttribute * | pAttr | Attribute
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcAttrAABBEntryArray (CNmcContext *pCtx, CNwnMdlNodeHeader *pNode, 
                            NmcAttribute *pAttr)
{

    //
    // Get a pointer to the entry root
    //

    CNwnPointer <NwnMdlAABBEntry> *pEntryPtr = (CNwnPointer <NwnMdlAABBEntry> *) 
        (((char *) pNode) + pAttr ->ulOffset);

    //
    // Read the array
    //

    NmcAttrAABBEntry (pCtx, pEntryPtr, false);
}

//-----------------------------------------------------------------------------
//
// @func Get a mesh integer array attribute
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CNwnMdlNodeHeader * | pNode | Node in question
//
// @parm NmcAttribute * | pAttr | Attribute
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcAttrMeshIntegerArray (CNmcContext *pCtx, 
                              CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr)
{
    CNwnMdlMeshHeader *pMesh = static_cast <CNwnMdlMeshHeader *> (pNode);
    CNwnArray <UINT32> *pArray = (CNwnArray <UINT32> *) 
        (((char *) pMesh ->m_pvTempMeshData) + pAttr ->ulOffset);
    CNmcListParser <UINT32>::ParseArray (pCtx, *pArray);
}

//-----------------------------------------------------------------------------
//
// @func Get a mesh Vector3 array attribute
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CNwnMdlNodeHeader * | pNode | Node in question
//
// @parm NmcAttribute * | pAttr | Attribute
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcAttrMeshVector3Array (CNmcContext *pCtx, 
                              CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr)
{
    CNwnMdlMeshHeader *pMesh = static_cast <CNwnMdlMeshHeader *> (pNode);
    CNwnArray <CVector3> *pArray = (CNwnArray <CVector3> *) 
        (((char *) pMesh ->m_pvTempMeshData) + pAttr ->ulOffset);
    CNmcListParser <CVector3>::ParseArray (pCtx, *pArray);
}

//-----------------------------------------------------------------------------
//
// @func Get a mesh NmcTexIndices array attribute
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CNwnMdlNodeHeader * | pNode | Node in question
//
// @parm NmcAttribute * | pAttr | Attribute
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcAttrMeshTexIndicesArray (CNmcContext *pCtx, 
                                 CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr)
{
    CNwnMdlMeshHeader *pMesh = static_cast <CNwnMdlMeshHeader *> (pNode);
    CNwnArray <NmcTexIndices> *pArray = (CNwnArray <NmcTexIndices> *) 
        (((char *) pMesh ->m_pvTempMeshData) + pAttr ->ulOffset);
    CNmcListParser <NmcTexIndices>::ParseArray (pCtx, *pArray);
}

//-----------------------------------------------------------------------------
//
// @func Get a mesh face array attribute
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CNwnMdlNodeHeader * | pNode | Node in question
//
// @parm NmcAttribute * | pAttr | Attribute
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcAttrMeshFaceArray (CNmcContext *pCtx, 
                           CNwnMdlNodeHeader *pNode, NmcAttribute *pAttr)
{
    CNwnMdlMeshHeader *pMesh = static_cast <CNwnMdlMeshHeader *> (pNode);
    CNwnArray <NmcMeshFace> *pArray = (CNwnArray <NmcMeshFace> *) 
        (((char *) pMesh ->m_pvTempMeshData) + pAttr ->ulOffset);
    CNmcListParser <NmcMeshFace>::ParseArray (pCtx, *pArray);
}


/* vim: set sw=4: */
