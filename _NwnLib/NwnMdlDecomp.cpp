//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnMdlDecomp.cpp - Decompiler support |
//
// This module contains the Nwn model decompiler.
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

#include "NwnMdlGeometry.h"
#include "NwnMdlNodes.h"
#include "../_MathLib/Constants.h"
#include "../_NmcLib/Nmc.h"

//
// Local structures
//

struct VertRemap
{
    int nVert;
    int nTVerts [4];
    int nVertBack;
    int nTVertsBack [4];
};

struct NodeSort
{
    CNwnMdlNodeHeader *pNode;
    CNwnMdlNodeHeader *pParent;
    int nIndex;
};

//
// Local routines
//

void ControllerRowDecomp (CNwnStream *pStream, float *pfValues,
                          int nColumns, bool bColumnize);
void AABBEntryDecomp (CNwnStream *pStream, 
                      NwnMdlAABBEntry *pEntry, unsigned char *pauchModelData, 
                      int nDepth);
int FindRemapVertex (int nVert, VertRemap *pRemap, int &nRemapVerts,
                     CVector3 *pVerts, CVector3 *pNormals, UINT32 *pulColors, 
                     float *pConstraints, int nV1x, int nV2x);
int FindRemapTVertex (int nTex, int nTVert, VertRemap *pRemap, 
                      int &nRemapVerts, CVector2 *pTVerts, int nV1x, int nV2x);
int CollectNodes (CNwnMdlNodeHeader *pNode, NodeSort *pList, 
                  CNwnMdlNodeHeader *pParentNode, int nIndex, 
                  unsigned char *pauchModelData, int nPart);
void GenerateSmoothMasks (int nFace, NwnMdlFace *pFaces,
                          int nFaceCount, CVector3 *paVerts, CVector3 *paVertNormals,
                          int *panSmoothSets, int nSet, unsigned long *pulConflicts);
CNwnMdlNodeHeader *GetNthNode (CNwnMdlNodeHeader *pNode, int &nIndex, 
                               unsigned char *pauchModelData);

//-----------------------------------------------------------------------------
//
// @mfunc Decompile a model
//
// @parm CNwnStream * | pStream | Output stream
//
// @parm CNwnMdlModel * | pModel | Pointer to the
//              model in question.
//
// @parm bool | fIncludeChildren | If true, include the model's children
//
// @parm bool | fIncludeExtended | If true, include extended diagnostic
//              information as comments.
//
// @parm bool | fIncludeSmoothing | If true, include smoothing masks
//
// @parm unsigned char * | pauchModelData | Pointer to the model data or
//              NULL if this is an absolute model.
//
// @parm unsigned char * | pauchRawData | Pointer to the raw data or
//              NULL if this is an absolute model.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlModel::Decompile (CNwnStream *pStream, bool fIncludeChildren, 
                              bool fIncludeExtended, bool fIncludeSmoothing, unsigned char *pauchModelData, 
                              unsigned char *pauchRawData)
{
    const char *pszText;
    char szText [512];

    //
    // Generate the header lines
    //

    pStream ->WriteLine ("#MAXMODEL ASCII");

    snprintf (szText, _countof (szText), 
              "# model: %s", m_szName);
    pStream ->WriteLine (szText);

    snprintf (szText, _countof (szText), 
              "filedependancy %s.max", m_szName);
    pStream ->WriteLine (szText);

    snprintf (szText, _countof (szText), 
              "newmodel %s", m_szName);
    pStream ->WriteLine (szText);

    pszText = m_szSuperModel;
    if (pszText [0] == 0)
        pszText = "NULL";
    snprintf (szText, _countof (szText),
              "setsupermodel %s %s", m_szName, pszText);
    pStream ->WriteLine (szText);

    switch (m_ucClassification)
    {
        case NwnMdlCls_Effect:
            pszText = "EFFECT";
            break;
        case NwnMdlCls_Tile:
            pszText = "TILE";
            break;
        case NwnMdlCls_Character:
            pszText = "CHARACTER";
            break;
        case NwnMdlCls_Door:
            pszText = "DOOR";
            break;
        default:
            pszText = NULL;
            break;
    }
    if (pszText)
    {
        snprintf (szText, _countof (szText),
                  "classification %s", pszText);
        pStream ->WriteLine (szText);
    }

    snprintf (szText, _countof (szText),
              "setanimationscale %.7g", m_fAnimationScale);
    pStream ->WriteLine (szText);

    if (m_ucFog == 0)
        pStream ->WriteLine ("ignorefog 1");

    //
    // Add in the geometry
    //

    if (m_pGeometry .GetData () != NULL)
    {

        //
        // Write geometry start
        //

        pStream ->WriteLine ("#MAXGEOM  ASCII");
        snprintf (szText, _countof (szText),
                  "beginmodelgeom %s", m_szName);
        pStream ->WriteLine (szText);

        //
        // If we are to include children
        //

        if (fIncludeChildren)
        {

            //
            // If we don't have a supermodel, then collect the children to 
            // write
            //

            if (m_szSuperModel [0] == 0 || stricmp (m_szSuperModel, "NULL") == 0)
            {

                //
                // Collect the nodes
                //

                NodeSort *pList = (NodeSort *)
                    alloca (sizeof (NodeSort) * m_ulNodeCount);
                memset (pList, 0, sizeof (NodeSort) * m_ulNodeCount);
                CollectNodes (m_pGeometry .GetData (pauchModelData), pList, 
                              NULL, 0, pauchModelData, 0);

                //
                // Serialize
                //

                for (int i = 0; i < (int) m_ulNodeCount; i++)
                {
                    if (pList [i] .pNode != NULL)
                    {
                        pList [i] .pNode ->Decompile (pStream, this, this, 
                                                      pList [i] .pParent, false, fIncludeExtended, 
                                                      fIncludeSmoothing, pauchModelData, pauchRawData);
                    }
                }
            }

            //
            // Otherwise, list nodes serialize children
            //

            else
            {
                CNwnMdlNodeHeader *pNode = 
                    m_pGeometry .GetData (pauchModelData);
                pNode ->Decompile (pStream, this, this, 
                                   NULL, true, fIncludeExtended, fIncludeSmoothing,
                                   pauchModelData, pauchRawData);
            }
        }

        //
        // Otherwise, just add a note
        //

        else
        {
            pStream ->WriteLine ("#GEOMETRY OMITTED");
        }

        //
        // Write geometry end
        //

        snprintf (szText, _countof (szText),
                  "endmodelgeom %s", m_szName);
        pStream ->WriteLine (szText);
    }

    //
    // If we have animations
    //

    if (m_apAnimations .GetCount () > 0)
    {
        //
        // If we are to include children
        //

        if (fIncludeChildren)
        {
            CNwnPointer <CNwnMdlAnimation> *ppAnimations = 
                m_apAnimations .GetData (pauchModelData);
            for (int i = 0; i < (int) m_apAnimations .GetCount (); i++)
            {
                CNwnMdlAnimation *pAnimation = ppAnimations 
                    [i] .GetData (pauchModelData);
                pAnimation ->Decompile (pStream, this, 
                                        true, fIncludeExtended, fIncludeSmoothing,
                                        pauchModelData, pauchRawData);
            }
        }

        //
        // Otherwise, just add a note
        //

        else
        {
            pStream ->WriteLine ("");
            pStream ->WriteLine ("#ANIMATIONS OMITTED");
        }
    }

    //
    // End the model
    //

    pStream ->WriteLine ("");
    snprintf (szText, _countof (szText),
              "donemodel %s", m_szName);
    pStream ->WriteLine (szText);
}

