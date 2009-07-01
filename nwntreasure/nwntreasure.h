#ifndef ETS_NWNTREASURE_H
#define ETS_NWNTREASURE_H

//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      nwntreasure.h - Global definitions |
//
// This module contains the global definitions
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

#include "resource.h"

//-----------------------------------------------------------------------------
//
// Forward definitions
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Enumerations
//
//-----------------------------------------------------------------------------

enum TB_TextStyle
{
    TBTS_NoText                                 = 0,
    TBTS_BottomText                             = 1,
    TBTS_RightText                              = 2,
};

enum TB_IconStyle
{
    TBIS_SmallIcons                             = 0,
    TBIS_LargeIcons                             = 1,
};

enum Object_Type
{
    Object_Type_Unknown                 = 0,
    Object_Type_Gold                    = 1,
    Object_Type_Item                    = 2,
    Object_Type_None                    = 3,
    Object_Type_Table                   = 4,
    Object_Type_Table_Ref               = 5,
    Object_Type_Root                    = 6,
    Object_Type_Encounter               = 7,
    Object_Type_Placeable               = 8,
    Object_Type_Profile                 = 9,
};

enum Object_Image
{
    Object_Image_Table                  = 0,
    Object_Image_Item                   = 1,
    Object_Image_Encounter              = 2,
    Object_Image_Gold                   = 3,
    Object_Image_None                   = 4,
    Object_Image_Table_NoRef    = 5,
    Object_Image_Table_Global   = 6,
    Object_Image_Placeable              = 7,
    Object_Image_Profile                = 8,
};

enum Table_Type
{
    Table_Type_Treasure                 = 0,
    Table_Type_Encounter                = 1,
    Table_Type_Placeable                = 2,
    Table_Type_Profile                  = 3,
    Table_Type_Any                              = 9999,
};

enum Table_Command
{
    Table_Command_None                          = 0,
    Table_Command_Pick_One                      = 1,
    Table_Command_Pick_Multiple         = 2,
    Table_Command_Racial_Type           = 3,
    Table_Command_Level_Range           = 4,
    Table_Command_Random_Class_Type     = 5,
    Table_Command_Specific                      = 6,
    Table_Command_Weapon_Focus          = 7,
    Table_Command_Armor_Proficiency     = 8,
    Table_Command_Class_Type            = 9,
    Table_Command_Error                         = -1,
};

enum Racial_Type
{
    Racial_Type_Any                                     = 0,
    Racial_Type_Dwarf               = 1,
    Racial_Type_Elf                 = 2,
    Racial_Type_Gnome               = 3,
    Racial_Type_Halfling            = 4,
    Racial_Type_HalfElf             = 5,
    Racial_Type_HalfOrc             = 6,
    Racial_Type_Human               = 7,
    Racial_Type_Aberration          = 8,
    Racial_Type_Animal              = 9,
    Racial_Type_Beast               = 10,
    Racial_Type_Construct           = 11,
    Racial_Type_Dragon              = 12,
    Racial_Type_Humanoid_Goblinoid  = 13,
    Racial_Type_Humanoid_Monstrous  = 14,
    Racial_Type_Humanoid_Orc        = 15,
    Racial_Type_Humanoid_Reptilian  = 16,
    Racial_Type_Elemental           = 17,
    Racial_Type_Fey                 = 18,
    Racial_Type_Giant               = 19,
    Racial_Type_Magical_Beast       = 20,
    Racial_Type_Outsider            = 21,
    Racial_Type_Shapechanger        = 22,
    Racial_Type_Undead              = 23,
    Racial_Type_Vermin              = 24,
    Racial_Type__Last                           = 24,
    Racial_Type_Error                           = -1,
};

