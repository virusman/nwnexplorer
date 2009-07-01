//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      TableObject.cpp - Table object |
//
// This module contains the table object.
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
#include "TableObject.h"
#include "TableRefObject.h"
#include "TableDlg.h"
#include "MainWnd.h"
#include "DataParser.h"
#include "ProfileObject.h"

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
// @mfunc <c CTableObject> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CTableObject::CTableObject ()
{
    m_nTableCommand = Table_Command_Pick_One;
    m_fGlobal = false;
    m_lRefCount = 0;
    m_nTableType = Table_Type_Treasure;
    m_nNumber = -1;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CTableObject> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CTableObject::~CTableObject ()
{
    for (int i = 0; i < m_apItems .GetCount (); i++)
        delete m_apItems [i];
    m_apItems .RemoveAll ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the name of the object
//
// @parm CMainWnd * | pMainWnd | Main window
// 
// @parm LPTSTR | pszName | Name of the object
//
// @parm int | cchMaxText | Size of the buffer
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CTableObject::GetDescription (CMainWnd *pMainWnd,
                                   LPTSTR pszName, int cchMaxText)
{
    CString str (m_strName);

    //
    // Create a complete name
    //

    if (m_nTableType != Table_Type_Profile)
    {
        CString strCmd;
        strCmd .LoadString (IDS_TREASURE_COMMAND_00 + m_nTableCommand);
        str += CString (_T (" - ")) + strCmd;
    }

    //
    // Get the root name
    //

    _tcsncpy (pszName, str, cchMaxText);
    pszName [cchMaxText - 1] = 0;
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the object image
//
// @rdesc Image index.
//
//-----------------------------------------------------------------------------

Object_Image CTableObject::GetImage ()
{
    if (GetGlobal ())
        return Object_Image_Table_Global;
    else if (GetRefCount () == 0)
        return Object_Image_Table_NoRef;
    else
        return Object_Image_Table;
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

void CTableObject::WriteRoutineBody (CMainWnd *pMainWnd, FILE *fp, bool fDebug)
{
    if (fDebug)
        fprintf (fp, "  dbSpeak (\"%s\");\r\n", (LPCTSTR) m_strName);

    //
    // If this is a pick one
    //

    if (m_nTableCommand == Table_Command_Pick_One)
    {

        //
        // Compute the total
        //

        int nTotal = 0;
        for (int i = 0; i < m_apItems .GetCount (); i++)
        {
            nTotal += m_apItems [i] ->GetChance ();
        }

        //
        // If there is only one
        //

        if (nTotal == 1)
        {
            for (int i = 0; i < m_apItems .GetCount (); i++)
            {
                if (m_apItems [i] ->GetChance () > 0)
                {
                    m_apItems [i] ->WriteRoutineBody (pMainWnd, fp, fDebug);
                    fprintf (fp, "\r\n");
                }
                break;
            }
        }

        //
        // Otherwise, if there is more than one
        //

        else if (nTotal > 1)
        {
            if (nTotal / m_apItems .GetCount () <= 3)
            {
                fprintf (fp, "  switch (Random (%d))\r\n  {", nTotal);
                nTotal = 0;
                for (int i = 0; i < m_apItems .GetCount (); i++)
                {
                    int nChance = m_apItems [i] ->GetChance ();
                    if (nChance > 0)
                    {
                        for (int j = 0; j < nChance; j++, nTotal++)
                        {
                            if ((j % 4) == 0)
                                fprintf (fp, "\r\n  ");
                            fprintf (fp, "case %d: ", nTotal);
                        }
                        m_apItems [i] ->WriteRoutineBody (pMainWnd, fp, fDebug);
                        fprintf (fp, " break;");
                    }
                }
                fprintf (fp, "\r\n  }\r\n");
            }
            else
            {
                fprintf (fp, "  g_n = Random (%d);\r\n", nTotal);
                nTotal = 0;
                bool fFirst = true;
                for (int i = 0; i < m_apItems .GetCount (); i++)
                {
                    int nChance = m_apItems [i] ->GetChance ();
                    if (nChance > 0)
                    {
                        nTotal += nChance;
                        if (!fFirst)
                            fprintf (fp, "  else\r\n");
                        fFirst = false;
                        fprintf (fp, "  if (g_n < %d)\r\n  {", nTotal);
                        m_apItems [i] ->WriteRoutineBody (pMainWnd, fp, fDebug);
                        fprintf (fp, " }\r\n");
                    }
                }
            }
        }
    }

    //
    // If this is a pick multiple
    //

    else if (m_nTableCommand == Table_Command_Pick_Multiple)
    {
        for (int i = 0; i < m_apItems .GetCount (); i++)
        {
            int nChance = m_apItems [i] ->GetChance ();
            if (nChance >= 100)
            {
                m_apItems [i] ->WriteRoutineBody (pMainWnd, fp, fDebug);
                fprintf (fp, "\r\n");
            }
            else if (nChance > 0)
            {
                fprintf (fp, "  if (Random (100) < %d)\r\n  {", nChance);
                m_apItems [i] ->WriteRoutineBody (pMainWnd, fp, fDebug);
                fprintf (fp, "  }\r\n");
            }
        }
    }

    //
    // If this is a level range
    //

    else if (m_nTableCommand == Table_Command_Level_Range)
    {
        fprintf (fp, "  switch (GetLevelRange (GetHitDice (g_oA) + g_nM))\r\n  {\r\n");
        for (int i = 1; i <= 6; i++)
        {
            for (int j = 0; j < m_apItems .GetCount (); j++)
            {
                int nChance = m_apItems [j] ->GetChance ();
                if (nChance == i)
                {
                    fprintf (fp, "  case %d: ", i);
                    m_apItems [j] ->WriteRoutineBody (pMainWnd, fp, fDebug);
                    fprintf (fp, " break;\r\n");
                    break;
                }
            }
        }
        fprintf (fp, "  }\r\n");
    }

    //
    // If this is a specific
    //

    else if (m_nTableCommand == Table_Command_Specific)
    {
        fprintf (fp, "  if (g_nS == 0)\r\n  {\r\n");
        for (int j = 0; j < m_apItems .GetCount (); j++)
        {
            int nChance = m_apItems [j] ->GetChance ();
            if (nChance == 0)
            {
                m_apItems [j] ->WriteRoutineBody (pMainWnd, fp, fDebug);
                break;
            }
        }
        fprintf (fp, "  }\r\n  else\r\n  {\r\n");
        for (int j = 0; j < m_apItems .GetCount (); j++)
        {
            int nChance = m_apItems [j] ->GetChance ();
            if (nChance == 1)
            {
                m_apItems [j] ->WriteRoutineBody (pMainWnd, fp, fDebug);
                break;
            }
        }
        fprintf (fp, "  }\r\n");
    }

    //
    // If this is racial type
    //

    else if (m_nTableCommand == Table_Command_Racial_Type)
    {
        //
        // Initialize the statements
        //

        fprintf (fp, "  g_n = -1;\r\n");
        fprintf (fp, "  if (GetObjectType (g_oA) == OBJECT_TYPE_CREATURE)\r\n");
        fprintf (fp, "      g_n = GetRacialType (g_oA);\r\n");

        //
        // Write the rest
        //

        WriteRoutineBodyList (pMainWnd, fp, 
                              IDS_RACIAL_TYPE_00, Racial_Type__Last,
                              "g_n == %s", "TRUE", fDebug);
    }

    //
    // If this is random class type
    //

    else if (m_nTableCommand == Table_Command_Random_Class_Type)
    {

        //
        // Initialize the statements
        //

        fprintf (fp, "  int g_n = MyRandomClassToUse (g_oA);\r\n");

        //
        // Write the rest
        //

        WriteRoutineBodyList (pMainWnd, fp, 
                              IDS_CLASS_TYPE_00, Class_Type__Last,
                              "g_n == %s", "TRUE", fDebug);
    }

    //
    // If this is class type
    //

    else if (m_nTableCommand == Table_Command_Class_Type)
    {

        //
        // Write the rest
        //

        WriteRoutineBodyList (pMainWnd, fp, 
                              IDS_CLASS_TYPE_00, Class_Type__Last,
                              "GetLevelByClass (%s, g_oA) > 0", "TRUE", fDebug);
    }

    //
    // If this is weapon feat
    //

    else if (m_nTableCommand == Table_Command_Weapon_Focus)
    {

        //
        // Write the rest
        //

        WriteRoutineBodyList (pMainWnd, fp, 
                              IDS_WEAPON_FOCUS_00, Weapon_Focus__Last,
                              "GetHasFeat (%s, g_oA)", "TRUE", fDebug);
    }

    //
    // If this is armor proficiency
    //

    else if (m_nTableCommand == Table_Command_Armor_Proficiency)
    {

        //
        // Write the rest
        //

        WriteRoutineBodyList (pMainWnd, fp, 
                              IDS_ARMOR_PROFICIENCY_00, Armor_Proficiency__Last,
                              "GetHasFeat (%s, g_oA)", "TRUE", fDebug);
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Write the routine body for a block that computes a value
//              and then matches it against a list of constants
//
// @parm CMainWnd * | pMainWnd | Main window
// 
// @parm FILE * | fp | Output file
//
// @parm int | nID | Start of the routine IDS
//
// @parm int | nLast | Last valid index
//
// @parm LPCSTR | pszTestFormat | Format string used to generate a teset
//
// @parm LPCSTR | pszAnyFormat | Format string used to ANY match
//
// @parm bool | fDebug | If true, write debug statements
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CTableObject::WriteRoutineBodyList (CMainWnd *pMainWnd, FILE *fp,
                                         int nID, int nLast, LPCSTR pszTestFormat, LPCSTR pszAnyFormat, bool fDebug)
{
    bool fFirst = true;

    //
    // Loop until all done
    //

    int i = 0;
    while (i < m_apItems .GetCount ())
    {

        //
        // Look for the first valid entry
        //

        while (i < m_apItems .GetCount ())
        {
            int n = m_apItems [i] ->GetChance ();
            if (n >= 0 && n <= nLast)
                break;
            i++;
        }
        if (i >= m_apItems .GetCount ())
            break;

        //
        // Loop while we have like messages
        //

        int j;
        for (j = i; j < m_apItems .GetCount (); j++)
        {

            //
            // If invalid, skip
            //

            int n = m_apItems [j] ->GetChance ();
            if (n < 0 || n > nLast)
                continue;

            //
            // If doesn't match, break
            //

            if (i != j && !m_apItems [i] ->Compare (m_apItems [j]))
                break;

            //
            // Start the if statement 
            //

            if (fFirst)
                fprintf (fp, "  if (");
            else if (i == j)
                fprintf (fp, "  else\r\n  if (");
            else
                fprintf (fp, " ||\r\n      ");
            fFirst = false;

            //
            // Generate the statement
            //

            if (n == 0)
            {
                fprintf (fp, pszAnyFormat);
            }
            else
            {
                char szText [64];
                ::LoadStringA (_Module .GetResourceInstance (), 
                               nID + n, szText, _countof (szText));
                char *p = strchr (szText, '\n') + 1;
                fprintf (fp, pszTestFormat, p);
            }
        }

        //
        // End the if statement
        //

        fprintf (fp, ")\r\n  {\r\n");
        m_apItems [i] ->WriteRoutineBody (pMainWnd, fp, fDebug);
        fprintf (fp, "  }\r\n");
        i = j;
    }
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

bool CTableObject::TableEdit (CMainWnd *pMainWnd, HTREEITEM hItem)
{
    CTableDlg dlg (this, pMainWnd, hItem);
    return dlg .DoModal (pMainWnd ->m_hWnd) == IDOK;
}

//-----------------------------------------------------------------------------
//
// @mfunc Add a new item
//
// @parm CMainWnd * | pMainWnd | Main window
//
// @parm CTreeViewCtrl & | tv | Tree view
//
// @parm CTableItemObject * | *pNewItem | Object added
//
// @parm HTREEITEM | hInsertAfter | Item to insert after
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

int CTableObject::AddNewItem (CMainWnd *pMainWnd, CTreeViewCtrl &tv, 
                              CTableItemObject *pNewItem, HTREEITEM hInsertAfter)
{

    //
    // If we are to insert at the end
    //

    if (hInsertAfter == TVI_LAST)
    {
        int nIndex = (int) m_apItems .Add (pNewItem);
        for (int i = 0; i < m_ahtvItem .GetCount (); i++)
        {
            HTREEITEM hParent = m_ahtvItem [i];
            if ((tv .GetItemState (hParent, 
                                   TVIS_EXPANDEDONCE) & TVIS_EXPANDEDONCE) == 0)
                continue;
            pNewItem ->AddToTV (tv, hParent, hInsertAfter);
        }
        return nIndex;
    }

    //
    // If we are to insert in the middle
    //

    else
    {

        //
        // Locate the insert position
        //

        int nIndex = 1;
        HTREEITEM hSib = tv .GetPrevSiblingItem (hInsertAfter);
        while (hSib != NULL)
        {
            nIndex++;
            hSib = tv .GetPrevSiblingItem (hSib);
        }

        //
        // Insert into the array
        //

        m_apItems .InsertAt (nIndex, pNewItem);

        //
        // Insert into the TV
        //

        for (int i = 0; i < m_ahtvItem .GetCount (); i++)
        {
            HTREEITEM hParent = m_ahtvItem [i];
            if ((tv .GetItemState (hParent, 
                                   TVIS_EXPANDEDONCE) & TVIS_EXPANDEDONCE) == 0)
                continue;
            pNewItem ->AddToTV (tv, hParent, TVI_LAST);
            SortTV (pMainWnd, tv, hParent);
        }
        return nIndex;
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Add a new item to table refs
//
// @parm CMainWnd * | pMainWnd | Main window
//
// @parm CTreeViewCtrl & | tv | Tree view
//
// @parm CTableItemObject * | pObject | Object added
//
// @parm LPCTSTR | pszTableName | Name of the table
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CTableObject::AddNewItemToTableRefs (CMainWnd *pMainWnd, 
                                          CTreeViewCtrl &tv, CTableItemObject *pObject, LPCTSTR pszTableName)
{
    for (int i = 0; i < m_apItems .GetCount (); i++)
    {
        CTableItemObject *pTIObject = m_apItems [i];
        if (pTIObject ->GetObjectType () == Object_Type_Table_Ref)
        {
            CTableRefObject *pTableRef = static_cast
                <CTableRefObject *> (pTIObject);
            if (_tcscmp (pTableRef ->GetName (), pszTableName) == 0)
                pTableRef ->AddNewItem (pMainWnd, tv, pObject);
        }
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Sort table refs
//
// @parm CMainWnd * | pMainWnd | Main window
//
// @parm CTreeViewCtrl & | tv | Tree view
//
// @parm LPCTSTR | pszTableName | Name of the table
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CTableObject::SortTableRefs (CMainWnd *pMainWnd, 
                                  CTreeViewCtrl &tv, LPCTSTR pszTableName)
{
    for (int i = 0; i < m_apItems .GetCount (); i++)
    {
        CTableItemObject *pTIObject = m_apItems [i];
        if (pTIObject ->GetObjectType () == Object_Type_Table_Ref)
        {
            CTableRefObject *pTableRef = static_cast
                <CTableRefObject *> (pTIObject);
            if (_tcscmp (pTableRef ->GetName (), pszTableName) == 0)
                pTableRef ->SortTV (pMainWnd, tv, NULL);
        }
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Move a child
//
// @parm CMainWnd * | pMainWnd | Main window
//
// @parm CTreeViewCtrl & | tv | Tree view
//
// @parm CTableItemObject * | pChild | Object to move
//
// @parm int | nDirection | Direction of motion
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CTableObject::MoveChild (CMainWnd *pMainWnd, CTreeViewCtrl &tv, 
                              CTableItemObject *pChild, int nDirection)
{
    int nIndex;
    for (nIndex = 0; nIndex < m_apItems .GetCount (); nIndex++)
        if (m_apItems [nIndex] == pChild)
            break;
    if (nIndex < m_apItems .GetCount ())
    {
        if (nDirection < 0)
        {
            if (nIndex > 0)
            {
                m_apItems .RemoveAt (nIndex);
                m_apItems .InsertAt (nIndex - 1, pChild);
            }
        }
        else if (nDirection > 0)
        {
            if (nIndex < m_apItems .GetCount () - 1)
            {
                m_apItems .RemoveAt (nIndex);
                m_apItems .InsertAt (nIndex + 1, pChild);
            }
        }
        else
            _ASSERTE (FALSE);
        DoSort (tv, this, NULL);
        for (int i = 0; i < pMainWnd ->GetTableCount (); i++)
        {
            CTableObject *pTable = pMainWnd ->GetTable (i);
            pTable ->SortTableRefs (pMainWnd, tv, GetName ());
        }
    }
    return;
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

bool CTableObject::Delete (CMainWnd *pMainWnd, 
                           CTreeViewCtrl &tv, HTREEITEM hItem, bool fPrompt)
{
    if (GetRefCount () != 0)
    {
        CString str;
        str .Format (IDS_ERR_REFED);
        ::MessageBox (pMainWnd ->m_hWnd, str, g_szAppName, MB_OK);
        return false;
    }
    while (m_apItems .GetCount () > 0)
    {
        CTableItemObject *pTIObject = m_apItems [0];
        pTIObject ->Delete (pMainWnd, tv, hItem, false);
    }
    pMainWnd ->RemoveTable (this);
    DeleteFromTV (pMainWnd, tv, NULL);
    delete this;
    return true;
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the default chance based on the table command
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

int CTableObject::GetDefaultChance ()
{
    switch (m_nTableCommand)
    {
        case Table_Command_Pick_One:
        case Table_Command_Pick_Multiple:
            return 1;

        case Table_Command_Level_Range:
            return 1;

        case Table_Command_Specific:
            return 0;

        case Table_Command_Racial_Type:
            return Racial_Type_Any;

        case Table_Command_Weapon_Focus:
            return Weapon_Focus_Any;

        case Table_Command_Armor_Proficiency:
            return Armor_Proficiency_Any;

        case Table_Command_Random_Class_Type:
        case Table_Command_Class_Type:
            return Class_Type_Any;

        default:
            _ASSERTE (FALSE);
            return 1;
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Count the number of table refs
//
// @parm CMainWnd * | pMainWnd | Main window
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CTableObject::CountTableRefs (CMainWnd *pMainWnd)
{
    m_nTableRefCount = 0;
    for (int i = 0; i < m_apItems .GetCount (); i++)
    {
        if (m_apItems [i] ->GetObjectType () == Object_Type_Table_Ref)
        {
            CTableRefObject *pTableRef = static_cast 
                <CTableRefObject *> (m_apItems [i]);
            if (pTableRef ->GetTableRef (pMainWnd))
                m_nTableRefCount++;
        }
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Remove table ref counts
//
// @parm LPCTSTR | pszTableName | Name of the table
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CTableObject::RemoveTableRefCount (LPCTSTR pszTableName)
{
    for (int i = 0; i < m_apItems .GetCount (); i++)
    {
        if (m_apItems [i] ->GetObjectType () == Object_Type_Table_Ref)
        {
            CTableRefObject *pTableRef = static_cast 
                <CTableRefObject *> (m_apItems [i]);
            if (_tcscmp (pTableRef ->GetName (), pszTableName) == 0)
                m_nTableRefCount--;
        }
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Remove table recusions
//
// @parm LPCTSTR | pszTableName | Name of the table
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CTableObject::RemoveTableRecusions (CMainWnd *pMainWnd)
{
    if (m_nTableRefCount == 0)
        return;
    m_nTableRefCount = 0;
    for (int i = 0; i < pMainWnd ->GetTableCount (); i++)
    {
        CTableObject *pTable = pMainWnd ->GetTable (i);
        if (pTable ->m_nTableRefCount != 0)
        {
            for (int i = 0; i < pTable ->m_apItems .GetCount (); i++)
            {
                if (pTable ->m_apItems [i] ->GetObjectType () == 
                    Object_Type_Table_Ref)
                {
                    CTableRefObject *pTableRef = static_cast 
                        <CTableRefObject *> (pTable ->m_apItems [i]);
                    CTableObject *pTable2 = pTableRef ->GetTableRef (pMainWnd);
                    if (pTable2 == this)
                        pTable ->RemoveTableRecusions (pMainWnd);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Write profile name to number body
//
// @parm FILE * | fp | Output file
//
// @rdesc TRUE if a line was output.
//
//-----------------------------------------------------------------------------

bool CTableObject::WriteProfileTest (FILE *fp)
{
    bool fFirst = true;
    for (int i = 0; i < m_apItems .GetCount (); i++)
    {
        if (m_apItems [i] ->GetObjectType () != Object_Type_Profile)
            continue;
        CProfileObject *pObject = static_cast <CProfileObject *> (m_apItems [i]);
        if (pObject ->WriteProfileTest (fp, fFirst))
            fFirst = false;
    }
    return !fFirst;
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

void CTableObject::OnStartElement (CDataParser *pParser, 
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
    // If this is start of gold
    //

    else if (nElement == Gold)
    {
        CTableItemObject *pItem = new CGoldObject;
        pItem ->SetTable (this);
        pParser ->SetItem (pItem);
        pParser ->SetState (CDataParser::InGold);
    }

    //
    // If this is start of item
    //

    else if (nElement == Item)
    {
        CTableItemObject *pItem = new CItemObject;
        pItem ->SetTable (this);
        pParser ->SetItem (pItem);
        pParser ->SetState (CDataParser::InItem);
    }

    //
    // If this is start of none
    //

    else if (nElement == None)
    {
        CTableItemObject *pItem = new CNoneObject;
        pItem ->SetTable (this);
        pParser ->SetItem (pItem);
        pParser ->SetState (CDataParser::InNone);
    }

    //
    // If this is start of tableref
    //

    else if (nElement == TableRef)
    {
        CTableItemObject *pItem = new CTableRefObject;
        pItem ->SetTable (this);
        pParser ->SetItem (pItem);
        pParser ->SetState (CDataParser::InTableRef);
    }

    //
    // If this is start of encounter
    //

    else if (nElement == Encounter)
    {
        CTableItemObject *pItem = new CEncounterObject;
        pItem ->SetTable (this);
        pParser ->SetItem (pItem);
        pParser ->SetState (CDataParser::InEncounter);
    }

    //
    // If this is start of placeable
    //

    else if (nElement == Placeable)
    {
        CTableItemObject *pItem = new CPlaceableObject;
        pItem ->SetTable (this);
        pParser ->SetItem (pItem);
        pParser ->SetState (CDataParser::InPlaceable);
    }

    //
    // If this is start of profile
    //

    else if (nElement == Profile)
    {
        CTableItemObject *pItem = new CProfileObject;
        pItem ->SetTable (this);
        pParser ->SetItem (pItem);
        pParser ->SetState (CDataParser::InProfile);
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

void CTableObject::OnEndElement (CDataParser *pParser, const XML_Char *pszName)
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

        case Table:
            pParser ->SetState (CDataParser::InData);
            pParser ->SaveTable ();
            break;

            //
            // If this is the name
            //

        case Name:
            m_strName = pParser ->GetText (false);
            break;

            //
            // If this is the command
            //

        case Command:
            m_nTableCommand = (Table_Command) atol (pParser ->GetText (false));
            break;

            //
            // If this is the global
            //

        case Global:
            m_fGlobal = atol (pParser ->GetText (false)) != 0;
            break;

            //
            // If this is the number
            //

        case Number:
            m_nNumber = atol (pParser ->GetText (false));
            break;

            //
            // If this is the table type
            //

        case TableType:
            m_nTableType = (Table_Type) atol (pParser ->GetText (false));
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

CTableObject::Element CTableObject::GetElement (const XML_Char *pszName)
{
    if (stricmp (pszName, "Table") == 0)
        return Table;
    else if (stricmp (pszName, "Name") == 0)
        return Name;
    else if (stricmp (pszName, "Command") == 0)
        return Command;
    else if (stricmp (pszName, "Global") == 0)
        return Global;
    else if (stricmp (pszName, "TableRef") == 0)
        return TableRef;
    else if (stricmp (pszName, "Gold") == 0)
        return Gold;
    else if (stricmp (pszName, "Item") == 0)
        return Item;
    else if (stricmp (pszName, "None") == 0)
        return None;
    else if (stricmp (pszName, "Number") == 0)
        return Number;
    else if (stricmp (pszName, "TableType") == 0)
        return TableType;
    else if (stricmp (pszName, "Encounter") == 0)
        return Encounter;
    else if (stricmp (pszName, "Placeable") == 0)
        return Placeable;
    else if (stricmp (pszName, "Profile") == 0)
        return Profile;
    else
        return Unknown;
}


/* vim: set sw=4: */
