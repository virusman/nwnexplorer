//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NmcGeometry.cpp - NWN Model compiler |
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

#include "Nmc.h"
#include "NmcCoreParsers.h"
#include "NmcMesh.h"
#include "../_NwnLib/NwnKeyFile.h"
#include "../_NwnLib/NwnModel.h"

//
// External routines
//

void NmcPostMesh (CNmcContext *pCtx, CNwnMdlMeshHeader *pMesh);
void NmcPostGeometryMesh (CNmcContext *pCtx, CNwnMdlMeshHeader *pMesh);

//-----------------------------------------------------------------------------
//
// @func Parse a new node block 
//
// @parm CNmcContext * | pCtx | Parser context
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcBeginNode (CNmcContext *pCtx)
{
    CNwnMdlNodeHeader *pNode = NULL;

    //
    // Get the node type and name
    //

    char szType [64];
    char szName [64];
    if (!NmcGetStringValue (pCtx, szType, _countof (szType), NULL, 0) ||
        !NmcGetStringValue (pCtx, szName, _countof (szName), NULL, 0))
        return;

    //
    // If this is a light node
    //

    if (stricmp (szType, "light") == 0)
    {

        //
        // Create a new light node
        //

        pNode = new CNwnMdlLightNode (szName);
    }

    //
    // If this is an emitter node
    //

    else if (stricmp (szType, "emitter") == 0)
    {

        //
        // Create a new emitter node
        //

        pNode = new CNwnMdlEmitterNode (szName);
    }

    //
    // If this is a camera node
    //

    else if (stricmp (szType, "camera") == 0)
    {

        //
        // Create a new camera node
        //

        pNode = new CNwnMdlCameraNode (szName);
    }

    //
    // If this is a referece node
    //

    else if (stricmp (szType, "reference") == 0)
    {

        //
        // Create a new reference node
        //

        pNode = new CNwnMdlReferenceNode (szName);
    }

    //
    // If this is a trimesh node
    //

    else if (stricmp (szType, "trimesh") == 0)
    {

        //
        // Create a new tri mesh node
        //

        pNode = new CNwnMdlTriMeshNode (szName);
    }

    //
    // If this is a skin mesh node
    //

    else if (stricmp (szType, "skin") == 0)
    {

        //
        // Create a new skin mesh node
        //

        pNode = new CNwnMdlSkinMeshNode (szName);
    }

    //
    // If this is an animation mesh node
    //

    else if (stricmp (szType, "animmesh") == 0)
    {

        //
        // Create a new anim mesh node
        //

        pNode = new CNwnMdlAnimMeshNode (szName);
    }

    //
    // If this is a danglymesh node
    //

    else if (stricmp (szType, "danglymesh") == 0)
    {

        //
        // Create a new dangly mesh node
        //

        pNode = new CNwnMdlDanglyMeshNode (szName);
    }

    //
    // If this is an AABB mesh node
    //

    else if (stricmp (szType, "aabb") == 0)
    {

        //
        // Create a new aabb mesh node
        //

        pNode = new CNwnMdlAABBMeshNode (szName);
    }

    //
    // Otherwise, we treat as a dummy node
    //

    else
    {

        //
        // Issue a friendly warning
        //

        if (stricmp (szType, "dummy") != 0)
            pCtx ->GenerateWarning ("Unknown node type, assuming dummy.");

        //
        // Create a new dummy node
        //

        pNode = new CNwnMdlDummyNode (szName);
    }

    //
    // Test for no node
    //

    if (pNode == NULL)
    {
        pCtx ->GenerateError ("Unable to create node");
        return;
    }

    //
    // For a mesh, create the temp structure
    //

    if (pNode ->m_ulFlags & NwnMdlNF_HasMesh)
    {
        CNwnMdlMeshHeader *pMesh = static_cast <CNwnMdlMeshHeader *> (pNode);
        pMesh ->m_pvTempMeshData = new NmcMeshTemp;
    }

    //
    // Set the parent geometry address
    //

    pNode ->m_pGeometryHeader .SetData (
        pCtx ->GetCurrentGeometry ());

    //
    // Parset the lines
    //

    for (;;)
    {

        //
        // Read the next line
        //

        if (!pCtx ->ReadNextLine ())
        {
            pCtx ->GenerateError ("Unexpected end of file, \"endnode\" not found");
            return;
        }

        //
        // Get the token
        //

        char *pszToken = pCtx ->GetNextToken ();
        if (pszToken == NULL)
            continue;

        //
        // If this is an end node, break
        //

        if (stricmp (pszToken, "endnode") == 0)
            break;

        //
        // Place the token back on the stack (will probably change this)
        //

        pCtx ->UnGetToken ();

        //
        // Allow for the controllers and attributes to parse
        //

        if (NmcParseController (pCtx, pNode) ||
            NmcParseAttribute (pCtx, pNode))
        {
            // woot
        }
    }

    //
    // Set the part number
    //

    pNode ->m_nPartNumber = pCtx ->GetCurrentGeometry () ->m_ulNodeCount++;

    //
    // If this was an emitter node
    //

    if (pNode ->m_ulFlags == NwnMdlNT_Emitter)
    {

        //
        // Set the radius to a minimum
        //

        CNwnMdlGeometryHeader *pGeometry = pNode ->m_pGeometryHeader .GetData ();
        if (pGeometry ->m_ucGeometryType == NwnMdlGT_Model)
        {
            CNwnMdlModel *pModel = static_cast <CNwnMdlModel *> (pGeometry);
            if (pModel ->m_fRadius < 40.0f)
                pModel ->m_fRadius = 40.0f;
        }
    }

    //
    // If this was a light node
    //

    else if (pNode ->m_ulFlags == NwnMdlNT_Light)
    {
        // Runtime only
    }

    //
    // If this was a mesh
    //

    else if ((pNode ->m_ulFlags & NwnMdlNF_HasMesh) != 0)
    {
        NmcPostMesh (pCtx, static_cast <CNwnMdlMeshHeader *> (pNode));
    }
    return;
}

