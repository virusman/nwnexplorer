#ifndef ETS_TABLEREFOBJECT_H
#define ETS_TABLEREFOBJECT_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      TableRefObject.h - Table ref object |
//
// This module contains the definition of the table ref object.
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

#include "TableItemObject.h"
#include "TableObject.h"

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

class CTableRefObject : public CTableItemObject
{
    public:
        enum Element
        {
            Unknown                     = 0,
            TableRef            = 1,
            Chance                      = 2,
            Name                        = 3,
            Count                       = 4,
            Range                       = 5,
            Modifier            = 6,
            Specific            = 7,
        };

        // @access Constructors and destructors
    public:

        // @cmember General constructor

        CTableRefObject ();

        // @cmember General destructor

        virtual ~CTableRefObject ();

        // @access Public methods
    public:

        // @cmember Return the referenced table

        CTableObject *GetTableRef (CMainWnd *pMainWnd);

        // @cmember Add a new item

        void AddNewItem (CMainWnd *pMainWnd, CTreeViewCtrl &tv, 
                         CTableItemObject *pNewItem)
        {

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
        }

        // @access Public virtual methods
    public:

        // @cmember Get the image

        virtual Object_Image GetImage ();

        // @cmember See if the object has children

        virtual bool HasChildren (CMainWnd *pMainWnd);

        // @cmember Add children

        virtual void AddChildren (CMainWnd *pMainWnd, 
                                  CTreeViewCtrl &tv, HTREEITEM hParent);

        // @cmember Generate ref counts

        virtual void ComputeRefCounts (CMainWnd *pMainWnd);

        // @cmember Get the full text of object

        virtual CString GetFullName (CMainWnd *pMainWnd);

        // @cmember Write the routine body

        virtual void WriteRoutineBody (CMainWnd *pMainWnd, FILE *fp, bool fDebug);

        // @cmember Get the object type

        virtual Object_Type GetObjectType () const
        {
            return Object_Type_Table_Ref;
        }

        // @cmember Compare two objects

        virtual bool Compare (const CObject *pRhs) const
        {
            if (pRhs ->GetObjectType () != Object_Type_Table_Ref)
                return false;
            const CTableRefObject *pSRhs = static_cast <const CTableRefObject *> (pRhs);
            if (_tcscmp (m_strName, pSRhs ->m_strName) != 0)
                return false;
            return 
                m_nCount == pSRhs ->m_nCount &&
                m_nRange == pSRhs ->m_nRange &&
                m_nModifier == pSRhs ->m_nModifier &&
                m_nSpecific == pSRhs ->m_nSpecific;
        }

        // @cmember Can this object be edited

        virtual bool CanEdit () const
        {
            return true;
        }

        // @cmember Can this object move children

        virtual bool CanMoveChild () const
        {
            return true;
        }

        // @cmember Move a child

        virtual void MoveChild (CMainWnd *pMainWnd, CTreeViewCtrl &tv, 
                                CTableItemObject *pChild, int nDirection)
        {
            CTableObject *pTable = GetTableRef (pMainWnd);
            if (pTable)
                pTable ->MoveChild (pMainWnd, tv, pChild, nDirection);
            return;
        }

        // @cmember Edit the object

        virtual bool TableEdit (CMainWnd *pMainWnd, HTREEITEM hItem);

        // @cmember Can this object be edited

        virtual bool CanTableEdit () const
        {
            return true;
        }

        // @cmember Can this object have new items added

        virtual bool CanNew () const
        {
            return true;
        }

        // @cmember Delete children from the TV

        virtual bool DeleteChildFromTV (CMainWnd *pMainWnd, 
                                        CTreeViewCtrl &tv, HTREEITEM hItem);

        // @cmember Change the table name

        virtual void ChangeTableName (CTableObject *pTable, 
                                      const CString &strOldName, const CString &strNewName)
        {
            if (m_strName == strOldName)
                m_strName = strNewName;
        }

        // @cmember Delete the object

        virtual bool Delete (CMainWnd *pMainWnd, CTreeViewCtrl &tv, 
                             HTREEITEM hItem, bool fPrompt);

        // @cmember Sort the children in the tree

        virtual void SortTV (CMainWnd *pMainWnd, CTreeViewCtrl &tv, HTREEITEM hItem)
        {
            CTableObject *pTable = GetTableRef (pMainWnd);
            if (pTable)
                DoSort (tv, pTable, hItem);
            return;
        }

        // @cmember Collect used tabled

        virtual void CollectUsedTables (CMainWnd *pMainWnd, 
                                        CAtlArray <CTableObject *> &apTables)
        {
            CTableObject *pTable = GetTableRef (pMainWnd);
            if (pTable)
                pTable ->CollectUsedTables (pMainWnd, apTables);
            return;
        }

        // @cmember Write an XML file

        virtual void WriteXML (FILE *fp)
        {
            USES_CONVERSION;

            fprintf (fp, _T ("  <TableRef>\r\n"));
            fprintf (fp, _T ("   <Chance>%d</Chance>\r\n"), m_nChance);  
            fprintf (fp, _T ("   <Name>%s</Name>\r\n"), T2CA (m_strName));  
            fprintf (fp, _T ("   <Count>%d</Count>\r\n"), m_nCount);  
            fprintf (fp, _T ("   <Range>%d</Range>\r\n"), m_nRange);  
            fprintf (fp, _T ("   <Modifier>%d</Modifier>\r\n"), m_nModifier);  
            fprintf (fp, _T ("   <Specific>%d</Specific>\r\n"), m_nSpecific);  
            fprintf (fp, _T ("  </TableRef>\r\n"));
            return;
        }

        // @cmember Start an XML element

        virtual void OnStartElement (CDataParser *pParser, 
                                     const XML_Char *pszName, const XML_Char **papszAttrs);

        // @cmember End an XML element

        virtual void OnEndElement (CDataParser *pParser, 
                                   const XML_Char *pszName);

        // @access Public inline methods
    public:

        // @cmember Get the count

        int GetCount () const
        {
            return m_nCount;
        }

        // @cmember Set the count

        void SetCount (int nCount)
        {
            m_nCount = nCount;
        }

        // @cmember Get the range

        int GetRange () const
        {
            return m_nRange;
        }

        // @cmember Set the range

        void SetRange (int nRange)
        {
            m_nRange = nRange;
        }

        // @cmember Get the modifier

        int GetModifier () const
        {
            return m_nModifier;
        }

        // @cmember Set the modifier

        void SetModifier (int nModifier)
        {
            m_nModifier = nModifier;
        }

        // @cmember Get the specific

        int GetSpecific () const
        {
            return m_nSpecific;
        }

        // @cmember Set the specific

        void SetSpecific (int nSpecific)
        {
            m_nSpecific = nSpecific;
        }

        // @access Protected methods
    protected:

        // @cmember Get the XML element number

        Element GetElement (const XML_Char *pszName);

        // @cmember Write the actual call

        void WriteRoutineBodyCall (CMainWnd *pMainWnd, 
                                   FILE *fp, CTableObject *pTable, bool fDebug);

        // @access Protected methods
    protected:

        // @cmember Number of items to create

        int                                     m_nCount;

        // @cmember Range in the item count

        int                                     m_nRange;

        // @cmember Modifier

        int                                     m_nModifier;

        // @cmember Specific

        int                                     m_nSpecific;
};

#endif // ETS_TABLEREFOBJECT_H

/* vim: set sw=4: */
