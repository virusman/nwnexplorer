#ifndef ETS_NSC_H
#define ETS_NSC_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      Nsc.h - Global parser definitions |
//
// This module contains general parser definitions
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
#include "../_NwnLib/NwnLoader.h"

//-----------------------------------------------------------------------------
//
// Symbol table general definitions
//
//-----------------------------------------------------------------------------

enum NscConstants
{
    NscMaxScript        = 0x80000,
    NscMaxHash          = 32,
    NscMaxLabelSize     = 16,           // internal setting only, don't sweat it
};

//-----------------------------------------------------------------------------
//
// Symbol types
//
//-----------------------------------------------------------------------------

enum NscSymType
{
    NscSymType_Token                    = 1,
    NscSymType_Function                 = 2,
    NscSymType_Structure                = 3,
    NscSymType_Variable                 = 4,
    NscSymType_Linker                   = 5,
};

//-----------------------------------------------------------------------------
//
// Symbol flags
//
//-----------------------------------------------------------------------------

enum NscSymFlags
{
    NscSymFlag_Global                   = 0x00000001,
    NscSymFlag_EngineFunc               = 0x00000002,
    NscSymFlag_PreIncrement             = 0x00000004,
    NscSymFlag_PostIncrement    = 0x00000008,
    NscSymFlag_PreDecrement             = 0x00000010,
    NscSymFlag_PostDecrement    = 0x00000020,
    NscSymFlag_Increments               = 0x0000003C,
    NscSymFlag_Constant                 = 0x00000080,
    NscSymFlag_LastDecl                 = 0x08000000,
    NscSymFlag_BeingDefined             = 0x10000000,
    NscSymFlag_TreatAsConstant  = 0x20000000,
    NscSymFlag_Referenced               = 0x40000000,
    NscSymFlag_Modified                 = 0x80000000,
};

//-----------------------------------------------------------------------------
//
// Value types
//
//-----------------------------------------------------------------------------

enum NscType
{
    NscType_Unknown                             = 0,
    NscType_Void                                = 1,
    NscType_Error                               = 2,
    NscType_Action                              = 3,
    NscType_Statement                   = 4,
    NscType_Struct                              = 5,
    NscType_Integer                             = 6,
    NscType_Float                               = 7,
    NscType_String                              = 8,
    NscType_Object                              = 9,
    NscType_Vector                              = 10,
    NscType_Engine_0                    = 16,
    NscType_Struct_0                    = 32,

    NscType__First_Compare              = 6,
};

//-----------------------------------------------------------------------------
//
// PCode types
//
//-----------------------------------------------------------------------------

enum NscPCode
{
    NscPCode_Variable                   = 0,
    NscPCode_Declaration                = 1,
    NscPCode_Argument                   = 2,
    NscPCode_Statement                  = 3,
    NscPCode_Call                               = 4,
    NscPCode_Element                    = 5,
    NscPCode_Break                              = 6,
    NscPCode_Continue                   = 7,
    NscPCode_Return                             = 8,
    NscPCode_Case                               = 9,
    NscPCode_Default                    = 10,
    NscPCode_LogicalAND                 = 11,
    NscPCode_LogicalOR                  = 12,
    NscPCode_Line                               = 13,
    NscPCode__Last_Special              = 13,

    // PRESEVE THIS BLOCKING ORDER!!!

    NscPCode__First_5Block              = NscPCode__Last_Special + 1,
    NscPCode_Switch                             = NscPCode__First_5Block + 0,
    NscPCode_If                                 = NscPCode__First_5Block + 1,
    NscPCode_Do                                 = NscPCode__First_5Block + 2,
    NscPCode_While                              = NscPCode__First_5Block + 3,
    NscPCode_For                                = NscPCode__First_5Block + 4,
    NscPCode_Conditional                = NscPCode__First_5Block + 5,
    NscPCode__Last_5Block               = NscPCode__First_5Block + 5,

    // PRESEVE THIS BLOCKING ORDER!!!

