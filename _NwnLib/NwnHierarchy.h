#ifndef ETS_NWNHIERARCHY_H
#define ETS_NWNHIERARCHY_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnHierarchy.h - Nwn hierarchy (assorted) file support |
//
// This module contains the definition of the NwnHierarchy.
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
#include "NwnMemoryFile.h"

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

class CNwnDialogTlkFile;

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

template <class TClass>
class CNwnHierarchyImpl
{
    public:

        enum ElementType
        {
            ElementType_UINT8           = 0,
            ElementType_INT8            = 1,
            ElementType_UINT16          = 2,
            ElementType_INT16           = 3,
            ElementType_UINT32          = 4,
            ElementType_INT32           = 5,
            ElementType_UINT64          = 6,
            ElementType_INT64           = 7,
            ElementType_FLOAT           = 8,
            ElementType_DOUBLE          = 9,
            ElementType_STRING          = 10,
            ElementType_RESREF          = 11,
            ElementType_STRREF          = 12,
            ElementType_DATREF          = 13,
            ElementType_CAPREF          = 14,
            ElementType_LIST            = 15,
        };

        struct Header 
        {
            char        achSig [4];
            char        achVersion [4];
            UINT32      ulEntryOffset;
            UINT32      ulEntryCount;
            UINT32      ulElementOffset;
            UINT32      ulElementCount;
            UINT32      ulVarNameOffset;
            UINT32      ulVarNameCount;
            UINT32      ulVarDataOffset;
            UINT32      ulVarDataSize;
            UINT32      ulMultiMapOffset;
            UINT32      ulMultiMapSize;
            UINT32      ulListOffset;
            UINT32      ulListSize;
        };

        struct Entry
        {
            UINT32      ulCode;
            UINT32      ulOffset;
            UINT32      ulCount;
        };

        struct Element
        {
            UINT32      ulType;
            UINT32      ulVarName;
            union
            {
                UINT8   u8Data;
                UINT16  u16Data;
                UINT32  u32Data;
                INT8    n8Data;
                INT16   n16Data;
                INT32   n32Data;
                float   fData;
            };
        };

        struct VarName
        {
            char        szName [16];
        };

        struct StrRef
        {
            UINT32      ulSize;                         // Not including myself
            UINT32      ulStrRef;                       // Default strref
            UINT32      ulLangSpecCount;        // Number of language specific entries

            //
            // If "ulLangSpecCount" is not zero, then "ulLangSpecCount" number
            // of StrRefLangSpec follow the StrRef in memory.
            //
        };

        struct StrRefLangSpec
        {
            UINT32      ulLanguage;                     //
            UINT32      ulLength;                       // Length of the following data
            char        szText [1];                     // Text of the data
        };

        // @access Public inline methods
    public:

        // @cmember Get the header

        Header *GetHeader () 
        {
            TClass *pT = static_cast <TClass *> (this);
            return (Header *) pT ->GetData (0);
        }

        // @cmember Get the entry count

        int GetEntryCount () 
        {
            Header *pHeader = GetHeader ();
            return (int) pHeader ->ulEntryCount;
        }

        // @cmember Get the n'th entry

        Entry *GetEntry (int nIndex) 
        {
            TClass *pT = static_cast <TClass *> (this);
            Header *pHeader = GetHeader ();
            Entry *pasEntry = (Entry *) 
                pT ->GetData (pHeader ->ulEntryOffset);
            return &pasEntry [nIndex];
        }

        // @cmember Get the element count

        int GetElementCount () 
        {
            Header *pHeader = GetHeader ();
            return (int) pHeader ->ulElementCount;
        }

        // @cmember Get the var name count

        int GetVarNameCount () 
        {
            Header *pHeader = GetHeader ();
            return (int) pHeader ->ulVarNameCount;
        }

        // @cmember Get the n'th variable name

        VarName *GetVarName (int nIndex) 
        {
            TClass *pT = static_cast <TClass *> (this);
            Header *pHeader = GetHeader ();
            VarName *pasVarName = (VarName *) 
                pT ->GetData (pHeader ->ulVarNameOffset);
            return &pasVarName [nIndex];
        }

        // @cmember Get the list count

