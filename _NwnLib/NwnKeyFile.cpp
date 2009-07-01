//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnKeyFile.cpp - Nwn key file |
//
// This module contains the Nwn key file.
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
// $History: Cnf.cpp $
//      
//-----------------------------------------------------------------------------

#include "NwnKeyFile.h"
#include "NwnBifFile.h"

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnKeyFile> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnKeyFile::CNwnKeyFile ()
{
    m_pauchData = NULL;
    m_pHeader = NULL;
    m_pasRes = NULL;
    m_pasBif = NULL;
    m_pasBifFiles = NULL;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnKeyFile> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnKeyFile::~CNwnKeyFile ()
{
    Close ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Close the file
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

bool CNwnKeyFile::Close ()
{
    if (m_pauchData)
        free (m_pauchData);
    if (m_pasBifFiles)
        delete [] m_pasBifFiles;
    m_pauchData = NULL;
    m_pHeader = NULL;
    m_pasRes = NULL;
    m_pasBif = NULL;
    m_pasBifFiles = NULL;
    m_strFileName .clear ();
    return true;
}

//-----------------------------------------------------------------------------
//
// @mfunc Open the file
//
// @parm const char * | pszKeyFile | Full path name
//
// @rdesc true if the file was opened.
//
//-----------------------------------------------------------------------------

bool CNwnKeyFile::Open (const char *pszKeyFile)
{

    //
    // Close the old file
    //

    Close ();

    //
    // Load the file
    //

    m_pauchData = NwnLoadFile (pszKeyFile, NULL);
    if (m_pauchData == NULL)
        return false;

    //
    // Get the header
    //

    m_pHeader = (Header *) m_pauchData;
    CNwnByteOrder<UINT32>::LittleEndianIP (m_pHeader ->ulBifCount);
    CNwnByteOrder<UINT32>::LittleEndianIP (m_pHeader ->ulBifOffset);
    CNwnByteOrder<UINT32>::LittleEndianIP (m_pHeader ->ulResCount);
    CNwnByteOrder<UINT32>::LittleEndianIP (m_pHeader ->ulResOffset);

    //
    // Get the bif data
    //

    m_pasBif = (Bif *) &m_pauchData [m_pHeader ->ulBifOffset];
    for (int i = 0; i < GetBifCount (); i++)
    {
        CNwnByteOrder<UINT32>::LittleEndianIP (m_pasBif [i] .ulLength);
        CNwnByteOrder<UINT32>::LittleEndianIP (m_pasBif [i] .ulNameOff);
        CNwnByteOrder<UINT16>::LittleEndianIP (m_pasBif [i] .usDiskPos);
        CNwnByteOrder<UINT16>::LittleEndianIP (m_pasBif [i] .usNameLength);
    }

    //
    // Get the ressources
    //

    m_pasRes = (Resource *) &m_pauchData [m_pHeader ->ulResOffset];
    for (int i = 0; i < GetResCount (); i++)
    {
        CNwnByteOrder<UINT32>::LittleEndianIP (m_pasRes [i] .ulBifID);
        CNwnByteOrder<UINT16>::LittleEndianIP (m_pasRes [i] .usType);
    }

    //
    // Allocate the bif files
    //

    m_pasBifFiles = new CNwnBifFile [m_pHeader ->ulBifCount];
    if (m_pasBifFiles == NULL)
    {
        Close ();
        return false;
    }

    //
    // Save the file name
    //

    m_strFileName = pszKeyFile;
    return true;
}

//-----------------------------------------------------------------------------
//
// @mfunc Load the given resource
//
// @parm const char * | pszName | Name of the resource
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

unsigned char *CNwnKeyFile::LoadRes (const char *pszName, 
                                     NwnResType nResType, UINT32 *pulSize, bool *pfAllocated)
{

    //
    // Find the resource
    //

    const Resource *psRes = FindRes (pszName, nResType);
    if (psRes == NULL)
        return NULL;

    //
    // Invoke the helper
    //

    return LoadRes (psRes, pulSize, pfAllocated);
}

//-----------------------------------------------------------------------------
//
// @mfunc Load the given resource
//
// @parm const Resource * | psRes | Resource to load
//
// @parm UINT32 * | pulSize | If loaded, populated with the resource size
//
// @parm bool * | pfAllocated | Pointer to a flag to received 
//              if the data is allocated or not.
//
// @rdesc Address of the allocated buffer.  Use free to free.
//
//-----------------------------------------------------------------------------

unsigned char *CNwnKeyFile::LoadRes (const Resource *psRes, 
                                     UINT32 *pulSize, bool *pfAllocated)
{

    //
    // Invoke the helper
    //

    return LoadRes (GetBifIndexFromRes (psRes), 
                    GetBifResIDFromRes (psRes), pulSize, pfAllocated);
}

//-----------------------------------------------------------------------------
//
// @mfunc Load the given resource
//
// @parm int | nBifIndex | Index of the bif
//
// @parm int | nBifResID | Res id in the bif file
//
// @parm UINT32 * | pulSize | If loaded, populated with the resource size
//
// @parm bool * | pfAllocated | Pointer to a flag to received 
//              if the data is allocated or not.
//
// @rdesc Address of the allocated buffer.  Use free to free.
//
//-----------------------------------------------------------------------------

unsigned char *CNwnKeyFile::LoadRes (int nBifIndex, 
                                     int nBifResID, UINT32 *pulSize, bool *pfAllocated)
{

    //
    // Open the bif file
    //

    CNwnBifFile *psBif = OpenBif (nBifIndex);
    if (psBif == NULL)
        return NULL;

    //
    // Load the resource from the bif
    //

    return psBif ->LoadRes (nBifResID, pulSize, pfAllocated);
}

//-----------------------------------------------------------------------------
//
// @mfunc Open the given bif file
//
// @parm int | nBifIndex | Index of the bif
//
// @rdesc Address of the bif file.  DO NOT DEALLOCATE.  If NULL, the
//              bif could not be loaded
//
//-----------------------------------------------------------------------------

CNwnBifFile *CNwnKeyFile::OpenBif (int nBifIndex)
{

    //
    // If already open, return
    //

    if (m_pasBifFiles [nBifIndex] .IsOpen ())
        return &m_pasBifFiles [nBifIndex];

    //
    // Get the bif
    //

    const Bif *psBif = GetBif (nBifIndex);

    //
    // Generate a path name
    //

    char szFileName [512];
    strncpy (szFileName, m_strFileName .c_str (), _countof (szFileName));
    char *pszTemp = szFileName;
    for (char *psz = szFileName; *psz; ++psz) 
    {
        if (*psz == '\\' || *psz == '/' || *psz == ':')
            pszTemp = psz + 1;
    }
    GetBifName (psBif, pszTemp, 
                (int) (&szFileName [_countof (szFileName)] - pszTemp));

    //
    // Open the file
    //

    if (m_pasBifFiles [nBifIndex] .Open (szFileName))
        return &m_pasBifFiles [nBifIndex];
    else
        return false;
}

/* vim: set sw=4: */
