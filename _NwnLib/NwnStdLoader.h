#ifndef ETS_NWNSTDLOADER_H
#define ETS_NWNSTDLOADER_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnStdLoader.h - NWN game file loader |
//
// This module contains the definition of the NwnStdLoader.  It provides a
// method of requesting resources from NWN.
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
#include "NwnKeyFile.h"
#include "NwnArray.h"
#include "NwnLoader.h"
#include <list>

//-----------------------------------------------------------------------------
//
// Type definitions
//
//-----------------------------------------------------------------------------
typedef std::list<std::string> DirSearchList;
typedef std::list<std::string>::const_iterator DirSearchListConstIter;

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

class CNwnModuleFile;
class CNwnHierarchy;

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CNwnStdLoader : public CNwnLoader
{

    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CNwnStdLoader ();

        // @cmember Delete the streams

        virtual ~CNwnStdLoader ();

        // @access Public methods
    public:

        // @cmember Initialize the loader

        bool Initialize (const char *pszNwnDir = NULL);

        // @cmember Load a resource

        virtual unsigned char *LoadResource (const char *pszName, 
                                             NwnResType nResType, UINT32 *pulSize, bool *pfAllocated);

        // @cmember Open a module

        bool OpenModule (CNwnModuleFile *pModule, const char *pszName);

        // @cmember Load the hak packs for the module

        bool AddModuleHaks ();

        // @cmember Add a hak file 

        bool AddHak (const char *pszHak);

        // @access Public inline methods
    public:

        // @cmember Get the number of key files

        size_t GetKeyFileCount () const
        {
            return _countof (m_asKeyFiles);
        }

        // @cmember Get the n'th key file

        CNwnKeyFile *GetNthKeyFile (int nKeyFile)
        {
            return &m_asKeyFiles [nKeyFile];
        }

        // @cmember Close the loader

        void Close ()
        {
            for (int i = 0; i < _countof (m_asKeyFiles); i++)
                m_asKeyFiles [i] .Close ();
        }

        // @cmember Set override enable

        void SetOverride (bool fEnable)
        {
            m_fOverride = fEnable;
        }

        // @cmember Get the override enable

        bool GetOverride () const
        {
            return m_fOverride;
        }

        // @cmember Set the module

        void SetModule (CNwnModuleFile *pModule)
        {
            m_pModule = pModule;
        }

        // @cmember Set the default directory

        void SetDefaultDir (const char *pszDirectory)
        {
            m_strDefaultDir = pszDirectory;
        }

        void AddSearchDir (std::string dir) {
            if (m_searchList == NULL)
                m_searchList = new DirSearchList();

            std::string::reverse_iterator pos = dir.rbegin();
            if (*pos != '\\')
                dir += '\\';

            m_searchList->push_front(dir);
        }

        // @access Protected methods
    protected:

        // @cmember Return the address of NWN from the registry

        const char *GetNwnDirectory ();

        // @cmember Look for a hak file setting

        bool LookForHak (CNwnHierarchy &ifo, int nEntry);

        // @cmember Protected members
    protected:

        // @cmember NWN directory

        std::string                             m_strRoot;

        // @cmember NWN override directory

        std::string                             m_strOverride;

        // @cmember NWN module directory

        std::string                             m_strModule;

        // @cmember NWN hak directory

        std::string                             m_strHak;

        // @cmember Default directory

        std::string                             m_strDefaultDir;
        DirSearchList*                  m_searchList;

        // @cmember NWN key file

        CNwnKeyFile                             m_asKeyFiles [7];

        // @cmember If true, override is enabled

        bool                                    m_fOverride;

        // @cmember Current module file

        CNwnModuleFile                  *m_pModule;

        // @cmember Array of hak files

        CNwnArray <CNwnModuleFile *> m_apHaks;
};

#endif // ETS_NWNLOADER_H

/* vim: set sw=4: */
