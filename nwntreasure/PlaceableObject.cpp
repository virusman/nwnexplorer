//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      PlaceableObject.cpp - Placeable object |
//
// This module contains the placeable object.
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
#include "PlaceableObject.h"
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
// @mfunc <c CPlaceableObject> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CPlaceableObject::CPlaceableObject ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CPlaceableObject> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CPlaceableObject::~CPlaceableObject ()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the object image
//
// @rdesc Image index.
//
//-----------------------------------------------------------------------------

Object_Image CPlaceableObject::GetImage ()
{
    return Object_Image_Placeable;
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

void CPlaceableObject::OnStartElement (CDataParser *pParser, 
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

void CPlaceableObject::OnEndElement (CDataParser *pParser, const XML_Char *pszName)
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

        case Placeable:
            pParser ->SetState (CDataParser::InTable);
            pParser ->SaveItem ();
            break;

            //
            // If this is the chance
            //

        case Chance:
            m_nChance = atol (pParser ->GetText (false));
            break;

            //
            // If this is the name
            //

        case Name:
            m_strName = pParser ->GetText (false);
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

CPlaceableObject::Element CPlaceableObject::GetElement (const XML_Char *pszName)
{
    if (stricmp (pszName, "Placeable") == 0)
        return Placeable;
    else if (stricmp (pszName, "Chance") == 0)
        return Chance;
    else if (stricmp (pszName, "Name") == 0)
        return Name;
    else
        return Unknown;
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

void CPlaceableObject::WriteRoutineBody (CMainWnd *pMainWnd, FILE *fp, bool fDebug)
{
    if (!m_strName .IsEmpty ())
    {
        fprintf (fp, 
                 "  CreateDelayProfileSpawn (g_fDelay, \"%s\", OBJECT_TYPE_PLACEABLE, "
                 "g_loc, g_nSingleShot, g_sProfile);\r\n", 
                 (LPCTSTR) m_strName);
    }            
    return;
}

/* vim: set sw=4: */
