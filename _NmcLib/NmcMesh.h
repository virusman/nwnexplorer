#ifndef ETS_NMCMESH_H
#define ETS_NMCMESH_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NmcMesh.h - Model compiler mesh include |
//
// This module contains the definition of structures needed for meshes.
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
// $History: CnfMainWnd.h $
//      
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include "../_NwnLib/NwnArray.h"
#include "NmcCoreParsers.h"

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

enum NmcBVF
{
    NmcBVF_HasTVert             = 0x01,
    NmcBVF_HasTVert1    = 0x02,
    NmcBVF_HasTVert2    = 0x04,
    NmcBVF_HasTVert3    = 0x08,
};

struct NmcBuiltVertex
{
    int                 nFlags;
    int                 nFace;
    int                 nVert;
    int                 nVertNormal;
    int                 nTVert;
    CVector3    vVert;
    CVector3    vVertNormal;
    CVector3    vTVert [4];
    CVector3    vColor;
    float               fContraint;
    bool                fMirror;
};

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

struct NmcMeshTemp
{
    CNwnArray <CVector3>                aVerts;                         // 0x00
    CNwnArray <CVector3>                aTVerts [4];            // 0x0C
    CNwnArray <CVector3>                aColors;                        // 0x3C
    CNwnArray <NmcMeshFace>             aFaces;                         // 0x48
    CNwnArray <CVector3>                aVertNormals;           // 0x54
    CNwnArray <NmcBuiltVertex>  aBuiltVertices;         // 0x60
    CNwnArray <UINT32>                  aMirrorList;            // 0x6C
    CNwnArray <NmcTexIndices>   aTexIndices [4];        // 0x78 not in original (0 not used)
    // 0xA8 - the end
};

#endif // ETS_NMC_H

/* vim: set sw=4: */
