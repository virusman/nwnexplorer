#ifndef ETS_NWNMODULEFILE_H
#define ETS_NWNMODULEFILE_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnModuleFile.h - Nwn module (MOD,NWM,HAK) file support |
//
// This module contains the definition of the NwnModuleFile.
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

class CNwnModuleFile 
{
    public:

        struct Header
        {
            char        achSig [4];                                     // 0x0000
            char        achVersion [4];                         // 0x0004
            UINT32      ulStringCount;                          // 0x0008
            UINT32      ulStringTotalSize;                      // 0x000C
            UINT32      ulResCount;                                     // 0x0010
            UINT32      ulStringOffset;                         // 0x0014
            UINT32      ulResOffset;                            // 0x0018
            UINT32      ulPositionOffset;                       // 0x001C
            UINT32      ulYear;                                         // 0x0020 // offset 1900
            UINT32      ulDay;                                          // 0x0024 // Jan 1st is 0               
            UINT32      ulUnknown;                                      // 0x0028 // MOD = -1, SAV = 0
            // ERF and HAK = uninitialized
            UINT32      aulSpare [29];                          // 0x002C // init to zero
            // 0x00A0
        };

        struct Resource
        {
            char        szName [16];
            UINT32      ulIndex;
            UINT32      ulType;
        };

        struct Position
        {
            UINT32      ulOffset;
            UINT32      ulLength;
        };

        struct String
        {
            UINT32      ulLanguage;
            UINT32      ulLength;
        };

        // @access Constructors and destructors
    public:

        // @cmember General constructor

        CNwnModuleFile ();

        // @cmember General destructor

        virtual ~CNwnModuleFile ();

        // @access Public methods
    public:

        // @cmember Open the file

        bool Open (const char *pszModuleFile);

        // @cmember Close the file

        bool Close ();

        // @cmember Load resource

        unsigned char *LoadRes (const char *pszName, 
                                NwnResType nResType, UINT32 *pulSize, bool *pfAllocated);

        // @cmember Load resource

        unsigned char *LoadRes (const Resource *psRes, 
                                UINT32 *pulSize, bool *pfAllocated);

        // @access Public inline methods
    public:

        // @cmember Is the file open

        bool IsOpen () const
        {
            return m_sHeader .achSig [0] != 0;
        }

        // @cmember Get the number of resources

        int GetResCount () const
        {
            assert (IsOpen ());
            return (int) m_sHeader .ulResCount;
        }

        // @cmember Get the n'th resource

        const Resource *GetRes (int nIndex) const
        {
            assert (IsOpen ());
            assert (nIndex >= 0 && (UINT32) nIndex < m_sHeader .ulResCount);
            return &m_pasRes [nIndex];
        }

        // @cmember Find a resource

        const Resource *FindRes (const char *pszName, NwnResType nResType)
        {
            for (UINT32 i = 0; i < m_sHeader .ulResCount; i++)
            {
                if (m_pasRes [i] .ulType == (UINT32) nResType &&
                    strnicmp (m_pasRes [i] .szName, pszName, _countof (
                            m_pasRes [i] .szName)) == 0)
                    return &m_pasRes [i];
            }
            return NULL;
        }

        // @cmember Get string count

        int GetStringCount () const
        {
            return (int) m_sHeader .ulStringCount;
        }

        // @cmember Return a pointer to the n'th string

        String *GetString (int nIndex)
        {
            if (nIndex >= 0 && nIndex < (int) m_sHeader .ulStringCount)
            {
                assert (false);
                return NULL;
            }
            unsigned char *p = m_pauchStrings;
            while (nIndex > 0)
            {
                String *pstr = (String *) p;
                p += sizeof (String) + pstr ->ulLength;
                nIndex--;
            }
            return (String *) p;
        }

        // @access Protected methods
    protected:

        // @cmember Name of the file

        std::string                     m_strFileName;

        // @cmember Header data

        Header                          m_sHeader;

        // @cmember Pointer to the res array

        Resource                        *m_pasRes;

        // @cmember Pointer to the position array

        Position                        *m_pasPosition;

        // @cmember String data

        unsigned char           *m_pauchStrings;
};

#endif // ETS_NWNMODULEFILE_H

/* vim: set sw=4: */
