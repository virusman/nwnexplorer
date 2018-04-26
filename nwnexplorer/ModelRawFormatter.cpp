//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ModelRawFormatter.cpp - Model raw formatter |
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
// $History: ModelRawFormatter.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "ModelRawFormatter.h"

//
// Debug NEW
//

#if defined (_DEBUG) && !defined (_WIN32_WCE)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//-----------------------------------------------------------------------------
//
// @mfunc <c CModelRawFormatter> constructor.
//
// @parm CData * | pFile | Model file
//
// @parm CNwnStream * | pStream | Output stream
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CModelRawFormatter::CModelRawFormatter (CData *pFile, CNwnStream *pStream)
{

    //
    // Initialize variables
    //

    m_sRes .Set (pFile);
    m_pStream = pStream;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CModelRawFormatter> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CModelRawFormatter::~CModelRawFormatter ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the max offset of an AABB tree entry
//
// @parm CResModelRef * | pFile | Pointer to the model data
//
// @parm int | nOffset | Offset of the AABB tree entry
//
// @parm int & | nMaxOffset | Max offset 
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::GetMaxAABBTreeOffset (CResModelRef *pFile, 
                                               int nOffset, int &nMaxOffset)
{

    //
    // If this isn't really an entry, return
    //

    if (nOffset == 0)
        return;

    //
    // Test for a new max
    //

    if (nOffset > nMaxOffset)
        nMaxOffset = nOffset;

    //
    // Do the children
    //

    NwnMdlAABBEntry *pNode = (NwnMdlAABBEntry *) pFile ->GetModelData (nOffset);
    GetMaxAABBTreeOffset (pFile, pNode ->psLeft .GetOffset (), nMaxOffset);
    GetMaxAABBTreeOffset (pFile, pNode ->psRight .GetOffset (), nMaxOffset);
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a block of unknown value to the stream
//
// @parm const UINT32 * | pulUnknowns | Pointer to the values
//
// @parm int | nCount | Number of unknowns
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteUnknowns (
    const UINT32 *pulUnknowns, int nCount)
{
    for (int i = 0; i < nCount; i++)
    {
        Format ("Unknown [%02d]          = %08X, %f\r\n", 
                i, pulUnknowns [i], ((float *) pulUnknowns) [i]);
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a block of unknown value to the stream
//
// @parm const UINT32 * | pulUnknowns | Pointer to the values
//
// @parm int | nCount | Number of unknowns
//
// @parm int | nIndex | Index of the unknowns for unknowns in blocks
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteUnknowns (
    const UINT32 *pulUnknowns, int nCount, int nIndex)
{
    for (int i = 0; i < nCount; i++)
    {
        Format ("Unknown [%02d] [%02d]     = %08X, %f\r\n", 
                nIndex, i, pulUnknowns [i], ((float *) pulUnknowns) [i]);
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a block of routine addresses
//
// @parm const UINT32 * | pulRoutines | Pointer to the routines
//
// @parm int | nCount | Number of routines
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteRoutines (
    const UINT32 *pulRoutines, int nCount)
{
    for (int i = 0; i < nCount; i++)
    {
        Format ("Routine [%02d]          = %08X\r\n", i, pulRoutines [i]);
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a block of data as hex values
//
// @parm int | nStart | Starting offset
//
// @parm size_t | nSize | Size of the block
//
// @parm int | nSkit | Number of bytes to skip
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteHex (int nStart, size_t nSize, int nSkip)
{

    //
    // Dump the binary data
    //

    const unsigned char *pauchStart = m_sRes .GetModelData ();
    const unsigned char *p = (const unsigned char *) pauchStart + nStart;
    const unsigned char *pauchBlockStart = p + nSkip;
    const unsigned char *pauchBlockEnd = pauchBlockStart + nSize - nSkip; 
    while (pauchBlockStart < pauchBlockEnd)
    {
        char szBuffer [80 + 3 + 9];

        //
        // Format the line of data
        //

        sprintf (szBuffer, "%08X %08X ", pauchBlockStart - pauchStart, 
                 pauchBlockStart - p);
        LPTSTR psz = &szBuffer [_tcslen (szBuffer)];
        for (int i = 0; i < 16; i++)
        {
            if (&pauchBlockStart [i] < pauchBlockEnd)
            {
                sprintf (psz, "%02X ", pauchBlockStart [i]);
                psz += 3;
            }
            else
            {
                *psz++ = ' ';
                *psz++ = ' ';
                *psz++ = ' ';
            }
        }
        for (int i = 0; i < 16; i++)
        {
            if (&pauchBlockStart [i] < pauchBlockEnd && 
                pauchBlockStart [i] >= ' ' && pauchBlockStart [i] < 127)
                *psz++ = (char) pauchBlockStart [i];
            else
                *psz++ = ' ';
        }
        *psz++ = 0;
        Format ("%s\r\n", szBuffer);
        pauchBlockStart += 16;
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a block of controllers keys
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteControllerKey (int nStart, size_t nSize)
{

    _ASSERTE ((nSize % sizeof (NwnMdlControllerKey)) == 0);

    Format ("Controller Key Data at %08X length %d\r\n\r\n", nStart, nSize);

    NwnMdlControllerKey *pController = (NwnMdlControllerKey *) m_sRes .GetModelData (nStart);
    for (int i = 0; i < nSize / sizeof (NwnMdlControllerKey); i++)
    {
        Format ("Controller Key [%02d]   = (Type = %d, Rows = %d, Columns = %d\r\n",
                i, pController [i] .lType, pController [i] .sRows, pController [i] .bColumns);

        Format ("                         KeyOffset = %d, DataOffset = %d)\r\n",
                pController [i] .sKeyOffset, pController [i] .sDataOffset);
    }

    TwoBlankLines ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a block of controller data
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteControllerData (int nStart, size_t nSize)
{

    _ASSERTE ((nSize % sizeof (UINT32)) == 0);

    Format ("Controller Data at %08X length %d\r\n\r\n", nStart, nSize);

    UINT32 *pulData = (UINT32 *) m_sRes .GetModelData (nStart);
    float *pfData = (float *) m_sRes .GetModelData (nStart);
    for (int i = 0; i < nSize / sizeof (UINT32); i++)
    {
        Format ("Controller Data [%02d]  = %08X, %f\r\n", 
                i, pulData [i], pfData [i]);
    }

    TwoBlankLines ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a block of face data
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteFaceData (int nStart, size_t nSize)
{
    _ASSERTE ((nSize % sizeof (NwnMdlFace)) == 0);

    Format ("Computed Face Data at %08X length %d\r\n\r\n", nStart, nSize);

    NwnMdlFace *pData = (NwnMdlFace *) m_sRes .GetModelData (nStart);
    for (int i = 0; i < nSize / sizeof (NwnMdlFace); i++)
    {
        Format ("Face %02d\r\n", i);

        Format ("  Plane               = N (%f, %f, %f) D (%f)\r\n", 
                pData [i] .vPlaneNormal .m_x, pData [i] .vPlaneNormal .m_y, 
                pData [i] .vPlaneNormal .m_z, pData [i] .fPlaneDistance);

        Format ("  Surface             = %d\r\n", 
                pData [i] .ulSurface);

        Format ("  Adjacent Faces      = (%d, %d, %d)\r\n", 
                pData [i] .asAdjFace [0], pData [i] .asAdjFace [1], 
                pData [i] .asAdjFace [2]);

        Format ("  Vertices           = (%d, %d, %d)\r\n", 
                pData [i] .ausVertex [0], pData [i] .ausVertex [1], 
                pData [i] .ausVertex [2]);
    }

    TwoBlankLines ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a block of face sets data
//
// @pstm int | nFaceSet | Face set number
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteFaceSet (int nFaceSet, int nStart, size_t nSize)
{
    Format ("Face Set %d at %08X length %d\r\n\r\n", nFaceSet, nStart, nSize);

    UINT16 *pusFaces = (UINT16 *) m_sRes .GetModelData (nStart);
    for (int i = 0; i < nSize / (sizeof (UINT16) * 3); i++)
    {
        Format ("Face [%02d]             = (%d, %d, %d)\r\n",
                i, pusFaces [0], pusFaces [1], pusFaces [2]);
        pusFaces += 3;
    }
    TwoBlankLines ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a block of vertices
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteVertices (int nStart, size_t nSize)
{
    Format ("Vertices at %08X length %d\r\n\r\n", nStart, nSize);

    float *pfVertices = (float *) m_sRes .GetModelData (nStart);
    for (int i = 0; i < nSize / (sizeof (float) * 3); i++)
    {
        Format ("Vertex [%02d]           = (%f, %f, %f)\r\n",
                i, pfVertices [0], pfVertices [1], pfVertices [2]);
        pfVertices += 3;
    }
    TwoBlankLines ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a block of bumpmap vectors
//
// @parm int | nFrame | Frame number (well, not really)
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteBumpmapVectors (int nFrame, int nStart, size_t nSize)
{
    Format ("Bumpmap %d Triples at %08X length %d\r\n\r\n", 
            nFrame, nStart, nSize);

    float *pfVertices = (float *) m_sRes .GetModelData (nStart);
    for (int i = 0; i < nSize / (sizeof (float) * 3); i++)
    {
        Format ("Vertex Triple [%02d]    = (%f, %f, %f)\r\n",
                i, pfVertices [0], pfVertices [1], pfVertices [2]);
        pfVertices += 3;
    }
    TwoBlankLines ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a block of texture coords
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteTextures (int nStart, size_t nSize)
{
    Format ("Texture Coords at %08X length %d\r\n\r\n", nStart, nSize);

    float *pfTextures = (float *) m_sRes .GetModelData (nStart);
    for (int i = 0; i < nSize / (sizeof (float) * 2); i++)
    {
        Format ("Texture Coord [%02d]    = (%f, %f)\r\n",
                i, pfTextures [0], pfTextures [1]);
        pfTextures += 2;
    }
    TwoBlankLines ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a block of normals
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteNormals (int nStart, size_t nSize)
{
    Format ("Normals at %08X length %d\r\n\r\n", nStart, nSize);

    float *pfNormals = (float *) m_sRes .GetModelData (nStart);
    for (int i = 0; i < nSize / (sizeof (float) * 3); i++)
    {
        Format ("Normal [%02d]           = (%f, %f, %f)\r\n",
                i, pfNormals [0], pfNormals [1], pfNormals [2]);
        pfNormals += 3;
    }
    TwoBlankLines ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a block of contraints
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteConstraints (int nStart, size_t nSize)
{
    Format ("Constraints at %08X length %d\r\n\r\n", nStart, nSize);

    float *pfConstraints = (float *) m_sRes .GetModelData (nStart);
    for (int i = 0; i < nSize / sizeof (float); i++)
    {
        Format ("Constraint [%02d]       = %f\r\n",
                i, pfConstraints [0]);
        pfConstraints += 1;
    }
    TwoBlankLines ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a block of skin node to bone maps
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteSkinUnkA (int nStart, size_t nSize)
{
    Format ("Skin Node To Bone Map at %08X length %d\r\n\r\n", nStart, nSize);

    INT16 *psData = (INT16 *) m_sRes .GetModelData (nStart);
    for (int i = 0; i < nSize / sizeof (UINT16) ; i++)
    {
        Format ("Sink N2B Map [%02d]     = %d\r\n",
                i, psData [0]);
        psData += 1;
    }
    TwoBlankLines ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a block of skin qbone ref inv something or another
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteSkinQBoneRefInv (int nStart, size_t nSize)
{
    Format ("Skin QBone Ref Inv at %08X length %d\r\n\r\n", nStart, nSize);

    CQuaternion *pqData = (CQuaternion *) m_sRes .GetModelData (nStart);
    for (int i = 0; i < nSize / sizeof (CQuaternion); i++)
    {
        Format ("QBoneRefInc [%02d]      = (%f, %f, %f, %f)\r\n",
                i, pqData ->m_x, pqData ->m_y, pqData ->m_z, pqData ->m_w);
        pqData++;
    }
    TwoBlankLines ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a block of skin tbone ref inv something or another
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteSkinTBoneRefInv (int nStart, size_t nSize)
{
    Format ("Skin TBone Ref Inv at %08X length %d\r\n\r\n", nStart, nSize);

    CVector3 *pvData = (CVector3 *) m_sRes .GetModelData (nStart);
    for (int i = 0; i < nSize / sizeof (CVector3); i++)
    {
        Format ("TBoneRefInc [%02d]      = (%f, %f, %f)\r\n",
                i, pvData ->m_x, pvData ->m_y, pvData ->m_z);
        pvData++;
    }
    TwoBlankLines ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a block of unknown skin data
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteSkinUnkD (int nStart, size_t nSize)
{
    Format ("Skin Unknown Data D at %08X length %d\r\n\r\n", nStart, nSize);

    UINT32 *pulData = (UINT32 *) m_sRes .GetModelData (nStart);
    for (int i = 0; i < nSize / (sizeof (UINT32) * 1) ; i++)
    {
        WriteUnknowns (pulData, 1, i);
        pulData += 1;
    }
    TwoBlankLines ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a block of colors
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteColors (int nStart, size_t nSize)
{
    Format ("Colors at %08X length %d\r\n\r\n", nStart, nSize);

    UINT32 *pulAlphas = (UINT32 *) m_sRes .GetModelData (nStart);
    for (int i = 0; i < nSize / sizeof (UINT32); i++)
    {
        Format ("Color [%02d]            = %08X\r\n",
                i, pulAlphas [0]);
        pulAlphas += 1;
    }
    TwoBlankLines ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a block of bumpmap floats
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteBumpmapFloats (int nStart, size_t nSize)
{
    Format ("Bumpmap floats at %08X length %d\r\n\r\n", nStart, nSize);

    WriteUnknowns ((UINT32 *) m_sRes .GetModelData (nStart), 
                   (int) nSize / sizeof (UINT32));
    TwoBlankLines ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a block of skin bone weights
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteSkinBoneWeight (int nStart, size_t nSize)
{
    Format ("Skin Weights at %08X length %d\r\n\r\n", nStart, nSize);

    float *pfData = (float *) m_sRes .GetModelData (nStart);
    for (int i = 0; i < nSize / (sizeof (float) * 4); i++)
    {
        Format ("Weights [%02d]          = (%f, %f, %f, %f)\r\n",
                i, pfData [0], pfData [1], pfData [2], pfData [3]);
        pfData += 4;
    }
    TwoBlankLines ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a block of skin bone references
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteSkinBoneRef (int nStart, size_t nSize)
{
    Format ("Skin Bone Refs at %08X length %d\r\n\r\n", nStart, nSize);

    INT16 *psData = (INT16 *) m_sRes .GetModelData (nStart);
    for (int i = 0; i < nSize / (sizeof (UINT16) * 4); i++)
    {
        Format ("Bone Refs [%02d]        = (%2d, %2d, %2d, %2d)\r\n",
                i, psData [0], psData [1], psData [2], psData [3]);
        psData += 4;
    }
    TwoBlankLines ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a block animation events
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteAnimationEvents (int nStart, size_t nSize)
{
    Format ("Animation Events at %08X length %d\r\n\r\n", nStart, nSize);

    NwnMdlAnimationEvent *pAE = (NwnMdlAnimationEvent *)
        m_sRes .GetModelData (nStart);
    for (int i = 0; i < nSize / sizeof (*pAE); i++)
    {
        Format ("Event [%02d]            = \"%s\" after = %f\r\n", 
                i, pAE [i] .szEventName, pAE [i] .fAfter);
    }
    TwoBlankLines ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a block of flare textures
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteFlareTextures (int nStart, size_t nSize)
{
    Format ("Flare Textures at %08X length %d\r\n\r\n", nStart, nSize);

    UINT32 *pulOffsets = (UINT32 *) m_sRes .GetModelData (nStart);
    for (int i = 0; i < nSize / sizeof (UINT32); i++)
    {
        char *pszText = (char *) m_sRes .GetModelData (pulOffsets [i]);
        Format ("Flare Texture  [%02d]   = %s\r\n", i, pszText);
    }
    TwoBlankLines ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a block of flare data
//
// @parm int | nIndex | Index
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteFlareData (int nIndex, int nStart, size_t nSize)
{
    Format ("Flare data %d at %08X length %d\r\n\r\n", nIndex, nStart, nSize);

    WriteUnknowns ((UINT32 *) m_sRes .GetModelData (nStart), (
            int) nSize / sizeof (UINT32));
    TwoBlankLines ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Write an AABB tree
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteAABBTree (int nStart, size_t nSize)
{
    Format ("AABBTree at %08X length %d\r\n\r\n", nStart, nSize);

    NwnMdlAABBEntry *pasNodes = 
        (NwnMdlAABBEntry *) m_sRes .GetModelData (nStart);
    for (int i = 0; i < nSize / sizeof (NwnMdlAABBEntry); i++)
    {

        Format ("  -- Offset %08X --\r\n", nStart + i * sizeof (NwnMdlAABBEntry));

        Format ("Min                   = (%f, %f, %f)\r\n",
                pasNodes ->vBMin .m_x, pasNodes ->vBMin .m_y, pasNodes ->vBMin .m_z);

        Format ("Max                   = (%f, %f, %f)\r\n",
                pasNodes ->vBMax .m_x, pasNodes ->vBMax .m_y, pasNodes ->vBMax .m_z);

        Format ("Left Offset           = %08X\r\n", pasNodes ->psLeft .GetOffset ());

        Format ("Right Offset          = %08X\r\n", pasNodes ->psRight .GetOffset ());

        Format ("Leaf Face             = %d\r\n", pasNodes ->ulLeafFace);

        Format ("Significant Plane     = %d\r\n", pasNodes ->ulPlane);

        BlankLine ();

        pasNodes += 1;
    }
    TwoBlankLines ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a node header
//
// @parm CNwnMdlNodeHeader * | pNode | Pointer to the node
//
// @parm LPCSTR | pszName | Name of the node
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteNodeHeader (CNwnMdlNodeHeader *pNode, 
                                          LPCSTR pszName, int nStart, size_t nSize)
{
    Format ("%s at %08X length %d\r\n\r\n", pszName, nStart, nSize);

    WriteRoutines (pNode ->m_aulNodeRoutines, _countof (pNode ->m_aulNodeRoutines));

    Format ("Inherit Color         = %d\r\n", pNode ->m_ulInheritColor);

    Format ("Part Number           = %d\r\n", pNode ->m_nPartNumber);

    Format ("Name                  = %s\r\n", pNode ->m_szName);

    Format ("Geometry Hdr. Offset  = %d\r\n", pNode ->m_pGeometryHeader .GetOffset ());

    Format ("Parent Node Offset    = %d\r\n", pNode ->m_pParentNode .GetOffset ());

    Format ("Children Offset       = %08X\r\n", pNode ->m_apChildren .GetOffset ());

    Format ("Children Count        = %d\r\n", pNode ->m_apChildren .GetCount ());

    Format ("Children Alloc        = %d\r\n", pNode ->m_apChildren .GetAllocated ());

    Format ("Controller Key Offset = %08X\r\n", pNode ->m_aControllerKey .GetOffset ());

    Format ("Controller Key Count  = %d\r\n", pNode ->m_aControllerKey .GetCount ());

    Format ("Controller Key Alloc  = %d\r\n", pNode ->m_aControllerKey .GetAllocated ());

    Format ("Controller Data Offset= %08X\r\n", pNode ->m_aControllerData .GetOffset ());

    Format ("Controller Data Count = %d\r\n", pNode ->m_aControllerData .GetCount ());

    Format ("Controller Data Alloc = %d\r\n", pNode ->m_aControllerData .GetAllocated ());

    Format ("Flags/Type            = %08X\r\n", pNode ->m_ulFlags);
}

//-----------------------------------------------------------------------------
//
// @mfunc Write the controller data
//
// @parm CNwnMdlNodeHeader * | pNode | Pointer to the node
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteNodeControllerData (CNwnMdlNodeHeader *pNode)
{
    if (pNode ->m_aControllerKey .GetCount () != 0)
    {
        WriteControllerKey (pNode ->m_aControllerKey .GetOffset (), 
                            pNode ->m_aControllerKey .GetCount () * 
                            pNode ->m_aControllerKey .GetElementSize ());
    }

    if (pNode ->m_aControllerData .GetCount () != 0)
    {
        WriteControllerData (pNode ->m_aControllerData .GetOffset (), 
                             pNode ->m_aControllerData .GetCount () * 
                             pNode ->m_aControllerData .GetElementSize ());
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a mesh header
//
// @parm CNwnMdlMeshHeader * | pMesh | Pointer to the mesh
//
// @parm LPCSTR | pszName | Name of the mesh
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteMeshHeader (CNwnMdlMeshHeader *pMesh, 
                                          LPCSTR pszName, int nStart, size_t nSize)
{
    WriteNodeHeader (pMesh, pszName, nStart, nSize);

    WriteRoutines (pMesh ->m_aulMeshRoutines, _countof (pMesh ->m_aulMeshRoutines));

    Format ("Face Offset           = %08X\r\n", pMesh ->m_sFaces .GetOffset ());

    Format ("Face Count            = %d\r\n", pMesh ->m_sFaces .GetCount ());

    Format ("Face Alloc            = %d\r\n", pMesh ->m_sFaces .GetAllocated ());

    Format ("Bounding Box Min      = (%f, %f, %f)\r\n", 
            pMesh ->m_vBMin .m_x, pMesh ->m_vBMin .m_y, pMesh ->m_vBMin .m_z);

    Format ("Bounding Box Max      = (%f, %f, %f)\r\n", 
            pMesh ->m_vBMax .m_x, pMesh ->m_vBMax .m_y, pMesh ->m_vBMax .m_z);

    Format ("Radius                = %f\r\n", pMesh ->m_fRadius);

    Format ("Bounding Box Average  = (%f, %f, %f)\r\n", 
            pMesh ->m_vBAverage .m_x, pMesh ->m_vBAverage .m_y, pMesh ->m_vBAverage .m_z);

    Format ("Diffuse               = (%f, %f, %f)\r\n", 
            pMesh ->m_vDiffuse .m_x, pMesh ->m_vDiffuse .m_y, pMesh ->m_vDiffuse .m_z);

    Format ("Ambient               = (%f, %f, %f)\r\n", 
            pMesh ->m_vAmbient .m_x, pMesh ->m_vAmbient .m_y, pMesh ->m_vAmbient .m_z);

    Format ("Specular              = (%f, %f, %f)\r\n", 
            pMesh ->m_vSpecular .m_x, pMesh ->m_vSpecular .m_y, pMesh ->m_vSpecular .m_z);

    Format ("Shininess             = %f\r\n", pMesh ->m_fShininess);

    Format ("Shadow                = %d\r\n", pMesh ->m_ulShadow);

    Format ("Beaming               = %d\r\n", pMesh ->m_ulBeaming);

    Format ("Render                = %d\r\n", pMesh ->m_ulRender);

    Format ("Transparency Hint     = %d\r\n", pMesh ->m_ulTransparencyHint);

    Format ("Render Hint           = %d\r\n", pMesh ->m_ulRenderHint);

    Format ("Texture 0/Bitmap      = %s\r\n", pMesh ->m_szTextures [0]);

    Format ("Texture 1             = %s\r\n", pMesh ->m_szTextures [1]);

    Format ("Texture 2             = %s\r\n", pMesh ->m_szTextures [2]);

    Format ("Material Name         = %s\r\n", pMesh ->m_szMaterialName);

    Format ("Tile Fade             = %d\r\n", pMesh ->m_ulTileFade);

    Format ("Vertex Indices Offset = %08X\r\n", pMesh ->m_sVertexIndices .GetOffset ());

    Format ("Vertex Indices Count  = %d\r\n", pMesh ->m_sVertexIndices .GetCount ());

    Format ("Vertex Indices Alloc  = %d\r\n", pMesh ->m_sVertexIndices .GetAllocated ());

    Format ("Left Over Faces Offset= %08X\r\n", pMesh ->m_sLeftOverFaces .GetOffset ());

    Format ("Left Over Faces Count = %d\r\n", pMesh ->m_sLeftOverFaces .GetCount ());

    Format ("Left Over Faces Alloc = %d\r\n", pMesh ->m_sLeftOverFaces .GetAllocated ());

    Format ("Vertex Ind.Cnt Offset = %08X\r\n", pMesh ->m_sVertexIndicesCount .GetOffset ());

    Format ("Vertex Ind.Cnt Count  = %d\r\n", pMesh ->m_sVertexIndicesCount .GetCount ());

    Format ("Vertex Ind.Cnt Alloc  = %d\r\n", pMesh ->m_sVertexIndicesCount .GetAllocated ());

    Format ("Vertex Ind.Off Offset = %08X\r\n", pMesh ->m_sRawVertexIndices .GetOffset ());

    Format ("Vertex Ind.Off Count  = %d\r\n", pMesh ->m_sRawVertexIndices .GetCount ());

    Format ("Vertex Ind.Off Alloc  = %d\r\n", pMesh ->m_sRawVertexIndices .GetAllocated ());

    Format ("Something 3 Offset    = %08X\r\n", pMesh ->m_ulSomething3Offset);

    Format ("Something 3 Count     = %08X\r\n", pMesh ->m_ulSomething3Count);

    Format ("Triangle Mode         = %d\r\n", pMesh ->m_ucTriangleMode);

    Format ("Temp Mesh Data        = %08X\r\n", pMesh ->m_pvTempMeshData);

    Format ("Vertex Offset         = %08X\r\n", pMesh ->m_pavVerts .GetOffset ());

    Format ("Vertex Count          = %d\r\n", pMesh ->m_usVertexCount);

    Format ("Texture Count         = %d\r\n", pMesh ->m_usTextureCount);

    Format ("Texture Offset        = %08X\r\n", pMesh ->m_pavTVerts [0] .GetOffset ());

    Format ("Texture 1 Offset      = %08X\r\n", pMesh ->m_pavTVerts [1] .GetOffset ());

    Format ("Texture 2 Offset      = %08X\r\n", pMesh ->m_pavTVerts [2] .GetOffset ());

    Format ("Texture 3 Offset      = %08X\r\n", pMesh ->m_pavTVerts [3] .GetOffset ());

    Format ("Normal Offset         = %08X\r\n", pMesh ->m_pavNormals .GetOffset ());

    Format ("Color Offset          = %08X\r\n", pMesh ->m_paColors .GetOffset ());

    Format ("Bumpmap Data 1 Off.   = %08X\r\n", pMesh ->m_pavBumpmapAnim1 .GetOffset ());

    Format ("Bumpmap Data 2 Off.   = %08X\r\n", pMesh ->m_pavBumpmapAnim2 .GetOffset ());

    Format ("Bumpmap Data 3 Off.   = %08X\r\n", pMesh ->m_pavBumpmapAnim3 .GetOffset ());

    Format ("Bumpmap Data 4 Off.   = %08X\r\n", pMesh ->m_pavBumpmapAnim4 .GetOffset ());

    Format ("Bumpmap Data 5 Off.   = %08X\r\n", pMesh ->m_pavBumpmapAnim5 .GetOffset ());

    Format ("Bumpmap Data 6 Off.   = %08X\r\n", pMesh ->m_pafBumpmapAnim6 .GetOffset ());

    Format ("Light Mapped          = %d\r\n", pMesh ->m_ucLightMapped);

    Format ("Rotate Texture        = %d\r\n", pMesh ->m_ucRotateTexture);

    Format ("Face Normal Sum Div 2 = %f\r\n", pMesh ->m_fFaceNormalSumDiv2);

    Format ("Unknown               = %d\r\n", pMesh ->m_ulUnknown1);
}

//-----------------------------------------------------------------------------
//
// @mfunc Write mesh headers
//
// @parm CNwnMdlMeshHeader * | pMesh | Pointer to the mesh
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteMeshFaces (CNwnMdlMeshHeader *pMesh)
{
    if (pMesh ->m_sFaces .GetCount () != 0)
    {
        WriteFaceData (pMesh ->m_sFaces .GetOffset (), 
                       pMesh ->m_sFaces .GetCount () * 
                       pMesh ->m_sFaces .GetElementSize ());
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Write mesh verticies
//
// @parm CNwnMdlMeshHeader * | pMesh | Pointer to the mesh
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteMeshVertices (CNwnMdlMeshHeader *pMesh)
{
    if (pMesh ->m_usVertexCount > 0)
    {
        if (pMesh ->m_pavVerts .GetOffset () != 0xFFFFFFFF)
        {
            WriteVertices (
                m_sRes .GetRawDataOffset () + 
                pMesh ->m_pavVerts .GetOffset (), 
                pMesh ->m_usVertexCount * sizeof (float) * 3);
        }

        for (int i = 0; i < 4; i++)
        {
            if (pMesh ->m_pavTVerts [i] .GetOffset () != 0xFFFFFFFF)
            {
                WriteTextures (
                    m_sRes .GetRawDataOffset () + 
                    pMesh ->m_pavTVerts [i] .GetOffset (), 
                    pMesh ->m_usVertexCount * sizeof (float) * 2);
            }
        }

        if (pMesh ->m_pavNormals .GetOffset () != 0xFFFFFFFF)
        {
            WriteNormals (
                m_sRes .GetRawDataOffset () + 
                pMesh ->m_pavNormals .GetOffset (), 
                pMesh ->m_usVertexCount * sizeof (float) * 3);
        }

        if (pMesh ->m_paColors .GetOffset () != 0xFFFFFFFF)
        {
            WriteColors (
                m_sRes .GetRawDataOffset () + 
                pMesh ->m_paColors .GetOffset (), 
                pMesh ->m_usVertexCount * sizeof (UINT32));
        }

        if (pMesh ->m_pavBumpmapAnim1 .GetOffset () != 0xFFFFFFFF)
        {
            WriteBumpmapVectors (1, 
                                 m_sRes .GetRawDataOffset () + 
                                 pMesh ->m_pavBumpmapAnim1 .GetOffset (), 
                                 pMesh ->m_usVertexCount * sizeof (CVector3));
        }

        if (pMesh ->m_pavBumpmapAnim2 .GetOffset () != 0xFFFFFFFF)
        {
            WriteBumpmapVectors (2, 
                                 m_sRes .GetRawDataOffset () + 
                                 pMesh ->m_pavBumpmapAnim2 .GetOffset (), 
                                 pMesh ->m_usVertexCount * sizeof (CVector3));
        }

        if (pMesh ->m_pavBumpmapAnim3 .GetOffset () != 0xFFFFFFFF)
        {
            WriteBumpmapVectors (3, 
                                 m_sRes .GetRawDataOffset () + 
                                 pMesh ->m_pavBumpmapAnim3 .GetOffset (), 
                                 pMesh ->m_usVertexCount * sizeof (CVector3));
        }

        if (pMesh ->m_pavBumpmapAnim4 .GetOffset () != 0xFFFFFFFF)
        {
            WriteBumpmapVectors (4, 
                                 m_sRes .GetRawDataOffset () + 
                                 pMesh ->m_pavBumpmapAnim4 .GetOffset (), 
                                 pMesh ->m_usVertexCount * sizeof (CVector3));
        }

        if (pMesh ->m_pavBumpmapAnim5 .GetOffset () != 0xFFFFFFFF)
        {
            WriteBumpmapVectors (5, 
                                 m_sRes .GetRawDataOffset () + 
                                 pMesh ->m_pavBumpmapAnim5 .GetOffset (), 
                                 pMesh ->m_usVertexCount * sizeof (CVector3));
        }

        if (pMesh ->m_pafBumpmapAnim6 .GetOffset () != 0xFFFFFFFF)
        {
            WriteBumpmapFloats (
                m_sRes .GetRawDataOffset () + 
                pMesh ->m_pafBumpmapAnim6 .GetOffset (), 
                pMesh ->m_usVertexCount * sizeof (float));
        }
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Write mesh face sets
//
// @parm CNwnMdlMeshHeader * | pMesh | Pointer to the mesh
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteMeshFaceSets (CNwnMdlMeshHeader *pMesh)
{
    if (pMesh ->m_sVertexIndicesCount .GetCount () != 0 ||
        pMesh ->m_sRawVertexIndices .GetCount () != 0)
    {
        _ASSERTE (
            pMesh ->m_sRawVertexIndices .GetCount () != 0 &&
            pMesh ->m_sVertexIndicesCount .GetCount () != 0);

        UINT32 *pulCounts = pMesh ->m_sVertexIndicesCount .
            GetData (m_sRes .GetModelData ());
        CNwnPointer <UINT16> *pulOffsets = pMesh ->m_sRawVertexIndices .
            GetData (m_sRes .GetModelData ());
        for (int i = 0; i < pMesh ->m_sRawVertexIndices .GetCount (); i++)
        {
            WriteFaceSet (i,
                          m_sRes .GetRawDataOffset () + pulOffsets [i] .GetOffset (), 
                          pulCounts [i] * sizeof (UINT16));
        }
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a tri mesh node
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteTriMeshNode (int nStart, size_t nSize)
{
    _ASSERTE (sizeof (CNwnMdlTriMeshNode) == nSize);

    CNwnMdlTriMeshNode *pData = (CNwnMdlTriMeshNode *)
        m_sRes .GetModelData (nStart);

    WriteMeshHeader (pData, "Node, TriMesh", nStart, nSize);

    TwoBlankLines ();

    WriteMeshVertices (pData);

    WriteMeshFaces (pData);

    WriteMeshFaceSets (pData);

    WriteNodeControllerData (pData);
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a skin node
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteSkinNode (int nStart, size_t nSize)
{
    _ASSERTE (sizeof (CNwnMdlSkinMeshNode) == nSize);

    CNwnMdlSkinMeshNode *pData =        (CNwnMdlSkinMeshNode *)
        m_sRes .GetModelData (nStart);

    WriteMeshHeader (pData, "Node, Skin", nStart, nSize);

    Format ("Weights Offset        = %08X\r\n", pData ->m_aWeights .GetOffset ());

    Format ("Weights Count         = %d\r\n", pData ->m_aWeights .GetCount ());

    Format ("Weights Alloc         = %d\r\n", pData ->m_aWeights .GetAllocated ());

    Format ("Skin Weights Offset   = %08X\r\n", pData ->m_pafSkinWeights .GetOffset ());

    Format ("Skin Bones Refs Offset= %08X\r\n", pData ->m_pasSkinBoneRefs .GetOffset ());

    Format ("Skin Node to Bone Off = %08X\r\n", pData ->m_pasNodeToBoneMap .GetOffset ());

    Format ("Skin Node to Bone Cnt = %d\r\n", pData ->m_ulNodeToBoneCount);

    Format ("QBone Ref Inv Offset  = %08X\r\n", pData ->m_aQBoneRefInv .GetOffset ());

    Format ("QBone Ref Inv Count   = %d\r\n", pData ->m_aQBoneRefInv .GetCount ());

    Format ("QBone Ref Inv Alloc   = %d\r\n", pData ->m_aQBoneRefInv .GetAllocated ());

    Format ("TBone Ref Inv Offset  = %08X\r\n", pData ->m_aTBoneRefInv .GetOffset ());

    Format ("TBone Ref Inv Count   = %d\r\n", pData ->m_aTBoneRefInv .GetCount ());

    Format ("TBone Ref Inv Alloc   = %d\r\n", pData ->m_aTBoneRefInv .GetAllocated ());

    Format ("Bone Const Ind Offset = %08X\r\n", pData ->m_aBoneConstantIndices .GetOffset ());

    Format ("Bone Const Ind Count  = %d\r\n", pData ->m_aBoneConstantIndices .GetCount ());

    Format ("Bone Const Ind Alloc  = %d\r\n", pData ->m_aBoneConstantIndices .GetAllocated ());

    for (int i = 0; i < _countof (pData ->m_asBoneNodeNumbers); i++)
    {
        Format ("Bone Part Number [%2d] = %d\r\n", 
                i, pData ->m_asBoneNodeNumbers [i]);
    }

    TwoBlankLines ();

    //
    // Dump the skin data
    //

    if (pData ->m_ulNodeToBoneCount > 0)
    {
        WriteSkinUnkA (
            pData ->m_pasNodeToBoneMap .GetOffset (), 
            sizeof (UINT16) * pData ->m_ulNodeToBoneCount);
    }

    if (pData ->m_aQBoneRefInv .GetCount () > 0)
    {
        WriteSkinQBoneRefInv (
            pData ->m_aQBoneRefInv .GetOffset (), 
            sizeof (float) * 4 * pData ->m_aQBoneRefInv .GetCount ());
    }

    if (pData ->m_aTBoneRefInv .GetCount () > 0)
    {
        WriteSkinTBoneRefInv (
            pData ->m_aTBoneRefInv .GetOffset (), 
            sizeof (float) * 3 * pData ->m_aTBoneRefInv .GetCount ());
    }

    if (pData ->m_aBoneConstantIndices .GetCount () > 0)
    {
        WriteSkinUnkD (
            pData ->m_aBoneConstantIndices .GetOffset (), 
            sizeof (float) * pData ->m_aBoneConstantIndices .GetCount ());
    }


    WriteMeshVertices (pData);

    if (pData ->m_usVertexCount > 0)
    {
        if (pData ->m_pafSkinWeights .GetDataNZ (NULL) != NULL)
        {
            WriteSkinBoneWeight (
                m_sRes .GetRawDataOffset () + 
                pData ->m_pafSkinWeights .GetOffset (), 
                pData ->m_usVertexCount * sizeof (float) * 4);
        }
        if (pData ->m_pasSkinBoneRefs .GetDataNZ (NULL) != NULL)
        {
            WriteSkinBoneRef (
                m_sRes .GetRawDataOffset () + 
                pData ->m_pasSkinBoneRefs .GetOffset (), 
                pData ->m_usVertexCount * sizeof (UINT16) * 4);
        }
    }

    WriteMeshFaces (pData);

    WriteMeshFaceSets (pData);

    WriteNodeControllerData (pData);
}

//-----------------------------------------------------------------------------
//
// @mfunc Write an animation mesh node
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteAnimMeshNode (int nStart, size_t nSize)
{
    _ASSERTE (sizeof (CNwnMdlAnimMeshNode) == nSize);

    CNwnMdlAnimMeshNode *pData = (CNwnMdlAnimMeshNode *)
        m_sRes .GetModelData (nStart);

    WriteMeshHeader (pData, "Node, AnimMesh", nStart, nSize);

    Format ("Sample Period         = %f\r\n", pData ->m_fSamplePeriod);

    Format ("Anim Verts Offset     = %08X\r\n", pData ->m_avAnimVerts .GetOffset ());

    Format ("Anim Verts Count      = %d\r\n", pData ->m_avAnimVerts .GetCount ());

    Format ("Anim Verts Alloc      = %d\r\n", pData ->m_avAnimVerts .GetAllocated ());

    Format ("Anim TVerts Offset    = %08X\r\n", pData ->m_avAnimTVerts .GetOffset ());

    Format ("Anim TVerts Count     = %d\r\n", pData ->m_avAnimTVerts .GetCount ());

    Format ("Anim TVerts Alloc     = %d\r\n", pData ->m_avAnimTVerts .GetAllocated ());

    Format ("Anim Normals Offset   = %08X\r\n", pData ->m_avAnimNormals .GetOffset ());

    Format ("Anim Normals Count    = %d\r\n", pData ->m_avAnimNormals .GetCount ());

    Format ("Anim Normals Alloc    = %d\r\n", pData ->m_avAnimNormals .GetAllocated ());

    Format ("Vertex Sets Offset    = %08X\r\n", pData ->m_pvVertexSets .GetOffset ());

    Format ("Texture Sets Offset   = %08X\r\n", pData ->m_pvTextureSets .GetOffset ());

    Format ("Vertex Sets Count     = %d\r\n", pData ->m_ulVertexSetCount);

    Format ("Texture Sets Count    = %d\r\n", pData ->m_ulTextureSetCount);

    TwoBlankLines ();

    WriteMeshVertices (pData);

    //
    // Dump the extra vertex and texture sets
    //

    if (pData ->m_ulVertexSetCount > 0)
    {
        size_t nSize = pData ->m_pvVertexSets .GetElementSize () * pData ->m_usVertexCount;
        for (int i = 0; i < pData ->m_ulVertexSetCount; i++)
        {
            WriteVertices (pData ->m_pvVertexSets .GetOffset () + i * nSize, nSize);
        }
    }
    if (pData ->m_ulTextureSetCount > 0)
    {
        size_t nSize = pData ->m_pvTextureSets .GetElementSize () * pData ->m_usVertexCount;
        for (int i = 0; i < pData ->m_ulTextureSetCount; i++)
        {
            WriteTextures (pData ->m_pvTextureSets .GetOffset () + i * nSize, nSize);
        }
    }

    WriteMeshFaces (pData);

    WriteMeshFaceSets (pData);

    WriteNodeControllerData (pData);
}

//-----------------------------------------------------------------------------
//
// @mfunc Write an AABB node
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteAABBNode (int nStart, size_t nSize)
{
    _ASSERTE (sizeof (CNwnMdlAABBMeshNode) == nSize);

    CNwnMdlAABBMeshNode *pData = (CNwnMdlAABBMeshNode *)
        m_sRes .GetModelData (nStart);

    WriteMeshHeader (pData, "Node, AABB", nStart, nSize);

    Format ("AABB Tree Offset      = %08X\r\n", pData ->m_pAABBTree .GetOffset ());

    TwoBlankLines ();

    //
    // If we have an AABB tree
    //

    if (pData ->m_pAABBTree .GetOffset () > 0)
    {
        int nMaxOffset = 0;
        GetMaxAABBTreeOffset (&m_sRes, 
                              pData ->m_pAABBTree .GetOffset (), nMaxOffset);
        nMaxOffset += sizeof (NwnMdlAABBEntry);
        WriteAABBTree (
            pData ->m_pAABBTree .GetOffset (), 
            nMaxOffset - pData ->m_pAABBTree .GetOffset ());
    }

    //
    // Write the other general values
    //

    WriteMeshVertices (pData);

    WriteMeshFaces (pData);

    WriteMeshFaceSets (pData);

    WriteNodeControllerData (pData);
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a dangly mesh node
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteDanglyMeshNode (int nStart, size_t nSize)
{
    _ASSERTE (sizeof (CNwnMdlDanglyMeshNode) == nSize);

    CNwnMdlDanglyMeshNode *pData = (CNwnMdlDanglyMeshNode *)
        m_sRes .GetModelData (nStart);

    WriteMeshHeader (pData, "Node, DanglyMesh", nStart, nSize);

    Format ("Constraints Offset    = %08X\r\n", pData ->m_afConstraints .GetOffset ());

    Format ("Constraints Count     = %d\r\n", pData ->m_afConstraints .GetCount ());

    Format ("Constraints Alloc     = %d\r\n", pData ->m_afConstraints .GetAllocated ());

    Format ("Displacement          = %f\r\n", pData ->m_fDisplacement);

    Format ("Tightness             = %f\r\n", pData ->m_fTightness);

    Format ("Period                = %f\r\n", pData ->m_fPeriod);

    TwoBlankLines ();

    WriteMeshVertices (pData);

    WriteMeshFaces (pData);

    WriteMeshFaceSets (pData);

    WriteConstraints (pData ->m_afConstraints .GetOffset (),
                      pData ->m_afConstraints .GetElementSize () * 
                      pData ->m_afConstraints .GetCount ());

    WriteNodeControllerData (pData);
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a dummy node
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteDummyNode (int nStart, size_t nSize)
{
    _ASSERTE (sizeof (CNwnMdlDummyNode) == nSize);

    CNwnMdlDummyNode *pData = (CNwnMdlDummyNode *)
        m_sRes .GetModelData (nStart);

    WriteNodeHeader (pData, "Node, Dummy", nStart, nSize);

    TwoBlankLines ();

    WriteNodeControllerData (pData);
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a reference node
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteReferenceNode (int nStart, size_t nSize)
{
    _ASSERTE (sizeof (CNwnMdlReferenceNode) == nSize);

    CNwnMdlReferenceNode *pData = (CNwnMdlReferenceNode *)
        m_sRes .GetModelData (nStart);

    WriteNodeHeader (pData, "Node, Reference", nStart, nSize);

    Format ("Ref Model             = %s\r\n", pData ->m_szRefModel);

    Format ("Reattachable          = %d\r\n", pData ->m_ulReattachable);

    TwoBlankLines ();

    WriteNodeControllerData (pData);
}

//-----------------------------------------------------------------------------
//
// @mfunc Write an emitter node
//
// @parm bool | fIsAnim | If true, this is part of the animation 
//              tree.
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteEmitterNode (bool fIsAnim, int nStart, size_t nSize)
{
    _ASSERTE (sizeof (CNwnMdlEmitterNode) == nSize);

    CNwnMdlEmitterNode *pData = (CNwnMdlEmitterNode *)
        m_sRes .GetModelData (nStart);

    WriteNodeHeader (pData, "Node, Emitter", nStart, nSize);

    if (!fIsAnim)
    {
        Format ("Blast Radius          = %f\r\n", pData ->m_fBlastRadius);

        Format ("Blast Length          = %f\r\n", pData ->m_fBlastLength);

        Format ("X-Grid                = %d\r\n", pData ->m_ulXGrid);

        Format ("Y-Grid                = %d\r\n", pData ->m_ulYGrid);

        Format ("Spawn Type            = %d\r\n", pData ->m_ulSpawnType);

        Format ("Update                = %s\r\n", pData ->m_szUpdate);

        Format ("Render                = %s\r\n", pData ->m_szRender);

        Format ("Blend                 = %s\r\n", pData ->m_szBlend);

        Format ("Texture               = %s\r\n", pData ->m_szTexture);

        Format ("Chunk Name            = %s\r\n", pData ->m_szChunkName);

        Format ("Two Sided Tex         = %d\r\n", pData ->m_ulTwoSidedTex);

        Format ("Loop                  = %d\r\n", pData ->m_ulLoop);

        Format ("Render Order          = %d\r\n", pData ->m_usRenderOrder);

        Format ("Emitter Flags         = %04X\r\n", pData ->m_ulEmitterFlags);
    }
    else
    {
        Format ("                        Remainder unused in animations.");
    }

    TwoBlankLines ();

    WriteNodeControllerData (pData);
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a light node
//
// @parm bool | fIsAnim | If true, this is part of the animation 
//              tree.
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteLightNode (bool fIsAnim, int nStart, size_t nSize)
{
    _ASSERTE (sizeof (CNwnMdlLightNode) == nSize);

    CNwnMdlLightNode *pData = (CNwnMdlLightNode *)
        m_sRes .GetModelData (nStart);

    WriteNodeHeader (pData, "Node, Light", nStart, nSize);

    Format ("Flare Radius          = %f\r\n", pData ->m_fFlareRadius);

    Format ("Something 1 Offset    = %08X\r\n", pData ->m_aSomething1 .GetOffset ());

    Format ("Something 1 Count     = %d\r\n", pData ->m_aSomething1 .GetCount ());

    Format ("Something 1 Alloc     = %d\r\n", pData ->m_aSomething1 .GetAllocated ());

    Format ("Flare Sizes Offset    = %08X\r\n", pData ->m_afFlareSizes .GetOffset ());

    Format ("Flare Sizes Count     = %d\r\n", pData ->m_afFlareSizes .GetCount ());

    Format ("Flare Sizes Alloc     = %d\r\n", pData ->m_afFlareSizes .GetAllocated ());

    Format ("Flare Pos. Offset     = %08X\r\n", pData ->m_afFlarePositions .GetOffset ());

    Format ("Flare Pos. Count      = %d\r\n", pData ->m_afFlarePositions .GetCount ());

    Format ("Flare Pos. Alloc      = %d\r\n", pData ->m_afFlarePositions .GetAllocated ());

    Format ("Flare ClrSft Offset   = %08X\r\n", pData ->m_avFlareColorShifts .GetOffset ());

    Format ("Flare ClrSft Count    = %d\r\n", pData ->m_avFlareColorShifts .GetCount ());

    Format ("Flare ClrSft Alloc    = %d\r\n", pData ->m_avFlareColorShifts .GetAllocated ());

    Format ("Texture Names Offset  = %08X\r\n", pData ->m_apszTextureNames .GetOffset ());

    Format ("Texture Names Count   = %d\r\n", pData ->m_apszTextureNames .GetCount ());

    Format ("Texture Names Alloc   = %d\r\n", pData ->m_apszTextureNames .GetAllocated ());

    Format ("Light Priority        = %d\r\n", pData ->m_ulLightPriority);

    Format ("Ambient Only          = %d\r\n", pData ->m_ulAmbientOnly);

    Format ("Dynamic Type          = %d\r\n", pData ->m_ulDynamicType);

    Format ("Affect Dynamic        = %d\r\n", pData ->m_ulAffectDynamic);

    Format ("Shadow                = %d\r\n", pData ->m_ulShadow);

    Format ("Generate Flare        = %d\r\n", pData ->m_ulGenerateFlare);

    Format ("Fading Light          = %d\r\n", pData ->m_ulFadingLight);

    TwoBlankLines ();

    if (pData ->m_apszTextureNames .GetCount () > 0)
    {
        WriteFlareTextures (pData ->m_apszTextureNames .GetOffset (), 
                            pData ->m_apszTextureNames .GetCount () * sizeof (UINT32));
    }

    if (pData ->m_afFlareSizes .GetCount () > 0)
    {
        WriteFlareData (1, pData ->m_afFlareSizes .GetOffset (), 
                        pData ->m_afFlareSizes .GetCount () * sizeof (UINT32) * 1);
    }

    if (pData ->m_afFlarePositions .GetCount () > 0)
    {
        WriteFlareData (2, pData ->m_afFlarePositions .GetOffset (), 
                        pData ->m_afFlarePositions .GetCount () * sizeof (UINT32) * 1);
    }

    if (pData ->m_avFlareColorShifts .GetCount () > 0)
    {
        WriteFlareData (3, pData ->m_avFlareColorShifts .GetOffset (), 
                        pData ->m_avFlareColorShifts .GetCount () * sizeof (UINT32) * 3);
    }
    WriteNodeControllerData (pData);
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a node header
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteNodeHeader (int nStart, size_t nSize)
{
    _ASSERTE (sizeof (CNwnMdlNodeHeader) == nSize);

    CNwnMdlNodeHeader *pData = (CNwnMdlNodeHeader *)
        m_sRes .GetModelData (nStart);

    WriteNodeHeader (pData, "Node, Unknown", nStart, nSize);

    TwoBlankLines ();

    WriteNodeControllerData (pData);
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a geometry header
//
// @parm CNwnMdlGeometryHeader * | pData | Pointer to the data
//
// @parm const char * | pszText | Text of the node
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteGeometryHeader (CNwnMdlGeometryHeader *pData,
                                              const char *pszText, int nStart, size_t nSize)
{
    Format ("%s at %08X length %d\r\n\r\n", pszText, nStart, nSize);

    WriteRoutines (pData ->m_aulGeomRoutines, _countof (pData ->m_aulGeomRoutines));

    Format ("Name                  = %s\r\n", pData ->m_szName);

    Format ("Geometry Root Offset  = %08X\r\n", pData ->m_pGeometry .GetOffset ());

    Format ("Geometry Node Count   = %08X\r\n", pData ->m_ulNodeCount);

    Format ("Runtime Array 1 Offset= %08X\r\n", pData ->m_aRtArray1 .GetOffset ());

    Format ("Runtime Array 1 Count = %d\r\n", pData ->m_aRtArray1 .GetCount ());

    Format ("Runtime Array 1 Alloc = %d\r\n", pData ->m_aRtArray1 .GetAllocated ());

    Format ("Runtime Array 2 Offset= %08X\r\n", pData ->m_aRtArray2 .GetOffset ());

    Format ("Runtime Array 2 Count = %d\r\n", pData ->m_aRtArray2 .GetCount ());

    Format ("Runtime Array 2 Alloc = %d\r\n", pData ->m_aRtArray2 .GetAllocated ());

    Format ("Unknown 2             = %08X\r\n", pData ->m_ulUnknown2);

    Format ("Geometry Type         = %d\r\n", pData ->m_ucGeometryType);
}

//-----------------------------------------------------------------------------
//
// @mfunc Write a model header
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteModelHeader (int nStart, size_t nSize)
{
    _ASSERTE (sizeof (CNwnMdlModel) == nSize);

    CNwnMdlModel *pData = (CNwnMdlModel *) 
        m_sRes .GetModelData (nStart);

    WriteGeometryHeader (pData, "Model Header", nStart, nSize);

    Format ("Unknown Flag 2        = %02X\r\n", pData ->m_aucFlags [0]);

    Format ("Unknown Flag 3        = %02X\r\n", pData ->m_aucFlags [1]);

    Format ("Classification        = %d\r\n", pData ->m_ucClassification);

    Format ("Fog                   = %d\r\n", pData ->m_ucFog);

    Format ("Reference Count       = %08X\r\n", pData ->m_ulRefCount);

    Format ("Animation Offset      = %08X\r\n", pData ->m_apAnimations .GetOffset ());

    Format ("Animation Count       = %d\r\n", pData ->m_apAnimations .GetCount ());

    Format ("Animation Alloc       = %d\r\n", pData ->m_apAnimations .GetAllocated ());

    Format ("Super Model Address   = %08X\r\n", pData ->m_pSuperModel .GetOffset ());

    Format ("BMin                  = (%f,%f,%f)\r\n", 
            pData ->m_vBMin .m_x, pData ->m_vBMin .m_y, pData ->m_vBMin .m_z);

    Format ("BMax                  = (%f,%f,%f)\r\n", 
            pData ->m_vBMax .m_x, pData ->m_vBMax .m_y, pData ->m_vBMax .m_z);

    Format ("Radius                = %f\r\n", pData ->m_fRadius);

    Format ("Animation Scale       = %f\r\n", pData ->m_fAnimationScale);

    Format ("Super Model           = %s\r\n", pData ->m_szSuperModel);

    TwoBlankLines ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Write an animation header
//
// @parm int | nStart | Start of the block
//
// @parm size_t | nSize | Size of the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawFormatter::WriteAnimationHeader (int nStart, size_t nSize)
{
    _ASSERTE (sizeof (CNwnMdlAnimation) == nSize);

    CNwnMdlAnimation *pData = (CNwnMdlAnimation *) 
        m_sRes .GetModelData (nStart);

    WriteGeometryHeader (pData, "Animation Header", nStart, nSize);

    Format ("Length                = %f\r\n", pData ->m_fLength);

    Format ("Transition Time       = %f\r\n", pData ->m_fTransTime);

    Format ("Animation Root        = %s\r\n", pData ->m_szAnimRoot);

    Format ("Event Offset          = %08X\r\n", pData ->m_asEvents .GetOffset ());

    Format ("Event Count           = %d\r\n", pData ->m_asEvents .GetCount ());

    Format ("Event Alloc           = %d\r\n", pData ->m_asEvents .GetAllocated ());

    TwoBlankLines ();

    //
    // Write the animation events
    //

    if (pData ->m_asEvents .GetCount () > 0)
    {
        WriteAnimationEvents (
            pData ->m_asEvents .GetOffset (), 
            pData ->m_asEvents .GetCount () * 
            pData ->m_asEvents .GetElementSize ());
    }
}


/* vim: set sw=4: */
