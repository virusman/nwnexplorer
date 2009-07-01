#ifndef ETS_NWNMEMORYFILE_H
#define ETS_NWNMEMORYFILE_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnMemoryFile.h - Nwn memory file support |
//
// This module contains the definition of the NwnMemoryFile.
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

class CNwnMemoryFile 
{

    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CNwnMemoryFile (unsigned char *pauchData = NULL, 
                        unsigned long ulSize = 0, bool fAllocated = true)
        {
            m_pauchData = NULL;
            m_ulSize = 0;
            SetData (pauchData, ulSize, fAllocated);
        }

        // @cmember General destructor

        virtual ~CNwnMemoryFile ()
        {
            ClearData ();
        }

        // @access Public inline methods
    public:

        // @cmember Set the data associated with the file

        void SetData (unsigned char *pauchData = NULL, 
                      unsigned long ulSize = 0, bool fAllocated = true)
        {
            if (m_pauchData == pauchData)
                return;
            ClearData ();
            m_pauchData = pauchData;
            m_ulSize = ulSize;
            m_fAllocated = fAllocated;
        }

        // @cmember Clear the data

        void ClearData ()
        {
            if (m_pauchData && m_fAllocated)
                free (m_pauchData);
            m_pauchData = NULL;
            m_ulSize = 0;
            m_fAllocated = false;
        }

        // @cmember Get a pointer to the data

        unsigned char *GetData (int nIndex = 0) 
        {
            assert (m_pauchData != NULL);
            // note: to simplify the coding of some routines, we are
            // allowed to return a pointer to the end of the data
            assert (nIndex >= 0 && nIndex <= (int) m_ulSize);
            return &m_pauchData [nIndex];
        }

        // @cmember Get the size

        unsigned long GetSize () 
        {
            return m_ulSize;
        }

        // @access Protected methods
    protected:

        // @cmember Pointer to the data

        unsigned char           *m_pauchData;

        // @cmember Size of the data

        unsigned long           m_ulSize;

        // @cmember If true, data is allocated

        bool                            m_fAllocated;
};

#endif // ETS_NWNMEMORYFILE_H

/* vim: set sw=4: */
