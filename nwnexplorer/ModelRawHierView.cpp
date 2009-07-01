//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ModelRawHierView.cpp - Hierarchy view window |
//
// This module contains the definition of the hierarchy view window.
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
// $History: ModelRawHierView.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "ModelRawHierView.h"
#include "ModelRawFormatter.h"
#include "../_NmcLib/Nmc.h"

//
// Debug NEW
//

#if defined (_DEBUG) && !defined (_WIN32_WCE)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// Globals
//

//-----------------------------------------------------------------------------
//
// @mfunc <c CModelRawHierView> constructor.
//
// @parm CData * | pFile | Hierarchy file
//
// @parm bool | fFormatted | If true, decode binary models
//
// @parm bool | fCompiled | If true, this model was compiled
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

    CModelRawHierView::CModelRawHierView (CData *pFile, bool fFormatted, bool fCompiled)
: m_wnd (this, pFile)
{

    //
    // Initialize variables
    //

    m_fFormatted = fFormatted;
    m_fCompiled = fCompiled;
    m_sRes .Set (pFile);
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CModelRawHierView> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CModelRawHierView::~CModelRawHierView ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc Initialize the window
//
// @parm UINT | uMsg | Message
//
// @parm WPARAM | wParam | Message w-parameter
//
// @parm LPARAM | lParam | Message l-parameter
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CModelRawHierView::OnCreate (UINT uMsg, 
                                     WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{

    //
    // Create the treeview
    //

    m_wnd .Create (m_hWnd, rcDefault, _T (""), 
                   WS_CHILD | WS_VISIBLE, WS_EX_CLIENTEDGE, (HMENU) 0, 0);

    //
    // Create the text window
    //

    m_wndText .Create (m_hWnd, rcDefault, _T (""),
                       WS_CHILD | WS_VISIBLE, WS_EX_CLIENTEDGE, (HMENU) 0, 0);

    //
    // Set the size of the splitter
    //

    SetSplitterExtendedStyle (0, SPLIT_PROPORTIONAL);
    CRect rect (0, 0, 1000, 1000);
    SetSplitterRect (rect);
    SetSplitterPos (250);

    //
    // Set the splitter panes
    //

    SetSplitterPanes (m_wnd .m_hWnd, m_wndText .m_hWnd);
    bHandled = FALSE;

    //
    // Update bottom
    //

    OnNodeChange (0, TVI_ROOT);
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle a change in selection
//
// @parm int | nOffset | New offset
//
// @parm HTREEITEM | hItem | Item in the tree view
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawHierView::OnNodeChange (int nOffset, HTREEITEM hItem)
{
    CNwnMemoryStream sStream;
    if (!m_fFormatted)
    {
        CModelRawFormatter s (m_sRes, &sStream);

        if (m_sRes .IsGeometryHeader (nOffset))
        {
            CNwnMdlGeometryHeader *pHeader = (CNwnMdlGeometryHeader *)
                m_sRes .GetModelData (nOffset);
            switch (pHeader ->m_ucGeometryType)
            {
                case NwnMdlGT_Model:
                    s .WriteModelHeader (nOffset, sizeof (CNwnMdlModel));
                    break;

                case NwnMdlGT_Animation:
                    s .WriteAnimationHeader (nOffset, sizeof (CNwnMdlAnimation));
                    break;

                default:
                    s .Format ("Unknown\r\n");
                    break;
            }
        }
        else
        {
            CNwnMdlNodeHeader *pHeader = (CNwnMdlNodeHeader *)
                m_sRes .GetModelData (nOffset);
            switch (pHeader ->m_ulFlags)
            {
                case NwnMdlNT_Dummy:
                    s .WriteDummyNode (nOffset, sizeof (CNwnMdlDummyNode));
                    break;

                case NwnMdlNT_TriMesh:
                    s .WriteTriMeshNode (nOffset, sizeof (CNwnMdlTriMeshNode));
                    break;

                case NwnMdlNT_AnimMesh:
                    s .WriteAnimMeshNode (nOffset, sizeof (CNwnMdlAnimMeshNode));
                    break;

                case NwnMdlNT_Emitter:
                    s .WriteEmitterNode (false, nOffset, sizeof (CNwnMdlEmitterNode));
                    break;

                case NwnMdlNT_Light:
                    s .WriteLightNode (false, nOffset, sizeof (CNwnMdlLightNode));
                    break;

                case NwnMdlNT_AABBMesh:
                    s .WriteAABBNode (nOffset, sizeof (CNwnMdlAABBMeshNode));
                    break;

                case NwnMdlNT_DanglyMesh:
                    s .WriteDanglyMeshNode (nOffset, sizeof (CNwnMdlDanglyMeshNode));
                    break;

                case NwnMdlNT_SkinMesh:
                    s .WriteSkinNode (nOffset, sizeof (CNwnMdlSkinMeshNode));
                    break;

                case NwnMdlNT_Reference:
                    s .WriteReferenceNode (nOffset, sizeof (CNwnMdlReferenceNode));
                    break;

                default:
                    s .WriteNodeHeader (nOffset, sizeof (CNwnMdlNodeHeader));
                    break;
            }
        }
    }
    else
    {

        //
        // If we have goemetry
        //

        if (m_sRes .IsGeometryHeader (nOffset))
        {
            CNwnMdlGeometryHeader *pHeader = (CNwnMdlGeometryHeader *)
                m_sRes .GetModelData (nOffset);
            switch (pHeader ->m_ucGeometryType)
            {
                case NwnMdlGT_Model:
                    static_cast <CNwnMdlModel *> (pHeader) ->Decompile (
                        &sStream, false, false, true, m_sRes .GetModelData (), 
                        m_sRes .GetRawData ());
                    break;

                case NwnMdlGT_Animation:
                    static_cast <CNwnMdlAnimation *> (pHeader) ->Decompile (
                        &sStream, 
                        (CNwnMdlModel *) (m_sRes .GetModelData ()),
                        false, false, true, m_sRes .GetModelData (), 
                        m_sRes .GetRawData ());
                    break;

                default:
                    sStream .WriteLine ("#UNKNOWN GEOMETRY TYPE");
                    break;
            }
        }

        //
        // Otherwise, we have a node
        //

        else
        {

            //
            // Get the node in question
            //

            CNwnMdlNodeHeader *pNode = (CNwnMdlNodeHeader *)
                m_sRes .GetModelData (nOffset);

            //
            // Locate the parent node and geometry
            //

            CNwnMdlNodeHeader *pParentNode = NULL;
            CNwnMdlGeometryHeader *pParentGeom = (CNwnMdlModel *) m_sRes .GetModelData ();
            CTreeViewCtrl &tv = m_wnd .GetTreeView ();
            while (true)
            {
                hItem = tv .GetParentItem (hItem);
                if (hItem == NULL)
                {
                    assert (false);
                    break;//this is actually a failure
                }
                LPARAM lParam = tv .GetItemData (hItem);
                if (m_sRes .IsGeometryHeader (lParam))
                {
                    pParentGeom = (CNwnMdlGeometryHeader *)
                        m_sRes .GetModelData (lParam);
                    break;
                }
                else if (pParentNode == NULL)
                {
                    pParentNode = (CNwnMdlNodeHeader *)
                        m_sRes .GetModelData (lParam);
                }
            }

            //
            // Invoke the routine
            //

            pNode ->Decompile (&sStream, (CNwnMdlModel *) (m_sRes .GetModelData ()),
                               pParentGeom, pParentNode, false, false, true, m_sRes .GetModelData (), 
                               m_sRes .GetRawData ());
        }
    }

    char szNull = 0;
    sStream .Write (&szNull, 1);
    m_wndText .SetText ((char *) sStream .GetData ());
}


/* vim: set sw=4: */