//-----------------------------------------------------------------------------
//
// @mfunc Decompile an animation
//
// @parm CNwnStream * | pStream | Output stream
//
// @parm CNwnMdlModel * | pModel | Pointer to the
//              model in question.
//
// @parm CNwnMdlAnimation * | pAnimation | Pointer to the
//              animation in question.
//
// @parm bool | fIncludeChildren | If true, include the animation's children
//
// @parm bool | fIncludeExtended | If true, include extended diagnostic
//              information as comments.
//
// @parm bool | fIncludeSmoothing | If true, include smoothing masks
//
// @parm unsigned char * | pauchModelData | Pointer to the model data or
//              NULL if this is an absolute model.
//
// @parm unsigned char * | pauchRawData | Pointer to the raw data or
//              NULL if this is an absolute model.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlAnimation::Decompile (CNwnStream *pStream, CNwnMdlModel *pModel,
                                  bool fIncludeChildren, bool fIncludeExtended, bool fIncludeSmoothing, 
                                  unsigned char *pauchModelData, unsigned char *pauchRawData)
{
    char szText [512];

    //
    // Generate the header lines
    //

    pStream ->WriteLine ("");
    pStream ->WriteLine ("#MAXANIM ASCII");

    snprintf (szText, _countof (szText), 
              "newanim %s %s", m_szName, pModel ->m_szName);
    pStream ->WriteLine (szText);

    snprintf (szText, _countof (szText), 
              "  length %.7g", m_fLength);
    pStream ->WriteLine (szText);

    snprintf (szText, _countof (szText), 
              "  transtime %.7g", m_fTransTime);
    pStream ->WriteLine (szText);

    if (m_szAnimRoot [0])
    {
        snprintf (szText, _countof (szText), 
                  "  animroot %s", m_szAnimRoot);
        pStream ->WriteLine (szText);
    }

    //
    // List the events
    //

    if (m_asEvents .GetCount () > 0)
    {
        NwnMdlAnimationEvent *pEvents = 
            m_asEvents .GetData (pauchModelData);
        for (int i = 0; i < (int) m_asEvents .GetCount (); i++)
        {
            snprintf (szText, _countof (szText), 
                      "  event %.7g %s", pEvents [i] .fAfter,
                      pEvents [i] .szEventName);
            pStream ->WriteLine (szText);
        }
    }

    //
    // Add in the geometry
    //

    if (m_pGeometry .GetData () != NULL)
    {

        //
        // If we are to include children
        //

        if (fIncludeChildren)
        {
            CNwnMdlNodeHeader *pNode = 
                m_pGeometry .GetData (pauchModelData);
            pStream ->SetLinePrefix("  ");
            pNode ->Decompile (pStream, pModel, this, 
                               NULL, true, fIncludeExtended, fIncludeSmoothing,
                               pauchModelData, pauchRawData);
            pStream ->SetLinePrefix(NULL);
        }

        //
        // Otherwise, just add a note
        //

        else
        {
            pStream ->WriteLine ("#GEOMETRY OMITTED");
        }
    }

    //
    // End the animation
    //

    snprintf (szText, _countof (szText),
              "doneanim %s %s", m_szName, 
              pModel ->m_szName);
    pStream ->WriteLine (szText);
}

