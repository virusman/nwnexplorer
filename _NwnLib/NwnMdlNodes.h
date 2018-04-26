#ifndef ETS_NWNMDLNODES_H
#define ETS_NWNMDLNODES_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnMdlNodes.h - Nwn model node support |
//
// This module contains the definition of the model nodes.
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
// Based on information from:
//
// http://nwn-j3d.sf.net
//
// @end
//
// $History: CnfMainWnd.h $
//      
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include "NwnArray.h"
#include "NwnStreams.h"
#include "../_MathLib/Vector3.h"
#include "../_MathLib/Vector2.h"
#include "../_MathLib/Quaternion.h"

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

class CNwnMdlGeometryHeader;
class CNwnMdlModel;

//-----------------------------------------------------------------------------
//
// Node flags
//
//-----------------------------------------------------------------------------

enum NwnMdlNF
{
    NwnMdlNF_HasHeader                  = 0x00000001,
    NwnMdlNF_HasLight                   = 0x00000002,
    NwnMdlNF_HasEmitter                 = 0x00000004,
    NwnMdlNF_HasCamera                  = 0x00000008,
    NwnMdlNF_HasReference               = 0x00000010,
    NwnMdlNF_HasMesh                    = 0x00000020,
    NwnMdlNF_HasSkin                    = 0x00000040,
    NwnMdlNF_HasAnim                    = 0x00000080,
    NwnMdlNF_HasDangly                  = 0x00000100,
    NwnMdlNF_HasAABB                    = 0x00000200,
    NwnMdlNF_HasUnknown                 = 0x00000400,
};

//-----------------------------------------------------------------------------
//
// Node types
//
//-----------------------------------------------------------------------------

enum NwnMdlNT
{
    NwnMdlNT_Dummy                              = (NwnMdlNF_HasHeader),
    NwnMdlNT_Light                              = (NwnMdlNF_HasHeader | NwnMdlNF_HasLight),
    NwnMdlNT_Emitter                    = (NwnMdlNF_HasHeader | NwnMdlNF_HasEmitter),
    NwnMdlNT_Camera                             = (NwnMdlNF_HasHeader | NwnMdlNF_HasCamera),
    NwnMdlNT_Reference                  = (NwnMdlNF_HasHeader | NwnMdlNF_HasReference),
    NwnMdlNT_TriMesh                    = (NwnMdlNF_HasHeader | NwnMdlNF_HasMesh),
    NwnMdlNT_SkinMesh                   = (NwnMdlNT_TriMesh | NwnMdlNF_HasSkin),
    NwnMdlNT_AnimMesh                   = (NwnMdlNT_TriMesh | NwnMdlNF_HasAnim),
    NwnMdlNT_DanglyMesh                 = (NwnMdlNT_TriMesh | NwnMdlNF_HasDangly),
    NwnMdlNT_AABBMesh                   = (NwnMdlNT_TriMesh | NwnMdlNF_HasAABB),
    NwnMdlNT_Unknown421Mesh             = (NwnMdlNT_TriMesh | NwnMdlNF_HasUnknown),
};

//-----------------------------------------------------------------------------
//
// Controller data types
//
//-----------------------------------------------------------------------------

enum NwnMdlCT
{

    // Common to all nodes

    NwnMdlCT_Position                           = 8,    
    NwnMdlCT_Orientation                        = 20,   
    NwnMdlCT_Scale                              = 36,   
    NwnMdlCT_Wirecolor                          = 20004,        

    // Light

    NwnMdlCT_Color                              = 76,   
    NwnMdlCT_Radius                             = 88,   
    NwnMdlCT_ShadowRadius                       = 96,   
    NwnMdlCT_VerticalDisplacement               = 100,  
    NwnMdlCT_Multiplier                         = 140,  

    // Emitter 

