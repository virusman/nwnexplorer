#ifndef ETS_DATAKEY_H
#define ETS_DATAKEY_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      DataKey.h - Nwn data support |
//
// This module contains the definition of the data object.
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

#include "../_NwnLib/NwnDefines.h"

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

class CNwnModuleFile;
class CNwnKeyFile;
class CNwnBifFile;

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CDataKey
{
    public:

        enum Type
        {
            Type_Raw,
            Type_OpenGLTexture,
            Type_BinaryModel,
        };

        // @access Constructors and destructors
    public:

        // @cmember General constructor

        CDataKey (CNwnKeyFile *pKeyFile, CNwnModuleFile *pModuleFile, 
                  CNwnBifFile *pBifFile, const char *pszName, NwnResType nResType, 
                  Type nType, unsigned char *pauchExtraData = NULL, 
                  int nExtraDataSize = 0) : m_strResName (pszName)
    {
        m_pKeyFile = pKeyFile;
        m_pModuleFile = pModuleFile;
        m_pBifFile = pBifFile;
        m_nResType = nResType;
        m_nType = nType;
        if (pauchExtraData != NULL && nExtraDataSize > 0)
        {
            m_pauchExtraData = new unsigned char [nExtraDataSize];
            memcpy (m_pauchExtraData, pauchExtraData, nExtraDataSize);
            m_nExtraDataSize = nExtraDataSize;
        }
        else
        {
            m_pauchExtraData = NULL;
        }
    }

        // @cmember General destructor

        ~CDataKey ()
        {
            if (m_pauchExtraData)
                delete [] m_pauchExtraData;
        }

        // @access Operators
    public:

        // @cmember Equal operator

        bool operator == (const CDataKey &sKey) const
        {
            if (m_pKeyFile != sKey .m_pKeyFile)
                return false;
            if (m_pModuleFile != sKey .m_pModuleFile)
                return false;
            if (m_pBifFile != sKey .m_pBifFile)
                return false;
            if (m_nResType != sKey .m_nResType)
                return false;
            if (m_nType != sKey .m_nType)
                return false;
            if (strcmp (m_strResName, sKey .m_strResName) != 0)
                return false;
            if (m_nExtraDataSize < 0 || sKey .m_nExtraDataSize < 0)
                return true;
            if (m_nExtraDataSize != sKey .m_nExtraDataSize)
                return false;
            return memcmp (m_pauchExtraData, 
                           sKey .m_pauchExtraData, m_nExtraDataSize) == 0;
        }

        // @cmember General comparison

        int Compare (const CDataKey &sKey) const
        {
            if (m_pKeyFile < sKey .m_pKeyFile)
                return -1;
            if (m_pKeyFile > sKey .m_pKeyFile)
                return 1;
            if (m_pModuleFile < sKey .m_pModuleFile)
                return -1;
            if (m_pModuleFile > sKey .m_pModuleFile)
                return 1;
            if (m_pBifFile < sKey .m_pBifFile)
                return -1;
            if (m_pBifFile > sKey .m_pBifFile)
                return 1;
            if (m_nResType < sKey .m_nResType)
                return -1;
            if (m_nResType > sKey .m_nResType)
                return 1;
            if (m_nType < sKey .m_nType)
                return -1;
            if (m_nType > sKey .m_nType)
                return 1;
            int nCompare = strcmp (m_strResName, sKey .m_strResName);
            if (nCompare != 0)
                return nCompare;
            if (m_nExtraDataSize < sKey .m_nExtraDataSize)
                return -1;
            if (m_nExtraDataSize > sKey .m_nExtraDataSize)
                return 1;
            return memcmp (m_pauchExtraData, 
                           sKey .m_pauchExtraData, m_nExtraDataSize);           
        }

        // @access Public variables
    public:

        // @cmember Key file source

        CNwnKeyFile                     *m_pKeyFile;

        // @cmember Module file source

        CNwnModuleFile          *m_pModuleFile;

        // @cmember Bif file source (Only used for directly opened bif files)

        CNwnBifFile                     *m_pBifFile;

        // @cmember Name of the resource

        //char                          m_szResName [16];
        CString                         m_strResName;

        // @cmember Resource type

        NwnResType                      m_nResType;

        // @cmember Type of the resource

        Type                            m_nType;

        // @cmember Patch file flag

        bool                            m_fPatch;

        // @cmember Extra data

        unsigned char           *m_pauchExtraData;

        // @cmember Size of the extra data

        int                                     m_nExtraDataSize;
};

#if 0
//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

template<>
class CElementTraits <CDataKey *> :
public CElementTraitsBase <T>
{
    public:
        static ULONG Hash (INARGTYPE guid)
        {
            ATLASSERT (FALSE);
            return 0;
        }

        static bool CompareElements( INARGTYPE element1, INARGTYPE element2 )
        {
            return( (*element1 == *element2) != 0 ); 
        }

        static int CompareElementsOrdered( INARGTYPE element1, INARGTYPE element2 )
        {
            return element1 ->Compare (*element2);
        }
};
#endif
#endif // ETS_DATAKEY_H

/* vim: set sw=4: */
