//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      DataCache.cpp - Resource cache |
//
// This module contains the definition of the resource cache.
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
// $History: DataCache.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "DataCache.h"
#include "Data.h"
#include "nwnexplorer.h"
#include "DataRef.h"
#include "MainWnd.h"
#include <gl/gl.h>
#include <gl/glu.h>

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

extern CString g_strNwnDirectory;
extern BOOL g_fViewModelsPreferDds;
extern BOOL g_fViewModelsPreventDds;
extern BOOL g_fViewModelsPreventTga;
extern DWORD g_nViewModelsAlphaBehavior;
extern CMainWnd *g_pMainWnd;
extern D3DCOLOR *g_apPalettes [NwnPalette__Count];

//-----------------------------------------------------------------------------
//
// @mfunc <c CDataCache> constructor.
//
// @parm CNwnKeyFile * | pKeyFiles | Key file
//
// @parm CNwnModuleFile * | pHakFiles | Hak files
//
// @parm CNwnDialogTlkFile * | pDlgTlkFile | Dialog talk file
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CDataCache::CDataCache (CNwnKeyFile *pKeyFiles, CNwnModuleFile *pHakFiles,
                        CNwnDialogTlkFile *pDlgTlkFile)
{
    m_pKeyFiles   = pKeyFiles;
    m_pHakFiles   = pHakFiles;
    m_pDlgTlkFile = pDlgTlkFile;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CDataCache> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CDataCache::~CDataCache ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc Load a resource based on a key file res structure
//
// @parm CNwnKeyFile * | psKeyFile | Key file in question
//
// @parm const CNwnKeyFile::Resource * | psRes | Resource from the key file
//
// @rdesc Pointer to the resource or NULL if there was an error.
//
//-----------------------------------------------------------------------------

CData *CDataCache::LoadRes (CNwnKeyFile *psKeyFile, 
                            const CNwnKeyFile::Resource *psRes)
{
    CData *psResource;

    //
    // Get the file to load file
    //

    if (psKeyFile == NULL || !psKeyFile ->IsOpen ())
        return NULL;

    //
    // Search for the resource
    //

    char szName [17];
    memmove (szName, psRes ->szName, 16);
    szName [16] = 0;
    CDataKey dk (psKeyFile, NULL, NULL, szName, 
                 (NwnResType) psRes ->usType, CDataKey::Type_Raw);
    if (m_mapData .Lookup (&dk, psResource))
    {
        //psResource ->AddRef ();
        return psResource;
    }

    //
    // If we found the resource, try to load it
    //

    bool fAllocated;
    UINT32 ulSize;
    unsigned char *pauchData = psKeyFile ->LoadRes (
        psRes, &ulSize, &fAllocated);
    if (pauchData == NULL)
        return NULL;

    //
    // Create a new data object
    //

    psResource = new CData (this, psKeyFile, NULL, NULL, szName, 
                            (NwnResType) psRes ->usType, CDataKey::Type_Raw, 
                            NULL, 0, pauchData, ulSize, fAllocated);

    //
    // If created, add to the map.  Otherwise, delete
    //

    if (psResource == NULL)
    {
        if (fAllocated)
            free (pauchData);
    }
    else
    {
        m_mapData .SetAt (psResource, psResource);
    }
    return psResource;
}

//-----------------------------------------------------------------------------
//
// @mfunc Load a resource based on a key file res structure
//
// @parm CNwnKeyFile * | psKeyFile | Key file in question
//
// @parm const char * | pszName | Name of the resource
//
// @parm NwnResType | nResType | Resource type
//
// @rdesc Pointer to the resource or NULL if there was an error.
//
//-----------------------------------------------------------------------------

CData *CDataCache::LoadRes (CNwnKeyFile *psKeyFile, 
                            const char *pszName, NwnResType nResType)
{
    CData *psResource;

    //
    // Get the file to load file
    //

    if (psKeyFile == NULL || !psKeyFile ->IsOpen ())
        return NULL;

    //
    // Search for the resource
    //

    CDataKey dk (psKeyFile, NULL, NULL, pszName, 
                 nResType, CDataKey::Type_Raw);
    if (m_mapData .Lookup (&dk, psResource))
    {
        //psResource ->AddRef ();
        return psResource;
    }

    //
    // Try to find the resource
    //

    const CNwnKeyFile::Resource *psRes = 
        psKeyFile ->FindRes (pszName, nResType);
    if (psRes == NULL)
        return NULL;

    //
    // If we found the resource, try to load it
    //

    bool fAllocated;
    UINT32 ulSize;
    unsigned char *pauchData = psKeyFile ->LoadRes (
        psRes, &ulSize, &fAllocated);
    if (pauchData == NULL)
        return NULL;

    //
    // Create a new data object
    //

    psResource = new CData (this, psKeyFile, NULL, NULL, psRes ->szName, 
                            (NwnResType) psRes ->usType, CDataKey::Type_Raw, 
                            NULL, 0, pauchData, ulSize, fAllocated);

    //
    // If created, add to the map.  Otherwise, delete
    //

    if (psResource == NULL)
    {
        if (fAllocated)
            free (pauchData);
    }
    else
    {
        m_mapData .SetAt (psResource, psResource);
    }
    return psResource;
}

//-----------------------------------------------------------------------------
//
// @mfunc Load a resource based on a module file res structure
//
// @parm CNwnModuleFile * | psModule | Pointer to the source module
//
// @parm const CNwnModuleFile::Resource * | psRes | Resource from the module file
//
// @rdesc Pointer to the resource or NULL if there was an error.
//
//-----------------------------------------------------------------------------

CData *CDataCache::LoadRes (CNwnModuleFile *psModule, 
                            const CNwnModuleFile::Resource *psRes)
{
    CData *psResource;

    //
    // If the module is invalid, exit
    //

    if (psModule == NULL || !psModule ->IsOpen ())
        return NULL;

    //
    // Search for the resource
    //

    char szName [17];
    memmove (szName, psRes ->szName, 16);
    szName [16] = 0;
    CDataKey dk (NULL, psModule, NULL, szName, 
                 (NwnResType) psRes ->ulType, CDataKey::Type_Raw);
    if (m_mapData .Lookup (&dk, psResource))
    {
        //psResource ->AddRef ();
        return psResource;
    }

    //
    // Load the data from the module
    //

    bool fAllocated;
    UINT32 ulSize;
    unsigned char *pauchData = psModule ->
        LoadRes (psRes, &ulSize, &fAllocated);
    if (pauchData == NULL)
        return NULL;

    //
    // Create a new data object
    //

    psResource = new CData (this, NULL, psModule, NULL, szName, 
                            (NwnResType) psRes ->ulType, CDataKey::Type_Raw, 
                            NULL, 0, pauchData, ulSize, fAllocated);

    //
    // If created, add to the map.  Otherwise, delete
    //

    if (psResource == NULL)
    {
        if (fAllocated)
            free (pauchData);
    }
    else
    {
        m_mapData .SetAt (psResource, psResource);
    }
    return psResource;
}

//-----------------------------------------------------------------------------
//
// @mfunc Load a resource based on a module file res structure
//
// @parm CNwnModuleFile * | psModule | Pointer to the source module
//
// @parm const char * | pszName | Name of the resource
//
// @parm NwnResType | nResType | Resource type
//
// @rdesc Pointer to the resource or NULL if there was an error.
//
//-----------------------------------------------------------------------------

CData *CDataCache::LoadRes (CNwnModuleFile *psModule, 
                            const char *pszName, NwnResType nResType)
{
    CData *psResource;

    //
    // If the module is invalid, exit
    //

    if (psModule == NULL || !psModule ->IsOpen ())
        return NULL;

    //
    // Search for the resource
    //

    CDataKey dk (NULL, psModule, NULL, pszName, 
                 nResType, CDataKey::Type_Raw);
    if (m_mapData .Lookup (&dk, psResource))
    {
        //psResource ->AddRef ();
        return psResource;
    }

    //
    // Try to find the resource
    //

    const CNwnModuleFile::Resource *psRes = 
        psModule ->FindRes (pszName, nResType);
    if (psRes == NULL)
        return NULL;

    //
    // Load the data from the module
    //

    bool fAllocated;
    UINT32 ulSize;
    unsigned char *pauchData = psModule ->
        LoadRes (psRes, &ulSize, &fAllocated);
    if (pauchData == NULL)
        return NULL;

    //
    // Create a new data object
    //

    psResource = new CData (this, NULL, psModule, NULL, psRes ->szName, 
                            (NwnResType) psRes ->ulType, CDataKey::Type_Raw, 
                            NULL, 0, pauchData, ulSize, fAllocated);

    //
    // If created, add to the map.  Otherwise, delete
    //

    if (psResource == NULL)
    {
        if (fAllocated)
            free (pauchData);
    }
    else
    {
        m_mapData .SetAt (psResource, psResource);
    }
    return psResource;
}

//-----------------------------------------------------------------------------
//
// @mfunc Load a resource based on a bif file res structure
//
// @parm CNwnBifFile * | psBifFile | Pointer to the source bif
//
// @parm const CNwnBifFile::Resource * | psRes | Resource from the bif file
//
// @rdesc Pointer to the resource or NULL if there was an error.
//
//-----------------------------------------------------------------------------

CData *CDataCache::LoadRes (CNwnBifFile *psBifFile, 
                            const CNwnBifFile::Resource *psRes)
{
    CData *psResource;

    //
    // If the module is invalid, exit
    //

    if (psBifFile == NULL || !psBifFile ->IsOpen ())
        return NULL;

    //
    // Search for the resource
    //

    char szName [17];
    sprintf (szName, "res%d", psRes ->ulID);
    CDataKey dk (NULL, NULL, psBifFile, szName, 
                 (NwnResType) psRes ->ulType, CDataKey::Type_Raw);
    if (m_mapData .Lookup (&dk, psResource))
    {
        //psResource ->AddRef ();
        return psResource;
    }

    //
    // Load the data from the module
    //

    bool fAllocated;
    UINT32 ulSize;
    unsigned char *pauchData = psBifFile ->
        LoadRes (psRes ->ulID, &ulSize, &fAllocated);
    if (pauchData == NULL)
        return NULL;

    //
    // Create a new data object
    //

    psResource = new CData (this, NULL, NULL, psBifFile, szName, 
                            (NwnResType) psRes ->ulType, CDataKey::Type_Raw, 
                            NULL, 0, pauchData, ulSize, fAllocated);

    //
    // If created, add to the map.  Otherwise, delete
    //

    if (psResource == NULL)
    {
        if (fAllocated)
            free (pauchData);
    }
    else
    {
        m_mapData .SetAt (psResource, psResource);
    }
    return psResource;
}

//-----------------------------------------------------------------------------
//
// @mfunc Load a resource based on a file name
//
// @parm const char * | pszFileName | File to be loaded
//
// @rdesc Pointer to the resource or NULL if there was an error.
//
//-----------------------------------------------------------------------------

CData *CDataCache::LoadRes (const char *pszFileName)
{
    CData *psResource;

    //
    // Get the resource type
    //

    const char *pszExt = strrchr (pszFileName, '.');
    NwnResType nResType;
    if (pszExt)
        nResType = NwnGetResTypeFromExtension (pszExt);
    else
        nResType = NwnResType_Unknown;

    //
    // Search for the resource
    //

    CDataKey dk (NULL, NULL, NULL, pszFileName, 
                 nResType, CDataKey::Type_Raw);
    if (m_mapData .Lookup (&dk, psResource))
    {
        //psResource ->AddRef ();
        return psResource;
    }

    //
    // Load the resource
    //

    UINT32 ulSize;
    unsigned char *pauchData = NwnLoadFile (pszFileName, &ulSize);
    if (pauchData == NULL)
        return NULL;

    //
    // Create a new data object
    //

    psResource = new CData (this, NULL, NULL, NULL, pszFileName, 
                            nResType, CDataKey::Type_Raw, NULL, 0, pauchData, 
                            ulSize, true);

    //
    // If created, add to the map.  Otherwise, delete
    //

    if (psResource == NULL)
    {
        free (pauchData);
    }
    else
    {
        m_mapData .SetAt (psResource, psResource);
    }
    return psResource;
}

//-----------------------------------------------------------------------------
//
// @mfunc Load a resource 
//
// @parm CNwnModuleFile * | psModule | Pointer to the source module
//
// @parm const char * | pszSourceFileName | If specified, the resource
//              will try to be located in this directory.
//
// @parm const char * | pszName | Name of the resource
//
// @parm NwnResType | nResType | Resource type
//
// @rdesc Pointer to the resource or NULL if there was an error.
//
//-----------------------------------------------------------------------------

CData *CDataCache::LoadOrderedRes (CNwnModuleFile *psModule, 
                                   const char *pszSourceFileName, const char *pszName, 
                                   NwnResType nResType)
{
    CData *psResource;

    //
    // First, try the model
    //

    psResource = LoadRes (psModule, pszName, nResType);
    if (psResource != NULL)
        return psResource;

    //
    // If we have a source file name
    //

    if (pszSourceFileName)
    {

        //
        // Split the input file name up
        //

        char szDrive [_MAX_DRIVE];
        char szDir [_MAX_DIR];
        char szFileName [_MAX_FNAME];
        char szExtension [_MAX_EXT];
        _splitpath (pszSourceFileName, szDrive, 
                    szDir, szFileName, szExtension);

        //
        // If we have a drive or directory
        //

        if (szDrive [0] != 0 || szDir [0] != 0)
        {
            CString str (szDrive);
            str += szDir;
            str += pszName;
            str += NwnGetResTypeExtension (nResType);
            psResource = LoadRes (str);
            if (psResource != NULL)
                return psResource;
        }
    }

    //
    // Next, try the hak files
    //

    for (int i = 0; i < IDS_TOTAL_USER_HAKS; i++) {
        psResource = LoadRes (&(m_pHakFiles[i]), pszName, nResType);
        if (psResource != NULL)
            return psResource;
    }

    //
    // Next, try the override directory
    //

    CString str (g_strNwnDirectory + "override\\" + 
                 pszName + NwnGetResTypeExtension (nResType));
    psResource = LoadRes (str);
    if (psResource != NULL)
        return psResource;

    //
    // Next, try the key files
    //

    for (int i = 0; i < IDS_TOTAL_KEY_FILES; i++) {
        psResource = LoadRes (&(m_pKeyFiles[i]), pszName, nResType);
        if (psResource != NULL)
            return psResource;
    }

    //
    // Well, at this point all is lost.  Time to just give up and die
    //

    return NULL;
}

//-----------------------------------------------------------------------------
//
// @mfunc Load a texture
//
// @parm CNwnModuleFile * | psModule | Pointer to the source module
//
// @parm const char * | pszModelFile | Pointer file source
//
// @parm const char * | pszName | Name of the resource
//
// @parm int * | panPalette | Pointer to the PLT texture palette information
//
// @rdesc Pointer to the resource or NULL if there was an error.
//
//-----------------------------------------------------------------------------

CData *CDataCache::LoadTexture (CNwnModuleFile *psModule, 
                                const char *pszModelFile, const char *pszName, int *panPalette)
{
    CData *psResource = NULL;

    //
    // Try a PLT texture
    //

    psResource = LoadPLTTexture (psModule, pszModelFile, pszName, panPalette);

    //
    // Try a TGA or DDS texture
    //

    if (psResource == NULL)
        psResource = LoadTGAorDDSTexture (psModule, pszModelFile, pszName,
                                          g_fViewModelsPreferDds);

    if (psResource == NULL)
        psResource = LoadTGAorDDSTexture (psModule, pszModelFile, pszName,
                                          !g_fViewModelsPreferDds);

    //
    // Return error
    //

    g_pMainWnd ->SetStatusBarText(NULL);

    return psResource;
}

//-----------------------------------------------------------------------------
//
// @mfunc Load a PLT texture
//
// @parm CNwnModuleFile * | psModule | Pointer to the source module
//
// @parm const char * | pszModelFile | Pointer file source
//
// @parm const char * | pszName | Name of the resource
//
// @parm int * | panPalette | Pointer to the PLT texture palette information
//
// @rdesc Pointer to the resource or NULL if there was an error.
//
//-----------------------------------------------------------------------------

CData *CDataCache::LoadPLTTexture (CNwnModuleFile *psModule, 
                                   const char *pszModelFile, const char *pszName, int *panPalette)
{
    CData *psResource;
    CString strName (pszName);

    //
    // Try the cache first
    //

    CDataKey dk (NULL, NULL, NULL, pszName, NwnResType_PLT, 
                 CDataKey::Type_OpenGLTexture, (unsigned char *) panPalette,
                 sizeof (int) * NwnPalette__Count);
    if (m_mapData .Lookup (&dk, psResource))
    {
        //psResource ->AddRef ();
        return psResource;
    }

    //
    // If not found, try to load the raw data
    //

    CData *psRaw = LoadOrderedRes (psModule, pszModelFile, 
                                   pszName, NwnResType_PLT);
    if (psRaw == NULL)
        return NULL;

    g_pMainWnd ->SetStatusBarText(_T("Loading texture ") + strName + _T(".plt"));

    //
    // Attach the data to an object so I can manage the data
    //

    CResTexturePltRef plt;
    plt .Set (psRaw);

    //
    // Get the size and allocate a buffer for the data
    //

    int nWidth = plt .GetWidth ();
    int nHeight = plt .GetHeight ();
    int nCount = nWidth * nHeight;
    unsigned char *pauchData = new unsigned char [nWidth * nHeight * 4];
    unsigned char *pauchOut = pauchData;

    //
    // Convert the PLT into an OpenGL texture
    //

    const CResTexturePltRef::Pixel *p = plt .GetPixelData ();
    for (int i = 0; i < nCount; i++)
    {
        D3DCOLOR *pData = g_apPalettes [p [i] .ucPalette];
        if (pData == NULL)
        {
            *pauchOut++ = 0;
            *pauchOut++ = 0;
            *pauchOut++ = 0;
            *pauchOut++ = 0;
        }
        else
        {
            pData += (255 - panPalette [p [i] .ucPalette]) * 256;
            D3DCOLOR clr = pData [p [i] .ucIndex];
            *pauchOut++ = RGBA_GETBLUE (clr);
            *pauchOut++ = RGBA_GETGREEN (clr);
            *pauchOut++ = RGBA_GETRED (clr);
            *pauchOut++ = RGBA_GETALPHA (clr);
        }
    }

    //
    // Create the texture
    //

    GLuint unTextureName;
    glGetError ();
    glGenTextures (1, &unTextureName);
    glBindTexture (GL_TEXTURE_2D, unTextureName);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, nWidth, nHeight, 0, 
                  GL_RGBA, GL_UNSIGNED_BYTE, pauchData);

    //
    // Create the mipmaps.  The GLU routine doesn't work for textures where
    // the width is greater than then height.  However, it does work for textures
    // where the height is greater than the width.  *shrug*
    //

#ifdef GLU_MIPMAP_DOESNT_WORK
    gluBuild2DMipmaps (GL_TEXTURE_2D, GL_RGBA, nWidth, nWidth, 
                       GL_RGBA, GL_UNSIGNED_BYTE, pauchData);
#else
    int nLevel = 1;
    while (nWidth > 1 || nHeight > 1)
    {
        MipMapInPlace32 (pauchData, nWidth, nHeight);
        nWidth >>= 1;
        if (nWidth == 0)
            nWidth = 1;
        nHeight >>= 1;
        if (nHeight == 0)
            nHeight = 1;
        glTexImage2D (GL_TEXTURE_2D, nLevel, GL_RGBA, nWidth, nHeight, 0, 
                      GL_RGBA, GL_UNSIGNED_BYTE, pauchData);
        nLevel++;
    }
    delete [] pauchData;
    if (glGetError () != GL_NO_ERROR)
    {
        glDeleteTextures (1, &unTextureName);
        return NULL;
    }
#endif

    //
    // Create a new data object
    //

    psResource = new CData (this, NULL, NULL, NULL, pszName, 
                            NwnResType_PLT, CDataKey::Type_OpenGLTexture, 
                            (unsigned char *) panPalette, sizeof (int) * NwnPalette__Count, 
                            (unsigned char *) &unTextureName, sizeof (unTextureName), 
                            false);

    //
    // If created, add to the map.  Otherwise, delete
    //

    if (psResource == NULL)
    {
        glDeleteTextures (1, &unTextureName);
    }
    else
    {
        m_mapData .SetAt (psResource, psResource);
    }
    return psResource;
}