    NwnMdlCT_AlphaEnd                           = 80,   
    NwnMdlCT_AlphaStart                         = 84,   
    NwnMdlCT_BirthRate                          = 88,   
    NwnMdlCT_Bounce_Co                          = 92,   
    NwnMdlCT_ColorEnd                           = 96,   
    NwnMdlCT_ColorStart                         = 108,  
    NwnMdlCT_CombineTime                        = 120,  
    NwnMdlCT_Drag                               = 124,  
    NwnMdlCT_FPS                                = 128,  
    NwnMdlCT_FrameEnd                           = 132,  
    NwnMdlCT_FrameStart                         = 136,  
    NwnMdlCT_Grav                               = 140,  
    NwnMdlCT_LifeExp                            = 144,  
    NwnMdlCT_Mass                               = 148,  
    NwnMdlCT_P2P_Bezier2                        = 152,  
    NwnMdlCT_P2P_Bezier3                        = 156,  
    NwnMdlCT_ParticleRot                        = 160,  
    NwnMdlCT_RandVel                            = 164,  
    NwnMdlCT_SizeStart                          = 168,  
    NwnMdlCT_SizeEnd                            = 172,  
    NwnMdlCT_SizeStart_Y                        = 176,  
    NwnMdlCT_SizeEnd_Y                          = 180,  
    NwnMdlCT_Spread                             = 184,  
    NwnMdlCT_Threshold                          = 188,  
    NwnMdlCT_Velocity                           = 192,  
    NwnMdlCT_XSize                              = 196,  
    NwnMdlCT_YSize                              = 200,  
    NwnMdlCT_BlurLength                         = 204,  
    NwnMdlCT_LightningDelay                     = 208,  
    NwnMdlCT_LightningRadius                    = 212,  
    NwnMdlCT_LightningScale                     = 216,  
    NwnMdlCT_Detonate                           = 228,  
    NwnMdlCT_AlphaMid                           = 464,          
    NwnMdlCT_ColorMid                           = 468,  
    NwnMdlCT_PercentStart                       = 480,  
    NwnMdlCT_PercentMid                         = 481,  
    NwnMdlCT_PercentEnd                         = 482,  
    NwnMdlCT_SizeMid                            = 484,  
    NwnMdlCT_SizeMid_Y                          = 488,  

    // Meshes

    NwnMdlCT_SelfIllumColor                     = 100,  
    NwnMdlCT_Alpha                              = 128,  
};

//-----------------------------------------------------------------------------
//
// Emitter flags
//
//-----------------------------------------------------------------------------

enum NwnMdlEF
{
    NwnMdlEF_P2P                                        = 0x0001,
    NwnMdlEF_P2PSel                                     = 0x0002,
    NwnMdlEF_AffectedByWind                     = 0x0004,
    NwnMdlEF_IsTinted                           = 0x0008,
    NwnMdlEF_Bounce                                     = 0x0010,
    NwnMdlEF_Random                                     = 0x0020,
    NwnMdlEF_Inherit                            = 0x0040,
    NwnMdlEF_InheritVel                         = 0x0080,
    NwnMdlEF_InheritLocal                       = 0x0100,
    NwnMdlEF_Splat                                      = 0x0200,
    NwnMdlEF_InheritPart                        = 0x0400,
};

//-----------------------------------------------------------------------------
//
// Triangle mode
//
//-----------------------------------------------------------------------------

enum NwnMdlTM
{
    NwnMdlTM_Triangle                           = 0x03,
    NwnMdlTM_TriStrip                           = 0x04,
};

//-----------------------------------------------------------------------------
//
// Forwards
//
//-----------------------------------------------------------------------------

class CNwnMdlDummyNode;
class CNwnMdlLightNode;
class CNwnMdlEmitterNode;
class CNwnMdlCameraNode;
class CNwnMdlReferenceNode;
class CNwnMdlMeshHeader;
class CNwnMdlTriMeshNode;
class CNwnMdlSkinMeshNode;
class CNwnMdlAnimMeshNode;
class CNwnMdlDanglyMeshNode;
class CNwnMdlAABBMeshNode;

//-----------------------------------------------------------------------------
//
// Controller key structure
//
//-----------------------------------------------------------------------------

