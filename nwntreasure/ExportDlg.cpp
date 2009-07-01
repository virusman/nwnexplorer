//-----------------------------------------------------------------------------
// 
// @doc
//
// @module      ExportDlg.cpp - Script export dialog |
//
// This module contains the definition of the export dialog.
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
// $History: ExportDlg.cpp $
//      
//-----------------------------------------------------------------------------

#include "StdAfx.h"
#include "resource.h"
#include "ExportDlg.h"
#include "MainWnd.h"
#include "nwntreasure.h"
#include "TableObject.h"

//
// Debug NEW
//

#if defined (_DEBUG) && !defined (_WIN32_WCE)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// Global variables
//

extern TCHAR g_szAppName [];
extern BiowareTables g_asBiowareTables [];
extern int g_nBiowareTables;

//
// Script filters
//

LPCTSTR g_pszNSSFilter =
_T("NWScript (*.nss)\0*.nss\0")
_T("All Files (*.*)\0*.*\0")
_T("");

//-----------------------------------------------------------------------------
//
// @mfunc <c CExportDlg> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CExportDlg::CExportDlg () : 
    CFileDialogImpl<CExportDlg>(FALSE, _T (".nss"), _T (""),
                                OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLETEMPLATE,
                                g_pszNSSFilter)
{
    m_ofn.lpTemplateName = MAKEINTRESOURCE (IDD_EXPORT_SAVE_OPTIONS);
    m_nDebugStyle = 0;
    m_nExportStyle = 0;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CExportDlg> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CExportDlg::~CExportDlg()
{
}

//-----------------------------------------------------------------------------
//
// @mfunc Initialize the dialog
//
// @parm UINT | uMsg | Message
//
// @parm WPARAM | wParam | Message w-parameter
//
// @parm LPARAM | lParam | Message l-parameter
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CExportDlg::OnInitDialog (UINT uMsg, 
                                  WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{

    //
    // Get the controls
    //

    m_cbExportStyle = GetDlgItem (IDC_EXPORT_STYLE);
    m_statExportStyle = GetDlgItem (IDC_EXPORT_STYLE_STATIC);
    m_cbDebugStyle = GetDlgItem (IDC_DEBUG_STYLE);
    m_statDebugStyle = GetDlgItem (IDC_DEBUG_STYLE_STATIC);

    //
    // Resize myself so I take up all my parent area
    //

    CRect rectMe;
    GetWindowRect (&rectMe);
    CRect rectParent;
    ::GetClientRect (GetParent (), &rectParent);
    rectMe .right = rectMe .left + rectParent .Width ();

    //
    // Set my new size
    //

    SetWindowPos (NULL, 0, 0, rectMe .Width (), rectMe .Height (),
                  SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);

    //
    // Reposition the controls
    //

    RepositionControls ();

    //
    // Initialize the combo
    //

    CString str;
    str .LoadString (IDS_EXPORT_STYLE_BIOWARE);
    m_cbExportStyle .AddString (str);
    str .LoadString (IDS_EXPORT_STYLE_SW);
    m_cbExportStyle .AddString (str);
    m_cbExportStyle .SetCurSel (0);

    //
    // Initialize the debug combo
    //

    str .LoadString (IDS_DEBUG_STYLE_NONE);
    m_cbDebugStyle .AddString (str);
    str .LoadString (IDS_DEBUG_STYLE_PRINT);
    m_cbDebugStyle .AddString (str);
    str .LoadString (IDS_DEBUG_STYLE_SPEAK);
    m_cbDebugStyle .AddString (str);
    m_cbDebugStyle .SetCurSel (0);
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Reposition the controls
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CExportDlg::RepositionControls ()
{

    //
    // Reposition the controls
    //

    RepositionControl (m_cbExportStyle, cmb1, true);
    RepositionControl (m_statExportStyle, stc2, false);
    RepositionControl (m_cbDebugStyle, cmb1, true);
    RepositionControl (m_statDebugStyle, stc2, false);
}

//-----------------------------------------------------------------------------
//
// @mfunc Reposition a control
//
// @parm CWindow & | wnd | Control to be reposition
//
// @parm UINT | nID | ID of the control used for positioning
//
// @parm bool | fSize | If true, adjust the width of the control
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CExportDlg::RepositionControl (CWindow &wnd, UINT nID, bool fSize)
{

    //
    // Get the window rect in the client area of the 
    // control we are interested in.
    //

    CWindow wndParent = GetParent ();
    CWindow wndAnchor = wndParent .GetDlgItem (nID);
    CRect rectAnchor;
    wndAnchor .GetWindowRect (&rectAnchor);
    wndParent .ScreenToClient (&rectAnchor);

    //
    // Reposition the control
    //

    DWORD dwSWFlags = SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE;
    CRect rectCtrl;
    wnd .GetWindowRect (&rectCtrl);
    ScreenToClient (&rectCtrl);
    rectCtrl .OffsetRect (rectAnchor .left - rectCtrl .left, 0);
    if (fSize)
    {
        rectCtrl .right = rectCtrl .left + rectAnchor .Width ();
        dwSWFlags &= ~SWP_NOSIZE;
    }
    wnd .SetWindowPos (NULL, rectCtrl .left, rectCtrl .top,
                       rectCtrl .Width (), rectCtrl .Height (), dwSWFlags);
    return;
}

//-----------------------------------------------------------------------------
//
// @mfunc Do the export
//
// @parm CMainWnd | pMainWnd | Main window
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CExportDlg::DoExport (CMainWnd *pMainWnd)
{

    //
    // Show the dialog
    //

    CExportDlg dlg;
    if (dlg .DoModal () != IDOK)
        return;

    //
    // Collect the list of used tables
    //

    CAtlArray <CTableObject *> apTables;
    for (int i = 0; i < pMainWnd ->GetTableCount (); i++)
    {
        CTableObject *pTable = pMainWnd ->GetTable (i);
        if (pTable ->GetGlobal ())
            pTable ->CollectUsedTables (pMainWnd, apTables);
    }
    int nTables = (int) apTables .GetCount ();
    CTableObject **ppTables = apTables .GetData ();

    //
    // Loop through the tables and count the number of refs each
    // table has to another table
    //

    for (int i = 0; i < nTables; i++)
        ppTables [i] ->CountTableRefs (pMainWnd);

    //
    // Continue reducing the number of references
    //

    int nStart = 0;
    while (nStart < nTables)
    {
        int nNew = ProcessReferences (
            nTables - nStart, &ppTables [nStart]);
        if (nNew == 0)
        {
            CString str;
            str .LoadString (IDS_ERR_FOUND_RECUSION);
            for (int i = nStart; i < nTables; i++)
                str += ppTables [i] ->GetName () + _T ("\r\n");
            ::MessageBox (pMainWnd ->m_hWnd, str, g_szAppName, MB_OK);
            break;
        }
        nStart += nNew;
    }

    //
    // Generate the string resource array
    //

    CAtlArray <CString> astrStrings;
    for (int i = 0; i < nTables; i++)
        ppTables [i] ->CollectStrRes (astrStrings);


    //
    // Open the output file
    //

    FILE *fp = fopen (T2CA (dlg .m_szFileName), "wb");
    if (fp == NULL)
    {
        CString str;
        str .Format (IDS_ERR_OPEN_SCRIPT, dlg .m_szFileTitle);
        ::MessageBox (pMainWnd ->m_hWnd, str, g_szAppName, MB_OK);
        return;
    }

    //
    // Copy the template
    //

    if (!CopyResource (fp, _T ("TEXT_INCLUDE"), IDR_SCRIPT_TEMPLATE_START))
    {
        fclose (fp);
        return;
    }

    //
    // Add the SW final set
    //

    if (dlg .m_nExportStyle == 1)
    {
        if (!CopyResource (fp, _T ("TEXT_INCLUDE"), IDR_SCRIPT_SW_RESPAWN_START))
        {
            fclose (fp);
            return;
        }
    }

    //
    // Generate the debug routine
    //

    bool fDebug = false;
    if (dlg .m_nDebugStyle == 1)
    {
        fprintf (fp, "void dbSpeak (string s)\r\n{\r\n    PrintString (s);\r\n}\r\n");
        fDebug = true;
    }
    else if (dlg .m_nDebugStyle == 2)
    {
        fprintf (fp, "void dbSpeak (string s)\r\n{\r\n    SpeakString (s);\r\n}\r\n");
        fDebug = true;
    }
    else
        fprintf (fp, "void dbSpeak (string s)\r\n{\r\n}\r\n");


    //
    // Dump the strings
    //

    for (int i = 0; i < astrStrings .GetCount (); i++)
    {
        fprintf (fp, "string g_sS%d = \"%s\";\r\n",
                 i, (LPCTSTR) astrStrings [i]);
    }

    //
    // Write the treasure tables
    //

    ExportTables (fp, pMainWnd, ppTables, nTables, fDebug, Table_Type_Treasure);
    if (dlg .m_nExportStyle == 1)
    {
        ExportTables (fp, pMainWnd, ppTables, nTables, fDebug, Table_Type_Placeable);
        ExportTables (fp, pMainWnd, ppTables, nTables, fDebug, Table_Type_Encounter);
    }

    //
    // Generate the GenerateNPCTreasure routine
    //

    fprintf (fp, "void GenerateNPCTreasure (int nType = 1, object oT = OBJECT_SELF, object oA = OBJECT_SELF)\r\n");
    fprintf (fp, "{\r\n");
    fprintf (fp, "  switch (nType)\r\n");
    fprintf (fp, "  {\r\n");
    fprintf (fp, "    case 1: GenerateTreasure (TREASURE_NPC_LOW, oA, oT); break;\r\n");
    fprintf (fp, "    case 2: GenerateTreasure (TREASURE_NPC_MEDIUM, oA, oT); break;\r\n");
    fprintf (fp, "    case 3: GenerateTreasure (TREASURE_NPC_HIGH, oA, oT); break;\r\n");
    fprintf (fp, "    case 4: GenerateTreasure (TREASURE_NPC_BOSS, oA, oT); break;\r\n");
    fprintf (fp, "  }\r\n");
    fprintf (fp, "}\r\n\r\n");

    //
    // If SW export
    //

    if (dlg .m_nExportStyle == 1)
    {

        //
        // Generate the monster profile routine
        //

        bool fWroteAny = false;
        fprintf (fp, "int ProfileExecute (object oA, object oT, string sTreasureSet, \r\n");
        fprintf (fp, "    string sRespawnSet, string sRespawnSetAlt, int nGenTreasure, \r\n");
        fprintf (fp, "    int nGenDeathOpen, int nGenRespawn)\r\n");
        fprintf (fp, "{\r\n");
        fprintf (fp, "  int nRespawnStyle = RESPAWN_STYLE_RESPAWN;\r\n");
        fprintf (fp, "  int nRespawnTime = 0;\r\n");
        fprintf (fp, "  string sRespawnSetAlias = \"\";\r\n");
        fprintf (fp, "  string sTreasureSetAlias = \"\";\r\n");
        fprintf (fp, "  string sDeathOpenSpawn = \"\";\r\n");
        fprintf (fp, "  string sDeathOpenSound = \"\";\r\n");
        fprintf (fp, "  int nDeathOpenDamage = 0;\r\n\r\n");
        for (int i = 0; i < nTables; i++)
        {
            if (ppTables [i] ->GetTableType () != Table_Type_Profile)
                continue;
            fWroteAny = ppTables [i] ->WriteProfileTest (fp);
            break;
        }
        if (fWroteAny)
            fprintf (fp, "  else\r\n");
        fprintf (fp, "  {\r\n");
        fprintf (fp, "    int nT = TreasureTableNameToNumber (sTreasureSet);\r\n");
        fprintf (fp, "    if (nT < 0)\r\n");
        fprintf (fp, "      return FALSE;\r\n");
        fprintf (fp, "  }\r\n");
        fprintf (fp, "  \r\n");
        fprintf (fp, "  if (sRespawnSet == \"\")\r\n");
        fprintf (fp, "    sRespawnSet = sRespawnSetAlias;\r\n");
        fprintf (fp, "  if (sRespawnSet == \"\")\r\n");
        fprintf (fp, "    sRespawnSet = sRespawnSetAlt;\r\n");
        fprintf (fp, "  if (sRespawnSet == \"\")\r\n");
        fprintf (fp, "    sRespawnSet = GetTag (OBJECT_SELF);\r\n");
        fprintf (fp, "  \r\n");
        fprintf (fp, "  if (nGenTreasure)\r\n");
        fprintf (fp, "    GenerateProfileTreasure (sTreasureSet, sTreasureSetAlias, oA, oT);\r\n");
        fprintf (fp, "  if (nGenRespawn)\r\n");
        fprintf (fp, "    GenerateProfileRespawn (nRespawnStyle, nRespawnTime, sRespawnSet, sTreasureSet);\r\n");
        fprintf (fp, "  if (nGenDeathOpen)\r\n");
        fprintf (fp, "    GenerateProfileDeathOpen (oA, nDeathOpenDamage, sDeathOpenSpawn, sDeathOpenSound);\r\n");
        fprintf (fp, "  return TRUE;");
        fprintf (fp, "}\r\n\r\n");
    }

    //
    // Copy the bioware resource
    //

    if (!CopyResource (fp, _T ("TEXT_INCLUDE"), IDR_SCRIPT_BIOWARE_ROUTINES))
    {
        fclose (fp);
        return;
    }

    //
    // Add the SW final set
    //

    if (dlg .m_nExportStyle == 1)
        CopyResource (fp, _T ("TEXT_INCLUDE"), IDR_SCRIPT_SW_RESPAWN_ROUTINES);

    //
    // Close the file and return
    //

    fclose (fp);
}

//-----------------------------------------------------------------------------
//
// @mfunc Sort the table array
//
// @parm int | nTables | Number of tables in the array
//
// @parm CTableObject ** | ppTables | Pointer to an array of table pointers
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CExportDlg::SortTableArray (int nTables, CTableObject **ppTables)
{
    bool fSwapped = true;
    while (fSwapped)
    {
        fSwapped = false;
        for (int i = 0; i < nTables - 1; i++)
        {
            if (ppTables [i] ->m_nTableRefCount > 
                ppTables [i+1] ->m_nTableRefCount)
            {
                fSwapped = true;
                CTableObject *pT = ppTables [i];
                ppTables [i] = ppTables [i+1];
                ppTables [i+1] = pT;
            }
        }
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Process the referneces
//
// @parm int | nTables | Number of tables in the array
//
// @parm CTableObject ** | ppTables | Pointer to an array of table pointers
//
// @rdesc The number of references reduced to 0
//
//-----------------------------------------------------------------------------

int CExportDlg::ProcessReferences (int nTables, CTableObject **ppTables)
{

    //
    // Sort the list
    //

    SortTableArray (nTables, ppTables);

    //
    // Count the number of zeros at the start
    //

    int nZeros;
    for (nZeros = 0; nZeros < nTables; nZeros++)
        if (ppTables [nZeros] ->m_nTableRefCount != 0) 
            break;

    //
    // If we have no zero or they are all zeros, return
    //

    if (nZeros == 0)
        return 0;
    if (nZeros == nTables)
        return nTables;

    //
    // Loop through each of the zeros and decrement the non-zeros
    //

    for (int i = 0; i < nZeros; i++)
    {
        for (int j = nZeros; j < nTables; j++)
        {
            ppTables [j] ->RemoveTableRefCount (
                ppTables [i] ->GetName ());
        }
    }

    //
    // Return the number of zeros processed
    //

    return nZeros;
}

//-----------------------------------------------------------------------------
//
// @mfunc Copy the resource
//
// @parm FILE * | fp | Destination file
//
// @parm LPCTSTR | pszResType | Resource type
//
// @parm UINT | nID | Resource ID
//
// @rdesc Success status.
//
//-----------------------------------------------------------------------------

bool CExportDlg::CopyResource (FILE *fp, LPCTSTR pszResType, UINT nID)
{
    //
    // Find the template
    //

    HRSRC hRsrc = FindResource (_Module .GetResourceInstance (),
                                MAKEINTRESOURCE (nID), pszResType);
    if (hRsrc == NULL)
    {
        CString str;
        str .Format (IDS_ERR_COPY_RESOURCE);
        ::MessageBox (NULL, str, g_szAppName, MB_OK);
        return false;
    }

    //
    // Write the resource
    //

    DWORD dwSize = ::SizeofResource (_Module .GetResourceInstance (), hRsrc);
    HGLOBAL hSource = ::LoadResource (_Module .GetResourceInstance (), hRsrc);
    LPVOID pSource = ::LockResource (hSource);
    fwrite (pSource, dwSize, 1, fp);
    UnlockResource (hSource);
    ::FreeResource (hSource);
    return true;
}

//-----------------------------------------------------------------------------
//
// @mfunc Export a type of table
//
// @parm FILE * | fp | Destination file
//
// @parm CMainWnd * | pMainWnd | Main window
//
// @parm CTableObject ** | ppTables | List of tables
//
// @parm int | nTables | Number of tables
//
// @parm bool | fDebug | If true, write debug statement
//
// @parm Table_Type | nTableType | Type of table to be exported
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CExportDlg::ExportTables (FILE *fp, CMainWnd *pMainWnd, 
                               CTableObject **ppTables, int nTables, bool fDebug, 
                               Table_Type nTableType)
{
    LPCSTR pszArgList;
    LPCSTR pszGlobalPrefix;
    LPCSTR pszRoutineBody;
    LPCSTR pszConstant;
    LPCSTR pszMainCase;
    LPCSTR pszTableName;
    BiowareTables *pasBiowareTables;
    int nBiowareTables;

    //
    // Based on the type, get the constant strings
    //

    switch (nTableType)
    {
        case Table_Type_Treasure:
            pszArgList = "object oA, object oT";
            pszGlobalPrefix = "CT_";
            pszConstant = "TREASURE_U_";
            pszMainCase = "CT_%s (oA, oT)";
            pszTableName = "Treasure";
            pszRoutineBody =
                "void %s%s (%s)\r\n"
                "{\r\n"
                "  g_nM = 0; g_nS = 0;\r\n"
                "  if (!InitializeGenerateTreasure (oA, oT))\r\n"
                "    return;\r\n"
                "  CI_%s ();\r\n"
                "}\r\n\r\n";
            pasBiowareTables = g_asBiowareTables;
            nBiowareTables = g_nBiowareTables;
            break;

        case Table_Type_Placeable:
            pszArgList = "location loc, float fDelay = 0.0f, int nSingleShot = 0, string sProfile = \"\"";
            pszGlobalPrefix = "CP_";
            pszConstant = "PLACEABLE_U_";
            pszMainCase = "CP_%s (loc, fDelay, nSingleShot, sProfile)";
            pszTableName = "Placeable";
            pszRoutineBody =
                "void %s%s (%s)\r\n"
                "{\r\n"
                "  g_loc = loc;\r\n"
                "  g_sProfile = sProfile;\r\n"
                "  g_nSingleShot = nSingleShot;\r\n"
                "  g_fDelay = fDelay;\r\n"
                "  g_nSomethingSpawned = FALSE;\r\n"
                "  CI_%s ();\r\n"
                "}\r\n\r\n";
            pasBiowareTables = NULL;
            nBiowareTables = 0;
            break;

        case Table_Type_Encounter:
            pszArgList = "location loc, float fDelay = 0.0f, int nSingleShot = 0, string sProfile = \"\"";
            pszGlobalPrefix = "CE_";
            pszConstant = "ENCOUNTER_U_";
            pszMainCase = "CE_%s (loc, fDelay, nSingleShot, sProfile)";
            pszTableName = "Encounter";
            pszRoutineBody =
                "void %s%s (%s)\r\n"
                "{\r\n"
                "  g_loc = loc;\r\n"
                "  g_sProfile = sProfile;\r\n"
                "  g_nSingleShot = nSingleShot;\r\n"
                "  g_fDelay = fDelay;\r\n"
                "  g_nSomethingSpawned = FALSE;\r\n"
                "  CI_%s ();\r\n"
                "}\r\n\r\n";
            pasBiowareTables = NULL;
            nBiowareTables = 0;
            break;

        default:
            _ASSERTE (FALSE);
            return;
    }

    //
    // Generate the routine names
    //

    for (int i = 0; i < nTables; i++)
    {
        if (ppTables [i] ->GetTableType () == nTableType && 
            ppTables [i] ->GetGlobal ())
        {
            fprintf (fp, "void %s%s (%s)", pszGlobalPrefix, 
                     (LPCTSTR) ppTables [i] ->GetName (), pszArgList);
            fprintf (fp, ";\r\n");
        }
    }

    //
    // Space this stuff
    //

    fprintf (fp, "\r\n\r\n");

    //
    // Write the routines 
    //

    for (int i = 0; i < nTables; i++)
    {
        if (ppTables [i] ->GetTableType () == nTableType &&
            (ppTables [i] ->GetGlobal () ||
             ppTables [i] ->GetRefCount () > 1))
        {
            fprintf (fp, "void CI_%s ()", (LPCTSTR) ppTables [i] ->GetName ());
            fprintf (fp, "\r\n{\r\n");
            ppTables [i] ->WriteRoutineBody (pMainWnd, fp, fDebug);
            fprintf (fp, "}\r\n\r\n");
        }
    }

    //
    // Write the user callable routines
    //

    for (int i = 0; i < nTables; i++)
    {
        if (ppTables [i] ->GetTableType () == nTableType &&
            (ppTables [i] ->GetGlobal ()))
        {
            LPCTSTR pszName = ppTables [i] ->GetName ();
            fprintf (fp, pszRoutineBody, pszGlobalPrefix, pszName, pszArgList, pszName);
        }
    }

    //
    // Figure out which Bioware tables we are missing
    //

    for (int i = 0; i < nBiowareTables; i++)
    {
        pasBiowareTables [i] .fFound = false;
        for (int j = 0; j < nTables; j++)
        {
            if (ppTables [j] ->GetTableType () == nTableType &&
                ppTables [j] ->GetGlobal ())
            {
                if (strcmp (ppTables [j] ->GetName (), 
                            pasBiowareTables [i] .pszScript) == 0)
                {
                    pasBiowareTables [i] .fFound = true;
                    break;
                }
            }
        }
    }

    //
    // Generate the constant list
    //

    fprintf (fp, "\r\n\r\n");
    for (int i = 0; i < nBiowareTables; i++)
    {
        fprintf (fp, "int %s = %d;\r\n",
                 pasBiowareTables [i] .pszConstant,
                 pasBiowareTables [i] .nValue);
    }
    for (int i = 0; i < nTables; i++)
    {
        if (ppTables [i] ->GetGlobal () &&
            ppTables [i] ->GetNumber () > 0)
        {
            CString strName (CString (pszConstant) + ppTables [i] ->GetName ());
            strName .MakeUpper ();
            fprintf (fp, "int %s = %d;\r\n",
                     T2CA (strName), ppTables [i] ->GetNumber ());
        }
    }
    fprintf (fp, "\r\n\r\n");

    //
    // Generate the two required routines
    //

    fprintf (fp, "void Generate%s (int nType, %s)\r\n", pszTableName, pszArgList);
    fprintf (fp, "{\r\n");
    fprintf (fp, "  switch (nType)\r\n");
    fprintf (fp, "  {\r\n");
    for (int i = 0; i < nBiowareTables; i++)
    {
        if (pasBiowareTables [i] .fFound)
        {
            fprintf (fp, "    case %d: ", pasBiowareTables [i] .nValue);
            fprintf (fp, pszMainCase, pasBiowareTables [i] .pszScript);
            fprintf (fp, "; break;\r\n");
        }
    }
    for (int i = 0; i < nTables; i++)
    {
        if (ppTables [i] ->GetGlobal () &&
            ppTables [i] ->GetNumber () > 0 &&
            ppTables [i] ->GetTableType () == nTableType)
        {
            fprintf (fp, "    case %d: ", ppTables [i] ->GetNumber ());
            fprintf (fp, pszMainCase, T2CA (ppTables [i] ->GetName ()));
            fprintf (fp, "; break;\r\n");
        }
    }
    fprintf (fp, "  }\r\n");
    fprintf (fp, "}\r\n\r\n");

    //
    // Generate the routine that converts the treasure table name to a number
    //

    fprintf (fp, "int %sTableNameToNumber (string sName)\r\n", pszTableName);
    fprintf (fp, "{\r\n");
    fprintf (fp, "  string s = GetStringLowerCase (sName);\r\n");
    for (int i = 0; i < nBiowareTables; i++)
    {
        if (pasBiowareTables [i] .fFound)
        {
            fprintf (fp, "  if (s == \"%s\") return %d;\r\n", 
                     pasBiowareTables [i] .pszScript,
                     pasBiowareTables [i] .nValue);
        }
    }
    for (int i = 0; i < nTables; i++)
    {
        if (ppTables [i] ->GetGlobal () &&
            ppTables [i] ->GetNumber () > 0 &&
            ppTables [i] ->GetTableType () == nTableType)
        {
            fprintf (fp, "  if (s == \"%s\") return %d;\r\n", 
                     T2CA (ppTables [i] ->GetName ()),
                     ppTables [i] ->GetNumber ());
        }
    }
    fprintf (fp, "  return -1;\r\n");
    fprintf (fp, "}\r\n\r\n");
}

/* vim: set sw=4: */
