#ifndef ETS_NWNBIFFILE_H
#define ETS_NWNBIFFILE_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnBifFile.h - Nwn bif file support |
//
// This module contains the definition of the NwnBifFile.
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

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include "NwnDefines.h"

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CNwnBifFile 
{
    public:

        struct Header 
        {
            char        achSig [4];
            char        achVersion [4];
            UINT32      ulCount;
            UINT32      ulUnknown;
            UINT32      ulOffset;
        };

        struct Resource
        {
            UINT32      ulID;
            UINT32      ulOffset;
            UINT32      ulLength;
            UINT32      ulType;
        };

        // @access Constructors and destructors
    public:

        // @cmember General constructor

        CNwnBifFile ();

        // @cmember General destructor

        virtual ~CNwnBifFile ();

        // @access Public methods
    public:

        // @cmember Open the file

        bool Open (const char *pszBifFile);

        // @cmember Close the file

        bool Close ();

        // @cmember Load resource

        unsigned char *LoadRes (UINT32 dwBifResID, 
                                UINT32 *pulSize, bool *pfAllocated);

        // @access Public inline methods
    public:

        // @cmember Is the bif file open

        bool IsOpen () const
        {
            return m_pasRes != NULL;
        }

        // @cmember Get the resource count

        int GetResCount () const
        {
            assert (m_pasRes != NULL);
            return (int) m_sHeader .ulCount;
        }

        // @cmember Get the nth res

        const CNwnBifFile::Resource *GetRes (int nIndex) const
        {
            assert (m_pasRes != NULL);
            assert (nIndex >= 0 && nIndex < (int) m_sHeader .ulCount);
            return &m_pasRes [nIndex];
        }

        // @access Protected methods
    protected:

        // @cmember Name of the file

        std::string                     m_strFileName;

        // @cmember Header

        Header                          m_sHeader;

        // @cmember Pointer to the res array

        Resource                                *m_pasRes;
};

#endif // ETS_NWNBIFFILE_H

/* vim: set sw=4: */
