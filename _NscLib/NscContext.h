#ifndef ETS_NSCCONTEXT_H
#define ETS_NSCCONTEXT_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NscContext.h - Script compiler context |
//
// This module contains the definition of the NscContext.
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
#include "../_NwnLib/NwnStreams.h"
#include "../_NwnLib/NwnArray.h"
#include "NscPStackEntry.h"
#include "NscSymbolTable.h"
#define YYSTYPE CNscPStackEntry *
#include "NscParser.h"

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

class CNwnLoader;

//-----------------------------------------------------------------------------
//
// Class definition
//
//-----------------------------------------------------------------------------

class CNscContext : public yyparser
{
    enum Constants
    {
        Max_Line_Length         = 0x8000,
        Max_Token_Length        = 0x1000,
        Max_Structs                     = 128, // Limited by sizeof of UINT8/NscType
    };

    struct Entry
    {
        CNwnStream              *pStream;
        Entry                   *pNext;
        char                    *pszLine;
        char                    *pszToken;
        char                    *pszNextTokenPos;
        int                             nLine;
        int                             nFile;
    };

    struct BackLink 
    {
        size_t                  nNext;
        size_t                  nOffset;
    };

    struct File
    {
        std::string             strName;
        int                             nOutputIndex;
        int                             nFileIndex;
    };

    // @access Constructors and destructors
    public:

    // @cmember General constructor

    CNscContext ();

    // @cmember Delete the streams

    ~CNscContext ();

    // @access Public methods
    public:

    // @cmember Get the next token

    virtual int yylex ();

    // @cmember Generate a parser error

    virtual void yyerror (const char *pszMessage)
    {
        pszMessage; // Get rid of warning 4100
        // GenerateError ("%s", pszMessage);
    }

    // @cmember Get a PStackEntry

#ifdef _DEBUG
    CNscPStackEntry *GetPStackEntry (const char *pszFile, int nLine);
#else
    CNscPStackEntry *GetPStackEntryInt ();
    CNscPStackEntry *GetPStackEntry (const char *pszFile, int nLine)
    {
        pszFile; nLine; // Get rid of warning 4100
        return GetPStackEntryInt ();
    }
#endif

    // @cmember Free a PStackEntry

    void FreePStackEntry (CNscPStackEntry *pEntry)
    {
        pEntry ->Free ();
        pEntry ->m_link .InsertHead (&m_listEntryFree);
    }

    // @cmember Add a new prototype to the symbol table

    NscSymbol *AddPrototype (const char *pszIdentifier, NscType nType,
                             UINT32 ulFlags, unsigned char *pauchArgData, size_t nArgDataSize);

    // @cmember Add a new structure to the symbol table

    void AddStructure (const char *pszIdentifier, 
                       unsigned char *pauchStructData, size_t nStructDataSize);

    // @cmember Add initialization to a global

    void AddVariableInit (NscSymbol *pSymbol, 
                          unsigned char *pauchInit, size_t nInitSize);

    // @cmember Add a new variable to the symbol table

    void AddVariable (const char *pszIdentifier, NscType nType,
                      UINT32 ulFlags);

    // @cmember Return the size of a type

    int GetTypeSize (NscType nType);

    // @access Public inline methods
    public:

    // @cmember Get the current fence

    NscSymbolFence *GetCurrentFence ()
    {
        return m_pCurrentFence;
    }

    // @cmember Load symbol table

    void LoadSymbolTable (CNscSymbolTable *pTable)
    {
        m_sSymbols .CopyFrom (pTable);
    }

    // @cmember Save the symbol table

    void SaveSymbolTable (CNscSymbolTable *pTable)
    {
        pTable ->CopyFrom (&m_sSymbols);
    }

    // @cmember Find a symbol

    NscSymbol *FindSymbol (const char *pszName)
    {
        return m_sSymbols .Find (pszName);
    }

    // @cmember Get the offset of a symbol

    size_t GetSymbolOffset (NscSymbol *pSymbol)
    {
        return m_sSymbols .GetSymbolOffset (pSymbol);
    }

    // @cmember Get the symbol at the given offset

    NscSymbol *GetSymbol (size_t nOffset)
    {
        return m_sSymbols .GetSymbol (nOffset);
    }

    // @cmember Get symbol data

    unsigned char *GetSymbolData (size_t nOffset)
    {
        return m_sSymbols .GetData (nOffset);
    }

    // @cmember Get the global variable count

    size_t GetGlobalVariableCount () const
    {
        return m_anGlobalVars .GetCount ();
    }

    // @cmember Get the symbol for the n'th global variable