struct NwnMdlControllerKey
{
    INT32                                       lType;                                                  // 0x0000
    INT16                                       sRows;                                                  // 0x0004
    INT16                                       sKeyOffset;                                             // 0x0006
    INT16                                       sDataOffset;                                    // 0x0008
    INT8                                        bColumns;                                               // 0x000A
    INT8                                        bPad;                                                   // 0x000B
    // 0x000C

    //
    // If bColumns == -1, then we have a keyed controller with
    //          NO VALUES AT ALL INCLUDING A KEY!!! (detonatekey)
    // If sRows == 1, then this is a normal controller
    // If sRows > 1 && bColumns <= 0x10, then this is a keyed controller
    // If sRows > 1 && bColumns > 0x10, then this is a bezier keyed controller
    //
};

//-----------------------------------------------------------------------------
//
// Face structure
//
//-----------------------------------------------------------------------------

struct NwnMdlFace
{
    CVector3                            vPlaneNormal;                                   // 0x0000
    float                                       fPlaneDistance;                                 // 0x000C
    UINT32                                      ulSurface;                                              // 0x0010
    INT16                                       asAdjFace [3];                                  // 0x0014
    UINT16                                      ausVertex [3];                                  // 0x001A
    // 0x0020
};

//-----------------------------------------------------------------------------
//
// AABB entry
//
//-----------------------------------------------------------------------------

struct NwnMdlAABBEntry
{
    CVector3                            vBMin;
    CVector3                            vBMax;
    CNwnPointer <NwnMdlAABBEntry>       
        psLeft;
    CNwnPointer <NwnMdlAABBEntry>       
        psRight;
    UINT32                                      ulLeafFace;
    UINT32                                      ulPlane;        // 0x01 = Positive X
    // 0x02 = Positive Y
    // 0x04 = Positive Z
    // 0x08 = Negative X
    // 0x10 = Negative Y
    // 0x20 = Negative Z
};

//-----------------------------------------------------------------------------
//
// Skin weight, transient value used by the compiler not in binary
//
//-----------------------------------------------------------------------------

struct NwnMdlSkinWeight
{
    char szBones [4] [32];
    float fWeights [4];
    // The following values are not parsed
    int nCount;
    int nNodeIndices [4];
};

//-----------------------------------------------------------------------------
//
// Common node header
//
//-----------------------------------------------------------------------------

class CNwnMdlNodeHeader
{
    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CNwnMdlNodeHeader (const char *pszName);

        // @cmember General destructor

        ~CNwnMdlNodeHeader ();

        // @access Public methods
    public:

        // @cmember Convert a node from relative to absolute addressing

        void RelativeToAbsolute (unsigned char *pauchModelData, 
                                 unsigned char *pauchRawData);

        // @cmember Serialize the node

        void Serialize (CNwnStream *pModelData, CNwnStream *pRawData);

        // @cmember Decompile the node

        void Decompile (CNwnStream *pStream, CNwnMdlModel *pModel,
                        CNwnMdlGeometryHeader *pParentGeom, CNwnMdlNodeHeader *pParentNode, 
                        bool fIncludeChildren, bool fIncludeExtended, bool fIncludeSmoothing, 
                        unsigned char *pauchModelData, unsigned char *pauchRawData);

        // @cmember Delete the node (not for relative models)

        void DeleteNode ();

        // @cmember Internal relative to absolute routine

        void _R2A (unsigned char *pauchModelData, 
                   unsigned char *pauchRawData);

        // @access Protected methods
    protected:

        // @cmember Internal serializer

        void NodeHeaderSerialize (CNwnMdlNodeHeader *pOutNode, 
                                  CNwnStream *pModelData, CNwnStream *pRawData);

        // @access Public member variables
    public:

        UINT32                                  m_aulNodeRoutines [6];                  // 0x0000
        UINT32                                  m_ulInheritColor;                               // 0x0018
        INT32                                   m_nPartNumber;                                  // 0x001C
        char                                    m_szName [32];                                  // 0x0020
        CNwnPointer <CNwnMdlGeometryHeader>
            m_pGeometryHeader;                          // 0x0040
        CNwnPointer <CNwnMdlNodeHeader>
            m_pParentNode;                                      // 0x0044
        CNwnArray <CNwnPointer <CNwnMdlNodeHeader> >    
            m_apChildren;                                       // 0x0048
        CNwnArray <NwnMdlControllerKey>         
            m_aControllerKey;                           // 0x0054
        CNwnArray <float>       m_aControllerData;                              // 0x0060
        UINT32                                  m_ulFlags;                                              // 0x006C
        // 0x0070
};

//-----------------------------------------------------------------------------
//
// Dummy Node 
//
//-----------------------------------------------------------------------------

class CNwnMdlDummyNode : public CNwnMdlNodeHeader
{
    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CNwnMdlDummyNode (const char *pszName);

        // @cmember General destructor

        ~CNwnMdlDummyNode ();

        // @access Public methods
    public:

        // @cmember Internal relative to absolute routine

        void _R2A (unsigned char *pauchModelData, 
                   unsigned char *pauchRawData);

        // @cmember Internal serializer

        void _Serialize (CNwnStream *pModelData, CNwnStream *pRawData);

        // @access Public member variables
    public:
        // 0x0070
};

//-----------------------------------------------------------------------------
//
// Light Node 
//
//-----------------------------------------------------------------------------

class CNwnMdlLightNode : public CNwnMdlNodeHeader
{
    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CNwnMdlLightNode (const char *pszName);

        // @cmember General destructor

        ~CNwnMdlLightNode ();

        // @access Public methods
    public:

        // @cmember Internal relative to absolute routine

        void _R2A (unsigned char *pauchModelData, 
                   unsigned char *pauchRawData);

        // @cmember Internal serializer

        void _Serialize (CNwnStream *pModelData, CNwnStream *pRawData);

        // @access Public member variables
    public:

        float                                   m_fFlareRadius;                                 // 0x0070
        CNwnArray <UINT32>      m_aSomething1;                                  // 0x0074
        CNwnArray <float>       m_afFlareSizes;                                 // 0x0080
        CNwnArray <float>       m_afFlarePositions;                             // 0x008C
        CNwnArray <CVector3>    m_avFlareColorShifts;                   // 0x0098
        CNwnArray <CNwnPointer <char> >   
            m_apszTextureNames;                         // 0x00A4
        UINT32                                  m_ulLightPriority;                              // 0x00B0
        UINT32                                  m_ulAmbientOnly;                                // 0x00B4
        UINT32                                  m_ulDynamicType;                                // 0x00B8
        UINT32                                  m_ulAffectDynamic;                              // 0x00BC
        UINT32                                  m_ulShadow;                                             // 0x00C0
        UINT32                                  m_ulGenerateFlare;                              // 0x00C4
        UINT32                                  m_ulFadingLight;                                // 0x00C8
        // 0x00CC
};

//-----------------------------------------------------------------------------
//
// Emitter Node 
//
//-----------------------------------------------------------------------------

class CNwnMdlEmitterNode : public CNwnMdlNodeHeader
{
    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CNwnMdlEmitterNode (const char *pszName);

        // @cmember General destructor

        ~CNwnMdlEmitterNode ();

        // @access Public methods
    public:

        // @cmember Internal relative to absolute routine

        void _R2A (unsigned char *pauchModelData, 
                   unsigned char *pauchRawData);

        // @cmember Internal serializer

        void _Serialize (CNwnStream *pModelData, CNwnStream *pRawData);

        // @access Public member variables
    public:

        float                                   m_fDeadSpace;                                   // 0x0070
        float                                   m_fBlastRadius;                                 // 0x0074
        float                                   m_fBlastLength;                                 // 0x0078
        UINT32                                  m_ulXGrid;                                              // 0x007C 
        UINT32                                  m_ulYGrid;                                              // 0x0080
        UINT32                                  m_ulSpawnType;                                  // 0x0084
        char                                    m_szUpdate [32];                                // 0x0088 
        char                                    m_szRender [32];                                // 0x00A8
        char                                    m_szBlend [32];                                 // 0x00C8
        char                                    m_szTexture [64];                               // 0x00E8
        char                                    m_szChunkName [16];                             // 0x0128
        UINT32                                  m_ulTwoSidedTex;                                // 0x0138
        UINT32                                  m_ulLoop;                                               // 0x013C
        UINT16                                  m_usRenderOrder;                                // 0x0140
        UINT16                                  m_usPad;                                                // 0x0142
        UINT32                                  m_ulEmitterFlags;                               // 0x0144
        // 0x0148
};