//-----------------------------------------------------------------------------
//
// @func Decompile a node
//
// @parm CNwnStream * | pStream | Output stream
//
// @parm CNwnMdlModel * | pModel | Pointer to the
//              model in question.
//
// @parm CNwnMdlGeometryHeader * | pParentGeom | Parent geometry
//
// @parm CNwnMdlNodeHeader * | pParentNode | Parent node (NULL if ROOT)
//
// @parm bool | fIncludeChildren | If true, include the animation's children
//
// @parm bool | fIncludeExtended | If true, include extended diagnostic
//              information as comments.
//
// @parm bool | fIncludeSmoothing | If true, include smoothing masks
//
// @parm unsigned char * | pauchModelData | Pointer to the model data or
//              NULL if this is an absolute model.
//
// @parm unsigned char * | pauchRawData | Pointer to the raw data or
//              NULL if this is an absolute model.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNwnMdlNodeHeader::Decompile (CNwnStream *pStream, CNwnMdlModel *pModel,
                                   CNwnMdlGeometryHeader *pParentGeom, CNwnMdlNodeHeader *pParentNode, 
                                   bool fIncludeChildren, bool fIncludeExtended, bool fIncludeSmoothing, 
                                   unsigned char *pauchModelData, unsigned char *pauchRawData)
{
    const char *pszText;
    char szText [512];
    VertRemap sRemapCounts;
    VertRemap *pRemap = NULL;
    int *panSmoothSets = NULL;

    //
    // Get rid of a 4701 warning about uninitialized value
    //

    memset (&sRemapCounts, 0, sizeof (sRemapCounts));

    //
    // Make sure the root flag is set
    //

    assert ((m_ulFlags & NwnMdlNF_HasHeader) != 0);

    //
    // See if this is part of an animation
    //

    bool fAnimation = (pParentGeom ->m_ucGeometryType & NwnMdlGF_HasAnimation) != 0;

    //
    // Generate the node start line
    //

    switch (m_ulFlags)
    {
        case NwnMdlNT_Dummy:
            pszText = "dummy";
            break;
        case NwnMdlNT_Light:
            pszText = "light";
            break;
        case NwnMdlNT_Emitter:
            pszText = "emitter";
            break;
        case NwnMdlNT_Camera:
            pszText = "camera";
            break;
        case NwnMdlNT_Reference:
            pszText = "reference";
            break;
        case NwnMdlNT_TriMesh:
            pszText = "trimesh";
            break;
        case NwnMdlNT_SkinMesh:
            pszText = "skin";
            break;
        case NwnMdlNT_AnimMesh:
            pszText = "animmesh";
            break;
        case NwnMdlNT_DanglyMesh:
            pszText = "danglymesh";
            break;
        case NwnMdlNT_AABBMesh:
            pszText = "aabb";
            break;
        default:
            pszText = "unknown";
            break;
    }

    if (strlen(m_szName) < 1) {
        if (pParentNode == NULL) {
            snprintf (m_szName, _countof (m_szName), "%s", pModel ->m_szName);
            snprintf (szText, _countof (szText), "node %s %s", pszText, m_szName);
        } else
        snprintf (szText, _countof (szText), "node %s nmc_%p", pszText, this);
    } else
        snprintf (szText, _countof (szText), "node %s %s", pszText, m_szName);

    pStream ->WriteLine (szText);

    //
    // Format the parent
    //

    if (pParentNode != NULL)
        pszText = pParentNode ->m_szName;
    else
        pszText = "NULL";

    if (strlen(pszText) < 1)
        snprintf (szText, _countof (szText), "  parent nmc_%p", pParentNode);
    else
        snprintf (szText, _countof (szText), "  parent %s", pszText);
    pStream ->WriteLine (szText);

    //
    // Save the part number
    //

    if (fIncludeExtended)
    {
        snprintf (szText, _countof (szText), 
                  "  #part-number %d", m_nPartNumber);
        pStream ->WriteLine (szText);
    }

    //
    // Format the common attributes
    //

    if (m_ulInheritColor != 0)// && !fAnimation)
    {
        snprintf (szText, _countof (szText), 
                  "  inheritcolor %d", m_ulInheritColor);
        pStream ->WriteLine (szText);
    }

    //
    // If we have light data
    //

    if ((m_ulFlags & NwnMdlNF_HasLight) != 0 && !fAnimation)
    {
        CNwnMdlLightNode *pLight = 
            static_cast <CNwnMdlLightNode *> (this);

        snprintf (szText, _countof (szText),
                  "  ambientonly %d", pLight ->m_ulAmbientOnly);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText),
                  "  shadow %d", pLight ->m_ulShadow);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText),
                  "  isdynamic %d", pLight ->m_ulDynamicType);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText),
                  "  affectdynamic %d", pLight ->m_ulAffectDynamic);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText),
                  "  lightpriority %d", pLight ->m_ulLightPriority);
        pStream ->WriteLine (szText);

        if (pLight ->m_ulGenerateFlare != 0)
        {
            snprintf (szText, _countof (szText),
                      "  generateflare %d", pLight ->m_ulGenerateFlare);
            pStream ->WriteLine (szText);
        }

        snprintf (szText, _countof (szText),
                  "  fadingLight %d", pLight ->m_ulFadingLight);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText),
                  "  flareradius %.7g", pLight ->m_fFlareRadius);
        pStream ->WriteLine (szText);

        if (pLight ->m_apszTextureNames .GetCount () > 0)
        {
            CNwnPointer <char> *ppNames = pLight ->
                m_apszTextureNames .GetData (pauchModelData);
            snprintf (szText, _countof (szText),
                      "  texturenames %ld", (long)(pLight ->m_apszTextureNames .GetCount ()));
            pStream ->WriteLine (szText);
            for (int i = 0; i < (int) pLight ->m_apszTextureNames .GetCount (); i++)
            {
                snprintf (szText, _countof (szText),
                          "    %s", ppNames [i] .GetData (pauchModelData));
                pStream ->WriteLine (szText);
            }
        }

        if (pLight ->m_afFlareSizes .GetCount () > 0)
        {
            float *pfSizes = pLight ->
                m_afFlareSizes .GetData (pauchModelData);
            snprintf (szText, _countof (szText),
                      "  flaresizes %ld", (long)(pLight ->m_afFlareSizes .GetCount ()));
            pStream ->WriteLine (szText);
            for (int i = 0; i < (int) pLight ->m_afFlareSizes .GetCount (); i++)
            {
                snprintf (szText, _countof (szText),
                          "    %.7g", pfSizes [i]);
                pStream ->WriteLine (szText);
            }
        }

        if (pLight ->m_afFlarePositions .GetCount () > 0)
        {
            float *pfPositions = pLight ->
                m_afFlarePositions .GetData (pauchModelData);
            snprintf (szText, _countof (szText),
                      "  flarepositions %ld", (long)(pLight ->m_afFlarePositions .GetCount ()));
            pStream ->WriteLine (szText);
            for (int i = 0; i < (int) pLight ->m_afFlarePositions .GetCount (); i++)
            {
                snprintf (szText, _countof (szText),
                          "    %.7g", pfPositions [i]);
                pStream ->WriteLine (szText);
            }
        }

        if (pLight ->m_avFlareColorShifts .GetCount () > 0)
        {
            CVector3 *pvColorShifts = pLight ->
                m_avFlareColorShifts .GetData (pauchModelData);
            snprintf (szText, _countof (szText),
                      "  flarecolorshifts %ld", (long)(pLight ->m_avFlareColorShifts .GetCount ()));
            pStream ->WriteLine (szText);
            for (int i = 0; i < (int) pLight ->m_avFlareColorShifts .GetCount (); i++)
            {
                snprintf (szText, _countof (szText),
                          "    %12.7f %12.7f %12.7f", pvColorShifts [i] .m_x, 
                          pvColorShifts [i] .m_y, pvColorShifts [i] .m_z);
                pStream ->WriteLine (szText);
            }
        }
    }

    //
    // If we have emitter data
    //

    if ((m_ulFlags & NwnMdlNF_HasEmitter) != 0 && !fAnimation)
    {
        CNwnMdlEmitterNode *pEmitter = 
            static_cast <CNwnMdlEmitterNode *> (this);

        snprintf (szText, _countof (szText), "  p2p %d", 
                  (pEmitter ->m_ulEmitterFlags & NwnMdlEF_P2P) != 0 ? 1 : 0);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText), "  p2p_sel %d", 
                  (pEmitter ->m_ulEmitterFlags & NwnMdlEF_P2PSel) != 0 ? 1 : 0);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText), "  affectedByWind %d", 
                  (pEmitter ->m_ulEmitterFlags & NwnMdlEF_AffectedByWind) != 0 ? 1 : 0);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText), "  m_isTinted %d", 
                  (pEmitter ->m_ulEmitterFlags & NwnMdlEF_IsTinted) != 0 ? 1 : 0);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText), "  bounce %d", 
                  (pEmitter ->m_ulEmitterFlags & NwnMdlEF_Bounce) != 0 ? 1 : 0);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText), "  random %d", 
                  (pEmitter ->m_ulEmitterFlags & NwnMdlEF_Random) != 0 ? 1 : 0);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText), "  inherit %d", 
                  (pEmitter ->m_ulEmitterFlags & NwnMdlEF_Inherit) != 0 ? 1 : 0);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText), "  inheritvel %d", 
                  (pEmitter ->m_ulEmitterFlags & NwnMdlEF_InheritVel) != 0 ? 1 : 0);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText), "  inherit_local %d", 
                  (pEmitter ->m_ulEmitterFlags & NwnMdlEF_InheritLocal) != 0 ? 1 : 0);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText), "  inherit_part %d", 
                  (pEmitter ->m_ulEmitterFlags & NwnMdlEF_InheritPart) != 0 ? 1 : 0);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText), "  splat %d", 
                  (pEmitter ->m_ulEmitterFlags & NwnMdlEF_Splat) != 0 ? 1 : 0);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText),
                  "  renderorder %d", pEmitter ->m_usRenderOrder);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText),
                  "  spawntype %d", pEmitter ->m_ulSpawnType);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText),
                  "  update %s", pEmitter ->m_szUpdate);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText),
                  "  render %s", pEmitter ->m_szRender);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText),
                  "  blend %s", pEmitter ->m_szBlend);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText),
                  "  texture %s", pEmitter ->m_szTexture);
        pStream ->WriteLine (szText);

        if (pEmitter ->m_szChunkName [0])
        {
            snprintf (szText, _countof (szText),
                      "  chunkName %s", pEmitter ->m_szChunkName);
            pStream ->WriteLine (szText);
        }

        snprintf (szText, _countof (szText),
                  "  xgrid %d", pEmitter ->m_ulXGrid);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText),
                  "  ygrid %d", pEmitter ->m_ulYGrid);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText),
                  "  loop %d", pEmitter ->m_ulLoop);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText),
                  "  deadspace %.7g", pEmitter ->m_fDeadSpace);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText),
                  "  twosidedtex %d", pEmitter ->m_ulTwoSidedTex);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText),
                  "  blastRadius %.7g", pEmitter ->m_fBlastRadius);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText),
                  "  blastLength %.7g", pEmitter ->m_fBlastLength);
        pStream ->WriteLine (szText);
    }

    //
    // If we have reference data
    //

    if ((m_ulFlags & NwnMdlNF_HasReference) != 0 && !fAnimation)
    {
        CNwnMdlReferenceNode *pReference = 
            static_cast <CNwnMdlReferenceNode *> (this);

        snprintf (szText, _countof (szText), 
                  "  refModel %s", pReference ->m_szRefModel);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText), 
                  "  reattachable %d", pReference ->m_ulReattachable);
        pStream ->WriteLine (szText);
    }

    //
    // If we have mesh data
    //

    if ((m_ulFlags & NwnMdlNF_HasMesh) != 0)// && !fAnimation)
    {
        CNwnMdlMeshHeader *pMesh = 
            static_cast <CNwnMdlMeshHeader *> (this);

        //
        // Get pointers to arrays we need for vertex compression
        //

        int nFaces = (int) pMesh ->m_sFaces .GetCount ();
        NwnMdlFace *pFaces = pMesh ->m_sFaces .GetData (pauchModelData);
        CVector3 *pVerts = pMesh ->m_pavVerts .GetDataNZ (pauchRawData);
        CVector3 *pNormals = pMesh ->m_pavNormals .GetDataNZ (pauchRawData);
        CVector2 *apTVerts [4];
        apTVerts [0] = pMesh ->m_pavTVerts [0] .GetDataNZ (pauchRawData);
        apTVerts [1] = pMesh ->m_pavTVerts [1] .GetDataNZ (pauchRawData);
        apTVerts [2] = pMesh ->m_pavTVerts [2] .GetDataNZ (pauchRawData);
        apTVerts [3] = pMesh ->m_pavTVerts [3] .GetDataNZ (pauchRawData);
        UINT32 *pulColors = pMesh ->m_paColors .GetDataNZ (pauchRawData);
        float *pConstraints = NULL;
        if (m_ulFlags & NwnMdlNF_HasDangly)
        {
            CNwnMdlDanglyMeshNode *pDangly = 
                static_cast <CNwnMdlDanglyMeshNode *> (this);
            pConstraints = pDangly ->m_afConstraints .GetData (pauchModelData);
        }

        //
        // If we aren't to do smoothing, then don't include normals into
        // the compression of vertices
        //

        CVector3 *pCNormals = fIncludeSmoothing ? pNormals : NULL;

        //
        // Allocate the space for the remap table
        //

        int nRCount = pMesh ->m_usVertexCount;
        if (nRCount < (int) pMesh ->m_sFaces .GetCount () * 3)
            nRCount = (int) pMesh ->m_sFaces .GetCount () * 3;
        pRemap = new VertRemap [nRCount];
        memset (&sRemapCounts, 0, sizeof (sRemapCounts));

        //
        // Initialize the face vert backs
        //

        for (int i = 0; i < (int) pMesh ->m_sFaces .GetCount () * 3; i++)
            pRemap [i] .nVertBack = -1;

        //
        // Here was to be code to help insure that the face adjacency
        // information was preserved.  Well, here is the kicker.  It can't
        // be.  Some of the models have invalid adjacency information.
        // Face A claims Face B but the two faces don't share 
        // vertices.  Sort of makes adjacency hard.
        //

        //
        // Compress the vertices by looping through the faces.  This
        // method produces a MUCH more reproducable decompilation that
        // makes decomp/comp verification mush more reliable.
        //

        for (int nFace = 0; nFace < (int) pMesh ->m_sFaces .GetCount (); nFace++)
        {
            int nFVert = nFace * 3;
            int v1 = pFaces [nFace] .ausVertex [0];
            int v2 = pFaces [nFace] .ausVertex [1];
            int v3 = pFaces [nFace] .ausVertex [2];
            if (pRemap [nFVert] .nVertBack == -1)
            {
                pRemap [nFVert] .nVertBack = FindRemapVertex (v1, pRemap,
                                                              sRemapCounts .nVert, pVerts, pCNormals, pulColors, 
                                                              pConstraints, pRemap [nFVert + 1] .nVertBack, 
                                                              pRemap [nFVert + 2] .nVertBack);
            }
            if (pRemap [nFVert + 1] .nVertBack == -1)
            {
                pRemap [nFVert + 1] .nVertBack = FindRemapVertex (v2, pRemap,
                                                                  sRemapCounts .nVert, pVerts, pCNormals, pulColors, 
                                                                  pConstraints, pRemap [nFVert] .nVertBack, 
                                                                  pRemap [nFVert + 2] .nVertBack);
            }
            if (pRemap [nFVert + 2] .nVertBack == -1)
            {
                pRemap [nFVert + 2] .nVertBack = FindRemapVertex (v3, pRemap,
                                                                  sRemapCounts .nVert, pVerts, pCNormals, pulColors, 
                                                                  pConstraints, pRemap [nFVert] .nVertBack, 
                                                                  pRemap [nFVert + 1] .nVertBack);
            }
        }

        //
        // Compress the tverts
        //

        for (int ti = 0; ti < 4; ti++)
        {
            if (apTVerts [ti] != NULL)
            {
                for (int i = 0; i < (int) pMesh ->m_sFaces .GetCount (); i++)
                {
                    int nFVert = i * 3;
                    int v1 = pFaces [i] .ausVertex [0];
                    int v2 = pFaces [i] .ausVertex [1];
                    int v3 = pFaces [i] .ausVertex [2];
                    pRemap [nFVert] .nTVertsBack [ti] = 
                        FindRemapTVertex (ti, v1, pRemap,
                                          sRemapCounts .nTVerts [ti], apTVerts [ti], -1, -1);
                    pRemap [nFVert + 1] .nTVertsBack [ti] = 
                        FindRemapTVertex (ti, v2, pRemap,
                                          sRemapCounts .nTVerts [ti], apTVerts [ti], 
                                          pRemap [nFVert] .nTVertsBack [ti], -1);
                    pRemap [nFVert + 2] .nTVertsBack [ti] = 
                        FindRemapTVertex (ti, v3, pRemap,
                                          sRemapCounts .nTVerts [ti], apTVerts [ti], 
                                          pRemap [nFVert] .nTVertsBack [ti], 
                                          pRemap [nFVert + 1] .nTVertsBack [ti]);
                }
            }
            assert (sRemapCounts .nTVerts [ti] <= nRCount);
        }

        //
        // Generate the smoothing masks
        //

        panSmoothSets = new int [nFaces];
        if (fIncludeSmoothing)
        {
            for (int i = 0; i < nFaces; i++)
                panSmoothSets [i] = 0;
            int nSet = 1;
            for (int i = 0; i < nFaces; i++)
            {

                //
                // If this face doesn't have a smoothing set yet
                //

                if (panSmoothSets [i] == 0)
                {

                    //
                    // Get the set for this face and collect any
                    // conflicts
                    //

                    unsigned long ulConflicts = 0;
                    GenerateSmoothMasks (i, 
                                         pMesh ->m_sFaces .GetData (pauchModelData),
                                         nFaces, pVerts, pNormals, panSmoothSets, 
                                         nSet, &ulConflicts);

                    //
                    // Try to locate a set slot that doesn't have a 
                    // conflict
                    //

                    int nRemapSet;
                    for (nRemapSet = 1; nRemapSet < nSet; nRemapSet++)
                    {
                        if ((ulConflicts & (1 << (nRemapSet - 1))) == 0)
                            break;
                    }

                    //
                    // If found, then remap.  Otherwise, we must use
                    // this slot
                    //

                    if (nRemapSet != nSet)
                    {
                        for (int j = 0; j < nFaces; j++)
                        {
                            if (panSmoothSets [j] == nSet)
                                panSmoothSets [j] = nRemapSet;
                        }
                    }
                    else
                        nSet++;
                }
            }
            //if (nSet > 32 + 1)
            //  printf ("SM overflow %d\n", nSet);
        }
        else
        {
            for (int i = 0; i < nFaces; i++)
                panSmoothSets [i] = 1;
        }

        //
        // Dump general information
        //

        snprintf (szText, _countof (szText),
                  "  ambient %.7g %.7g %.7g", pMesh ->m_vAmbient .m_x,
                  pMesh ->m_vAmbient .m_y, pMesh ->m_vAmbient .m_z);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText),
                  "  diffuse %.7g %.7g %.7g", pMesh ->m_vDiffuse .m_x,
                  pMesh ->m_vDiffuse .m_y, pMesh ->m_vDiffuse .m_z);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText),
                  "  specular %.7g %.7g %.7g", pMesh ->m_vSpecular .m_x,
                  pMesh ->m_vSpecular .m_y, pMesh ->m_vSpecular .m_z);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText),
                  "  shininess %.7g", pMesh ->m_fShininess);
        pStream ->WriteLine (szText);

        if (pMesh ->m_ulShadow != 1)
        {
            snprintf (szText, _countof (szText),
                      "  shadow %d", pMesh ->m_ulShadow);
            pStream ->WriteLine (szText);
        }

        if ((pMesh ->m_ulTransparencyHint != 0 && 
             (m_ulFlags & NwnMdlNF_HasAABB) == 0) ||
            (pMesh ->m_ulTransparencyHint == 0 && 
             (m_ulFlags & NwnMdlNF_HasAABB) != 0))
        {
            snprintf (szText, _countof (szText),
                      "  transparencyhint %d", pMesh ->m_ulTransparencyHint);
            pStream ->WriteLine (szText);
        }

        if (pMesh ->m_ulRenderHint != 0)
        {
            if (pMesh ->m_ulRenderHint == 2)
            {
                snprintf (szText, _countof (szText),
                          "  renderhint NormalAndSpecMapped");
            }
            else
            {
                snprintf (szText, _countof (szText),
                          "  renderhint None");
            }
            pStream ->WriteLine (szText);
        }

        if (pMesh ->m_ulBeaming != 0)
        {
            snprintf (szText, _countof (szText),
                      "  beaming %d", pMesh ->m_ulBeaming);
            pStream ->WriteLine (szText);
        }

        if ((pMesh ->m_ulRender == 0 && 
             (m_ulFlags & NwnMdlNF_HasAABB) == 0) ||
            (pMesh ->m_ulRender != 0 &&
             (m_ulFlags & NwnMdlNF_HasAABB) != 0))
        {
            snprintf (szText, _countof (szText),
                      "  render %d", pMesh ->m_ulRender);
            pStream ->WriteLine (szText);
        }

        if (pMesh ->m_ucRotateTexture != 0)
        {
            snprintf (szText, _countof (szText),
                      "  rotatetexture %d", pMesh ->m_ucRotateTexture);
            pStream ->WriteLine (szText);
        }

        if (pMesh ->m_ulTileFade != 0)
        {
            snprintf (szText, _countof (szText),
                      "  tilefade %d", pMesh ->m_ulTileFade);
            pStream ->WriteLine (szText);
        }

        if (pMesh ->m_ucLightMapped != 0)
        {
            snprintf (szText, _countof (szText),
                      "  lightmapped %d", pMesh ->m_ucLightMapped);
            pStream ->WriteLine (szText);
        }

        //
        // Write the textures
        //

        for (int i = 0; i < 3; i++)
        {
            if (pMesh ->m_szTextures [i] [0] != 0)
            {
                if (i == 0)
                {
                    snprintf (szText, _countof (szText),
                              "  bitmap %s", pMesh ->m_szTextures [i]);
                }
                else
                {
                    snprintf (szText, _countof (szText),
                              "  texture%d %s", i, pMesh ->m_szTextures [i]);
                }
                pStream ->WriteLine (szText);
            }
        }

        //
        // Write the material name
        //
        if (pMesh ->m_szMaterialName [0] != 0)
        {
            snprintf (szText, _countof (szText),
                      "  materialname %s", pMesh ->m_szMaterialName);
            pStream ->WriteLine (szText);
        }

        //
        // Dump the vertex list
        //

        if (pVerts && sRemapCounts .nVert > 0)
        {
            snprintf (szText, _countof (szText),
                      "  verts %d", sRemapCounts .nVert);
            pStream ->WriteLine (szText);
            for (int i = 0; i < sRemapCounts .nVert; i++)
            {
                int k = pRemap [i] .nVert;
                snprintf (szText, _countof (szText),
                          "    %12.7f %12.7f %12.7f", pVerts [k] .m_x, 
                          pVerts [k] .m_y, pVerts [k] .m_z);
                pStream ->WriteLine (szText);
            }
        }

        //
        // Dump the tverts lists
        //

        for (int i = 0; i < 4; i++)
        {
            if (apTVerts [i] && sRemapCounts .nTVerts [i] > 0)
            {
                if (i == 0)
                {
                    snprintf (szText, _countof (szText),
                              "  tverts %d", sRemapCounts .nTVerts [i]);
                }
                else
                {
                    snprintf (szText, _countof (szText),
                              "  tverts%d %d", i, sRemapCounts .nTVerts [i]);
                }
                pStream ->WriteLine (szText);
                for (int j = 0; j < sRemapCounts .nTVerts [i]; j++)
                {
                    int k = pRemap [j] .nTVerts [i];
                    snprintf (szText, _countof (szText), "    %12.7f %12.7f  0", 
                              apTVerts [i] [k] .m_x, apTVerts [i] [k] .m_y);
                    pStream ->WriteLine (szText);
                }
            }
        }

        //
        // Dump the colors
        //

        if (pulColors && sRemapCounts .nVert > 0)
        {
            int i;
            for (i = 0; i < pMesh ->m_usVertexCount; i++)
            {
                if (pulColors [i] != 0xFFFFFFFF)
                    break;
            }
            if (i < pMesh ->m_usVertexCount)
            {
                snprintf (szText, _countof (szText),
                          "  colors %d", sRemapCounts .nVert);
                pStream ->WriteLine (szText);
                for (int i = 0; i < sRemapCounts .nVert; i++)
                {
                    int k = pRemap [i] .nVert;
                    float r = ((float) (pulColors [k] & 0xFF)) / 255.0f;
                    float g = ((float) ((pulColors [k] >> 8) & 0xFF)) / 255.0f;
                    float b = ((float) ((pulColors [k] >> 16) & 0xFF)) / 255.0f;
                    snprintf (szText, _countof (szText),
                              "    %12.7f %12.7f %12.7f", r, g, b);
                    pStream ->WriteLine (szText);
                }
            }
        }

        //
        // Dump texindices
        //

        for (int ti = 1; ti < 4; ti++)
        {
            if (apTVerts [ti] != NULL && pMesh ->m_sFaces .GetCount () > 0)
            {
                snprintf (szText, _countof (szText),
                          "  texindices%d %ld", ti, (long)(pMesh ->m_sFaces .GetCount ()));
                pStream ->WriteLine (szText);
                for (int i = 0; i < (int) pMesh ->m_sFaces .GetCount (); i++)
                {

                    snprintf (szText, _countof (szText), "    %4d %4d %4d", 
                              pRemap [(i * 3) + 0] .nTVertsBack [ti],
                              pRemap [(i * 3) + 1] .nTVertsBack [ti],
                              pRemap [(i * 3) + 2] .nTVertsBack [ti]);
                    pStream ->WriteLine (szText);
                }
            }
        }

        //
        // Dump the faces
        //

        if (nFaces > 0)
        {

            //
            // Write the faces count line
            //

            snprintf (szText, _countof (szText), "  faces %d", nFaces);
            pStream ->WriteLine (szText);

            //
            // Calculate widths (ugly duplicate code)
            //

            int nVal1 = 1;
            int nVal2 = 1;
            int nVal3 = 1;

            for (int i = 0; i < nFaces; i++)
            {
                if (pRemap [(i * 3) + 0] .nVertBack > nVal1)
                    nVal1 = pRemap [(i * 3) + 0] .nVertBack;
                if (pRemap [(i * 3) + 1] .nVertBack > nVal1)
                    nVal1 = pRemap [(i * 3) + 1] .nVertBack;
                if (pRemap [(i * 3) + 2] .nVertBack > nVal1)
                    nVal1 = pRemap [(i * 3) + 2] .nVertBack;

                int nSmooth = 1 << (panSmoothSets [i] - 1);

                if (nSmooth > nVal2)
                    nVal2 = nSmooth;

                if (pRemap [(i * 3) + 0] .nTVertsBack [0] > nVal3)
                    nVal3 = pRemap [(i * 3) + 0] .nTVertsBack [0];
                if (pRemap [(i * 3) + 1] .nTVertsBack [0] > nVal3)
                    nVal3 = pRemap [(i * 3) + 1] .nTVertsBack [0];
                if (pRemap [(i * 3) + 2] .nTVertsBack [0] > nVal3)
                    nVal3 = pRemap [(i * 3) + 2] .nTVertsBack [0];
            }

            int nWidth1 = 1, nWidth2 = 1, nWidth3 = 1;

            while (nVal1 >= 10) {
                nWidth1++;
                nVal1 /= 10;
            }

            while (nVal2 >= 10) {
                nWidth2++;
                nVal2 /= 10;
            }

            while (nVal3 >= 10) {
                nWidth3++;
                nVal3 /= 10;
            }


            //
            // Write the actual faces
            //

            for (int i = 0; i < nFaces; i++)
            {
                int nTVert1;
                int nTVert2;
                int nTVert3;
                if (pMesh ->m_usTextureCount >= 1)
                {
                    nTVert1 = pRemap [(i * 3) + 0] .nTVertsBack [0];
                    nTVert2 = pRemap [(i * 3) + 1] .nTVertsBack [0];
                    nTVert3 = pRemap [(i * 3) + 2] .nTVertsBack [0];
                }
                else
                {
                    nTVert1 = nTVert2 = nTVert3 = 0;
                }

                snprintf (szText, _countof (szText),
                          "    %*d %*d %*d  %*d  %*d %*d %*d  %2d", 
                          nWidth1, pRemap [(i * 3) + 0] .nVertBack, 
                          nWidth1, pRemap [(i * 3) + 1] .nVertBack, 
                          nWidth1, pRemap [(i * 3) + 2] .nVertBack, 
                          nWidth2, 1 << (panSmoothSets [i] - 1), 
                          nWidth3, nTVert1,
                          nWidth3, nTVert2,
                          nWidth3, nTVert3, 
                          pFaces [i] .ulSurface);
                pStream ->WriteLine (szText);
            }

            //
            // If we are to include extended information, then
            // dump the vertex normals
            //

            if (fIncludeExtended && fIncludeSmoothing)
            {
                for (int i = 0; i < nFaces; i++)
                {
                    snprintf (szText, _countof (szText),
                              "  #face-vertnormal0 %d  %12.7f %12.7f %12.7f", i,
                              pNormals [pFaces [i] .ausVertex [0]] .m_x,
                              pNormals [pFaces [i] .ausVertex [0]] .m_y,
                              pNormals [pFaces [i] .ausVertex [0]] .m_z);
                    pStream ->WriteLine (szText);
                    snprintf (szText, _countof (szText),
                              "  #face-vertnormal1 %d  %12.7f %12.7f %12.7f", i,
                              pNormals [pFaces [i] .ausVertex [1]] .m_x,
                              pNormals [pFaces [i] .ausVertex [1]] .m_y,
                              pNormals [pFaces [i] .ausVertex [1]] .m_z);
                    pStream ->WriteLine (szText);
                    snprintf (szText, _countof (szText),
                              "  #face-vertnormal2 %d  %12.7f %12.7f %12.7f", i,
                              pNormals [pFaces [i] .ausVertex [2]] .m_x,
                              pNormals [pFaces [i] .ausVertex [2]] .m_y,
                              pNormals [pFaces [i] .ausVertex [2]] .m_z);
                    pStream ->WriteLine (szText);
                }
            }
        }

        //
        // Dump some note lines for bumpmap information
        //

        if (fIncludeExtended)
        {
            snprintf (szText, _countof (szText), 
                      "#face-norma-sum/2 %.7g", pMesh ->m_fFaceNormalSumDiv2);
            pStream ->WriteLine (szText);
            if (pMesh ->m_pavBumpmapAnim1 .GetDataNZ (pauchRawData) != NULL)
                pStream ->WriteLine ("  #has-bumpmap1");
            if (pMesh ->m_pavBumpmapAnim2 .GetDataNZ (pauchRawData) != NULL)
                pStream ->WriteLine ("  #has-bumpmap2");
            if (pMesh ->m_pavBumpmapAnim3 .GetDataNZ (pauchRawData) != NULL)
                pStream ->WriteLine ("  #has-bumpmap3");
            if (pMesh ->m_pavBumpmapAnim4 .GetDataNZ (pauchRawData) != NULL)
                pStream ->WriteLine ("  #has-bumpmap4");
            if (pMesh ->m_pavBumpmapAnim5 .GetDataNZ (pauchRawData) != NULL)
                pStream ->WriteLine ("  #has-bumpmap5");
            if (pMesh ->m_pafBumpmapAnim6 .GetDataNZ (pauchRawData) != NULL)
                pStream ->WriteLine ("  #has-bumpmap6");
        }

        // mirrorlist
        // bmin
        // bmax
        // vertexindicescount
        // leftoverfaces
        // vertexindices
    }

    //
    // If we have skin mesh data
    //

    if ((m_ulFlags & NwnMdlNF_HasSkin) != 0)// && !fAnimation)
    {
        CNwnMdlSkinMeshNode *pSkin = 
            static_cast <CNwnMdlSkinMeshNode *> (this);

        //
        // Dump the weights
        //

        float *pafWeights = pSkin ->m_pafSkinWeights .GetDataNZ (pauchRawData);
        INT16 *pasBones = pSkin ->m_pasSkinBoneRefs .GetDataNZ (pauchRawData);
        if (pafWeights && pasBones && sRemapCounts .nVert > 0)
        {
            snprintf (szText, _countof (szText),
                      "  weights %d", sRemapCounts .nVert);
            pStream ->WriteLine (szText);
            for (int i = 0; i < sRemapCounts .nVert; i++)
            {
                pStream ->WriteLine ("    ", false);
                int nVert = pRemap [i] .nVert * 4;
                for (int j = 0; j < 4; j++)
                {
                    int nBone = pasBones [nVert + j];
                    if (nBone >= 0)
                    {
                        int nNode = pSkin ->m_asBoneNodeNumbers [nBone];
                        CNwnMdlNodeHeader *pNode = GetNthNode (
                            pModel ->m_pGeometry .GetData (pauchModelData), 
                            nNode, pauchModelData);
                        const char *pszText;
                        if (pNode)
                            pszText = pNode ->m_szName;
                        else
                            pszText = "invalidnodeindex";
                        snprintf (szText, _countof (szText),
                                  "%s %.7g ", pszText, pafWeights [nVert + j]);
                        pStream ->WriteLine (szText, false);
                    }
                }
                pStream ->WriteLine ("", true);
            }
        }

        //
        // If we are to included extended information
        //

        if (fIncludeExtended)
        {

            //
            // Dump the QBones
            //

            CQuaternion *pQs = pSkin ->m_aQBoneRefInv 
                .GetData (pauchModelData);
            for (int i = 0; i < (int) pSkin ->m_aQBoneRefInv .GetCount (); i++)
            {
                snprintf (szText, _countof (szText),
                          "  #qbone %d %12.7f %12.7f %12.7f %12.7f", i,
                          pQs [i] .m_x, pQs [i] .m_y, pQs [i] .m_z, pQs [i] .m_w);
                pStream ->WriteLine (szText);
            }

            //
            // Dump the TBones
            //

            CVector3 *pTs = pSkin ->m_aTBoneRefInv 
                .GetData (pauchModelData);
            for (int i = 0; i < (int) pSkin ->m_aTBoneRefInv .GetCount (); i++)
            {
                snprintf (szText, _countof (szText),
                          "  #tbone %d %12.7f %12.7f %12.7f", i,
                          pTs [i] .m_x, pTs [i] .m_y, pTs [i] .m_z);
                pStream ->WriteLine (szText);
            }

            //
            // Dump the node to bone map
            //

            INT16 *pasN2B = pSkin ->m_pasNodeToBoneMap  
                .GetDataNZ (pauchModelData);
            for (int i = 0; i < (int) pSkin ->m_ulNodeToBoneCount; i++)
            {
                snprintf (szText, _countof (szText),
                          "  #n2b %d %d", i, pasN2B [i]);
                pStream ->WriteLine (szText);
            }

            //
            // NOTE: Weights and skin bone refs are not dumped because
            // the decompiler doesn't preserve vertex order.  Thus even though
            // the comparison would be valid in theory, it doesn't work
            // in practice.
            //
        }
    }

    //
    // If we have anim mesh data
    //

    if ((m_ulFlags & NwnMdlNF_HasAnim) != 0)// && !fAnimation)
    {
        CNwnMdlAnimMeshNode *pAnim = 
            static_cast <CNwnMdlAnimMeshNode *> (this);

        snprintf (szText, _countof (szText),
                  "  sampleperiod %.7g", pAnim ->m_fSamplePeriod);
        pStream ->WriteLine (szText);

        if (pAnim ->m_ulVertexSetCount > 0 && pAnim ->m_usVertexCount > 0)
        {
            CVector3 *pAnimVerts = pAnim ->m_pvVertexSets .GetData (pauchModelData);
            snprintf (szText, _countof (szText), "  animverts %d", 
                      pAnim ->m_ulVertexSetCount * sRemapCounts .nVert);
            pStream ->WriteLine (szText);
            for (int i = 0; i < (int) pAnim ->m_ulVertexSetCount; i++)
            {
                for (int j = 0; j < sRemapCounts .nVert; j++)
                {
                    int n = pRemap [j] .nVert;
                    CVector3 *p = &pAnimVerts [
                        n * pAnim ->m_ulVertexSetCount + i];
                    snprintf (szText, _countof (szText), 
                              "    %12.7f %12.7f %12.7f", p ->m_x, p ->m_y, p ->m_z);
                    pStream ->WriteLine (szText);
                }
            }
        }

        if (pAnim ->m_ulTextureSetCount > 0 && pAnim ->m_usVertexCount > 0)
        {
            CVector2 *pAnimTVerts = pAnim ->m_pvTextureSets .GetData (pauchModelData);
            snprintf (szText, _countof (szText), "  animtverts %d", 
                      pAnim ->m_ulTextureSetCount * sRemapCounts .nTVerts [0]);
            pStream ->WriteLine (szText);
            for (int i = 0; i < (int) pAnim ->m_ulTextureSetCount; i++)
            {
                for (int j = 0; j < sRemapCounts .nTVerts [0]; j++)
                {
                    int n = pRemap [j] .nTVerts [0];
                    CVector2 *p = &pAnimTVerts [
                        n * pAnim ->m_ulTextureSetCount + i];
                    snprintf (szText, _countof (szText),
                              "    %12.7f %12.7f  0", p ->m_x, p ->m_y);
                    pStream ->WriteLine (szText);
                }
            }
        }
    }

    //
    // If we have dangly mesh data
    //

    if ((m_ulFlags & NwnMdlNF_HasDangly) != 0)// && !fAnimation)
    {
        CNwnMdlDanglyMeshNode *pDangly = 
            static_cast <CNwnMdlDanglyMeshNode *> (this);

        snprintf (szText, _countof (szText),
                  "  displacement %.7g", pDangly ->m_fDisplacement);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText),
                  "  period %.7g", pDangly ->m_fPeriod);
        pStream ->WriteLine (szText);

        snprintf (szText, _countof (szText),
                  "  tightness %.7g", pDangly ->m_fTightness);
        pStream ->WriteLine (szText);

        if (pDangly ->m_afConstraints .GetCount () > 0)
        {
            float *pfValues = pDangly ->
                m_afConstraints .GetData (pauchModelData);
            snprintf (szText, _countof (szText),
                      "  constraints %d", sRemapCounts .nVert);
            pStream ->WriteLine (szText);
            for (int i = 0; i < sRemapCounts .nVert; i++)
            {
                int k = pRemap [i] .nVert;
                snprintf (szText, _countof (szText),
                          "    %.7g", pfValues [k]);
                pStream ->WriteLine (szText);
            }
        }
    }

    //
    // If we have aabb mesh data
    //

    if ((m_ulFlags & NwnMdlNF_HasAABB) != 0)// && !fAnimation)
    {
        CNwnMdlAABBMeshNode *pAABB = 
            static_cast <CNwnMdlAABBMeshNode *> (this);
        NwnMdlAABBEntry *pEntry = pAABB ->m_pAABBTree .
            GetDataNZ (pauchModelData);
        if (pEntry)
            AABBEntryDecomp (pStream, pEntry, pauchModelData, 0);
    }

    //
    // Dump the controllers
    //

    if (m_aControllerData .GetCount () > 0 &&
        m_aControllerKey .GetCount () > 0)
    {

        //
        // Get pointers to the structures 
        //

        float *pfData = m_aControllerData .GetData (pauchModelData);
        NwnMdlControllerKey *pKeys = m_aControllerKey .GetData (pauchModelData);

        //
        // Loop through the controllers
        //

        for (int i = 0; i < (int) m_aControllerKey .GetCount (); i++)
        {

            //
            // Get pointers to the time keys and data for this key
            //

            float *pfKeys = &pfData [pKeys [i] .sKeyOffset];
            float *pfData2 = &pfData [pKeys [i] .sDataOffset];

            //
            // Test to see if this controller is keyed
            //

            bool fBezierKeyed;
            bool fKeyed;
            int nColumns;
            if (pKeys [i] .bColumns == -1)
            {
                nColumns = 0;
                fKeyed = true;
                fBezierKeyed = false;
            }
            else
            {
                fBezierKeyed = (pKeys [i] .bColumns & 0x10) != 0;
                fKeyed = pKeys [i] .sRows > 1 || 
                    pfKeys [0] != 0.0f || fBezierKeyed;
                nColumns = pKeys [i] .bColumns & 0x0F;
            }


            //
            // Get the name of the controller
            //

            const char *pszController = NmcGetControllerName (
                (int) pKeys [i] .lType, m_ulFlags);
            if (pszController == NULL)
                pszController = "unknowncontroller";

            //
            // Generate the full controller name
            //

            char szController [64];
            strcpy (szController, pszController);
            if (fBezierKeyed)
                strcat (szController, "bezierkey");
            else if (fKeyed)
                strcat (szController, "key");

            //
            // Format
            //

            if (fKeyed)
            {
                snprintf (szText, _countof (szText),
                          "  %s %d", szController, pKeys [i] .sRows);
                pStream ->WriteLine (szText);
                for (int j = 0; j < pKeys [i] .sRows; j++)
                {
                    if (pKeys [i] .bColumns == -1)
                    {
                        szText [0] = 0;
                        pStream ->WriteLine (szText, true);
                    }
                    else
                    {
                        snprintf (szText, _countof (szText),
                                  "    %12.7f", *pfKeys++);
                        pStream ->WriteLine (szText, false);
                        ControllerRowDecomp (pStream, pfData2, nColumns, true);
                        pfData2 += nColumns;
                    }
                }
            }
            else
            {
                snprintf (szText, _countof (szText),
                          "  %s", szController);
                pStream ->WriteLine (szText, false);
                ControllerRowDecomp (pStream, pfData2, nColumns, false);
                pfData2 += nColumns;
            }
        }
    }

    //
    // End the node
    //

    pStream ->WriteLine ("endnode");

    //
    // Do children
    //

    if (m_apChildren .GetCount () > 0)
    {

        //
        // If we are to include children
        //

        if (fIncludeChildren)
        {

            //
            // Dump all the children
            //

            CNwnPointer <CNwnMdlNodeHeader> *ppChildren = 
                m_apChildren .GetData (pauchModelData);
            int nChildren = (int) m_apChildren .GetCount ();
            for (int i = 0; i < nChildren; i++)
            {
                CNwnMdlNodeHeader *pChild = 
                    ppChildren [i] .GetData (pauchModelData);
                pChild ->Decompile (pStream, pModel, pParentGeom,
                                    this, true, fIncludeExtended, fIncludeSmoothing,
                                    pauchModelData, pauchRawData);
            }
        }

        //
        // Otherwise, just add a note
        //

        else
        {
            //pStream ->WriteLine ("#CHILDREN OMITTED");
        }
    }

    //
    // Delete the remap data
    //

    if (pRemap)
        delete [] pRemap;
    if (panSmoothSets)
        delete [] panSmoothSets;
}

