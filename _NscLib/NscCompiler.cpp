//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NscCompiler.cpp - External compiler routines |
//
// This module contains the compiler.
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

#include "Nsc.h"
#include "NscContext.h"
#include "NscPStackEntry.h"
#include "NscSymbolTable.h"
#include "NscCodeGenerator.h"

//
// Globals
//

CNscSymbolTable g_sNscReservedWords (0x400);
int g_nNscActionCount = 0;
CNwnArray <size_t> g_anNscActions;
CNscSymbolTable g_sNscNWScript;
CNscContext *g_pCtx;
std::string g_astrNscEngineTypes [16];

//-----------------------------------------------------------------------------
//
// @func Add a token to the reserved words
//
// @parm char * | pszName | Name of the token
//
// @parm int | nToken | Token value
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

inline void NscAddToken (char *pszName, int nToken)
{
    NscSymbol *pSymbol = g_sNscReservedWords .Add (
        pszName, NscSymType_Token);
    pSymbol ->nToken = nToken;
    pSymbol ->nEngineObject = -1;
}

//-----------------------------------------------------------------------------
//
// @func Initialize the compiler
//
// @parm CNwnLoader * | pLoader | Pointer to the resource loader to use
//
// @parm int | nVersion | Compilation version
//
// @parm bool | fEnableExtensions | If true, enable non-bioware extensions
//
// @rdesc TRUE if the compiler initialized
//
//-----------------------------------------------------------------------------

bool NscCompilerInitialize (CNwnLoader *pLoader, int nVersion, 
                            bool fEnableExtensions)
{
    fEnableExtensions; //4100

    //
    // Reset 
    //

    g_nNscActionCount = 0;
    g_anNscActions .RemoveAll ();
    g_sNscReservedWords .Reset ();
    g_sNscNWScript .Reset ();

    //
    // Add the reserved words
    //

    NscAddToken ("int",            INT_TYPE);
    NscAddToken ("float",          FLOAT_TYPE);
    NscAddToken ("object",         OBJECT_TYPE);
    NscAddToken ("string",         STRING_TYPE);
    NscAddToken ("struct",         STRUCT_TYPE);
    NscAddToken ("void",           VOID_TYPE);
    NscAddToken ("vector",         VECTOR_TYPE);
    NscAddToken ("action",         ACTION_TYPE);

    NscAddToken ("break",          BREAK);
    NscAddToken ("case",           CASE);
    NscAddToken ("continue",       CONTINUE);
    NscAddToken ("default",        DEFAULT);
    NscAddToken ("do",             DO);
    NscAddToken ("else",           ELSE);
    NscAddToken ("for",            FOR);
    NscAddToken ("if",             IF);
    NscAddToken ("return",         RETURN);
    NscAddToken ("switch",         SWITCH);
    NscAddToken ("while",          WHILE);
    if (fEnableExtensions || nVersion >= 999)
        NscAddToken ("const",      CONST);

    NscAddToken ("OBJECT_SELF",    OBJECT_SELF_CONST);
    NscAddToken ("OBJECT_INVALID", OBJECT_INVALID_CONST);

    //
    // Read NWSCRIPT
    //

    bool fAllocated;
    UINT32 ulSize;
    unsigned char *pauchData = pLoader ->LoadResource (
        "nwscript", NwnResType_NSS, &ulSize, &fAllocated);
    if (pauchData == NULL)
    {
        printf ("Unable to load nwscript.nss\n");
        return false;
    }

    //
    // Compile
    //

    CNwnMemoryStream *pStream = new CNwnMemoryStream 
        ("nwscript.nss", pauchData, ulSize, fAllocated);
    CNscContext sCtx;
    sCtx .AddStream (pStream);
    sCtx .SetLoader (pLoader);
    sCtx .SetNWScript (true);
    sCtx .SetOptExpression (true);
    g_pCtx = &sCtx;
    if (sCtx .yyparse () != 0 || sCtx .GetErrors () > 0)
    {
        printf ("Error compiling nwscipt.nss\n");
        return false;
    }

    //
    // Copy the symbol table
    //

    sCtx .SaveSymbolTable (&g_sNscNWScript);
    return true;
}

//-----------------------------------------------------------------------------
//
// @func Compile a script in a buffer
//
// @parm CNwnLoader * | pLoader | Pointer to the resource loader to use
//
// @parm const char * | pszName | Name of the script
//
// @parm unsigned char * | pauchData | Resource data
//
// @parm UINT32 | ulSize | Length of the resource
//
// @parm bool | fAllocated | true if the resource is allocated
//
// @parm int | nVersion | Compilation version
//
// @parm bool | fEnableOptimizations | If true, enable optimizations
//
// @parm bool | fIgnoreIncludes | If true, ignore include files
//
// @parm CNwnStream * | pCodeOutput | Destination stream for NCS file
//
// @parm CNwnStream * | pDebugOutput | Destination stream for NDB file. 
//              (Can be NULL)
//
// @rdesc Results of the compilation
//
//-----------------------------------------------------------------------------

