//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      DataSourceDir.cpp - Resource data source |
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
#include "DataSourceDir.h"
#include "NwnExplorer.h"
#include "Data.h"
#include "DataCache.h"
#include "DataSourceMod.h"
#include "MainWnd.h"
#include <io.h>

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
extern CString g_strNwnDirectory;
extern CString g_strUserDirectory;
extern CMainWnd *g_pMainWnd;

//-----------------------------------------------------------------------------
//
// @mfunc <c CDataSourceDir> constructor.
//
// @parm CNwnModuleFile * | pModFile | Pointer to the mod file
//
// @parm LPCTSTR | pszName | Name of the module file
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CDataSourceDir::CDataSourceDir (UINT nID, LPCTSTR pszDir, LPCTSTR pszMask, bool fModules, bool fUserDir)
{

    //
    // Save the settings
    //

    m_nID      = nID;
    m_strDir   = pszDir;
    m_strMask  = pszMask;
    m_fModules = fModules;
    m_fUserDir = fUserDir;

    //
    // Initialize other flags
    //

    m_fInitialized = false;

    //
    // Initialize the root
    //

    m_sRootElement .pDataSource = this;
    m_sRootElement .usType = Type_Root;
    m_sRootElement .usIndex2 = 0;
    m_sRootElement .ulIndex1 = 0;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CDataSourceDir> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CDataSourceDir::~CDataSourceDir ()
{
    for (int i = 0; i < m_apSources .GetCount (); i++)
        delete m_apSources [i];
    m_apSources .RemoveAll ();
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

bool CDataSourceDir::HasChildren (DataElement *pElement)
{
    //
    // Based on the type
    //

    switch (pElement ->usType)
    {
        case Type_Root:
            if (m_fInitialized)
            {
                return m_apSources .GetCount () != 0 ||
                    m_astrFileNames .GetCount () != 0;
            }
            else
                return true;

        case TypeResourceClass:
            return true;

        case TypeResourceource:
            return false;

        default:
            return false;
    }
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

bool CDataSourceDir::IsResource (DataElement *pElement)
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

bool CDataSourceDir::CanExport (DataElement *pElement)
{
    //
    // Based on the type
    //

    switch (pElement ->usType)
    {
        case Type_Root:
            return !m_fModules;

        case TypeResourceClass:
            return true;

        case TypeResourceource:
            return true;

        default:
            return false;
    }
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

bool CDataSourceDir::CanExportTga (DataElement *pElement)
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

HTREEITEM CDataSourceDir::AddRoot (CTreeViewCtrl &tv, HTREEITEM hParent)
{
    return AddToTV (tv, hParent, &m_sRootElement);
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

void CDataSourceDir::AddChildren (CTreeViewCtrl &tv, 
                                  HTREEITEM hParent, DataElement *pElement)
{

    //
    // Initialize the system
    //

    Initialize ();

    //
    // Based on the type
    //

    switch (pElement ->usType)
    {
        case Type_Root:
            if (m_fModules)
            {
                for (int i = 0; i < m_apSources .GetCount (); i++)
                    m_apSources [i] ->AddRoot (tv, hParent);
            }
            else
            {
                for (int i = (int) m_astrFileNames .GetCount ()
                     ; i < (int) m_asElements .GetCount (); i++)
                    AddToTV (tv, hParent, &m_asElements [i]);
            }
            break;

        case TypeResourceClass:
            if (m_fModules)
            {
            }
            else
            {
                for (int i = 0; i < (int) m_astrFileNames .GetCount (); i++)
                {
                    if (m_asElements [i] .usIndex2 == pElement ->usIndex2)
                        AddToTV (tv, hParent, &m_asElements [i]);
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

void CDataSourceDir::GetResourceText (DataElement *pElement, 
                                      LPTSTR pszText, int cchMaxText)
{

    //
    // Based on the type
    //

    switch (pElement ->usType)
    {
        case Type_Root:
            LoadString (_Module .GetResourceInstance (), 
                        m_nID, pszText, cchMaxText); 
            break;

        case TypeResourceClass:
            if (m_fModules)
            {
            }
            else
            {
                LoadString (_Module .GetResourceInstance (), 
                            IDS_RESCLASS_FIRST + pElement ->usIndex2, 
                            pszText, cchMaxText); 
            }
            break;

        case TypeResourceource:
            if (m_fModules)
            {
            }
            else
            {

                strncpy (pszText, m_astrFileNames 
                         [pElement ->ulIndex1], cchMaxText);
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

void CDataSourceDir::GetResourceName (DataElement *pElement, 
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

    const char *pszFileName = m_astrFileNames [pElement ->ulIndex1];
    int nCopy;
    const char *pszExt = strrchr (pszFileName, '.');
    if (pszExt)
        nCopy = pszExt - pszFileName;
    else
        nCopy = (int) strlen (pszFileName);
    if (nCopy > cchMaxText - 1)
        nCopy = cchMaxText - 1;
    memcpy (pszText, pszFileName, nCopy);
    pszText [nCopy] = 0;

    //
    // Save the type
    //

    *pnResType = NwnGetResTypeFromExtension (pszExt);
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

CData *CDataSourceDir::LoadRes (DataElement *pElement)
{
    const char *pszFileName = m_astrFileNames [pElement ->ulIndex1];
    CString str (g_strNwnDirectory + m_strDir + pszFileName);
    return g_sResourceCache .LoadRes (str);
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

int CDataSourceDir::Export (DataElement *pElement, 
                            const char *pszFileName)
{

    //
    // Initialize to make sure everything is loaded
    //

    Initialize ();

    //
    // Based on the type
    //

    int nCount = 0;
    switch (pElement ->usType)
    {
        case Type_Root:
            if (m_fModules)
            {
            }
            else
            {
                CString strDir (pszFileName);
                for (int i = 0; i < (int) m_astrFileNames .GetCount (); i++)
                {
                    CData *pData = LoadRes (&m_asElements [i]);
                    if (pData)
                    {
                        char szName [_MAX_PATH];
                        GetResourceText (&m_asElements [i], 
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
            if (m_fModules)
            {
            }
            else
            {
                CString strDir (pszFileName);
                for (int i = 0; i < (int) m_astrFileNames .GetCount (); i++)
                {
                    if (m_asElements [i] .usIndex2 != pElement ->usIndex2)
                        continue;
                    CData *pData = LoadRes (&m_asElements [i]);
                    if (pData)
                    {
                        char szName [_MAX_PATH];
                        GetResourceText (&m_asElements [i], 
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
            if (m_fModules)
            {
            }
            else
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
// @mfunc Initialize the data 
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CDataSourceDir::Initialize ()
{

    //
    // If already initialized, return
    //

    if (m_fInitialized)
        return;
    m_fInitialized = true;

    //
    // Initiate the find
    //

    CString strDir (g_strNwnDirectory + m_strDir);
    CString strSearch (strDir + m_strMask);
    struct _finddata_t sFind;
    intptr_t id = _findfirst (strSearch, &sFind);
    if (id <= 0)
        return;

    //
    // Loop through the files
    //

    do
    {

        //
        // If system, hidden, or subdir, then skip
        //

        if ((sFind .attrib & (_A_HIDDEN | 
                              _A_SYSTEM | _A_SUBDIR)) != 0)
            continue;

        //
        // Get the extension
        //

        const char *pszExt = strrchr (sFind .name, '.');
        if (pszExt == NULL)
            continue;

        //
        // Get the resource type
        //

        NwnResType nResType = 
            NwnGetResTypeFromExtension (pszExt);

        //
        // If this is a module we should include
        //

        bool fIsModule = 
            nResType == NwnResType_MOD || 
            nResType == NwnResType_NWM ||
            nResType == NwnResType_HAK || 
            nResType == NwnResType_ERF ||
            nResType == NwnResType_SAV;
        if (m_fModules && fIsModule)
        {
            CString str (strDir + sFind .name);
            g_pMainWnd ->SetStatusBarText(_T("Loading ") + str);

            CNwnModuleFile *pModFile = new CNwnModuleFile ();

            if (pModFile ->Open (str) && pModFile ->GetResCount () > 0) {
                CDataSource *pSource = new CDataSourceMod (
                    pModFile, sFind .name);
                m_apSources .Add (pSource);
            } else {
                pModFile ->Close ();
                delete pModFile;
            }
        }

        //
        // Otherwise is it not a module we should include
        //

        else if (!m_fModules && !fIsModule)
        {
            g_pMainWnd ->SetStatusBarText(_T("Indexing ") + strDir + sFind .name);

            DataElement sElement;
            sElement .pDataSource = this;
            sElement .usType = TypeResourceource;
            sElement .usIndex2 = (UINT16) GetResourceClass (nResType, NULL);
            sElement .ulIndex1 = (UINT32) m_astrFileNames .GetCount ();
            m_astrFileNames .Add (sFind .name);
            m_asElements .Add (sElement);
        }
    } while (_findnext (id, &sFind) >= 0);

    //
    // Close the find
    //

    _findclose (id);
    g_pMainWnd ->SetStatusBarText(NULL);

    //
    // If this isn't a module system, the create the res classes
    //

    if (m_astrFileNames .GetCount () > 0)
    {
        bool *pafFlags = (bool *) alloca (sizeof (bool) * ResClass__Last);
        for (int i = 0; i < ResClass__Last; i++)
            pafFlags [i] = false;
        for (int i = 0; i < m_asElements .GetCount (); i++)
        {

            assert (m_asElements [i] .usIndex2 >= 0 &&
                    m_asElements [i] .usIndex2 < ResClass__Last);
            pafFlags [m_asElements [i] .usIndex2] = true;
        }
        for (int i = 0; i < ResClass__Last; i++)
        {
            if (pafFlags [i])
            {
                DataElement sElement;
                sElement .pDataSource = this;
                sElement .usType = TypeResourceClass;
                sElement .usIndex2 = (UINT16) i;
                sElement .ulIndex1 = 0;
                m_asElements .Add (sElement);
            }
        }
    }
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

int CDataSourceDir::ExportTga (DataElement *pElement, 
                               const char *pszFileName)
{
    return 0;
}

/* vim: set sw=4: */