enum Class_Type
{
    Class_Type_Any                                      = 0,
    Class_Type_Barbarian                        = 1,
    Class_Type_Bard                                     = 2,
    Class_Type_Cleric                           = 3,
    Class_Type_Druid                            = 4,
    Class_Type_Fighter                          = 5,
    Class_Type_Monk                                     = 6,
    Class_Type_Paladin                          = 7,
    Class_Type_Ranger                           = 8,
    Class_Type_Rogue                            = 9,
    Class_Type_Sorcerer                         = 10,
    Class_Type_Wizard                           = 11,
    Class_Type_Aberration                       = 12,
    Class_Type_Animal                           = 13,
    Class_Type_Construct                        = 14,
    Class_Type_Humanoid                         = 15,
    Class_Type_Monstrous                        = 16,
    Class_Type_Elemental                        = 17,
    Class_Type_Fey                                      = 18,
    Class_Type_Dragon                           = 19,
    Class_Type_Undead                           = 20,
    Class_Type_Commoner                         = 21,
    Class_Type_Beast                            = 22,
    Class_Type_Giant                            = 23,
    Class_Type_Magical_Beast            = 24,
    Class_Type_Outsider                         = 25,
    Class_Type_Shapechanger                     = 26,
    Class_Type_Vermin                           = 27,
    Class_Type__Last                            = 27,
    Class_Type_Error                            = -1,
};

enum Weapon_Focus_Type
{
    Weapon_Focus_Any                            = 0,
    Weapon_Focus__Last                          = 38,
    Weapon_Focus_Error                          = -1,
};

enum Armor_Proficiency_Type
{
    Armor_Proficiency_Any                       = 0,
    Armor_Proficiency__Last                     = 3,
    Armor_Proficiency_Error                     = -1,
};

//-----------------------------------------------------------------------------
//
// Structures
//
//-----------------------------------------------------------------------------

struct TB_Command
{
    UINT                        nCmd;
    UINT                        fsStyle;
    UINT                        iBitmap;
};

struct BiowareTables 
{
    LPCSTR                      pszConstant;
    LPCSTR                      pszScript;
    int                         nValue;
    bool                        fFound;
};

//-----------------------------------------------------------------------------
//
// Configuration definitions
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Profile strings
//
//-----------------------------------------------------------------------------

#define PROFILE_SETTINGS                        _T ("Settings")
#define PROFILE_WINPLACEMENT            _T ("WindowPlacement2")
#define PROFILE_TBTEXTSTYLE                     _T ("TBTextStyle")
#define PROFILE_TBICONSTYLE                     _T ("TBIconStyle")
#define PROFILE_TBBUTTONS                       _T ("TBButtons")

#define PROFILE_TBTEXTSTYLE_DEFAULT     TBTS_BottomText
#define PROFILE_TBICONSTYLE_DEFAULT     TBIS_LargeIcons

//-----------------------------------------------------------------------------
//
// Bitmap routines
//
//-----------------------------------------------------------------------------

SIZE GetBitmapSize (HBITMAP hBitmap);

//-----------------------------------------------------------------------------
//
// Other general routines
//
//-----------------------------------------------------------------------------

bool ExtractString (LPTSTR pszReturn, int nMaxChar, 
                    LPCTSTR pszSource, int nSubString, TCHAR cSeperator);
void CenterPropertySheet (CWindow &sSheet);
HMENU LoadPopupMenu (HINSTANCE hInstance, UINT id, UINT uSubMenu);
bool IsValidBlueprintName (LPCTSTR pszName);
bool IsValidTableName (LPCTSTR pszName);
bool IsValidProfileName (LPCTSTR pszName);
BOOL GetDllVersion (LPCTSTR pszDll, DWORD &dwMajor, DWORD &dwMinor);
int DecodeToolbarButtons (LPCTSTR pszText, TBBUTTON *pTBBtns, int nMaxButtons);

DWORD GetSetting (LPCTSTR pszName, DWORD dwDefault);
bool SetSetting (LPCTSTR pszName, DWORD dwValue);
bool GetSetting (LPCTSTR pszName, LPCTSTR pszDefault, 
                 LPTSTR pszValue, int cchMaxText);
bool SetSetting (LPCTSTR pszName, LPCTSTR pszValue);

#endif // ETS_UOTH_H

/* vim: set sw=4: */
