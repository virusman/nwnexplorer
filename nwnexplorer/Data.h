#ifndef ETS_DATA_H
#define ETS_DATA_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      Data.h - Nwn data support |
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

#include "DataKey.h"
#include "DataCache.h"

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

class CData : public CDataKey 
{

    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CData (CDataCache *pDataCache, CNwnKeyFile *pKeyFile, 
               CNwnModuleFile *pModuleFile, CNwnBifFile *pBifFile, 
               const char *pszName, NwnResType nResType, 
               CDataKey::Type nType, unsigned char *pauchExtraData, 
               int nExtraDataSize, unsigned char *pauchData, unsigned long ulSize, 
               bool fAllocated) : 
            CDataKey (pKeyFile, pModuleFile, pBifFile, pszName, nResType, nType, 
                      pauchExtraData, nExtraDataSize)
    {
        m_pDataCache = pDataCache;
        m_pauchData = NULL;
        m_ulSize = 0;
        m_ulRefCount = 0;
        SetData (pauchData, ulSize, fAllocated);
    }

        // @cmember General destructor

        ~CData ()
        {
            if (m_pDataCache != NULL)
                m_pDataCache ->DataReleased (this);
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
            if (!fAllocated)
            {
                m_pauchData = (unsigned char *) malloc (ulSize);
                memmove (m_pauchData, pauchData, ulSize);
            }
            else
                m_pauchData = pauchData;
            m_ulSize = ulSize;
            m_fAllocated = true;
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
            _ASSERTE (m_pauchData != NULL);
            // note: to simplify the coding of some routines, we are
            // allowed to return a pointer to the end of the data
            _ASSERTE (nIndex >= 0 && nIndex <= m_ulSize);
            return &m_pauchData [nIndex];
        }

        // @cmember Get the size

        unsigned long GetSize () 
        {
            return m_ulSize;
        }

        // @cmember Add a reference

        unsigned long AddRef ()
        {
            m_ulRefCount++;
            return m_ulRefCount;
        }

        // @cmember Remove a reference

        unsigned long Release ()
        {
            m_ulRefCount--;
            unsigned long ulRefCount = m_ulRefCount;
            if (m_ulRefCount == 0)
                delete this;
            return ulRefCount;
        }

        // @cmember Save the file

        bool SaveToFile (const char *pszFileName)
        {
            FILE *fp = fopen (pszFileName, "wb");
            if (fp == NULL)
                return false;
            bool fResults = fwrite (m_pauchData, m_ulSize, 1, fp) == 1;
            fclose (fp);
            return fResults;
        }

        // @access Protected methods
    protected:

        // @cmember Pointer to the owning data cache

        CDataCache                      *m_pDataCache;

        // @cmember Pointer to the data

        unsigned char           *m_pauchData;

        // @cmember Size of the data

        unsigned long           m_ulSize;

        // @cmember If true, data is allocated

        bool                            m_fAllocated;

        // @cmember Reference count

        unsigned long           m_ulRefCount;
};

#endif // ETS_DATA_H

/* vim: set sw=4: */
