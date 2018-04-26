//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnBifFile.cpp - Nwn bif file |
//
// This module contains the Nwn bif file.
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

#include "NwnBifFile.h"

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnBifFile> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnBifFile::CNwnBifFile ()
{
    m_sHeader .achSig [0] = 0;
    m_pasRes = NULL;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnBifFile> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnBifFile::~CNwnBifFile ()
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

bool CNwnBifFile::Close ()
{
    if (m_pasRes)
        delete [] m_pasRes;
    m_sHeader .achSig [0] = 0;
    m_pasRes = NULL;
    m_strFileName .clear ();
    return true;
}

//-----------------------------------------------------------------------------
//
// @mfunc Open the file
//
// @parm const char * | pszBifFile | Full path name
//
// @rdesc true if the file was opened.
//
//-----------------------------------------------------------------------------

bool CNwnBifFile::Open (const char *pszBifFile)
{

    //
    // Close the old file
    //

    Close ();

    //
    // Try to open the new file
    //

    FILE *fp = fopen (pszBifFile, "rb");
	if (fp == NULL)
	{
		char *pszParentPath = new char[strlen(pszBifFile) + 4];
		sprintf(pszParentPath, "..\\%s", pszBifFile);
		pszParentPath[strlen(pszBifFile) + 3] = '\0';
		fp = fopen(pszParentPath, "rb");
		delete pszParentPath;
	}

    if (fp == NULL)
        return false;

    //
    // Read the header
    //

    fread (&m_sHeader, 1, sizeof (m_sHeader), fp);
    CNwnByteOrder<UINT32>::LittleEndianIP (m_sHeader .ulCount);
    CNwnByteOrder<UINT32>::LittleEndianIP (m_sHeader .ulOffset);
    CNwnByteOrder<UINT32>::LittleEndianIP (m_sHeader .ulUnknown);

    //
    // Allocate memory for the resources
    //

    m_pasRes = new Resource [m_sHeader .ulCount];
    if (m_pasRes == NULL)
    {
        fclose (fp);
        Close ();
        return false;
    }

    //
    // Read the res table
    //

    fseek (fp, m_sHeader .ulOffset, SEEK_SET);
    fread (m_pasRes, 1, m_sHeader .ulCount * sizeof (Resource), fp);
    fclose (fp);

    //
    // Convert the resource data
    //

    for (unsigned int i = 0; i < m_sHeader .ulCount; i++)
    {
        CNwnByteOrder<UINT32>::LittleEndianIP (m_pasRes [i] .ulID);
        CNwnByteOrder<UINT32>::LittleEndianIP (m_pasRes [i] .ulLength);
        CNwnByteOrder<UINT32>::LittleEndianIP (m_pasRes [i] .ulOffset);
        CNwnByteOrder<UINT32>::LittleEndianIP (m_pasRes [i] .ulType);
    }

    //
    // Save the file namme
    //

    m_strFileName = pszBifFile;
    return true;
}

//-----------------------------------------------------------------------------
//
// @mfunc Load a resource
//
// @parm UINT32 | dwBifResID | Resource ID
//
// @parm UINT32 * | pulSize | Size of the resource
//
// @parm bool * | pfAllocated | Pointer to a flag to received 
//              if the data is allocated or not.
//
// @rdesc Address of the allocated buffer.  Use free to free.
//
//-----------------------------------------------------------------------------

unsigned char *CNwnBifFile::LoadRes (UINT32 dwBifResID, 
                                     UINT32 *pulSize, bool *pfAllocated)
{

    //
    // If we aren't open, return an error
    //

    if (!IsOpen ())
        return NULL;

    //
    // Remove the bif index from the res ID and validate the index
    //

    dwBifResID &= 0xFFFFF;
    if ((int) dwBifResID >= GetResCount ())
        return NULL;

    //
    // Get the entry for the res
    //

    Resource *psRes = &m_pasRes [dwBifResID];
    assert ((psRes ->ulID & 0xFFFFF) == dwBifResID);

    //
    // Open the file
    //

    FILE *fp = fopen (m_strFileName .c_str (), "rb");
    if (fp == NULL)
        return NULL;

    //
    // Allocate the data
    //

    unsigned char *pauchData = (unsigned char *) malloc (psRes ->ulLength);
    if (pauchData == NULL)
    {
        fclose (fp);
        return NULL;
    }

    //
    // Seek and read
    //

    fseek (fp, psRes ->ulOffset, SEEK_SET);
    fread (pauchData, 1, psRes ->ulLength, fp);
    fclose (fp);

    //
    // Save the length
    //

    if (pulSize != NULL)
        *pulSize = psRes ->ulLength;
    if (pfAllocated)
        *pfAllocated = true;
    return pauchData;
}


/* vim: set sw=4: */
