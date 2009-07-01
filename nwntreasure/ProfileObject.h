#ifndef ETS_PROFILEOBJECT_H
#define ETS_PROFILEOBJECT_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ProfileObject.h - Profile object |
//
// This module contains the definition of the profile object.
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

#include "TableItemObject.h"

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

enum Respawn_Type
{
    Respawn_Type_None           = 0,
    Respawn_TypeResourcepawn    = 1,
};

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CProfileObject : public CTableItemObject
{
    public:
        enum Element
        {
            Unknown                     = 0,
            Profile                     = 1,
            Name                        = 2,
            RespawnType         = 3,
            RespawnTime         = 4,
            RespawnName         = 5,
            DeathOpenSpawn      = 6,
            DeathOpenSound      = 7,
            DeathOpenDamage = 8,
            TreasureTable       = 9,
        };

        // @access Constructors and destructors
    public:

        // @cmember General constructor

        CProfileObject ();

        // @cmember General destructor

        virtual ~CProfileObject ();

        // @access Public methods
    public:

        // @cmember Write profile name to number body

        bool WriteProfileTest (FILE *fp, bool fFirst);

        // @access Public virtual methods
    public:

        // @cmember Get the name of the object

        virtual void GetDescription (CMainWnd *pMainWnd, 
                                     LPTSTR pszName, int cchMaxText);

        // @cmember Get the image

        virtual Object_Image GetImage ();

        // @cmember Get the full text of object

        virtual CString GetFullName (CMainWnd *pMainWnd)
        {
            return m_strName;
        }

        // @cmember Write the routine body

        virtual void WriteRoutineBody (CMainWnd *pMainWnd, FILE *fp, bool fDebug);

        // @cmember Get the object type

        virtual Object_Type GetObjectType () const
        {
            return Object_Type_Profile;
        }

        // @cmember Compare two objects

        virtual bool Compare (const CObject *pRhs) const
        {
            return false;
        }

        // @cmember Can this object be edited

        virtual bool CanEdit () const
        {
            return true;
        }

        // @cmember Write an XML file

        virtual void WriteXML (FILE *fp)
        {
            USES_CONVERSION;

            fprintf (fp, _T ("  <Profile>\r\n"));
            fprintf (fp, _T ("   <Name>%s</Name>\r\n"), T2CA (m_strName));  
            fprintf (fp, _T ("   <RespawnType>%d</RespawnType>\r\n"), m_nRespawnType);  
            fprintf (fp, _T ("   <RespawnTime>%d</RespawnTime>\r\n"), m_nRespawnTime);  
            fprintf (fp, _T ("   <RespawnName>%s</RespawnName>\r\n"), T2CA (m_strRespawnName));  
            fprintf (fp, _T ("   <DeathOpenSpawn>%s</DeathOpenSpawn>\r\n"), T2CA (m_strDeathOpenSpawn));  
            fprintf (fp, _T ("   <DeathOpenSound>%s</DeathOpenSound>\r\n"), T2CA (m_strDeathOpenSound));  
            fprintf (fp, _T ("   <DeathOpenDamage>%d</DeathOpenDamage>\r\n"), m_nDeathOpenDamage);  
            fprintf (fp, _T ("   <TreasureTable>%s</TreasureTable>\r\n"), T2CA (m_strTreasureTable));  
            fprintf (fp, _T ("  </Profile>\r\n"));
            return;
        }

        // @cmember Start an XML element

        virtual void OnStartElement (CDataParser *pParser, 
                                     const XML_Char *pszName, const XML_Char **papszAttrs);

        // @cmember End an XML element

        virtual void OnEndElement (CDataParser *pParser, 
                                   const XML_Char *pszName);

        // @access Public inline methods
    public:

        // @cmember Get the respawn type

        Respawn_Type GetRespawnType () const
        {
            return m_nRespawnType;
        }

        // @cmember Set the respawn type

        void SetRespawnType (Respawn_Type nRespawnType)
        {
            m_nRespawnType = nRespawnType;
        }

        // @cmember Get the respawn time

        int GetRespawnTime () const
        {
            return m_nRespawnTime;
        }

        // @cmember Set the respawn time

        void SetRespawnTime (int nRespawnTime)
        {
            m_nRespawnTime = nRespawnTime;
        }

        // @cmember Get the respawn name

        const CString &GetRespawnName () const
        {
            return m_strRespawnName;
        }

        // @cmember Set the respawn name

        void SetRespawnName (const CString &strRespawnName)
        {
            m_strRespawnName = strRespawnName;
        }

        // @cmember Get the DeathOpenSpawn

        const CString &GetDeathOpenSpawn () const
        {
            return m_strDeathOpenSpawn;
        }

        // @cmember Set the DeathOpenSpawn

        void SetDeathOpenSpawn (const CString &strDeathOpenSpawn)
        {
            m_strDeathOpenSpawn = strDeathOpenSpawn;
        }

        // @cmember Get the DeathOpenSound

        const CString &GetDeathOpenSound () const
        {
            return m_strDeathOpenSound;
        }

        // @cmember Set the DeathOpenSound

        void SetDeathOpenSound (const CString &strDeathOpenSound)
        {
            m_strDeathOpenSound = strDeathOpenSound;
        }

        // @cmember Get the DeathOpenDamage

        int GetDeathOpenDamage () const
        {
            return m_nDeathOpenDamage;
        }

        // @cmember Set the DeathOpenDamage

        void SetDeathOpenDamage (int nDeathOpenDamage)
        {
            m_nDeathOpenDamage = nDeathOpenDamage;
        }

        // @cmember Get the TreasureTable

        const CString &GetTreasureTable () const
        {
            return m_strTreasureTable;
        }

        // @cmember Set the TreasureTable

        void SetTreasureTable (const CString &strTreasureTable)
        {
            m_strTreasureTable = strTreasureTable;
        }

        // @access Protected methods
    protected:

        // @cmember Get the XML element number

        Element GetElement (const XML_Char *pszName);

        // @access Protected methods
    protected:

        // @cmember Respawn type

        Respawn_Type                    m_nRespawnType;

        // @cmember Respawn time

        int                                             m_nRespawnTime;

        // @cmember Respawn name

        CString                                 m_strRespawnName;

        // @cmember Death/Open spawn

        CString                                 m_strDeathOpenSpawn;

        // @cmember Death/Open sound

        CString                                 m_strDeathOpenSound;

        // @cmember Death/Open damage

        int                                             m_nDeathOpenDamage;

        // @cmember Treasure table

        CString                                 m_strTreasureTable;
};

#endif // ETS_PROFILEOBJECT_H

/* vim: set sw=4: */
