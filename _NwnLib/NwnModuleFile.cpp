//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnModuleFile.cpp - Nwn module file |
//
// This module contains the Nwn module file.
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

#include "NwnModuleFile.h"

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnModuleFile> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnModuleFile::CNwnModuleFile ()
{
    m_sHeader .achSig [0] = 0;
    m_sHeader .achVersion [0] = 0;
    m_pasRes = NULL;
    m_pasPosition = NULL;
    m_pauchStrings = NULL;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnModuleFile> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnModuleFile::~CNwnModuleFile ()
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

bool CNwnModuleFile::Close ()
{
    if (m_pasRes)
        delete [] m_pasRes;
    if (m_pasPosition)
        delete [] m_pasPosition;
    if (m_pauchStrings)
        delete [] m_pauchStrings;
    m_pasRes = NULL;
    m_pasPosition = NULL;
    m_pauchStrings = NULL;
    m_sHeader .achSig [0] = 0;
    m_strFileName .clear ();
    return true;
}

//-----------------------------------------------------------------------------
//
// @mfunc Open the file
//
// @parm const char * | pszModuleFile | Full path name
//
// @rdesc true if the file was opened.
//
//-----------------------------------------------------------------------------

bool CNwnModuleFile::Open (const char *pszModuleFile)
{

    //
    // Close the old file
    //

    Close ();

    //
    // Try to open the new file
    //

    FILE *fp = fopen (pszModuleFile, "rb");
    if (fp == NULL)
        return false;

    //
    // Read the header
    //

    fread (&m_sHeader, 1, sizeof (m_sHeader), fp);

    //
    // Check for encrypted haks
    // 
    if (memcmp(m_sHeader .achSig, "HAK ", 4) == 0 &&
        memcmp(m_sHeader .achVersion, "V1.1", 4) == 0) {

        Close ();
        fclose (fp);
        return false;
    }

    CNwnByteOrder<UINT32>::LittleEndianIP (m_sHeader .ulDay);
    CNwnByteOrder<UINT32>::LittleEndianIP (m_sHeader .ulPositionOffset);
    CNwnByteOrder<UINT32>::LittleEndianIP (m_sHeader .ulResCount);
    CNwnByteOrder<UINT32>::LittleEndianIP (m_sHeader .ulResOffset);
    CNwnByteOrder<UINT32>::LittleEndianIP (m_sHeader .ulStringCount);
    CNwnByteOrder<UINT32>::LittleEndianIP (m_sHeader .ulStringOffset);
    CNwnByteOrder<UINT32>::LittleEndianIP (m_sHeader .ulStringTotalSize);
    CNwnByteOrder<UINT32>::LittleEndianIP (m_sHeader .ulUnknown);
    CNwnByteOrder<UINT32>::LittleEndianIP (m_sHeader .ulYear);
    for (int i = 0; i < _countof (m_sHeader .aulSpare); i++)
    {
        CNwnByteOrder<UINT32>::LittleEndianIP (m_sHeader .aulSpare [i]);
    }

    //
    // Allocate the buffers
    //

    m_pasRes = new Resource [m_sHeader .ulResCount];
    m_pasPosition = new Position [m_sHeader .ulResCount];
    m_pauchStrings = new unsigned char [m_sHeader .ulStringTotalSize];
    if (m_pasRes == NULL || m_pasPosition == NULL || m_pauchStrings == NULL)
    {
        Close ();
        fclose (fp);
        return false;
    }

    //
    // Read the resources and position
    //

    fseek (fp, m_sHeader .ulResOffset, SEEK_SET);
    fread (m_pasRes, m_sHeader .ulResCount, sizeof (Resource), fp);
    fseek (fp, m_sHeader .ulPositionOffset, SEEK_SET);
    fread (m_pasPosition, m_sHeader .ulResCount, sizeof (Position), fp);
    fseek (fp, m_sHeader .ulStringOffset, SEEK_SET);
    fread (m_pauchStrings, m_sHeader .ulStringTotalSize, 1, fp);

    //
    // Convert the byte order of the resource data
    //

    for (unsigned int i = 0; i < m_sHeader .ulResCount; i++)
    {
        CNwnByteOrder<UINT32>::LittleEndianIP (m_pasRes [i] .ulIndex);
        CNwnByteOrder<UINT32>::LittleEndianIP (m_pasRes [i] .ulType);
    }
    for (unsigned int i = 0; i < m_sHeader .ulResCount; i++)
    {
        CNwnByteOrder<UINT32>::LittleEndianIP (m_pasPosition [i] .ulLength);
        CNwnByteOrder<UINT32>::LittleEndianIP (m_pasPosition [i] .ulOffset);
    }

    //
    // Convert the byte order of the string data
    //

    {
        unsigned char *p = m_pauchStrings;
        for (unsigned int i = 0; i < m_sHeader .ulStringCount; i++)
        {
            String *pstr = (String *) p;
            CNwnByteOrder<UINT32>::LittleEndianIP (pstr ->ulLanguage);
            CNwnByteOrder<UINT32>::LittleEndianIP (pstr ->ulLength);
            p += sizeof (String) + pstr ->ulLength;
        }
    }

    //
    // Save the file name
    //

    m_strFileName = pszModuleFile;
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

unsigned char *CNwnModuleFile::LoadRes (const char *pszName, 
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

unsigned char *CNwnModuleFile::LoadRes (const Resource *psRes, 
                                        UINT32 *pulSize, bool *pfAllocated)
{

    //
    // Open the file
    //

    FILE *fp = fopen (m_strFileName .c_str (), "rb");
    if (fp == NULL)
        return NULL;

    //
    // Get the position
    //

    int nIndex = (int) (psRes - m_pasRes);
    Position *psPos = &m_pasPosition [nIndex];

    //
    // Allocate the data
    //

    unsigned char *pauchData = (unsigned char *) malloc (psPos ->ulLength);
    if (pauchData == NULL)
    {
        fclose (fp);
        return NULL;
    }

    //
    // Read the data
    //

    fseek (fp, psPos ->ulOffset, SEEK_CUR);
    fread (pauchData, 1, psPos ->ulLength, fp);
    fclose (fp);
    if (pulSize)
        *pulSize = psPos ->ulLength;
    if (pfAllocated)
        *pfAllocated = true;
    return pauchData;
}

/* vim: set sw=4: */