//-----------------------------------------------------------------------------
//
// @func Decompile controller row
//
// @parm CNwnStream * | pStream | Output stream
//
// @parm float * | pfValues | Array of values
//
// @parm int | nColumns | Number of columns
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void ControllerRowDecomp (CNwnStream *pStream, float *pfValues,
                          int nColumns, bool bColumnize)
{
    char szText [32];
    float afValues [4];

    //
    // If there are 4 columns, then this is a quaternion and must be
    // converted back to an axis/rotation
    //

    if (nColumns == 4)
    {
        CQuaternion *pQ = (CQuaternion *) pfValues;
        CVector3 *pV = (CVector3 *) afValues;
        pQ ->GetAxisRotation (pV, &afValues [3]);
        pfValues = afValues;
    }

    //
    // Write the data
    //

    for (int k = 0; k < nColumns; k++)
    {
        snprintf (szText, _countof (szText), (bColumnize ? " %12.7f" : " %.7g"), *pfValues++);
        pStream ->WriteLine (szText, false);
    }
    pStream ->WriteLine ("", true);
}

//-----------------------------------------------------------------------------
//
// @func Decompile an AABB entry
//
// @parm CNwnStream * | pStream | Output stream
//
// @parm NwnMdlAABBEntry * | pEntry | Entry to decomp
//
// @parm unsigned char * | pauchModelData | Pointer to the model data or
//              NULL if this is an absolute model.
//
// @parm unsigned char * | pauchRawData | Pointer to the raw data or
//              NULL if this is an absolute model.
//
// @parm int | nDepth | Current tree depth
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void AABBEntryDecomp (CNwnStream *pStream, NwnMdlAABBEntry *pEntry, 
                      unsigned char *pauchModelData, int nDepth)
{
    char szDepth [128];
    char szText [256];
    const char *pszDepth;

    if (nDepth == 0)
        pszDepth = "  aabb";
    else
    {
        memset (szDepth, ' ', nDepth + 6);
        szDepth [nDepth + 6] = 0;
        pszDepth = szDepth;
    }

    snprintf (szText, _countof (szText), "%s %12.7f %12.7f %12.7f %12.7f %12.7f %12.7f %d", 
              pszDepth, 
              pEntry ->vBMin .m_x, pEntry ->vBMin .m_y, pEntry ->vBMin .m_z,
              pEntry ->vBMax .m_x, pEntry ->vBMax .m_y, pEntry ->vBMax .m_z,
              pEntry ->ulLeafFace);
    pStream ->WriteLine (szText);
    if (pEntry ->ulLeafFace == 0xFFFFFFFF)
    {
        AABBEntryDecomp (pStream, 
                         pEntry ->psLeft .GetData (pauchModelData),
                         pauchModelData, nDepth + 1);
        AABBEntryDecomp (pStream, 
                         pEntry ->psRight .GetData (pauchModelData),
                         pauchModelData, nDepth + 1);
    }
}

