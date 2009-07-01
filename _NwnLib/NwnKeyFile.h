#ifndef ETS_NWNKEYFILE_H
#define ETS_NWNKEYFILE_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnKeyFile.h - Nwn key file support |
//
// This module contains the definition of the NwnKeyFile.
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

class CNwnBifFile;

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CNwnKeyFile 
{
    public:

        struct Header
        {
            char        achSig [4];
            char        achVersion [4];
            UINT32      ulBifCount;
            UINT32      ulResCount;
            UINT32      ulBifOffset;
            UINT32      ulResOffset;
        };

        struct Bif
        {
            UINT32      ulLength;
            UINT32      ulNameOff;
            UINT16      usNameLength;
            UINT16      usDiskPos;
        };

#pragma pack (push,1)
#pragma pack (1)
        struct Resource
        {
            char        szName [16];
            UINT16      usType;
            UINT32      ulBifID;
        };
#pragma pack (pop)

        // @access Constructors and destructors
    public:

        // @cmember General constructor

        CNwnKeyFile ();

        // @cmember General destructor

        virtual ~CNwnKeyFile ();

        // @access Public methods
    public:

        // @cmember Open the file

        bool Open (const char *pszKeyFile);

        // @cmember Close the file

        bool Close ();

        // @cmember Return the address of a opened bif file

        CNwnBifFile *OpenBif (int nBifIndex);

        // @cmember Load resource

        unsigned char *LoadRes (const char *pszName, 
                                NwnResType nResType, UINT32 *pulSize, bool *pfAllocated);

        // @cmember Load resource

        unsigned char *LoadRes (const Resource *psRes, 
                                UINT32 *pulSize, bool *pfAllocated);

        // @cmember Load resource

        unsigned char *LoadRes (int nBifIndex, int nBifResID, 
                                UINT32 *pulSize, bool *pfAllocated);

        // @access Public inline methods
    public:

        // @cmember Is the file open

        bool IsOpen () const
        {
            return m_pauchData != NULL;
        }

        // @cmember Get the number of bifs

        int GetBifCount () const
        {
            assert (m_pHeader != NULL);
            return m_pHeader ->ulBifCount;
        }

        // @cmember Get the n'th big

        Bif *GetBif (int nIndex) const
        {
            assert (m_pHeader != NULL);
            assert (nIndex >= 0 && (UINT32) nIndex < m_pHeader ->ulBifCount);
            return &m_pasBif [nIndex];
        }

        // @cmember Get the bif name

        int GetBifName (const Bif *psBif, char *pszName, int cchMaxText) const
        {
            int nLength = (int) psBif ->usNameLength;
            if (nLength > cchMaxText - 1)
                nLength = cchMaxText -1;
            memmove (pszName, (char *) &m_pauchData [psBif ->ulNameOff], nLength);
            pszName [nLength] = 0;
#ifndef _WIN32 
            for (int i = 0; i < nLength; i++) 
            {
                if (pszName [i] == '\\') 
                    pszName [i] = '/'; 
            }
#endif 
            return nLength;
        }

        // @cmember Get the bif name

        int GetBifName (int nBifIndex, char *pszName, int cchMaxText) const
        {
            const Bif *psBif = GetBif (nBifIndex);
            return GetBifName (psBif, pszName, cchMaxText);
        }

        // @cmember Get the number of resources

        int GetResCount () const
        {
            assert (m_pHeader != NULL);
            return m_pHeader ->ulResCount;
        }

        // @cmember Get the n'th resource

        Resource *GetRes (int nIndex) const
        {
            assert (m_pHeader != NULL);
            assert (nIndex >= 0 && (UINT32) nIndex < m_pHeader ->ulResCount);
            return &m_pasRes [nIndex];
        }

        // @cmember Find a resource

        Resource *FindRes (const char *pszName, NwnResType nResType)
        {
            for (UINT32 dw = 0; dw < m_pHeader ->ulResCount; dw++)
            {
                if (m_pasRes [dw] .usType == nResType &&
                    strnicmp (m_pasRes [dw] .szName, pszName, _countof (
                            m_pasRes [dw] .szName)) == 0)
                    return &m_pasRes [dw];
            }
            return NULL;
        }

        // @cmember Get the BIF index from the res

        static int GetBifIndexFromRes (const Resource *psRes)
        {
            return (int) (psRes ->ulBifID >> 20);
        }

        // @cmember Get the BIF index from the res

        static int GetBifResIDFromRes (const Resource *psRes)
        {
            return (int) (psRes ->ulBifID & 0xFFFFF);
        }

        // @access Protected methods
    protected:

        // @cmember Name of the file

        std::string                     m_strFileName;

        // @cmember Pointer to the full buffer

        unsigned char           *m_pauchData;

        // @cmember Pointer to the header

        Header                          *m_pHeader;

        // @cmember Pointer to the res array

        Resource                                *m_pasRes;

        // @cmember Pointer to the bif array

        Bif                             *m_pasBif;

        // @cmember Array of bif files

        CNwnBifFile                     *m_pasBifFiles;
};

#endif // ETS_NWNKEYFILE_H

/* vim: set sw=4: */
