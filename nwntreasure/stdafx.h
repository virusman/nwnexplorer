//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      stdafx.h - Standard AFX include |
//
// This module contains the Standard AFX include.
//
// Copyright (c) 2002-2003 - Edward T. Smith
//
// @end
//
// $History: stdafx.h $
//      
//-----------------------------------------------------------------------------

//
// Disable some warnings
//

#pragma warning (disable : 4786 4018 4244 4996)

//
// Version selections
//

#define _WIN32_IE               0x0501
#define WINVER                  0x0501
#define _WIN32_WINNT    0x0501
#define VC_EXTRALEAN

#define _STLP_NO_IOSTREAMS

//
// Windows includes
//

#ifndef STRICT
#define STRICT
#endif
#include <windows.h>
#include <commctrl.h>

//
// Standard libraries
//

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <wtypes.h>

//
// ATL includes
//

#define USE_WTL70
#define _WTL_NO_CSTRING
#include <atlstr.h>                     // ATL string defintion
#include <atlbase.h>            // ATL base definitions
#include <wtlapp.h>                     // ATL main application
extern CAppModule _Module;
#include <atlwin.h>                     // ATL window support
#include <atlctrls.h>           // ATL control definition
#include <atldlgs.h>            // ATL dialogs
#include <atlframe.h>           // ATL frame window
#include <atlctrlw.h>           // ATL control definitions
#include <atlmisc.h>            // ATL assorted items
#include <atlctrlx.h>           // ATL control definition
#include <atlscrl.h>            // ATL control definition
#include <atlprint.h>           // ATL printing
#include <atlcoll.h>            // ATL collection
#include <atltheme.h>           // ATL theme support (XP)
#include <atlsplit.h>

//
// Get EXPAT to define things locally
//

#define XMLPARSEAPI(type) type

//
// Handy definitions
//

#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

//
// Debug macros
//

#ifdef _DEBUG
#define UNUSED(x)
#else
#define UNUSED(x) x
#endif
#define UNUSED_ALWAYS(x) x

//
// Define a TRACE
//

#ifndef TRACE
#ifdef _DEBUG
#define TRACE ATLTRACE
#else
inline void TRACE (LPCTSTR pszFormat,...) {}
#endif
#endif

//
// Debug helper
//

#if defined (_DEBUG)
#ifndef DEBUG_NEW
#define DEBUG_NEW new (_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
inline bool DtEnableMemoryTracking (bool fTrack)
{
    int nOldState = _CrtSetDbgFlag (_CRTDBG_REPORT_FLAG);
    if (fTrack)
        _CrtSetDbgFlag (nOldState | _CRTDBG_ALLOC_MEM_DF);
    else
        _CrtSetDbgFlag (nOldState & ~_CRTDBG_ALLOC_MEM_DF);
    return nOldState & _CRTDBG_ALLOC_MEM_DF;
};
#else
#define DtEnableMemoryTracking(x) FALSE
#endif

/* vim: set sw=4: */