        int GetListCount () 
        {
            Header *pHeader = GetHeader ();
            return (int) pHeader ->ulListSize / sizeof (unsigned int);
        }

        // @cmember Get the nth list entry

        UINT32 GetList (int nIndex)
        {
            TClass *pT = static_cast <TClass *> (this);
            Header *pHeader = GetHeader ();
            UINT32 *palList = (UINT32 *) pT ->GetData (pHeader ->ulListOffset);
            return palList [nIndex];
        }

        // @cmember Get the variable data

        unsigned char *GetVarData (int nIndex = 0) 
        {
            TClass *pT = static_cast <TClass *> (this);
            Header *pHeader = GetHeader ();
            return pT ->GetData (pHeader ->ulVarDataOffset + nIndex);
        }

        // @cmember Get the variable data size

        int GetVarDataSize () 
        {
            Header *pHeader = GetHeader ();
            return (int) pHeader ->ulVarDataSize;
        }

        // @cmember Get the MultiMap count

        int GetMultiMapCount () 
        {
            Header *pHeader = GetHeader ();
            return (int) pHeader ->ulMultiMapSize / sizeof (unsigned int);
        }

        // @cmember Get the nth MultiMap entry

        UINT32 GetMultiMap (int nIndex)
        {
            TClass *pT = static_cast <TClass *> (this);
            Header *pHeader = GetHeader ();
            UINT32 *palMultiMap = (UINT32 *) 
                pT ->GetData (pHeader ->ulMultiMapOffset);
            return palMultiMap [nIndex];
        }

        // @cmember Get the n'th element

        Element *GetElement (int nIndex) 
        {
            TClass *pT = static_cast <TClass *> (this);
            Header *pHeader = GetHeader ();
            Element *pasElement = (Element *) 
                pT ->GetData (pHeader ->ulElementOffset);
            return &pasElement [nIndex];
        }

        // @cmember Get the MultiMap table for the entry

        UINT32 *GetMultiMapTable (Entry *pEntry)
        {
            if (pEntry ->ulCount <= 1)
            {
                return &pEntry ->ulOffset;
            }
            else
            {
                TClass *pT = static_cast <TClass *> (this);
                Header *pHeader = GetHeader ();
                assert ((pEntry ->ulOffset % sizeof (UINT32)) == 0);
                UINT32 *palMultiMap = (UINT32 *) pT ->GetData (
                    pHeader ->ulMultiMapOffset);
                return &palMultiMap [pEntry ->ulOffset / sizeof (UINT32)];
            }
        }

        // @cmember Get the list table

        UINT32 *GetListTable (const Element *pElement)
        {
            TClass *pT = static_cast <TClass *> (this);
            Header *pHeader = GetHeader ();
            UINT32 *palList = (UINT32 *) pT ->GetData (pHeader ->ulListOffset);
            return &palList [pElement ->u32Data / sizeof (UINT32)];
        }

        // @cmember Test the element type

        bool IsElement (const Element *pElement, const char *pszType) 
        {
            Header *pHeader = GetHeader ();
            if (pElement ->ulVarName >= pHeader ->ulVarNameCount)
                return false;
            const VarName *pVarName = GetVarName (pElement ->ulVarName);
            return strcmp (pVarName ->szName, pszType) == 0;
        }

        // @cmember Test the element type

        bool IsElement (int nElement, const char *pszType) 
        {
            return IsElement (GetElement (nElement), pszType);
        }


        // @cmember Get a STRREF structure

        StrRef *GetElementStrRef (const Element *pElement) 
        {
            return (StrRef *) GetVarData (pElement ->u32Data);
        }

        // @cmember Get a short string

        int GetElementShortString (const Element *pElement, 
                                   char *pszText, int cchMaxText) 
        {
            unsigned char *ps = GetVarData (pElement ->u32Data);
            unsigned char l = *ps++;
            if (l > cchMaxText - 1)
                l = cchMaxText -1;
            memmove (pszText, ps, l);
            pszText [l] = 0;
            return l;
        }

        // @cmember Get a long string

        int GetElementLongString (const Element *pElement, 
                                  char *pszText, int cchMaxText) 
        {
            unsigned char *ps = GetVarData (pElement ->u32Data);
            unsigned long l = *((unsigned long *) ps);
            ps += sizeof (unsigned long);
            if ((int) l > cchMaxText - 1)
                l = cchMaxText -1;
            memmove (pszText, ps, l);
            pszText [l] = 0;
            return l;
        }

