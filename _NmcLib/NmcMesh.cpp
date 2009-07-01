//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NmcMesh.cpp - NWN Model compiler |
//
// This module contains the Nwn model compiler support.
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

#include <math.h>
#include "Nmc.h"
#include "NmcCoreParsers.h"
#include "NmcMesh.h"

#pragma warning (disable : 4311) // pointer truncation
#pragma warning (disable : 4312) // greater size

inline UINT32 MakeRGBA (unsigned char r, unsigned char g, 
                        unsigned char b, unsigned char a)
{
    return (a << 24) | (b << 16) | (g << 8) | (r << 0);
}

//-----------------------------------------------------------------------------
//
// @func Validate the face data
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CNwnMdlMeshHeader * | pMesh | Mesh object
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcValidateFaceData (CNmcContext *pCtx, CNwnMdlMeshHeader *pMesh)
{
    //
    // Get pointers to structures we will be using a LOT
    //

    NmcMeshTemp *pMeshTemp = (NmcMeshTemp *) pMesh ->m_pvTempMeshData;

    //
    // Compute the max face texindex and vert index
    //

    int nMaxVI = -1;
    int nMaxTI = -1;
    for (int i = 0; i < (int) pMeshTemp ->aFaces .GetCount (); i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (nMaxVI < pMeshTemp ->aFaces [i] .anVerts [j])
                nMaxVI = pMeshTemp ->aFaces [i] .anVerts [j];
            if (nMaxTI < pMeshTemp ->aFaces [i] .sTexIndices .anIndices [j])
                nMaxTI = pMeshTemp ->aFaces [i] .sTexIndices .anIndices [j];
        }
    }

    //
    // Validate the vertex count
    //

    if (nMaxVI > (int) pMeshTemp ->aVerts .GetCount ())
    {
        pCtx ->GenerateWarning ("Number of verts (%d/%d) "
                                "not enough for the given face data, "
                                "faces will be ignored",
                                (int) pMeshTemp ->aVerts .GetCount (), nMaxVI);
        pMeshTemp ->aFaces .RemoveAll ();
        return;
    }

    //
    // Validate the texture count
    //

    if (nMaxTI > (int) pMeshTemp ->aTVerts [0] .GetCount ())
    {
        pCtx ->GenerateWarning ("Number of tverts (%d/%d) "
                                "not enough for the given face data, "
                                "faces will be ignored",
                                (int) pMeshTemp ->aTVerts [0] .GetCount (), nMaxTI);
        pMeshTemp ->aFaces .RemoveAll ();
        return;
    }

    //
    // Validate the colors
    //

    if (pMeshTemp ->aColors .GetCount () > 0 &&
        nMaxVI > (int) pMeshTemp ->aColors .GetCount ())
    {
        pCtx ->GenerateWarning ("Number of colors (%d/%d) "
                                "not enough for the given face data, "
                                "colors will be ignored",
                                (int) pMeshTemp ->aColors .GetCount (), nMaxVI);
        pMeshTemp ->aColors .RemoveAll ();
    }

    //
    // Validate the mirror list
    //

    if (pMeshTemp ->aMirrorList .GetCount () > 0 &&
        pMeshTemp ->aMirrorList .GetCount () < 
        pMeshTemp ->aFaces .GetCount ())
    {
        pCtx ->GenerateWarning ("Number of entries in the mirrorlist (%d/%d) "
                                "not enough for the given number of faces, "
                                "mirrorlist will be ignored",
                                pMeshTemp ->aMirrorList .GetCount (),
                                pMeshTemp ->aFaces .GetCount ());
        pMeshTemp ->aMirrorList .RemoveAll ();
    }


    //
    // If this is a dangly, then validate contraint
    //

    if (pMesh ->m_ulFlags == NwnMdlNT_DanglyMesh)
    {
        CNwnMdlDanglyMeshNode *pDangly = static_cast <CNwnMdlDanglyMeshNode *> (pMesh);
        if (pDangly ->m_afConstraints .GetCount () > 0 &&
            nMaxVI > (int) pDangly ->m_afConstraints .GetCount ())
        {
            pCtx ->GenerateWarning ("Number of contraints (%d/%d) "
                                    "not enough for the given face data, "
                                    "contraints will be ignored",
                                    (int) pDangly ->m_afConstraints .GetCount (), nMaxVI);
            pDangly ->m_afConstraints .RemoveAll ();
        }
    }

    //
    // Make sure the tex indices and vertices are good
    //

    for (int i = 1; i < 4; i++)
    {

        //
        // If we have a mismatch in the face count
        //

        if (pMeshTemp ->aTexIndices [i] .GetCount () != 0 &&
            pMeshTemp ->aTexIndices [i] .GetCount () != 
            pMeshTemp ->aFaces .GetCount ())
        {
            pCtx ->GenerateWarning ("Number of faces %d "
                                    "differs from number of texindices%d, "
                                    "texindices%d will be ignored", 
                                    pMeshTemp ->aFaces .GetCount (), i, i);
            pMeshTemp ->aTexIndices [i] .RemoveAll ();
        }

        //
        // Compute the max texindex
        //

        int nMaxTI = -1;
        for (int j = 0; j < (int) pMeshTemp ->aTexIndices [i] .GetCount (); j++)
        {
            for (int k = 0; k < 3; k++)
            {
                if (nMaxTI < pMeshTemp ->aTexIndices [i] [j] .anIndices [j])
                    nMaxTI = pMeshTemp ->aTexIndices [i] [j] .anIndices [j];
            }
        }

        //
        // If we don't have enough.
        //

        if (nMaxTI > (int) pMeshTemp ->aTVerts [i] .GetCount () &&
            (nMaxTI != 0 || i != 0 || pMeshTemp ->aTVerts [i] .GetCount () != 0))
        {
            pCtx ->GenerateWarning ("Number of tverts%d (%d/%d) "
                                    "not enough for the given texindices%d, "
                                    "texindices%d will be ignored", i,
                                    (int) pMeshTemp ->aTVerts [i] .GetCount (), nMaxTI,
                                    i, i);
            pMeshTemp ->aTexIndices [i] .RemoveAll ();
        }
    }
}

