#ifndef ETS_DATACACHE_H
#define ETS_DATACACHE_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      DataCache.h - Resource cache for data files |
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
// $History: HexView.h $
//      
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include "DataKey.h"
#include "../_NwnLib/NwnDialogTlkFile.h"
#include "../_NwnLib/NwnKeyFile.h"
#include "../_NwnLib/NwnModuleFile.h"
#include "../_NwnLib/NwnBifFile.h"

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

class CData;

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CDataCache
{

    // @access Construction and destruction
    public:

        // @cmember General constructor

        CDataCache (CNwnKeyFile *pKeyFiles, CNwnModuleFile *pHakFiles,
                    CNwnDialogTlkFile *pDlgTlkFile);

        // @cmember General destructor

        ~CDataCache ();

        // @access Public method
    public:

        // @cmember Load a resource based on a key file res structure

        CData *LoadRes (CNwnKeyFile *psKeyFile, 
                        const CNwnKeyFile::Resource *psRes);

        // @cmember Load a resource based on a key file res structure

        CData *LoadRes (CNwnKeyFile *psKeyFile, 
                        const char *pszName, NwnResType nResType);

        // @cmember Load a resource based on a module file res structure

        CData *LoadRes (CNwnModuleFile *psModule, 
                        const CNwnModuleFile::Resource *psRes);

        // @cmember Load a resource based on a module file res structure

        CData *LoadRes (CNwnModuleFile *psModule, 
                        const char *pszName, NwnResType nResType);

        // @cmember Load a resource based on a bif file res structure

        CData *LoadRes (CNwnBifFile *psBifFile, 
                        const CNwnBifFile::Resource *psRes);

        // @cmember Load a resource based on a file name

        CData *LoadRes (const char *pszFileName);

        // @cmember Load a resource 

        CData *LoadOrderedRes (CNwnModuleFile *psModule, 
                               const char *pszSourceFileName, const char *pszName, 
                               NwnResType nResType);

        // @cmember Load a texture

        CData *LoadTexture (CNwnModuleFile *psModule, const char *pszModelFile, 
                            const char *pszName, int *panPalette);

        // @cmember Notification from a data object that is has been released

        void DataReleased (CData *pData);

        // @access Protected methods
    protected:

        // @cmember Load a PLT texture

        CData *LoadPLTTexture (CNwnModuleFile *psModule, 
                               const char *pszModelFile, const char *pszName,
                               int *panPalette);

        // @cmember Load a TGA or DDS texture

        CData *LoadTGAorDDSTexture (CNwnModuleFile *psModule, 
                                    const char *pszModelFile,
                                    const char *pszName,
                                    BOOL bLoadDds);

        // @cmember Mipmap a 32 bit texture in place

        void MipMapInPlace32 (unsigned char *pIn, int nWidth, int nHeight);

        // @cmember Mipmap a 16 bit texture in place

        void MipMapInPlace24 (unsigned char *pIn, int nWidth, int nHeight);

        // @cmember Mipmap a 8 bit texture in place

        void MipMapInPlace8 (unsigned char *pIn, int nWidth, int nHeight);

        // @cmember Mipmap a texture in place

        void MipMapInPlace (unsigned char *pIn, int nComponents, 
                            int nWidth, int nHeight);

        // @access Protected variables
    protected:

        // @cmember Pointer to the key files

        CNwnKeyFile                     *m_pKeyFiles;

        // @cmember Pointer to the hak files

        CNwnModuleFile                  *m_pHakFiles;

        // @cmember Pointer to the dialog talk file

        CNwnDialogTlkFile               *m_pDlgTlkFile;

        // @cmember Loaded data map

        CRBMap <CDataKey *, CData *> m_mapData;
};

#endif // ETS_DATACACHE_H

/* vim: set sw=4: */