    NscSymbol *GetGlobalVariable (size_t nIndex)
    {
        return m_sSymbols .GetSymbol (m_anGlobalVars [nIndex]);
    }

    // @cmember Add a global definition

    void AddGlobalDefinition (size_t nOffset)
    {
        m_anGlobalDefs .Add (nOffset);
    }

    // @cmember Get the global definition count

    size_t GetGlobalDefinitionCount () const
    {
        return m_anGlobalDefs .GetCount ();
    }

    // @cmember Get the symbol for the n'th global definition

    NscSymbol *GetGlobalDefinition (size_t nIndex)
    {
        return m_sSymbols .GetSymbol (m_anGlobalDefs [nIndex]);
    }

    // @cmember Add a new global function

    void AddGlobalFunction (size_t nSymbol)
    {
        m_anGlobalFuncs .Add (nSymbol);
    }

    // @cmember Get the global function count

    size_t GetGlobalFunctionCount () const
    {
        return m_anGlobalFuncs .GetCount ();
    }

    // @cmember Get the symbol for the n'th global function

    NscSymbol *GetGlobalFunction (size_t nIndex)
    {
        return m_sSymbols .GetSymbol (m_anGlobalFuncs [nIndex]);
    }

    // @cmember Append symbol data

    size_t AppendSymbolData (unsigned char *pauchData, size_t nDataSize)
    {
        return m_sSymbols .AppendData (pauchData, nDataSize);
    }

    // @cmember Is this a structure

    bool IsStructure (NscType nType)
    {
        int nIndex = nType - NscType_Struct_0;
        return nIndex >= 0 && nIndex < m_nStructs;
    }

    // @cmember Get a structure symbol

    NscSymbol *GetStructSymbol (NscType nType)
    {
        int nIndex = nType - NscType_Struct_0;
        assert (nIndex >= 0 && nIndex < m_nStructs);
        return m_sSymbols .GetSymbol (m_anStructSymbol [nIndex]);
    }

    // @cmember Generate a warning

    void GenerateWarning (const char *pszText, ...)
    {
        va_list marker;
        va_start (marker, pszText);
        GenerateError ("Warning", pszText, marker);
        va_end (marker);
        m_nWarnings++;
    }

    // @cmember Generate an error

    void GenerateError (const char *pszText, ...)
    {
        va_list marker;
        va_start (marker, pszText);
        GenerateError ("Error", pszText, marker);
        va_end (marker);
        m_nErrors++;
    }

    // @cmember Get the top stream

    CNwnStream *GetCurrentStream ()
    {
        if (m_pStreamTop == NULL)
            return NULL;
        else
            return m_pStreamTop ->pStream;
    }

    // @cmember Add a new stream

    void AddStream (CNwnStream *pStream)
    {

        //
        // Add a new entry to the list
        //

        Entry *pEntry = new Entry;
        pEntry ->pNext = m_pStreamTop;
        pEntry ->pStream = pStream;
        pEntry ->pszLine = new char [Max_Line_Length + Max_Token_Length];
        pEntry ->pszToken = &pEntry ->pszLine [Max_Line_Length];
        pEntry ->pszNextTokenPos = NULL;
        pEntry ->nLine = 0;
        pEntry ->nFile = -1;
        m_pStreamTop = pEntry;
        m_nStreamDepth++;

        //
        // Add the file name to the list of files
        //

        const char *pszFileName = pStream ->GetFileName ();
        if (pszFileName)
        {
            pEntry ->nFile = (int) m_asFiles .GetCount ();
            const char *pszBaseName = NwnBasename (pszFileName);
            size_t nLength = strlen (pszBaseName);
            char *pszCopy = (char *) alloca (nLength + 1);
            strcpy (pszCopy, pszBaseName);
            char *pszExt = strrchr (pszCopy, '.');
            if (pszExt)
                *pszExt = 0;
            // In the NDB file, the main is lowercase...
            if (m_asFiles .GetCount () == 0)
                strlwr (pszCopy);
            File sFile;
            sFile .strName = pszCopy;
            sFile .nOutputIndex = -1;
            sFile .nFileIndex = -1;
            m_asFiles .Add (sFile);
        }
        return;
    }

    // @cmember Remove the top stream

    void RemoveTopStream ()
    {
        if (m_pStreamTop == NULL)
            return;
        m_nStreamDepth--;
        Entry *pEntry = m_pStreamTop;
        m_pStreamTop = pEntry ->pNext;
        delete pEntry ->pStream;
        delete [] pEntry ->pszLine;
        delete pEntry;
        return;
    }

    // @cmember Clear the current list of file

    void ClearFiles ()
    {
        m_asFiles .RemoveAll ();
    }

    // @cmember Get the number of warnings