//-----------------------------------------------------------------------------
//
// @func Perform a series for face initializations
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CNwnMdlMeshHeader * | pMesh | Mesh object
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcInitializeFaceData (CNmcContext *pCtx, CNwnMdlMeshHeader *pMesh)
{
    pCtx; //4100

    //
    // Get pointers to structures we will be using a LOT
    //

    NmcMeshTemp *pMeshTemp = (NmcMeshTemp *) pMesh ->m_pvTempMeshData;
    NmcMeshFace *pTFaces = pMeshTemp ->aFaces .GetData (); 

    //
    // Initialize the vert normal indices
    //

    for (size_t nFace = 0; nFace < pMeshTemp ->aFaces .GetCount (); nFace++)
    {
        pTFaces [nFace] .anVertNormals [0] = -1;
        pTFaces [nFace] .anVertNormals [1] = -1;
        pTFaces [nFace] .anVertNormals [2] = -1;
    }

    //
    // Loop through the faces and compute the vertex normals
    //

    for (size_t nFace = 0; nFace < pMeshTemp ->aFaces .GetCount (); nFace++)
    {

        //
        // Loop through the vertices
        //

        for (int nVert = 0; nVert < 3; nVert++)
        {

            //
            // If we have already done this vert, skip
            //

            if (pTFaces [nFace] .anVertNormals [nVert] != -1)
                continue;

            //
            // Loop through all the faces and sum up the face formals
            //

            CVector3 vSum (0, 0, 0);
            for (size_t nFace2 = nFace; nFace2 < 
                 pMeshTemp ->aFaces .GetCount (); nFace2++)
            {

                //
                // If the smooth mask doesn't match, skip
                //

                if (nFace2 != nFace &&
                    (pTFaces [nFace] .nSmoothMask & 
                     pTFaces [nFace2] .nSmoothMask) == 0)
                    continue;

                //
                // Loop through all the verts in this face
                //

                for (int nVert2 = 0; nVert2 < 3; nVert2++)
                {

                    //
                    // If we have done this vert, then skip
                    //

                    if (pTFaces [nFace2] .anVertNormals [nVert2] != -1)
                        continue;

                    //
                    // Skip if not the same vertex
                    //

                    if (pTFaces [nFace2] .anVerts [nVert2] != 
                        pTFaces [nFace] .anVerts [nVert])
                        continue;

                    //
                    // Compute the face normal
                    //

                    CVector3 &p1 = pMeshTemp ->aVerts [pTFaces [nFace2] .anVerts [0]];
                    CVector3 &p2 = pMeshTemp ->aVerts [pTFaces [nFace2] .anVerts [1]];
                    CVector3 &p3 = pMeshTemp ->aVerts [pTFaces [nFace2] .anVerts [2]];
                    CVector3 vFaceNormal;
                    vFaceNormal .Cross (p2 - p1, p3 - p2);

                    //
                    // Add to the sum
                    //

                    vSum += vFaceNormal;

                    //
                    // Save the index
                    //

                    pTFaces [nFace2] .anVertNormals [nVert2] = (int) 
                        pMeshTemp ->aVertNormals .GetCount ();
                }
            }

            //
            // Verify we did it
            //

            assert (pTFaces [nFace] .anVertNormals [nVert] != -1);

            //
            // Normalize the sum and add to the array
            //

            vSum .Normalize ();
            pMeshTemp ->aVertNormals .Add (vSum);
        }
    }

    //
    // Compute the bmin/bmax/average for the vertices
    //

    CVector3 vSum (0, 0, 0);
    CVector3 *pVerts = pMeshTemp ->aVerts .GetData ();
    for (size_t nVert = 0; nVert < pMeshTemp ->aVerts .GetCount (); nVert++)
    {
        if (pMesh ->m_vBMin .m_x > pVerts [nVert] .m_x)
            pMesh ->m_vBMin .m_x = pVerts [nVert] .m_x;
        if (pMesh ->m_vBMin .m_y > pVerts [nVert] .m_y)
            pMesh ->m_vBMin .m_y = pVerts [nVert] .m_y;
        if (pMesh ->m_vBMin .m_z > pVerts [nVert] .m_z)
            pMesh ->m_vBMin .m_z = pVerts [nVert] .m_z;
        if (pMesh ->m_vBMax .m_x < pVerts [nVert] .m_x)
            pMesh ->m_vBMax .m_x = pVerts [nVert] .m_x;
        if (pMesh ->m_vBMax .m_y < pVerts [nVert] .m_y)
            pMesh ->m_vBMax .m_y = pVerts [nVert] .m_y;
        if (pMesh ->m_vBMax .m_z < pVerts [nVert] .m_z)
            pMesh ->m_vBMax .m_z = pVerts [nVert] .m_z;
        vSum += pVerts [nVert];
    }
    pMesh ->m_vBAverage = vSum / (float) pMeshTemp ->aVerts .GetCount ();

    //
    // Compute the radius
    //

    for (size_t nVert = 0; nVert < pMeshTemp ->aVerts .GetCount (); nVert++)
    {
        CVector3 v (pVerts [nVert] - pMesh ->m_vBAverage);
        float fLength = v .Length ();
        if (pMesh ->m_fRadius < fLength)
            pMesh ->m_fRadius = fLength;
    }
}

//-----------------------------------------------------------------------------
//
// @func Build a vertex for a face
//
// @parm CNwnMdlMeshHeader * | pMesh | Mesh object
//
// @parm int | nFace | Index of the face
//
// @parm int | nFVert | Face vertex to build
//
// @parm NmcBuiltVertex * | pVertex | Fully qualified vertex
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcBuildVertexFromFace (CNwnMdlMeshHeader *pMesh, int nFace, 
                             int nFVert, NmcBuiltVertex *pVertex)
{
    //
    // Get pointers to structures we will be using a LOT
    //

    NmcMeshTemp *pMeshTemp = (NmcMeshTemp *) pMesh ->m_pvTempMeshData;
    NmcMeshFace *pTFace = &pMeshTemp ->aFaces [nFace]; 

    //
    // Initialize the vertex
    //

    pVertex ->nFlags = 0;
    pVertex ->nFace = (int) nFace;
    pVertex ->nTVert = -1;

    //
    // Get the mirror
    //

    if (pMeshTemp ->aMirrorList .GetCount () > 0)
        pVertex ->fMirror = pMeshTemp ->aMirrorList [nFace] != 0;
    else
        pVertex ->fMirror = false;

    //
    // Initialize the vertex/vertex normal
    //

    pVertex ->nVert = pTFace ->anVerts [nFVert];
    pVertex ->vVert = pMeshTemp ->aVerts [pTFace ->anVerts [nFVert]];
    pVertex ->nVertNormal = pTFace ->anVertNormals [nFVert];
    pVertex ->vVertNormal = pMeshTemp ->aVertNormals [pTFace ->anVertNormals [nFVert]];

    //
    // Build the texture vertices
    //

    for (int i = 0; i < 4; i++)
    {
        if (pMeshTemp ->aTVerts [i] .GetCount () > 0)
        {
            int n;
            if (i == 0)
            {
                n = pTFace ->sTexIndices .anIndices [nFVert];
                pVertex ->nTVert = n;
            }
            else
                n = pMeshTemp ->aTexIndices [1] [nFace] .anIndices [nFVert];
            pVertex ->vTVert [i] = pMeshTemp ->aTVerts [i] [n];
            pVertex ->nFlags |= 1 << i;
        }
        else
            pVertex ->vTVert [i] = g_vOrigin;           
    }

    //
    // Get the color (only for Trimesh, AABBMesh, and AnimMesh)
    //

    if (pMeshTemp ->aColors .GetCount () > 0 && 
        (pMesh ->m_ulFlags == NwnMdlNT_TriMesh || 
         pMesh ->m_ulFlags == NwnMdlNT_AABBMesh ||
         pMesh ->m_ulFlags == NwnMdlNT_AnimMesh))
        pVertex ->vColor = pMeshTemp ->aColors [pTFace ->anVerts [nFVert]];
    else
        pVertex ->vColor = CVector3 (1, 1, 1);  

    //
    // If this is a dangly, then get the contraint
    //

    pVertex ->fContraint = 0;
    if (pMesh ->m_ulFlags == NwnMdlNT_DanglyMesh)
    {
        CNwnMdlDanglyMeshNode *pDangly = static_cast <CNwnMdlDanglyMeshNode *> (pMesh);
        if (pDangly ->m_afConstraints .GetCount () > 0)
            pVertex ->fContraint = pDangly ->m_afConstraints [pTFace ->anVerts [nFVert]];
    }
    return;
}