//-----------------------------------------------------------------------------
//
// @func Locate a vertex coord in the remap list
//
// @parm int | nVert | Vertex index
//
// @parm VertRemap * | pRemap | Pointer to the remap array
//
// @parm int & | nRemapVerts | Number of remapped verts.  Can be updated.
//
// @parm CVector3 * | pVerts | Vertex array
//
// @parm CVector3 * | pNormals | Normal array
//
// @parm UINT32 * | pulColors | Colors array (can be NULL)
//
// @parm float * | pContraints | Contraints array (can be NULL)
//
// @parm int | nV1x | Vertex 1 exclusion index.  If >= 0, this vertex
//              will be excluded.
//
// @parm int | nV2x | Vertex 2 exclusion index.  If >= 0, this vertex
//              will be excluded.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

int FindRemapVertex (int nVert, VertRemap *pRemap, int &nRemapVerts,
                     CVector3 *pVerts, CVector3 *pNormals, UINT32 *pulColors, 
                     float *pConstraints, int nV1x, int nV2x)
{
    nV1x; nV2x; pNormals; //get rid of 4100 warning

    //
    // Try to find a match
    //

    for (int j = 0; j < nRemapVerts; j++)
    {
        //if (j == nV1x || j == nV2x)
        //      continue;
        int k = pRemap [j] .nVert;
        if (pVerts [k] != pVerts [nVert])
            continue;
        //if (!pNormals [k] .AlmostEqual (pNormals [nVert], 0.000001f))
        //      continue;
        if (pulColors != NULL && 
            pulColors [k] != pulColors [nVert])
            continue;
        if (pConstraints != NULL &&
            pConstraints [k] != pConstraints [nVert])
            continue;
        return j;
    }

    //
    // If not found, add
    //

    pRemap [nRemapVerts] .nVert = nVert;
    return nRemapVerts++;
}

