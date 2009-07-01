//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnMdlComp.cpp - NWN Model compiler/decompiler |
//
// This module contains the Nwn model compiler/decompiler.
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

#include "../_NmcLib/Nmc.h"
#include "../_NwnLib/NwnModel.h"
#include "../_NwnLib/NwnStdLoader.h"

#ifdef _WIN32
#include <atlbase.h>
#include <io.h>
#endif
#ifdef HAVE_LIBIBERTY_H
#include <libiberty.h>
#elif HAVE_LIBGEN_H
#include <libgen.h>
#endif

//
// Compile flags
//

bool g_fCompile = true;
bool g_fExtract = false;
bool g_fPurgeNullFaces = true;
bool g_fDisableExtension = false;
bool g_fQuiet = false;
int g_nTest = 0;

//
// Test flags
//

int g_nWarnings = 0;
int g_nErrors = 0;
int g_nMismatch = 0;

//
// Other globals
//

CNwnStdLoader g_sLoader;
CNmcCache g_sCache;

//
// Callbacks
//

typedef void (FN_EBMCALLBACK) (const CNwnKeyFile::Resource *psRes,
                               unsigned char *pauchData, UINT32 ulSize, bool fAllocated);

//
// Ensure cleared memory for allocation
//

void *operator new (size_t size) {
    void *p = calloc(1, size);
    if (p == NULL)
        throw std::bad_alloc();
    return p;
}

void operator delete (void *p) {
    free(p);
}

//-----------------------------------------------------------------------------
//
// @func Enum the binary models
//
// @parm FN_EBMCALLBACK | pfnCallback | Routine to be invoked
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void EnumBinaryModels (FN_EBMCALLBACK *pfnCallback)
{
    CNwnKeyFile &sKeyFile = *g_sLoader .GetNthKeyFile (0);

    //
    // Loop through the resources
    //

    for (int i = 0; i < sKeyFile .GetResCount (); i++)
        //      for (int i = 3714; i <= 3714; i++)
    {

        //
        // Skip if not a model
        //

        const CNwnKeyFile::Resource *psRes = sKeyFile .GetRes (i);
        if (psRes ->usType != NwnResType_MDL)
            continue;

        //
        // Print the name of the model as we process
        //

        char szName [17];
        memmove (szName, psRes ->szName, 16);
        szName [16] = 0;
        printf ("Model %s (%d) - ", szName, i);

        //
        // Read the model
        //

        bool fAllocated;
        UINT32 ulSize;
        long lUnknown = 0;
        unsigned char *pauchData = sKeyFile .LoadRes (
            psRes, &ulSize, &fAllocated);

        //
        // If failed, then skip
        //

        if (pauchData == NULL || ulSize < 12)
        {
            printf ("Unable to read or invalid model\n");
            continue;
        }

        //
        // If not binary, skip
        //

        if (pauchData [0] != 0 || pauchData [1] != 0 ||
            pauchData [2] != 0 || pauchData [3] != 0)
        {
            if (fAllocated)
                free (pauchData);
            printf ("ASCII Model, skipped\n");
            continue;
        }

        //
        // Invoke routine
        //

        pfnCallback (psRes, pauchData, ulSize, fAllocated);
    }
}

//-----------------------------------------------------------------------------
//
// @func Callback for test 1
//
// @parm const CNwnKeyFile::Resource * | psRes | Resource in question
//
// @parm unsigned char * | pauchData | Resource data
//
// @parm UINT32 | ulSize | Length of the resource
//
// @parm bool | fAllocated | true if the resource is allocated
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void Test1Callback (const CNwnKeyFile::Resource *psRes,
                    unsigned char *pauchData, UINT32 ulSize, bool fAllocated)
{
    CNwnModel sMdl (pauchData, ulSize, fAllocated);
    CNwnMemoryStream sStream;
    CNwnMdlModel *pModel = (CNwnMdlModel *) sMdl .GetModelData (0);
    pModel ->Decompile (&sStream, true, true, true,
                        sMdl .GetModelData (), sMdl .GetRawData ());
    printf ("Passed\n");
}

