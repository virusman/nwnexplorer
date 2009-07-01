//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      TableRefObject.cpp - TableRef object |
//
// This module contains the table reference object.
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
#include "TableRefObject.h"
#include "MainWnd.h"
#include "DataParser.h"

//
// Debug NEW
//

#if defined (_DEBUG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// Global variables
//

extern TCHAR g_szAppName [];

//-----------------------------------------------------------------------------
//
// @mfunc <c CTableRefObject> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CTableRefObject::CTableRefObject ()
{
    m_nCount = 1;
    m_nRange = 0;
    m_nModifier = -1;
    m_nSpecific = -1;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CTableRefObject> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CTableRefObject::~CTableRefObject ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the object image
//
// @rdesc Image index.
//
//-----------------------------------------------------------------------------

Object_Image CTableRefObject::GetImage ()
{
    return Object_Image_Table;
}

//-----------------------------------------------------------------------------
//
// @mfunc See if the object has children
//
// @parm CMainWnd * | pMainWnd | Main window
//
// @rdesc true if the object has children
//
//-----------------------------------------------------------------------------

bool CTableRefObject::HasChildren (CMainWnd *pMainWnd)
{
    CTableObject *pTable = GetTableRef (pMainWnd); 
    if (pTable)
        return pTable ->HasChildren (pMainWnd);
    else
        return false;
}

//-----------------------------------------------------------------------------
//
// @mfunc Add children
//
// @parm CMainWnd * | pMainWnd | Main window
//
// @parm CTreeViewCtrl & | tv | Tree view
//
// @parm HTREEITEM | hParent | Parent item
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CTableRefObject::AddChildren (CMainWnd *pMainWnd, 
                                   CTreeViewCtrl &tv, HTREEITEM hParent)
{
    CTableObject *pTable = GetTableRef (pMainWnd);
    if (pTable)
        pTable ->AddChildren (pMainWnd, tv, hParent);
}

//-----------------------------------------------------------------------------
//
// @mfunc Generate ref counts
//
// @parm CMainWnd * | pMainWnd | Main window
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CTableRefObject::ComputeRefCounts (CMainWnd *pMainWnd)
{
    CTableObject *pTable = GetTableRef (pMainWnd);
    if (pTable)
        pTable ->AddRef ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the full text of object
//
// @parm CMainWnd * | pMainWnd | Main window
//
// @rdesc Full text of the object
//
//-----------------------------------------------------------------------------

CString CTableRefObject::GetFullName (CMainWnd *pMainWnd)
{
    CTableObject *pTable = GetTableRef (pMainWnd);
    if (pTable)
    {
        TCHAR szText [512];
        pTable ->GetDescription (pMainWnd, szText, _countof (szText));
        CString str (szText);
        if (m_nCount > 1 || m_nRange >= 2)
        {
            CString str2;
            if (m_nRange >= 2)
                str2 .Format (IDS_COUNT_RANGE, m_nCount, m_nCount + m_nRange - 1);
            else
                str2 .Format (IDS_COUNT, m_nCount);
            str += ", " + str2;
        }
        if (m_nModifier != -1)
        {
            CString str2;
            str2 .Format (IDS_MODIFIER, m_nModifier);
            str += ", " + str2;
        }
        if (m_nSpecific != -1)
        {
            CString str2;
            str2 .LoadString (m_nSpecific != 0 ? IDS_SPECIFIC : IDS_GENERIC);
            str += ", " + str2;
        }
        return str;
    }
    else
        return m_strName;
}

//-----------------------------------------------------------------------------
//
// @mfunc Edit the object
//
// @parm CMainWnd * | pMainWnd | Main window
//
// @parm HTREEITEM | hItem | Exact tree view item being edited
//
// @rdesc Edit status
//
//-----------------------------------------------------------------------------

bool CTableRefObject::TableEdit (CMainWnd *pMainWnd, HTREEITEM hItem)
{
    CTableObject *pTable = GetTableRef (pMainWnd);
    if (pTable)
    {
        return pTable ->TableEdit (pMainWnd, hItem);
    }
    CString str;
    str .Format (IDS_ERR_NO_TABLE);
    ::MessageBox (pMainWnd ->m_hWnd, str, g_szAppName, MB_OK);
    return false;
}

//-----------------------------------------------------------------------------
//
// @mfunc Write the routine body
//
// @parm CMainWnd * | pMainWnd | Main window
// 
// @parm FILE * | fp | Output file
//
// @parm bool | fDebug | If true, write debug statements
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CTableRefObject::WriteRoutineBody (CMainWnd *pMainWnd, FILE *fp, bool fDebug)
{
    CTableObject *pTable = GetTableRef (pMainWnd);
    if (pTable)
    {
        if (m_nRange >= 2)
        {
            fprintf (fp, " { int n = %d + Random (%d); int i;", m_nCount, m_nRange);
            fprintf (fp, " for (i = 0; i < n; i++)");
            WriteRoutineBodyCall (pMainWnd, fp, pTable, fDebug);
            fprintf (fp, " }");
        }
        else if (m_nCount > 1)
        {
            fprintf (fp, " { int i;");
            fprintf (fp, " for (i = 0; i < %d; i++)", m_nCount);
            WriteRoutineBodyCall (pMainWnd, fp, pTable, fDebug);
            fprintf (fp, " }");
        }
        else
        {
            WriteRoutineBodyCall (pMainWnd, fp, pTable, fDebug);
        }
    }
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Write the routine body call
//
// @parm CMainWnd * | pMainWnd | Main window
// 
// @parm FILE * | fp | Output file
//
// @parm CTableObject * | pTable | Table
//
// @parm bool | fDebug | If true, write debug statements
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CTableRefObject::WriteRoutineBodyCall (CMainWnd *pMainWnd, 
                                            FILE *fp, CTableObject *pTable, bool fDebug)
{
    if (m_nModifier >= 0 || m_nSpecific >= 0 || pTable ->GetRefCount () <= 1)
        fprintf (fp, " {");
    if (m_nModifier >= 0)
        fprintf (fp, " int nMS = g_nM; g_nM += %d;",    m_nModifier);
    if (m_nSpecific >= 0)
        fprintf (fp, " int nSS = g_nS; g_nS = %d;", m_nSpecific);

    if (pTable ->GetRefCount () > 1)
        fprintf (fp, " CI_%s();", (LPCTSTR) m_strName);
    else
    {
        fprintf (fp, "\r\n");
        pTable ->WriteRoutineBody (pMainWnd, fp, fDebug);
        fprintf (fp, "\r\n");
    }

    if (m_nModifier >= 0)
        fprintf (fp, " g_nM = nMS;");
    if (m_nSpecific >= 0)
        fprintf (fp, " g_nS = nSS;");
    if (m_nModifier >= 0 || m_nSpecific >= 0 || pTable ->GetRefCount () <= 1)
        fprintf (fp, " }");
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Delete children from the TV
//
// @parm CMainWnd * | pMainWnd | Main window
//
// @parm CTreeViewCtrl & | tv | Tree view
//
// @parm HTREEITEM | hItem | Child being deleted
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

bool CTableRefObject::DeleteChildFromTV (CMainWnd *pMainWnd, 
                                         CTreeViewCtrl &tv, HTREEITEM hItem)
{
    CTableObject *pTable = GetTableRef (pMainWnd);
    if (pTable)
        return pTable ->DeleteChildFromTV (pMainWnd, tv, hItem);
    else
        return false;
}

//-----------------------------------------------------------------------------
//
// @mfunc Delete the table ref
//
// @parm CMainWnd * | pMainWnd | Main window
//
// @parm CTreeViewCtrl & | tv | Tree view
//
// @parm HTREEITEM | hItem | Child being deleted
//
// @parm bool | fPrompt | If true, prompt for delete.  Otherwise, take defaults
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

bool CTableRefObject::Delete (CMainWnd *pMainWnd, 
                              CTreeViewCtrl &tv, HTREEITEM hItem, bool fPrompt)
{
    if (pMainWnd ->HandleTableRefDelete (this, hItem, fPrompt))
    {
        CTableItemObject::Delete (pMainWnd, tv, hItem, fPrompt);
        return true;
    }
    else
        return false;
}

//-----------------------------------------------------------------------------
//
// @mfunc Return the referenced table
//
// @parm CMainWnd * | pMainWnd | Main window
//
// @rdesc Address of the referenced table or NULL if not found.
//
//-----------------------------------------------------------------------------

CTableObject *CTableRefObject::GetTableRef (CMainWnd *pMainWnd)
{
    return      pMainWnd ->FindTable (m_strName, 
                                      m_pTable ->GetTableType ());
}

//-----------------------------------------------------------------------------
//
// @mfunc Begin the processing of an XML element
//
// @parm CDataParser * | pParser | Current parser
//
// @parm const XML_Char * | pszName | Name of the element
//
// @parm const XML_Char ** | papszAttrs | Attributes
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CTableRefObject::OnStartElement (CDataParser *pParser, 
                                      const XML_Char *pszName, const XML_Char **papszAttrs)
{
    //
    // Get the element
    //

    Element nElement = GetElement (pszName);
    if (nElement == Unknown)
    {
        pParser ->SetError (IDS_ERR_XML_UNEXPECTED_ELEMENT);
        return;
    }

    //
    // Otherwise
    //

    else
    {

        //
        // No attributes are allowed
        //

        if (papszAttrs [0] != NULL)
        {
            pParser ->SetError (IDS_ERR_XML_NO_ATTRS);
            return;
        }
    }
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc End an XML element
//
// @parm CDataParser * | pParser | Current parser
//
// @parm const XML_Char * | pszName | Name of the element
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CTableRefObject::OnEndElement (CDataParser *pParser, const XML_Char *pszName)
{

    //
    // Get the element
    //

    Element nElement = GetElement (pszName);
    if (nElement == Unknown)
    {
        pParser ->SetError (IDS_ERR_XML_UNEXPECTED_ELEMENT);
        return;
    }

    //
    // Switch based on the element
    //

    switch (nElement)
    {

        //
        // If we are ending the Table
        //

        case TableRef:
            pParser ->SetState (CDataParser::InTable);
            pParser ->SaveItem ();
            break;

            //
            // If this is the chance
            //

        case Chance:
            m_nChance = atol (pParser ->GetText (false));
            break;

            //
            // If this is the name
            //

        case Name:
            m_strName = pParser ->GetText (false);
            break;

            //
            // If this is the count
            //

        case Count:
            m_nCount = atol (pParser ->GetText (false));
            break;

            //
            // If this is the range
            //

        case Range:
            m_nRange = atol (pParser ->GetText (false));
            break;

            //
            // If this is the modifier
            //

        case Modifier:
            m_nModifier = atol (pParser ->GetText (false));
            break;

            //
            // If this is the specific
            //

        case Specific:
            m_nSpecific = atol (pParser ->GetText (false));
            break;
    }
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the element number
//
// @parm const XML_Char * | pszName | Element name
//
// @rdesc Element id.
//
//-----------------------------------------------------------------------------

CTableRefObject::Element CTableRefObject::GetElement (const XML_Char *pszName)
{
    if (stricmp (pszName, "TableRef") == 0)
        return TableRef;
    else if (stricmp (pszName, "Chance") == 0)
        return Chance;
    else if (stricmp (pszName, "Name") == 0)
        return Name;
    else if (stricmp (pszName, "Count") == 0)
        return Count;
    else if (stricmp (pszName, "Range") == 0)
        return Range;
    else if (stricmp (pszName, "Modifier") == 0)
        return Modifier;
    else if (stricmp (pszName, "Specific") == 0)
        return Specific;
    else
        return Unknown;
}

/* vim: set sw=4: */
