//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NscContext.cpp - Compiler context support |
//
// This module contains the compiler context support.
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

//
// Externals
//

extern CNscSymbolTable g_sNscReservedWords;
extern int g_nNscActionCount;
extern CNwnArray <size_t> g_anNscActions;
extern std::string g_astrNscEngineTypes [16];

//-----------------------------------------------------------------------------
//
// @mfunc <c CNscContext> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNscContext::CNscContext ()
{
    m_pStreamTop = NULL;
    m_nStreamDepth = 0;
    m_nWarnings = 0;
    m_nErrors = 0;
    m_pLoader = NULL;
    m_fNWScript = false;
    m_fPhase2 = false;
    m_fGlobalScope = true;
    m_fHasMain = false;
    m_nStructs = 0;
    m_pCurrentFence = NULL;
    m_fOptReturn = false;
    m_fOptExpression = false;
    m_nUsedFiles = 0;
    m_pErrorStream = NULL;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CNscContext> denstructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CNscContext::~CNscContext ()
{

    //
    // Delete any allocated entries
    //

    while (m_listEntryAllocated .GetNext () != &m_listEntryAllocated)
    {
        CNwnDoubleLinkList *pNext = m_listEntryAllocated .GetNext ();
        CNscPStackEntry *pEntry = (CNscPStackEntry *) pNext;
#ifdef _DEBUG
        printf ("Leaked PStackEntry (%s,%d)\n", 
                pEntry ->m_pszFile, pEntry ->m_nLine);
#endif
        pEntry ->Free ();
        delete pEntry;
    }

    //
    // Delete all the free entries
    //

    while (m_listEntryFree .GetNext () != &m_listEntryFree)
    {
        CNwnDoubleLinkList *pNext = m_listEntryFree .GetNext ();
        CNscPStackEntry *pEntry = (CNscPStackEntry *) pNext;
        delete pEntry;
    }

    //
    // Delete the streams
    //

    while (m_nStreamDepth)
        RemoveTopStream ();
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the next token from the current line or NULL if out
//
// @rdesc Token ID.
//
//-----------------------------------------------------------------------------

int CNscContext::yylex ()
{

    //
    // Initialize lvalue
    //

    yylval = NULL;

    //
    // If we have no stream, return nothing
    //

    if (m_pStreamTop == NULL)
        return EOF;

    //
    // If we need to read a line
    //

try_again:;
          if (m_pStreamTop ->pszNextTokenPos == NULL || 
              *m_pStreamTop ->pszNextTokenPos == 0)
          {
read_another_line:;
                  if (!ReadNextLine (false))
                      return EOF;
          }

          //
          // Skip the white space
          //

          char c;
          for (;;)
          {
              c = *m_pStreamTop ->pszNextTokenPos;
              if (c == 0)
                  goto read_another_line;
              else if (c <= ' ' || c > 126)
                  m_pStreamTop ->pszNextTokenPos++;
              else
                  break;
          }

          //
          // If we have an identifier
          //

          if (isalpha (c) || c == '_')
          {
              char *pszStart = m_pStreamTop ->pszNextTokenPos;
              m_pStreamTop ->pszNextTokenPos++;
              for (;;)
              {
                  c = *m_pStreamTop ->pszNextTokenPos;
                  if (isalnum (c) || c == '_')
                      m_pStreamTop ->pszNextTokenPos++;
                  else
                      break;
              }

              //
              // Get the hash value for the ID
              //

              int nCount = (int) (m_pStreamTop ->pszNextTokenPos - pszStart);
              UINT32 ulHash = CNscSymbolTable::GetHash (pszStart, nCount);

              //
              // See if it is a reserved word
              //

              NscSymbol *pSymbol = g_sNscReservedWords .Find (pszStart, nCount, ulHash);

              //
              // If so, return that word
              //

              if (pSymbol != NULL)
              {
                  assert (pSymbol ->nSymType == NscSymType_Token);
                  if (pSymbol ->nToken == ENGINE_TYPE)
                  {
                      CNscPStackEntry *pEntry = GetPStackEntry (__FILE__, __LINE__);
                      pEntry ->SetType ((NscType) (
                              NscType_Engine_0 + pSymbol ->nEngineObject));
                      yylval = pEntry;
                      return pSymbol ->nToken;
                  }
                  else
                      return pSymbol ->nToken;
              }
              else 
              {
                  CNscPStackEntry *pEntry = GetPStackEntry (__FILE__, __LINE__);
                  pEntry ->SetIdentifier (pszStart, nCount);
                  yylval = pEntry;
                  return IDENTIFIER;
              }
          }

          //
          // If we have a number.
          //
          // The bioware compiler doesn't like a number starting
          // with a '.'
          //

          else if (isdigit (c))
          {

              // 
              // If this is a hex value
              //

              if (c == '0' && 
                  (m_pStreamTop ->pszNextTokenPos [1] == 'x' ||
                   m_pStreamTop ->pszNextTokenPos [1] == 'X'))
              {

                  //
                  // Parse the number
                  //

                  m_pStreamTop ->pszNextTokenPos += 2;
                  int nValue = 0;
                  for (;;)
                  {
                      c = *m_pStreamTop ->pszNextTokenPos;
                      if (isdigit (c))
                      {
                          nValue = nValue * 16 + (c - '0');
                          m_pStreamTop ->pszNextTokenPos++;
                      }
                      else if (c >= 'A' && c <= 'F')
                      {
                          nValue = nValue * 16 + (c - 'A' + 10);
                          m_pStreamTop ->pszNextTokenPos++;
                      }
                      else if (c >= 'a' && c <= 'f')
                      {
                          nValue = nValue * 16 + (c - 'a' + 10);
                          m_pStreamTop ->pszNextTokenPos++;
                      }
                      else
                          break;
                  }

                  //
                  // Return results
                  //

                  CNscPStackEntry *pEntry = GetPStackEntry (__FILE__, __LINE__);
                  pEntry ->SetType (NscType_Integer);
                  pEntry ->PushConstantInteger (nValue);
                  yylval = pEntry;
                  return INTEGER_CONST; 
              }

              //
              // Otherwise, treat as a normal number
              //

              else
              {

                  //
                  // Parse the number
                  //

                  bool fHasDecimal = false;
                  char *pszStart = m_pStreamTop ->pszNextTokenPos;
                  for (;;)
                  {
                      c = *m_pStreamTop ->pszNextTokenPos;
                      if (isdigit (c))
                          m_pStreamTop ->pszNextTokenPos++;
                      else if (c == '.' && !fHasDecimal)
                      {
                          fHasDecimal = true;
                          m_pStreamTop ->pszNextTokenPos++;
                      }
                      else
                          break;
                  }

                  //
                  // Test for 'F' extension
                  //

                  int nCharacter = (int) (m_pStreamTop ->pszNextTokenPos - pszStart);
                  if (c == 'f' || c == 'F')
                  {
                      fHasDecimal = true;
                      m_pStreamTop ->pszNextTokenPos++;
                  }

                  //
                  // Convert the value
                  //

                  char *psz = (char *) alloca (nCharacter + 1);
                  memcpy (psz, pszStart, nCharacter);
                  psz [nCharacter] = 0;
                  CNscPStackEntry *pEntry = GetPStackEntry (__FILE__, __LINE__);
                  yylval = pEntry;
                  if (fHasDecimal)
                  {
                      pEntry ->SetType (NscType_Float);
                      pEntry ->PushConstantFloat ((float) atof (psz));
                      return FLOAT_CONST;
                  }
                  else
                  {
                      pEntry ->SetType (NscType_Integer);
                      pEntry ->PushConstantInteger (atol (psz));
                      return INTEGER_CONST;
                  }
              }
          }

          //
          // Otherwise, we have a symbol (hopefully)
          //

          else
          {
              m_pStreamTop ->pszNextTokenPos++;
              switch (c)
              {
                  case '/':
                      c = *m_pStreamTop ->pszNextTokenPos;
                      if (c == '*')
                      {
                          m_pStreamTop ->pszNextTokenPos++;
                          for (;;)
                          {
                              if (m_pStreamTop ->pszNextTokenPos [0] == '*' &&
                                  m_pStreamTop ->pszNextTokenPos [1] == '/')
                              {
                                  m_pStreamTop ->pszNextTokenPos += 2;
                                  goto try_again;
                              }
                              else if (m_pStreamTop ->pszNextTokenPos [0] == 0)
                              {
                                  if (!ReadNextLine (true))
                                  {
                                      //GenerateError ("End of file reached while processing comment");
                                      if (!IsPhase2 ())
                                          GenerateWarning ("End of file reached while processing comment");
                                      return EOF;
                                  }
                              }
                              else
                                  m_pStreamTop ->pszNextTokenPos++;
                          }
                      }
                      else if (c == '/')
                      {
                          m_pStreamTop ->pszNextTokenPos++;
                          goto read_another_line;
                      }
                      else if (c == '=')
                      {
                          m_pStreamTop ->pszNextTokenPos++;
                          return DIVEQ;
                      }
                      else 
                      {
                          return '/';
                      }
                      break;

                  case '{':
                  case '}':
                  case '[':
                  case ']':
                  case '(':
                  case ')':
                  case ';':
                  case ':':
                  case '?':
                  case ',':
                  case '~':
                  case '.':
                      return c;

                  case '+':
                      c = *m_pStreamTop ->pszNextTokenPos;
                      if (c == '=')
                      {
                          m_pStreamTop ->pszNextTokenPos++;
                          return ADDEQ;
                      }
                      else if (c == '+')
                      {
                          m_pStreamTop ->pszNextTokenPos++;
                          return PLUSPLUS;
                      }
                      else
                          return '+';
                      break;

                  case '-':
                      c = *m_pStreamTop ->pszNextTokenPos;
                      if (c == '=')
                      {
                          m_pStreamTop ->pszNextTokenPos++;
                          return SUBEQ;
                      }
                      else if (c == '-')
                      {
                          m_pStreamTop ->pszNextTokenPos++;
                          return MINUSMINUS;
                      }
                      else
                          return '-';
                      break;

                  case '*':
                      c = *m_pStreamTop ->pszNextTokenPos;
                      if (c == '=')
                      {
                          m_pStreamTop ->pszNextTokenPos++;
                          return MULEQ;
                      }
                      else
                          return '*';
                      break;

                  case '%':
                      c = *m_pStreamTop ->pszNextTokenPos;
                      if (c == '=')
                      {
                          m_pStreamTop ->pszNextTokenPos++;
                          return MODEQ;
                      }
                      else
                          return '%';
                      break;

                  case '^':
                      c = *m_pStreamTop ->pszNextTokenPos;
                      if (c == '=')
                      {
                          m_pStreamTop ->pszNextTokenPos++;
                          return XOREQ;
                      }
                      else
                          return '^';
                      break;

                  case '&':
                      c = *m_pStreamTop ->pszNextTokenPos;
                      if (c == '=')
                      {
                          m_pStreamTop ->pszNextTokenPos++;
                          return ANDEQ;
                      }
                      else if (c == '&')
                      {
                          m_pStreamTop ->pszNextTokenPos++;
                          return ANDAND;
                      }
                      else
                          return '&';
                      break;

                  case '|':
                      c = *m_pStreamTop ->pszNextTokenPos;
                      if (c == '=')
                      {
                          m_pStreamTop ->pszNextTokenPos++;
                          return OREQ;
                      }
                      else if (c == '|')
                      {
                          m_pStreamTop ->pszNextTokenPos++;
                          return OROR;
                      }
                      else
                          return '|';
                      break;

                  case '!':
                      c = *m_pStreamTop ->pszNextTokenPos;
                      if (c == '=')
                      {
                          m_pStreamTop ->pszNextTokenPos++;
                          return NOTEQ;
                      }
                      else
                          return '!';
                      break;

                  case '=':
                      c = *m_pStreamTop ->pszNextTokenPos;
                      if (c == '=')
                      {
                          m_pStreamTop ->pszNextTokenPos++;
                          return EQ;
                      }
                      else
                          return '=';
                      break;

                  case '<':
                      c = *m_pStreamTop ->pszNextTokenPos;
                      if (c == '=')
                      {
                          m_pStreamTop ->pszNextTokenPos++;
                          return LTEQ;
                      }
                      else if (c == '<')
                      {
                          m_pStreamTop ->pszNextTokenPos++;
                          c = *m_pStreamTop ->pszNextTokenPos;
                          if (c == '=')
                          {
                              m_pStreamTop ->pszNextTokenPos++;
                              return SLEQ;
                          }
                          else
                              return SL;
                      }
                      else
                          return '<';
                      break;

                  case '>':
                      c = *m_pStreamTop ->pszNextTokenPos;
                      if (c == '=')
                      {
                          m_pStreamTop ->pszNextTokenPos++;
                          return GTEQ;
                      }
                      else if (c == '>')
                      {
                          m_pStreamTop ->pszNextTokenPos++;
                          c = *m_pStreamTop ->pszNextTokenPos;
                          if (c == '=')
                          {
                              m_pStreamTop ->pszNextTokenPos++;
                              return SREQ;
                          }
                          else if (c == '>')
                          {
                              m_pStreamTop ->pszNextTokenPos++;
                              c = *m_pStreamTop ->pszNextTokenPos;
                              if (c == '=')
                              {
                                  m_pStreamTop ->pszNextTokenPos++;
                                  return USREQ;
                              }
                              else
                                  return USR;
                          }
                          else
                              return SR;
                      }
                      else
                          return '>';
                      break;

                  case '"':
                      {
                          char *pszStart = m_pStreamTop ->pszNextTokenPos;
                          char *pszOut = pszStart;
                          for (;;)
                          {
                              c = *m_pStreamTop ->pszNextTokenPos++;
                              if (c == '"')
                              {
                                  CNscPStackEntry *pEntry = GetPStackEntry (__FILE__, __LINE__);
                                  pEntry ->SetType (NscType_String);
                                  pEntry ->PushConstantString (pszStart, (int) (pszOut - pszStart));
                                  yylval = pEntry;
                                  return STRING_CONST;
                              }
                              else if (c == '\\')
                              {
                                  c = *m_pStreamTop ->pszNextTokenPos;
                                  if (c == 'n')
                                  {
                                      *pszOut++ = '\n';
                                      m_pStreamTop ->pszNextTokenPos++;
                                  }
                                  else
                                      ;
                              }
                              else if (c == 0)
                              {
                                  CNscPStackEntry *pEntry = GetPStackEntry (__FILE__, __LINE__);
                                  pEntry ->SetType (NscType_String);
                                  pEntry ->PushConstantString (pszStart, (int) (pszOut - pszStart));
                                  yylval = pEntry;
                                  GenerateError ("Unterminated string");
                                  return STRING_CONST; 
                              }
                              else
                                  *pszOut++ = c;
                          }
                      }
                      break;

                  default:
                      if (!IsPhase2 ())
                      {
                          GenerateWarning ("Invalid character '%c' (0x%02X) "
                                           "found in source, ignored", c, c);
                      }
                      goto try_again;
              }
          }
}

//-----------------------------------------------------------------------------
//
// @mfunc Read the next line in the current script
//
// @parm bool | fInComment | If true, don't process preprocessor statements
//              or process EOF for a stream.
//
// @rdesc Token ID.
//
//-----------------------------------------------------------------------------

bool CNscContext::ReadNextLine (bool fInComment)
{

    //
    // Read the next line
    //
try_again:;
          for (;;)
          {
              m_pStreamTop ->nLine++;
              if (m_pStreamTop ->pStream ->ReadLine (
                      m_pStreamTop ->pszLine, Max_Line_Length) == NULL)
              {
                  if (fInComment || m_pStreamTop ->pNext == NULL)
                      return false;
                  RemoveTopStream ();
              }
              else
                  break;
          }

          //
          // If we aren't in a comment, then test for preprocessor
          //

          if (!fInComment)
          {

              //
              // Search for the first non-white character
              //

              char *p = m_pStreamTop ->pszLine;
              while (*p != 0 && (*p <= ' ' || *p > 126))
                  p++;

              //
              // If this is a pre-processor statement
              //

              if (*p == '#')
              {

                  //
                  // If we have an include
                  //

                  if (strncmp (p, "#include", 8) == 0)
                  {
                      //
                      // Force a new line read on this stream on return
                      //

                      m_pStreamTop ->pszNextTokenPos = NULL;

                      //
                      // Extract the name
                      //

                      p += 8;
                      while (*p && *p != '"')
                          p++;
                      p++;
                      char *pszNameStart = p;
                      while (*p && *p != '"')
                          p++;
                      int nCount = (int) (p - pszNameStart);
                      char *pszTemp = (char *) alloca (nCount + 5);
                      memmove (pszTemp, pszNameStart, nCount);
                      pszTemp [nCount] = 0;
                      //
                      // Remove any extension
                      //

                      p = strchr (pszTemp, '.');
                      if (p)
                          *p = 0;

                      //
                      // Search the current list of included files and see
                      // if we have already done it
                      //

                      size_t i;
                      for (i = 0; i < m_asFiles .GetCount (); i++)
                      {
                          if (stricmp (m_asFiles [i] .strName .c_str (), pszTemp) == 0)
                              break;
                      }

                      //
                      // If this isn't a duplicate
                      //

                      if (i >= m_asFiles .GetCount ())
                      {

                          //
                          // Try to load the resource
                          //

                          bool fAllocated = false;
                          UINT32 ulSize = 0;
                          unsigned char *pauchData = NULL;

                          if (m_pLoader)
                          {
                              pauchData = m_pLoader ->LoadResource (
                                  pszTemp, NwnResType_NSS, &ulSize, 
                                  &fAllocated);
                          }
                          if (pauchData == NULL)
                          {
                              GenerateError ("Unable to open the "
                                             "include file \"%s\"", pszTemp);
                              return false;
                          }

                          //
                          // Add stream
                          //

                          strcat (pszTemp, ".nss");
                          CNwnStream *pStream = new CNwnMemoryStream (
                              pszTemp, pauchData, ulSize, fAllocated);
                          AddStream (pStream);
                      }

                      //
                      // Read the next line
                      //
                      goto try_again;
                  }

                  //
                  // If we have a define
                  //

                  if (strncmp (p, "#define", 7) == 0)
                  {
                      p += 7;

                      //
                      // Get the first parameter
                      //

                      while (*p && (*p <= ' ' || *p > 126))
                          p++;
                      if (*p == 0)
                      {
                          GenerateError ("#define syntax");
                          goto try_again;
                      }
                      char *pszDefine = p;
                      while (*p && (*p > ' ' && *p <= 126))
                          p++;
                      char *pszDefineEnd = p;

                      //
                      // Get the second parameter
                      //

                      while (*p && (*p <= ' ' || *p > 126))
                          p++;
                      if (*p == 0)
                      {
                          GenerateError ("#define syntax");
                          goto try_again;
                      }
                      char *pszValue = p;
                      while (*p && (*p > ' ' && *p <= 126))
                          p++;
                      char *pszValueEnd = p;

                      //
                      // Make sure there isn't anything at the end
                      //

                      while (*p && (*p <= ' ' || *p > 126))
                          p++;
                      if (*p != 0)
                      {
                          GenerateError ("#define syntax");
                          goto try_again;
                      }

                      //
                      // Copy the two strings into temp arrays
                      //

                      int nDefine = (int) (pszDefineEnd - pszDefine);
                      char *pszDTmp = (char *) alloca (nDefine + 1);
                      memcpy (pszDTmp, pszDefine, nDefine);
                      pszDTmp [nDefine] = 0;

                      int nValue = (int) (pszValueEnd - pszValue);
                      char *pszVTmp = (char *) alloca (nValue + 1);
                      memcpy (pszVTmp, pszValue, nValue);
                      pszVTmp [nValue] = 0;

                      //
                      // If this is the number of engine structures
                      //

                      if (strcmp (pszDTmp, "ENGINE_NUM_STRUCTURES") == 0)
                      {
                          // Bah, we ignore this
                      }

                      //
                      // If this is an engine structure
                      //

                      else if (strncmp (pszDTmp, "ENGINE_STRUCTURE_", 17) == 0)
                      {
                          p = &pszDTmp [17];
                          int nIndex = atol (p);
                          g_astrNscEngineTypes [nIndex] = pszVTmp;
                          if (g_sNscReservedWords .Find (pszVTmp) == NULL)
                          {
                              NscSymbol *pSymbol = g_sNscReservedWords .Add (
                                  pszVTmp, NscSymType_Token);
                              pSymbol ->nToken = ENGINE_TYPE;
                              pSymbol ->nEngineObject = nIndex;
                          }
                      }

                      //
                      // Otherwise, unknown
                      //

                      else 
                      {
                          GenerateError ("#define unknown or invalid definition");
                      }
                      goto try_again;
                  }
              }
          }

          //
          // Set the starting pointer
          //

          m_pStreamTop ->pszNextTokenPos = m_pStreamTop ->pszLine;
          return true;
}

//-----------------------------------------------------------------------------
//
// @mfunc Get a new pstack entry
//
// @rdesc Address of the pstack entry
//
//-----------------------------------------------------------------------------

#ifdef _DEBUG
CNscPStackEntry *CNscContext::GetPStackEntry (const char *pszFile, int nLine)
#else
CNscPStackEntry *CNscContext::GetPStackEntryInt ()
#endif
{

    //
    // If we can get off the free stack, then do so.  Otherwise, 
    // create a new one
    //

    CNscPStackEntry *pEntry;
    if (m_listEntryFree .GetNext () != &m_listEntryFree)
    {
        CNwnDoubleLinkList *pNext = m_listEntryFree .GetNext ();
        pEntry = (CNscPStackEntry *) pNext;
    }
    else
        pEntry = new CNscPStackEntry;

    //
    // Add to the allocated list
    //

    pEntry ->m_link .InsertTail (&m_listEntryAllocated);
    pEntry ->Initialize ();
#ifdef _DEBUG
    pEntry ->m_pszFile = pszFile;
    pEntry ->m_nLine = nLine;
#endif
    return pEntry;
}

//-----------------------------------------------------------------------------
//
// @mfunc Add initialization to an existing variable
//
// @parm NscSymbol * | pSymbol | Symbol in question
//
// @parm unsigned char * | pauchInit | Initial value
//
// @parm size_t | nInitSize | Size of the data
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNscContext::AddVariableInit (NscSymbol *pSymbol, 
                                   unsigned char *pauchInit, size_t nInitSize)
{

    //
    // Validate
    //

    assert (pSymbol ->nSymType == NscSymType_Variable);
    assert (pSymbol ->nExtra == 0);
    assert ((pSymbol ->ulFlags & NscSymFlag_Global) != 0 ||
            (pSymbol ->ulFlags & NscSymFlag_Constant) != 0);

    //
    // Add the symbol data
    //

    size_t nSymbol = m_sSymbols .GetSymbolOffset (pSymbol);
    NscSymbolVariableExtra sExtra;
    sExtra .nInitSize = nInitSize;
    sExtra .nFile = GetFile (0);
    sExtra .nLine = GetLine (0);
    size_t nExtra = m_sSymbols .AppendData (&sExtra, sizeof (sExtra));
    m_sSymbols .AppendData (pauchInit, nInitSize);

    //
    // Finish initialization
    //

    pSymbol = m_sSymbols .GetSymbol (nSymbol);
    pSymbol ->nExtra = nExtra;
}

//-----------------------------------------------------------------------------
//
// @mfunc Add a new variable to the symbol table
//
// @parm const char * | pszIdentifier | New identifier
//
// @parm NscType | nType | Type of the function
//
// @parm UINT32 | ulFlags | Symbol flags
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNscContext::AddVariable (const char *pszIdentifier, NscType nType, 
                               UINT32 ulFlags)
{

    //
    // Add a new symbol
    //

    NscSymbol *pSymbol = m_sSymbols .Add (
        pszIdentifier, NscSymType_Variable);
    pSymbol ->nType = nType;
    pSymbol ->ulFlags = ulFlags;
    pSymbol ->nStackOffset = 0;
    pSymbol ->nExtra = 0;
    pSymbol ->nCompiledStart = 0xffffffff;
    pSymbol ->nCompiledEnd = 0xffffffff;
    size_t nSymbol = m_sSymbols .GetSymbolOffset (pSymbol);

    //
    // Depending on the scope of the variable, adjust offsets and such
    //

    if (IsNWScript () || (pSymbol ->ulFlags & NscSymFlag_Constant) != 0)
    {
        assert (!IsPhase2 ());
        pSymbol ->ulFlags |= NscSymFlag_Constant;
    }
    else if (IsGlobalScope ())
    {
        assert (!IsPhase2 ());
        m_anGlobalVars .Add (nSymbol);
        m_anGlobalDefs .Add (nSymbol);
        pSymbol ->ulFlags |= NscSymFlag_Global;
    }
    else 
    {
        assert (IsPhase2 ());
        assert (m_pCurrentFence);
        pSymbol ->nStackOffset = m_pCurrentFence ->nLocals
            + m_pCurrentFence ->nPrevLocals;
        m_pCurrentFence ->nLocals += GetTypeSize (nType);
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Add a new prototype to the symbol table
//
// @parm const char * | pszIdentifier | New identifier
//
// @parm NscType | nType | Type of the function
//
// @parm UINT32 | ulFlags | Flags
//
// @parm unsigned char * | pauchArgData | Argument data 
//
// @parm size_t | nArgDataSize | Size of the data
//
// @rdesc Address of the added symbol.
//
//-----------------------------------------------------------------------------

NscSymbol *CNscContext::AddPrototype (const char *pszIdentifier, 
                                      NscType nType, UINT32 ulFlags, unsigned char *pauchArgData, 
                                      size_t nArgDataSize)
{

    //
    // Check for duplicate
    //

    NscSymbol *pSymbol = m_sSymbols .Find (pszIdentifier);
    if (pSymbol != NULL)
        return pSymbol;

    //
    // Add a new symbol
    //

    pSymbol = m_sSymbols .Add (
        pszIdentifier, NscSymType_Function);
    pSymbol ->nType = nType;
    pSymbol ->ulFlags = ulFlags;
    pSymbol ->nStackOffset = 0;
    pSymbol ->nCompiledStart = 0xffffffff;
    pSymbol ->nCompiledEnd = 0xffffffff;
    size_t nSymbol = m_sSymbols .GetSymbolOffset (pSymbol);

    //
    // Count the number of arguments
    //

    NscSymbolFunctionExtra sExtra;
    sExtra .nArgCount = 0;
    sExtra .nArgSize = 0;
    sExtra .nAction = 0;
    sExtra .nCodeOffset = 0;
    sExtra .nCodeSize = 0;
    sExtra .nFile = -1;
    sExtra .nLine = -1;
    unsigned char *pauchData = pauchArgData;
    unsigned char *pauchEnd = &pauchData [nArgDataSize];
    while (pauchData < pauchEnd)
    {
        NscPCodeHeader *p = (NscPCodeHeader *) pauchData;
        sExtra .nArgCount++;
        sExtra .nArgSize += GetTypeSize (p ->nType);
        pauchData += p ->nOpSize;
    }

    //
    // If this is a global, then set the action
    //

    if ((ulFlags & NscSymFlag_EngineFunc) != 0)
    {
        g_anNscActions .Add (nSymbol);
        sExtra .nAction = g_nNscActionCount++;
    }

    //
    // Otherwise, add this function to the global symbol table
    //

    else if (IsPhase2 ())
    {
        AddGlobalFunction (nSymbol);
    }

    //
    // Add the symbol data
    //

    size_t nExtra = m_sSymbols .AppendData (&sExtra, sizeof (sExtra));
    m_sSymbols .AppendData (pauchArgData, nArgDataSize);

    //
    // Save the data
    //

    pSymbol = m_sSymbols .GetSymbol (nSymbol);
    pSymbol ->nExtra = nExtra;
    return pSymbol;
}

//-----------------------------------------------------------------------------
//
// @mfunc Add a new structure to the symbol table
//
// @parm const char * | pszIdentifier | New identifier
//
// @parm unsigned char * | pauchStructData | Data 
//
// @parm size_t | nStructDataSize | Size of the data
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CNscContext::AddStructure (const char *pszIdentifier, 
                                unsigned char *pauchStructData, size_t nStructDataSize)
{

    //
    // Make sure we still have room
    //

    if (m_nStructs >= Max_Structs)
    {
        GenerateError ("Too many defined structures, limit of %d",
                       m_nStructs);
        return;
    }

    //
    // Add a new symbol
    //

    NscType nType = (NscType) (NscType_Struct_0 + m_nStructs);
    NscSymbol *pSymbol = m_sSymbols .Add (
        pszIdentifier, NscSymType_Structure);
    pSymbol ->nType = nType;
    pSymbol ->ulFlags = 0;
    pSymbol ->nStackOffset = 0;
    size_t nSymbol = m_sSymbols .GetSymbolOffset (pSymbol);

    //
    // Count the number of elements
    //

    NscSymbolStructExtra sExtra;
    sExtra .nElementCount = 0;
    sExtra .nTotalSize = 0;
    unsigned char *pauchData = pauchStructData;
    unsigned char *pauchEnd = &pauchData [nStructDataSize];
    while (pauchData < pauchEnd)
    {
        NscPCodeHeader *p = (NscPCodeHeader *) pauchData;
        sExtra .nElementCount++;
        sExtra .nTotalSize += GetTypeSize (p ->nType);
        pauchData += p ->nOpSize;
    }

    //
    // Add the symbol data
    //

    size_t nExtra = m_sSymbols .AppendData (&sExtra, sizeof (sExtra));
    m_sSymbols .AppendData (pauchStructData, nStructDataSize);

    //
    // Save the data
    //

    m_sSymbols .GetSymbol (nSymbol) ->nExtra = nExtra;

    //
    // Save the symbol offset
    //

    m_anStructSymbol [m_nStructs++] = nSymbol;
    m_anGlobalDefs .Add (nSymbol);
}

//-----------------------------------------------------------------------------
//
// @mfunc Get the size of a type
//
// @parm NscType |nType | Type of the declaration
//
// @rdesc Size in elements
//
//-----------------------------------------------------------------------------

int CNscContext::GetTypeSize (NscType nType)
{

    //
    // Switch based on the type
    //

    switch (nType)
    {
        case NscType_Integer:
        case NscType_Float:
        case NscType_String:
        case NscType_Object:
            return 1;

        case NscType_Vector:
            return 3;

        case NscType_Action:
            return 0;

            //
            // Default case catches all the other types
            //

        default:

            //
            // If this is an engine type
            //

            if (nType >= NscType_Engine_0 &&
                nType < NscType_Struct_0)
            {
                return 1;
            }

            //
            // If we have a structure
            //

            else if (nType >= NscType_Struct_0)
            {

                //
                // Loop through the values in the structure
                // and add them as variables.
                //

                NscSymbol *pSymbol = GetStructSymbol (nType);
                unsigned char *pauchData = m_sSymbols .GetData (pSymbol ->nExtra);
                NscSymbolStructExtra *pExtra = (NscSymbolStructExtra *) pauchData;
                return pExtra ->nTotalSize;
            }

            //
            // Otherwise, error.  Unknown type
            //

            else
            {
                return 0;
            }
            break;
    }
}

/* vim: set sw=4: */
