//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ModelRawHierWnd.cpp - Model raw hierarchy window |
//
// This module contains the definition of the model raw hierarchy window.
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
// $History: ModelRawHierWnd.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "ModelRawHierWnd.h"
#include "ModelRawHierView.h"

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
// @mfunc <c CModelRawHierWnd> constructor.
//
// @parm CModelRawHierView * | pParent | Parent window
//
// @parm CData * | pFile | Hierarchy file
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CModelRawHierWnd::CModelRawHierWnd (CModelRawHierView *pParent, CData *pFile)
{

    //
    // Initialize variables
    //

    m_pParent = pParent;
    m_sRes .Set (pFile);
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CModelRawHierWnd> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CModelRawHierWnd::~CModelRawHierWnd ()
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

LRESULT CModelRawHierWnd::OnCreate (UINT uMsg, 
                                    WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{
    DWORD dwStyle;

    //
    // Create the treeview
    //

    dwStyle = WS_CHILD | TVS_HASLINES | TVS_HASBUTTONS | 
        TVS_LINESATROOT | TVS_SHOWSELALWAYS | WS_VISIBLE | WS_CLIPSIBLINGS;
    m_tv .Create (m_hWnd, 0, _T (""), dwStyle, 
                  0, (HMENU) IDC_EXP_TREEVIEW, NULL);
    m_tv .SetBkColor (GetSysColor (COLOR_WINDOW));

    //
    // Create the header
    //

    dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;
    m_header .Create (m_hWnd, 0, _T (""), dwStyle,
                      0, (HMENU) IDC_EXP_HEADER, NULL);

    //
    // Add the headers
    //

    HDITEM hdi;
    memset (&hdi, 0, sizeof (hdi));
    hdi .mask = HDI_TEXT | HDI_WIDTH;
    hdi .pszText = _T ("Type");
    hdi .cxy = 300;
    m_header .InsertItem (0, &hdi);
    hdi .pszText = _T ("Name");
    hdi .cxy = 10000;
    m_header .InsertItem (1, &hdi);

    //
    // Set the fonts
    //

    SetFont ((HFONT) GetStockObject (DEFAULT_GUI_FONT));
    m_tv .SetFont ((HFONT) GetStockObject (DEFAULT_GUI_FONT));
    m_header .SetFont ((HFONT) GetStockObject (DEFAULT_GUI_FONT));

    //
    // Write the first entry
    //

    HTREEITEM hItem = AddToTV (0, TVI_ROOT);
    m_tv .Expand (hItem, TVE_EXPAND);

    //
    // Return true
    //

    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle treeview display info request
//
// @parm int | idCtrl | Id of the control
//
// @parm LPNMHDR | pnmh | Notify message header
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CModelRawHierWnd::OnTVGetDispInfo (
    int idCtrl, LPNMHDR pnmh, BOOL &bHandled) 
{

    //
    // Get the object
    //

    TV_DISPINFO *pTVDispInfo = (TV_DISPINFO *) pnmh;
    TVITEM *pTVItem = &pTVDispInfo ->item;
    int nOffset = (int) pTVItem ->lParam;

    //
    // Get the text
    //

    GetElementText (nOffset, m_szTVText, _countof (m_szTVText), 0);

    //
    // Get the child count
    //

    bool fHasChildren = false;
    if (m_sRes .IsGeometryHeader (nOffset))
    {
        CNwnMdlGeometryHeader *pHeader = (CNwnMdlGeometryHeader *)
            m_sRes .GetModelData (nOffset);
        switch (pHeader ->m_ucGeometryType)
        {
            case NwnMdlGT_Model:
                {
                    CNwnMdlModel *pModelHeader = 
                        (CNwnMdlModel *) m_sRes .GetModelData (nOffset);
                    fHasChildren = pModelHeader ->m_apAnimations .GetCount () != 0 ||
                        pModelHeader ->m_pGeometry .GetOffset () != 0;
                }
                break;

            case NwnMdlGT_Animation:
                {
                    CNwnMdlAnimation *pAnimationHeader = 
                        (CNwnMdlAnimation *) m_sRes .GetModelData (nOffset);
                    fHasChildren = pAnimationHeader ->m_pGeometry .GetOffset () != 0;
                }
                break;
        }
    }
    else
    {
        CNwnMdlNodeHeader *pNodeHeader =
            (CNwnMdlNodeHeader *) m_sRes .GetModelData (nOffset);
        fHasChildren = pNodeHeader ->m_apChildren .GetCount () != 0;
    }

    //
    // Populate the items
    //

    if (pTVItem ->mask & TVIF_CHILDREN)
        pTVItem ->cChildren = fHasChildren;
    if (pTVItem ->mask & TVIF_TEXT)
        pTVItem ->pszText = m_szTVText;
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle item expanding
//
// @parm int | idCtrl | Id of the control
//
// @parm LPNMHDR | pnmh | Notify message header
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CModelRawHierWnd::OnTVItemExpanding (
    int idCtrl, LPNMHDR pnmh, BOOL &bHandled) 
{
    NM_TREEVIEW *pNMTreeView = (NM_TREEVIEW *) pnmh;
    TVITEM *pTVItem = &pNMTreeView ->itemNew;

    //
    // If the action is expanding
    //

    if (pNMTreeView ->action == TVE_EXPAND) 
    {

        //
        // If this item has never been expanded
        //

        if ((pTVItem ->state & TVIS_EXPANDEDONCE) == 0) 
        {

            //
            // Show a wait cursor
            //

            CWaitCursor wait;

            //
            // Turn off redraw
            //

            m_tv .SendMessage (WM_SETREDRAW, FALSE, 0);

            //
            // Set the expand once flag
            //

            m_tv .SetItemState (pTVItem ->hItem, 
                                TVIS_EXPANDEDONCE, TVIS_EXPANDEDONCE);

            //
            // Add the children
            //

            AddChildren ((int) m_tv .GetItemData (
                    pTVItem ->hItem), pTVItem ->hItem);

            //
            // Turn on redraw
            //

            m_tv .SendMessage (WM_SETREDRAW, TRUE, 0);
        }
    }
    return FALSE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle selection change
//
// @parm int | idCtrl | Id of the control
//
// @parm LPNMHDR | pnmh | Notify message header
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CModelRawHierWnd::OnTVSelChanged (int idCtrl, LPNMHDR pnmh, BOOL &bHandled) 
{
    NM_TREEVIEW *pNMTreeView = (NM_TREEVIEW *) pnmh;
    TVITEM *pTVItem = &pNMTreeView ->itemNew;
    m_pParent ->OnNodeChange (pTVItem ->lParam, pTVItem ->hItem);
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Add this object to the treeview
//
// @parm int | nOffset | Offset of the data into the model
//
// @parm HTREEITEM | hParent | Parent of the item
//
// @rdesc Handle of the added item
//
//-----------------------------------------------------------------------------

HTREEITEM CModelRawHierWnd::AddToTV (int nOffset, HTREEITEM hParent)
{
    TVINSERTSTRUCT tvi;

    //
    // Iniitalize tree view item
    //

    tvi .hParent = hParent;
    tvi .hInsertAfter = TVI_LAST;
    tvi .item .mask = TVIF_CHILDREN | /* TVIF_IMAGE | */
        /* TVIF_SELECTEDIMAGE | */ TVIF_PARAM | TVIF_TEXT;
    tvi .item .hItem = 0;
    tvi .item .state = 0;
    tvi .item .stateMask = 0;
    tvi .item .pszText = LPSTR_TEXTCALLBACK;
    tvi .item .cchTextMax = 0;
    tvi .item .iImage = 0; // I_IMAGECALLBACK;
    tvi .item .iSelectedImage = I_IMAGECALLBACK;
    tvi .item .cChildren = I_CHILDRENCALLBACK;
    tvi .item .lParam = (LPARAM) nOffset;
    HTREEITEM hTI = m_tv .InsertItem (&tvi);
    return hTI;
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the element text
//
// @parm int | nOffset | Offset of the data into the model
//
// @parm LPTSTR | pszText | Destination text
//
// @parm int | cchMaxText | Length of the buffer
//
// @parm int | nColumn | Column number
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawHierWnd::GetElementText (int nOffset, 
                                       LPTSTR pszText, int cchMaxText, int nColumn)
{
    if (m_sRes .IsGeometryHeader (nOffset))
    {
        CNwnMdlGeometryHeader *pHeader = (CNwnMdlGeometryHeader *)
            m_sRes .GetModelData (nOffset);
        switch (pHeader ->m_ucGeometryType)
        {
            case NwnMdlGT_Model:
                {
                    CNwnMdlModel *pModelHeader = 
                        (CNwnMdlModel *) m_sRes .GetModelData (nOffset);
                    if (nColumn == 0)
                        strncpy (pszText, "Model Header", cchMaxText);
                    else
                        pszText [0] = 0;
                }
                break;

            case NwnMdlGT_Animation:
                {
                    CNwnMdlAnimation *pAnimationHeader = 
                        (CNwnMdlAnimation *) m_sRes .GetModelData (nOffset);
                    if (nColumn == 0)
                        strncpy (pszText, "Animation Header", cchMaxText);
                    else
                    {
                        strncpy (pszText, pAnimationHeader ->m_szName,
                                 min (cchMaxText, _countof (pAnimationHeader ->m_szName)));
                        pszText [cchMaxText - 1] = 0;
                    }
                }
                break;
        }
    }
    else
    {
        CNwnMdlNodeHeader *pNodeHeader =
            (CNwnMdlNodeHeader *) m_sRes .GetModelData (nOffset);
        if (nColumn == 0)
        {
            if (pNodeHeader ->m_ulFlags == NwnMdlNT_Dummy)
                strncpy (pszText, "dummy", cchMaxText);
            else if (pNodeHeader ->m_ulFlags == NwnMdlNT_TriMesh)
                strncpy (pszText, "triMesh", cchMaxText);
            else if (pNodeHeader ->m_ulFlags == NwnMdlNT_AnimMesh)
                strncpy (pszText, "animMesh", cchMaxText);
            else if (pNodeHeader ->m_ulFlags == NwnMdlNT_Emitter)
                strncpy (pszText, "emitter", cchMaxText);
            else if (pNodeHeader ->m_ulFlags == NwnMdlNT_Light)
                strncpy (pszText, "light", cchMaxText);
            else if (pNodeHeader ->m_ulFlags == NwnMdlNT_AABBMesh)
                strncpy (pszText, "AABB", cchMaxText);
            else if (pNodeHeader ->m_ulFlags == NwnMdlNT_DanglyMesh)
                strncpy (pszText, "danglyMesh", cchMaxText);
            else if (pNodeHeader ->m_ulFlags == NwnMdlNT_SkinMesh)
                strncpy (pszText, "skin", cchMaxText);
            else if (pNodeHeader ->m_ulFlags == NwnMdlNT_Reference)
                strncpy (pszText, "reference", cchMaxText);
            else 
                strncpy (pszText, "unknown", cchMaxText);
        }
        else
        {
            strncpy (pszText, pNodeHeader ->m_szName,
                     min (cchMaxText, _countof (pNodeHeader ->m_szName)));
            pszText [cchMaxText - 1] = 0;
        }
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Add the elements for the given list
//
// @parm int | nOffset | Offset of the data into the model
//
// @parm HTREEITEM | hParent | Parent
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawHierWnd::AddChildren (int nOffset, HTREEITEM hParent)
{
    if (m_sRes .IsGeometryHeader (nOffset))
    {
        CNwnMdlGeometryHeader *pHeader = (CNwnMdlGeometryHeader *)
            m_sRes .GetModelData (nOffset);
        switch (pHeader ->m_ucGeometryType)
        {
            case NwnMdlGT_Model:
                {
                    CNwnMdlModel *pModelHeader = 
                        (CNwnMdlModel *) m_sRes .GetModelData (nOffset);
                    if (pModelHeader ->m_pGeometry .GetOffset () != 0)
                        AddToTV (pModelHeader ->m_pGeometry .GetOffset (), hParent);
                    CNwnPointer <CNwnMdlAnimation> *pOffsets = pModelHeader ->m_apAnimations .
                        GetData (m_sRes .GetModelData ());
                    for (int i = 0; i < pModelHeader ->m_apAnimations .GetCount (); i++)
                    {
                        AddToTV (pOffsets [i] .GetOffset (), hParent);
                    }
                }
                break;

            case NwnMdlGT_Animation:
                {
                    CNwnMdlAnimation *pAnimationHeader = 
                        (CNwnMdlAnimation *) m_sRes .GetModelData (nOffset);
                    if (pAnimationHeader ->m_pGeometry .GetOffset () != 0)
                        AddToTV (pAnimationHeader ->m_pGeometry .GetOffset (), hParent);
                }
                break;
        }
    }
    else
    {
        CNwnMdlNodeHeader *pNodeHeader =
            (CNwnMdlNodeHeader *) m_sRes .GetModelData (nOffset);
        CNwnPointer <CNwnMdlNodeHeader> *pulOffsets = pNodeHeader ->
            m_apChildren .GetData (m_sRes .GetModelData ());
        for (int i = 0; i < pNodeHeader ->m_apChildren .GetCount (); i++)
        {
            AddToTV (pulOffsets [i] .GetOffset (), hParent);
        }
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Draw a tree item
//
// @parm HDC | hDC | DC of the tree view
//
// @parm HTREEITEM | hItem | Current item
//
// @parm UINT | iState | Drawing state
//
// @parm RECT & | rcItem | item rect
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CModelRawHierWnd::DrawTreeItem (HDC hDC, HTREEITEM hItem, 
                                     UINT iState, RECT &rcItem)
{
    CDCHandle dc (hDC);
    CRect rectItemHeader;
    m_header .GetItemRect (0, &rectItemHeader);

    //
    // Get the element data
    //

    int nOffset = (int) m_tv .GetItemData (hItem);

    //
    // If we are selected, then highlight
    //

    if (iState & CDIS_SELECTED) 
    {
        CRect rc (rcItem);
        if (rc .right > rectItemHeader .right)
            rc .right = rectItemHeader .right;
        COLORREF clrBack;
        if (GetFocus () == m_tv .m_hWnd)
            clrBack = GetSysColor (COLOR_HIGHLIGHT);
        else
            clrBack = GetSysColor (COLOR_3DFACE);
        COLORREF clrOld = dc .SetBkColor (clrBack);
        dc .ExtTextOut (rc .left, rc .top, 
                        ETO_OPAQUE, &rc, NULL, 0, NULL);
        dc .SetBkColor (clrOld);
    }

    //
    // If the item had focus then draw it
    //

    if (iState & CDIS_FOCUS) 
    {
        CRect rc (rcItem);
        if (rc .right > rectItemHeader .right)
            rc .right = rectItemHeader .right;
        dc .DrawFocusRect (&rc);
    }

    //
    // Always write text without background
    //

    dc .SetBkMode (TRANSPARENT);

    //
    // Draw all columns of the item
    //

    CRect rc (rcItem);
    for (int i = 0; i < 2; i++)
    {

        //
        // Get the rectangle for this item
        //

        if (i != 0)
        {
            m_header .GetItemRect (i, &rectItemHeader);
            rc .left = rectItemHeader .left;
        }
        rc .right = rectItemHeader .right;
        rc .left += 2;

        //
        // Get the text color
        //

        COLORREF clrText;
        if ((iState & CDIS_SELECTED) != 0 && i == 0) 
        {
            if (GetFocus () == m_tv .m_hWnd)
                clrText = ::GetSysColor (COLOR_HIGHLIGHTTEXT);
            else
                clrText = ::GetSysColor (COLOR_WINDOWTEXT);
        }
        else
            clrText = ::GetSysColor (COLOR_WINDOWTEXT);
        dc .SetTextColor (clrText);

        //
        // Get the text
        //

        char szText [256];
        GetElementText (nOffset, szText, _countof (szText), i);

        //
        // And draw
        //

        ::DrawText (hDC, szText, -1, &rc, 
                    DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
    }
}



/* vim: set sw=4: */