//-----------------------------------------------------------------------------
//
// @func Compare two vertices
//
// @parm NmcBuiltVertex * | pV1 | First vertex
//
// @parm NmcBuiltVertex * | pV2 | Second vertex
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

bool NmcCompareBuiltVertices (NmcBuiltVertex *pV1, NmcBuiltVertex *pV2)
{
    return 
        pV1 ->nFlags == pV2 ->nFlags &&
        pV1 ->vVert == pV2 ->vVert &&
        pV1 ->vVertNormal == pV2 ->vVertNormal &&
        pV1 ->vTVert [0] == pV2 ->vTVert [0] &&
        pV1 ->vTVert [1] == pV2 ->vTVert [1] &&
        pV1 ->vTVert [2] == pV2 ->vTVert [2] &&
        pV1 ->vTVert [3] == pV2 ->vTVert [3] &&
        pV1 ->vColor == pV2 ->vColor &&
        pV1 ->fContraint == pV2 ->fContraint &&
        pV1 ->fMirror == pV2 ->fMirror;
}

//-----------------------------------------------------------------------------
//
// @func Build the faces and include the vertices
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CNwnMdlMeshHeader * | pMesh | Mesh object
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcBuildFacesWithVertices (CNmcContext *pCtx, CNwnMdlMeshHeader *pMesh)
{
    //
    // Get pointers to structures we will be using a LOT
    //

    NmcMeshTemp *pMeshTemp = (NmcMeshTemp *) pMesh ->m_pvTempMeshData;
    NmcMeshFace *pTFaces = pMeshTemp ->aFaces .GetData (); 

    //
    // Initialize a dummy face structure
    //

    NwnMdlFace sFace;
    sFace .asAdjFace [0] = -1;
    sFace .asAdjFace [1] = -1;
    sFace .asAdjFace [2] = -1;
    sFace .ausVertex [0] = 0xFFFF;
    sFace .ausVertex [1] = 0xFFFF;
    sFace .ausVertex [2] = 0xFFFF;
    sFace .fPlaneDistance = 0;
    sFace .vPlaneNormal = g_vOrigin;
    sFace .ulSurface = 0;

    //
    // If we use trimesh/aabb vertex creation style
    //

    if (pMesh ->m_ulFlags == NwnMdlNT_TriMesh ||
        pMesh ->m_ulFlags == NwnMdlNT_AABBMesh)
    {

        //
        // Loop through the faces
        //

        for (int nFace = 0; nFace < (int) 
             pMeshTemp ->aFaces .GetCount (); nFace++)
        {
            NmcMeshFace *pTFace = &pTFaces [nFace];

            //
            // Initialize the face
            //

            sFace .ulSurface = pTFace ->nSurface;

            //
            // Build the 3 veriticies
            //

            NmcBuiltVertex asVertices [3];
            NmcBuildVertexFromFace (pMesh, nFace, 0, &asVertices [0]);
            NmcBuildVertexFromFace (pMesh, nFace, 1, &asVertices [1]);
            NmcBuildVertexFromFace (pMesh, nFace, 2, &asVertices [2]);

            //
            // Located vertices
            //

            int nLocatedCount = 0;
            int nLocatedVertices [3];

            //
            // Loop through all previous faces looking for vertex matches
            //

            for (int nFace2 = 0; nFace2 < 
                 (int) pMesh ->m_sFaces .GetCount (); nFace2++)
            {
                nLocatedCount = 0;
                for (int nFVert = 0; nFVert < 3; nFVert++)
                {
                    nLocatedVertices [nFVert] = 0xFFFFFFFF;
                    for (int nFVert2 = 0; nFVert2 < 3; nFVert2++)
                    {
                        NwnMdlFace *pFace2 = &pMesh ->m_sFaces [nFace2];
                        NmcBuiltVertex *pTestVertex = &pMeshTemp ->
                            aBuiltVertices [pFace2 ->ausVertex [nFVert2]];
                        if (NmcCompareBuiltVertices (
                                pTestVertex, &asVertices [nFVert]))
                        {
                            nLocatedCount++;
                            nLocatedVertices [nFVert] = 
                                pFace2 ->ausVertex [nFVert2];
                            break;              
                        }
                    }
                }

                //
                // If we had 2 matches, break
                //

                if (nLocatedCount > 1)
                    break;
            }

            //
            // Add the vertices
            //

            for (int nFVert = 0; nFVert < 3; nFVert++)
            {
                if (nLocatedVertices [nFVert] == -1 || nLocatedCount < 2)
                {
                    sFace .ausVertex [nFVert] = (UINT16) pMeshTemp ->
                        aBuiltVertices .GetCount ();
                    pMeshTemp ->aBuiltVertices .Add (asVertices [nFVert]);
                }
                else
                {
                    sFace .ausVertex [nFVert] = (UINT16) nLocatedVertices [nFVert];
                }
            }

            //
            // Add the face
            //

            if (!pCtx ->GetPurgeNullFaces () ||
                (sFace .ausVertex [0] != sFace .ausVertex [1] &&
                 sFace .ausVertex [0] != sFace .ausVertex [2] &&
                 sFace .ausVertex [1] != sFace .ausVertex [2]))
                pMesh ->m_sFaces .Add (sFace);
            else
            {
                pCtx ->GenerateWarning ("Face %d contains duplicate vertices "
                                        "(%d %d %d), ignored",
                                        nFace,
                                        sFace .ausVertex [0],
                                        sFace .ausVertex [1],
                                        sFace .ausVertex [2]);
            }
        }                                                       
    }

    //
    // Otherwise, for dangly, skin, and animation mesh
    //

    else
    {
        //
        // Loop through the faces
        //

        for (int nFace = 0; nFace < (int) 
             pMeshTemp ->aFaces .GetCount (); nFace++)
        {
            NmcMeshFace *pTFace = &pTFaces [nFace];

            //
            // Initialize the face
            //

            sFace .ulSurface = pTFace ->nSurface;

            //
            // Loop through the vertices
            //

            for (int nFVert = 0; nFVert < 3; nFVert++)
            {

                //
                // Build the veriticies
                //

                NmcBuiltVertex sVertex;
                NmcBuildVertexFromFace (pMesh, nFace, nFVert, &sVertex);

                //
                // Loop through the vertices
                //

                int nVert;
                for (nVert = 0; nVert < (int) pMeshTemp 
                     ->aBuiltVertices .GetCount (); nVert++)
                {
                    //if (nFVert == 2 && nVert == sFace .ausVertex [1])
                    //  continue;
                    //if (nFVert == 1 && nVert == sFace .ausVertex [0])
                    //  continue;
                    if (NmcCompareBuiltVertices (
                            &pMeshTemp ->aBuiltVertices [nVert], 
                            &sVertex))
                    {
                        break;
                    }
                }

                //
                // Add if not found
                //

                if (nVert >= (int) pMeshTemp ->aBuiltVertices .GetCount ())
                    pMeshTemp ->aBuiltVertices .Add (sVertex);
                sFace .ausVertex [nFVert] = (UINT16) nVert;
            }

            //
            // Add the face
            //

            pMesh ->m_sFaces .Add (sFace);
        }
    }
    return;
}