//-----------------------------------------------------------------------------
//
// @func Callback for test 2
//
// @parm const CNwnKeyFile::Resource * | psRes | Resource in question
//
// @parm unsigned char * | pauchData | Resource data
//
// @parm UINT32 | ulSize | Length of the resource
//
// @parm bool | fAllocated | true if the resource is allocated
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void Test2Callback (const CNwnKeyFile::Resource *psRes,
                    unsigned char *pauchData, UINT32 ulSize, bool fAllocated)
{

    //
    // First, decompile the model
    //

    CNwnModel sMdl (pauchData, ulSize, fAllocated);
    CNwnMemoryStream sDecomp1;
    CNwnMdlModel *pModel = (CNwnMdlModel *) sMdl .GetModelData (0);
    pModel ->Decompile (&sDecomp1, true, true, g_nTest == 4,
                        sMdl .GetModelData (), sMdl .GetRawData ());

    //
    // Next, recompile the model
    //

    CNmcContext sCtx;
    for (int i = 0; i < (int) g_sLoader .GetKeyFileCount (); i++)
        sCtx .AddKeyFile (g_sLoader .GetNthKeyFile (i));
    sCtx .SetCache (&g_sCache);
    sCtx .SetPurgeNullFaces (false); // otherwise we get 50 errors
    CNwnMemoryStream *pStream = new 
        CNwnMemoryStream ("internal", sDecomp1 .GetData (), 
                          sDecomp1 .GetLength (), false);
    sCtx .AddStream (pStream);
    NmcParseModelFile (&sCtx);

    //
    // Inspect the errors and warnings
    //

    int nErrors = sCtx .GetErrors ();
    int nWarnings = sCtx .GetWarnings ();
    g_nErrors += nErrors;
    g_nWarnings += nWarnings;
    if (nErrors != 0)
    {
        printf ("Failed with %d errors and %d warnings\n",
                nErrors, nWarnings);
        return;
    }

    //
    // If this is a t3, then make relative
    //

    pModel = sCtx .GetCurrentModel ();
    unsigned char *pauchModel = NULL;
    unsigned char *pauchRaw = NULL;
    CNwnMemoryStream sRelModel;
    CNwnMemoryStream sRelRaw;
    if (g_nTest == 3)
    {
        pModel ->Serialize (&sRelModel, &sRelRaw);
        pauchModel = sRelModel .GetData ();
        pauchRaw = sRelRaw .GetData ();
        pModel = (CNwnMdlModel *) pauchModel;
    }

    // 
    //
    // Decompile the compiled version and free it (we don't need it anymore)
    //

    CNwnMemoryStream sDecomp2;
    pModel ->Decompile (&sDecomp2, true, true, 
                        g_nTest == 4, pauchModel, pauchRaw);
    delete sCtx .GetCurrentModel ();

    //
    // Create two compiler contexts that we will use to iterate through the
    // ASCII files
    //

    CNmcContext sCtx1;
    CNwnMemoryStream *pStream1 = new 
        CNwnMemoryStream ("internal", sDecomp1 .GetData (), 
                          sDecomp1 .GetLength (), false);
    sCtx1 .AddStream (pStream1);
    CNmcContext sCtx2;
    CNwnMemoryStream *pStream2 = new 
        CNwnMemoryStream ("internal", sDecomp2 .GetData (), 
                          sDecomp2 .GetLength (), false);
    sCtx2 .AddStream (pStream2);

    //
    // Loop through the text
    //

    int nLine = 0;
    while (true)
    {

        //
        // Read the next line
        //

        nLine++;
        bool fR1 = sCtx1 .ReadNextLine ();
        bool fR2 = sCtx2 .ReadNextLine ();
        if (fR1 != fR2)
        {
            g_nMismatch++;
            printf ("*** Mismatch - different number of files\n");
            return;
        }
        if (!fR1)
            break;

        //
        // Do a simple line compare
        //

        if (strcmp (sCtx1 .GetCurrentLine (),
                    sCtx2 .GetCurrentLine ()) == 0)
            continue;

        //
        // Otherwise, we do a token compare
        //

        while (true)
        {
            const char *pszT1 = sCtx1 .GetNextToken ();
            const char *pszT2 = sCtx2 .GetNextToken ();
            if (pszT1 == NULL && pszT2 == NULL)
                break;
            if (pszT1 == NULL || pszT2 == NULL)
            {
                g_nMismatch++;
                printf ("*** Mismatch at line %d\n%s%s", nLine, 
                        sCtx1 .GetCurrentLine (), 
                        sCtx2 .GetCurrentLine ());
                return;
            }
            if (strcmp (pszT1, pszT2) == 0)
                continue;
            char *pszTE1 = NULL;
            double dReturn1 = strtod (pszT1, &pszTE1);
            char *pszTE2 = NULL;
            double dReturn2 = strtod (pszT2, &pszTE2);
            bool fError;
            if (*pszTE1 != 0 || *pszTE2 != 0)
                fError = true;
            else if (fabs (dReturn1 - dReturn2) > 0.00001 && 
                     fabs ((dReturn1 - dReturn2) / (dReturn1 + dReturn2)) > 0.00001)
                fError = true;
            else
                fError = false;
            if (fError)
            {
                g_nMismatch++;
                printf ("*** Mismatch at line %d\n%s%s", nLine, 
                        sCtx1 .GetCurrentLine (), 
                        sCtx2 .GetCurrentLine ());
                return;
            }
        }
    }

    if (nWarnings != 0)
        printf ("Passed with %d warnings\n", nWarnings);
    else
        printf ("Passed\n");
}

