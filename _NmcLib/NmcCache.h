#ifndef ETS_NMCCACHE_H
#define ETS_NMCCACHE_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NmcCache.h - Model/Texture cache |
//
// This module contains the definition of the NmcCache.
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
#include <map>

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

class CNwnMdlModel;

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CNmcCache
{

    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CNmcCache ();

        // @cmember Delete the streams

        ~CNmcCache ();

        // @access Public methods
    public:

        // @cmember Find a model in the cache

        CNwnMdlModel *FindModel (const char *pszName);

        // @cmember Add a model to the cache

        void AddModel (CNwnMdlModel *pModel, const char *pszName);

        // @cmember Find a texture in the cache

        bool FindTexture (const char *pszName, bool *pfBumpmapped);

        // @cmember Add a texture to the cache

        void AddTexture (const char *pszName, bool fBumpmapped);

        // @cmember Clear the cache

        void ClearCache ();

        // @cmember Protected members
    protected:

        // @cmember Model cache

        std::map <std::string, CNwnMdlModel *> m_mapModel;

        // @cmember Texture cache

        std::map <std::string, bool> m_mapTexture;
};

#endif // ETS_NMCCACHE_H

/* vim: set sw=4: */