//-----------------------------------------------------------------------------
//
// @func Initialize the face place and adjacency information
//
// @parm CNwnMdlMeshHeader * | pMesh | Mesh object
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcInitFacePlaneAndAdjFaces (CNwnMdlMeshHeader *pMesh)
{
    //
    // Get pointers to structures we will be using a LOT
    //

    NmcMeshTemp *pMeshTemp = (NmcMeshTemp *) pMesh ->m_pvTempMeshData;
    NwnMdlFace *pFaces = pMesh ->m_sFaces .GetData (); 
    NmcMeshFace *pTFaces = pMeshTemp ->aFaces .GetData (); 

    //
    // Loop through the faces
    //

    for (int nFace = 0; nFace < (int) pMesh ->m_sFaces .GetCount (); nFace++)
    {

        //
        // Get the face and temporary face data
        //

        NwnMdlFace *pFace = &pFaces [nFace];
        NmcMeshFace *pTFace = &pTFaces [nFace];

        //
        // Get pointers to the three vertices
        //

        CVector3 &p3 = pMeshTemp ->aVerts [pTFace ->anVerts [2]];
        CVector3 &p2 = pMeshTemp ->aVerts [pTFace ->anVerts [1]];
        CVector3 &p1 = pMeshTemp ->aVerts [pTFace ->anVerts [0]];

        //
        // Compute the face plane
        //

        pFace ->vPlaneNormal .Cross (p2 - p1, p3 - p2);
        pFace ->vPlaneNormal .Normalize ();
        pFace ->fPlaneDistance = - pFace ->vPlaneNormal .Dot (p1);

        //
        // Loop through the three edges of the face
        //

        for (int nFVert = 0; nFVert < 3; nFVert++)
        {

            //
            // Get the two vertices of the edge
            //

            int nV1 = pTFace ->anVerts [nFVert];
            int nV2 = pTFace ->anVerts [(nFVert + 1) % 3];
            pFace ->asAdjFace [nFVert] = -1;

            //
            // Loop through the faces again
            //

            for (int nFace2 = 0; nFace2 < (int) 
                 pMesh ->m_sFaces .GetCount (); nFace2++)
            {

                //
                // Skip myself
                //

                if (nFace == nFace2)
                    continue;

                //
                // Get the face and temporary face data
                //

                NmcMeshFace *pTFace2 = &pTFaces [nFace2];

                //
                // Loop through that faces 3 edges
                //

                for (int nFVert2 = 0; nFVert2 < 3; nFVert2++)
                {
                    if (pTFace2 ->anVerts [nFVert2] == nV1 &&
                        (pTFace2 ->anVerts [(nFVert2 + 1) % 3] == nV2 || 
                         pTFace2 ->anVerts [(nFVert2 + 2) % 3] == nV2))
                    {
                        pFace ->asAdjFace [nFVert] = (INT16) nFace2;
                        break;
                    }
                }

                //
                // If we have a face, break
                //

                if (pFace ->asAdjFace [nFVert] != -1)
                    break;
            }
        }
    }
}

//-----------------------------------------------------------------------------
//
// @func Build the final vertex list
//
// @parm CNwnMdlMeshHeader * | pMesh | Mesh object
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcBuildVertexList (CNwnMdlMeshHeader *pMesh)
{

    //
    // Create an array of all the vertices ordered by face
    //

    int nVertCount = 3 * (int) pMesh ->m_sFaces .GetCount ();
    UINT32 *pulVerts = new UINT32 [nVertCount];
    for (int nFace = 0, nVert = 0; nFace < (int) 
         pMesh ->m_sFaces .GetCount (); nFace++)
    {
        NwnMdlFace &sFace = pMesh ->m_sFaces [nFace];
        pulVerts [nVert++] = sFace .ausVertex [0];
        pulVerts [nVert++] = sFace .ausVertex [1];
        pulVerts [nVert++] = sFace .ausVertex [2];
    }

    //
    // Save the pointer and the count
    //

    pMesh ->m_sVertexIndices .Add (pulVerts);
    pMesh ->m_sVertexIndicesCount .Add (nVertCount);    

    //
    // We are in triangle mode
    //

    pMesh ->m_ucTriangleMode = 3;       

    //
    // Copy the vertex from the VertexIndices to VertexIndicesOffset
    //

    for (int nIndex = 0; nIndex < (int) 
         pMesh ->m_sVertexIndices .GetCount (); nIndex++)
    {
        int nCount = (int) pMesh ->m_sVertexIndicesCount [nIndex];
        UINT32 *pulVerts = pMesh ->m_sVertexIndices [nIndex] .GetData ();
        UINT16 *pusVerts = new UINT16 [nCount];
        pMesh ->m_sRawVertexIndices .Add (pusVerts);
        for (int i = 0; i < nCount; i++)
            pusVerts [i] = (UINT16) pulVerts [i];
        delete [] pulVerts;
    }

    //
    // Clear the vert array
    //

    pMesh ->m_sVertexIndices .RemoveAll ();
    pMesh ->m_ulSomething3Offset = 0xFFFFFFFF;
    pMesh ->m_ulSomething3Count = 0;
    return;
}

//-----------------------------------------------------------------------------
//
// @func Compute the face normal something or another
//
// @parm CNwnMdlMeshHeader * | pMesh | Mesh object
//
// @parm const CNwnArray <NwnMdlFace> & | aFaces | Face array
//
// @rdesc Resulting value
//
//-----------------------------------------------------------------------------

float NmcComputeFaceNormalSubDiv2 (CNwnMdlMeshHeader *pMesh, 
                                   const CNwnArray <NwnMdlFace> &aFaces)
{
    const NwnMdlFace *pFaces = aFaces .GetData ();
    CVector3 *pVertices = pMesh ->m_pavVerts .GetData ();
    float fResult = 0.0f;

    //
    // I really don't know what this is doing.
    //
    // HEY, DEAL WITH IT.  At least I am honest about it!!!
    //

    for (int nFace = 0; nFace < (int) aFaces .GetCount (); nFace++)
    {
        const NwnMdlFace *pFace = &pFaces [nFace];
        CVector3 &p1 = pVertices [pFace ->ausVertex [0]];
        CVector3 &p2 = pVertices [pFace ->ausVertex [1]];
        CVector3 &p3 = pVertices [pFace ->ausVertex [2]];
        CVector3 vNormal;
        vNormal .Cross (p2 - p1, p3 - p1);
        fResult += vNormal .Length () * 0.5f;
    }
    return fResult;
}