//-----------------------------------------------------------------------------
//
// Camera Node 
//
//-----------------------------------------------------------------------------

class CNwnMdlCameraNode : public CNwnMdlNodeHeader
{
    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CNwnMdlCameraNode (const char *pszName);

        // @cmember General destructor

        ~CNwnMdlCameraNode ();

        // @access Public methods
    public:

        // @cmember Internal relative to absolute routine

        void _R2A (unsigned char *pauchModelData, 
                   unsigned char *pauchRawData);

        // @cmember Internal serializer

        void _Serialize (CNwnStream *pModelData, CNwnStream *pRawData);

        // @access Public member variables
    public:
        // 0x0070
};

//-----------------------------------------------------------------------------
//
// Reference Node 
//
//-----------------------------------------------------------------------------

class CNwnMdlReferenceNode : public CNwnMdlNodeHeader
{
    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CNwnMdlReferenceNode (const char *pszName);

        // @cmember General destructor

        ~CNwnMdlReferenceNode ();

        // @access Public methods
    public:

        // @cmember Internal relative to absolute routine

        void _R2A (unsigned char *pauchModelData, 
                   unsigned char *pauchRawData);

        // @cmember Internal serializer

        void _Serialize (CNwnStream *pModelData, CNwnStream *pRawData);

        // @access Public member variables
    public:

        char                                    m_szRefModel [64];                              // 0x0070
        UINT32                                  m_ulReattachable;                               // 0x00B0
        // 0x00B4
};

//-----------------------------------------------------------------------------
//
// Mesh header
//
//-----------------------------------------------------------------------------

class CNwnMdlMeshHeader : public CNwnMdlNodeHeader
{
    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CNwnMdlMeshHeader (const char *pszName);

        // @cmember General destructor

        ~CNwnMdlMeshHeader ();

        // @access Public methods
    public:

        // @cmember Internal relative to absolute routine

        void _R2A (unsigned char *pauchModelData, 
                   unsigned char *pauchRawData);

        // @access Protected methods
    protected:

        // @cmember Internal serializer

        void MeshHeaderSerialize (CNwnMdlMeshHeader *pOutNode, 
                                  CNwnStream *pModelData, CNwnStream *pRawData);

        // @access Public member variables
    public:

        UINT32                                  m_aulMeshRoutines [2];                  // 0x0070
        CNwnArray <NwnMdlFace>   
            m_sFaces;                                           // 0x0078
        CVector3                                m_vBMin;                                                // 0x0084
        CVector3                                m_vBMax;                                                // 0x0090
        float                                   m_fRadius;                                              // 0x009C
        CVector3                                m_vBAverage;                                    // 0x00A0
        CVector3                                m_vDiffuse;                                             // 0x00AC
        CVector3                                m_vAmbient;                                             // 0x00B8
        CVector3                                m_vSpecular;                                    // 0x00C4
        float                                   m_fShininess;                                   // 0x00D0
        UINT32                                  m_ulShadow;                                             // 0x00D4
        UINT32                                  m_ulBeaming;                                    // 0x00D8
        UINT32                                  m_ulRender;                                             // 0x00DC
        UINT32                                  m_ulTransparencyHint;                   // 0x00E0
        UINT32                                  m_ulRenderHint;                                   // 0x00E4
        char                                    m_szTextures [3] [64];                  // 0x00E8 
        char                                    m_szMaterialName [64];                  // 0x01A8
        UINT32                                  m_ulTileFade;                                   // 0x01E8
        CNwnArray <CNwnPointer <UINT32> >   
            m_sVertexIndices;                           // 0x01EC
        CNwnArray <UINT32>      m_sLeftOverFaces;                               // 0x01F8
        CNwnArray <UINT32>      m_sVertexIndicesCount;                  // 0x0204
        CNwnArray <CNwnPointer <UINT16> >   
            m_sRawVertexIndices;                        // 0x0210
        UINT32                                  m_ulSomething3Offset;                   // 0x021C
        UINT32                                  m_ulSomething3Count;                    // 0x0220
        UINT8                                   m_ucTriangleMode;                               // 0x0224
        UINT8                                   m_ucPad [3];                                    // 0x0225
        void                                    *m_pvTempMeshData;                              // 0x0228
        CNwnPointer2 <CVector3> m_pavVerts;                                             // 0x022C
        UINT16                                  m_usVertexCount;                                // 0x0230
        UINT16                                  m_usTextureCount;                               // 0x0232
        CNwnPointer2 <CVector2> m_pavTVerts [4];                                // 0x0234
        CNwnPointer2 <CVector3> m_pavNormals;                                   // 0x0244
        CNwnPointer2 <UINT32>   m_paColors;                                             // 0x0248
        CNwnPointer2 <CVector3> m_pavBumpmapAnim1;                              // 0x024C
        CNwnPointer2 <CVector3> m_pavBumpmapAnim2;                              // 0x0250
        CNwnPointer2 <CVector3> m_pavBumpmapAnim3;                              // 0x0254
        CNwnPointer2 <CVector3> m_pavBumpmapAnim4;                              // 0x0258
        CNwnPointer2 <CVector3> m_pavBumpmapAnim5;                              // 0x025C
        CNwnPointer2 <float>    m_pafBumpmapAnim6;                              // 0x0260
        UINT8                                   m_ucLightMapped;                                // 0x0264
        UINT8                                   m_ucRotateTexture;                              // 0x0265
        UINT16                                  m_usPad;                                                // 0x0266
        float                                   m_fFaceNormalSumDiv2;                   // 0x0268
        UINT32                                  m_ulUnknown1;                                   // 0x026C
        // 0x0270
};

//-----------------------------------------------------------------------------
//
// TriMesh node
//
//-----------------------------------------------------------------------------

class CNwnMdlTriMeshNode : public CNwnMdlMeshHeader
{
    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CNwnMdlTriMeshNode (const char *pszName);

        // @cmember General destructor

        ~CNwnMdlTriMeshNode ();

        // @access Public methods
    public:

        // @cmember Internal relative to absolute routine

        void _R2A (unsigned char *pauchModelData, 
                   unsigned char *pauchRawData);

        // @cmember Internal serializer

        void _Serialize (CNwnStream *pModelData, CNwnStream *pRawData);

        // @access Public member variables
    public:

        // 0x0270
};

//-----------------------------------------------------------------------------
//
// Skin node
//
//-----------------------------------------------------------------------------

class CNwnMdlSkinMeshNode : public CNwnMdlMeshHeader
{
    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CNwnMdlSkinMeshNode (const char *pszName);

        // @cmember General destructor

        ~CNwnMdlSkinMeshNode ();

        // @access Public methods
    public:

        // @cmember Internal relative to absolute routine

        void _R2A (unsigned char *pauchModelData, 
                   unsigned char *pauchRawData);

        // @cmember Internal serializer

        void _Serialize (CNwnStream *pModelData, CNwnStream *pRawData);

        // @access Public member variables
    public:

        CNwnArray <NwnMdlSkinWeight>
            m_aWeights;                                         // 0x0270
        CNwnPointer2 <float>    m_pafSkinWeights;                               // 0x027C
        CNwnPointer2 <INT16>    m_pasSkinBoneRefs;                              // 0x0280
        CNwnPointer2 <INT16>    m_pasNodeToBoneMap;                             // 0x0284
        UINT32                                  m_ulNodeToBoneCount;                    // 0x0288
        CNwnArray <CQuaternion> m_aQBoneRefInv;                                 // 0x028C
        CNwnArray <CVector3>    m_aTBoneRefInv;                                 // 0x0298
        CNwnArray <UINT32>      m_aBoneConstantIndices;                 // 0x02A4
        INT16                                   m_asBoneNodeNumbers [17];               // 0x02B0
        INT16                                   m_sPad;                                                 // 0x02D2
        // 0x02D4
};