//-----------------------------------------------------------------------------
//
// @func Make output file name
//
// @parm const char * | pszInFile | Current input file
//
// @parm const char * | pszOutFile | Default output file
//
// @parm const char * | pszToReplaceExt | Extension to replace
//
// @parm const char * | pszReplaceWithExt | Extension to replace with
//
// @parm const char * | pszDefaultExt | Default extension
//
// @rdesc Pointer to the file name
//
//-----------------------------------------------------------------------------

const char *MakeOutFile (const char *pszInFile, const char *pszOutFile,
                         const char *pszToReplaceExt, const char *pszReplaceWithExt, 
                         const char *pszDefaultExt)
{
    static char szOutName [512];
    bool fDoExt = false;

    //
    // If we don't have an output file name
    //

    if (pszOutFile == NULL || pszOutFile [0] == 0)
    {
        static const char *pszExt = ".mdl.ascii";

        //
        // Start with the input file name
        //

        strcpy (szOutName, pszInFile);
        pszOutFile = szOutName;

        //
        // We need to process the ext
        //

        fDoExt = true;
    }

    //
    // Otherwise, we have an output file name, but it might be a directory
    //

    else
    {

        //
        // If it is just a path
        //

        int nLength = (int) strlen (pszOutFile);
        if (pszOutFile [nLength - 1] == '\\' ||
            pszOutFile [nLength - 1] == '/')
        {

            //
            // Start with the input file name
            //

            strcpy (szOutName, pszOutFile);

            //
            // Append the base name of the input file name
            //

            strcat (szOutName, NwnBasename (pszInFile));
            pszOutFile = szOutName;

            //
            // We need to process the ext
            //

            fDoExt = true;
        }
    }

    //
    // If we need to process the extension
    //

    if (fDoExt)
    {

        //
        // Make the extension
        //

        bool fUseDefault = pszDefaultExt != NULL && !g_fDisableExtension;
        if (pszToReplaceExt != NULL && pszReplaceWithExt != NULL)
        {
            int nLen = (int) strlen (szOutName);
            int nLen2 = (int) strlen (pszToReplaceExt);
            if (nLen > nLen2 && stricmp (&szOutName 
                                         [nLen - nLen2], pszToReplaceExt) == 0)
            {
                strcpy (&szOutName [nLen - nLen2], pszReplaceWithExt);
                fUseDefault = false;
            }
        }

        //
        // Use default if needed
        //

        if (fUseDefault)
            strcat (szOutName, pszDefaultExt);
    }
    return pszOutFile;
}

//-----------------------------------------------------------------------------
//
// @func Compile a file
//
// @parm unsigned char * | pauchData | Model data
//
// @parm UINT32 | ulSize | Size of the file in bytes
//
// @parm const char * | pszInFile | Input file name
//
// @parm const char * | pszOutFile | Output file name (can be NULL)
//
// @rdesc Decompile status
//
//-----------------------------------------------------------------------------