    int GetWarnings () const
    {
        return m_nWarnings;
    }

    // @cmember Get the number of errors

    int GetErrors () const
    {
        return m_nErrors;
    }

    // @cmember Get the loader

    CNwnLoader *GetLoader ()
    {
        return m_pLoader;
    }

    // @cmember Set the loader

    void SetLoader (CNwnLoader *pLoader)
    {
        m_pLoader = pLoader;
    }

    // @cmember TRUE if a main was found 

    bool HasMain () const
    {
        return m_fHasMain;
    }

    // @cmember Set our main flag

    void SetMain (bool fHasMain)
    {
        m_fHasMain = fHasMain;
    }

    // @cmember Return the number of structures

    int GetStructCount () const
    {
        return m_nStructs;
    }

    // @cmember TRUE if we are compiling NWScript

    bool IsNWScript () const
    {
        return m_fNWScript;
    }

    // @cmember Set the NWScript flag

    void SetNWScript (bool fNWScript)
    {
        m_fNWScript = fNWScript;
    }

    // @cmember TRUE if we are in phase 2

    bool IsPhase2 () const
    {
        return m_fPhase2;
    }

    // @cmember Set the phase2 flag

    void SetPhase2 (bool fPhase2) 
    {
        m_fPhase2 = fPhase2;
    }

    // @cmember Return TRUE if we are in global scope

    bool IsGlobalScope () const
    {
        return m_fGlobalScope;
    }

    // @cmember Set the global scope

    void SetGlobalScope (bool fGlobalScope)
    {
        m_fGlobalScope = fGlobalScope;
    }

    // @cmember Return TRUE if we are optimize trailing return movsp

    bool GetOptReturn () const
    {
        return m_fOptReturn;
    }

    // @cmember Set the optimize trailing return movsp

    void SetOptReturn (bool fOptReturn)
    {
        m_fOptReturn = fOptReturn;
    }

    // @cmember Return TRUE if we are optimizing expressions

    bool GetOptExpression () const
    {
        return m_fOptExpression;
    }

    // @cmember Set the optimize expressions

    void SetOptExpression (bool fOptExpression)
    {
        m_fOptExpression = fOptExpression;
    }

    // @cmember Get the current fence

    void GetFence (CNscPStackEntry *pEntry, size_t nFnSymbol,
                   NscFenceType nFenceType, bool fEatScope)
    {
        if (pEntry ->m_pFence == NULL)
            pEntry ->m_pFence = new NscSymbolFence;
        m_sSymbols .GetFence (pEntry ->m_pFence);
        pEntry ->m_pFence ->nFnSymbol = nFnSymbol;
        pEntry ->m_pFence ->nFenceType = nFenceType;
        pEntry ->m_pFence ->fEatScope = fEatScope;
        pEntry ->m_pFence ->pNext = m_pCurrentFence;
        pEntry ->m_pFence ->nLocals = 0;
        pEntry ->m_pFence ->nPrevLocals = 0;
        pEntry ->m_pFence ->fHasDefault = false;
        pEntry ->m_pFence ->nFenceReturn = NscFenceReturn_Unknown;
        if (m_pCurrentFence)
        {
            pEntry ->m_pFence ->nPrevLocals += 
                m_pCurrentFence ->nLocals +
                m_pCurrentFence ->nPrevLocals;
        }
        pEntry ->m_fFenceValid = true;
        m_pCurrentFence = pEntry ->m_pFence;
        return;
    }

    // @cmember Restore the given fence

    void RestoreFence (CNscPStackEntry *pEntry)
    {
        if (pEntry ->m_fFenceValid)
        {
            assert (pEntry ->m_pFence);
            //assert (m_pCurrentFence == pEntry ->m_pFence);
            m_pCurrentFence = pEntry ->m_pFence ->pNext;
            m_sSymbols .RestoreFence (pEntry ->m_pFence);
        }
    }

    // @cmember Get the current file number

    int GetCurrentFile () const
    {
        return m_pStreamTop ->nFile;
    }

    // @cmember Get the current line number

    int GetCurrentLine () const
    {
        return m_pStreamTop ->nLine;
    }

    // @cmember Mark a file as used

    int MarkUsedFile (int nFile)
    {
        if (m_asFiles [nFile] .nOutputIndex == -1)
        {
            m_asFiles [m_nUsedFiles] .nFileIndex = nFile;
            m_asFiles [nFile] .nOutputIndex = m_nUsedFiles;
            m_nUsedFiles++;
        }
        return m_asFiles [nFile] .nOutputIndex;
    }

    // @cmember Get the number of used files

    int GetUsedFiles () const
    {
        return m_nUsedFiles;
    }

