//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ITPView.cpp - ITP window |
//
// This module contains the definition of the ITP window.
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
// $History: ITPView.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "ITPView.h"
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
// @mfunc <c CITPView> constructor.
//
// @parm CData * | pFile | Hierarchy file
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CITPView::CITPView (CData *pFile) : 
    CTextViewImpl (IDS_ITP)
{

    //
    // Initialize variables
    //

    m_sRes .Set (pFile);
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CITPView> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CITPView::~CITPView ()
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

CWindow *CITPView::GetWindow (HWND hWnd)
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

void CITPView::Write ()
{
    CStringA str;

    CResHierarchyRef::Entry *pEntry = m_sRes .GetEntry (0);
    UINT32 *plMap = m_sRes .GetMultiMapTable (pEntry);
    for (int j = 0; j < pEntry ->ulCount; j++)
    {
        int nElement = plMap [j];
        const CResHierarchyRef::Element *pElement = 
            m_sRes .GetElement (nElement);
        if (pElement ->ulType == CNwnHierarchy::ElementType_LIST)
        {
            UINT32 *plList = m_sRes .GetListTable (pElement);
            long lCount = *plList++;
            for (j = 0; j < lCount; j++)
                WriteTree (plList [j], 0);
        }
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Write an entry to the string
//
// @parm int | nEntry | Entry number
//
// @parm int | nIndent | Indent amount
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CITPView::WriteTree (int nEntry, int nIndent)
{
    char szName [64];
    char szBlueprint [64];
    char szIndent [65];
    int nList = -1;

    //
    // Generate the indent string
    //

    if (nIndent > _countof (szIndent) - 1)
        nIndent = _countof (szIndent) - 1;
    memset (szIndent, ' ', nIndent);
    szIndent [nIndent] = 0;

    //
    // Init the string
    //

    szName [0] = szBlueprint [0] = 0;

    //
    // Get the entry in question
    //

    CResHierarchyRef::Entry *pEntry = m_sRes .GetEntry (nEntry);

    //
    // Collect information about what we need to add
    //

    UINT32 *plMap = m_sRes .GetMultiMapTable (pEntry);
    for (int j = 0; j < pEntry ->ulCount; j++)
    {
        int nElement = plMap [j];
        const CResHierarchyRef::Element *pElement = 
            m_sRes .GetElement (nElement);

        //
        // If this is a list, get the element
        //

        if (pElement ->ulType == CNwnHierarchy::ElementType_LIST)
        {
            nList = nElement;
        }

        //
        // Otherwise, inspect the variable name to get 
        // the other information
        //

        else 
        {

            //
            // Get the name
            //

            CResHierarchyRef::VarName *pVarName = 
                m_sRes .GetVarName (pElement ->ulVarName);

            //
            // If this is the strref, get the name
            //

            if (stricmp (pVarName ->szName, "STRREF") == 0)
            {
                if (pElement ->u32Data >= 16777216)
                    snprintf(szName, _countof (szName), "CUSTOM #%u",
                             (pElement ->u32Data & 0xFFFFFF));
                else
                    g_sDialogTlkFile .GetString (pElement ->u32Data, 
                                                 szName, _countof (szName));
            }

            //
            // If this is a name, get the name
            //

            else if (stricmp (pVarName ->szName, "NAME") == 0)
            {
                m_sRes .GetElementLongString (pElement, 
                                              szName, _countof (szName));
            }

            //
            // If this is a resref, get the blueprint
            //

            else if (stricmp (pVarName ->szName, "RESREF") == 0)
            {
                m_sRes .GetElementShortString (pElement, 
                                               szBlueprint, _countof (szBlueprint));
            }
        }
    }

    //
    // Generate the name
    //

    CString str (szIndent);
    str += szName;
    if (szBlueprint [0] != 0)
    {
        str += _T (" [");
        str += szBlueprint;
        str += _T ("]");
    }
    m_strText += str + "\r\n";

    //
    // If we have a list, add this list
    //

    if (nList != -1)
    {
        const CResHierarchyRef::Element *pElement = 
            m_sRes .GetElement (nList);
        UINT32 *plList = m_sRes .GetListTable (pElement);
        long lCount = *plList++;
        for (long j = 0; j < lCount; j++)
            WriteTree (plList [j], nIndent + 2);
    }
}

/* vim: set sw=4: */
