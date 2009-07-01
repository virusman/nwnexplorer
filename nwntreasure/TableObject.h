#ifndef ETS_TABLEOBJECT_H
#define ETS_TABLEOBJECT_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      TableObject.h - Table object |
//
// This module contains the definition of the table object.
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
// $History: CnfMainWnd.h $
//      
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include "Object.h"
#include "TableItemObject.h"

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CTableObject : public CObject
{
    public:
        enum Element
        {
            Unknown                     = 0,
            Table                       = 1,
            Name                        = 2,
            Command                     = 3,
            Global                      = 4,
            TableRef            = 5,
            Gold                        = 6,
            Item                        = 7,
            None                        = 8,
            Number                      = 9,
            TableType           = 10,
            Encounter           = 11,
            Placeable           = 12,
            Profile                     = 13,
        };

        // @access Constructors and destructors
    public:

        // @cmember General constructor

        CTableObject ();

        // @cmember General destructor

        virtual ~CTableObject ();

        // @access Public methods
    public:

        // @cmember Add a new item

        int AddNewItem (CMainWnd *pMainWnd, CTreeViewCtrl &tv, 
                        CTableItemObject *pNewItem, HTREEITEM hInsertAfter);

        // @cmember Add a new item to table refs

        void AddNewItemToTableRefs (CMainWnd *pMainWnd, CTreeViewCtrl &tv,
                                    CTableItemObject *pObject, LPCTSTR pszTableName);

        // @cmember Sort all the table refs

        void SortTableRefs (CMainWnd *pMainWnd, CTreeViewCtrl &tv,
                            LPCTSTR pszTableName);

        // @cmember Get the default chance for a table

        int GetDefaultChance ();

        // @cmember Count the number of table refs

        void CountTableRefs (CMainWnd *pMainWnd);

        // @cmmeber Remove table ref counts

        void RemoveTableRefCount (LPCTSTR pszTableName);

        // @cmember Remove table recursions

        void RemoveTableRecusions (CMainWnd *pMainWnd);

        // @cmember Write profile name to number body

        bool WriteProfileTest (FILE *fp);

        // @access Public virtual methods
    public:

        // @cmember Get the name of the object

        virtual void GetDescription (CMainWnd *pMainWnd,
                                     LPTSTR pszName, int cchMaxText);

        // @cmember Get the image

        virtual Object_Image GetImage ();

        // @cmember See if the object has children

        virtual bool HasChildren (CMainWnd *pMainWnd)
        {
            return m_apItems .GetCount () > 0;
        }

        // @cmember Add children

        virtual void AddChildren (CMainWnd *pMainWnd, 
                                  CTreeViewCtrl &tv, HTREEITEM hParent)
        {
            for (int i = 0; i < m_apItems .GetCount (); i++)
                m_apItems [i] ->AddToTV (tv, hParent, TVI_LAST);
        }

        // @cmember Generate ref counts

        virtual void ComputeRefCounts (CMainWnd *pMainWnd)
        {
            for (int i = 0; i < m_apItems .GetCount (); i++)
                m_apItems [i] ->ComputeRefCounts (pMainWnd);
        }

        // @cmember Write the routine body

        virtual void WriteRoutineBody (CMainWnd *pMainWnd, FILE *fp, bool fDebug);

        // @cmember Get the object type

        virtual Object_Type GetObjectType () const
        {
            return Object_Type_Table;
        }

        // @cmember Compare two objects

        virtual bool Compare (const CObject *pRhs) const
        {
            return false;
        }

        // @cmember Edit the object

        virtual bool TableEdit (CMainWnd *pMainWnd, HTREEITEM hItem);

        // @cmember Can this object be edited

        virtual bool CanTableEdit () const
        {
            return m_nTableType != Table_Type_Profile;
        }

        // @cmember Can this object have new items added

        virtual bool CanNew () const
        {
            return true;
        }

        // @cmember Can this object have new items added

        virtual bool CanTableNew () const
        {
            return m_nTableType != Table_Type_Profile;
        }

        // @cmember Can this object be deleted

        virtual bool CanDelete () const
        {
            return m_nTableType != Table_Type_Profile;
        }

        // @cmember Delete the object

        virtual bool Delete (CMainWnd *pMainWnd, CTreeViewCtrl &tv, 
                             HTREEITEM hItem, bool fPrompt);

        // @cmember Can this object move children

        virtual bool CanMoveChild () const
        {
            return true;
        }

        // @cmember Move a child

        virtual void MoveChild (CMainWnd *pMainWnd, CTreeViewCtrl &tv, 
                                CTableItemObject *pChild, int nDirection);

        // @cmember Delete children from the TV

        virtual bool DeleteChildFromTV (CMainWnd *pMainWnd, 
                                        CTreeViewCtrl &tv, HTREEITEM hItem)
        {
            for (int i = 0; i < m_apItems .GetCount (); i++)
            {
                if (m_apItems [i] ->DeleteFromTV (pMainWnd, tv, hItem))
                    return true;
            }
            return false;
        }


        // @cmember Change the table name

        virtual void ChangeTableName (CTableObject *pTable, 
                                      const CString &strOldName, const CString &strNewName)
        {
            for (int i = 0; i < m_apItems .GetCount (); i++)
                m_apItems [i] ->ChangeTableName (pTable, strOldName, strNewName);
        }

        // @cmember Sort the children in the tree

        virtual void SortTV (CMainWnd *pMainWnd, 
                             CTreeViewCtrl &tv, HTREEITEM hItem)
        {
            DoSort (tv, NULL, hItem);
            return;
        }

        // @cmember Compare TV routine

        virtual int CompareTV (LPARAM lp1, LPARAM lp2)
        {
            CTableItemObject *p1 = (CTableItemObject *) lp1;
            CTableItemObject *p2 = (CTableItemObject *) lp2;

            if (p1 == p2)
                return 0;
            for (int i = 0; i < m_apItems .GetCount (); i++)
            {
                if (m_apItems [i] == p1)
                    return -1;
                if (m_apItems [i] == p2)
                    return 1;
            }
            return 0;
        }

        // @cmember Collect used tabled

        virtual void CollectUsedTables (CMainWnd *pMainWnd, 
                                        CAtlArray <CTableObject *> &apTables)
        {
            if (AddToUsedTables (apTables))
            {
                for (int i = 0; i < m_apItems .GetCount (); i++)
                {
                    m_apItems [i] ->CollectUsedTables (pMainWnd, apTables);
                }
            }
            return;
        }

        // @cmember Collect string resources

        virtual void CollectStrRes (CAtlArray <CString> &astrStrRes)
        {
            for (int i = 0; i < m_apItems .GetCount (); i++)
            {
                m_apItems [i] ->CollectStrRes (astrStrRes);
            }
        }

        // @cmember Write an XML file

        virtual void WriteXML (FILE *fp)
        {
            USES_CONVERSION;

            fprintf (fp, _T (" <Table>\r\n"));
            fprintf (fp, _T ("  <Name>%s</Name>\r\n"), T2CA (m_strName));  
            fprintf (fp, _T ("  <Command>%d</Command>\r\n"), m_nTableCommand);  
            fprintf (fp, _T ("  <Global>%d</Global>\r\n"), m_fGlobal ? 1 : 0);
            fprintf (fp, _T ("  <TableType>%d</TableType>\r\n"), m_nTableType);
            if (m_nNumber > 0)
                fprintf (fp, _T ("  <Number>%d</Number>\r\n"), m_nNumber);
            for (int i = 0; i < m_apItems .GetCount (); i++)
                m_apItems [i] ->WriteXML (fp);
            fprintf (fp, _T (" </Table>\r\n"));
            return;
        }

        // @cmember Start an XML element

        virtual void OnStartElement (CDataParser *pParser, 
                                     const XML_Char *pszName, const XML_Char **papszAttrs);

        // @cmember End an XML element

        virtual void OnEndElement (CDataParser *pParser, 
                                   const XML_Char *pszName);

        // @access Helper methods
    public:

        // @cmember Get the name

        const CString &GetName () const
        {
            return m_strName;
        }

        // @cmember Set the name

        void SetName (CString &str)
        {
            m_strName = str;
        }

        // @cmember Get the table command

        Table_Command GetTableCommand () const
        {
            return m_nTableCommand;
        }

        // @cmember Set the table command

        void SetTableCommand (Table_Command nTableCommand)
        {
            m_nTableCommand = nTableCommand;
        }

        // @cmember Get the global flag

        bool GetGlobal () const
        {
            return m_fGlobal;
        }

        // @cmember Set the global flag

        void SetGlobal (bool fGlobal)
        {
            m_fGlobal = fGlobal;
        }

        // @cmember Get the table type

        Table_Type GetTableType () const
        {
            return m_nTableType;
        }

        // @cmember Set the table type

        void SetTableType (Table_Type nTableType)
        {
            m_nTableType = nTableType;
        }

        // @cmember Get the number

        int GetNumber () const
        {
            return m_nNumber;
        }

        // @cmember Set the number

        void SetNumber (int nNumber)
        {
            m_nNumber = nNumber;
        }

        // @cmember Add a new item

        void AddItem (CTableItemObject *pItem)
        {
            m_apItems .Add (pItem);
        }

        // @cmember Remvoe an item

        void RemoveItem (CTableItemObject *pItem)
        {
            for (int i = 0; i < m_apItems .GetCount (); i++)
            {
                if (m_apItems [i] == pItem)
                {
                    m_apItems .RemoveAt (i);
                    return;
                }
            }
            return;
        }

        // @cmember Add a reference

        void AddRef ()
        {
            m_lRefCount++;
        }

        // @cmember Release a reference

        void ReleaseRef ()
        {
            m_lRefCount--;
        }

        // @cmember Get the ref count

        int GetRefCount ()
        {
            return m_lRefCount;
        }

        // @cmember Test for root

        bool IsRoot ()
        {
            return m_fGlobal || m_lRefCount == 0;
        }

        // @access Protected methods
    protected:

        // @cmember Get the XML element number

        Element GetElement (const XML_Char *pszName);

        // @cmember Write body for a list

        void WriteRoutineBodyList (CMainWnd *pMainWnd, FILE *fp,
                                   int nID, int nLast, LPCSTR pszTestFormat, LPCSTR pszAnyFormat, 
                                   bool fDebug);

        // @cmember Add this object to the used tables

        bool AddToUsedTables (CAtlArray <CTableObject *> &apTables)
        {
            for (int i = 0; i < apTables .GetCount (); i++)
            {
                if (apTables [i] == this)
                    return false;
            }
            apTables .Add (this);
            return true;
        }

        // @access Public variables
    public:

        // @cmember Table ref count (used by export)

        int                                     m_nTableRefCount;

        // @access Protected variables
    protected:

        // @cmember Name of the table

        CString                         m_strName;

        // @cmember Table command

        Table_Command           m_nTableCommand;

        // @cmember List of items

        CAtlArray <CTableItemObject *> m_apItems;

        // @cmember Global flag

        bool                            m_fGlobal;

        // @cmember Table number

        int                                     m_nNumber;

        // @cmember Table type

        Table_Type                      m_nTableType;

        // @cmember Reference count

        int                                     m_lRefCount;
};

#endif // ETS_TABLEOBJECT_H

/* vim: set sw=4: */
