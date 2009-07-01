#ifndef ETS_DATAPARSER_H
#define ETS_DATAPARSER_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      DataParser.h - Data parser |
//
// This module contains the definition of the data parser
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
// $History: ExpatImpl.h $
//      
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Required include files
//
//-----------------------------------------------------------------------------

#include <assert.h>
#include "ExpatImpl.h"
#include "nwntreasure.h"
#include "TableObject.h"
#include "GoldObject.h"
#include "NoneObject.h"
#include "ItemObject.h"
#include "TableRefObject.h"
#include "EncounterObject.h"
#include "PlaceableObject.h"
#include "ProfileObject.h"
#include "MainWnd.h"

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Template class definition
//
//-----------------------------------------------------------------------------

class CDataParser : public CExpatImpl <CDataParser> 
{
    public:

        enum State
        {
            Start                       = 0,
            InData                      = 1,
            InTable                     = 2,
            InTableRef          = 3,
            InGold                      = 4,
            InNone                      = 5,
            InItem                      = 6,
            InEncounter         = 7,
            InPlaceable         = 8,
            InProfile           = 9,
        };

    public:

        // Constructor 

        CDataParser (CMainWnd *pMainWnd) 
        {
            m_pMainWnd = pMainWnd;
            m_nState = Start;
            m_nTextLength = 0;
            m_pTable = NULL;
            m_pItem = NULL;
        }

        // Invoked by CExpatImpl after the parser is created

        void OnPostCreate ()
        {
            // Enable all the event routines we want
            EnableStartElementHandler ();
            EnableEndElementHandler ();
            // Note: EnableElementHandler will do both start and end
            EnableCharacterDataHandler ();
        }

        // @cmember Start element handler

        void OnStartElement (const XML_Char *pszName, const XML_Char **papszAttrs)
        {

            //
            // If we have already failed, return
            //

            if (!m_strError .IsEmpty ())
                return;

            //
            // Reset the text length
            //

            m_nTextLength = 0;

            //
            // Switch based on the state
            //

            switch (m_nState)
            {

                //
                // If this is the start element
                //

                case Start:

                    //
                    // If the data element
                    //

                    if (stricmp (pszName, "nwtreasure-data") == 0 ||
                        stricmp (pszName, "nwntreasure-data") == 0)
                    {

                        //
                        // No attributes are allowed
                        //

                        if (papszAttrs [0] != NULL)
                        {
                            SetError (IDS_ERR_XML_NO_ATTRS);
                            return;
                        }

                        //
                        // Set the state
                        //

                        m_nState = InData;
                    }

                    //
                    // Otherwise, all other elements are invalid
                    //

                    else
                    {
                        SetError (IDS_ERR_XML_UNEXPECTED_ELEMENT);
                        return;
                    }
                    break;

                    //
                    // If we are already in data
                    //

                case InData:
                    {
                        if (stricmp (pszName, "Table") == 0)
                        {
                            m_pTable = new CTableObject ();
                            m_pTable ->SetTableType (Table_Type_Treasure);
                            m_nState = InTable;
                        }
                        else
                        {
                            SetError (IDS_ERR_XML_UNEXPECTED_ELEMENT);
                            return;
                        }
                    }
                    break;

                case InTable:
                    m_pTable ->OnStartElement (this, pszName, papszAttrs);
                    break;

                case InTableRef:
                case InGold:
                case InNone:
                case InItem:
                case InEncounter:
                case InPlaceable:
                case InProfile:
                    m_pItem ->OnStartElement (this, pszName, papszAttrs);
                    break;
            }
            return;
        }

        // End element handler

