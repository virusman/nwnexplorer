/***************************************************************************
 *   Copyright (C) 2003 by John S. Brown                                   *
 *   malakh@dragoncat dot net                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <wx/wx.h>
#include <wx/image.h>
#include <wx/imaglist.h>
#include <wx/treectrl.h>
#include <wx/string.h>



#include "wxNwnTreasure.h"
#include "TablePropDlg.h"
#include "ItemPropDlg.h"
#include "EncItemPropDlg.h"
#include "PlcItemPropDlg.h"
#include "ProItemPropDlg.h"
#include "TblData.h"
#include "ItemTblData.h"
#include "EncItemTblData.h"




//#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)

#include "new.xpm"
#include "fileopen.xpm"
#include "save.xpm"
#include "none.xpm"
#include "table.xpm"
#include "gold.xpm"
#include "after.xpm"
#include "prop.xpm"
#include "tableprop.xpm"
#include "up.xpm"
#include "down.xpm"
#include "helpicon.xpm"
#include "exit.xpm"

#include "item.xpm"
#include "encounter.xpm"
#include "placeable.xpm"
#include "profile.xpm"
#include "global.xpm"
#include "noref.xpm"

#include "nwntreasure.xpm"






//#endif

    BEGIN_EVENT_TABLE(NwnTFrame, wxFrame)
    EVT_MENU(ID_New, NwnTFrame::OnNew)
    EVT_MENU(ID_Open, NwnTFrame::OnOpen)
    EVT_MENU(ID_Save, NwnTFrame::OnSave)
    EVT_MENU(ID_Create, NwnTFrame::OnCreate)
    EVT_MENU(ID_After, NwnTFrame::OnAfter)
    EVT_MENU(ID_Table, NwnTFrame::OnTable)
    EVT_MENU(ID_Prop, NwnTFrame::OnProp)
    EVT_MENU(ID_TProp, NwnTFrame::OnTProp)
    EVT_MENU(ID_Delete, NwnTFrame::OnDelete)
    EVT_MENU(ID_Up, NwnTFrame::OnUp)
    EVT_MENU(ID_Down, NwnTFrame::OnDown)
    EVT_MENU(ID_About, NwnTFrame::OnAbout)
    EVT_MENU(ID_Exit, NwnTFrame::OnExit)
    EVT_TREE_SEL_CHANGED(ID_TreeChg, NwnTFrame::OnTreeChg)
END_EVENT_TABLE()



    NwnTFrame::NwnTFrame(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):

        wxFrame(parent, id, title, pos, size, wxDEFAULT_FRAME_STYLE)
{
    // Give it an icon
    SetIcon(wxIcon(nwntreasure_xpm));

    _mb = new wxMenuBar();
    SetMenuBar(_mb);
    wxMenu* _mFile = new wxMenu();
    _mFile->Append(ID_New, wxT("New"), wxT(""));
    _mFile->Append(ID_Open, wxT("Open"), wxT(""));
    _mFile->Append(ID_Save, wxT("Save"), wxT(""));
    _mFile->Append(wxNewId(), wxT("Save As..."), wxT(""));
    _mFile->AppendSeparator();
    _mFile->Append(wxNewId(), wxT("Export Script..."), wxT(""));
    _mFile->AppendSeparator();
    wxMenu* _mFile_Recent = new wxMenu();
    _mFile_Recent->Append(ID_Empty, wxT("<Empty>"), wxT(""));
    _mFile_Recent->Enable(ID_Empty, 0);
    _mFile->Append(wxNewId(), wxT("Recent Files"), _mFile_Recent, wxT(""));
    _mFile->AppendSeparator();
    _mFile->Append(ID_Exit, wxT("Exit"), wxT(""));
    _mb->Append(_mFile, wxT("File"));

    wxMenu* _mEdit = new wxMenu();
    _mEdit->Append(ID_Create, wxT("Insert New In..."), wxT(""));
    _mEdit->Append(ID_After, wxT("Insert New After..."), wxT(""));
    _mEdit->Append(ID_Table, wxT("Insert Table..."), wxT(""));
    _mEdit->AppendSeparator();
    _mEdit->Append(ID_Prop, wxT("Proporties..."), wxT(""));
    _mEdit->Append(ID_TProp, wxT("Table Proporties..."), wxT(""));
    _mEdit->AppendSeparator();
    _mEdit->Append(ID_Up, wxT("Move Up"), wxT(""));
    _mEdit->Append(ID_Down, wxT("Move Down"), wxT(""));
    _mEdit->AppendSeparator();
    _mEdit->Append(ID_Delete, wxT("Delete"), wxT(""));
    _mb->Append(_mEdit, wxT("Edit"));
    wxMenu* _mHelp = new wxMenu();
    _mHelp->Append(ID_About, wxT("About"), wxT(""));
    _mb->Append(_mHelp, wxT("Help"));
    _sb = CreateStatusBar(1);



    _tb = CreateToolBar(wxTB_HORIZONTAL | wxTB_TEXT | wxTB_FLAT | wxTB_DOCKABLE, -1, wxT("Toolbar"));

    _tb->AddTool(ID_New, wxT("New"), wxBitmap(new_xpm), wxT("New..."));
    _tb->AddTool(ID_Open, wxT("Open"), wxBitmap(fileopen_xpm), wxT("Open..."));
    _tb->AddTool(ID_Save, wxT("Save"), wxBitmap(save_xpm), wxT("Save..."));
    _tb->AddSeparator();
    _tb->AddTool(ID_Create, wxT("Create in"), wxBitmap(gold_xpm), wxT("Create in..."));
    _tb->EnableTool(ID_Create, 0);
    _tb->AddTool(ID_After, wxT("After"), wxBitmap(after_xpm), wxT("After..."));
    _tb->EnableTool(ID_After, 0);
    _tb->AddTool(ID_Table, wxT("Table"), wxBitmap(table_xpm), wxT("Table..."));
    _tb->EnableTool(ID_Table, 0);
    _tb->AddSeparator();
    _tb->AddTool(ID_Prop, wxT("Prop."), wxBitmap(prop_xpm), wxT("Proporties..."));
    _tb->EnableTool(ID_Prop, 0);
    _tb->AddTool(ID_TProp, wxT("Table Prop."), wxBitmap(tableprop_xpm), wxT("Table Proporties..."));
    _tb->EnableTool(ID_TProp, 0);  
    _tb->AddSeparator();
    _tb->AddTool(ID_Delete, wxT("Delete"), wxBitmap(none_xpm), wxT("Delete..."));
    _tb->AddSeparator();
    _tb->AddTool(ID_Up, wxT("Up"), wxBitmap(up_xpm), wxT("Up..."));
    _tb->EnableTool(ID_Up, 0);
    _tb->AddTool(ID_Down, wxT("Down"), wxBitmap(down_xpm), wxT("Down..."));
    _tb->EnableTool(ID_Down, 0);
    _tb->AddSeparator();
    _tb->AddTool(ID_About, wxT("About"), wxBitmap(helpicon_xpm), wxT("About..."));
    _tb->AddTool(ID_Exit, wxT("Exit"), wxBitmap(exit_xpm), wxT("Exit..."));

    _tb->Realize();

    _tc1 = new wxTreeCtrl(this, ID_TreeChg, wxDefaultPosition, wxDefaultSize, 
                          wxTR_LINES_AT_ROOT|wxTR_HIDE_ROOT|wxTR_HAS_BUTTONS|wxSUNKEN_BORDER);

    wxImageList *images = new wxImageList(16, 16, TRUE);
    images->Add(wxIcon(item_xpm));       // image 0
    images->Add(wxIcon(encounter_xpm));  // image 1
    images->Add(wxIcon(placeable_xpm));  // image 2
    images->Add(wxIcon(profile_xpm));    // image 3
    images->Add(wxIcon(global_xpm));     // image 4
    images->Add(wxIcon(noref_xpm));      // image 5
    images->Add(wxIcon(none_xpm));       // image 6
    images->Add(wxIcon(gold_xpm));       // image 7
    images->Add(wxIcon(table_xpm));      // image 8



    _tc1->AddRoot(wxT("Tables"));

    _tc1_b1 = _tc1->AppendItem(_tc1->GetRootItem(), wxT("Treasure Table"), 0);
    _tc1_b2 = _tc1->AppendItem(_tc1->GetRootItem(), wxT("Encounter Table"), 1);
    _tc1_b3 = _tc1->AppendItem(_tc1->GetRootItem(), wxT("Placeable Table"), 2);
    _tc1_b4 =  _tc1->AppendItem(_tc1->GetRootItem(), wxT("Profile Table"), 3);
    _tc1_b4_sb1 = _tc1->AppendItem(_tc1_b4, wxT("Profile"), 4);


    _tc1->AssignImageList(images);


    set_properties();
    do_layout();

}


void NwnTFrame::set_properties()
{

    SetTitle(wxT("NWN Treasure Editor (OpenKnights Edition)"));
    SetSize(wxSize(1100, 600));
    int statusbar_widths[] = { -1 };
    _sb->SetStatusWidths(1, statusbar_widths);
    const wxString statusbar_fields[] = {
        wxT("Welcome to NWN Treasure Editor (OpenKnights Edition)")
    };

    for(int i = 0; i < _sb->GetFieldsCount(); ++i) {
        _sb->SetStatusText(statusbar_fields[i], i);
    }

}



void NwnTFrame::do_layout()
{

    wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
    sizer_1->Add(_tc1, 1, wxEXPAND, 0);
    SetAutoLayout(true);
    SetSizer(sizer_1);
    Layout();

    Centre();    

}

void NwnTFrame::OnNew(wxCommandEvent& WXUNUSED(event))
{

    wxString msg;
    msg.Printf( _T("Insert new file code here."));
    wxMessageBox(msg, _T("New placeholder"), wxOK | wxICON_INFORMATION, this);

}

void NwnTFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("Insert file open code here."));
    wxMessageBox(msg, _T("Open placeholder"), wxOK | wxICON_INFORMATION, this);
}

void NwnTFrame::OnSave(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("Insert file save code here."));
    wxMessageBox(msg, _T("Save placeholder"), wxOK | wxICON_INFORMATION, this);
}

void NwnTFrame::OnCreate(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId tree_sel = _tc1->GetSelection();

    if (_tc1->GetItemParent(tree_sel)==_tc1_b1) {

        ItemPropDlg
            IPropDlg ( this, -1,
                       this->GetTitle(),
                       wxPoint(300,200),
                       wxSize(250, 300),
                       wxRESIZE_BORDER |  wxDEFAULT_DIALOG_STYLE
                     );

        if (IPropDlg.ShowModal()==wxID_OK) {
            ItemTblData* pItemTblData = new ItemTblData( IPropDlg.GetAction(), 
                                                         IPropDlg.GetChance());
            if (IPropDlg.GetAction()==wxT("Drop Gold")) {
                pItemTblData->SetGold(IPropDlg.GetNumDice(), IPropDlg.GetDie(), IPropDlg.GetMultiplier());
            }
            else if (IPropDlg.GetAction()==wxT("Drop Item")) {
                pItemTblData->SetItem(IPropDlg.GetBlueprint(), IPropDlg.GetMin(), IPropDlg.GetMax());
            }
            else if (IPropDlg.GetAction()==wxT("Move To Table")) {
                pItemTblData->SetTable(IPropDlg.GetTableName(), IPropDlg.GetMin(), IPropDlg.GetMax(), IPropDlg.GetMod(), IPropDlg.GetSpecific());
            }
            _tc1->AppendItem(tree_sel, IPropDlg.GetTblName(), IPropDlg.GetIcon(), IPropDlg.GetIcon(), pItemTblData);
        }
    }

    if (_tc1->GetItemParent(tree_sel)==_tc1_b2) {       
        EncItemPropDlg
            EPropDlg ( this, -1,
                       this->GetTitle(),

                       wxPoint(300,200),
                       wxSize(250, 300),
                       wxRESIZE_BORDER |  wxDEFAULT_DIALOG_STYLE
                     );

        if (EPropDlg.ShowModal()==wxID_OK) {
            EncItemTblData* pEncItemTblData = new EncItemTblData( EPropDlg.GetAction(),
                                                                  EPropDlg.GetChance());
            if (EPropDlg.GetAction()==wxT("Drop Item")) {
                pEncItemTblData->SetItem(EPropDlg.GetBlueprint(), EPropDlg.GetMin(), EPropDlg.GetMax());
            }
            else if (EPropDlg.GetAction()==wxT("Move To Table")) {
                pEncItemTblData->SetTable(EPropDlg.GetTableName(), EPropDlg.GetMin(), EPropDlg.GetMax(), EPropDlg.GetMod(), EPropDlg.GetSpecific());
            }
            _tc1->AppendItem(tree_sel, EPropDlg.GetTblName(), EPropDlg.GetIcon(), EPropDlg.GetIcon(), pEncItemTblData);
        }
    }

    if (_tc1->GetItemParent(tree_sel)==_tc1_b3) { 
        PlcItemPropDlg
            PPropDlg ( this, -1,
                       this->GetTitle(),
                       wxPoint(300,200),
                       wxSize(250, 300),
                       wxRESIZE_BORDER |  wxDEFAULT_DIALOG_STYLE
                     );

        if (PPropDlg.ShowModal()==wxID_OK) {
            _tc1->AppendItem(tree_sel, PPropDlg.GetTblName(), PPropDlg.GetIcon());
        }
    }
    if (_tc1->GetItemParent(tree_sel)==_tc1_b4) { 
        ProItemPropDlg
            ProPropDlg ( this, -1,
                         this->GetTitle(),
                         wxPoint(300,200),
                         wxSize(250, 300),
                         wxRESIZE_BORDER |  wxDEFAULT_DIALOG_STYLE
                       );

        ProPropDlg.ShowModal();

    }

}

void NwnTFrame::OnAfter(wxCommandEvent& WXUNUSED(event))
{

}

void NwnTFrame::OnTable(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId branch = _tc1->GetSelection();
    if ((branch == _tc1_b1 || branch == _tc1_b2 || branch == _tc1_b3) && (branch != _tc1_b4)) {
        TblPropDialog
            TPropDlg ( this, -1,
                       this->GetTitle(),
                       wxPoint(100,100),
                       wxSize(250, 300),
                       wxRESIZE_BORDER |  wxDEFAULT_DIALOG_STYLE
                     );


        if (TPropDlg.ShowModal()==wxID_OK) {
            TblData* pTblData = new TblData( TPropDlg.GetName(), TPropDlg.GetMode(), 
                                             TPropDlg.GetGlobal(), TPropDlg.GetTblNum());
            int icon;                
            if (TPropDlg.GetGlobal()==true) { icon = 4; }
            else {icon = 5; }
            _tc1->AppendItem(branch, TPropDlg.GetTblName(), icon, icon, pTblData); 


        }
    }
}

void NwnTFrame::OnProp(wxCommandEvent& WXUNUSED(event))
{

    wxTreeItemId branch = _tc1->GetSelection();
    wxTreeItemId parent = _tc1->GetItemParent(branch);

    if (_tc1->GetItemParent(parent)==_tc1_b1) {
        ShowItemProp();
    }
    if (_tc1->GetItemParent(parent)==_tc1_b2) {
        ShowEncProp();    
    }
}


void NwnTFrame::ShowItemProp()
{
    wxTreeItemId branch = _tc1->GetSelection();
    ItemPropDlg
        IPropDlg ( this, -1,
                   this->GetTitle(),
                   wxPoint(300,200),
                   wxSize(250, 300),
                   wxRESIZE_BORDER |  wxDEFAULT_DIALOG_STYLE
                 );
    ItemTblData *pItemTblData = static_cast<ItemTblData*>(_tc1->GetItemData(branch));
    wxString action; int chance;
    action = pItemTblData->GetAction(); IPropDlg.SetAction(action);
    chance = pItemTblData->GetChance(); IPropDlg.SetChance(chance);
    if (action == wxT("Drop Gold")) {
        int numdice; int die; wxString multiplier;
        numdice = pItemTblData->GetNumDice();
        die = pItemTblData->GetDie();
        multiplier = pItemTblData->GetMultiplier();
        IPropDlg.SetGold(numdice, die, multiplier);
        IPropDlg.ShowGold();
    }
    if (action == wxT("Drop Item")) {
        wxString blueprint; int min; int max;
        blueprint = pItemTblData->GetBlueprint();
        min = pItemTblData->GetMin();
        max = pItemTblData->GetMax();
        IPropDlg.SetItem(blueprint, min, max);
        IPropDlg.ShowItem();
    }
    if (action == wxT("Move To Table")) {
        wxString tablename; int min; int max; wxString mod; wxString specific;
        tablename = pItemTblData->GetTableName(); min = pItemTblData->GetMin();
        max = pItemTblData->GetMax(); mod = pItemTblData->GetMod();
        specific = pItemTblData->GetSpecific();
        IPropDlg.SetTable(tablename, min, max, mod, specific);
        IPropDlg.ShowTable();
    }
    if (IPropDlg.ShowModal()==wxID_OK) {

        action = IPropDlg.GetAction(); chance = IPropDlg.GetChance();
        pItemTblData->SetData(action, chance);
        if (action == wxT("Drop Gold")) {
            int numdice; int die; wxString multiplier;
            numdice = IPropDlg.GetNumDice(); die = IPropDlg.GetDie();
            multiplier = IPropDlg.GetMultiplier();
            pItemTblData->SetGold(numdice, die, multiplier);
        }
        if (action == wxT("Drop Item")) {
            wxString blueprint; int min; int max;
            blueprint = IPropDlg.GetBlueprint(); min = IPropDlg.GetMin();
            max = IPropDlg.GetMax();
            pItemTblData->SetItem(blueprint, min, max);
        }
        if (action == wxT("Move To Table")) {
            wxString tablename; int min; int max; wxString mod; wxString specific;
            tablename = IPropDlg.GetTableName(); min = IPropDlg.GetMin();
            max = IPropDlg.GetMax(); mod = IPropDlg.GetMod();
            specific = IPropDlg.GetSpecific();
            pItemTblData->SetTable(tablename, min, max, mod, specific);

        }
        _tc1->SetItemText(branch, IPropDlg.GetTblName());
    }
}

void NwnTFrame::ShowEncProp()
{
    wxTreeItemId branch = _tc1->GetSelection();
    EncItemPropDlg
        EPropDlg ( this, -1,
                   this->GetTitle(),
                   wxPoint(300,200),
                   wxSize(250, 300),
                   wxRESIZE_BORDER |  wxDEFAULT_DIALOG_STYLE
                 );
    EncItemTblData *pEncItemTblData = static_cast<EncItemTblData*>(_tc1->GetItemData(branch));
    if(!pEncItemTblData) {
        printf("pEncitemTblData == null\n");
    }
    wxString action; int chance;
    /* This next line appears to be where the Segfualt happens. */
    action = pEncItemTblData->GetAction();
    EPropDlg.SetAction(action);
    chance = pEncItemTblData->GetChance();
    EPropDlg.SetChance(chance);
    if (action == wxT("Spawn Creature/NPC")) {
        wxString blueprint; int min; int max;
        blueprint = pEncItemTblData->GetBlueprint();
        min = pEncItemTblData->GetMin();
        max = pEncItemTblData->GetMax();
        EPropDlg.SetItem(blueprint, min, max);
        EPropDlg.ShowItem();
    }
    if (action == wxT("Move To Table")) {
        wxString tablename; int min; int max; wxString mod; wxString specific;
        tablename = pEncItemTblData->GetTableName(); min = pEncItemTblData->GetMin();
        max = pEncItemTblData->GetMax(); mod = pEncItemTblData->GetMod();
        specific = pEncItemTblData->GetSpecific();
        EPropDlg.SetTable(tablename, min, max, mod, specific);
        EPropDlg.ShowTable();
    }

    if (EPropDlg.ShowModal()==wxID_OK) {
        action = EPropDlg.GetAction(); chance = EPropDlg.GetChance();
        pEncItemTblData->SetData(action, chance);
        if (action == wxT("Drop Item")) {
            wxString blueprint; int min; int max;
            blueprint = EPropDlg.GetBlueprint(); min = EPropDlg.GetMin();
            max = EPropDlg.GetMax();
            pEncItemTblData->SetItem(blueprint, min, max);
        }
        if (action == wxT("Move To Table")) {
            wxString tablename; int min; int max; wxString mod; wxString specific;
            tablename = EPropDlg.GetTableName(); min = EPropDlg.GetMin();
            max = EPropDlg.GetMax(); mod = EPropDlg.GetMod();
            specific = EPropDlg.GetSpecific();
            pEncItemTblData->SetTable(tablename, min, max, mod, specific);

        }
        _tc1->SetItemText(branch, EPropDlg.GetTblName());

    }
}

