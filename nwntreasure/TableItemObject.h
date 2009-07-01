#ifndef ETS_TABLETABLEITEMOBJECT_H
#define ETS_TABLETABLEITEMOBJECT_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      TableItemObject.h - Table Item object |
//
// This module contains the definition of the table item object.
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

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

class CTableObject;

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CTableItemObject : public CObject
{

    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CTableItemObject ();

        // @cmember General destructor

        virtual ~CTableItemObject ();

        // @access Public methods
    public:

        // @cmember Set the default chance based on the table command

        void SetDefaultChance ();

        // @access Public virtual methods
    public:

        // @cmember Get the name of the object

        virtual void GetDescription (CMainWnd *pMainWnd, 
                                     LPTSTR pszName, int cchMaxText);

        // @cmember See if the object has children

        virtual bool HasChildren (CMainWnd *pMainWnd)
        {
            return false;
        }

        // @cmember Get the full text of object

        virtual CString GetFullName (CMainWnd *pMainWnd)
        {
            return m_strName;
        }

        // @cmember Delete the object

        virtual bool Delete (CMainWnd *pMainWnd, CTreeViewCtrl &tv, 
                             HTREEITEM hItem, bool fPrompt);

        // @cmember Can this object be deleted

        virtual bool CanDelete () const
        {
            return true;
        }

        // @access Public methods
    public:

        // @cmember Get the table

        CTableObject *GetTable () const
        {
            return m_pTable;
        }

        // @cmember Set the table

        void SetTable (CTableObject *pTable)
        {
            m_pTable = pTable;
        }

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

        // @cmember Get the chance

        int GetChance () const
        {
            return m_nChance;
        }

        // @cmember Set the chance

        void SetChance (int nChance) 
        {
            m_nChance = nChance;
        }

        // @access Protected methods
    protected:

        // @cmember Parent table

        CTableObject                    *m_pTable;

        // @cmember Name of the item

        CString                                 m_strName;

        // @cmember Chance of the item

        int                                             m_nChance;
};

#endif // ETS_TABLETABLEITEMOBJECT_H

/* vim: set sw=4: */