    NscPCode__First_Assignment  = NscPCode__Last_5Block + 1,
    NscPCode_AsnMultiply                = NscPCode__First_Assignment + 0,
    NscPCode_AsnDivide                  = NscPCode__First_Assignment + 1,
    NscPCode_AsnModulus                 = NscPCode__First_Assignment + 2,
    NscPCode_AsnAdd                             = NscPCode__First_Assignment + 3,
    NscPCode_AsnSubtract                = NscPCode__First_Assignment + 4,
    NscPCode_AsnShiftLeft               = NscPCode__First_Assignment + 5,
    NscPCode_AsnShiftRight              = NscPCode__First_Assignment + 6,
    NscPCode_AsnUnsignedShiftRight      = NscPCode__First_Assignment + 7,
    NscPCode_AsnBitwiseAND              = NscPCode__First_Assignment + 8,
    NscPCode_AsnBitwiseXOR              = NscPCode__First_Assignment + 9,
    NscPCode_AsnBitwiseOR               = NscPCode__First_Assignment + 10,
    NscPCode_Assignment                 = NscPCode__First_Assignment + 11,
    NscPCode__Last_Assignment   = NscPCode__First_Assignment + 11,

    // PRESEVE THIS BLOCKING ORDER!!!

    NscPCode__First_Simple              = NscPCode__Last_Assignment + 1,
    NscPCode_Negate                             = NscPCode__First_Simple + 0,
    NscPCode_BitwiseNot                 = NscPCode__First_Simple + 1,
    NscPCode_LogicalNot                 = NscPCode__First_Simple + 2,
    NscPCode_Multiply                   = NscPCode__First_Simple + 3,
    NscPCode_Divide                             = NscPCode__First_Simple + 4,
    NscPCode_Modulus                    = NscPCode__First_Simple + 5,
    NscPCode_Add                                = NscPCode__First_Simple + 6,
    NscPCode_Subtract                   = NscPCode__First_Simple + 7,
    NscPCode_ShiftLeft                  = NscPCode__First_Simple + 8,
    NscPCode_ShiftRight                 = NscPCode__First_Simple + 9,
    NscPCode_UnsignedShiftRight = NscPCode__First_Simple + 10,
    NscPCode_LessThan                   = NscPCode__First_Simple + 11,
    NscPCode_GreaterThan                = NscPCode__First_Simple + 12,
    NscPCode_LessThanEq                 = NscPCode__First_Simple + 13,
    NscPCode_GreaterThanEq              = NscPCode__First_Simple + 14,
    NscPCode_Equal                              = NscPCode__First_Simple + 15,
    NscPCode_NotEqual                   = NscPCode__First_Simple + 16,
    NscPCode_BitwiseAND                 = NscPCode__First_Simple + 17,
    NscPCode_BitwiseXOR                 = NscPCode__First_Simple + 18,
    NscPCode_BitwiseOR                  = NscPCode__First_Simple + 19,
    NscPCode_Constant                   = NscPCode__First_Simple + 20,
    NscPCode_ExpressionEnd              = NscPCode__First_Simple + 21,
    NscPCode__Last_Simple               = NscPCode__First_Simple + 21,
};

//-----------------------------------------------------------------------------
//
// Symbol table header
//
//-----------------------------------------------------------------------------

#ifdef _WIN32
#pragma warning (push)
#pragma warning (disable : 4201)
#endif

struct NscSymbol
{
    size_t                              nNext;
    size_t                              nLength;
    UINT32                              ulHash;
    NscSymType                  nSymType;
    union 
    {
        struct // Only valid for _Token sym type
        {
            int                 nToken;
            int                 nEngineObject;
        };
        struct // Only valid for _Linker sym type
        {
            size_t              nOffset;
            size_t              nFirstBackLink;
        };
        struct // Valid for all other sym types
        {
            NscType             nType;
            UINT32              ulFlags;
            size_t              nExtra;
            int                 nStackOffset;
            size_t              nCompiledStart;
            size_t              nCompiledEnd;
        };
    };
    char                                szString [1];
};

#ifdef _WIN32
#pragma warning (pop)
#endif

//-----------------------------------------------------------------------------
//
// Symbol table fence
//
//-----------------------------------------------------------------------------

enum NscFenceType
{
    NscFenceType_Function               = 0,
    NscFenceType_Scope                  = 1,
    NscFenceType_Switch                 = 2,
    NscFenceType_For                    = 3,
    NscFenceType_Do                             = 4,
    NscFenceType_While                  = 5,
    NscFenceType_If                             = 6,
};

enum NscFenceReturn
{
    NscFenceReturn_Unknown              = 0,
    NscFenceReturn_No                   = 1,
    NscFenceReturn_Yes                  = 2,
};

struct NscSymbolFence
{
    //Used by symbol table
    size_t                      nSize;
    size_t                      anHashStart [NscMaxHash];
#ifdef _DEBUG
    int                         anHashDepth [NscMaxHash];
#endif
    //Used by the context manager
    size_t                      nFnSymbol;
    NscFenceType        nFenceType;
    NscFenceReturn      nFenceReturn;
    bool                        fEatScope;
    bool                        fHasDefault;
    int                         nLocals;
    int                         nPrevLocals;
    NscSymbolFence      *pNext;
};

