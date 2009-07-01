#ifndef ETS_DATASOURCE_H
#define ETS_DATASOURCE_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      DataSource.h - Resource file data source |
//
// This module contains the definition of the resource file data sources.
//
// Copyright (c) 2003 - Edward T. Smith
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
// $History: HexView.h $
//      
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include "../_NwnLib/NwnDefines.h"

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

class CDataSource;
class CData;

//-----------------------------------------------------------------------------
//
// Helper structures
//
//-----------------------------------------------------------------------------

struct DataElement
{
    CDataSource         *pDataSource;
    UINT16                      usType;
    UINT16                      usIndex2;
    UINT32                      ulIndex1;
};

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CDataSource
{

    // @access Construction and destruction
    public:

        // @cmember General constructor

        CDataSource ();

        // @cmember General destructor

        virtual ~CDataSource ();

        // @access Public method
    public:

        // @cmember Does this element have children

        virtual bool HasChildren (DataElement *pElement) = 0;

        // @cmember Is this element a resource

        virtual bool IsResource (DataElement *pElement) = 0;

        // @cmember Can export

        virtual bool CanExport (DataElement *pElement) = 0;

        // @cmember Can export TGA

        virtual bool CanExportTga (DataElement *pElement) = 0;

        // @cmember Add the root item

        virtual HTREEITEM AddRoot (CTreeViewCtrl &tv, HTREEITEM hParent) = 0;

        // @cmember Add the children to the tree view

        virtual void AddChildren (CTreeViewCtrl &tv, 
                                  HTREEITEM hParent, DataElement *pElement) = 0;

        // @cmember Get the resource text

        virtual void GetResourceText (DataElement *pElement, 
                                      LPTSTR pszText, int cchMaxText) = 0;

        // @cmember Get the resource name

        virtual void GetResourceName (DataElement *pElement, 
                                      LPTSTR pszText, int cchMaxText, NwnResType *pnResType) = 0;

        // @cmember Load a resource

        virtual CData *LoadRes (DataElement *pElement) = 0;

        // @cmember Export resources

        virtual int Export (DataElement *pElement, 
                            const char *pszFileName) = 0;

        // @cmember Export TGA resources

        virtual int ExportTga (DataElement *pElement, 
                               const char *pszFileName) = 0;

        // @access Protected methods
    protected:

        // @cmember Add an item to the tree view

        HTREEITEM AddToTV (CTreeViewCtrl &tv, 
                           HTREEITEM hParent, DataElement *pElement);
};

#endif // ETS_DATASOURCE_H

/* vim: set sw=4: */
