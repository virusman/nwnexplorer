#ifndef ETS_NWNMDLGEOMETRY_H
#define ETS_NWNMDLGEOMETRY_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnMdlGeometry.h - Nwn model geometry support |
//
// This module contains the definition of the Nwn model geometry.
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

#include "NwnDefines.h"
#include "NwnArray.h"
#include "NwnStreams.h"
#include "../_MathLib/Vector3.h"

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Geometry flags
//
//-----------------------------------------------------------------------------

enum NwnMdlGF
{
    NwnMdlGF_HasGeometry                = 0x01,
    NwnMdlGF_HasModel                   = 0x02,
    NwnMdlGF_HasAnimation               = 0x04,
    NwnMdlGF_LoadedBinary               = 0x80,
};

//-----------------------------------------------------------------------------
//
// Geometry types 
//
//-----------------------------------------------------------------------------

enum NwnMdlGT
{
    NwnMdlGT_Geometry                   = 1,
    NwnMdlGT_Model                              = 2,
    NwnMdlGT_Animation                  = 5,
};

//-----------------------------------------------------------------------------
//
// Model classification
//
//-----------------------------------------------------------------------------

enum NwnMdlCls
{
    NwnMdlCls_Effect                    = 1,
    NwnMdlCls_Tile                              = 2,
    NwnMdlCls_Character                 = 4,
    NwnMdlCls_Door                              = 8,
};

//-----------------------------------------------------------------------------
//
// Forwards
//
//-----------------------------------------------------------------------------

class CNwnMdlGeometryHeader;
class CNwnMdlNodeHeader;
class CNwnMdlModel;
class CNwnMdlAnimation;

//-----------------------------------------------------------------------------
//
// Animation event
//
//-----------------------------------------------------------------------------

struct NwnMdlAnimationEvent
{
    float               fAfter;
    char                szEventName [32];
};

//-----------------------------------------------------------------------------
//
// Geometry header
//
//-----------------------------------------------------------------------------

class CNwnMdlGeometryHeader 
{
    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CNwnMdlGeometryHeader (const char *pszName);

        // @cmember General destructor

        ~CNwnMdlGeometryHeader ();

        // @access Public methods
    public:

        // @cmember Convert from relative to absolute addressing

        void RelativeToAbsolute (unsigned char *pauchModelData, 
                                 unsigned char *pauchRawData);

        // @access Public member variables
    public:
        UINT32                                  m_aulGeomRoutines [2];                  // 0x0000
        char                                    m_szName [64];                                  // 0x0008
        CNwnPointer <CNwnMdlNodeHeader>
            m_pGeometry;                                        // 0x0048
        UINT32                                  m_ulNodeCount;                                  // 0x004C
        CNwnArray <UINT32>      m_aRtArray1;                                    // 0x0050
        CNwnArray <UINT32>      m_aRtArray2;                                    // 0x005C
        UINT32                                  m_ulUnknown2;                                   // 0x0068
        UINT8                                   m_ucGeometryType;                               // 0x006C
        UINT8                                   m_auchPad [3];                                  // 0x006D
        // 0x0070
};

//-----------------------------------------------------------------------------
//
// Animation header
//
//-----------------------------------------------------------------------------

class CNwnMdlAnimation : public CNwnMdlGeometryHeader 
{
    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CNwnMdlAnimation (const char *pszName);

        // @cmember General destructor

        ~CNwnMdlAnimation ();

        // @access Public methods
    public:

        // @cmember Convert from relative to absolute addressing

        void RelativeToAbsolute (unsigned char *pauchModelData, 
                                 unsigned char *pauchRawData);

        // @cmember Serialize the model

        void Serialize (CNwnStream *pModelData, CNwnStream *pRawData);

        // @cmember Decompile the model

        void Decompile (CNwnStream *pStream, CNwnMdlModel *pModel,
                        bool fIncludeChildren, bool fIncludeExtended, bool fIncludeSmoothing, 
                        unsigned char *pauchModelData, unsigned char *pauchRawData);

        // @access Public member variables
    public:
        float                                   m_fLength;                                              // 0x0070
        float                                   m_fTransTime;                                   // 0x0074
        char                                    m_szAnimRoot [64];                              // 0x0078
        CNwnArray <NwnMdlAnimationEvent>   
            m_asEvents;                                         // 0x00B8
        // 0x00C4
};

//-----------------------------------------------------------------------------
//
// Model header
//
//-----------------------------------------------------------------------------

class CNwnMdlModel : public CNwnMdlGeometryHeader
{
    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CNwnMdlModel (const char *pszName);

        // @cmember General destructor

        ~CNwnMdlModel ();

        // @access Public methods
    public:

        // @cmember Convert from relative to absolute addressing

        void RelativeToAbsolute (unsigned char *pauchModelData, 
                                 unsigned char *pauchRawData);

        // @cmember Serialize the model

        void Serialize (CNwnStream *pModelData, CNwnStream *pRawData);

        // @cmember Decompile the model

        void Decompile (CNwnStream *pStream, bool fIncludeChildren, 
                        bool fIncludeExtended, bool fIncludeSmoothing, 
                        unsigned char *pauchModelData, unsigned char *pauchRawData);

        // @cmember Delete the model

        void DeleteModel ();

        // @cmember Add a reference

        void AddRef ()
        {
            m_ulRefCount++;
        }

        // @cmember Release a reference

        void Release ()
        {
            m_ulRefCount--;
            if (m_ulRefCount == 0)
                DeleteModel ();
        }

        // @access Public member variables
    public:
        UINT8                                   m_aucFlags [2];                                 // 0x0070
        UINT8                                   m_ucClassification;                             // 0x0072
        UINT8                                   m_ucFog;                                                // 0x0073
        UINT32                                  m_ulRefCount;                                   // 0x0074
        CNwnArray <CNwnPointer <CNwnMdlAnimation> >
            m_apAnimations;                                     // 0x0078
        CNwnPointer <CNwnMdlModel>
            m_pSuperModel;                                      // 0x0084
        CVector3                                m_vBMin;                                                // 0x0088
        CVector3                                m_vBMax;                                                // 0x0094
        float                                   m_fRadius;                                              // 0x00A0
        float                                   m_fAnimationScale;                              // 0x00A4
        char                                    m_szSuperModel [64];                    // 0x00A8
        // 0x00E8
};

#endif // ETS_NWNMDLGEOMETRY_H

/* vim: set sw=4: */