//-----------------------------------------------------------------------------
//
// @func Parse a model geometry line
//
// @parm CNmcContext * | pCtx | Parser context
//
// @parm CNwnMdlModel * | pModel | Current model
//
// @parm char char * | pszToken | Current token
//
// @rdesc If true, this routine understood the token.
//
//-----------------------------------------------------------------------------

bool NmcParseModelGeometry (CNmcContext *pCtx, 
                            CNwnMdlModel *pModel, const char *pszToken)
{

    //
    // If this is the radius
    //

    if (stricmp (pszToken, "radius") == 0)
    {
        NmcGetFloatValue (pCtx, &pModel ->m_fRadius, pModel ->m_fRadius, 0);
        return true;
    }

    //
    // If this is the bounding min
    //

    else if (stricmp (pszToken, "bmin") == 0)
    {
        NmcGetVector3Value (pCtx, &pModel ->m_vBMin, pModel ->m_vBMin, 0);
        return true;
    }

    //
    // If this is the bounding max
    //

    else if (stricmp (pszToken, "bmax") == 0)
    {
        NmcGetVector3Value (pCtx, &pModel ->m_vBMax, pModel ->m_vBMax, 0);
        return true;
    }

    //
    // Otherwise, we don't know what this is
    //

    else
        return false;
}

//-----------------------------------------------------------------------------
//
// @func Parse an animation geometry line
//
// @parm CNmcContext * | pCtx | Parser context
//
// @parm CNwnMdlAnimation * | pAnimation | Current animation
//
// @parm char char * | pszToken | Current token
//
// @rdesc If true, this routine understood the token.
//
//-----------------------------------------------------------------------------