//-----------------------------------------------------------------------------
//
// @func If required, generate the texture bumpmap data
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CNwnMdlMeshHeader * | pMesh | Mesh object
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcGenerateBumpmapData (CNmcContext *pCtx, CNwnMdlMeshHeader *pMesh)
{
    NmcMeshTemp *pMeshTemp = (NmcMeshTemp *) pMesh ->m_pvTempMeshData;

    //
    // Test to see if the texture should have information generated
    //

    if (pMesh ->m_szTextures [0] == 0 ||
        stricmp (pMesh ->m_szTextures [0], "NULL") == 0 ||
        !pCtx ->IsTextureBumpmapped (pMesh ->m_szTextures [0]))
        return;

    //
    // Allocate the data
    //

    CVector3 *pavBumpmapData1 = new CVector3 [pMesh ->m_usVertexCount];
    CVector3 *pavBumpmapData2 = new CVector3 [pMesh ->m_usVertexCount];
    CVector3 *pavBumpmapData3 = new CVector3 [pMesh ->m_usVertexCount];
    CVector3 *pavBumpmapData4 = new CVector3 [pMesh ->m_usVertexCount];
    CVector3 *pavBumpmapData5 = new CVector3 [pMesh ->m_usVertexCount];
    float *pafBumpmapData6 = new float [pMesh ->m_usVertexCount];

    //
    // Save the pointers
    //

    pMesh ->m_pavBumpmapAnim1 .SetData (pavBumpmapData1);
    pMesh ->m_pavBumpmapAnim2 .SetData (pavBumpmapData2);
    pMesh ->m_pavBumpmapAnim3 .SetData (pavBumpmapData3);
    pMesh ->m_pavBumpmapAnim4 .SetData (pavBumpmapData4);
    pMesh ->m_pavBumpmapAnim5 .SetData (pavBumpmapData5);
    pMesh ->m_pafBumpmapAnim6 .SetData (pafBumpmapData6);

    //
    // Initialize some values
    //

    for (int i = 0; i < pMesh ->m_usVertexCount; i++)
    {
        pavBumpmapData4 [i] = g_vOrigin;
        pavBumpmapData5 [i] = g_vOrigin;
        pafBumpmapData6 [i] = 0;
    }

    //
    // Loop through the faces
    //

    CVector3 *pNormals = pMesh ->m_pavNormals .GetData ();
    CVector3 *pVerts = pMesh ->m_pavVerts .GetData ();
    CVector2 *pTVerts = pMesh ->m_pavTVerts [0] .GetData ();
    for (int nFace = 0; nFace < (int) 
         pMesh ->m_sFaces .GetCount (); nFace++)
    {
        NwnMdlFace *pFace = &pMesh ->m_sFaces [nFace];

        //
        // Get the mirror flag
        //

        bool fMirror = false;
        if (pMeshTemp ->aMirrorList .GetCount () > 0)
            fMirror = pMeshTemp ->aMirrorList [nFace] != 0;

        //
        // Get the verts and tverts
        //

        CVector3 v0 = pVerts [pFace ->ausVertex [0]];
        CVector3 v1 = pVerts [pFace ->ausVertex [1]];
        CVector3 v2 = pVerts [pFace ->ausVertex [2]];
        CVector2 t0 = pTVerts [pFace ->ausVertex [0]];
        CVector2 t1 = pTVerts [pFace ->ausVertex [1]];
        CVector2 t2 = pTVerts [pFace ->ausVertex [2]];

        //
        // Compute the deltas
        //

        CVector3 v1m0 (v1 - v0);
        CVector3 v2m0 (v2 - v0);
        CVector2 t1m0 (t1 - t0);
        CVector2 t2m0 (t2 - t0);

        //
        // Do something
        //

        CVector3 vCol1 ((v2m0 * t1m0 .m_x) - (v1m0 * t2m0 .m_x));
        CVector3 vCol2 ((v2m0 * t1m0 .m_y) - (v1m0 * t2m0 .m_y));
        vCol1 .Normalize ();
        vCol2 .Normalize ();

        //
        // If we are mirror, negate 
        //

        if (fMirror)
            vCol1 .Negate ();

        //
        // Compute the cross
        //

        CVector3 vCol3 (vCol1 .Cross (vCol2));

        //
        // Negate the right if mirror
        //

        if (fMirror)
            vCol2 .Negate ();

        //
        // Loop through the verts
        //

        for (int nFVert = 0; nFVert < 3; nFVert++)
        {
            int nVert = pFace ->ausVertex [nFVert];

            //
            // If the normal dot the cross is negative, then negate
            //

            if (pNormals [nVert] .Dot (vCol3) < 0)
            {
                pavBumpmapData1 [nVert] = vCol1;
                pavBumpmapData2 [nVert] = - vCol2;
                pavBumpmapData3 [nVert] = - vCol3;
            }
            else
            {
                pavBumpmapData1 [nVert] = vCol1;
                pavBumpmapData2 [nVert] = vCol2;
                pavBumpmapData3 [nVert] = vCol3;
            }
        }
    }
}