//-----------------------------------------------------------------------------
//
// @mfunc Load a TGA or DDS texture
//
// @parm CNwnModuleFile * | psModule | Pointer to the source module
//
// @parm const char * | pszModelFile | Pointer file source
//
// @parm const char * | pszName | Name of the resource
//
// @rdesc Pointer to the resource or NULL if there was an error.
//
//-----------------------------------------------------------------------------

CData *CDataCache::LoadTGAorDDSTexture (CNwnModuleFile *psModule, 
                                        const char *pszModelFile,
                                        const char *pszName,
                                        BOOL bLoadDds)
{
    CData *psResource;
    CString strName (pszName);
    NwnResType restype;

    if (bLoadDds) {
        if (g_fViewModelsPreventDds)
            return NULL;

        restype = NwnResType_DDS;
    } else {
        if (g_fViewModelsPreventTga)
            return NULL;

        restype = NwnResType_TGA;
    }

    //
    // Try the cache first
    //

    CDataKey dk (NULL, NULL, NULL, pszName, restype, 
                 CDataKey::Type_OpenGLTexture, NULL, -1);
    if (m_mapData .Lookup (&dk, psResource))
    {
        //psResource ->AddRef ();
        return psResource;
    }

    //
    // If not found, try to load the raw data
    //

    int nWidth, nHeight;
    D3DCOLOR *pData = NULL;

    //
    // Try loading the texture
    //

    CData *psRaw = LoadOrderedRes (psModule, pszModelFile, 
                                   pszName, restype);

    if (psRaw != NULL) {
        if (bLoadDds)
            pData = DdsLoad (psRaw ->GetData (0), &nWidth, &nHeight);
        else
            pData = TgaLoad (psRaw ->GetData (0), &nWidth, &nHeight);

        psRaw ->AddRef ();
        psRaw ->Release ();
        g_pMainWnd ->SetStatusBarText(_T("Loading texture ") + strName + (bLoadDds ? _T(".dds") : _T(".tga")));
    }

    if (pData == NULL)
        return NULL;

    //
    // Convert the data
    //

    unsigned char *pauchData = new unsigned char [nWidth * nHeight * 4];
    unsigned char *pauchOut = pauchData;

    if (g_nViewModelsAlphaBehavior == 2) {
        for (int i = 0; i < nWidth * nHeight; i++)
        {
            *pauchOut++ = RGBA_GETBLUE (pData [i]);
            *pauchOut++ = RGBA_GETGREEN (pData [i]);
            *pauchOut++ = RGBA_GETRED (pData [i]);
            *pauchOut++ = 255;
        }
    } else if (g_nViewModelsAlphaBehavior == 1) {
        for (int i = 0; i < nWidth * nHeight; i++)
        {
            *pauchOut++ = RGBA_GETBLUE (pData [i]);
            *pauchOut++ = RGBA_GETGREEN (pData [i]);
            *pauchOut++ = RGBA_GETRED (pData [i]);
            *pauchOut++ = (RGBA_GETALPHA (pData [i]) == 255 ? 255 : 0);
        }
    } else {
        for (int i = 0; i < nWidth * nHeight; i++)
        {
            *pauchOut++ = RGBA_GETBLUE (pData [i]);
            *pauchOut++ = RGBA_GETGREEN (pData [i]);
            *pauchOut++ = RGBA_GETRED (pData [i]);
            *pauchOut++ = RGBA_GETALPHA (pData [i]);
        }
    }
    delete pData;

    //
    // Create the texture
    //

    GLuint unTextureName;
    glGetError ();
    glGenTextures (1, &unTextureName);
    glBindTexture (GL_TEXTURE_2D, unTextureName);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, nWidth, nHeight, 0, 
                  GL_RGBA, GL_UNSIGNED_BYTE, pauchData);

    //
    // Create the mipmaps.  The GLU routine doesn't work for textures where
    // the width is greater than then height.  However, it does work for textures
    // where the height is greater than the width.  *shrug*
    //