bool Compile (unsigned char *pauchData, UINT32 ulSize, 
              const char *pszInFile, const char *pszOutFile)
{
    //
    // Issue message
    //

    printf ("Compiling: %s\n", pszInFile);

    //
    // Make sure it looks ascii
    //

    int nLength = 128;
    if (nLength > (int) ulSize)
        nLength = ulSize;
    for (int i = 0; i < nLength; i++)
    {
        if (pauchData [nLength] > 127 ||
            (pauchData [nLength] < 32 && 
             pauchData [nLength] != '\t' &&
             pauchData [nLength] != '\r' &&
             pauchData [nLength] != '\n'))
        {
            printf ("Error: File \"%s\" isn't an ASCII file\n", 
                    pszInFile);
            return false;
        }
    }

    //
    // Compile
    //

    CNmcContext sCtx;
    for (int i = 0; i < (int) g_sLoader .GetKeyFileCount (); i++)
        sCtx .AddKeyFile (g_sLoader .GetNthKeyFile (i));
    sCtx .SetCache (&g_sCache);
    sCtx .SetPurgeNullFaces (g_fPurgeNullFaces);
    CNwnMemoryStream *pStream = new 
        CNwnMemoryStream (pszInFile, pauchData, ulSize);
    sCtx .AddStream (pStream);
    NmcParseModelFile (&sCtx);

    //
    // If we have an error, return
    //

    if (sCtx .GetErrors () > 0)
    {
        printf ("Compilation aborted with errors\n");
        return false;
    }

    //
    // Make sure we have a model
    //

    if (sCtx .GetCurrentModel () == NULL)
    {
        printf ("Error: File \"%s\" doesn't contain a valid model\n", 
                pszInFile);
        return false;
    }

    //
    // Create the output
    //

    CNwnMemoryStream sModelStream;
    CNwnMemoryStream sRawStream;
    sCtx .GetCurrentModel () ->Serialize (
        &sModelStream, &sRawStream);

    //
    // If need be, generate output file name
    //

    pszOutFile = MakeOutFile (pszInFile, pszOutFile, 
                              ".mdl.ascii", ".mdl", ".mdl");

    //
    // Create the output file
    //

    FILE *pf = fopen (pszOutFile, "wb");
    if (pf == NULL)
    {
        printf ("Error: Unable to open output file \"%s\"\n", pszOutFile);
        return false;
    }
    CNwnModel::Header hdr;
    hdr .ulZero = 0;
    hdr .ulRawDataOffset = (UINT32) sModelStream .GetLength ();
    hdr .ulRawDataSize = (UINT32) sRawStream .GetLength ();
    fwrite (&hdr, sizeof (hdr), 1, pf);
    fwrite (sModelStream .GetData (), sModelStream .GetPosition (), 1, pf);
    fwrite (sRawStream .GetData (), sRawStream .GetPosition (), 1, pf);
    fclose (pf);

    //
    // Free the model
    //

    delete sCtx .GetCurrentModel ();
    return true;
}

//-----------------------------------------------------------------------------
//
// @func Decompile a file
//
// @parm unsigned char * | pauchData | Model data
//
// @parm UINT32 | ulSize | Size of the file in bytes
//
// @parm const char * | pszInFile | Input file name
//
// @parm const char * | pszOutFile | Output file name (can be NULL)
//
// @rdesc Decompile status
//
//-----------------------------------------------------------------------------

bool Decompile (unsigned char *pauchData, UINT32 ulSize, 
                const char *pszInFile, const char *pszOutFile)
{

    //
    // Issue message
    //

    printf ("Decompiling: %s\n", pszInFile);

    //
    // Create the model
    //

    CNwnModel sMdl (pauchData, ulSize, true);

    //
    // Verify this is binary
    //

    if (pauchData [0] != 0)
    {
        printf ("Error: \"%s\" isn't a binary model file\n", pszInFile);
        return false;
    }

    //
    // Make ASCII
    //

    CNwnMemoryStream sStream;
    CNwnMdlModel *pModel = (CNwnMdlModel *) sMdl .GetModelData (0);
    pModel ->Decompile (&sStream, true, false, true,
                        sMdl .GetModelData (), sMdl .GetRawData ());

    //
    // Create the output file
    //

    pszOutFile = MakeOutFile (pszInFile, pszOutFile, NULL, NULL, ".ascii");
    FILE *pf = fopen (pszOutFile, "wb");
    if (pf == NULL)
    {
        printf ("Error: Unable to open output file \"%s\"\n", pszInFile);
        return false;
    }
    fwrite (sStream .GetData (), sStream .GetPosition (), 1, pf);
    fclose (pf);
    return true;
}