//-----------------------------------------------------------------------------
//
// AnimMesh node
//
//-----------------------------------------------------------------------------

class CNwnMdlAnimMeshNode : public CNwnMdlMeshHeader
{
    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CNwnMdlAnimMeshNode (const char *pszName);

        // @cmember General destructor

        ~CNwnMdlAnimMeshNode ();

        // @access Public methods
    public:

        // @cmember Internal relative to absolute routine

        void _R2A (unsigned char *pauchModelData, 
                   unsigned char *pauchRawData);

        // @cmember Internal serializer

        void _Serialize (CNwnStream *pModelData, CNwnStream *pRawData);

        // @access Public member variables
    public:

        float                                   m_fSamplePeriod;                                // 0x0270
        CNwnArray <CVector3>    m_avAnimVerts;                                  // 0x0274
        CNwnArray <CVector3>    m_avAnimTVerts;                                 // 0x0280
        CNwnArray <CVector3>    m_avAnimNormals;                                // 0x028C
        CNwnPointer <CVector3>  m_pvVertexSets;                                 // 0x0298
        CNwnPointer <CVector2>  m_pvTextureSets;                                // 0x029C
        UINT32                                  m_ulVertexSetCount;                             // 0x02A0
        UINT32                                  m_ulTextureSetCount;                    // 0x02A4
        // 0x02A8
};

//-----------------------------------------------------------------------------
//
// DanglyMesh node
//
//-----------------------------------------------------------------------------

class CNwnMdlDanglyMeshNode : public CNwnMdlMeshHeader
{
    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CNwnMdlDanglyMeshNode (const char *pszName);

        // @cmember General destructor

        ~CNwnMdlDanglyMeshNode ();

        // @access Public methods
    public:

        // @cmember Internal relative to absolute routine

        void _R2A (unsigned char *pauchModelData, 
                   unsigned char *pauchRawData);

        // @cmember Internal serializer

        void _Serialize (CNwnStream *pModelData, CNwnStream *pRawData);

        // @access Public member variables
    public:

        CNwnArray <float>       m_afConstraints;                                // 0x0270
        float                                   m_fDisplacement;                                // 0x027C
        float                                   m_fTightness;                                   // 0x0280
        float                                   m_fPeriod;                                              // 0x0284
        // 0x0288
};

//-----------------------------------------------------------------------------
//
// AABB Mesh node
//
//-----------------------------------------------------------------------------

class CNwnMdlAABBMeshNode : public CNwnMdlMeshHeader
{
    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CNwnMdlAABBMeshNode (const char *pszName);

        // @cmember General destructor

        ~CNwnMdlAABBMeshNode ();

        // @access Public methods
    public:

        // @cmember Internal relative to absolute routine

        void _R2A (unsigned char *pauchModelData, 
                   unsigned char *pauchRawData);

        // @cmember Internal serializer

        void _Serialize (CNwnStream *pModelData, CNwnStream *pRawData);

        // @access Protected methods
    protected:

        // @cmember Delete an AABBEntry structure

        static void DeleteAABBEntry (NwnMdlAABBEntry *pEntry);

        // @cmember Convert an entry from relative to absolute

        static void _R2AAABBEntry (
            CNwnPointer <NwnMdlAABBEntry> &pAABBEntry,
            unsigned char *pauchModel);

        // @cmember Serialize an AABB entry

        static void SerializeAABBEntry (NwnMdlAABBEntry *pAABBEntry, 
                                        CNwnStream *pModelData);

        // @access Public member variables
    public:

        CNwnPointer <NwnMdlAABBEntry>
            m_pAABBTree;                                        // 0x0270
        // 0x0274
};

#endif // ETS_NWNMDLNODES_H

/* vim: set sw=4: */
