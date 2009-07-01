//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      HierarchyRawView.cpp - Hierarchy view window |
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
// $History: HierarchyRawView.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "HierarchyRawView.h"
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
// @mfunc <c CHierarchyRawView> constructor.
//
// @parm CData * | pFile | Hierarchy file
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CHierarchyRawView::CHierarchyRawView (CData *pFile) : 
    CTextViewImpl (IDS_HIERARCHY_RAW)
{

    //
    // Initialize variables
    //

    m_sRes .Set (pFile);
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CHierarchyRawView> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CHierarchyRawView::~CHierarchyRawView ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the window for this view
//
// @parm HWND | hWnd | Parent window
//
// @rdesc Pointer to the window
//
//-----------------------------------------------------------------------------

CWindow *CHierarchyRawView::GetWindow (HWND hWnd)
{
    if (m_hWnd == NULL && m_sRes != NULL)
        Write ();
    return CTextViewImpl::GetWindow (hWnd);
}

//-----------------------------------------------------------------------------
//
// @mfunc Write the contents of the hierarchy to the string
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CHierarchyRawView::Write ()
{
    CStringA str;

    //
    // Write the header
    //

    const CResHierarchyRef::Header *pHeader = m_sRes .GetHeader ();
    str .Format ("Entry = (%d,%d), Element = (%d, %d), VarName = (%d, %d)\r\n"
                 "VarData = (%d, %d), MultiMap = (%d, %d), List = (%d, %d)\r\n\r\n",
                 pHeader ->ulEntryOffset, pHeader ->ulEntryCount,
                 pHeader ->ulElementOffset, pHeader ->ulElementCount,
                 pHeader ->ulVarNameOffset, pHeader ->ulVarNameCount,
                 pHeader ->ulVarDataOffset, pHeader ->ulVarDataSize,
                 pHeader ->ulMultiMapOffset, pHeader ->ulMultiMapSize,
                 pHeader ->ulListOffset, pHeader ->ulListSize);
    m_strText += str;

    //
    // Print the entries
    //

    for (int i = 0; i < m_sRes .GetEntryCount (); i++)
    {
        const CResHierarchyRef::Entry *pEntry = m_sRes .GetEntry (i);
        str .Format ("Entry[%d] = (%d, %d, %d)\r\n",
                     i, pEntry ->ulCode, pEntry ->ulOffset, pEntry ->ulCount);
        m_strText += str;
    }
    m_strText += "\r\n";

    //
    // Dump the elements
    //

    for (int i = 0; i < m_sRes .GetElementCount (); i++)
        WriteElement (i);
    m_strText += "\r\n";

    //
    // Dump the variables
    //

    for (int i = 0; i < m_sRes .GetVarNameCount (); i++)
    {
        const CResHierarchyRef::VarName *pVarName = m_sRes .GetVarName (i);
        char sz [17];
        memmove (sz, pVarName ->szName, 16);
        sz [16] = 0;
        str .Format ("VarName[%d] = %s\r\n", i, sz);
        m_strText += str;
    }
    m_strText += "\r\n";

    //
    // Dump the variable data
    //

    unsigned char *pauchData = m_sRes .GetVarData ();
    unsigned char *pauchStart = pauchData;
    unsigned char *pauchEnd = pauchData + m_sRes .GetVarDataSize ();
    while (pauchStart < pauchEnd)
    {
        char szBuffer [80 + 3];

        //
        // Format the line of data
        //

        sprintf (szBuffer, "%08X ", pauchStart - pauchData);
        LPTSTR psz = &szBuffer [_tcslen (szBuffer)];
        for (int i = 0; i < 16; i++)
        {
            if (&pauchStart [i] < pauchEnd)
            {
                sprintf (psz, "%02X ", pauchStart [i]);
                psz += 3;
            }
            else
            {
                *psz++ = ' ';
                *psz++ = ' ';
                *psz++ = ' ';
            }
        }
        for (int i = 0; i < 16; i++)
        {
            if (&pauchStart [i] < pauchEnd && 
                pauchStart [i] >= ' ' && pauchStart [i] < 127)
                *psz++ = (char) pauchStart [i];
            else
                *psz++ = ' ';
        }
        *psz++ = '\r';
        *psz++ = '\n';
        *psz++ = 0;
        m_strText += szBuffer; 
        pauchStart += 16;
    }
    m_strText += "\r\n";

    //
    // Dump the multi map
    //

    for (int i = 0; i < m_sRes .GetMultiMapCount (); i++)
    {
        str .Format ("MultiMap[%d] = %d\r\n", i, m_sRes .GetMultiMap (i));
        m_strText += str;
    }
    m_strText += "\r\n";

    //
    // Dump the tree data
    //

    for (int i = 0; i < m_sRes .GetListCount (); i++)
    {
        str .Format ("List[%d] = %d\r\n", i, m_sRes .GetList (i));
        m_strText += str;
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Write an element to the string
//
// @parm int | nElement | Element number
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CHierarchyRawView::WriteElement (int nElement)
{

    //
    // Get a pointer to the element
    //

    const CResHierarchyRef::Element *pElement = 
        m_sRes .GetElement (nElement);

    //
    // Get the element name
    //

    char szText1 [17];
    if (pElement ->ulVarName >= m_sRes .GetVarNameCount ())
        sprintf (szText1, "%d", pElement ->ulVarName);
    else
    {
        const CResHierarchyRef::VarName *pVarName = 
            m_sRes .GetVarName (pElement ->ulVarName);
        strncpy (szText1, pVarName ->szName, _countof (szText1));
        szText1 [_countof (szText1) - 1] = 0;
    }

    //
    // Get the value
    //

    char szText2 [256];
    if (pElement ->ulType == CResHierarchyRef::ElementType_UINT8)
        sprintf (szText2, "%u", pElement ->u8Data);
    else if (pElement ->ulType == CResHierarchyRef::ElementType_UINT16)
        sprintf (szText2, "%u", pElement ->u16Data);
    else if (pElement ->ulType == CResHierarchyRef::ElementType_UINT32)
    {
        if (m_sRes .IsElement (pElement, "STRREF") ||
            m_sRes .IsElement (pElement, "Obj_StrRef") ||
            m_sRes .IsElement (pElement, "StrRef") ||
            m_sRes .IsElement (pElement, "Text") ||
            m_sRes .IsElement (pElement, "CatName"))
        {
            if (pElement ->u32Data == 0xFFFFFFFF)
            {
                szText2 [0] = 0;
            }
            else if (!g_sDialogTlkFile .IsOpen () || 
                     (g_sDialogTlkFile .GetString (pElement ->u32Data, szText2, _countof (szText2)) < 0 &&
                      g_sDialogTlkFile .GetStringName (pElement ->u32Data, szText2, _countof (szText2)) < 0))
            {
                _snprintf (szText2, _countof (szText2), "STRREF=%u", 
                           pElement ->u32Data);
            }
        }
        else
            sprintf (szText2, "%u", pElement ->u32Data);
    }
    else if (pElement ->ulType == CResHierarchyRef::ElementType_INT8)
        sprintf (szText2, "%d", pElement ->n8Data);
    else if (pElement ->ulType == CResHierarchyRef::ElementType_INT16)
        sprintf (szText2, "%d", pElement ->n16Data);
    else if (pElement ->ulType == CResHierarchyRef::ElementType_INT32)
        sprintf (szText2, "%d", pElement ->n32Data);
    else if (pElement ->ulType == CResHierarchyRef::ElementType_UINT64)
    {
        UINT64 *pu64 = (UINT64 *) m_sRes .GetVarData (pElement ->n32Data);
        sprintf (szText2, "%I64u", *pu64);
    }
    else if (pElement ->ulType == CResHierarchyRef::ElementType_INT64)
    {
        INT64 *p64 = (INT64 *) m_sRes .GetVarData (pElement ->n32Data);
        sprintf (szText2, "%I64d", *p64);
    }
    else if (pElement ->ulType == CResHierarchyRef::ElementType_FLOAT)
        sprintf (szText2, "%f", pElement ->fData);
    else if (pElement ->ulType == CResHierarchyRef::ElementType_DOUBLE)
    {
        double *pd = (double *) m_sRes .GetVarData (pElement ->n32Data);
        sprintf (szText2, "%f", *pd);
    }
    else if (pElement ->ulType == CResHierarchyRef::ElementType_STRING)
        m_sRes .GetElementLongString (pElement, szText2, _countof (szText2));
    else if (pElement ->ulType == CResHierarchyRef::ElementType_RESREF)
        m_sRes .GetElementShortString (pElement, szText2, _countof (szText2));
    else if (pElement ->ulType == CResHierarchyRef::ElementType_STRREF)
    {
        const CResHierarchyRef::StrRef *pStrRef = 
            m_sRes .GetElementStrRef (pElement);
        if (pStrRef ->ulStrRef == 0xFFFFFFFF)
        {
            if (pStrRef ->ulLangSpecCount > 0)
            {
                const CResHierarchyRef::StrRefLangSpec *pString =
                    (const CResHierarchyRef::StrRefLangSpec *) 
                    &pStrRef [1];
                int cchMaxText = _countof (szText2) - 1;
                if (cchMaxText > pString ->ulLength)
                    cchMaxText = pString ->ulLength;
                memmove (szText2, pString ->szText, cchMaxText);
                szText2 [cchMaxText] = 0;

            }
            else
                szText2 [0] = 0;
        }
        else if (!g_sDialogTlkFile .IsOpen () || 
                 (g_sDialogTlkFile .GetString (pStrRef ->ulStrRef, szText2, _countof (szText2)) < 0 &&
                  g_sDialogTlkFile .GetStringName (pStrRef ->ulStrRef, szText2, _countof (szText2)) < 0))
        {
            _snprintf (szText2, _countof (szText2), "STRREF=%d", pStrRef ->ulStrRef);
        }
    }
    else if (pElement ->ulType == CResHierarchyRef::ElementType_LIST)
    {
        _snprintf (szText2, _countof (szText2), "LIST=%d", 
                   pElement ->u32Data / sizeof (UINT32));
    }
    else if (pElement ->ulType == CResHierarchyRef::ElementType_CAPREF)
    {
        _snprintf (szText2, _countof (szText2), "CAPREF=%d", 
                   pElement ->u32Data);
    }
    else
    {
        _snprintf (szText2, _countof (szText2), "UNKNOWN=%d,%08X", 
                   pElement ->u32Data, pElement ->u32Data);
    }

    //
    // Format the string
    //

    CStringA str;
    str .Format ("Element[%d] = (%d, %s, %s)\r\n",
                 nElement, pElement ->ulType, szText1, szText2);
    m_strText += str;
}

/* vim: set sw=4: */
