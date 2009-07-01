#ifndef ETS_MODELRAWFORMATTER_H
#define ETS_MODELRAWFORMATTER_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ModelRawFormatter.h - Model raw formatter |
//
// This module contains the definition of the model raw formatter.
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
#include "../_NwnLib/NwnStreams.h"

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CModelRawFormatter
{

    // @access Construction and destruction
    public:

        // @cmember General constructor

        CModelRawFormatter (CData *pFile, CNwnStream *pStream);

        // @cmember General destructor

        ~CModelRawFormatter ();

        // @access Public methods
    public:

        // Whole series of different formatting routines...

        void WriteUnknowns (const UINT32 *pulUnknowns, int nCount);

        void WriteUnknowns (const UINT32 *pulUnknowns, int nCount, int nIndex);

        void WriteRoutines (const UINT32 *pulRoutines, int nCount);

        void WriteNodeHeader (CNwnMdlNodeHeader *pNode, 
                              LPCSTR pszName, int nStart, size_t nSize);

        void WriteNodeControllerData (CNwnMdlNodeHeader *pNode);

        void WriteMeshHeader (CNwnMdlMeshHeader *pMesh, 
                              LPCSTR pszName, int nStart, size_t nSize);

        void WriteMeshVertices (CNwnMdlMeshHeader *pMesh);

        void WriteMeshFaceSets (CNwnMdlMeshHeader *pMesh);

        void WriteMeshFaces (CNwnMdlMeshHeader *pMesh);

        void WriteHex (int nStart, size_t nSize, int nSkip);

        void WriteModelHeader (int nStart, size_t nSize);

        void WriteAnimationHeader (int nStart, size_t nSize);

        void WriteControllerData (int nStart, size_t nSize);

        void WriteControllerKey (int nStart, size_t nSize);

        void WriteNodeHeader (int nStart, size_t nSize);

        void WriteDummyNode (int nStart, size_t nSize);

        void WriteTriMeshNode (int nStart, size_t nSize);

        void WriteSkinNode (int nStart, size_t nSize);

        void WriteAABBNode (int nStart, size_t nSize);

        void WriteDanglyMeshNode (int nStart, size_t nSize);

        void WriteAnimMeshNode (int nStart, size_t nSize);

        void WriteEmitterNode (bool fIsAnim, int nStart, size_t nSize);

        void WriteLightNode (bool fIsAnim, int nStart, size_t nSize);

        void WriteReferenceNode (int nStart, size_t nSize);

        void WriteFaceData (int nStart, size_t nSize);

        void WriteFaceSet (int nFaceSet, int nStart, size_t nSize);

        void WriteVertices (int nStart, size_t nSize);

        void WriteBumpmapVectors (int nFrame, int nStart, size_t nSize);

        void WriteBumpmapFloats (int nStart, size_t nSize);

        void WriteTextures (int nStart, size_t nSize);

        void WriteNormals (int nStart, size_t nSize);

        void WriteColors (int nStart, size_t nSize);

        void WriteConstraints (int nStart, size_t nSize);

        void WriteAABBTree (int nStart, size_t nSize);

        void WriteSkinUnkA (int nStart, size_t nSize);

        void WriteSkinQBoneRefInv (int nStart, size_t nSize);

        void WriteSkinTBoneRefInv (int nStart, size_t nSize);

        void WriteSkinUnkD (int nStart, size_t nSize);

        void WriteSkinBoneRef (int nStart, size_t nSize);

        void WriteSkinBoneWeight (int nStart, size_t nSize);

        void WriteAnimationEvents (int nStart, size_t nSize);

        void WriteFlareTextures (int nStart, size_t nSize);

        void WriteFlareData (int nIndex, int nStart, size_t nSize);

        void WriteGeometryHeader (CNwnMdlGeometryHeader *pData,
                                  const char *pszText, int nStart, size_t nSize);

        static void GetMaxAABBTreeOffset (CResModelRef *pFile, 
                                          int nOffset, int &nMaxOffset);

        // @cmember Generate an error

        void Format (const char *pszText, ...)
        {
            va_list marker;
            va_start (marker, pszText);
            ::_vsnprintf (m_szScratch, _countof (m_szScratch), 
                          pszText, marker);
            va_end (marker);
            m_szScratch [_countof (m_szScratch) - 1] = 0;
            m_pStream ->WriteLine (m_szScratch, false);
        }

        // @cmember Insert a blank line

        void BlankLine ()
        {
            m_pStream ->WriteLine ("\r\n", false);
        }

        // @cmember Insert two blank lines

        void TwoBlankLines ()
        {
            m_pStream ->WriteLine ("\r\n\r\n", false);
        }

        // @access Protected variables
    protected:

        // @cmember Output stream

        CNwnStream                              *m_pStream;

        // @cmember Pointer to the file

        CResModelRef                    m_sRes;

        // @cmember Large scratch buffer

        char                                    m_szScratch [8000];
};

#endif // ETS_MODELRAWFORMATTER_H


/* vim: set sw=4: */