void NwnTFrame::OnTProp(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId branch = _tc1->GetSelection();

    TblPropDialog
        TPropDlg ( this, -1,
                   this->GetTitle(),
                   wxPoint(100,100),
                   wxSize(250, 300),
                   wxRESIZE_BORDER |  wxDEFAULT_DIALOG_STYLE
                 );
    TblData *pTblData = static_cast<TblData*>(_tc1->GetItemData(branch));
    wxString name; wxString mode; bool global; int tblnum;
    name = pTblData->GetName(); TPropDlg.SetName(name);
    mode = pTblData->GetMode(); TPropDlg.SetMode(mode);
    global = pTblData->GetGlobal(); TPropDlg.SetGlobal(global);
    tblnum = pTblData->GetTblNum(); TPropDlg.SetTblNum(tblnum);

    if (TPropDlg.ShowModal()==wxID_OK) {
        name = TPropDlg.GetName();
        mode = TPropDlg.GetMode();
        global = TPropDlg.GetGlobal();
        tblnum = TPropDlg.GetTblNum();
        pTblData->SetName(name);
        pTblData->SetMode(mode);
        pTblData->SetGlobal(global);
        pTblData->SetTblNum(tblnum);
        _tc1->SetItemText(branch, TPropDlg.GetTblName());
        int icon; 
        if (TPropDlg.GetGlobal()==true) { icon = 4; }
        else {icon = 5; }
        _tc1->SetItemImage(branch, icon, wxTreeItemIcon_Normal );
        _tc1->SetItemImage(branch, icon, wxTreeItemIcon_Selected );
    }



}

