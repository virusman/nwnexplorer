//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      DataSourceBif.cpp - Resource data source |
//
// This module contains the definition of the resource data source.
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
// $History: DataCache.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"
#include "DataSourceBif.h"
#include "NwnExplorer.h"
#include "Data.h"
#include "DataCache.h"

//
// Debug NEW
//

#if defined (_DEBUG) && !defined (_WIN32_WCE)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// Externals
//

extern CDataCache g_sResourceCache;

//-----------------------------------------------------------------------------
//
// @mfunc <c CDataSourceBif> constructor.
//
// @parm CNwnBifFile * | pBifFile | Pointer to the bif file
//
// @parm const char * | pszFileName | Name of the bif file
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CDataSourceBif::CDataSourceBif (CNwnBifFile *pBifFile, 
                                const char *pszFileName) : m_strFileName (pszFileName)
{

    //
    // Save the settings
    //

    m_pBifFile = pBifFile;

    //
    // Save the list of resources
    //

    m_pasRes = m_pBifFile ->GetRes (0);

    //
    // Compute the number of elements and allocate
    //

    int nResCount = m_pBifFile ->GetResCount ();
    int nElementCount = 1 + (ResClass__Last + 1) + nResCount;
    m_pasElements = new DataElement [nElementCount];

    //
    // Initialize the resources
    //

    for (int i = 0; i < nResCount; i++)
    {
        m_pasElements [i] .pDataSource = this;
        m_pasElements [i] .usType = TypeResourceource;
        m_pasElements [i] .usIndex2 = 0;
        m_pasElements [i] .ulIndex1 = (UINT32) i;
    }

    //
    // Initialize the root
    //

    m_nRootElement = nResCount;
    m_pasElements [m_nRootElement] .pDataSource = this;
    m_pasElements [m_nRootElement] .usType = Type_Root;
    m_pasElements [m_nRootElement] .usIndex2 = 0;
    m_pasElements [m_nRootElement] .ulIndex1 = 0;

    //
    // Initialize the bif's resclass entries
    //

    m_nFirstResClassElement = m_nRootElement + 1;
    int n = m_nFirstResClassElement;
    for (int i = 0; i < ResClass__Last; i++)
    {
        m_pasElements [n] .pDataSource = this;
        m_pasElements [n] .usType = Type_Root; //adjusted later
        m_pasElements [n] .usIndex2 = (UINT16) i;
        m_pasElements [n] .ulIndex1 = (UINT32) 0;
        n++;
    }

    //
    // Loop through the resources and determine which res classes
    // are valid for each bif
    //

    for (int i = 0; i < nResCount; i++)
    {
        ResClass nTI = GetResourceClass ((NwnResType) m_pasRes [i] .ulType, NULL);
        n = m_nFirstResClassElement + nTI;
        m_pasElements [n] .usType = TypeResourceClass;
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CDataSourceBif> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CDataSourceBif::~CDataSourceBif ()
{
    delete [] m_pasElements;
    delete m_pBifFile;
}

//-----------------------------------------------------------------------------
//
// @mfunc Does this element have children
//
// @parm DataElement * | pElement | Resource element
//
// @rdesc Resulting flag
//
//-----------------------------------------------------------------------------

bool CDataSourceBif::HasChildren (DataElement *pElement)
{
    return pElement ->usType != TypeResourceource;
}

//-----------------------------------------------------------------------------
//
// @mfunc Is this element a resource
//
// @parm DataElement * | pElement | Resource element
//
// @rdesc Resulting flag
//
//-----------------------------------------------------------------------------

bool CDataSourceBif::IsResource (DataElement *pElement)
{
    return pElement ->usType == TypeResourceource;
}

//-----------------------------------------------------------------------------
//
// @mfunc Can this element be exported
//
// @parm DataElement * | pElement | Resource element
//
// @rdesc Resulting flag
//
//-----------------------------------------------------------------------------

bool CDataSourceBif::CanExport (DataElement *pElement)
{
    return pElement ->usType != Type_Root;
}

//-----------------------------------------------------------------------------
//
// @mfunc Can this element be exported as a TGA
//
// @parm DataElement * | pElement | Resource element
//
// @rdesc Resulting flag
//
//-----------------------------------------------------------------------------

bool CDataSourceBif::CanExportTga (DataElement *pElement)
{
    if (!IsResource(pElement))
        return false;

    char szName [32];
    NwnResType nResType;

    GetResourceName (pElement, szName, _countof (szName), &nResType);

    return (nResType == NwnResType_DDS ||
            nResType == NwnResType_PLT ||
            nResType == NwnResType_TGA ||
            nResType == NwnResType_MDL);
}

//-----------------------------------------------------------------------------
//
// @mfunc Add the root item
//
// @parm CTreeViewCtrl & | tv | Tree view control
//
// @parm HTREEITEM | hParent | Parent item
//
// @rdesc Handle of the item
//
//-----------------------------------------------------------------------------

HTREEITEM CDataSourceBif::AddRoot (CTreeViewCtrl &tv, HTREEITEM hParent)
{
    return AddToTV (tv, hParent, &m_pasElements [m_nRootElement]);
}

//-----------------------------------------------------------------------------
//
// @mfunc Add the children of an element
//
// @parm CTreeViewCtrl & | tv | Tree view control
//
// @parm HTREEITEM | hParent | Parent item
//
// @parm DataElement * | pElement | Resource element
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CDataSourceBif::AddChildren (CTreeViewCtrl &tv, 
                                  HTREEITEM hParent, DataElement *pElement)
{

    //
    // Based on the type
    //

    switch (pElement ->usType)
    {
        case Type_Root:
            {
                int n = m_nFirstResClassElement + 
                    (pElement ->ulIndex1 * ResClass__Last);
                for (int i = ResClass__First; i < ResClass__Last; i++)
                {
                    if (m_pasElements [n + i] .usType == TypeResourceClass)
                        AddToTV (tv, hParent, &m_pasElements [n + i]);
                }
            }
            break;

        case TypeResourceClass:
            {
                int nResCount = m_pBifFile ->GetResCount ();
                for (int i = 0; i < nResCount; i++)
                {
                    if (pElement ->usIndex2 != GetResourceClass (
                            (NwnResType) m_pasRes [i] .ulType, NULL))
                        continue;
                    AddToTV (tv, hParent, &m_pasElements [i]);
                }
            }
            break;

        case TypeResourceource:
            break;
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the resource text
//
// @parm DataElement * | pElement | Resource element
//
// @parm LPTSTR | pszText | Destination text
//
// @parm int | cchMaxText | Length of the buffer
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CDataSourceBif::GetResourceText (DataElement *pElement, 
                                      LPTSTR pszText, int cchMaxText)
{

    //
    // Based on the type
    //

    switch (pElement ->usType)
    {
        case Type_Root:
            strncpy (pszText, m_strFileName, cchMaxText);
            pszText [cchMaxText - 1] = 0;
            break;

        case TypeResourceClass:
            LoadString (_Module .GetResourceInstance (), 
                        IDS_RESCLASS_FIRST + pElement ->usIndex2, 
                        pszText, cchMaxText); 
            break;

        case TypeResourceource:
            {
                const CNwnBifFile::Resource *psRes = &m_pasRes [pElement ->ulIndex1];
                LPCTSTR pszExt = NwnGetResTypeExtension (
                    (NwnResType) psRes ->ulType);
                TCHAR szTemp [32];
                if (pszExt == NULL)
                {
                    _stprintf (szTemp, "res%d.%X", 
                               pElement ->ulIndex1, psRes ->ulType);
                    pszExt = szTemp;
                }
                else
                {
                    _stprintf (szTemp, "res%d.%s", 
                               pElement ->ulIndex1, pszExt);
                }
                strncpy (pszText, szTemp, cchMaxText);
            }
            break;
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the resource name
//
// @parm DataElement * | pElement | Resource element
//
// @parm LPTSTR | pszText | Destination text
//
// @parm int | cchMaxText | Length of the buffer
//
// @parm NwnResType * | pnResType | Resource type
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CDataSourceBif::GetResourceName (DataElement *pElement, 
                                      LPTSTR pszText, int cchMaxText, NwnResType *pnResType)
{

    //
    // Return if we aren't a resource
    //

    if (pElement ->usType != TypeResourceource)
        return;

    //
    // Get the name
    //

    const CNwnBifFile::Resource *psRes = &m_pasRes [pElement ->ulIndex1];
    char szName [17];
    sprintf (szName, "res%d", pElement ->ulIndex1);
    strncpy (pszText, szName, cchMaxText);
    pszText [cchMaxText - 1] = 0;

    //
    // Save the type
    //

    *pnResType = (NwnResType) psRes ->ulType;
}

//-----------------------------------------------------------------------------
//
// @mfunc Load a resource
//
// @parm DataElement * | pElement | Resource element
//
// @rdesc Address of the resource.
//
//-----------------------------------------------------------------------------

CData *CDataSourceBif::LoadRes (DataElement *pElement)
{

    //
    // Load the resource
    //

    const CNwnBifFile::Resource *psRes = &m_pasRes [pElement ->ulIndex1];
    return g_sResourceCache .LoadRes (m_pBifFile, psRes);
    return NULL;
}

//-----------------------------------------------------------------------------
//
// @mfunc Export a resource
//
// @parm DataElement * | pElement | Resource element
//
// @parm const char * | pszFileName | Destination file name or directory
//
// @rdesc Number of resources exported
//
//-----------------------------------------------------------------------------

int CDataSourceBif::Export (DataElement *pElement, 
                            const char *pszFileName)
{

    //
    // Based on the type
    //

    int nCount = 0;
    switch (pElement ->usType)
    {
        case Type_Root:
            {
                CString strDir (pszFileName);
                int nResCount = m_pBifFile ->GetResCount ();
                for (int i = 0; i < nResCount; i++)
                {
                    CData *pData = LoadRes (&m_pasElements [i]);
                    if (pData)
                    {
                        char szName [_MAX_PATH];
                        GetResourceText (&m_pasElements [i], 
                                         szName, _countof (szName));
                        CString str (strDir + szName);
                        if (pData ->SaveToFile (str))
                            nCount++;
                        pData ->AddRef ();
                        pData ->Release ();
                    }
                }
            }
            break;

        case TypeResourceClass:
            {
                CString strDir (pszFileName);
                int nResCount = m_pBifFile ->GetResCount ();
                for (int i = 0; i < nResCount; i++)
                {
                    if (pElement ->usIndex2 != GetResourceClass (
                            (NwnResType) m_pasRes [i] .ulType, NULL))
                        continue;
                    CData *pData = LoadRes (&m_pasElements [i]);
                    if (pData)
                    {
                        char szName [_MAX_PATH];
                        GetResourceText (&m_pasElements [i], 
                                         szName, _countof (szName));
                        CString str (strDir + szName);
                        if (pData ->SaveToFile (str))
                            nCount++;
                        pData ->AddRef ();
                        pData ->Release ();
                    }
                }
            }
            break;

        case TypeResourceource:
            {
                CData *pData = LoadRes (pElement);
                if (pData)
                {
                    if (pData ->SaveToFile (pszFileName))
                        nCount++;
                    pData ->AddRef ();
                    pData ->Release ();
                }
            }
            break;
    }
    return nCount;
}

//-----------------------------------------------------------------------------
//
// @mfunc Export TGA resources
//
// @parm DataElement * | pElement | Resource element
//
// @parm const char * | pszFileName | Destination file name or directory
//
// @rdesc Number of resources exported
//
//-----------------------------------------------------------------------------

int CDataSourceBif::ExportTga (DataElement *pElement, 
                               const char *pszFileName)
{

    //
    // Based on the type
    //

    int nCount = 0;
    switch (pElement ->usType)
    {
        case Type_Root:
            break;

        case TypeResourceClass:
            {
                CString strDir (pszFileName);
                int nResCount = m_pBifFile ->GetResCount ();
                for (int i = 0; i < nResCount; i++)
                {
                    if (pElement ->usIndex2 != GetResourceClass (
                            (NwnResType) m_pasRes [i] .ulType, NULL))
                        continue;

                    char szName [_MAX_PATH], *p;
                    GetResourceText (&m_pasElements [i], 
                                     szName, _countof (szName));
                    if ((p = strrchr(szName, '.')) != NULL)
                        *p = '\0';

                    CString str (strDir + szName);
                    str += ".tga";

                    if (TgaWrite(&m_pasElements [i], str, (NwnResType) m_pasRes [i] .ulType))
                        nCount++;
                }
            }
            break;

        case TypeResourceource:
            break;
    }
    return nCount;
}

/* vim: set sw=4: */