//-----------------------------------------------------------------------------
//
// Symbol extra data structures
//
//-----------------------------------------------------------------------------

struct NscSymbolFunctionExtra
{
    int                 nArgCount;
    int                 nArgSize;
    int                 nAction;
    size_t              nCodeOffset;
    size_t              nCodeSize;
    int                 nFile;
    int                 nLine;
};

struct NscSymbolStructExtra
{
    int                 nElementCount;
    int                 nTotalSize;
    // Structure data follows
};

struct NscSymbolVariableExtra
{
    size_t              nInitSize;
    int                 nFile;
    int                 nLine;
    // Init data follows
};

//-----------------------------------------------------------------------------
//
// PCode structures
//
//-----------------------------------------------------------------------------

struct NscPCodeHeader
{
    size_t                      nOpSize;
    NscPCode            nOpCode;
    NscType                     nType;
};

struct NscPCodeBinaryOp
{
    size_t                      nOpSize;
    NscPCode            nOpCode;
    NscType                     nType;
    NscType                     nLhsType;
    NscType                     nRhsType;
};

struct NscPCodeConstantInteger
{
    size_t                      nOpSize;
    NscPCode            nOpCode;
    NscType                     nType;
    INT32                       lValue;
};

struct NscPCodeConstantFloat
{
    size_t                      nOpSize;
    NscPCode            nOpCode;
    NscType                     nType;
    float                       fValue;
};

struct NscPCodeConstantString
{
    size_t                      nOpSize;
    NscPCode            nOpCode;
    NscType                     nType;
    size_t                      nLength;
    char                        szString [1];
};

struct NscPCodeConstantObject
{
    size_t                      nOpSize;
    NscPCode            nOpCode;
    NscType                     nType;
    UINT32                      ulid;
};

struct NscPCodeConstantVector
{
    size_t                      nOpSize;
    NscPCode            nOpCode;
    NscType                     nType;
    float                       v [3];
};

struct NscPCodeDeclaration
{
    size_t                      nOpSize;
    NscPCode            nOpCode;
    NscType                     nType;
    size_t                      nIdLength;
    size_t                      nDataSize;
    size_t                      nDataOffset;
    size_t                      nAltStringOffset;
    int                         nFile;
    int                         nLine;
    char                        szString [1];
};

struct NscPCodeCall
{
    size_t                      nOpSize;
    NscPCode            nOpCode;
    NscType                     nType;
    size_t                      nFnSymbol;
    size_t                      nArgCount;
    size_t                      nDataSize;
    size_t                      nDataOffset;
};

struct NscPCodeArgument
{
    size_t                      nOpSize;
    NscPCode            nOpCode;
    NscType                     nType;
    size_t                      nDataSize;
    size_t                      nDataOffset;
};

struct NscPCodeStatement
{
    size_t                      nOpSize;
    NscPCode            nOpCode;
    NscType                     nType;
    int                         nLocals;
    size_t                      nDataSize;
    size_t                      nDataOffset;
};

struct NscPCode5Block
{
    size_t                      nOpSize;
    NscPCode            nOpCode;
    NscType                     nType;
    size_t                      anSize [5];
    size_t                      anOffset [5];
    int                         anFile [5];
    int                         anLine [5];
};

struct NscPCodeVariable
{
    size_t                      nOpSize;
    NscPCode            nOpCode;
    NscType                     nType;
    NscType                     nSourceType;
    size_t                      nSymbol;
    int                         nElement;
    int                         nStackOffset;
    UINT32                      ulFlags;
};

struct NscPCodeAssignment
{
    size_t                      nOpSize;
    NscPCode            nOpCode;
    NscType                     nType;
    NscType                     nSourceType;
    NscType                     nRhsType;
    size_t                      nSymbol;
    int                         nElement;
    int                         nStackOffset;
    UINT32                      ulFlags;
    size_t                      nDataSize;
    size_t                      nDataOffset;
};

struct NscPCodeElement
{
    size_t                      nOpSize;
    NscPCode            nOpCode;
    NscType                     nType;
    NscType                     nLhsType;
    int                         nElement;
    size_t                      nDataSize;
    size_t                      nDataOffset;
};

struct NscPCodeReturn
{
    size_t                      nOpSize;
    NscPCode            nOpCode;
    NscType                     nType;
    size_t                      nDataSize;
    size_t                      nDataOffset;
};

