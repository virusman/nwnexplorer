#ifndef ETS_PLACEABLEOBJECT_H
#define ETS_PLACEABLEOBJECT_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      PlaceableObject.h - Placeable object |
//
// This module contains the definition of the placeable object.
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

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CPlaceableObject : public CTableItemObject
{
    public:
        enum Element
        {
            Unknown                     = 0,
            Placeable           = 1,
            Chance                      = 2,
            Name                        = 3,
        };

        // @access Constructors and destructors
    public:

        // @cmember General constructor

        CPlaceableObject ();

        // @cmember General destructor

        virtual ~CPlaceableObject ();

        // @access Public virtual methods
    public:

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
            return Object_Type_Placeable;
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

            fprintf (fp, _T ("  <Placeable>\r\n"));
            fprintf (fp, _T ("   <Chance>%d</Chance>\r\n"), m_nChance);  
            fprintf (fp, _T ("   <Name>%s</Name>\r\n"), T2CA (m_strName));  
            fprintf (fp, _T ("  </Placeable>\r\n"));
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

        // @access Protected methods
    protected:

        // @cmember Get the XML element number

        Element GetElement (const XML_Char *pszName);

        // @access Protected methods
    protected:

};

#endif // ETS_PLACEABLEOBJECT_H

/* vim: set sw=4: */