//-----------------------------------------------------------------------------
//
// @func Handle post mesh processing
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CNwnMdlMeshHeader * | pMesh | Mesh object
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcPostMesh (CNmcContext *pCtx, CNwnMdlMeshHeader *pMesh)
{
    //
    // Get pointers to structures we will be using a LOT
    //

    NmcMeshTemp *pMeshTemp = (NmcMeshTemp *) pMesh ->m_pvTempMeshData;

    //
    // Some helper casts
    //

    CNwnMdlAnimMeshNode *pAnimMesh = static_cast <CNwnMdlAnimMeshNode *> (pMesh);
    CNwnMdlDanglyMeshNode *pDangly = static_cast <CNwnMdlDanglyMeshNode *> (pMesh);

    //
    // Validate the face data
    //

    NmcValidateFaceData (pCtx, pMesh);

    //
    // Initialize the face data
    //

    NmcInitializeFaceData (pCtx, pMesh);

    //
    // If this is an animation node
    //

    if (pMesh ->m_ulFlags == NwnMdlNT_AnimMesh)
    {

        //
        // Compute the number of sets
        //

        if (pMeshTemp ->aVerts .GetCount () != 0)
        {
            pAnimMesh ->m_ulVertexSetCount = (UINT32) (
                pAnimMesh ->m_avAnimVerts .GetCount () / 
                pMeshTemp ->aVerts .GetCount ());
        }
        if (pMeshTemp ->aTVerts [0] .GetCount () != 0)
        {
            pAnimMesh ->m_ulTextureSetCount = (UINT32) (
                pAnimMesh ->m_avAnimTVerts .GetCount () / 
                pMeshTemp ->aTVerts [0] .GetCount ());
        }

        //
        // Add the animation verts to the min/max
        //

        CVector3 *pVerts = pAnimMesh ->m_avAnimVerts .GetData ();
        for (size_t nVert = 0; nVert < 
             pAnimMesh ->m_avAnimVerts .GetCount (); nVert++)
        {
            if (pMesh ->m_vBMin .m_x > pVerts [nVert] .m_x)
                pMesh ->m_vBMin .m_x = pVerts [nVert] .m_x;
            if (pMesh ->m_vBMin .m_y > pVerts [nVert] .m_y)
                pMesh ->m_vBMin .m_y = pVerts [nVert] .m_y;
            if (pMesh ->m_vBMin .m_z > pVerts [nVert] .m_z)
                pMesh ->m_vBMin .m_z = pVerts [nVert] .m_z;
            if (pMesh ->m_vBMax .m_x < pVerts [nVert] .m_x)
                pMesh ->m_vBMax .m_x = pVerts [nVert] .m_x;
            if (pMesh ->m_vBMax .m_y < pVerts [nVert] .m_y)
                pMesh ->m_vBMax .m_y = pVerts [nVert] .m_y;
            if (pMesh ->m_vBMax .m_z < pVerts [nVert] .m_z)
                pMesh ->m_vBMax .m_z = pVerts [nVert] .m_z;
        }
    }

    //
    // If we have faces
    //

    if (pMeshTemp ->aFaces .GetCount () > 0)
    {

        //
        // Build the vertex list from the faces
        //

        NmcBuildFacesWithVertices (pCtx, pMesh);

        //
        // Save the vertex count
        //

        pMesh ->m_usVertexCount = (UINT16) 
            pMeshTemp ->aBuiltVertices .GetCount ();

        //
        // Initialize the face planes and adjacency information
        //

        NmcInitFacePlaneAndAdjFaces (pMesh);

        //
        // Build the vertex list
        //

        NmcBuildVertexList (pMesh);

        //
        // Get the texture coord flags
        //

        int nFlags = 0;
        if (pMeshTemp ->aBuiltVertices .GetCount () > 0)
            nFlags = pMeshTemp ->aBuiltVertices [0] .nFlags;

        //
        // Allocate the vertex array
        //

        CVector3 *pVerts = new CVector3 [pMesh ->m_usVertexCount];
        pMesh ->m_pavVerts .SetData (pVerts);

        //
        // Allocate the normal array
        //

        CVector3 *pVertNormals = new CVector3 [pMesh ->m_usVertexCount];
        pMesh ->m_pavNormals .SetData (pVertNormals);

        //
        // Allocate the color array
        //

        UINT32 *pulColors = NULL;
        if (pMesh ->m_ulFlags == NwnMdlNT_TriMesh || 
            pMesh ->m_ulFlags == NwnMdlNT_AABBMesh)
        {
            pulColors = new UINT32 [pMesh ->m_usVertexCount];
            pMesh ->m_paColors .SetData (pulColors);
        }

        //
        // Allocate the texture vertex arrays
        //

        CVector2 *pTVerts [4];
        for (int i = 0; i < 4; i++)
        {
            if ((nFlags & (1 << i)) != 0)
            {
                pTVerts [i] = new CVector2 [pMesh ->m_usVertexCount]; 
                pMesh ->m_pavTVerts [i] .SetData (pTVerts [i]);
                pMesh ->m_usTextureCount++;
            }
            else
                pTVerts [i] = NULL;
        }

        //
        // Populate the arrays
        //

        NmcBuiltVertex *pBVertices = pMeshTemp ->aBuiltVertices .GetData ();
        for (int nVert = 0; nVert < pMesh ->m_usVertexCount; nVert++)
        {
            NmcBuiltVertex *pBVertex = &pBVertices [nVert];

            //
            // Save the texture verts
            //

            for (int i = 0; i < 4; i++)
            {
                if (pTVerts [i])
                {
                    pTVerts [i] [nVert] .m_x = pBVertex ->vTVert [i] .m_x;
                    pTVerts [i] [nVert] .m_y = pBVertex ->vTVert [i] .m_y;
                }
            }

            //
            // Save the verts
            //

            pVerts [nVert] = pBVertex ->vVert;
            pVertNormals [nVert] = pBVertex ->vVertNormal;

            //
            // Save the color
            //

            if (pulColors)
            {
                pulColors [nVert] = MakeRGBA (
                    (unsigned char) (pBVertex ->vColor .m_x * 255.0f + 0.5f),
                    (unsigned char) (pBVertex ->vColor .m_y * 255.0f + 0.5f),
                    (unsigned char) (pBVertex ->vColor .m_z * 255.0f + 0.5f),
                    0xFF);
            }
        }

        //
        // If this is an animmesh, then build the animation sets
        //

        if (pMesh ->m_ulFlags == NwnMdlNT_AnimMesh)
        {

            //
            // Allocate the data
            //

            CVector3 *pAVerts = new CVector3 [
                pAnimMesh ->m_ulVertexSetCount *
                pAnimMesh ->m_usVertexCount];
            CVector2 *pATVerts = new CVector2 [
                pAnimMesh ->m_ulTextureSetCount *
                pAnimMesh ->m_usVertexCount];

            //
            // Save the pointers
            //

            pAnimMesh ->m_pvVertexSets .SetData (pAVerts);
            pAnimMesh ->m_pvTextureSets .SetData (pATVerts);

            //
            // Loop through the vertices filling the data
            //

            int nVerts = (int) pMeshTemp ->aVerts .GetCount ();
            int nTVerts = (int) pMeshTemp ->aTVerts [0] .GetCount ();
            NmcBuiltVertex *pBVertices = pMeshTemp ->aBuiltVertices .GetData ();
            for (int nVert = 0; nVert < pMesh ->m_usVertexCount; nVert++)
            {
                NmcBuiltVertex *pBVertex = &pBVertices [nVert];
                for (int i = 0; i < (int) pAnimMesh ->m_ulVertexSetCount; i++)
                {
                    *pAVerts++ = pAnimMesh ->m_avAnimVerts [
                        (i * nVerts) + pBVertex ->nVert];
                }
                for (int i = 0; i < (int) pAnimMesh ->m_ulTextureSetCount; i++)
                {
                    CVector3 &v = pAnimMesh ->m_avAnimTVerts [
                        (i * nTVerts) + pBVertex ->nTVert];
                    pATVerts ->m_x = v .m_x;
                    pATVerts ->m_y = v .m_y;
                    pATVerts++;
                }
            }
        }
    }

    //
    // If this is a dangly mesh, we need to rebuilt the contraint array to
    // be based on the new expanded vertex array
    //

    if (pMesh ->m_ulFlags == NwnMdlNT_DanglyMesh)
    {
        CNwnArray <float> m_afConstraints;
        m_afConstraints .SetCount (pDangly ->m_usVertexCount);
        for (int nVert = 0; nVert < pDangly ->m_usVertexCount; nVert++)
            m_afConstraints [nVert] = pMeshTemp ->aBuiltVertices [nVert] .fContraint;
        pDangly ->m_afConstraints .Swap (m_afConstraints);
    }

    //
    // If this is an animmesh, then clear the compiler arrays
    //

    if (pMesh ->m_ulFlags == NwnMdlNT_AnimMesh)
    {
        pAnimMesh ->m_avAnimVerts .RemoveAll ();
        pAnimMesh ->m_avAnimTVerts .RemoveAll ();
        pAnimMesh ->m_avAnimNormals .RemoveAll ();
    }

    //
    // If this is a skin
    //

    if (pMesh ->m_ulFlags == NwnMdlNT_SkinMesh)
    {
        //
        // Texture animation
        //

        NmcGenerateBumpmapData (pCtx, pMesh);
    }

    //
    // If this is a trimesh or AABB
    //

    if (pMesh ->m_ulFlags == NwnMdlNT_TriMesh || 
        pMesh ->m_ulFlags == NwnMdlNT_AABBMesh)
    {

        //
        // Texture animation
        //

        NmcGenerateBumpmapData (pCtx, pMesh);

        //
        // Face normal sum (not in dangly)
        //

        pMesh ->m_fFaceNormalSumDiv2 = 
            NmcComputeFaceNormalSubDiv2 (
                pMesh, pMesh ->m_sFaces);
    }
    return;
}

