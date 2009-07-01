//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnStdLoader.cpp - NWN game file loader |
//
// This module contains the definition of the NwnStdLoader.  It provides a
// method of requesting resources from NWN.
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

#include "NwnStdLoader.h"
#include "NwnModuleFile.h"
#include "NwnHierarchy.h"
#ifdef _WIN32
#include <atlbase.h>
#endif

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnStdLoader> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnStdLoader::CNwnStdLoader ()
{
    m_fOverride = true;
    m_pModule = NULL;
    m_searchList = NULL;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnStdLoader> denstructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnStdLoader::~CNwnStdLoader ()
{

    //
    // Delete the loaded hacks
    //

    for (int i = 0; i < (int) m_apHaks .GetCount (); i++)
        delete m_apHaks [i];

    if (m_searchList != NULL) {
        delete m_searchList;
        m_searchList = NULL;
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Initialize the loader
//
// @parm const char * | pszNwnDir | NWN directory.  If NULL, the expected
//              locations will be tested.
//
// @rdesc TRUE if the loaded initialized
//
//-----------------------------------------------------------------------------

bool CNwnStdLoader::Initialize (const char *pszNwnDir)
{
    //
    // If the key file has already been opened, then return
    //

    if (m_asKeyFiles [0] .IsOpen ())
        return true;

    //
    // Get the NWN directory
    //

    if (pszNwnDir == NULL)
        pszNwnDir = GetNwnDirectory ();
    if (pszNwnDir == NULL)
        pszNwnDir = "C:/NeverwinterNights/Nwn/";
    m_strRoot = pszNwnDir;
    if (m_strRoot .empty ())
        return false;

    //
    // Add a '/' if not present
    //

    if (m_strRoot [m_strRoot .size () - 1] != '/' &&
        m_strRoot [m_strRoot .size () - 1] != '\\')
        m_strRoot += "/";

    //
    // Open the key file
    //

    std::string str = m_strRoot + "chitin.key";
    if (!m_asKeyFiles [0] .Open (str .c_str ()))
        return false;

    //
    // Open the patch file
    //

    str = m_strRoot + "patch.key";
    m_asKeyFiles [1] .Open (str .c_str ());

    //
    // Open the xp1 file
    //

    str = m_strRoot + "xp1.key";
    m_asKeyFiles [2] .Open (str .c_str ());

    //
    // Open the xp1 patch file
    //

    str = m_strRoot + "xp1patch.key";
    m_asKeyFiles [3] .Open (str .c_str ());

    //
    // Open the xp2 file
    //

    str = m_strRoot + "xp2.key";
    m_asKeyFiles [4] .Open (str .c_str ());

    //
    // Open the xp2 patch file
    //

    str = m_strRoot + "xp2patch.key";
    m_asKeyFiles [5] .Open (str .c_str ());

    //
    // added by fluffyamoeba 2008-03-04
    // Open the xp3 file (1.69 data)
    //

    str = m_strRoot + "xp3.key";
    m_asKeyFiles [6] .Open (str .c_str ());

    //
    // Create the other directories
    //
    // Currently a HACK
    //

    m_strOverride = m_strRoot + "override/";
    m_strModule = m_strRoot + "modules/";
    m_strHak = m_strRoot + "hak/";

    // Cleanup the search path
    if (m_searchList != NULL) {
        delete m_searchList;
        m_searchList = NULL;
    }

    return true;
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the NWN directory from the registry
//
// @rdesc Pointer to the directory
//
//-----------------------------------------------------------------------------

const char *CNwnStdLoader::GetNwnDirectory ()
{
#ifdef _WIN32
    //
    // Open the NWN registry key
    //

    CRegKey rkParent;
    LONG lRet = rkParent .Open (HKEY_LOCAL_MACHINE, 
                                _T ("SOFTWARE\\BioWare\\NWN\\Neverwinter"),
                                KEY_READ);
    if (lRet != ERROR_SUCCESS)
        return NULL;

    //
    // Read the key
    //

    static char szText [_MAX_PATH];
    DWORD dwLen = _MAX_PATH;
    lRet = rkParent .QueryStringValue (_T ("Location"), szText, &dwLen);
    if (lRet != ERROR_SUCCESS)
        return NULL;
    strcat (szText, "\\");
    return szText;
#else
    return "";
#endif
}

//-----------------------------------------------------------------------------
//
// @mfunc Load the resource
//
// @parm const char * | pszName | Name of the resource to load
//
// @parm NwnResType | nResType | Type of the resource
//
// @parm UINT32 * | pulSize | If loaded, populated with the resource size
//
// @parm bool * | pfAllocated | Pointer to a flag to received 
//              if the data is allocated or not.
//
// @rdesc Address of the allocated buffer.  Use free to free.
//
//-----------------------------------------------------------------------------

unsigned char *CNwnStdLoader::LoadResource (const char *pszName, 
                                            NwnResType nResType, UINT32 *pulSize, bool *pfAllocated)
{
    unsigned char *pauchData;

    //
    // Try the module
    //

    if (m_pModule)
    {
        pauchData = m_pModule ->LoadRes (pszName, 
                                         nResType, pulSize, pfAllocated);
        if (pauchData)
            return pauchData;
    }
    //
    // Try from the default dir
    //

    if (m_strDefaultDir .length () > 0)
    {
        std::string str (m_strDefaultDir);
        str += pszName;
        str += NwnGetResTypeExtension (nResType);
        pauchData = NwnLoadFile (str .c_str (), pulSize);
        if (pauchData != NULL)
        {
            if (pfAllocated)
                *pfAllocated = true;
            return pauchData;
        }
    }

    //
    // Try from the search list
    //
    if (m_searchList != NULL) {
        DirSearchListConstIter it;
        for (it = m_searchList->begin(); it!=m_searchList->end(); ++it) {
            std::string str (*it);
            str += pszName;
            str += NwnGetResTypeExtension (nResType);
            pauchData = NwnLoadFile (str .c_str (), pulSize);
            if (pauchData != NULL)
            {
                if (pfAllocated)
                    *pfAllocated = true;
                return pauchData;
            }
        }
    }

    //
    // Try to load the file from the current dir
    //

    std::string str (pszName);
    str += NwnGetResTypeExtension (nResType);
    pauchData = NwnLoadFile (str .c_str (), pulSize);
    if (pauchData != NULL)
    {
        if (pfAllocated)
            *pfAllocated = true;
        return pauchData;
    }

    //
    // Try to load the file from the haks
    //

    for (int i = 0; i < (int) m_apHaks .GetCount (); i++)
    {
        pauchData = m_apHaks [i] ->LoadRes (pszName, 
                                            nResType, pulSize, pfAllocated);
        if (pauchData)
            return pauchData;
    }

    //
    // If override is enabled, try it
    //

    if (m_fOverride)
    {
        str = m_strOverride;
        str += pszName;
        str += NwnGetResTypeExtension (nResType);
        unsigned char *pauchData = NwnLoadFile (str .c_str (), pulSize);
        if (pauchData != NULL)
        {
            if (pfAllocated)
                *pfAllocated = true;
            return pauchData;
        }
    }

    // 
    // Try the key files
    //

    for (int i = _countof (m_asKeyFiles); i-- > 0;)
    {
        if (m_asKeyFiles [i] .IsOpen ())
        {
            pauchData = m_asKeyFiles [i] .LoadRes (pszName, 
                                                   nResType, pulSize, pfAllocated);
            if (pauchData != NULL)
                return pauchData;
        }
    }
    return NULL;
}

//-----------------------------------------------------------------------------
//
// @mfunc Open a module
//
// @parm CNwnModuleFile * | pModule | Module structure to open
//
// @parm const char * | pszName | Name of the module
//
// @rdesc TRUE if the module was opened
//
//-----------------------------------------------------------------------------

bool CNwnStdLoader::OpenModule (CNwnModuleFile *pModule, const char *pszName)
{

    //
    // Try simple
    //

    std::string str (pszName);
    if (pModule ->Open (str .c_str ()))
        return true;

    //
    // Try with an extension
    //

    str += ".mod";
    if (pModule ->Open (str .c_str ()))
        return true;

    //
    // Try the module directory
    //

    str = m_strModule;
    str += pszName;
    if (pModule ->Open (str .c_str ()))
        return true;

    //
    // Try with an extension
    //

    str += ".mod";
    if (pModule ->Open (str .c_str ()))
        return true;

    //
    // Give up
    //

    return false;
}

//-----------------------------------------------------------------------------
//
// @mfunc Open module hak file
//
// @rdesc TRUE if any hak file was added
//
//-----------------------------------------------------------------------------

bool CNwnStdLoader::AddModuleHaks ()
{
    static const char *pszName = "module";
    static const NwnResType nResType = NwnResType_IFO;
    unsigned char *pauchData = NULL;
    UINT32 ulSize = 0;
    bool fAllocated = false;

    //
    // Try the module
    //

    if (m_pModule)
    {
        pauchData = m_pModule ->LoadRes (pszName, 
                                         nResType, &ulSize, &fAllocated);
    }

    //
    // Next, try to load the file from the current dir
    //

    if (pauchData == NULL)
    {
        std::string str = pszName;
        str += NwnGetResTypeExtension (nResType);
        pauchData = NwnLoadFile (str .c_str (), &ulSize);
        fAllocated = true;
    }

    //
    // If we can't find the module file, then return
    //

    if (pauchData == NULL)
        return false;

    //
    // Locate the hak file name
    //

    CNwnHierarchy ifo (pauchData, ulSize, fAllocated);
    return LookForHak (ifo, 0);
}

//-----------------------------------------------------------------------------
//
// @mfunc Look for a hak file setting
//
// @parm CNwnHierarchy & | ifo | Module information file
//
// @parm int | nEntry | Entry to search
//
// @rdesc TRUE if any hak file was added
//
//-----------------------------------------------------------------------------

bool CNwnStdLoader::LookForHak (CNwnHierarchy &ifo, int nEntry)
{
    //
    // Locate the hak file name
    //

    bool fGotOne = false;
    CNwnHierarchy::Entry *pEntry = ifo .GetEntry (nEntry);
    UINT32 *plMap = ifo .GetMultiMapTable (pEntry);
    for (int j = 0; j < (int) pEntry ->ulCount; j++)
    {
        int nElement = plMap [j];
        const CNwnHierarchy::Element *pElement = ifo .GetElement (nElement);
        if (pElement ->ulType == CNwnHierarchy::ElementType_STRING)
        {
            CNwnHierarchy::VarName *pVarName = 
                ifo .GetVarName (pElement ->ulVarName);
            if (stricmp (pVarName ->szName, "Mod_Hak") == 0)
            {
                char szHak [256];
                ifo .GetElementLongString (
                    pElement, szHak, _countof (szHak));
                if (AddHak (szHak))
                    fGotOne = true;
                break;
            }
        }
        else if (pElement ->ulType == CNwnHierarchy::ElementType_LIST)
        {
            CNwnHierarchy::VarName *pVarName = 
                ifo .GetVarName (pElement ->ulVarName);
            if (stricmp (pVarName ->szName, "Mod_HakList") == 0)
            {
                int nTable = pElement ->u32Data;
                nTable /= sizeof (UINT32);
                int nCount = ifo .GetList (nTable++);
                for (int i = 0; i < nCount; i++)
                {
                    fGotOne |= LookForHak (ifo, (int) ifo .GetList (nTable++));
                }
            }
        }
    }
    return fGotOne;
}

//-----------------------------------------------------------------------------
//
// @mfunc Add the given hak file to the list
//
// @parm const char * | pszHak | Name of the hak file (no directory or ext)
//
// @rdesc TRUE if hak file was added
//
//-----------------------------------------------------------------------------

bool CNwnStdLoader::AddHak (const char *pszHak)
{

    //
    // Create a new module
    //

    CNwnModuleFile *pHak = new CNwnModuleFile;

    //
    // Next, try to load the file from the current dir
    //

    std::string str (m_strHak);
    str += pszHak;
    str += ".hak";
    if (!pHak ->Open (str .c_str ()))
    {
        delete pHak;
        return false;
    }

    //
    // Add the hak to the list
    //

    m_apHaks .Add (pHak);
    return true;
}


/* vim: set sw=4: */