//-----------------------------------------------------------------------------
//
// @func Locate a texture coord in the remap list
//
// @parm int | nTex | Texture set (0-3)
//
// @parm int | nTVert | Texture vertex index
//
// @parm VertRemap * | pRemap | Pointer to the remap array
//
// @parm int & | nRemapVerts | Number of remapped verts.  Can be updated.
//
// @parm CVector2 * | pTVerts | TVert arrray
//
// @parm int | nV1x | Vertex 1 exclusion index.  If >= 0, this vertex
//              will be excluded.
//
// @parm int | nV2x | Vertex 2 exclusion index.  If >= 0, this vertex
//              will be excluded.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

int FindRemapTVertex (int nTex, int nTVert, VertRemap *pRemap, 
                      int &nRemapVerts, CVector2 *pTVerts, int nV1x, int nV2x)
{

    //
    // Try to find a match
    //

    for (int j = 0; j < nRemapVerts; j++)
    {
        if (j == nV1x || j == nV2x)
            continue;
        int k = pRemap [j] .nTVerts [nTex];
        if (pTVerts [k] != pTVerts [nTVert])
            continue;
        return j;
    }

    //
    // If not found, add
    //

    pRemap [nRemapVerts] .nTVerts [nTex] = nTVert;
    return nRemapVerts++;
}

