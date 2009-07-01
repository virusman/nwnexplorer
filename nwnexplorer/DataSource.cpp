//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      DataSource.cpp - Resource data source |
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
#include "DataSource.h"

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
// @mfunc <c CDataSource> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CDataSource::CDataSource ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CDataSource> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CDataSource::~CDataSource ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc Add this object to the treeview
//
// @mfunc Add the children of an element
//
// @parm CTreeViewCtrl & | tv | Tree view control
//
// @parm HTREEITEM | hParent | Parent item
//
// @parm DataElement * | pElement | Resource element
//
// @rdesc Handle of the added item
//
//-----------------------------------------------------------------------------

HTREEITEM CDataSource::AddToTV (CTreeViewCtrl &tv, 
                                HTREEITEM hParent, DataElement *pElement)
{
    TVINSERTSTRUCT tvi;

    //
    // Iniitalize tree view item
    //

    tvi .hParent = hParent;
    tvi .hInsertAfter = TVI_LAST;
    tvi .item .mask = TVIF_CHILDREN | TVIF_PARAM | TVIF_TEXT;
    tvi .item .hItem = 0;
    tvi .item .state = 0;
    tvi .item .stateMask = 0;
    tvi .item .pszText = LPSTR_TEXTCALLBACK;
    tvi .item .cchTextMax = 0;
    tvi .item .iImage = 0;
    tvi .item .iSelectedImage = 0;
    tvi .item .cChildren = I_CHILDRENCALLBACK;
    tvi .item .lParam = (LPARAM) pElement;
    HTREEITEM hTI = tv .InsertItem (&tvi);
    return hTI;
}


/* vim: set sw=4: */