        // @access Protected inline methods
    protected:

        // @cmember Convert the byte order

        void ConvertByteOrderFromFile ()
        {
#ifdef WORDS_BIGENDIAN

            //
            // If we have nothing, return
            //

            TClass *pT = static_cast <TClass *> (this);
            if (pT ->GetSize () == 0)
                return;

            //
            // Convert the header
            //

            Header *pHeader = GetHeader ();
            CNwnByteOrder<UINT32>::LittleEndianIP (pHeader ->ulEntryOffset);
            CNwnByteOrder<UINT32>::LittleEndianIP (pHeader ->ulEntryCount);
            CNwnByteOrder<UINT32>::LittleEndianIP (pHeader ->ulElementOffset);
            CNwnByteOrder<UINT32>::LittleEndianIP (pHeader ->ulElementCount);
            CNwnByteOrder<UINT32>::LittleEndianIP (pHeader ->ulVarNameOffset);
            CNwnByteOrder<UINT32>::LittleEndianIP (pHeader ->ulVarNameCount);
            CNwnByteOrder<UINT32>::LittleEndianIP (pHeader ->ulVarDataOffset);
            CNwnByteOrder<UINT32>::LittleEndianIP (pHeader ->ulVarDataSize);
            CNwnByteOrder<UINT32>::LittleEndianIP (pHeader ->ulMultiMapOffset);
            CNwnByteOrder<UINT32>::LittleEndianIP (pHeader ->ulMultiMapSize);
            CNwnByteOrder<UINT32>::LittleEndianIP (pHeader ->ulListOffset);
            CNwnByteOrder<UINT32>::LittleEndianIP (pHeader ->ulListSize);

            //
            // Convert the entry table
            //

            int nEntryCount = GetEntryCount ();
            Entry *pasEntries = GetEntry (0);
            for (int i = 0; i < nEntryCount; i++)
            {
                CNwnByteOrder<UINT32>::LittleEndianIP (pasEntries [i] .ulCode);
                CNwnByteOrder<UINT32>::LittleEndianIP (pasEntries [i] .ulOffset);
                CNwnByteOrder<UINT32>::LittleEndianIP (pasEntries [i] .ulCount);
            }

            //
            // Convert the multimap
            //

            int nMutliMapCount = pHeader ->ulMultiMapSize / sizeof (UINT32);
            UINT32 *palMultiMap = (UINT32 *) pT ->GetData (pHeader ->ulMultiMapOffset);
            for (int i = 0; i < nMutliMapCount; i++)
            {
                CNwnByteOrder<UINT32>::LittleEndianIP (palMultiMap [i]);
            }

            //
            // Convert the list table
            //

            int nListCount = pHeader ->ulListSize / sizeof (UINT32);
            UINT32 *palList = (UINT32 *) pT ->GetData (pHeader ->ulListOffset);
            for (int i = 0; i < nListCount; i++)
            {
                CNwnByteOrder<UINT32>::LittleEndianIP (palList [i]);
            }

            //
            // Convert the elements (oh god)
            //

            int nElementCount = GetElementCount ();
            Element *pasElements = GetElement (0);
            for (int i = 0; i < nElementCount; i++)
            {
                CNwnByteOrder<UINT32>::LittleEndianIP (pasElements [i] .ulType);
                CNwnByteOrder<UINT32>::LittleEndianIP (pasElements [i] .ulVarName);
                switch (pasElements [i] .ulType)
                {
                    default:
                        break;

                    case ElementType_UINT8:
                        break;

                    case ElementType_INT8:
                        break;

                    case ElementType_UINT16:
                        CNwnByteOrder<UINT16>::LittleEndianIP (pasElements [i] .u16Data);
                        break;

                    case ElementType_INT16:
                        CNwnByteOrder<INT16>::LittleEndianIP (pasElements [i] .n16Data);
                        break;

                    case ElementType_UINT32:
                        CNwnByteOrder<UINT32>::LittleEndianIP (pasElements [i] .u32Data);
                        break;

                    case ElementType_INT32:
                        CNwnByteOrder<INT32>::LittleEndianIP (pasElements [i] .n32Data);
                        break;

                    case ElementType_UINT64:
                        CNwnByteOrder<INT32>::LittleEndianIP (pasElements [i] .n32Data);
                        CNwnByteOrder<UINT64>::LittleEndianIP (GetVarData (pasElements [i] .n32Data));
                        break;

                    case ElementType_INT64:
                        CNwnByteOrder<INT32>::LittleEndianIP (pasElements [i] .n32Data);
                        CNwnByteOrder<INT64>::LittleEndianIP (GetVarData (pasElements [i] .n32Data));
                        break;

                    case ElementType_FLOAT:
                        CNwnByteOrder<float>::LittleEndianIP (pasElements [i] .fData);
                        break;

                    case ElementType_DOUBLE:
                        CNwnByteOrder<INT32>::LittleEndianIP (pasElements [i] .n32Data);
                        CNwnByteOrder<double>::LittleEndianIP (GetVarData (pasElements [i] .n32Data));
                        break;

                    case ElementType_STRING:
                        //The string starts with a UINT32 string length
                        CNwnByteOrder<INT32>::LittleEndianIP (pasElements [i] .n32Data);
                        CNwnByteOrder<UINT32>::LittleEndianIP (GetVarData (pasElements [i] .n32Data));
                        break;

                    case ElementType_RESREF:
                        CNwnByteOrder<INT32>::LittleEndianIP (pasElements [i] .n32Data);
                        break;

                    case ElementType_STRREF:
                        {
                            CNwnByteOrder<INT32>::LittleEndianIP (pasElements [i] .n32Data);
                            int n = pasElements [i] .n32Data;
                            StrRef *psStrRef = (StrRef *) GetVarData (n);
                            n += sizeof (StrRef);
                            CNwnByteOrder<UINT32>::LittleEndianIP (psStrRef ->ulLangSpecCount);
                            CNwnByteOrder<UINT32>::LittleEndianIP (psStrRef ->ulSize);
                            CNwnByteOrder<UINT32>::LittleEndianIP (psStrRef ->ulStrRef);
                            for (UINT32 ul = 0; ul < psStrRef ->ulLangSpecCount; ul++)
                            {
                                StrRefLangSpec *pLS = (StrRefLangSpec *) GetVarData (n);
                                CNwnByteOrder<UINT32>::LittleEndianIP (pLS ->ulLanguage);
                                CNwnByteOrder<UINT32>::LittleEndianIP (pLS ->ulLength);
                                n += sizeof (StrRefLangSpec) + pLS ->ulLength;
                            }
                        }
                        break;

                    case ElementType_DATREF:
                        //The data starts with a UINT32 length
                        CNwnByteOrder<INT32>::LittleEndianIP (pasElements [i] .n32Data);
                        CNwnByteOrder<UINT32>::LittleEndianIP (GetVarData (pasElements [i] .n32Data));
                        break;

                    case ElementType_CAPREF:
                        CNwnByteOrder<INT32>::LittleEndianIP (pasElements [i] .n32Data);
                        break;

                    case ElementType_LIST:
                        CNwnByteOrder<INT32>::LittleEndianIP (pasElements [i] .n32Data);
                        break;
                }
            }
#endif
        }
};

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CNwnHierarchy :
    public CNwnHierarchyImpl <CNwnHierarchy>,
    public CNwnMemoryFile
{
    // @access Constructors and destructors
    public:

        // @cmember General constructor

        CNwnHierarchy (unsigned char *pauchData = NULL, 
                       unsigned long ulSize = 0, bool fAllocated = true) :
            CNwnMemoryFile (pauchData, ulSize, fAllocated)
    {
        ConvertByteOrderFromFile ();
    }

        // @access Public methods
    public:

        // @cmember Set the data associated with the file

        void SetData (unsigned char *pauchData = NULL, 
                      unsigned long ulSize = 0, bool fAllocated = true)
        {
            CNwnMemoryFile::SetData (pauchData, ulSize, fAllocated);
            ConvertByteOrderFromFile ();
        }
};

#endif // ETS_NWNHIERARCHY_H

/* vim: set sw=4: */