//-----------------------------------------------------------------------------
//
// @func Do wildcard based compile/decompile
//
// @parm const char * | pszInFile | Input file
//
// @parm const char * | pszOutFile | Output file
//
// @rdesc Number of files found
//
//-----------------------------------------------------------------------------

int Wildcard (const char *pszInFile, const char *pszOutFile)
{
#ifdef _WIN32
    struct _finddata_t sFind;

    //
    // Split the input file name up
    //

    char szDrive [_MAX_DRIVE];
    char szDir [_MAX_DIR];
    char szFileName [_MAX_FNAME];
    char szExtension [_MAX_EXT];
    _splitpath (pszInFile, szDrive, szDir, szFileName, szExtension);

    //
    // Make a root name
    //

    char szInFile [_MAX_PATH];
    strcpy (szInFile, szDrive);
    strcat (szInFile, szDir);
    int nRootLength = (int) strlen (szInFile);

    //
    // Initiate the find
    //

    intptr_t id = _findfirst (pszInFile, &sFind); 
    if (id <= 0)
        return 0;

    //
    // Loop through the files
    //

    int nCount = 0;
    do
    {

        //
        // If system, hidden, or subdir, then skip
        //

        if ((sFind .attrib & (_A_HIDDEN | _A_SYSTEM | _A_SUBDIR)) != 0)
            continue;

        //
        // Create the file name
        //

        strcpy (&szInFile [nRootLength], sFind .name);

        //
        // Load the file
        //

        UINT32 ulSize;
        unsigned char *pauchData = NULL;
        pauchData = NwnLoadFile (szInFile, &ulSize);

        //
        // Compile/decompile
        //

        if (pauchData == NULL)
        {
            printf ("Error: Unable to open file %s\n", sFind .name);
            continue;
        }

        //
        // Compile/decompile
        //

        if (g_fCompile)
            Compile (pauchData, ulSize, sFind .name, pszOutFile);
        else
            Decompile (pauchData, ulSize, sFind .name, pszOutFile);
        nCount++;
    } while (_findnext (id, &sFind) >= 0);

    //
    // Close the find
    //

    _findclose (id);
    return nCount;
#else

    //
    // Load the file
    //

    UINT32 ulSize;
    unsigned char *pauchData = NULL;
    pauchData = NwnLoadFile (pszInFile, &ulSize);
    if (pauchData == NULL)
        return 0;

    //
    // Compile
    //

    if (g_fCompile)
        Compile (pauchData, ulSize, pszInFile, pszOutFile);
    else
        Decompile (pauchData, ulSize, pszInFile, pszOutFile);
    return 1;
#endif
}

//-----------------------------------------------------------------------------
//
// @func Match a simple pattern
//
// @parm const char * | pszString | String to test
//
// @parm const char * | pszPattern | Pattern string
//
// @rdesc TRUE if the pattern matches
//
//-----------------------------------------------------------------------------

bool MatchPattern (const char *pszString, const char *pszPattern)
{
    for (;;)
    {
        char p = *pszPattern++;
        p = tolower (p);
        switch (p)
        {


            //
            // If end of pattern, return success if end of string reached
            //

            case 0:
                return *pszString == 0;

                //
                // If matching zero or more characters
                //

            case '*':
                while (*pszString) 
                {
                    if (MatchPattern (pszString++, pszPattern))
                        return true; 
                }
                return MatchPattern (pszString, pszPattern);

                //
                // Match any one character but not end of string
                //

            case '?':
                if (*pszString++ == 0)
                    return false;
                break; 

            default:
                {
                    char c = *pszString++;
                    c = tolower (c);
                    if (c != p)
                        return false;
                }
                break; 
        } 
    } 
}

//-----------------------------------------------------------------------------
//
// @func The main
//
// @parm int | argc | Argument count
//
// @parm char * | argv [] | Argument list
//
// @rdesc Exit status
//
//-----------------------------------------------------------------------------