//-----------------------------------------------------------------------------
//
// @func Collect the children
//
// @parm CNwnMdlNodeHeader * | pNode | Node in question
//
// @parm NodeSort * | pList | Output list
//
// @parm CNwnMdlNodeHeader * | pParentNode | Parent node
//
// @parm int | nIndex | Index of the node
//
// @parm unsigned char * | pauchModelData | Pointer to the model data or
//              NULL if this is an absolute model.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

int CollectNodes (CNwnMdlNodeHeader *pNode, NodeSort *pList, 
                  CNwnMdlNodeHeader *pParentNode, int nIndex, 
                  unsigned char *pauchModelData, int nPart)
{

    //
    // If I am NULL, return
    //

    if (pNode == NULL) 
        return nPart;

    //
    // Save myself
    //

    pList [nPart] .pNode = pNode;
    pList [nPart] .pParent = pParentNode;
    pList [nPart] .nIndex = nIndex;
    nPart++;

    //
    // Do the children
    //

    CNwnPointer <CNwnMdlNodeHeader> *ppChildren = 
        pNode ->m_apChildren .GetData (pauchModelData);
    int nChildren = (int) pNode ->m_apChildren .GetCount ();
    for (int i = 0; i < nChildren; i++)
    {
        nPart = CollectNodes (ppChildren [i] .GetData (pauchModelData), 
                              pList, pNode, i, pauchModelData, nPart);
    }

    return nPart;
}

