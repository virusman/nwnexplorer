#ifndef ETS_OBJECT_H
#define ETS_OBJECT_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      Object.h - Root object for all of the managed object |
//
// This module contains the definition of the root object.
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

#include "nwntreasure.h"
#include "ExpatImpl.h"

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

class CTableObject;
class CTableItemObject;
class CMainWnd;
class CDataParser;

//-----------------------------------------------------------------------------
//
// Enumerations
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CObject
{

    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CObject ();

        // @cmember General destructor

        virtual ~CObject ();

        // @access Public methods
    public:

        // @cmember Insert in tree view

        HTREEITEM AddToTV (CTreeViewCtrl &tv, HTREEITEM hParent, 
                           HTREEITEM hInsertAfter); 

        // @cmember Delete the object from the TV

        bool DeleteFromTV (CMainWnd *pMainWnd, 
                           CTreeViewCtrl &tv, HTREEITEM hItem);

        // @cmember Delete children from the TV

        void DeleteChildrenFromTV (CMainWnd *pMainWnd, 
                                   CTreeViewCtrl &tv, HTREEITEM hItem);

        // @cmember Get the HTREEITEM

        HTREEITEM GetHItem (int nItem = 0) const
        {
            if (nItem < 0 || nItem >= m_ahtvItem .GetCount ())
                return NULL;
            else
                return m_ahtvItem [nItem];
        }

        // @cmember Test to see if the object has the HTREEITEM

        int GetHItemIndex (HTREEITEM hItem) const
        {
            for (int i = 0; i < m_ahtvItem .GetCount (); i++)
                if (m_ahtvItem [i] == hItem)
                    return i;
            return -1;
        }

        // @access Public virtual methods
    public:

        // @cmember Get the name of the object

        virtual void GetDescription (CMainWnd *pMainWnd,
                                     LPTSTR pszName, int cchMaxText) = 0;

        // @cmember Get the image

        virtual Object_Image GetImage () = 0;

        // @cmember See if the object has children

        virtual bool HasChildren (CMainWnd *pMainWnd) = 0;

        // @cmember Add children

        virtual void AddChildren (CMainWnd *pMainWnd,
                                  CTreeViewCtrl &tv, HTREEITEM hParent)
        {
            return;
        }

        // @cmember Generate ref counts

        virtual void ComputeRefCounts (CMainWnd *pMainWnd)
        {
            return;
        }

        // @cmember Write the routine body

        virtual void WriteRoutineBody (CMainWnd *pMainWnd, FILE *fp, bool fDebug)
        {
            return;
        }

        // @cmember Get the object type

        virtual Object_Type GetObjectType () const = 0;

        // @cmember Compare two objects

        virtual bool Compare (const CObject *pRhs) const = 0;

        // @cmember Can this object be edited

        virtual bool CanEdit () const
        {
            return false;
        }

        // @cmember Edit the object

        virtual bool TableEdit (CMainWnd *pMainWnd, HTREEITEM hItem)
        {
            return false;
        }

        // @cmember Can this object be edited

        virtual bool CanTableEdit () const
        {
            return false;
        }

        // @cmember Delete the object

        virtual bool Delete (CMainWnd *pMainWnd, CTreeViewCtrl &tv, 
                             HTREEITEM hItem, bool fPrompt)
        {
            return false;
        }

        // @cmember Can this object be deleted

        virtual bool CanDelete () const
        {
            return false;
        }

        // @cmember Can this object have new items added

        virtual bool CanNew () const
        {
            return false;
        }

        // @cmember Can this object have new items added

        virtual bool CanTableNew () const
        {
            return true;
        }

        // @cmember Can this object move children

        virtual bool CanMoveChild () const
        {
            return false;
        }

        // @cmember Move a child

        virtual void MoveChild (CMainWnd *pMainWnd, CTreeViewCtrl &tv, 
                                CTableItemObject *pChild, int nDirection)
        {
            return;
        }

        // @cmember Delete children from the TV

        virtual bool DeleteChildFromTV (CMainWnd *pMainWnd, 
                                        CTreeViewCtrl &tv, HTREEITEM hItem)
        {
            _ASSERTE (FALSE);
            return false;
        }

        // @cmember Change the table name

        virtual void ChangeTableName (CTableObject *pTable, 
                                      const CString &strOldName, const CString &strNewName)
        {
            return;
        }

        // @cmember Sort the children in the tree

        virtual void SortTV (CMainWnd *pMainWnd, 
                             CTreeViewCtrl &tv, HTREEITEM hItem)
        {
            return;
        }

        // @cmember Compare TV routine

        virtual int CompareTV (LPARAM lp1, LPARAM lp2)
        {
            return 0;
        }

        // @cmember Collect used tabled

        virtual void CollectUsedTables (CMainWnd *pMainWnd, 
                                        CAtlArray <CTableObject *>      &apTables)
        {
            return;
        }

        // @cmember Collect string resources

        virtual void CollectStrRes (CAtlArray <CString> &astrStrRes)
        {
            return;
        }

        // @cmember Write an XML file

        virtual void WriteXML (FILE *fp)
        {
            return;
        }

        // @cmember Start an XML element

        virtual void OnStartElement (CDataParser *pParser, 
                                     const XML_Char *pszName, const XML_Char **papszAttrs)
        {
            return;
        }

        // @cmember End an XML element

        virtual void OnEndElement (CDataParser *pParser, 
                                   const XML_Char *pszName)
        {
            return;
        }

        // @access Protected methods
    protected:

        // @cmember Perform the actual sort

        void DoSort (CTreeViewCtrl &tv, CObject *pCompare, HTREEITEM hItem)
        {
            if (pCompare == NULL)
                pCompare = this;
            if (hItem == NULL)
            {
                for (int i = 0; i < m_ahtvItem .GetCount (); i++)
                    DoSort (tv, pCompare, m_ahtvItem [i]);
            }
            else
            {
                TVSORTCB tvs;
                tvs .hParent = hItem;
                tvs .lpfnCompare = OnCompareFunc;
                tvs .lParam = (LPARAM) pCompare;
                tv .SortChildrenCB (&tvs);
            }
        }

        // @cmember Tree view item comparison

        static int CALLBACK OnCompareFunc (LPARAM lParam1, 
                                           LPARAM lParam2, LPARAM lParamSort)
        {
            CObject *pObject = (CObject *) lParamSort;
            return pObject ->CompareTV (lParam1, lParam2);
        }

        // @access Protected variables
    protected:

        // @cmember Array of where this object exists in the tree control

        CAtlArray <HTREEITEM>   m_ahtvItem;
};

#endif // ETS_OBJECT_H

/* vim: set sw=4: */