NscResult NscCompileScript (CNwnLoader *pLoader, const char *pszName, 
                            unsigned char *pauchData, UINT32 ulSize, bool fAllocated,
                            int nVersion, bool fEnableOptimizations, bool fIgnoreIncludes, 
                            CNwnStream *pCodeOutput, CNwnStream *pDebugOutput,
                            CNwnStream *pErrorOutput)
{
    //yydebug = 1;

    //
    // Generate a full name from the partial
    //

    char *pszFullName = (char *) pszName;
    if (strchr (pszName, '.') == NULL)
    {
        size_t nLength = strlen (pszName);
        pszFullName = (char *) alloca (nLength + 5);
        strcpy (pszFullName, pszName);
        strcat (pszFullName, ".nss");
    }

    //
    // Initialize context
    //

    CNscContext sCtx;
    sCtx .SetLoader (pLoader);
    sCtx .LoadSymbolTable (&g_sNscNWScript);
    if (pErrorOutput)
    {
        sCtx. SetErrorOutputStream(pErrorOutput);
    }

    if (fEnableOptimizations)
    {
        sCtx .SetOptReturn (true);
        sCtx .SetOptExpression (true);
    }
    g_pCtx = &sCtx;

    //
    // PHASE 1
    //

    CNwnMemoryStream *pStream = new CNwnMemoryStream 
        (pszFullName, pauchData, ulSize, false);

    sCtx .AddStream (pStream);
    //sCtx.yydebug = 1;
    sCtx .yyparse ();
    if (sCtx .GetErrors () > 0)
    {
        if (fAllocated)
            free (pauchData);
        return NscResult_Failure;
    }

    //
    // Search for main or starting conditional
    //

    if (fIgnoreIncludes && !sCtx .HasMain ())
        return NscResult_Include;

    //
    // PHASE 2
    //

    pStream = new CNwnMemoryStream 
        (pszFullName, pauchData, ulSize, fAllocated);
    sCtx .ClearFiles ();
    sCtx .AddStream (pStream);
    sCtx .SetPhase2 (true);
    sCtx .yyparse ();
    if (sCtx .GetErrors () > 0)
        return NscResult_Failure;

    //
    // Generate the output
    //

    CNscCodeGenerator sGen (&sCtx, nVersion, fEnableOptimizations);
    if (!sGen .GenerateOutput (pCodeOutput, pDebugOutput))
        return NscResult_Failure;

    //
    // Success
    //

    return NscResult_Success;
}

//-----------------------------------------------------------------------------
//
// @func Compile a script
//
// @parm CNwnLoader * | pLoader | Pointer to the resource loader to use
//
// @parm const char * | pszName | Name of the script
//
// @parm int | nVersion | Compilation version
//
// @parm bool | fEnableOptimizations | If true, enable optimizations
//
// @parm bool | fIgnoreIncludes | If true, ignore include files
//
// @parm CNwnStream * | pCodeOutput | Destination stream for NCS file
//
// @parm CNwnStream * | pDebugOutput | Destination stream for NDB file. 
//              (Can be NULL)
//
// @rdesc Results of the compilation
//
//-----------------------------------------------------------------------------

NscResult NscCompileScript (CNwnLoader *pLoader, const char *pszName, 
                            int nVersion, bool fEnableOptimizations, bool fIgnoreIncludes, 
                            CNwnStream *pCodeOutput, CNwnStream *pDebugOutput, CNwnStream *pErrorOutput)
{

    //
    // Load the script
    //

    bool fAllocated;
    UINT32 ulSize;
    unsigned char *pauchData = pLoader ->LoadResource (pszName,
                                                       NwnResType_NSS, &ulSize, &fAllocated);
    if (!fAllocated)
    {
        printf ("Unable to load resource \"%s\"\n", pszName);
        return NscResult_Failure;
    }

    //
    // Invoke the main routine
    //

    return NscCompileScript (pLoader, pszName, pauchData, 
                             ulSize, fAllocated, nVersion, fEnableOptimizations, 
                             fIgnoreIncludes, pCodeOutput, pDebugOutput, pErrorOutput);
}

//-----------------------------------------------------------------------------
//
// @func Return the name of an action
//
// @parm int | nAction | Action index
//
// @rdesc Pointer to the action name
//
//-----------------------------------------------------------------------------

const char *NscGetActionName (int nAction)
{
    if (nAction < 0 || nAction >= g_nNscActionCount)
        return "UnknownAction";
    else
    {
        NscSymbol *pSymbol = g_sNscNWScript .GetSymbol (g_anNscActions [nAction]);
        return pSymbol ->szString;
    }
}

/* vim: set sw=4: */