//-----------------------------------------------------------------------------
//
// @func Get the node's position and orientation
//
// @parm CNwnMdlNodeHeader * | pNode | Current Node
//
// @parm CVector3 * | pvPosition | Node position
//
// @parm CQuaternion * | pqOrientation | Node orientation
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcGetNodePositionAndOrientation (CNwnMdlNodeHeader *pNode,
                                       CVector3 *pvPosition, CQuaternion *pqOrientation)
{

    //
    // Initialize the output
    //

    pvPosition ->m_x = 0;
    pvPosition ->m_y = 0;
    pvPosition ->m_z = 0;
    pqOrientation ->m_x = 0;
    pqOrientation ->m_y = 0;
    pqOrientation ->m_z = 0;
    pqOrientation ->m_w = 1;

    //
    // Loop through the controllers 
    //

    NwnMdlControllerKey *pControllerKey = pNode ->m_aControllerKey .GetData ();
    float *pControllerData = pNode ->m_aControllerData .GetData ();
    for (int i = 0; i < (int) pNode ->m_aControllerKey .GetCount (); i++)
    {
        if (pControllerKey [i] .lType == NwnMdlCT_Position)
        {
            float *pfData = &pControllerData [pControllerKey [i] .sDataOffset];
            pvPosition ->m_x = pfData [0];
            pvPosition ->m_y = pfData [1];
            pvPosition ->m_z = pfData [2];
        }
        else if (pControllerKey [i] .lType == NwnMdlCT_Orientation)
        {
            float *pfData = &pControllerData [pControllerKey [i] .sDataOffset];
            pqOrientation ->m_x = pfData [0];
            pqOrientation ->m_y = pfData [1];
            pqOrientation ->m_z = pfData [2];
            pqOrientation ->m_w = pfData [3];
        }
    }
    return;
}

//-----------------------------------------------------------------------------
//
// @func Build the T and Q bone arrays
//
// @parm CNwnMdlNodeHeader * | pNode | Current Node
//
// @parm CNwnArray <CVector3> & | aTBoneRefInv | TBone array
//
// @parm CNwnArray <CQuaternion> & | aQBoneRefInv | QBone array
//
// @parm CVector3 | vPosition | Node position
//
// @parm CQuaternion | qOrientation | Node orientation
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcBuildTQBoneArrays (CNwnMdlNodeHeader *pNode,
                           CNwnArray <CVector3> &aTBoneRefInv, CNwnArray <CQuaternion> &aQBoneRefInv,
                           CVector3 vPosition, CQuaternion qOrientation)
{

    //
    // Get my position and orientation
    //

    CVector3 vMyPosition;
    CQuaternion qMyOrientation;
    NmcGetNodePositionAndOrientation (pNode,
                                      &vMyPosition, &qMyOrientation);

    //
    // Rotate the position, sum and add to the array
    //

    qOrientation .Rotate (vMyPosition, vMyPosition);
    vPosition += vMyPosition;
    aTBoneRefInv .Add (vPosition);

    //
    // Combine the orientations and add
    //

    qOrientation *= qMyOrientation;
    aQBoneRefInv .Add (qOrientation);

    //
    // Invoke on the children
    //

    for (int i = 0; i < (int) pNode ->m_apChildren .GetCount (); i++)
    {
        CNwnMdlNodeHeader *pChild = pNode ->m_apChildren [i] .GetData ();
        NmcBuildTQBoneArrays (pChild, aTBoneRefInv,
                              aQBoneRefInv, vPosition, qOrientation);
    }
}

//-----------------------------------------------------------------------------
//
// @func Get the node index for a given node
//
// @parm CNwnMdlNodeHeader * | pNode | Current Node
//
// @parm const char * | pszName | Name 
//
// @parm int & | nIndex | Node index
//
// @rdesc true if found
//
//-----------------------------------------------------------------------------

