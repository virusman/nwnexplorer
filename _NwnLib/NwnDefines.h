#ifndef ETS_NWNDEFINES_H
#define ETS_NWNDEFINES_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      NwnDefines.h - Global definitions |
//
// This module contains the definition of the global values.
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

#ifdef _WIN32
#include "../win32_config.h"
#else
#include "../config.h"
#endif

#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif
#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif
#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif
#ifdef HAVE_LIBIBERTY_H
#include <libiberty.h>
#elif HAVE_LIBGEN_H
#include <libgen.h>
#endif

#include <string>

#ifdef _CRT_SECURE_NO_WARNINGS
#define stricmp _stricmp
#define strnicmp _strnicmp
#define strlwr _strlwr
#endif

//-----------------------------------------------------------------------------
//
// Types
//
//-----------------------------------------------------------------------------

#ifdef _WIN32
typedef unsigned __int64 UINT64;
typedef signed __int64 INT64;
#else
typedef unsigned long long UINT64;
typedef signed long long INT64;
#endif
typedef unsigned int UINT32;
typedef signed int INT32;
typedef unsigned short UINT16;
typedef signed short INT16;
typedef unsigned char UINT8;
typedef signed char INT8;

//-----------------------------------------------------------------------------
//
// Handy definitions
//
//-----------------------------------------------------------------------------

#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

//-----------------------------------------------------------------------------
//
// Cross platform routines
//
//-----------------------------------------------------------------------------

#ifndef HAVE_STRICMP
#ifndef HAVE_STRCASECMP
int stricmp (const char *string1, const char *string2);
#else
#define stricmp strcasecmp
#endif
#endif

#ifndef HAVE_STRNICMP
#ifndef HAVE_STRNCASECMP
int strnicmp (const char *string1, const char *string2, size_t count);
#else
#define strnicmp strncasecmp
#endif
#endif

#ifndef HAVE_SNPRINTF
#ifndef HAVE__SNPRINTF
int snprintf (char *buffer, size_t count, const char *format, ...);
#else
#define snprintf _snprintf
#endif
#endif

#ifndef HAVE_STRLWR
char *strlwr (char *string);
#endif

#ifndef _WIN32
#define __cdecl __attribute__((__cdecl__))
#endif

//-----------------------------------------------------------------------------
//
// Enumerations
//
//-----------------------------------------------------------------------------

enum NwnResType
{
    NwnResType_RES              = 0x0000,
    NwnResType_BMP              = 0x0001,
    NwnResType_MVE              = 0x0002,
    NwnResType_TGA              = 0x0003,
    NwnResType_WAV              = 0x0004,
    NwnResType_PLT              = 0x0006,
    NwnResType_INI              = 0x0007,
    NwnResType_BMU              = 0x0008,
    NwnResType_MPG              = 0x0009,
    NwnResType_TXT              = 0x000A,
    NwnResType_PLH              = 0x07D0,
    NwnResType_TEX              = 0x07D1,
    NwnResType_MDL              = 0x07D2,
    NwnResType_THG              = 0x07D3,
    NwnResType_FNT              = 0x07D5,
    NwnResType_LUA              = 0x07D7,
    NwnResType_SLT              = 0x07D8,
    NwnResType_NSS              = 0x07D9,
    NwnResType_NCS              = 0x07DA,
    NwnResType_MOD              = 0x07DB,
    NwnResType_ARE              = 0x07DC,
    NwnResType_SET              = 0x07DD,
    NwnResType_IFO              = 0x07DE,
    NwnResType_BIC              = 0x07DF,
    NwnResType_WOK              = 0x07E0,
    NwnResType_2DA              = 0x07E1,
    NwnResType_TLK              = 0x07E2,
    NwnResType_TXI              = 0x07E6,
    NwnResType_GIT              = 0x07E7,
    NwnResType_BTI              = 0x07E8,
    NwnResType_UTI              = 0x07E9,
    NwnResType_BTC              = 0x07EA,
    NwnResType_UTC              = 0x07EB,
    NwnResType_DLG              = 0x07ED,
    NwnResType_ITP              = 0x07EE,
    NwnResType_BTT              = 0x07EF,
    NwnResType_UTT              = 0x07F0,
    NwnResType_DDS              = 0x07F1,
    NwnResType_UTS              = 0x07F3,
    NwnResType_LTR              = 0x07F4,
    NwnResType_GFF              = 0x07F5,
    NwnResType_FAC              = 0x07F6,
    NwnResType_BTE              = 0x07F7,
    NwnResType_UTE              = 0x07F8,
    NwnResType_BTD              = 0x07F9,
    NwnResType_UTD              = 0x07FA,
    NwnResType_BTP              = 0x07FB,
    NwnResType_UTP              = 0x07FC,
    NwnResType_DTF              = 0x07FD,
    NwnResType_GIC              = 0x07FE,
    NwnResType_GUI              = 0x07FF,
    NwnResType_CSS              = 0x0800,
    NwnResType_CCS              = 0x0801,
    NwnResType_BTM              = 0x0802,
    NwnResType_UTM              = 0x0803,
    NwnResType_DWK              = 0x0804,
    NwnResType_PWK              = 0x0805,
    NwnResType_BTG              = 0x0806,
    NwnResType_UTG              = 0x0807,
    NwnResType_JRL              = 0x0808,
    NwnResType_SAV              = 0x0809,
    NwnResType_UTW              = 0x080A,
    NwnResType_4PC              = 0x080B,
    NwnResType_SSF              = 0x080C,
    NwnResType_HAK              = 0x080D,
    NwnResType_NWM              = 0x080E,
    NwnResType_BIK              = 0x080F,
    NwnResType_NDB              = 0x0810,
    NwnResType_PTM              = 0x0811,
    NwnResType_PTT              = 0x0812,
    NwnResType_BAK              = 0x0813,
    NwnResType_DAT              = 0x0814,
    NwnResType_SHD              = 0x0815,
    NwnResType_XBC              = 0x0816,
    NwnResType_WBM              = 0x0817,
    NwnResType_MTR              = 0x0818,
    NwnResType_ERF              = 0x270D,
    NwnResType_BIF              = 0x270E,
    NwnResType_KEY              = 0x270F,