bool NmcParseAnimationGeometry (CNmcContext *pCtx, 
                                CNwnMdlAnimation *pAnimation, const char *pszToken)
{

    //
    // If this is the animation length
    //

    if (stricmp (pszToken, "animlength") == 0)
    {
        char szName [64];
        NmcGetStringValue (pCtx, szName, 
                           _countof (szName), NULL, 0);
        NmcGetFloatValue (pCtx, &pAnimation ->m_fLength, 
                          pAnimation ->m_fLength, 0);
        return true;
    }

    //
    // If this is the animantion length (style #2)
    //

    else if (stricmp (pszToken, "length") == 0)
    {
        NmcGetFloatValue (pCtx, &pAnimation ->m_fLength, 
                          pAnimation ->m_fLength, 0);
        return true;
    }

    //
    // If this is the trans time
    //

    else if (stricmp (pszToken, "transtime") == 0)
    {
        NmcGetFloatValue (pCtx, &pAnimation ->m_fTransTime, 
                          pAnimation ->m_fTransTime, 0);
        return true;
    }

    //
    // If this is the animation root
    //

    else if (stricmp (pszToken, "animroot") == 0)
    {
        NmcGetStringValue (pCtx, pAnimation ->m_szAnimRoot,
                           _countof (pAnimation ->m_szAnimRoot), NULL, 0);
        return true;
    }

    //
    // If this is an event
    //

    else if (stricmp (pszToken, "event") == 0)
    {

        //
        // Parse the event elements
        //

        NwnMdlAnimationEvent sAEvent;
        if (NmcGetFloatValue (pCtx, &sAEvent .fAfter, 0.0f, 0) &&
            NmcGetStringValue (pCtx, sAEvent .szEventName, 
                               _countof (sAEvent .szEventName), NULL, 0))
        {

            //
            // Locate an insert spot
            //

            size_t nIndex;
            for (nIndex = 0; nIndex < pAnimation ->m_asEvents .GetCount (); nIndex++)
            {
                if (pAnimation ->m_asEvents [nIndex] .fAfter > sAEvent .fAfter)
                    break;
            }

            //
            // Insert
            //

            pAnimation ->m_asEvents .InsertAt (nIndex, &sAEvent);
        }
    }

    //
    // Otherwise, we don't understand
    //

    else
        return false;
    return false;
}

//-----------------------------------------------------------------------------
//
// @func Merge the part numbers for a node
//
// @parm CNwnMdlNodeHeader * | pDestin | Destination node
//
// @parm CNwnMdlNodeHeader * | pSource | Source node
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcMergeNodePartNumbers (
    CNwnMdlNodeHeader *pDestin, CNwnMdlNodeHeader *pSource)
{

    //
    // At this point, we know the nodes match, so just copy the part number
    //

    pDestin ->m_nPartNumber = pSource ->m_nPartNumber;

    //
    // Loop through the children in a NxM method to merge the children
    //

    CNwnPointer <CNwnMdlNodeHeader> *ppDChildren = 
        pDestin ->m_apChildren .GetData ();
    CNwnPointer <CNwnMdlNodeHeader> *ppSChildren = 
        pSource ->m_apChildren .GetData ();
    for (int i = 0; i < (int) pDestin ->m_apChildren .GetCount (); i++)
    {
        CNwnMdlNodeHeader *pDC = ppDChildren [i] .GetData ();
        pDC ->m_nPartNumber = -1;
        for (int j = 0; j < (int) pSource ->m_apChildren .GetCount (); j++)
        {
            CNwnMdlNodeHeader *pSC = ppSChildren [j] .GetData ();
            if (stricmp (pDC ->m_szName, pSC ->m_szName) == 0)
            {
                NmcMergeNodePartNumbers (pDC, pSC);
                break;
            }
        }
    }
    return;
}

//-----------------------------------------------------------------------------
//
// @func Merge the part numbers for a two geometries
//
// @parm CNwnMdlGeometryHeader * | pDestin | Destination node
//
// @parm CNwnMdlGeometryHeader * | pSource | Source node
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcMergeGeometryPartNumbers (
    CNwnMdlGeometryHeader *pDestin, 
    CNwnMdlGeometryHeader *pSource)
{

    //
    // If we have a source
    //

    if (pSource != NULL && pSource ->m_pGeometry .GetData () != NULL &&
        pDestin != NULL && pDestin ->m_pGeometry .GetData () != NULL)
    {
        NmcMergeNodePartNumbers (
            pDestin ->m_pGeometry .GetData (),
            pSource ->m_pGeometry .GetData ());
    }
    return;
}

