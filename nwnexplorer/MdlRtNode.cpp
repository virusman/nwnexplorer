//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      MdlRtNode.cpp - Resource data source |
//
// This module contains the definition of the resource data source.
//
// Copyright (c) 2003 - Edward T. Smith
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
// $History: DataCache.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"
#include "MdlRtNode.h"
#include "../_MathLib/Matrix.h"
#include <gl/gl.h>
#include <gl/glu.h>

//
// Debug NEW
//

#if defined (_DEBUG) && !defined (_WIN32_WCE)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// Externals
//

extern CDataCache g_sResourceCache;
extern BOOL g_fViewModelsMdlNameTex;

//-----------------------------------------------------------------------------
//
// @mfunc <c CMdlRtNode> constructor.
//
// @parm CNwnMdlNodeHeader * | pNode | Pointer to the parent node
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CMdlRtNode::CMdlRtNode (CNwnMdlNodeHeader *pNode) : m_vPosition (0, 0, 0),
    m_qOrientation (0, 0, 0, 1)
{
    m_pNode = pNode;
    m_fScale = 1.0f;
    m_fAlpha = 1.0f;
    m_fMesh = false;
    m_pTexture = NULL;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CMdlRtNode> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CMdlRtNode::~CMdlRtNode ()
{
    for (int i = 0; i < m_apRtNodes .GetCount (); i++)
        delete m_apRtNodes [i];
}

//-----------------------------------------------------------------------------
//
// @mfunc Create the node and it's children
//
// @parm CResModelRef & | sRes | Resource 
//
// @parm int | nOffset | Offset to the node in the geometry
//
// @rdesc Pointer to the node
//
//-----------------------------------------------------------------------------

CMdlRtNode *CMdlRtNode::CreateNodes (CResModelRef &sRes, int nOffset, CMdlRtNode *pParent)
{

    //
    // Get the node
    //

    CNwnMdlNodeHeader *pNode = (CNwnMdlNodeHeader *) 
        sRes .GetModelData (nOffset);

    //
    // Normally, each type of node would have it's own type of
    // runtime structure.  But currently, I am just going to support
    // the basic mesh type.
    //

    CMdlRtNode *pRtNode = new CMdlRtNode (pNode);
    pRtNode ->m_fMesh   = (pNode ->m_ulFlags & NwnMdlNF_HasMesh) != 0;
    pRtNode ->m_pParent = pParent;

    //
    // Create the children
    //

    if (pNode ->m_apChildren .GetCount () > 0)
    {
        CNwnPointer <CNwnMdlNodeHeader> *pulOffsets = pNode ->
            m_apChildren .GetData (sRes .GetModelData ());
        for (int i = 0; i < pNode ->m_apChildren .GetCount (); i++)
        {
            CMdlRtNode *pRtChild = CreateNodes (sRes, 
                                                pulOffsets [i] .GetOffset (),
                                                pRtNode);
            pRtNode ->m_apRtNodes .Add (pRtChild);
        }
    }

    //
    // Return the node
    //

    return pRtNode;
}

//-----------------------------------------------------------------------------
//
// @mfunc Render the node
//
// @parm CResModelRef & | sRes | Resource 
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMdlRtNode::Render (CResModelRef &sRes, float fTime, DWORD nFlags)
{

    //
    // Save the current matrix
    //

    glPushMatrix ();

    //
    // Look for controller data we are interested in
    //

    ProcessControllers (
        m_pNode ->m_aControllerKey .GetData (sRes .GetModelData ()),
        (int) m_pNode ->m_aControllerKey .GetCount (), 
        m_pNode ->m_aControllerData .GetData (sRes .GetModelData ()));

    //
    // Update the matrix
    //

    if (m_pNode ->m_ulFlags == NwnMdlNT_AABBMesh)
        glTranslatef (m_vPosition .m_x, m_vPosition .m_y, m_vPosition .m_z + 0.05f);
    else
        glTranslatef (m_vPosition .m_x, m_vPosition .m_y, m_vPosition .m_z);

    CMatrix m;
    m .MakeQuaternion (m_qOrientation);
    glMultMatrixf (m .m_m [0]);
    glScalef (m_fScale, m_fScale, m_fScale);

    //
    // If we have a mesh
    //

    if (m_fMesh && (nFlags & MDL_RENDER_SHOW_MESH))
    {
        CNwnMdlMeshHeader *pMesh = (CNwnMdlMeshHeader *) m_pNode;

        //
        // Draw (NOTE: Alpha images really must be depth queued.
        //              When they are not depth queued, the Z-plane prevents
        //              triangles deeper in the scene from painting properly.)
        //

        if (pMesh ->m_usVertexCount != 0 && 
            pMesh ->m_pavVerts .GetOffset () != 0xFFFFFFFF &&
            pMesh ->m_sVertexIndicesCount .GetCount () != 0 &&
            !(pMesh ->m_ulTileFade & (nFlags & MDL_RENDER_HIDE_FADE)) &&
            !((nFlags & MDL_RENDER_ORTHOGRAPHIC) && pMesh ->m_ulTileFade > 1) &&
            (pMesh ->m_ulRender ||
             ((nFlags & MDL_RENDER_SHOW_AABB) && m_pNode ->m_ulFlags == NwnMdlNT_AABBMesh))
            // &&       m_pTexture != NULL
           )
        {

            //
            // Enable/Disable the texture
            //

            if (m_pTexture == NULL)
                glDisable (GL_TEXTURE_2D);
            else
            {
                GLuint *pname = (GLuint *) m_pTexture ->GetData (0);
                glEnable (GL_TEXTURE_2D);
                glBindTexture (GL_TEXTURE_2D, *pname);
                assert (GL_NO_ERROR == glGetError ());
            }

            //
            // Set the material
            //

            float fv [4];
            fv [3] = ((nFlags & MDL_RENDER_TRANSLUCENT) ? 0.7f : m_fAlpha);
            glMaterialf (GL_FRONT, GL_SHININESS, pMesh ->m_fShininess);

            fv [0] = pMesh ->m_vAmbient .m_x;
            fv [1] = pMesh ->m_vAmbient .m_y;
            fv [2] = pMesh ->m_vAmbient .m_z;
            glMaterialfv (GL_FRONT, GL_AMBIENT, fv);

            fv [0] = pMesh ->m_vDiffuse .m_x;
            fv [1] = pMesh ->m_vDiffuse .m_y;
            fv [2] = pMesh ->m_vDiffuse .m_z;
            glMaterialfv (GL_FRONT, GL_DIFFUSE, fv);

            fv [0] = pMesh ->m_vSpecular .m_x;
            fv [1] = pMesh ->m_vSpecular .m_y;
            fv [2] = pMesh ->m_vSpecular .m_z;
            glMaterialfv (GL_FRONT, GL_SPECULAR, fv);

            //
            // XXX: Support selfillumcolor/wirecolor in the future
            //
            if ((nFlags & MDL_RENDER_IS_DWK) && m_pTexture == NULL && m_pNode-> m_szName [0] != 0) {
                float fv3 = fv [3];
                size_t nLen = strlen(m_pNode ->m_szName);

                fv [0] = 0.0f;
                fv [1] = 0.0f;
                fv [2] = 0.0f;
                fv [3] = 0.5f;
                glMaterialfv (GL_FRONT, GL_AMBIENT, fv);
                glMaterialfv (GL_FRONT, GL_DIFFUSE, fv);
                glMaterialfv (GL_FRONT, GL_SPECULAR, fv);

                if ((nLen > 6 && _stricmp(&(m_pNode ->m_szName[nLen - 6]), "_open1") == 0) ||
                    (nLen > 7 && _stricmp(&(m_pNode ->m_szName[nLen - 7]), "_open01") == 0)) {
                    fv [0] = 0.0f;
                    fv [1] = 1.0f;
                    fv [2] = 0.3f;
                    glMaterialfv (GL_FRONT, GL_EMISSION, fv);
                } else if ((nLen > 6 && _stricmp(&(m_pNode ->m_szName[nLen - 6]), "_open2") == 0) ||
                           (nLen > 7 && _stricmp(&(m_pNode ->m_szName[nLen - 6]), "_open02") == 0)) {
                    fv [0] = 0.0f;
                    fv [1] = 0.3f;
                    fv [2] = 1.0f;
                    glMaterialfv (GL_FRONT, GL_EMISSION, fv);
                } else if ((nLen > 7 && _stricmp(&(m_pNode ->m_szName[nLen - 7]), "_closed") == 0)  ||
                           (nLen > 8 && _stricmp(&(m_pNode ->m_szName[nLen - 6]), "_closed1") == 0) ||
                           (nLen > 9 && _stricmp(&(m_pNode ->m_szName[nLen - 9]), "_closed01") == 0)) {
                    fv [0] = 1.0f;
                    fv [1] = 0.0f;
                    fv [2] = 0.3f;
                    glMaterialfv (GL_FRONT, GL_EMISSION, fv);
                } else if ((nLen > 8 && _stricmp(&(m_pNode ->m_szName[nLen - 6]), "_closed2") == 0) ||
                           (nLen > 9 && _stricmp(&(m_pNode ->m_szName[nLen - 9]), "_closed02") == 0)) {
                    fv [0] = 1.0f;
                    fv [1] = 0.3f;
                    fv [2] = 0.0f;
                    glMaterialfv (GL_FRONT, GL_EMISSION, fv);
                }

                fv [3] = fv3;
            } else {
                fv [0] = 0.0f;
                fv [1] = 0.0f;
                fv [2] = 0.0f;
                glMaterialfv (GL_FRONT, GL_EMISSION, fv);
            }

            //
            // Draw
            //

            if (nFlags & MDL_RENDER_OUTLINE_POLYS)
            {
                NwnMdlFace *pFaces = (NwnMdlFace *) sRes .GetModelData (pMesh ->m_sFaces .GetOffset());
                float *pVerts = (float *) sRes .GetRawData (pMesh ->m_pavVerts .GetOffset ());

                glMaterialf (GL_FRONT, GL_SHININESS, 0.0);

                fv [0] = 0.0f;
                fv [1] = 0.0f;
                fv [2] = 0.0f;
                fv [3] = 0.9f;
                glMaterialfv (GL_FRONT, GL_AMBIENT, fv);
                glMaterialfv (GL_FRONT, GL_DIFFUSE, fv);
                glMaterialfv (GL_FRONT, GL_SPECULAR, fv);

                fv [0] = 1.0f;
                fv [1] = 1.0f;
                fv [2] = 1.0f;
                fv [3] = 1.0f;
                glMaterialfv (GL_FRONT, GL_EMISSION, fv);

                glDisableClientState (GL_NORMAL_ARRAY);
                glDisableClientState (GL_TEXTURE_COORD_ARRAY);
                glDisable (GL_TEXTURE_2D);
                glDisable (GL_DEPTH_TEST);
                glLineWidth(1.0);
                glBegin (GL_LINES);

                for (int i = 0; i < pMesh ->m_sFaces .GetCount (); i++)
                {
                    glVertex3fv(&(pVerts[pFaces[i] .ausVertex[0] * 3]));
                    glVertex3fv(&(pVerts[pFaces[i] .ausVertex[1] * 3]));

                    glVertex3fv(&(pVerts[pFaces[i] .ausVertex[1] * 3]));
                    glVertex3fv(&(pVerts[pFaces[i] .ausVertex[2] * 3]));

                    glVertex3fv(&(pVerts[pFaces[i] .ausVertex[2] * 3]));
                    glVertex3fv(&(pVerts[pFaces[i] .ausVertex[0] * 3]));
                }

                glEnd();
                glEnable (GL_DEPTH_TEST);
            }
            else if (m_pNode ->m_ulFlags == NwnMdlNT_AABBMesh)
            {
                UINT8 aucColor[3];
                NwnMdlFace *pFaces = (NwnMdlFace *) sRes .GetModelData (pMesh ->m_sFaces .GetOffset());
                float *pVerts = (float *) sRes .GetRawData (pMesh ->m_pavVerts .GetOffset ());

                glMaterialf (GL_FRONT, GL_SHININESS, 0.0);
                fv [0] = 0.0f;
                fv [1] = 0.0f;
                fv [2] = 0.0f;
                fv [3] = 0.9f;
                glMaterialfv (GL_FRONT, GL_AMBIENT, fv);
                glMaterialfv (GL_FRONT, GL_DIFFUSE, fv);
                glMaterialfv (GL_FRONT, GL_SPECULAR, fv);

                glDisableClientState (GL_NORMAL_ARRAY);
                glDisableClientState (GL_TEXTURE_COORD_ARRAY);
                glDisable (GL_TEXTURE_2D);
                glBegin (GL_TRIANGLES);

                for (int i = 0; i < pMesh ->m_sFaces .GetCount (); i++)
                {
                    switch (pFaces[i] .ulSurface) {
                        case  1: aucColor[0] = 160; aucColor[1] =  80; aucColor[2] =  48; break;  // Dirt (medium brown)
                        case  2: aucColor[0] = 255; aucColor[1] = 255; aucColor[2] = 255; break;  // Obscuring (white)
                        case  3: aucColor[0] =   0; aucColor[1] = 128; aucColor[2] =   0; break;  // Grass (green)
                        case  4: aucColor[0] = 128; aucColor[1] = 128; aucColor[2] = 128; break;  // Stone (grey)
                        case  5: aucColor[0] = 112; aucColor[1] = 144; aucColor[2] =  32; break;  // Wood (olive)
                        case  6: aucColor[0] =   0; aucColor[1] =   0; aucColor[2] = 192; break;  // Water (light blue)
                        case  7: aucColor[0] = 176; aucColor[1] =   0; aucColor[2] = 255; break;  // Nonwalk (purple)
                        case  8: aucColor[0] =  64; aucColor[1] =  64; aucColor[2] =  64; break;  // Transparent (dark grey)
                        case  9: aucColor[0] = 255; aucColor[1] = 255; aucColor[2] =   0; break;  // Carpet (yellow)
                        case 10: aucColor[0] =   0; aucColor[1] = 192; aucColor[2] = 192; break;  // Metal (light cyan)
                        case 11: aucColor[0] =   0; aucColor[1] =  64; aucColor[2] = 192; break;  // Puddles (medium cyan)
                        case 12: aucColor[0] =   0; aucColor[1] =  64; aucColor[2] =   0; break;  // Swamp (dark green)
                        case 13: aucColor[0] =  96; aucColor[1] =  64; aucColor[2] =  32; break;  // Mud (dark brown)
                        case 14: aucColor[0] =   0; aucColor[1] = 192; aucColor[2] =   0; break;  // Leaves (light green)
                        case 15: aucColor[0] = 192; aucColor[1] =   0; aucColor[2] =   0; break;  // Lava (red)
                        case 16: aucColor[0] = 128; aucColor[1] =   0; aucColor[2] =  64; break;  // Bottomless Pit (dark purple)
                        case 17: aucColor[0] =   0; aucColor[1] =   0; aucColor[2] =  96; break;  // Deep Water (dark blue)
                        case 18: aucColor[0] = 255; aucColor[1] = 255; aucColor[2] =   0; break;  // Door (yellow)
                        case 19: aucColor[0] = 192; aucColor[1] = 192; aucColor[2] = 192; break;  // Snow (light grey)
                        case 20: aucColor[0] = 240; aucColor[1] = 240; aucColor[2] = 144; break;  // Sand (light brown)
                        case 21: aucColor[0] = 128; aucColor[1] = 128; aucColor[2] = 128; break;  // Barebones (grey)
                        case 22: aucColor[0] = 128; aucColor[1] = 128; aucColor[2] = 128; break;  // Stone Bridge (grey)
                        case 30: aucColor[0] = 255; aucColor[1] = 192; aucColor[2] = 192; break;  // Trigger (pink)
                        default: aucColor[0] =   0; aucColor[1] =   0; aucColor[2] =   0; break;  // Default (black)
                    }

                    fv [0] = (float)aucColor[0] / 255.0f;
                    fv [1] = (float)aucColor[1] / 255.0f;
                    fv [2] = (float)aucColor[2] / 255.0f;
                    glMaterialfv (GL_FRONT, GL_EMISSION, fv);

                    glVertex3fv(&(pVerts[pFaces[i] .ausVertex[0] * 3]));
                    glVertex3fv(&(pVerts[pFaces[i] .ausVertex[1] * 3]));
                    glVertex3fv(&(pVerts[pFaces[i] .ausVertex[2] * 3]));
                }

                glEnd();
            }
            else
            {
                //
                // Initialize the veritices
                //

                glVertexPointer (3, GL_FLOAT, 0, (float *) sRes .GetRawData (
                        pMesh ->m_pavVerts .GetOffset ()));

                //
                // Initialize the normal array
                //

                if (pMesh ->m_pavNormals .GetOffset () != 0xFFFFFFFF)
                {
                    glNormalPointer (GL_FLOAT, 0, (float *) sRes .GetRawData (
                            pMesh ->m_pavNormals .GetOffset ()));
                    glEnableClientState (GL_NORMAL_ARRAY);
                }
                else
                    glDisableClientState (GL_NORMAL_ARRAY);

                //
                // Initialize the tex coords
                //

                if (pMesh ->m_pavTVerts [0] .GetOffset () != 0xFFFFFFFF)
                {
                    glTexCoordPointer (2, GL_FLOAT, 0, (float *) sRes .GetRawData (
                            pMesh ->m_pavTVerts [0] .GetOffset ()));
                    glEnableClientState (GL_TEXTURE_COORD_ARRAY);
                }
                else
                    glDisableClientState (GL_TEXTURE_COORD_ARRAY);

                UINT32 *pulCounts = pMesh ->m_sVertexIndicesCount .
                    GetData (sRes .GetModelData ());
                CNwnPointer <UINT16> *pulOffsets = pMesh ->m_sRawVertexIndices .
                    GetData (sRes .GetModelData ());

                for (int i = 0; i < pMesh ->m_sRawVertexIndices .GetCount (); i++)
                {
                    UINT16 *pusFaces = (UINT16 *) 
                        sRes .GetRawData (pulOffsets [i] .GetOffset ());
                    glDrawElements (GL_TRIANGLES, pulCounts [i],
                                    GL_UNSIGNED_SHORT, pusFaces);
                }
            }
        }
    } else if (!m_fMesh &&
               (nFlags & (m_pNode -> m_ulFlags == NwnMdlNT_Emitter ? MDL_RENDER_SHOW_EMITTER :
                         (m_pNode -> m_ulFlags == NwnMdlNT_Light  ? MDL_RENDER_SHOW_LIGHT   :
                         (m_pParent == NULL ? MDL_RENDER_SHOW_ROOT :
                          MDL_RENDER_SHOW_DUMMY))))) {
        float fv[4];

        glMaterialf (GL_FRONT, GL_SHININESS, 0.0);

        /* set up the actual node colors */
        fv [0] = 0.0f;
        fv [1] = 0.0f;
        fv [2] = 0.0f;
        fv [3] = 0.9f;
        glMaterialfv (GL_FRONT, GL_AMBIENT, fv);
        glMaterialfv (GL_FRONT, GL_DIFFUSE, fv);
        glMaterialfv (GL_FRONT, GL_SPECULAR, fv);

        //
        // XXX: support wirecolor in the future
        //
        if (m_pParent == NULL) {
            fv [0] = 0.0f;
            fv [1] = 1.0f;
            fv [2] = 1.0f;
            fv [3] = 1.0f;
        } else if (m_pNode -> m_ulFlags == NwnMdlNT_Emitter) {
            fv [0] = 1.0f;
            fv [1] = 0.0f;
            fv [2] = 1.0f;
            fv [3] = 1.0f;
        } else if (m_pNode -> m_ulFlags == NwnMdlNT_Light) {
            fv [0] = 1.0f;
            fv [1] = 1.0f;
            fv [2] = 0.0f;
            fv [3] = 1.0f;
        } else {
            fv [0] = 1.0f;
            fv [1] = 1.0f;
            fv [2] = 1.0f;
            fv [3] = 1.0f;
        }

        glMaterialfv (GL_FRONT, GL_EMISSION, fv);

        glDisableClientState (GL_NORMAL_ARRAY);
        glDisableClientState (GL_TEXTURE_COORD_ARRAY);
        glDisable (GL_TEXTURE_2D);
        glDisable (GL_DEPTH_TEST);

        glLineWidth(1.0);
        glBegin (GL_LINES);

        GLUquadricObj *quadric = gluNewQuadric();
        gluQuadricDrawStyle(quadric, GLU_LINE);

        if (m_pParent == NULL) {
            /* draw twice to fix odd bug */
            gluDisk(quadric, 0.05, 0.25, 16, 6);
            gluDisk(quadric, 0.05, 0.25, 16, 6);
        } else if (m_pNode -> m_ulFlags == NwnMdlNT_Light)
            gluSphere(quadric, 0.1, 10, 10);
        else
            gluCylinder(quadric, 0.05, 0.0, 0.2, 10, 12);

        gluDeleteQuadric(quadric);
        glEnd();
        glEnable (GL_DEPTH_TEST);
    } else if (m_pParent == NULL && (nFlags & MDL_RENDER_DRAW_AXES)) {
        float fv[4];

        glMaterialf (GL_FRONT, GL_SHININESS, 0.0);

        /* set up the actual axis colors */
        fv [0] = 0.0f;
        fv [1] = 0.0f;
        fv [2] = 0.0f;
        fv [3] = 0.5f;
        glMaterialfv (GL_FRONT, GL_AMBIENT, fv);
        glMaterialfv (GL_FRONT, GL_DIFFUSE, fv);
        glMaterialfv (GL_FRONT, GL_SPECULAR, fv);

        fv [0] = 1.0f;
        fv [1] = 1.0f;
        fv [2] = 1.0f;
        fv [3] = 1.0f;
        glMaterialfv (GL_FRONT, GL_EMISSION, fv);

        glDisableClientState (GL_NORMAL_ARRAY);
        glDisableClientState (GL_TEXTURE_COORD_ARRAY);
        glDisable (GL_TEXTURE_2D);
        glDisable (GL_DEPTH_TEST);

        glLineWidth(1.0);
        glBegin (GL_LINES);

        glVertex3f(-100.0f,    0.0f,    0.0f);
        glVertex3f( 100.0f,    0.0f,    0.0f);

        glVertex3f(   0.0f, -100.0f,    0.0f);
        glVertex3f(   0.0f,  100.0f,    0.0f);

        glVertex3f(   0.0f,    0.0f, -100.0f);
        glVertex3f(   0.0f,    0.0f,  100.0f);

#if 0
        glEnd();

        /* set up the disk colors */
        fv [0] = 0.1f;
        fv [1] = 0.1f;
        fv [2] = 0.1f;
        fv [3] = 0.2f;
        glMaterialfv (GL_FRONT, GL_AMBIENT, fv);
        glMaterialfv (GL_FRONT, GL_DIFFUSE, fv);
        glMaterialfv (GL_FRONT, GL_SPECULAR, fv);

        fv [0] = 0.1f;
        fv [1] = 0.1f;
        fv [2] = 0.1f;
        fv [3] = 0.1f;
        glMaterialfv (GL_FRONT, GL_EMISSION, fv);

        /* draw disks on the X-Y plane */
        glBegin (GL_TRIANGLES);

        GLUquadricObj *quadric = gluNewQuadric();
        gluQuadricDrawStyle(quadric, GLU_FILL);

        /* gluDisk(quadric, 100.0, 100.0, 128, 1); */
        gluDisk(quadric,  0.0,   5.0, 128, 10);
        gluDisk(quadric,  0.0,  10.0, 256, 20);
        gluDisk(quadric,  0.0,  15.0, 384, 30);
        gluDisk(quadric,  0.0,  20.0, 512, 40);

        gluDeleteQuadric(quadric);
#else
        GLUquadricObj *quadric = gluNewQuadric();
        gluQuadricDrawStyle(quadric, GLU_LINE);

        gluDisk(quadric, 100.0, 100.0, 128, 1);
        gluDisk(quadric,   5.0,   5.0, 128, 1);
        gluDisk(quadric,  10.0,  10.0, 256, 1);
        gluDisk(quadric,  15.0,  15.0, 384, 1);
        gluDisk(quadric,  20.0,  20.0, 512, 1);
        gluDisk(quadric,  25.0,  25.0, 640, 1);

        gluDeleteQuadric(quadric);
#endif

        glEnd();
        glEnable (GL_DEPTH_TEST);
    }

    //
    // Handle the children
    //

    for (int i = 0; i < m_apRtNodes .GetCount (); i++)
        m_apRtNodes [i] ->Render (sRes, fTime, nFlags);

    //
    // Restore the matrix
    //

    glPopMatrix ();
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Load the textures
//
// @parm CResModelRef & | sRes | Resource 
//
// @parm int * | pasPalette | Texture palette
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMdlRtNode::LoadTextures (CResModelRef &sRes, int *pasPalette)
{

    //
    // If we have a texture, load it
    //

    if (m_fMesh)
    {
        CNwnMdlMeshHeader *pMesh = (CNwnMdlMeshHeader *) m_pNode;
        const char *szTexture = pMesh ->m_szTextures[0];

        if (g_fViewModelsMdlNameTex) {
            CNwnMdlModel *pHeader = (CNwnMdlModel *) sRes .GetModelData ();
            szTexture = pHeader ->m_szName;
        }

        if (szTexture != NULL && szTexture [0] != 0)
        {
            m_pTexture = g_sResourceCache .LoadTexture (
                sRes ->m_pModuleFile, sRes ->m_pModuleFile == NULL &&
                sRes ->m_pKeyFile == NULL ? 
                (const char *) sRes ->m_strResName : NULL,
                szTexture, pasPalette);
        }
    }

    //
    // Handle the children
    //

    for (int i = 0; i < m_apRtNodes .GetCount (); i++)
        m_apRtNodes [i] ->LoadTextures (sRes, pasPalette);
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Unload the textures
//
// @parm CResModelRef & | sRes | Resource 
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMdlRtNode::UnloadTextures (CResModelRef &sRes)
{

    //
    // Clear the texture
    //

    m_pTexture = NULL;

    //
    // Handle the children
    //

    for (int i = 0; i < m_apRtNodes .GetCount (); i++)
        m_apRtNodes [i] ->UnloadTextures (sRes);
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Process the controller data
//
// @parm NwnMdlControllerKey * | paControllerKey | Controller key list
//
// @parm int | nControllerKeyCount | Controller key count
//
// @parm float * | pafControllerData | Controller data
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CMdlRtNode::ProcessControllers (NwnMdlControllerKey *paControllerKey, 
                                     int nControllerKeyCount, float *pafControllerData)
{

    //
    // Look for controller data we are interested in
    //

    for (int i = 0; i < nControllerKeyCount; i++)
    {
        if (paControllerKey [i] .lType == NwnMdlCT_Position)
        {
            float *pafData = &pafControllerData [
                paControllerKey [i] .sDataOffset];
            m_vPosition .m_x = pafData [0];
            m_vPosition .m_y = pafData [1];
            m_vPosition .m_z = pafData [2];
        }
        else if (paControllerKey [i] .lType == NwnMdlCT_Orientation)
        {
            float *pafData = &pafControllerData [
                paControllerKey [i] .sDataOffset];
            m_qOrientation .m_x = pafData [0];
            m_qOrientation .m_y = pafData [1];
            m_qOrientation .m_z = pafData [2];
            m_qOrientation .m_w = pafData [3];
        }
        else if (paControllerKey [i] .lType == NwnMdlCT_Scale)
        {
            float *pafData = &pafControllerData [
                paControllerKey [i] .sDataOffset];
            m_fScale = pafData [0];
        }
        else if (paControllerKey [i] .lType == NwnMdlCT_Alpha)
        {
            float *pafData = &pafControllerData [
                paControllerKey [i] .sDataOffset];
            m_fAlpha = pafData [0];
        }
    }
}


/* vim: set sw=4: */
