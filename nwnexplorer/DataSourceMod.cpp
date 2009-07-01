//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      DataSourceMod.cpp - Resource data source |
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
#include "DataSourceMod.h"
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
// @mfunc <c CDataSourceMod> constructor.
//
// @parm CNwnModuleFile * | pModFile | Pointer to the mod file
//
// @parm LPCTSTR | pszName | Name of the module file
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CDataSourceMod::CDataSourceMod (CNwnModuleFile *pModFile, 
                                LPCTSTR pszName) : m_strModFileName (pszName)
{

    //
    // Save the settings
    //

    m_pModFile = pModFile;

    //
    // Save the list of resources
    //

    m_pasRes = m_pModFile ->GetRes (0);

    //
    // Compute the number of elements and allocate
    //

    int nResCount = m_pModFile ->GetResCount ();
    int nElementCount = 1 + nResCount + ResClass__Last;
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
        m_pasElements [n] .ulIndex1 = 0;
        n++;
    }

    //
    // Loop through the resources and determine which res classes
    // are valid for each bif
    //

    for (int i = 0; i < nResCount; i++)
    {
        ResClass nTI = GetResourceClass ((NwnResType) m_pasRes [i] .ulType,
                                         m_pasRes [i] .szName);
        n = m_nFirstResClassElement + nTI;
        m_pasElements [n] .usType = TypeResourceClass;
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CDataSourceMod> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CDataSourceMod::~CDataSourceMod ()
{
    delete [] m_pasElements;
    m_pModFile ->Close ();
    delete m_pModFile;
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

bool CDataSourceMod::HasChildren (DataElement *pElement)
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

bool CDataSourceMod::IsResource (DataElement *pElement)
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

bool CDataSourceMod::CanExport (DataElement *pElement)
{
    return true;
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

bool CDataSourceMod::CanExportTga (DataElement *pElement)
{
    if (pElement ->usType == TypeResourceClass)
        return true;

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

HTREEITEM CDataSourceMod::AddRoot (CTreeViewCtrl &tv, HTREEITEM hParent)
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

void CDataSourceMod::AddChildren (CTreeViewCtrl &tv, 
                                  HTREEITEM hParent, DataElement *pElement)
{

    //
    // Based on the type
    //

    switch (pElement ->usType)
    {
        case Type_Root:
            {
                int n = m_nFirstResClassElement;
                for (int i = ResClass__First; i < ResClass__Last; i++)
                {
                    if (m_pasElements [n + i] .usType == TypeResourceClass)
                        AddToTV (tv, hParent, &m_pasElements [n + i]);
                }
            }
            break;

        case TypeResourceClass:
            {
                int nResCount = m_pModFile ->GetResCount ();
                for (int i = 0; i < nResCount; i++)
                {
                    if (pElement ->usIndex2 != GetResourceClass (
                            (NwnResType) m_pasRes [i] .ulType, m_pasRes [i] .szName))
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

void CDataSourceMod::GetResourceText (DataElement *pElement, 
                                      LPTSTR pszText, int cchMaxText)
{

    //
    // Based on the type
    //

    switch (pElement ->usType)
    {
        case Type_Root:
            _tcsncpy (pszText, m_strModFileName, cchMaxText);
            break;

        case TypeResourceClass:
            LoadString (_Module .GetResourceInstance (), 
                        IDS_RESCLASS_FIRST + pElement ->usIndex2, 
                        pszText, cchMaxText); 
            break;

        case TypeResourceource:
            {
                const CNwnModuleFile::Resource *psRes = 
                    &m_pasRes [pElement ->ulIndex1];
                char szName [17];
                memcpy (szName, psRes ->szName, 16);
                szName [16] = 0;
                strncpy (pszText, szName, cchMaxText);
                pszText [cchMaxText - 1] = 0;
                LPCTSTR pszExt = NwnGetResTypeExtension (
                    (NwnResType) psRes ->ulType);
                TCHAR szTemp [32];
                if (pszExt == NULL)
                {
                    _stprintf (szTemp, ".%X", psRes ->ulType);
                    pszExt = szTemp;
                }
                strncat (pszText, pszExt, cchMaxText);
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

void CDataSourceMod::GetResourceName (DataElement *pElement, 
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

    const CNwnModuleFile::Resource *psRes = &m_pasRes [pElement ->ulIndex1];
    char szName [17];
    memcpy (szName, psRes ->szName, 16);
    szName [16] = 0;
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

CData *CDataSourceMod::LoadRes (DataElement *pElement)
{

    //
    // Load the resource
    //

    const CNwnModuleFile::Resource *psRes = &m_pasRes [pElement ->ulIndex1];
    return g_sResourceCache .LoadRes (m_pModFile, psRes);
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

int CDataSourceMod::Export (DataElement *pElement, 
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
                int nResCount = m_pModFile ->GetResCount ();
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
                int nResCount = m_pModFile ->GetResCount ();
                for (int i = 0; i < nResCount; i++)
                {
                    if (pElement ->usIndex2 != GetResourceClass (
                            (NwnResType) m_pasRes [i] .ulType, m_pasRes [i] .szName))
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

int CDataSourceMod::ExportTga (DataElement *pElement, 
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
                int nResCount = m_pModFile ->GetResCount ();
                for (int i = 0; i < nResCount; i++)
                {
                    if (pElement ->usIndex2 != GetResourceClass (
                            (NwnResType) m_pasRes [i] .ulType, m_pasRes [i] .szName))
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