    //
    // The following are not Bioware IDS but used internally
    // in this software
    //

    NwnResType_Unknown  = 0xFFFFFFFF,
};

enum NwnPalette
{
    NwnPalette_Skin             = 0,
    NwnPalette_Hair             = 1,
    NwnPalette_Metal1   = 2,
    NwnPalette_Metal2   = 3,
    NwnPalette_Cloth1   = 4,
    NwnPalette_Cloth2   = 5,
    NwnPalette_Leather1 = 6,
    NwnPalette_Leather2 = 7,
    NwnPalette_Tattoo1  = 8,
    NwnPalette_Tattoo2  = 9,
    NwnPalette__Count   = 10,
};

enum NwnLanguage
{
    NwnLanguage_English                 = 0, 
    NwnLanguage_French_Male             = 2, 
    NwnLanguage_French_Female   = 3, 
    NwnLanguage_German_Male             = 4, 
    NwnLanguage_German_Female   = 5, 
    NwnLanguage_Italian_Male    = 6, 
    NwnLanguage_Italian_Female  = 7, 
    NwnLanguage_Spanish_Male    = 8, 
    NwnLanguage_Spanish_Female  = 9, 
};

//-----------------------------------------------------------------------------
//
// Helper routines
//
//-----------------------------------------------------------------------------

const char *NwnGetResTypeExtension (NwnResType nType);
NwnResType NwnGetResTypeFromExtension (const char *pszExtension);
unsigned char *NwnLoadFile (const char *pszKeyFile, UINT32 *pulSize);
const char *NwnBasename (const char *pszFile);

//-----------------------------------------------------------------------------
//
// Byte order information
//
//-----------------------------------------------------------------------------

template <class Type>
class CNwnByteOrderSubCore
{
    public:
        static void LittleEndianSwap (const unsigned char *in, unsigned char *out)
        {
            *((Type *) out) = *((const Type *) in);
        }
        static void BigEndianSwap (const unsigned char *in, unsigned char *out)
        {
            *((Type *) out) = *((const Type *) in);
        }
};

template <class Type>
class CNwnByteOrderCore : public CNwnByteOrderSubCore <Type>
{
};

template <class Type>
class CNwnByteOrder : public CNwnByteOrderCore <Type>
{
    public:
        static Type LittleEndian (const Type &in)
        {
            Type out;
            LittleEndian (in, out);
            return out;
        }
        static Type LittleEndian (const unsigned char *in)
        {
            Type out;
            CNwnByteOrderCore<Type>::LittleEndianSwap (in, (unsigned char *) &out);
            return out;
        }
        static void LittleEndian (const Type &in, Type &out)
        {
            CNwnByteOrderCore<Type>::LittleEndianSwap ((const unsigned char *) &in, (unsigned char *) &out);
        }
        static void LittleEndian (const unsigned char *in, Type &out)
        {
            CNwnByteOrderCore<Type>::LittleEndianSwap (in, (unsigned char *) &out);
        }
        static void LittleEndian (const Type &in, unsigned char *out)
        {
            CNwnByteOrderCore<Type>::LittleEndianSwap ((const unsigned char *) &in, out);
        }
        static void LittleEndian (const unsigned char *in, unsigned char *out)
        {
            CNwnByteOrderCore<Type>::LittleEndianSwap (in, out);
        }
        static void LittleEndianIP (Type &inout)
        {
            CNwnByteOrderCore<Type>::LittleEndianSwap ((const unsigned char *) &inout, (unsigned char *) &inout);
        }
        static void LittleEndianIP (unsigned char *inout)
        {
            CNwnByteOrderCore<Type>::LittleEndianSwap (inout, inout);
        }