        void OnEndElement (const XML_Char *pszName)
        {

            //
            // If we have already failed, return
            //

            if (!m_strError .IsEmpty ())
                return;

            //
            // Switch based on the state
            //

            switch (m_nState)
            {
                case Start:
                    SetError (IDS_ERR_XML_UNEXPECTED);
                    return;

                case InData:
                    if (stricmp (pszName, "nwtreasure-data") == 0 ||
                        stricmp (pszName, "nwntreasure-data") == 0)
                    {
                        m_nState = Start;
                    }
                    else
                    {
                        SetError (IDS_ERR_XML_UNEXPECTED_ELEMENT);
                        return;
                    }
                    break;

                case InTable:
                    m_pTable ->OnEndElement (this, pszName);
                    break;

                case InTableRef:
                case InGold:
                case InNone:
                case InItem:
                case InEncounter:
                case InPlaceable:
                case InProfile:
                    m_pItem ->OnEndElement (this, pszName);
                    break;
            }
            return;
        }

        // Character data handler

        void OnCharacterData (const XML_Char *pszData, int nLength)
        {

            //
            // If we have already failed, return
            //

            if (!m_strError .IsEmpty ())
                return;

            //
            // Add the text
            //

            if (nLength + m_nTextLength > _countof (m_szText) - 1)
                nLength = _countof (m_szText) - 1 - m_nTextLength;
            memcpy (&m_szText [m_nTextLength], pszData, 
                    nLength * sizeof (XML_Char));
            m_nTextLength += nLength;
            return;
        }

        // @access Public methods
    public:

        // @cmember Set the current state

        void SetState (State nState)
        {
            m_nState = nState;
        }

        // @cmember Return the text 

        XML_Char *GetText (bool fRemoveWhitespace)
        {
            if (fRemoveWhitespace)
                RemoveWhitespace ();
            else
                m_szText [m_nTextLength] = 0;
            return m_szText;
        }

        // @cmember Set the error string

        void SetError (LPCTSTR pszError)
        {
            m_strError = pszError;
            m_nLineNumber = GetCurrentLineNumber ();
            m_nColumnNumber = GetCurrentColumnNumber ();
        }

        // @cmember Set the error string

        void SetError (UINT nID)
        {
            m_strError .LoadString (nID);
            m_nLineNumber = GetCurrentLineNumber ();
            m_nColumnNumber = GetCurrentColumnNumber ();
        }

        // @cmember Save the current table

        void SaveTable ()
        {
            m_pMainWnd ->AddTable (m_pTable);
        }

        // @cmember Save the current item

        void SaveItem ()
        {
            m_pTable ->AddItem (m_pItem);
        }

        // @cmember Set a new item

        void SetItem (CTableItemObject *pItem)
        {
            m_pItem = pItem;
        }

        // @access Protected methods
    protected:

        // @cmember Remove whitespace

        void RemoveWhitespace ()
        {
            int nOut = 0;
            bool fHaveWhite = true;
            int nIn = 0;
            while (nIn < m_nTextLength)
            {
                XML_Char c = m_szText [nIn++];
                if (isspace (c))
                {
                    if (!fHaveWhite)
                    {
                        fHaveWhite = true;
                    }
                }
                else
                {
                    if (fHaveWhite)
                    {
                        fHaveWhite = false;
                        if (nOut != 0)
                            m_szText [nOut++] = ' ';
                    }
                    m_szText [nOut++] = c;
                }
            }
            m_szText [nOut] = 0;
            m_nTextLength = nOut;
        }

        // @access Public data
    public:

        // @cmember Current state

        State                           m_nState;

        // @cmember Current text buffer

        XML_Char                        m_szText [1024]; //FIXME

        // @cmember Current text length

        int                                     m_nTextLength;

        // @cmember If set, the process has failed

        CString                         m_strError;

        // @cmember Error line number

        int                                     m_nLineNumber;

        // @cmember column number

        int                                     m_nColumnNumber;

        // @cmember Main window

        CMainWnd                        *m_pMainWnd;

        // @cmember Current table

        CTableObject            *m_pTable;

        // @cmember Current item

        CTableItemObject        *m_pItem;
};

#endif // ETS_DATAPARSER_H

/* vim: set sw=4: */
