//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      HierarchyView.cpp - Hierarchy view window |
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
// $History: HierarchyView.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "HierarchyView.h"
#include "../_NwnLib/NwnDialogTlkFile.h"

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

extern CNwnDialogTlkFile g_sDialogTlkFile;

//-----------------------------------------------------------------------------
//
// @mfunc <c CHierarchyView> constructor.
//
// @parm CData * | pFile | Hierarchy file
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CHierarchyView::CHierarchyView (CData *pFile)
{

    //
    // Initialize variables
    //

    m_sRes .Set (pFile);
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CHierarchyView> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CHierarchyView::~CHierarchyView ()
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

LRESULT CHierarchyView::OnCreate (UINT uMsg, 
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
    hdi .pszText = _T ("Variable");
    hdi .cxy = 200;
    m_header .InsertItem (0, &hdi);
    hdi .pszText = _T ("Type");
    hdi .cxy = 75;
    m_header .InsertItem (1, &hdi);
    hdi .pszText = _T ("Value");
    hdi .cxy = 10000;
    m_header .InsertItem (2, &hdi);

    //
    // Set the fonts
    //

    SetFont ((HFONT) GetStockObject (DEFAULT_GUI_FONT));
    m_tv .SetFont ((HFONT) GetStockObject (DEFAULT_GUI_FONT));
    m_header .SetFont ((HFONT) GetStockObject (DEFAULT_GUI_FONT));

    //
    // Write the first entry
    //

    if (m_sRes != NULL && m_sRes .GetEntryCount () > 0)
    {
        HTREEITEM hItem = AddToTV (-1, TVI_ROOT);
        m_tv .Expand (hItem, TVE_EXPAND);
    }

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

LRESULT CHierarchyView::OnTVGetDispInfo (int idCtrl, LPNMHDR pnmh, BOOL &bHandled) 
{

    //
    // Get the object
    //

    TV_DISPINFO *pTVDispInfo = (TV_DISPINFO *) pnmh;
    TVITEM *pTVItem = &pTVDispInfo ->item;
    int nEntryElement = (int) pTVItem ->lParam;

    //
    // Get the text
    //

    GetElementText (nEntryElement, m_szTVText, _countof (m_szTVText), 0);

    //
    // Get the child count
    //

    bool fHasChildren = nEntryElement < 0;
    if (!fHasChildren)
    {
        const CResHierarchyRef::Element *psElement = 
            m_sRes .GetElement (nEntryElement);
        if (psElement ->ulType == CResHierarchyRef::ElementType_LIST)
        {
            int nTable = psElement ->u32Data;
            nTable /= sizeof (UINT32);
            int nCount2 = m_sRes .GetList (nTable++);
            fHasChildren = nCount2 != 0;
        }
        else if (psElement ->ulType == CResHierarchyRef::ElementType_CAPREF)
        {
            fHasChildren = true;
        }
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

LRESULT CHierarchyView::OnTVItemExpanding (int idCtrl, LPNMHDR pnmh, BOOL &bHandled) 
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

LRESULT CHierarchyView::OnTVSelChanged (int idCtrl, LPNMHDR pnmh, BOOL &bHandled) 
{
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Add this object to the treeview
//
// @parm int | nEntryElement | If negative, this is an entry.  If positive
//              this is an element
//
// @parm HTREEITEM | hParent | Parent of the item
//
// @rdesc Handle of the added item
//
//-----------------------------------------------------------------------------

HTREEITEM CHierarchyView::AddToTV (int nEntryElement, HTREEITEM hParent)
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
    tvi .item .iSelectedImage = I_IMAGECALLBACK;
    tvi .item .cChildren = I_CHILDRENCALLBACK;
    tvi .item .lParam = (LPARAM) nEntryElement;
    HTREEITEM hTI = m_tv .InsertItem (&tvi);
    return hTI;
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the element text
//
// @parm int | nEntryElement | If negative, this is an entry.  If positive
//              this is an element
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

void CHierarchyView::GetElementText (int nEntryElement, 
                                     LPTSTR pszText, int cchMaxText, int nColumn)
{
    if (nEntryElement < 0)
    {
        nEntryElement = - nEntryElement - 1;
        const CResHierarchyRef::Entry *psEntry = 
            m_sRes .GetEntry (nEntryElement);
        if (nColumn == 0)
            _snprintf (pszText, cchMaxText, "Entry_%d", nEntryElement);
        else if (nColumn == 1)
            _snprintf (pszText, cchMaxText, "ENTRY");
        else if (nColumn == 2)
            _snprintf (pszText, cchMaxText, "%d", psEntry ->ulCode);
        else
            _ASSERTE (FALSE);
    }
    else
    {
        const CResHierarchyRef::Element *psElement = 
            m_sRes .GetElement (nEntryElement);
        if (nColumn == 0)
        {
            if (psElement ->ulVarName >= m_sRes .GetVarNameCount ())
                _snprintf (pszText, cchMaxText, "Variable_%d", psElement ->ulVarName);
            else
            {
                if (cchMaxText > 17)
                    cchMaxText = 17;
                const CResHierarchyRef::VarName *pVarName = 
                    m_sRes .GetVarName (psElement ->ulVarName);
                strncpy (pszText, pVarName ->szName, cchMaxText);
                pszText [cchMaxText - 1] = 0;
            }
        }
        else if (nColumn == 1)
        {
            switch (psElement ->ulType)
            {
                case CResHierarchyRef::ElementType_UINT8:
                    strncpy (pszText, "UINT8", cchMaxText);
                    break;

                case CResHierarchyRef::ElementType_UINT16:
                    strncpy (pszText, "UINT16", cchMaxText);
                    break;

                case CResHierarchyRef::ElementType_UINT32:
                    strncpy (pszText, "UINT32", cchMaxText);
                    break;

                case CResHierarchyRef::ElementType_INT8:
                    strncpy (pszText, "INT8", cchMaxText);
                    break;

                case CResHierarchyRef::ElementType_INT16:
                    strncpy (pszText, "INT16", cchMaxText);
                    break;

                case CResHierarchyRef::ElementType_INT32:
                    strncpy (pszText, "INT32", cchMaxText);
                    break;

                case CResHierarchyRef::ElementType_UINT64:
                    strncpy (pszText, "UINT64", cchMaxText);
                    break;

                case CResHierarchyRef::ElementType_INT64:
                    strncpy (pszText, "INT64", cchMaxText);
                    break;

                case CResHierarchyRef::ElementType_FLOAT:
                    strncpy (pszText, "FLOAT", cchMaxText);
                    break;

                case CResHierarchyRef::ElementType_DOUBLE:
                    strncpy (pszText, "DOUBLE", cchMaxText);
                    break;

                case CResHierarchyRef::ElementType_STRING:
                    strncpy (pszText, "STRING", cchMaxText);
                    break;

                case CResHierarchyRef::ElementType_RESREF:
                    strncpy (pszText, "RESREF", cchMaxText);
                    break;

                case CResHierarchyRef::ElementType_STRREF:
                    strncpy (pszText, "STRREF", cchMaxText);
                    break;

                case CResHierarchyRef::ElementType_DATREF:
                    strncpy (pszText, "DATREF", cchMaxText);
                    break;

                case CResHierarchyRef::ElementType_CAPREF:
                    strncpy (pszText, "CAPREF", cchMaxText);
                    break;

                case CResHierarchyRef::ElementType_LIST:
                    strncpy (pszText, "LIST", cchMaxText);
                    break;

                default:
                    _snprintf (pszText, cchMaxText, "TYPE_%d", 
                               psElement ->ulType);
                    break;
            }
        }
        else if (nColumn == 2)
        {
            switch (psElement ->ulType)
            {
                case CResHierarchyRef::ElementType_UINT8:
                    sprintf (pszText, "%u", psElement ->u8Data);
                    break;

                case CResHierarchyRef::ElementType_UINT16:
                    sprintf (pszText, "%u", psElement ->u16Data);
                    break;

                case CResHierarchyRef::ElementType_UINT32:
                    if (m_sRes .IsElement (psElement, "STRREF") ||
                        m_sRes .IsElement (psElement, "Obj_StrRef") ||
                        m_sRes .IsElement (psElement, "StrRef") ||
                        m_sRes .IsElement (psElement, "Text") ||
                        m_sRes .IsElement (psElement, "CatName"))
                    {
                        if (psElement ->u32Data == 0xFFFFFFFF)
                        {
                            pszText [0] = 0;
                        }
                        else if (!g_sDialogTlkFile .IsOpen () || 
                                 (g_sDialogTlkFile .GetString (psElement ->u32Data, pszText, cchMaxText) < 0 &&
                                  g_sDialogTlkFile .GetStringName (psElement ->u32Data, pszText, cchMaxText) < 0))
                        {
                            _snprintf (pszText, cchMaxText, "STRREF=%u", 
                                       psElement ->u32Data);
                        }
                    }
                    else
                        sprintf (pszText, "%u", psElement ->u32Data);
                    break;

                case CResHierarchyRef::ElementType_INT8:
                    sprintf (pszText, "%d", psElement ->n8Data);
                    break;

                case CResHierarchyRef::ElementType_INT16:
                    sprintf (pszText, "%d", psElement ->n16Data);
                    break;

                case CResHierarchyRef::ElementType_INT32:
                    sprintf (pszText, "%d", psElement ->n32Data);
                    break;

                case CResHierarchyRef::ElementType_UINT64:
                    {
                        UINT64 *pu64 = (UINT64 *) m_sRes .GetVarData (psElement ->n32Data);
                        sprintf (pszText, "%I64u", *pu64);
                    }
                    break;

                case CResHierarchyRef::ElementType_INT64:
                    {
                        INT64 *p64 = (INT64 *) m_sRes .GetVarData (psElement ->n32Data);
                        sprintf (pszText, "%I64d", *p64);
                    }
                    break;

                case CResHierarchyRef::ElementType_FLOAT:
                    sprintf (pszText, "%f", psElement ->fData);
                    break;

                case CResHierarchyRef::ElementType_DOUBLE:
                    {
                        double *pd = (double *) m_sRes .GetVarData (psElement ->n32Data);
                        sprintf (pszText, "%f", *pd);
                    }
                    break;

                case CResHierarchyRef::ElementType_STRING:
                    m_sRes .GetElementLongString (psElement, pszText, cchMaxText);
                    break;

                case CResHierarchyRef::ElementType_RESREF:
                    m_sRes .GetElementShortString (psElement, pszText, cchMaxText);
                    break;

                case CResHierarchyRef::ElementType_STRREF:
                    {
                        const CResHierarchyRef::StrRef *pStrRef = 
                            m_sRes .GetElementStrRef (psElement);
                        if (pStrRef ->ulStrRef == 0xFFFFFFFF)
                        {
                            if (pStrRef ->ulLangSpecCount > 0)
                            {
                                const CResHierarchyRef::StrRefLangSpec *pString =
                                    (const CResHierarchyRef::StrRefLangSpec *) 
                                    &pStrRef [1];
                                cchMaxText--;
                                if (cchMaxText > pString ->ulLength)
                                    cchMaxText = pString ->ulLength;
                                memmove (pszText, pString ->szText, cchMaxText);
                                pszText [cchMaxText] = 0;

                            }
                            else
                                pszText [0] = 0;
                        }
                        else if (!g_sDialogTlkFile .IsOpen () || 
                                 (g_sDialogTlkFile .GetString (pStrRef ->ulStrRef, pszText, cchMaxText) < 0 &&
                                  g_sDialogTlkFile .GetStringName (pStrRef ->ulStrRef, pszText, cchMaxText) < 0))
                        {
                            _snprintf (pszText, cchMaxText, "STRREF=%d", pStrRef ->ulStrRef);
                        }
                    }
                    break;

                case CResHierarchyRef::ElementType_DATREF:
                    _snprintf (pszText, cchMaxText, "Binary Data");
                    break;

                case CResHierarchyRef::ElementType_CAPREF:
                    _snprintf (pszText, cchMaxText, "%d", 
                               psElement ->u32Data);
                    break;

                case CResHierarchyRef::ElementType_LIST:
                    _snprintf (pszText, cchMaxText, "%d", 
                               psElement ->u32Data / sizeof (UINT32));
                    break;

                default:
                    _snprintf (pszText, cchMaxText, "%d,%08X", 
                               psElement ->u32Data, psElement ->u32Data);
                    break;
            }
        }
        else
            _ASSERTE (FALSE);
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Add the elements for the given list
//
// @parm int | nEntryElement | If negative, this is an entry.  If positive
//              this is an element
//
// @parm HTREEITEM | hParent | Parent
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CHierarchyView::AddChildren (int nEntryElement, HTREEITEM hParent)
{
    if (nEntryElement < 0)
    {
        nEntryElement = - nEntryElement - 1;
        CResHierarchyRef::Entry *pEntry = m_sRes .GetEntry (nEntryElement);
        UINT32 *pulMap = m_sRes .GetMultiMapTable (pEntry);
        for (int i = 0; i < pEntry ->ulCount; i++)
            AddToTV (pulMap [i], hParent);
    }
    else
    {
        CResHierarchyRef::Element *psElement = 
            m_sRes .GetElement (nEntryElement);
        if (psElement ->ulType == CResHierarchyRef::ElementType_LIST)
        {
            int nTable = psElement ->u32Data;
            nTable /= sizeof (UINT32);
            int nCount2 = m_sRes .GetList (nTable++);
            for (int i = 0; i < nCount2; i++)
                AddToTV (- ((int) m_sRes .GetList (nTable++)) - 1, hParent);
        }
        else if (psElement ->ulType == CResHierarchyRef::ElementType_CAPREF)
        {
            AddToTV (- ((int) psElement ->u32Data) - 1, hParent);
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

void CHierarchyView::DrawTreeItem (HDC hDC, HTREEITEM hItem, 
                                   UINT iState, RECT &rcItem)
{
    CDCHandle dc (hDC);
    CRect rectItemHeader;
    m_header .GetItemRect (0, &rectItemHeader);

    //
    // Get the element data
    //

    int nEntryElement = (int) m_tv .GetItemData (hItem);

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
    for (int i = 0; i < 3; i++)
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
        GetElementText (nEntryElement, szText, _countof (szText), i);

        //
        // And draw
        //

        ::DrawText (hDC, szText, -1, &rc, 
                    DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
    }
}



/* vim: set sw=4: */