//-----------------------------------------------------------------------------
//
// @func Generate the smoothing masks for the node
//
// @parm int | nFace | Current face
//
// @parm NwnMdlFace * | pFaces | Array of faces
//
// @parm int | nFaceCount | Number of faces in the array
//
// @parm CVector3 * | paVerts | Array of verts
//
// @parm CVector3 * | paVertNormals | Array of vertex normals
//
// @parm int * | panSmoothSets | List of smoothing sets
//
// @parm int | nSet | Current smoothing set
//
// @parm unsigned long * | pulConflicts | Bitmask of conflicting sets
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void GenerateSmoothMasks (int nFace, NwnMdlFace *pFaces,
                          int nFaceCount, CVector3 *paVerts, CVector3 *paVertNormals,
                          int *panSmoothSets, int nSet, unsigned long *pulConflicts)
{
    NwnMdlFace *pFace = &pFaces [nFace];

    //
    // If this node already has a smooth mask, then just validate
    //

    if (panSmoothSets [nFace] != 0)
    {
        assert (panSmoothSets [nFace] == nSet);
        return;
    }

    //
    // Save the face mask and propagate
    //

    panSmoothSets [nFace] = nSet;
    for (int nVert = 0; nVert < 3; nVert++)
    {
        CVector3 v = paVerts [pFace ->ausVertex [nVert]];
        CVector3 vn = paVertNormals [pFace ->ausVertex [nVert]];
        for (int nFace2 = 0; nFace2 < nFaceCount; nFace2++)
        {
            if (nFace2 == nFace) 
                continue;
            for (int nVert2 = 0; nVert2 < 3; nVert2++)
            {
                CVector3 v2 = paVerts [pFaces [nFace2] .ausVertex [nVert2]];
                CVector3 vn2 = paVertNormals [pFaces [nFace2] .ausVertex [nVert2]];
                if (v2 == v)
                {
                    if (vn2 .AlmostEqual (vn, 0.00001f))
                    {
                        GenerateSmoothMasks (nFace2, pFaces, nFaceCount, paVerts,
                                             paVertNormals, panSmoothSets, nSet, pulConflicts);
                        break;
                    }
                    else
                    {
                        if (panSmoothSets [nFace2] > 0)
                        {
                            *pulConflicts |= 1 << (panSmoothSets [nFace2] - 1);
                        }
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
//
// @func Return a pointer to the n'th node
//
// @parm CNwnMdlNodeHeader * | pNode | Pointer to the current node
//
// @parm int & | nIndex | Node index
//
// @parm unsigned char * | pauchModelData | Model data
//
// @rdesc Address of the node or NULL if not found
//
//-----------------------------------------------------------------------------

CNwnMdlNodeHeader *GetNthNode (CNwnMdlNodeHeader *pNode, int &nIndex, 
                               unsigned char *pauchModelData)
{

    //
    // Check for error
    //

    if (nIndex < 0)
        return NULL;
    if (pNode == NULL)
        return NULL;

    //
    // Check for found
    //

    if (nIndex == 0)
        return pNode;
    nIndex--;

    //
    // Invoke on children
    //

    if (pNode ->m_apChildren .GetCount () > 0)
    {
        CNwnPointer <CNwnMdlNodeHeader> *ppChildren = 
            pNode ->m_apChildren .GetData (pauchModelData);
        for (int i = 0; i < (int) pNode ->m_apChildren .GetCount (); i++)
        {
            CNwnMdlNodeHeader *pChildNode = 
                ppChildren [i] .GetData (pauchModelData);
            pChildNode = GetNthNode (pChildNode, nIndex, pauchModelData);
            if (pChildNode != NULL)
                return pChildNode;
        }
    }
    return NULL;
}

/* vim: set sw=4: */