#ifdef GLU_MIPMAP_DOESNT_WORK
    gluBuild2DMipmaps (GL_TEXTURE_2D, GL_RGBA, nWidth, nWidth, 
                       GL_RGBA, GL_UNSIGNED_BYTE, pauchData);
#else
    int nLevel = 1;
    while (nWidth > 1 || nHeight > 1)
    {
        MipMapInPlace32 (pauchData, nWidth, nHeight);
        nWidth >>= 1;
        if (nWidth == 0)
            nWidth = 1;
        nHeight >>= 1;
        if (nHeight == 0)
            nHeight = 1;
        glTexImage2D (GL_TEXTURE_2D, nLevel, GL_RGBA, nWidth, nHeight, 0, 
                      GL_RGBA, GL_UNSIGNED_BYTE, pauchData);
        nLevel++;
    }
    delete [] pauchData;
    if (glGetError () != GL_NO_ERROR)
    {
        glDeleteTextures (1, &unTextureName);
        return NULL;
    }
#endif

    //
    // Create a new data object
    //

    psResource = new CData (this, NULL, NULL, NULL, pszName, 
                            restype, CDataKey::Type_OpenGLTexture, NULL, 0, 
                            (unsigned char *) &unTextureName, sizeof (unTextureName), 
                            false);

    //
    // If created, add to the map.  Otherwise, delete
    //

    if (psResource == NULL)
    {
        glDeleteTextures (1, &unTextureName);
    }
    else
    {
        m_mapData .SetAt (psResource, psResource);
    }
    return psResource;
}

