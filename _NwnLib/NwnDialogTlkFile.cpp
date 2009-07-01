//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnDialogTlkFile.cpp - Nwn dialog talk file |
//
// This module contains the Nwn dialog talk file.
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

#include "NwnDialogTlkFile.h"
#include "NwnDefines.h"

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnDialogTlkFile> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnDialogTlkFile::CNwnDialogTlkFile ()
{
    m_pauchData = NULL;
    m_pHeader = NULL;
    m_pasString = NULL;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNwnDialogTlkFile> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNwnDialogTlkFile::~CNwnDialogTlkFile ()
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

bool CNwnDialogTlkFile::Close ()
{
    if (m_pauchData)
        free (m_pauchData);
    m_pauchData = NULL;
    m_pHeader = NULL;
    m_pasString = NULL;
    m_strFileName .clear ();
    return true;
}

//-----------------------------------------------------------------------------
//
// @mfunc Open the file
//
// @parm const char * | pszDialogTlkFile | Full path name
//
// @rdesc true if the file was opened.
//
//-----------------------------------------------------------------------------

bool CNwnDialogTlkFile::Open (const char *pszDialogTlkFile)
{

    //
    // Close the old file
    //

    Close ();

    //
    // Load the file
    //

    m_pauchData = NwnLoadFile (pszDialogTlkFile, NULL);
    if (m_pauchData == NULL)
        return false;

    //
    // Get the other pointers
    //

    m_pHeader = (Header *) m_pauchData;
    m_pasString = (String *) &m_pauchData [sizeof (Header)];

    //
    // Save the file name
    //

    m_strFileName = pszDialogTlkFile;
    return true;
}

/* vim: set sw=4: */
