#ifndef ETS_NWNDIALOGTLKFILE_H
#define ETS_NWNDIALOGTLKFILE_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnDialogTlkFile.h - Nwn dialog talk file support |
//
// This module contains the definition of the NwnDialogTlkFile.
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

class CNwnDialogTlkFile 
{
    public:

        enum StringType
        {
            StringType_Unused           = 0, //placeholder
            StringType_ResRef           = 6, //name, no string
            StringType_String           = 7, //string and maybe name
        };

        struct Header
        {
            char        achSig [4];
            char        achVersion [4];
            UINT32      ulUnknown;
            UINT32      ulStringCount;
            UINT32      ulStringOffset;
        };

        struct String
        {
            UINT32      ulType;
            char        szName [16];
            UINT32      ulUnknown1; // pitch??
            UINT32      ulUnknown2; // volume??
            UINT32      ulOffset;
            UINT32      ulSize;
            UINT32      ulUnknown3;
        };

        // @access Constructors and destructors
    public:

        // @cmember General constructor

        CNwnDialogTlkFile ();

        // @cmember General destructor

        virtual ~CNwnDialogTlkFile ();

        // @access Public methods
    public:

        // @cmember Open the file

        bool Open (const char *pszDialogTlkFile);

        // @cmember Close the file

        bool Close ();

        // @access Public inline methods
    public:

        // @cmember Is the file open

        bool IsOpen () const
        {
            return m_pauchData != NULL;
        }

        // @cmember Get the number of strings

        int GetStringCount () const
        {
            assert (m_pHeader != NULL);
            return (int)m_pHeader ->ulStringCount;
        }

        // @cmember Get the n'th string

        const String *GetString (int nIndex) const
        {
            assert (m_pHeader != NULL);
            assert (nIndex >= 0 && (UINT32) nIndex < m_pHeader ->ulStringCount);
            return &m_pasString [nIndex];
        }

        // @cmember Get the string length

        int GetStringLength (int nIndex) const
        {
            assert (m_pHeader != NULL);
            assert (nIndex >= 0 && (UINT32) nIndex < m_pHeader ->ulStringCount);
            return (int) m_pasString [nIndex] .ulSize;
        }

        // @cmember Get the string

        int GetString (int nIndex, char *pszText, int cchMaxText)
        {
            assert (m_pHeader != NULL);
            assert (nIndex >= 0 && (UINT32) nIndex < m_pHeader ->ulStringCount);
            return GetString (&m_pasString [nIndex], pszText, cchMaxText);
        }

        // @cmember Get the string

        int GetString (const String *pString, char *pszText, int cchMaxText)
        {
            assert (cchMaxText > 0);
            assert (m_pHeader != NULL);
            if (pString ->ulOffset == 0 && pString ->ulSize == 0)
                return -1;
            int nLength = (int) pString ->ulSize;
            if (nLength > cchMaxText - 1)
                nLength = cchMaxText -1;
            memmove (pszText, &m_pauchData [m_pHeader ->ulStringOffset + 
                     pString ->ulOffset], nLength);
            pszText [nLength] = 0;
            return nLength;
        }

        // @cmember Get the string name

        int GetStringName (int nIndex, char *pszText, int cchMaxText)
        {
            assert (m_pHeader != NULL);
            assert (nIndex >= 0 && (UINT32) nIndex < m_pHeader ->ulStringCount);
            return GetStringName (&m_pasString [nIndex], pszText, cchMaxText);
        }

        // @cmember Get the string name

        int GetStringName (const String *pString, char *pszText, int cchMaxText)
        {
            assert (cchMaxText > 0);
            assert (m_pHeader != NULL);
            int nLength;
            for (nLength = 0; nLength < _countof (pString ->szName); nLength++)
                if (pString ->szName [nLength] == 0)
                    break;
            if (nLength == 0)
                return -1;
            if (nLength > cchMaxText - 1)
                nLength = cchMaxText -1;
            memmove (pszText, pString ->szName, nLength);
            pszText [nLength] = 0;
            return nLength;
        }

        // @access Protected methods
    protected:

        // @cmember Name of the file

        std::string                     m_strFileName;

        // @cmember Pointer to the full buffer

        unsigned char           *m_pauchData;

        // @cmember Pointer to the header

        Header                          *m_pHeader;

        // @cmember Pointer to the string array

        String                          *m_pasString;
};

#endif // ETS_NWNDIALOGTLKFILE_H

/* vim: set sw=4: */