//-----------------------------------------------------------------------------
//
// @mfunc Notification from a data object that is has been released
//
// @parm CData * | pData | Data being released
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CDataCache::DataReleased (CData *pData)
{

    //
    // If the data is a texture, delete it
    //

    if (pData ->m_nType == CDataKey::Type_OpenGLTexture)
    {
        GLuint *pname = (GLuint *) pData ->GetData (0);
        glDeleteTextures (1, pname);
    }

    //
    // Remvoe the key
    //

    m_mapData .RemoveKey (pData);
}

//-----------------------------------------------------------------------------
//
// @mfunc Mipmap a 32 bit texture in place
//
// @parm unsigned char * | pIn | Pointer to the data
//
// @parm int | nWidth | Width of the texture
//
// @parm int | nHeight | Height of the texture
// 
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CDataCache::MipMapInPlace32 (unsigned char *pIn, int nWidth, int nHeight)
{
    //
    // Compute the working width and height.  
    //
    //  The width is not adjusted for the mipmap size, but is 
    //  adjusted for the size of the data
    //
    //  The height is adjusted for the mipmap size
    //

    nWidth  *= 4;
    nHeight /= 2;

    //
    // Using input buffer as output also
    //

    unsigned char *pOut = pIn;

    //
    // Loop through the buffer
    //

    for (int i = 0; i < nHeight; i++)
    {
        for (int j = 0; j < nWidth; j += 8, pIn += 8, pOut += 4)
        {
            unsigned char *pIn2 = pIn + nWidth;
            pOut [0] = unsigned char ((pIn [0] + pIn [4] + pIn2 [0] + pIn2 [4]) >> 2);
            pOut [1] = unsigned char ((pIn [1] + pIn [5] + pIn2 [1] + pIn2 [5]) >> 2);
            pOut [2] = unsigned char ((pIn [2] + pIn [6] + pIn2 [2] + pIn2 [6]) >> 2);
            pOut [3] = unsigned char ((pIn [3] + pIn [7] + pIn2 [3] + pIn2 [7]) >> 2);
        }
        pIn += nWidth;
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Mipmap a 24 bit texture in place
//
// @parm unsigned char * | pIn | Pointer to the data
//
// @parm int | nWidth | Width of the texture
//
// @parm int | nHeight | Height of the texture
// 
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CDataCache::MipMapInPlace24 (unsigned char *pIn, int nWidth, int nHeight)
{
    //
    // Compute the working width and height.  
    //
    //  The width is not adjusted for the mipmap size, but is 
    //  adjusted for the size of the data
    //
    //  The height is adjusted for the mipmap size
    //

    nWidth  *= 3;
    nHeight /= 2;

    //
    // Using input buffer as output also
    //

    unsigned char *pOut = pIn;

    //
    // Loop through the buffer
    //

    for (int i = 0; i < nHeight; i++)
    {
        for (int j = 0; j < nWidth; j += 6, pIn += 6, pOut += 3)
        {
            unsigned char *pIn2 = pIn + nWidth;
            pOut [0] = unsigned char ((pIn [0] + pIn [3] + pIn2 [0] + pIn2 [3]) >> 2);
            pOut [1] = unsigned char ((pIn [1] + pIn [4] + pIn2 [1] + pIn2 [4]) >> 2);
            pOut [2] = unsigned char ((pIn [2] + pIn [5] + pIn2 [2] + pIn2 [5]) >> 2);
        }
        pIn += nWidth;
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Mipmap a 8 bit texture in place
//
// @parm unsigned char * | pIn | Pointer to the data
//
// @parm int | nWidth | Width of the texture
//
// @parm int | nHeight | Height of the texture
// 
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CDataCache::MipMapInPlace8 (unsigned char *pIn, int nWidth, int nHeight)
{
    //
    // Compute the working width and height.  
    //
    //  The width is not adjusted for the mipmap size, but is 
    //  adjusted for the size of the data
    //
    //  The height is adjusted for the mipmap size
    //

    //nWidth  *= 1;
    nHeight /= 2;

    //
    // Using input buffer as output also
    //

    unsigned char *pOut = pIn;

    //
    // Loop through the buffer
    //

    for (int i = 0; i < nHeight; i++)
    {
        for (int j = 0; j < nWidth; j += 2, pIn += 2, pOut += 1)
        {
            unsigned char *pIn2 = pIn + nWidth;
            pOut [0] = unsigned char ((pIn [0] + pIn [1] + pIn2 [0] + pIn2 [1]) >> 2);
        }
        pIn += nWidth;
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Mipmap a texture in place
//
// @parm unsigned char * | pIn | Pointer to the data
//
// @parm int | nComponents | Number of components
//
// @parm int | nWidth | Width of the texture
//
// @parm int | nHeight | Height of the texture
// 
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CDataCache::MipMapInPlace (unsigned char *pIn, int nComponents, 
                                int nWidth, int nHeight)
{
    switch (nComponents)
    {
        case 4:
            MipMapInPlace32 (pIn, nWidth, nHeight);
            break;
        case 3:
            MipMapInPlace24 (pIn, nWidth, nHeight);
            break;
        case 1:
            MipMapInPlace8 (pIn, nWidth, nHeight);
            break;
        default:
            _ASSERT (false);
            break;
    }
}


/* vim: set sw=4: */
