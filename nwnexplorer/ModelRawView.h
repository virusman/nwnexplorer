#ifndef ETS_MODELRAWVIEW_H
#define ETS_MODELRAWVIEW_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ModelRawView.h - Model view window |
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
// $History: HexView.h $
//      
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include "resource.h"
#include "DataRef.h"
#include "TextViewImpl.h"

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

class CModelRawFormatter;

enum BlockType
{
    BlockType_Unknown                   = 0,
    BlockType_ModelHeader               = 1,
    BlockType_AnimationHeader   = 2,
    BlockType_NodeHeader                = 3,
    BlockType_AnimationOffsets  = 4,
    BlockType_GenericOffsets    = 5,
    BlockType_AnimationEvents   = 6,
    BlockType_DummyNode                 = 7,
    BlockType_ControllerData    = 8,
    BlockType_ControllerKey             = 9,
    BlockType_TriMeshNode               = 10,
    BlockType_FaceData                  = 11,
    BlockType_FaceSetOffsets    = 12,
    BlockType_FaceSetFaces              = 13,
    BlockType_VertexArray               = 14,
    BlockType_TextureArray              = 15,
    BlockType_NormalArray               = 16,
    BlockType_ColorArray                = 17,
    BlockType_FaceSetCounts             = 18,
    BlockType_AnimMeshNode              = 19,
    BlockType_EmitterNode               = 20,
    BlockType_LightNode                 = 21,
    BlockType_AABBNode                  = 22,
    BlockType_AABBTree                  = 23,
    BlockType_DanglyMeshNode    = 24,
    BlockType_Constraints       = 25,
    BlockType_SkinNode                  = 26,
    BlockType_SkinUnkA                  = 27,
    BlockType_SkinQBoneRefInv   = 28,
    BlockType_SkinTBoneRefInv   = 29,
    BlockType_SkinUnkD                  = 30,
    BlockType_SkinBoneRef               = 31,
    BlockType_SkinBoneWeight    = 32,
    BlockType_ReferenceNode             = 33,
    BlockType_FaceAnimVertex    = 34,
    BlockType_FaceAnimValue             = 35,
    BlockType_FlareTexOffset    = 36,
    BlockType_FlareTex                  = 37,
    BlockType_FlareData                 = 38,
};

struct Block
{
    BlockType nBlockType;
    int nSize;
    bool fIsAnim;
};

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CModelRawView :
    public CTextViewImpl
{

    // @access Construction and destruction
    public:

        // @cmember General constructor

        CModelRawView (CData *pFile, bool fFormatted);

        // @cmember General destructor

        ~CModelRawView ();

        // @access Public static method
    public:

        static bool __cdecl PartitionCallback (CResModelRef *pFile, 
                                               void *lParam, void *pData, bool fIsGeometryHeader, UINT32 ulFlags, 
                                               int nOffset);

        // @access Public virtual method
    public:

        // @cmember Get the given window

        virtual CWindow *GetWindow (HWND hWnd);

        // @cmember Get the data

        virtual char *GetData ()
        {
            return (char *) (LPCSTR) m_strText;
        }

        // @cmember Get the size

        virtual unsigned long GetSize ()
        {
            return m_strText .GetLength ();
        }

        // @access Protected methods
    protected:

        void WriteBlock (CModelRawFormatter *pFormatter, Block &b, int nStart);

        static void BlockUsed (CAtlList <Block> &aBlocks, 
                               BlockType nBlockType, bool fIsAnim, int nOffset, 
                               size_t nSize);

        static void HandleMeshHeader (CResModelRef *pFile, 
                                      CAtlList <Block> &aBlocks, bool fIsAnim, 
                                      CNwnMdlMeshHeader *pMesh);

        // @access Protected members
    protected:

        // @cmember If true, format the model

        bool                                    m_fFormatted;

        // @cmember Pointer to the file

        CResModelRef                    m_sRes;

        // @cmember Text of the file

        CStringA                                m_strText;

};

#endif // ETS_MODELRAWVIEW_H


/* vim: set sw=4: */