//-----------------------------------------------------------------------------
//
// @func Parse a goemetry block
//
// @parm CNmcContext * | pCtx | Parser context
//
// @parm CNwnMdlGeometryHeader * | pGeometry | Current geometry
//
// @parm char char * | pszEndText | Text to end the block
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcParseGeometry (CNmcContext *pCtx, 
                       CNwnMdlGeometryHeader *pGeometry, const char *pszEndText)
{

    //
    // Save the geometry
    //

    pCtx ->SetCurrentGeometry (pGeometry);

    //
    // Loop while there are lines to process
    //

    while (pCtx ->ReadNextLine ())
    {

        //
        // Get the token
        //

        char *pszToken = pCtx ->GetNextToken ();
        if (pszToken == NULL)
            continue;

        //
        // If this is the end of the model
        //

        if (stricmp (pszToken, pszEndText) == 0)
            return;

        //
        // If this is a model, try model specifics
        //

        if ((pGeometry ->m_ucGeometryType & NwnMdlGF_HasModel) != 0)
        {
            if (NmcParseModelGeometry (pCtx, 
                                       (CNwnMdlModel *) pGeometry, pszToken))
                continue;
        }

        //
        // If this is an animation, try animation specifics
        //

        if ((pGeometry ->m_ucGeometryType & NwnMdlGF_HasAnimation) != 0)
        {
            if (NmcParseAnimationGeometry (pCtx, 
                                           (CNwnMdlAnimation *) pGeometry, pszToken))
                continue;
        }

        //
        // If this a node?
        //

        if (stricmp (pszToken, "node") == 0)
        {
            NmcBeginNode (pCtx);
        }
    }

    //
    // End of file
    //

    pCtx ->GenerateError ("Unexpected end of file, \"%s\" not found", pszEndText);
    return;
}

//-----------------------------------------------------------------------------
//
// @func Compare two nodes
//
// @parm const void * | p1 | First node to compare
//
// @parm const void * | p2 | Second node to compare
//
// @rdesc Results
//
//-----------------------------------------------------------------------------

int __cdecl NmcTransparencyComp (const void *p1, const void *p2)
{
    CNwnMdlMeshHeader *pM1 = *((CNwnMdlMeshHeader **) p1);
    CNwnMdlMeshHeader *pM2 = *((CNwnMdlMeshHeader **) p2);

    //
    // Compare the flags
    //

    if ((pM1 ->m_ulFlags & NwnMdlNF_HasMesh) == 0)
    {
        if ((pM2 ->m_ulFlags & NwnMdlNF_HasMesh) == 0)
            return 0;
        else
            return -1;
    }
    if ((pM2 ->m_ulFlags & NwnMdlNF_HasMesh) == 0)
        return 1;

    //
    // Compare the hint
    //

    if (pM1 ->m_ulTransparencyHint == 0)
    {
        if (pM2 ->m_ulTransparencyHint == 0)
            return 0;
        else
            return -1;
    }
    if (pM2 ->m_ulTransparencyHint == 0)
        return 1;
    return 0;
}

//-----------------------------------------------------------------------------
//
// @func Sort the transparency 
//
// @parm CNwnMdlNodeHeader * | pNode | Node to sort
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcSortTransparency (CNwnMdlNodeHeader *pNode)
{

    //
    // Return if NULL
    //

    if (pNode == NULL)
        return;

    //
    // Get the child list
    //

    int nChildren = (int) pNode ->m_apChildren .GetCount ();
    CNwnPointer <CNwnMdlNodeHeader> *ppChildren = pNode ->m_apChildren .GetData ();

    //
    // See if we have any transparency
    //

    bool fHasTrans = false;
    bool fHasNonTransAfter = false;
    for (int i = 0; i < nChildren; i++)
    {

        //
        // If this node is transparent
        //

        bool fIsTrans = false;
        CNwnMdlNodeHeader *pNode = ppChildren [i] .GetData ();
        if ((pNode ->m_ulFlags & NwnMdlNF_HasMesh) != 0)
        {
            CNwnMdlMeshHeader *pMesh = static_cast <CNwnMdlMeshHeader *> (pNode);
            fIsTrans = pMesh ->m_ulTransparencyHint > 0;
        }

        //
        // Set transparent flag
        //

        if (fIsTrans)
            fHasTrans = true;
        else if (fHasTrans)
            fHasNonTransAfter = true;
    }

    //
    // If we have a non-transparent after a transparent, then sort
    //

    if (fHasNonTransAfter)
    {
        qsort (ppChildren, nChildren, 
               sizeof (CNwnMdlNodeHeader *), NmcTransparencyComp);
    }

    //
    // Invoke on children
    //

    for (int i = 0; i < nChildren; i++)
    {
        CNwnMdlNodeHeader *pNode = ppChildren [i] .GetData ();
        NmcSortTransparency (pNode);
    }
}

