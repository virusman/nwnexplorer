//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ProfileObject.cpp - Profile object |
//
// This module contains the profile object.
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
// $History: Cnf.cpp $
//      
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"
#include "ProfileObject.h"
#include "MainWnd.h"
#include "DataParser.h"

//
// Debug NEW
//

#if defined (_DEBUG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//-----------------------------------------------------------------------------
//
// @mfunc <c CProfileObject> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CProfileObject::CProfileObject ()
{
    m_nRespawnType = Respawn_Type_None;
    m_nRespawnTime = 5 * 60;
    m_nDeathOpenDamage = 0;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CProfileObject> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CProfileObject::~CProfileObject ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the name of the object
//
// @parm CMainWnd * | pMainWnd | Main window
// 
// @parm LPTSTR | pszName | Name of the object
//
// @parm int | cchMaxText | Size of the buffer
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CProfileObject::GetDescription (CMainWnd *pMainWnd,
                                     LPTSTR pszName, int cchMaxText)
{
    //
    // Get the name
    //

    _tcsncpy (pszName, m_strName, cchMaxText);
    pszName [cchMaxText - 1] = 0;
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the object image
//
// @rdesc Image index.
//
//-----------------------------------------------------------------------------

Object_Image CProfileObject::GetImage ()
{
    return Object_Image_Profile;
}

//-----------------------------------------------------------------------------
//
// @mfunc Write the routine body
//
// @parm CMainWnd * | pMainWnd | Main window
// 
// @parm FILE * | fp | Output file
//
// @parm bool | fDebug | If true, write debug statements
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CProfileObject::WriteRoutineBody (CMainWnd *pMainWnd, FILE *fp, bool fDebug)
{
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Write profile name to number body
//
// @parm FILE * | fp | Output file
//
// @parm bool | fFirst | If true, this is the first one
//
// @rdesc TRUE if a line was output.
//
//-----------------------------------------------------------------------------

bool CProfileObject::WriteProfileTest (FILE *fp, bool fFirst)
{
    TCHAR szText [256];
    _tcsncpy (szText, m_strName, _countof (szText));
    LPTSTR psz = _tcschr (szText, '*');
    if (!fFirst)
        fprintf (fp, "  else\r\n");
    if (psz)
        *psz = 0;
    if (psz == NULL)
    {
        fprintf (fp, "  if (sTreasureSet == \"%s\")\r\n", szText);
    }
    else 
    {
        fprintf (fp, "  if (MatchPartialProfileName (sTreasureSet, \"%s\", \"%s\"))\r\n", 
                 szText, psz + 1);
    }
    fprintf (fp, "  {\r\n");
    if (m_nRespawnType != 0)
    {
        fprintf (fp, "    nRespawnStyle = %d;\r\n", m_nRespawnType);
        if (!m_strRespawnName .IsEmpty ())
            fprintf (fp, "    sRespawnSetAlias = \"%s\";\r\n", T2CA (m_strRespawnName));
        if (m_nRespawnTime != 0)
            fprintf (fp, "    nRespawnTime = %d;\r\n", m_nRespawnTime);
    }
    if (!m_strDeathOpenSpawn .IsEmpty ())
        fprintf (fp, "    sDeathOpenSpawn = \"%s\";\r\n", T2CA (m_strDeathOpenSpawn));
    if (!m_strDeathOpenSound .IsEmpty ())
        fprintf (fp, "    sDeathOpenSound = \"%s\";\r\n", T2CA (m_strDeathOpenSound));
    if (m_nDeathOpenDamage != 0)
        fprintf (fp, "    nDeathOpenDamage = %d;\r\n", m_nDeathOpenDamage);
    if (!m_strTreasureTable .IsEmpty ())
        fprintf (fp, "    sTreasureSetAlias = \"%s\";\r\n", T2CA (m_strTreasureTable));
    fprintf (fp, "  }\r\n");
    return true;
}

//-----------------------------------------------------------------------------
//
// @mfunc Begin the processing of an XML element
//
// @parm CDataParser * | pParser | Current parser
//
// @parm const XML_Char * | pszName | Name of the element
//
// @parm const XML_Char ** | papszAttrs | Attributes
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CProfileObject::OnStartElement (CDataParser *pParser, 
                                     const XML_Char *pszName, const XML_Char **papszAttrs)
{
    //
    // Get the element
    //

    Element nElement = GetElement (pszName);
    if (nElement == Unknown)
    {
        pParser ->SetError (IDS_ERR_XML_UNEXPECTED_ELEMENT);
        return;
    }

    //
    // Otherwise
    //

    else
    {

        //
        // No attributes are allowed
        //

        if (papszAttrs [0] != NULL)
        {
            pParser ->SetError (IDS_ERR_XML_NO_ATTRS);
            return;
        }
    }
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc End an XML element
//
// @parm CDataParser * | pParser | Current parser
//
// @parm const XML_Char * | pszName | Name of the element
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CProfileObject::OnEndElement (CDataParser *pParser, const XML_Char *pszName)
{

    //
    // Get the element
    //

    Element nElement = GetElement (pszName);
    if (nElement == Unknown)
    {
        pParser ->SetError (IDS_ERR_XML_UNEXPECTED_ELEMENT);
        return;
    }

    //
    // Switch based on the element
    //

    switch (nElement)
    {

        //
        // If we are ending the Table
        //

        case Profile:
            pParser ->SetState (CDataParser::InTable);
            pParser ->SaveItem ();
            break;

            //
            // If this is the name
            //

        case Name:
            m_strName = pParser ->GetText (false);
            break;

            //
            // If this is the respawn type
            //

        case RespawnType:
            m_nRespawnType = (Respawn_Type) _ttol (pParser ->GetText (false));
            break;

            //
            // If this is the respawn time
            //

        case RespawnTime:
            m_nRespawnTime = _ttol (pParser ->GetText (false));
            break;

            //
            // If this is the respawn name
            //

        case RespawnName:
            m_strRespawnName = pParser ->GetText (false);
            break;

            //
            // If this is the death/open space name
            //

        case DeathOpenSpawn:
            m_strDeathOpenSpawn = pParser ->GetText (false);
            break;

            //
            // If this is the death/open sound name
            //

        case DeathOpenSound:
            m_strDeathOpenSound = pParser ->GetText (false);
            break;

            //
            // If this is the death/open damage name
            //

        case DeathOpenDamage:
            m_nDeathOpenDamage = _ttol (pParser ->GetText (false));
            break;

            //
            // If this is the treasure table name
            //

        case TreasureTable:
            m_strTreasureTable = pParser ->GetText (false);
            break;
    }
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the element number
//
// @parm const XML_Char * | pszName | Element name
//
// @rdesc Element id.
//
//-----------------------------------------------------------------------------

CProfileObject::Element CProfileObject::GetElement (const XML_Char *pszName)
{
    if (stricmp (pszName, "Profile") == 0)
        return Profile;
    else if (stricmp (pszName, "Name") == 0)
        return Name;
    else if (stricmp (pszName, "RespawnType") == 0)
        return RespawnType;
    else if (stricmp (pszName, "RespawnTime") == 0)
        return RespawnTime;
    else if (stricmp (pszName, "RespawnName") == 0)
        return RespawnName;
    else if (stricmp (pszName, "DeathOpenSpawn") == 0)
        return DeathOpenSpawn;
    else if (stricmp (pszName, "DeathOpenSound") == 0)
        return DeathOpenSound;
    else if (stricmp (pszName, "DeathOpenDamage") == 0)
        return DeathOpenDamage;
    else if (stricmp (pszName, "TreasureTable") == 0)
        return TreasureTable;
    else
        return Unknown;
}


/* vim: set sw=4: */