        static Type BigEndian (const Type &in)
        {
            Type out;
            BigEndian (in, out);
            return out;
        }
        static Type BigEndian (const unsigned char *in)
        {
            Type out;
            CNwnByteOrderCore<Type>::BigEndianSwap (in, (unsigned char *) &out);
            return out;
        }
        static void BigEndian (const Type &in, Type &out)
        {
            CNwnByteOrderCore<Type>::BigEndianSwap ((const unsigned char *) &in, (unsigned char *) &out);
        }
        static void BigEndian (const unsigned char *in, Type &out)
        {
            CNwnByteOrderCore<Type>::BigEndianSwap (in, (unsigned char *) &out);
        }
        static void BigEndian (const Type &in, unsigned char *out)
        {
            CNwnByteOrderCore<Type>::BigEndianSwap ((const unsigned char *) &in, out);
        }
        static void BigEndian (const unsigned char *in, unsigned char *out)
        {
            CNwnByteOrderCore<Type>::BigEndianSwap (in, out);
        }
        static void BigEndianIP (Type &inout)
        {
            CNwnByteOrderCore<Type>::BigEndianSwap ((const unsigned char *) &inout, (unsigned char *) &inout);
        }
        static void BigEndianIP (unsigned char *inout)
        {
            CNwnByteOrderCore<Type>::BigEndianSwap (inout, inout);
        }
};

#ifndef WORDS_BIGENDIAN
//
// LITTLE-Endian
//
template <>
class CNwnByteOrderCore <float> : public CNwnByteOrderSubCore <float>
{
    public:
        static void BigEndianSwap (const unsigned char *in, unsigned char *out)
        {
            unsigned char ca = in [0];
            unsigned char cb = in [3];
            out [3] = ca;
            out [0] = cb;
            ca = in [1];
            cb = in [2];
            out [2] = ca;
            out [1] = cb;
        }
};
template <>
class CNwnByteOrderCore <UINT32> : public CNwnByteOrderSubCore <UINT32>
{
    public:
        static void BigEndianSwap (const unsigned char *in, unsigned char *out)
        {
            unsigned char ca = in [0];
            unsigned char cb = in [3];
            out [3] = ca;
            out [0] = cb;
            ca = in [1];
            cb = in [2];
            out [2] = ca;
            out [1] = cb;
        }
};
template <>
class CNwnByteOrderCore <INT32> : public CNwnByteOrderSubCore <INT32>
{
    public:
        static void BigEndianSwap (const unsigned char *in, unsigned char *out)
        {
            unsigned char ca = in [0];
            unsigned char cb = in [3];
            out [3] = ca;
            out [0] = cb;
            ca = in [1];
            cb = in [2];
            out [2] = ca;
            out [1] = cb;
        }
};
template <>
class CNwnByteOrderCore <UINT16> : public CNwnByteOrderSubCore <UINT16>
{
    public:
        static void BigEndianSwap (const unsigned char *in, unsigned char *out)
        {
            unsigned char ca = in [0];
            unsigned char cb = in [1];
            out [1] = ca;
            out [0] = cb;
        }
};
template <>
class CNwnByteOrderCore <INT16> : public CNwnByteOrderSubCore <INT16>
{
    public:
        static void BigEndianSwap (const unsigned char *in, unsigned char *out)
        {
            unsigned char ca = in [0];
            unsigned char cb = in [1];
            out [1] = ca;
            out [0] = cb;
        }
};
#else
//
// BIG-Endian
//
template <>
class CNwnByteOrderCore <float> : public CNwnByteOrderSubCore <float>
{
    public:
        static void LittleEndianSwap (const unsigned char *in, unsigned char *out)
        {
            unsigned char ca = in [0];
            unsigned char cb = in [3];
            out [3] = ca;
            out [0] = cb;
            ca = in [1];
            cb = in [2];
            out [2] = ca;
            out [1] = cb;
        }
};
template <>
class CNwnByteOrderCore <UINT32> : public CNwnByteOrderSubCore <UINT32>
{
    public:
        static void LittleEndianSwap (const unsigned char *in, unsigned char *out)
        {
            unsigned char ca = in [0];
            unsigned char cb = in [3];
            out [3] = ca;
            out [0] = cb;
            ca = in [1];
            cb = in [2];
            out [2] = ca;
            out [1] = cb;
        }
};
template <>
class CNwnByteOrderCore <INT32> : public CNwnByteOrderSubCore <INT32>
{
    public:
        static void LittleEndianSwap (const unsigned char *in, unsigned char *out)
        {
            unsigned char ca = in [0];
            unsigned char cb = in [3];
            out [3] = ca;
            out [0] = cb;
            ca = in [1];
            cb = in [2];
            out [2] = ca;
            out [1] = cb;
        }
};
template <>
class CNwnByteOrderCore <UINT16> : public CNwnByteOrderSubCore <UINT16>
{
    public:
        static void LittleEndianSwap (const unsigned char *in, unsigned char *out)
        {
            unsigned char ca = in [0];
            unsigned char cb = in [1];
            out [1] = ca;
            out [0] = cb;
        }
};
template <>
class CNwnByteOrderCore <INT16> : public CNwnByteOrderSubCore <INT16>
{
    public:
        static void LittleEndianSwap (const unsigned char *in, unsigned char *out)
        {
            unsigned char ca = in [0];
            unsigned char cb = in [1];
            out [1] = ca;
            out [0] = cb;
        }
};
#endif

#endif // ETS_NWNDEFINES_H

/* vim: set sw=4: */