void NwnTFrame::OnDelete(wxCommandEvent& WXUNUSED(event))
{
    wxTreeItemId item;
    item = _tc1->GetSelection();
    if ((item != _tc1_b1) && (item != _tc1_b2) && (item != _tc1_b3) 
        && (item != _tc1_b4) && (item != _tc1_b4_sb1)) {
        _tc1->Delete(item);
    }

}

void NwnTFrame::OnUp(wxCommandEvent& WXUNUSED(event))
{

    wxString cur_text;
    wxString pre_text;

    int cur_icon;
    int pre_icon;  

    wxTreeItemId current;
    wxTreeItemId previous;

    current = _tc1->GetSelection();
    previous = _tc1->GetPrevSibling(current);

    cur_text = _tc1->GetItemText(current);
    pre_text = _tc1->GetItemText(previous);

    cur_icon = _tc1->GetItemImage(current);
    pre_icon = _tc1->GetItemImage(previous);

    _tc1->SetItemText(current, pre_text);
    _tc1->SetItemText(previous, cur_text); 

    _tc1->SetItemImage(current, pre_icon);
    _tc1->SetItemImage(previous, cur_icon);

}

void NwnTFrame::OnDown(wxCommandEvent& WXUNUSED(event))
{

    wxString cur_text;
    wxString next_text;

    int cur_icon;
    int next_icon;

    wxTreeItemId current;
    wxTreeItemId next;

    current = _tc1->GetSelection();
    next = _tc1->GetNextSibling(current);

    cur_text = _tc1->GetItemText(current);
    next_text = _tc1->GetItemText(next);

    cur_icon = _tc1->GetItemImage(current);
    next_icon = _tc1->GetItemImage(next);

    _tc1->SetItemText(current, next_text);
    _tc1->SetItemText(next, cur_text);

    _tc1->SetItemImage(current, next_icon);
    _tc1->SetItemImage(next, cur_icon);

}

void NwnTFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("Nwn Treasure Editor (OpenKnights Edition)\n")
                _T("Using %s"), wxVERSION_STRING);

    wxMessageBox(msg, _T("About wxNwnTreasure"), wxOK | wxICON_INFORMATION, this);
}

void NwnTFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close

    Close(TRUE);
}

void NwnTFrame::OnTreeChg(wxCommandEvent& WXUNUSED(event))
{

    wxTreeItemId branch = _tc1->GetSelection();

    if ( (branch == _tc1_b1) || (branch == _tc1_b2) 
         || (branch == _tc1_b3)) {
        _tb->EnableTool(ID_Table, 1);

    }
    else if ( (branch != _tc1_b1) && (branch != _tc1_b2) 
              && (branch != _tc1_b3)) {
        _tb->EnableTool(ID_Table, 0);

    }  

    if ( (_tc1->GetItemParent(branch) == _tc1_b1) 
         || (_tc1->GetItemParent(branch) == _tc1_b2)
         || (_tc1->GetItemParent(branch) == _tc1_b3) 
         || (_tc1->GetItemParent(branch) == _tc1_b4)) {
        _tb->EnableTool(ID_Create, 1);
        _tb->EnableTool(ID_TProp, 1);    
    }
    else if ( (_tc1->GetItemParent(branch) != _tc1_b1) 
              && (_tc1->GetItemParent(branch) != _tc1_b2)
              && (_tc1->GetItemParent(branch) != _tc1_b3) 
              && (_tc1->GetItemParent(branch) != _tc1_b4)) {
        _tb->EnableTool(ID_Create, 0);
        _tb->EnableTool(ID_TProp, 0);    
    }

    if ((_tc1->GetItemParent(_tc1->GetItemParent(branch)) == _tc1_b1)
        || (_tc1->GetItemParent(_tc1->GetItemParent(branch)) == _tc1_b2)
        || (_tc1->GetItemParent(_tc1->GetItemParent(branch)) == _tc1_b3)
        || (_tc1->GetItemParent(_tc1->GetItemParent(branch)) == _tc1_b4)) {
        _tb->EnableTool(ID_Prop, 1);
        _tb->EnableTool(ID_Up, 1);
        _tb->EnableTool(ID_Down, 1);
    }
    else if ((_tc1->GetItemParent(_tc1->GetItemParent(branch)) != _tc1_b1)
             && (_tc1->GetItemParent(_tc1->GetItemParent(branch)) != _tc1_b2)
             && (_tc1->GetItemParent(_tc1->GetItemParent(branch)) != _tc1_b3)
             && (_tc1->GetItemParent(_tc1->GetItemParent(branch)) != _tc1_b4)) {
        _tb->EnableTool(ID_Prop, 0);
        _tb->EnableTool(ID_Up, 0);
        _tb->EnableTool(ID_Down, 0);
    }
}



class wxNwnTreasure: public wxApp {
    public:
        bool OnInit();
};




IMPLEMENT_APP(wxNwnTreasure)

bool wxNwnTreasure::OnInit()
{

    NwnTFrame* frame_1 = new NwnTFrame(0, -1, wxT(""));
    SetTopWindow(frame_1);
    frame_1->Show();
    return true;
}



/* vim: set sw=4: */