struct NscPCodeCase
{
    size_t                      nOpSize;
    NscPCode            nOpCode;
    NscType                     nType;
    char                        szLabel [NscMaxLabelSize];
    size_t                      nCaseOffset;
    size_t                      nCaseSize;
    int                         nFile;
    int                         nLine;
};

struct NscPCodeLogicalOp
{
    size_t                      nOpSize;
    NscPCode            nOpCode;
    NscType                     nType;
    size_t                      nLhsOffset;
    size_t                      nLhsSize;
    size_t                      nRhsOffset;
    size_t                      nRhsSize;
};

struct NscPCodeLine
{
    size_t                      nOpSize;
    NscPCode            nOpCode;
    NscType                     nType;
    int                         nFile;
    int                         nLine;
};

//-----------------------------------------------------------------------------
//
// Bioware PCode
//
//-----------------------------------------------------------------------------

enum NscCode
{
    NscCode_CPDOWNSP    = 0x01,
    NscCode_RSADD               = 0x02,
    NscCode_CPTOPSP             = 0x03,
    NscCode_CONST               = 0x04,
    NscCode_ACTION              = 0x05,
    NscCode_LOGAND              = 0x06,
    NscCode_LOGOR               = 0x07,
    NscCode_INCOR               = 0x08,
    NscCode_EXCOR               = 0x09,
    NscCode_BOOLAND             = 0x0A,
    NscCode_EQUAL               = 0x0B,
    NscCode_NEQUAL              = 0x0C,
    NscCode_GEQ                 = 0x0D,
    NscCode_GT                  = 0x0E,
    NscCode_LT                  = 0x0F,
    NscCode_LEQ                 = 0x10,
    NscCode_SHLEFT              = 0x11,
    NscCode_SHRIGHT             = 0x12,
    NscCode_USHRIGHT    = 0x13,
    NscCode_ADD                 = 0x14,
    NscCode_SUB                 = 0x15,
    NscCode_MUL                 = 0x16,
    NscCode_DIV                 = 0x17,
    NscCode_MOD                 = 0x18,
    NscCode_NEG                 = 0x19,
    NscCode_COMP                = 0x1A,
    NscCode_MOVSP               = 0x1B,
    NscCode_STORE_STATEALL = 0x1C, //OLD
    NscCode_JMP                 = 0x1D,
    NscCode_JSR                 = 0x1E,
    NscCode_JZ                  = 0x1F,
    NscCode_RETN                = 0x20,
    NscCode_DESTRUCT    = 0x21,
    NscCode_NOT                 = 0x22,
    NscCode_DECISP              = 0x23,
    NscCode_INCISP              = 0x24,
    NscCode_JNZ                 = 0x25,
    NscCode_CPDOWNBP    = 0x26,
    NscCode_CPTOPBP             = 0x27,
    NscCode_DECIBP              = 0x28,
    NscCode_INCIBP              = 0x29,
    NscCode_SAVEBP              = 0x2A,
    NscCode_RESTOREBP   = 0x2B,
    NscCode_STORE_STATE = 0x2C,
    NscCode_NOP                 = 0x2D,
    NscCode_Size                = 0x42,
};

//-----------------------------------------------------------------------------
//
// Compiler/Decompiler routines
//
//-----------------------------------------------------------------------------

enum NscResult
{
    NscResult_Failure   = 0,
    NscResult_Success   = 1,
    NscResult_Include   = 2,
};

bool NscCompilerInitialize (CNwnLoader *pLoader, int nVersion, bool fEnableExtensions);
NscResult NscCompileScript (CNwnLoader *pLoader, const char *pszName, 
                            unsigned char *pauchData, UINT32 ulSize, bool fAllocated,
                            int nVersion, bool fEnableOptimizations, bool fIgnoreIncludes, 
                            CNwnStream *pCodeOutput, CNwnStream *pDebugOutput, 
                            CNwnStream *pErrorOuput = NULL);
NscResult NscCompileScript (CNwnLoader *pLoader, const char *pszName, 
                            int nVersion, bool fEnableOptimizations, bool fIgnoreIncludes, 
                            CNwnStream *pCodeOutput, CNwnStream *pDebugOutput,
                            CNwnStream *pErrorOuput = NULL);
void NscScriptDecompile (CNwnStream &sStream, 
                         unsigned char *pauchData, unsigned long ulSize);
const char *NscGetActionName (int nAction);

#endif // ETS_NSC_H

/* vim: set sw=4: */
