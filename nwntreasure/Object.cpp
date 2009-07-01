//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      Object.cpp - Core object |
//
// This module contains the core object.
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

#include "stdafx.h"
#include "resource.h"
#include "Object.h"

//
// Debug NEW
//

#if defined (_DEBUG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void DeleteTheItem (CTreeViewCtrl &tv, HTREEITEM hItem)
{
    HTREEITEM hParent = tv .GetParentItem (hItem);
    tv .DeleteItem (hItem);
    if (hParent != NULL)
    {
        if (tv .GetChildItem (hParent) == NULL)
        {
            tv .SetItemState (hParent, 0, 
                              TVIS_EXPANDEDONCE | TVIS_EXPANDED);
        }
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CObject> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CObject::CObject ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CObject> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CObject::~CObject ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc Add this object to the treeview
//
// @parm CTreeViewCtrl & | tv | Tree view
//
// @parm HTREEITEM | hParent | Parent of the item
//
// @parm HTREEITEM | hInsertAfter | Item to insert after
//
// @rdesc Handle of the added item
//
//-----------------------------------------------------------------------------

HTREEITEM CObject::AddToTV (CTreeViewCtrl &tv, 
                            HTREEITEM hParent, HTREEITEM hInsertAfter)
{
    TVINSERTSTRUCT tvi;

    //
    // Iniitalize tree view item
    //

    tvi .hParent = hParent;
    tvi .hInsertAfter = hInsertAfter;
    tvi .item .mask = TVIF_CHILDREN | TVIF_IMAGE | 
        TVIF_SELECTEDIMAGE | TVIF_PARAM | TVIF_TEXT;
    tvi .item .hItem = 0;
    tvi .item .state = 0;
    tvi .item .stateMask = 0;
    tvi .item .pszText = LPSTR_TEXTCALLBACK;
    tvi .item .cchTextMax = 0;
    tvi .item .iImage = I_IMAGECALLBACK;
    tvi .item .iSelectedImage = I_IMAGECALLBACK;
    tvi .item .cChildren = I_CHILDRENCALLBACK;
    tvi .item .lParam = (LPARAM) this;
    HTREEITEM hTI = tv .InsertItem (&tvi);
    if (hTI != NULL)
        m_ahtvItem .Add (hTI);
    return hTI;
}

//-----------------------------------------------------------------------------
//
// @mfunc Delete from the TV
//
// @parm CMainWnd * | pMainWnd | Main window
//
// @parm CTreeViewCtrl * | tv | Tree view
//
// @parm HTREEITEM | hItem | Item handle being deleted.  If NULL,
//              delete all handles used by this object.
//
// @rdesc true if the item was deleted.
//
//-----------------------------------------------------------------------------

bool CObject::DeleteFromTV (CMainWnd *pMainWnd, 
                            CTreeViewCtrl &tv, HTREEITEM hItem)
{

    //
    // If we are to delete all
    //

    if (hItem == NULL)
    {

        //
        // Just loop through the handles and remove them
        //

        while (m_ahtvItem .GetCount () > 0)
        {
            if (!DeleteFromTV (pMainWnd, tv, m_ahtvItem [0]))
                m_ahtvItem .RemoveAt (0);
        }
        return true;
    }

    //
    // Otherwise, we are deleting a specific one
    //

    else
    {

        //
        // Loop through the list and locate the item
        //

        int nIndex = GetHItemIndex (hItem);
        if (nIndex >= 0)
        {
            m_ahtvItem .RemoveAt (nIndex);
            DeleteChildrenFromTV (pMainWnd, tv, hItem);
            DeleteTheItem (tv, hItem);
            return true;
        }
        else
            return false;
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Delete all children from the TV
//
// @parm CMainWnd * | pMainWnd | Main window
//
// @parm CTreeViewCtrl * | tv | Tree view
//
// @parm HTREEITEM | hItem | Item handle who's children are to be deleted.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CObject::DeleteChildrenFromTV (CMainWnd *pMainWnd, 
                                    CTreeViewCtrl &tv, HTREEITEM hItem)
{
    while (true)
    {
        HTREEITEM hChild = tv .GetChildItem (hItem);
        if (hChild == NULL)
            return;
        if (!DeleteChildFromTV (pMainWnd, tv, hChild))
            DeleteTheItem (tv, hItem);
    }
}


/* vim: set sw=4: */