    // @cmember Get the n'th file name

    const char *GetUsedFileName (int nIndex) const
    {
        return m_asFiles [m_asFiles [nIndex] .nFileIndex] .strName .c_str ();
    }

    // @cmember Get the n'th file main status

    bool GetUsedFileMainStatus (int nIndex) const
    {
        return m_asFiles [nIndex] .nFileIndex == 0;
    }

    // @cmember Save the current line information

    void SaveFileAndLine (int nIndex)
    {
        m_anFileNumbers [nIndex] = GetCurrentFile ();
        m_anLineNumbers [nIndex] = GetCurrentLine ();
    }

    // @cmember Get the given file number

    int GetFile (int nIndex) const
    {
        return m_anFileNumbers [nIndex];
    }

    // @cmember Get the given line number

    int GetLine (int nIndex) const
    {
        return m_anLineNumbers [nIndex];
    }

    // @cmember Copy file and line information

    void CopyFileAndLine (int nDest, int nSource)
    {
        m_anFileNumbers [nDest] = m_anFileNumbers [nSource];
        m_anLineNumbers [nDest] = m_anLineNumbers [nSource];
    }

    // @cmember set the stream to output errors to

    void SetErrorOutputStream (CNwnStream *s)
    {
        m_pErrorStream = s;
    }

    // @access Protected methods
    protected:

    // @cmember Generate an error

    void GenerateError (const char *pszType, 
                        const char *pszText, va_list marker)
    {
        char prefix[200],error[200];
        if (m_pStreamTop == NULL)
            sprintf (prefix,"%s: ", pszType);
        else
        {
            sprintf (prefix,"%s(%d): %s: ", 
                     m_pStreamTop ->pStream ->GetFileName (), 
                     m_pStreamTop ->nLine, pszType);
        }
        printf("%s", prefix);

        vsprintf (error,pszText, marker);
        printf("%s", error);

        if (m_pErrorStream)
        {
            m_pErrorStream->Write(prefix,strlen(prefix));
            m_pErrorStream->Write(error,strlen(error));
            m_pErrorStream->Write((void*)"\n",1);
        }

        printf ("\n");
        //free(prefix);
        //free(error);
    }

    // @cmember Read the next line

    bool ReadNextLine (bool fInComment);

    // @cmember Protected members
    protected:

    //
    // ------- GENERAL
    //

    // @cmember Number of warnings

    int                                         m_nWarnings;

    // @cmember Number of errors

    int                                         m_nErrors;

    // @cmember Stream containing errors and warnings produced

    CNwnStream                                     *m_pErrorStream;

    //
    // ------- COMPILE TIME
    //

    // @cmember Current top of stream

    Entry                                       *m_pStreamTop;

    // @cmember Current stream depth

    int                                         m_nStreamDepth;

    // @cmember Number of defined structures

    int                                         m_nStructs;

    // @cmember Structure symbol offset

    size_t                                      m_anStructSymbol [Max_Structs];

    // @cmember Loader used to access resources

    CNwnLoader                          *m_pLoader;

    // @cmember Allocated entry list

    CNwnDoubleLinkList          m_listEntryAllocated;

    // @cmember Free entry list

    CNwnDoubleLinkList          m_listEntryFree;

    // @cmember My symbol table

    CNscSymbolTable                     m_sSymbols;

    // @cmember Current scope fence

    NscSymbolFence                      *m_pCurrentFence;

    // @cmember If true, we are compiling NWScript

    bool                                        m_fNWScript;

    // @cmember If true, this is phase 2 of compilation

    bool                                        m_fPhase2;

    // @cmember If true, a main was seen

    bool                                        m_fHasMain;

    // @cmember If true, we are in global scope

    bool                                        m_fGlobalScope;

    // @cmember Global variable array

    CNwnArray <size_t>          m_anGlobalVars;

    // @cmember Global function array

    CNwnArray <size_t>          m_anGlobalFuncs;

    // @cmember Global definitions

    CNwnArray <size_t>          m_anGlobalDefs;

    // @cmember List of included files

    CNwnArray <File>            m_asFiles;

    // @cmember Number of files actually included in the compiled script

    int                                         m_nUsedFiles;

    // @cmember File number list

    int                                         m_anFileNumbers [10];

    // @cmember Line number list

    int                                         m_anLineNumbers [10];

    //
    // ------- OPTIMIZATION FLAGS
    //

    // @cmember If true, optimize the trailing return movsp

    bool                                        m_fOptReturn;

    // @cmember If true, optimize expressions

    bool                                        m_fOptExpression;
};

#endif // ETS_NSCCONTEXT_H

/* vim: set sw=4: */
