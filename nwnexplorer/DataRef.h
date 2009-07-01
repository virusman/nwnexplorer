#ifndef ETS_DATAREF_H
#define ETS_DATAREF_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      DataRef.h - Data reference support |
//
// This module contains the definition of the data reference object.
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

#include "Data.h"
#include "../_NwnLib/NwnModel.h"
#include "../_NwnLib/NwnHierarchy.h"
#include "../_NwnLib/NwnSoundSet.h"
#include "../_NwnLib/NwnTexturePlt.h"

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

class CDataRef
{
    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CDataRef ()
        {
            m_p = NULL;
        }

        // @cmember Assignment constructor

        CDataRef (CData *p)
        {
            if ((m_p = p) != NULL)
                p ->AddRef ();
        }

        // @cmember Copy constructor

        CDataRef (const CDataRef &p)
        {
            if ((m_p = p .m_p) != NULL)
                m_p ->AddRef ();
        }

        // @cmember General destructor

        ~CDataRef ()
        {
            if (m_p)
                m_p ->Release ();
        }

        // @access Public member variable
    public:

        // @cmember Release the interface

        void Release ()
        {
            CData *p = m_p;
            if (p)
            {
                m_p = NULL;
                p ->Release ();
            }
        }

        // @cmember Get the pointer

        operator CData * () const
        {
            return m_p;
        }

        // @cmember Dereference the pointer

        CData & operator * () const
        {
            assert (m_p != NULL);
            return *m_p;
        }

        // @cmember Access via the member pointer

        CData * operator -> () const
        {
            assert (m_p != NULL);
            return m_p;
        }

        // @cmember Set the pointer

        void Set (CData *p)
        {
            if (p)
                p ->AddRef ();
            if (m_p)
                m_p ->Release ();
            m_p = p;
        }

        // @cmember Copy the pointer

        CData *operator = (CData *p)
        {
            Set (p);
            return m_p;
        }

        // @cmember Copy the pointer

        CData *operator = (const CDataRef &p)
        {
            Set (p .m_p);
            return m_p;
        }

        // @cmember Is pointer set

        bool operator ! () const
        {
            return (m_p == NULL);
        }

        // @cmember Is pointer less 

        bool operator < (CData *p) const
        {
            return m_p < p;
        }

        // @cmember Is pointer equal

        bool operator == (CData *p) const
        {
            return m_p == p;
        }

        void Attach (CData *p)
        {
            if (m_p)
                m_p ->Release ();
            m_p = p;
        }

        CData *Detach ()
        {
            CData *p = m_p;
            m_p = NULL;
            return p;
        }

        bool CopyTo (CData **pp)
        {
            assert (pp != NULL);
            if (pp == NULL)
                return false;
            *pp = m_p;
            if (m_p)
                m_p ->AddRef ();
            return true;
        }

        // @cmember Get a pointer to the data

        unsigned char *GetData (int nIndex = 0) 
        {
            assert (m_p != NULL);
            if (m_p == NULL)
                return NULL;
            return m_p ->GetData (nIndex);
        }

        // @cmember Get the size

        unsigned long GetSize () 
        {
            assert (m_p != NULL);
            if (m_p == NULL)
                return 0;
            return m_p ->GetSize ();
        }

        // @access Public member variable
    public:

        // @cmember Pointer to the object

        CData           *m_p;
};

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CResModelRef :
    public CNwnModelImpl <CResModelRef>,
    public CDataRef
{
    public:
        CResModelRef ()
        {
        }
};

class CResHierarchyRef :
    public CNwnHierarchyImpl <CResHierarchyRef>,
    public CDataRef
{
    public:
        CResHierarchyRef ()
        {
        }
};

class CResSoundSetRef :
    public CNwnSoundSetImpl <CResSoundSetRef>,
    public CDataRef
{
    public:
        CResSoundSetRef ()
        {
        }
};

class CResTexturePltRef :
    public CNwnTexturePltImpl <CResTexturePltRef>,
    public CDataRef
{
    public:
        CResTexturePltRef ()
        {
        }
};

#endif // ETS_DATAREF_H

/* vim: set sw=4: */