//-----------------------------------------------------------------------------
//
// @func Handle post geometry processing
//
// @parm CNmcContext * | pCtx | Parser context
//
// @parm CNwnMdlNodeHeader * | pNode | Current node
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcPostGeometry (CNmcContext *pCtx, CNwnMdlNodeHeader *pNode)
{

    //
    // If this is a mesh, invoke the mesh routine
    //

    if ((pNode ->m_ulFlags & NwnMdlNF_HasMesh) != 0)
    {
        NmcPostGeometryMesh (pCtx, static_cast <CNwnMdlMeshHeader *> (pNode));
    }

    //
    // Invoke this routine on the children
    //

    for (int i = 0; i < (int) pNode ->m_apChildren .GetCount (); i++)
    {
        CNwnMdlNodeHeader *pChild = pNode ->m_apChildren [i] .GetData ();
        NmcPostGeometry (pCtx, pChild);
    }
}

//-----------------------------------------------------------------------------
//
// @func Parse a model block
//
// @parm CNmcContext * | pCtx | Parser context
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcParseModel (CNmcContext *pCtx)
{
    //
    // Get the model
    //

    CNwnMdlModel *pModel = pCtx ->GetCurrentModel ();

    //
    // Loop while there are lines to process
    //

    while (pCtx ->ReadNextLine ())
    {

        //
        // Get the token
        //

        char *pszToken = pCtx ->GetNextToken ();
        if (pszToken == NULL)
            continue;

        //
        // If this is the end of the model
        //

        if (stricmp (pszToken, "donemodel") == 0)
            return;

        //
        // If this is set animation scale
        //

        if (stricmp (pszToken, "setanimationscale") == 0)
        {

            //
            // Get the value
            //

            NmcGetFloatValue (pCtx, &pModel ->m_fAnimationScale, 1.0f, 0);
        }

        //
        // If this is set super model
        //

        else if (stricmp (pszToken, "setsupermodel") == 0)
        {

            //
            // If we can get the two names
            //

            char szModel [64];
            char szSuperModel [64];
            if (NmcGetStringValue (pCtx, szModel, _countof (szModel), NULL, 0) &&
                NmcGetStringValue (pCtx, szSuperModel, _countof (szSuperModel), NULL, 0))
            {

                //
                // Warn on mismatch of model name
                //

                if (stricmp (szModel, pModel ->m_szName) != 0)
                    pCtx ->GenerateWarning ("Model name mismatch in setsupermodel statement");

                //
                // Issue warning if we already have a super model
                //

                if (pModel ->m_pSuperModel .GetData () != NULL)
                {
                    pCtx ->GenerateWarning ("Supermodel already defined for model");
                    pModel ->m_pSuperModel .GetData () ->Release ();
                    pModel ->m_pSuperModel .SetData (NULL);
                }

                //
                // Save the super model name
                //

                strncpy (pModel ->m_szSuperModel, szSuperModel, 
                         _countof (pModel ->m_szSuperModel));
                pModel ->m_szSuperModel [_countof (pModel ->m_szSuperModel) - 1] = 0;

                //
                // Try to load the model
                //

                if (szSuperModel [0] != 0 && stricmp (szSuperModel, "NULL") != 0)
                {
                    CNwnMdlModel *pSuperModel = pCtx ->LoadModel (szSuperModel);
                    if (pSuperModel == NULL)
                    {
                        pCtx ->GenerateError ("Unable to locate "
                                              "super model \"%s\"", szSuperModel);
                    }
                    else
                    {
                        pModel ->m_pSuperModel .SetData (pSuperModel);
                        pSuperModel ->AddRef ();
                    }
                }
            }
        }

        //
        // If this is ignore model
        //

        else if (stricmp (pszToken, "ignorefog") == 0)
        {

            //
            // Get the fog value
            //

            int fIgnoreFog;
            if (NmcGetIntegerValue (pCtx, &fIgnoreFog, 0, 0))
                pModel ->m_ucFog = fIgnoreFog != 0;
        }

        //
        // If this is the classification
        //

        else if (stricmp (pszToken, "classification") == 0)
        {

            //
            // Get the classification
            //

            char szClass [64];
            if (NmcGetStringValue (pCtx, szClass, _countof (szClass), NULL, 0))
            {
                if (stricmp (szClass, "effect") == 0)
                    pModel ->m_ucClassification = NwnMdlCls_Effect;
                else if (stricmp (szClass, "effects") == 0)
                    pModel ->m_ucClassification = NwnMdlCls_Effect;
                else if (stricmp (szClass, "tile") == 0)
                    pModel ->m_ucClassification = NwnMdlCls_Tile;
                else if (stricmp (szClass, "character") == 0)
                    pModel ->m_ucClassification = NwnMdlCls_Character;
                else if (stricmp (szClass, "door") == 0)
                    pModel ->m_ucClassification = NwnMdlCls_Door;
                else
                    pCtx ->GenerateWarning ("Unrecognized classification");
            }
        }

        //
        // If this is the begin model geometry
        //

        else if (stricmp (pszToken, "beginmodelgeom") == 0)
        {

            //
            // Validate the name
            //

            char szModel [64];
            if (NmcGetStringValue (pCtx, szModel, _countof (szModel), NULL, 0))
            {
                if (stricmp (szModel, pModel ->m_szName) != 0)
                {
                    pCtx ->GenerateWarning ("Model name mismatch"
                                            "in beginmodelgeom statement");
                }
            }

            //
            // Initialize the node count
            //

            if (pModel ->m_pSuperModel .GetData () != NULL)
            {
                pModel ->m_ulNodeCount = pModel ->
                    m_pSuperModel .GetData () ->m_ulNodeCount + 1;
            }

            //
            // Parse the geometry
            //

            NmcParseGeometry (pCtx, pModel, "endmodelgeom");

            //
            // Handle post geometry processing
            //

            NmcPostGeometry (pCtx, pModel ->m_pGeometry .GetData ());

            //
            // Sort the transparency hints
            //

            NmcSortTransparency (pModel ->m_pGeometry .GetData ());

            //
            // Merge the trees
            //

            if (pModel ->m_pSuperModel .GetData () != NULL)
            {
                NmcMergeGeometryPartNumbers (pModel,
                                             pModel ->m_pSuperModel .GetData ());
            }
        }

        //
        // If this is a new animation
        //

        else if (stricmp (pszToken, "newanim") == 0)
        {

            //
            // If we can get the two names
            //

            char szModel [64];
            char szAnim [64];
            if (NmcGetStringValue (pCtx, szAnim, _countof (szAnim), NULL, 0) &&
                NmcGetStringValue (pCtx, szModel, _countof (szModel), NULL, 0))
            {

                //
                // Warn on mismatch of model name
                //

                if (stricmp (szModel, pModel ->m_szName) != 0)
                    pCtx ->GenerateWarning ("Model name mismatch in newanim statement");

                //
                // Create the new animation
                //

                CNwnMdlAnimation *pAnimation = new CNwnMdlAnimation (szAnim);

                //
                // Add the animation to the list
                //

                pModel ->m_apAnimations .Add (pAnimation);

                //
                // Parse the geometry
                //

                NmcParseGeometry (pCtx, pAnimation, "doneanim");

                //
                // Handle post geometry processing
                //

                NmcPostGeometry (pCtx, pAnimation ->m_pGeometry .GetData ());

                //
                // Merge the part numbers
                //

                NmcMergeGeometryPartNumbers (pAnimation, pModel);
            }
        }
    }

    //
    // End of file
    //

    pCtx ->GenerateError ("Unexpected end of file, \"donemodel\" not found");
    return;
}

//-----------------------------------------------------------------------------
//
// @func Parse a model file
//
// @parm CNmcContext * | pCtx | Parser context
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void NmcParseModelFile (CNmcContext *pCtx)
{

    //
    // Loop while there are lines to process
    //

    while (pCtx ->ReadNextLine ())
    {

        //
        // Get the token
        //

        char *pszToken = pCtx ->GetNextToken ();
        if (pszToken == NULL)
            continue;

        //
        // If this is a new model token
        //

        if (stricmp (pszToken, "newmodel") == 0)
        {

            //
            // If we already have a model, issue an error
            //

            if (pCtx ->GetCurrentModel () != NULL)
            {
                pCtx ->GenerateError ("Multiple models found");
                continue;
            }

            //
            // Get the name of the model
            //

            char *pszName = pCtx ->GetNextToken ();
            if (pszName == NULL)
            {
                pCtx ->GenerateError ("No model name specified");
                continue;
            }

            //
            // Create the new model
            //

            CNwnMdlModel *pModel = new CNwnMdlModel (pszName);
            pCtx ->SetCurrentModel (pModel);

            //
            // Parse the model
            //

            NmcParseModel (pCtx);
        }
    }
    return;
}


/* vim: set sw=4: */