int main (int argc, char *argv [])
{
    char *pszOutFile = NULL;
    char *pszNWNDir = NULL;
    char **papszInFiles = NULL;
    int nInFileCount = 0;

    //
    // Enable leak checking
    //

#ifdef _WIN32
#if defined (_DEBUG)
    _CrtSetDbgFlag (_CrtSetDbgFlag (_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif
#endif

    //
    // Get start time
    //

#ifdef _WIN32
    DWORD dwT1 = GetTickCount ();
#endif

    //
    // Allocate an array that will hold the input file names.  On
    // a WIN32 system, this will always max out at one entry.  However
    // on a Linux system, it can be a list up to the number of arguments
    //

    papszInFiles = new char * [argc];

    //
    // Loop through the arguments
    //

    bool fError = false;
    for (int i = 1; i < argc && !fError; i++)
    {

        //
        // If this is a switch
        //

        if (argv [i] [0] == '-')
        {
            char *p = &argv [i] [1];
            while (*p && !fError)
            {
                switch (tolower (*p))
                {
                    case 'd':
                        g_fCompile = false;
                        break;
                    case 'c':
                        g_fCompile = true;
                        break;
                    case 'x':
                        g_fExtract = true;
                        break;
                    case 'n':
                        g_fPurgeNullFaces = false;
                        break;
                    case 'e':
                        g_fDisableExtension = true;
                        break;
                    case 'q':
                        g_fQuiet = true;
                        break;
                    case 't':
                        {
                            char c = p [1];
                            if (c < '1' || c > '4')
                            {
                                printf ("Error: Unrecognized test number\n");
                                fError = true;
                            }
                            else
                                g_nTest = c - '0';
                            p++;
                        }
                        break;
                    default:
                        printf ("Error: Unrecognized option \"%c\"\n", *p);
                        fError = true;
                        break;
                }
                p++;
            }
        }

        //
        // Otherwise, save the name
        //

#ifdef _WIN32
        else if (nInFileCount == 0)
        {
            papszInFiles [nInFileCount++] = argv [i];
        }
        else if (pszOutFile == NULL)
        {
            pszOutFile = argv [i];
        }
        else
        {
            printf ("Error: Too many arguments\n");
            fError = true;
            break;
        }
#else
        else if (pszNWNDir == NULL)
        {
            pszNWNDir = argv [i];
        }
        else
        {
            papszInFiles [nInFileCount++] = argv [i];
        }
#endif
    }

    //
    // Advertise
    //

    if (!g_fQuiet) {
        printf ("NeverWinter Nights Model Compiler/Decompiler\n");
        printf ("Copyright 2002-2003, Edward T. Smith\n");
        printf ("Copyright 2003, The Open Knights Consortium\n\n");
    }

    //
    // Check for using -e with no output
    //

    if (g_fDisableExtension && (pszOutFile == NULL && !g_fExtract))
    {
        printf ("Error: To use -e, you must specify an output file or path\n");
        printf ("       or be extracting files using the -x option.\n");
        exit (0);
    }

    //
    // If there is an error, display the help
    //

    if (fError || (nInFileCount == 0 && g_nTest == 0))
    {
        printf ("Usage:\n");
#ifdef _WIN32
        printf ("nwnmdlcomp [-cdxe] [-t#] infile [outfile]\n\n");
#else
        printf ("nwnmdlcomp [-cdxe] [-t#] nwndir infile\n\n");
        printf ("  nwndir - directory where NWN is installed.\n");
#endif
        printf ("  infile - name of the input file.\n");
#ifdef _WIN32
        printf ("  outfile - name of the output file.\n");
#endif
        printf ("  -c - Compile the model (default)\n");
        printf ("  -d - Decompile the model (can't be used with -c)\n");
        printf ("  -x - Extract model from NWN data files\n");
        printf ("  -n - When compiling, don't remove empty faces\n");
        printf ("  -e - Disable appended extension mode.  Only usable when an\n");
        printf ("       output file name or path is specified or extracting\n");
        printf ("       files from the NWN data files using -x.\n");
        printf ("  -t1 - Perform a decompilation test on all Bioware models\n");
        printf ("  -t2 - Perform a decomp/recomp test on all Bioware models (absolute)\n");
        printf ("  -t3 - Perform a decomp/recomp test on all Bioware models (relative)\n");
        printf ("  -t4 - Perform a decomp/recomp test on all Bioware models (smoothing)\n");
        exit (0);
    }

    //
    // We must be able to open the file
    //

    if (!g_sLoader .Initialize (pszNWNDir) && (g_fCompile || g_fExtract))
    {
        printf ("Unable to locate or open Neverwinter Night\n");
        exit (0);
    }

    //
    // If we are testing decompilation
    //

    if (g_nTest == 1)
    {

        //
        // Enum the models
        //

        EnumBinaryModels (Test1Callback);
    }

    //
    // If we are testing compilation
    //

    else if (g_nTest == 2 || g_nTest == 3)
    {

        //
        // Enum the models
        //

        EnumBinaryModels (Test2Callback);
        printf ("Totals: %d Mismatches, %d Errors, %d Warnings\n", 
                g_nMismatch, g_nErrors, g_nWarnings);
        printf ("*** NOTE: One model will fail to compile because the \n");
        printf ("    c_raven model is missing.  Thus, the model that uses\n");
        printf ("    it as a supermodel can't compile.\n\n");
        printf ("*** NOTE: Test 4 is known to generate mismatchs\n");
        printf ("    for 3 percent of all models\n");
    }

    //
    // If we are to extract
    //

    else if (g_fExtract)
    {

        //
        // Get to the root file name
        //

        char *pszOrgInFile = papszInFiles [0];
        char szPattern [512];
        strcpy (szPattern, papszInFiles [0]);
        char *p = (char *)strchr (NwnBasename (szPattern), '.');
        if (p) 
            *p = 0;

        //
        // Loop through the resources
        //

        int nCount = 0;
        CNwnKeyFile &sKeyFile = *g_sLoader .GetNthKeyFile (0);
        for (int n = 0; n < sKeyFile .GetResCount (); n++)
        {
            CNwnKeyFile::Resource *pRes = sKeyFile .GetRes (n);
            if (pRes ->usType != NwnResType_MDL)
                continue;
            char szName [32];
            memcpy (szName, pRes ->szName, 16);
            szName [16] = 0;
            if (MatchPattern (szName, szPattern))
            {
                UINT32 ulSize;
                unsigned char *pauchData = NULL;
                pauchData = sKeyFile .LoadRes (pRes, &ulSize, NULL);
                if (pauchData == NULL)
                {
                    printf ("Error: Unable to open extraction input file \"%s\"\n", szName);
                }
                else
                {
                    strcat (szName, ".mdl");
                    if (g_fCompile)
                        Compile (pauchData, ulSize, szName, pszOutFile);
                    else
                        Decompile (pauchData, ulSize, szName, pszOutFile);
                    nCount++;
                }
            }
        }

        //
        // Issue error if nothing found
        //

        if (nCount == 0)
        {
            printf ("Error: Unable to open extraction input file \"%s\"\n", pszOrgInFile);
            exit (0);
        }
    }

    //
    // Otherwise, normal operation
    //

    else
    {

        //
        // Loop through the files
        //

        for (int i = 0; i < nInFileCount; i++)
        {

            //
            // Try the wildcards
            //

            char *pszOrgInFile = papszInFiles [i];
            char *pszInFile = papszInFiles [i];
            char szInFile [512];
            int nCount = Wildcard (pszInFile, pszOutFile);
            if (nCount == 0 && g_fCompile)
            {
                strcpy (szInFile, pszOrgInFile);
                strcat (szInFile, ".mdl.ascii");
                pszInFile = szInFile;
                nCount = Wildcard (pszInFile, pszOutFile);
            }
            if (nCount == 0)
            {
                strcpy (szInFile, pszOrgInFile);
                strcat (szInFile, ".mdl");
                pszInFile = szInFile;
                nCount = Wildcard (pszInFile, pszOutFile);
            }
            if (nCount == 0)
            {
                printf ("Error: Unable to open input file \"%s\"\n", pszOrgInFile);
                exit (0);
            }
        }
    }

    //
    // Delete the input file pointers
    //

    delete [] papszInFiles;

    //
    // Close the key file for good measure
    //

    g_sCache .ClearCache ();
    g_sLoader .Close ();

    //
    // Write time
    //

#ifdef _WIN32
    if (!g_fQuiet)
        printf ("Total Execution time = %d ms\n", GetTickCount () - dwT1);
#endif
}

/* vim: set sw=4: */
