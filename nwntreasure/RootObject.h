#ifndef ETS_ROOTOBJECT_H
#define ETS_ROOTOBJECT_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      RootObject.h - Root object for all of the managed object |
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

#include "Object.h"
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

class CRootObject : public CObject
{

    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CRootObject (Table_Type nTableType);

        // @cmember General destructor

        virtual ~CRootObject ();

        // @access Public methods
    public:

        // @cmember Add a new item

        void AddNewItem (CMainWnd *pMainWnd, CTreeViewCtrl &tv, 
                         CObject *pNewItem)
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

        // @cmember Get the name of the object

        virtual void GetDescription (CMainWnd *pMainWnd,
                                     LPTSTR pszName, int cchMaxText);

        // @cmember Get the image

        virtual Object_Image GetImage ();

        // @cmember See if the object has children

        virtual bool HasChildren (CMainWnd *pMainWnd);

        // @cmember Add children

        virtual void AddChildren (CMainWnd *pMainWnd, 
                                  CTreeViewCtrl &tv, HTREEITEM hParent);

        // @cmember Get the object type

        virtual Object_Type GetObjectType () const
        {
            return Object_Type_Root;
        }

        // @cmember Compare two objects

        virtual bool Compare (const CObject *pRhs) const
        {
            return false;
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
            CTableObject *p1 = (CTableObject *) lp1;
            CTableObject *p2 = (CTableObject *) lp2;
            return _tcscmp (p1 ->GetName (), p2 ->GetName ());
        }

        // @cmember Can this object have new items added

        virtual bool CanTableNew () const
        {
            return m_nTableType != Table_Type_Profile;
        }

        // @access Protected methods
    protected:

        // @cmember Type of table

        Table_Type                      m_nTableType;
};

#endif // ETS_ROOTOBJECT_H

/* vim: set sw=4: */