bool NmcGetNodeIndex (CNwnMdlNodeHeader *pNode, 
                      const char *pszName, int &nIndex)
{

    //
    // Return if NULL
    //

    if (pNode == NULL)
        return false;

    //
    // Test myself
    //

    if (stricmp (pszName, pNode ->m_szName) == 0)
        return true;

    //
    // Not found, increment index
    //

    nIndex++;

    //
    // Invoke on the children
    //

    for (int i = 0; i < (int) pNode ->m_apChildren .GetCount (); i++)
    {
        CNwnMdlNodeHeader *pChild = pNode ->m_apChildren [i] .GetData ();
        if (NmcGetNodeIndex (pChild, pszName, nIndex))
            return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
//
// @func Count the number of nodes
//
// @parm CNwnMdlNodeHeader * | pNode | Current Node
//
// @rdesc Number of nodes
//
//-----------------------------------------------------------------------------

int NmcCountNodes (CNwnMdlNodeHeader *pNode)
{

    //
    // Return if NULL
    //

    if (pNode == NULL)
        return 0;

    //
    // Invoke on the children
    //

    int nCount = 1;
    for (int i = 0; i < (int) pNode ->m_apChildren .GetCount (); i++)
    {
        CNwnMdlNodeHeader *pChild = pNode ->m_apChildren [i] .GetData ();
        nCount += NmcCountNodes (pChild);
    }
    return nCount;
}

//-----------------------------------------------------------------------------
//
// @func Handle post mesh processing.  This phase is done after
//              all nodes in a geometry have been processed
//
// @parm CNmcContext * | pCtx | Current context
//
// @parm CNwnMdlMeshHeader * | pMesh | Mesh object
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcPostGeometryMesh (CNmcContext *pCtx, CNwnMdlMeshHeader *pMesh)
{
    //
    // Get pointers to structures we will be using a LOT
    //

    NmcMeshTemp *pMeshTemp = (NmcMeshTemp *) pMesh ->m_pvTempMeshData;

    //
    // If this is a skin node
    //

    if (pMesh ->m_ulFlags == NwnMdlNT_SkinMesh)
    {
        CNwnMdlSkinMeshNode *pSkin = static_cast 
            <CNwnMdlSkinMeshNode *> (pMesh);

        //
        // If this is a model geometry
        //

        CNwnMdlGeometryHeader *pModel = pCtx ->GetCurrentGeometry ();
        if (pModel ->m_ucGeometryType == NwnMdlGT_Model)
        {

            //
            // If we don't have any tbones
            //

            if (pSkin ->m_aTBoneRefInv .GetCount () == 0)
            {

                //
                // STEP 1: The first step is to get the root position and
                // orientation of the model
                //

                //
                // Get our current reversed information
                //

                CVector3 vRootPosition;
                CQuaternion qRootOrientation;
                NmcGetNodePositionAndOrientation (pSkin,
                                                  &vRootPosition, &qRootOrientation);
                qRootOrientation .m_w = - qRootOrientation .m_w;
                qRootOrientation .Rotate (- vRootPosition, vRootPosition);

                //
                // While we have a parent, reverse 
                //

                CNwnMdlNodeHeader *pNode = pSkin;
                while (pNode ->m_pParentNode .GetData () != NULL)
                {

                    //
                    // Move to next parent
                    //

                    pNode = pNode ->m_pParentNode .GetData ();

                    //
                    // Get this parents reversed data
                    //

                    CVector3 vPosition;
                    CQuaternion qOrientation;
                    NmcGetNodePositionAndOrientation (pNode,
                                                      &vPosition, &qOrientation);
                    qOrientation .m_w = - qOrientation .m_w;
                    qOrientation .Rotate (- vPosition, vPosition);

                    //
                    // Combine the position and orientation
                    //

                    vRootPosition += vPosition;
                    qRootOrientation *= qOrientation;
                }

                //
                // STEP 2: Build the TQBone arrays
                //

                NmcBuildTQBoneArrays (pNode, pSkin ->m_aTBoneRefInv,
                                      pSkin ->m_aQBoneRefInv, vRootPosition, 
                                      qRootOrientation);

                //
                // STEP 3: Reverse all the elements in the T/Q arrays
                //

                CVector3 *pavPos = pSkin ->m_aTBoneRefInv .GetData ();
                CQuaternion *paqOr = pSkin ->m_aQBoneRefInv .GetData ();
                for (int i = 0; i < (int) pSkin ->m_aTBoneRefInv .GetCount (); i++)
                {

                    //
                    // Adjust the values
                    //

                    paqOr [i] .m_w = - paqOr [i] .m_w;
                    paqOr [i] .Rotate (- pavPos [i], pavPos [i]);

                    //
                    // Bioware stores their Quats differently.  They use
                    // w,x,y,z.  I use x,y,z,w.  Now you could says, "Well hell,
                    // why don't you just change your order."  Well, you see
                    // they don't always store it in that order.  If you look
                    // at the orientation key, it is stored in x,y,z,w.  So
                    // at this point, we have 1 vote for x,y,z,w and one for
                    // w,x,y,z.  Life sucks, then you die and become a 
                    // technical support staffer.
                    //

                    float w = paqOr [i] .m_v [3];
                    paqOr [i] .m_v [3] = paqOr [i] .m_v [2];
                    paqOr [i] .m_v [2] = paqOr [i] .m_v [1];
                    paqOr [i] .m_v [1] = paqOr [i] .m_v [0];
                    paqOr [i] .m_v [0] = w;
                }

                //
                // T/Q Bones have been built
                //
            }

            //
            // Loop through the weights and resolve their indices
            //

            NwnMdlSkinWeight *paWeights = pSkin ->m_aWeights .GetData ();
            for (int i = 0; i < (int) pSkin ->m_aWeights .GetCount (); i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    paWeights [i] .nNodeIndices [j] = -1;
                    if (j < paWeights [i] .nCount &&
                        paWeights [i] .szBones [j] [0] != 0)
                    {
                        int nIndex = 0;
                        if (NmcGetNodeIndex (pModel ->m_pGeometry .GetData (),
                                             paWeights [i] .szBones [j], nIndex))
                        {
                            paWeights [i] .nNodeIndices [j] = nIndex;
                        }
                        else
                        {
                            pCtx ->GenerateError ("Bone \"%s\" is an unknown node name",
                                                  paWeights [i] .szBones [j]);
                        }
                    }
                }
            }

            //
            // Count the number of nodes we have
            //

            int nNodes = NmcCountNodes (
                pModel ->m_pGeometry .GetData ());

            //
            // Size the bone list (and initialized.  Not done by
            // bioware...)
            //

            pSkin ->m_aBoneConstantIndices .SetCount (nNodes);
            for (int i = 0; i < nNodes; i++)
                pSkin ->m_aBoneConstantIndices [i] = 0;

            //
            // Create the node to bone map
            //

            INT16 *pasNodeToBoneMap = new INT16 [nNodes];
            pSkin ->m_ulNodeToBoneCount = (UINT32) nNodes;
            pSkin ->m_pasNodeToBoneMap .SetData (pasNodeToBoneMap);
            for (int i = 0; i < nNodes; i++)
                pasNodeToBoneMap [i] = -1;

            //
            // Create the weights and bone refs
            //

            float *pafSkinWeights = new float [pSkin ->m_usVertexCount * 4];
            INT16 *pasSkinBoneRefs = new INT16 [pSkin ->m_usVertexCount * 4];
            pSkin ->m_pafSkinWeights .SetData (pafSkinWeights);
            pSkin ->m_pasSkinBoneRefs .SetData (pasSkinBoneRefs);

            //
            // Populate the weights and bone refs
            //

            int nTotalBones = 0;
            for (int i = 0; i < pSkin ->m_usVertexCount; i++)
            {
                int nWeight = pMeshTemp ->aBuiltVertices [i] .nVert;
                NwnMdlSkinWeight *pWeight = &paWeights [nWeight];
                for (int j = 0; j < 4; j++)
                {
                    pafSkinWeights [i * 4 + j] = 0; 
                    pasSkinBoneRefs [i * 4 + j] = -1;
                    float fWeight = pWeight ->fWeights [j];
                    if (fWeight != 0)
                    {
                        int nNode = pWeight ->nNodeIndices [j];
                        if (pasNodeToBoneMap [nNode] == -1)
                        {
                            if (nTotalBones > 
                                _countof (pSkin ->m_asBoneNodeNumbers))
                            {
                                pCtx ->GenerateError ("Too many bones "
                                                      "in skin, max allowed is %d", 
                                                      _countof (pSkin ->m_asBoneNodeNumbers));
                                goto things_went_really_bad;
                            }
                            pasNodeToBoneMap [nNode] = (INT16) nTotalBones;
                            pSkin ->m_asBoneNodeNumbers [nTotalBones++] = (INT16) nNode;
                        }
                        pafSkinWeights [i * 4 + j] = fWeight;
                        pasSkinBoneRefs [i * 4 + j] = pasNodeToBoneMap [nNode];
                    }
                }
            }
things_went_really_bad:;
        }

        //
        // Delete the weights
        //

        pSkin ->m_aWeights .RemoveAll ();
    }

    //
    // Delete the temp mesh information
    //

    delete pMeshTemp;
    pMesh ->m_pvTempMeshData = NULL;
    return;
}


/* vim: set sw=4: */
