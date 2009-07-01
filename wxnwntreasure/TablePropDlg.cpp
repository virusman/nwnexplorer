/***************************************************************************
 *   Copyright (C) 2003 by John S. Brown                                   *
 *   malakh@dragoncat dot net                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#include "TablePropDlg.h"


    BEGIN_EVENT_TABLE(TblPropDialog, wxDialog)
    EVT_BUTTON( wxID_OK, TblPropDialog::OnOk )
    EVT_CHECKBOX( ID_GlblCheckbox, TblPropDialog::OnGlblCheck )
END_EVENT_TABLE()

    TblPropDialog::TblPropDialog(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
        wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE)
{


    label_TblName = new wxStaticText(this, -1, wxT("Table Name"));
    text_TblName = new wxTextCtrl(this, -1, wxT(""));
    label_DMode = new wxStaticText(this, -1, wxT("Decision Mode"));

    const wxString combo_box_1_choices[] = {
        wxT("Pick One Of"),
        wxT("Pick Multiple From"),
        wxT("Pick By Racial Type"),
        wxT("Pick By Level Range"),
        wxT("Pick By Random Class"),
        wxT("Pick By Specific"),
        wxT("Pick By Weapon Focus"),
        wxT("Pick By Armor Proficiency"),
        wxT("Pick By Class")
    };
    combo_box_1 = new wxComboBox(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 9, combo_box_1_choices, wxCB_DROPDOWN);
    checkbox_1 = new wxCheckBox(this, ID_GlblCheckbox, wxT("Global Table - Global tables will be made "));
    label_1 = new wxStaticText(this, -1, wxT("available as starting points."));
    label_TblNum = new wxStaticText(this, -1, wxT("Table Number"));
    spin_ctrl_1 = new wxSpinCtrl(this, -1, wxT("100"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100);
    button_1 = new wxButton(this, wxID_OK, wxT("OK"));
    button_2 = new wxButton(this, wxID_CANCEL, wxT("Cancel"));

    set_properties();
    do_layout();

}



void TblPropDialog::set_properties()
{


    SetTitle(wxT("Table Proporties"));

    SetSize(wxSize(300, 263));
    combo_box_1->SetSelection(0);
    label_TblNum->Disable();
    spin_ctrl_1->Disable();
    button_1->SetDefault();

    if (checkbox_1->GetValue()==true) {
        label_TblNum->Enable(true);
        spin_ctrl_1->Enable(true);
    }
    else {
        label_TblNum->Disable();
        spin_ctrl_1->Disable();
    }

}


void TblPropDialog::do_layout()
{

    wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
    wxGridSizer* grid_sizer_2 = new wxGridSizer(2, 2, 0, 0);
    wxBoxSizer* sizer_2 = new wxBoxSizer(wxVERTICAL);
    wxGridSizer* grid_sizer_1 = new wxGridSizer(2, 2, 0, 0);
    grid_sizer_1->Add(label_TblName, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
    grid_sizer_1->Add(text_TblName, 0, wxTOP, 5);
    grid_sizer_1->Add(label_DMode, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
    grid_sizer_1->Add(combo_box_1, 0, wxALIGN_CENTER_VERTICAL, 10);
    sizer_1->Add(grid_sizer_1, 1, wxEXPAND, 0);
    sizer_2->Add(checkbox_1, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL, 10);
    sizer_2->Add(label_1, 0, wxLEFT, 60);
    sizer_1->Add(sizer_2, 1, wxALL|wxEXPAND, 20);
    grid_sizer_2->Add(label_TblNum, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 10);
    grid_sizer_2->Add(spin_ctrl_1, 0, wxALIGN_CENTER_VERTICAL, 0);
    grid_sizer_2->Add(button_1, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 10);
    grid_sizer_2->Add(button_2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 10);
    sizer_1->Add(grid_sizer_2, 1, wxEXPAND, 0);
    SetAutoLayout(true);
    SetSizer(sizer_1);
    Layout();
    Centre();

}

wxString TblPropDialog::GetTblName()
{
    wxString value = text_TblName->GetValue() + wxT(" - ") +  combo_box_1->GetValue();    

    return value;
}

wxString TblPropDialog::GetName()
{
    wxString name;
    name = text_TblName->GetValue();

    return name;
}

wxString TblPropDialog::GetMode()
{
    wxString mode;
    mode = combo_box_1->GetValue();

    return mode;
}

bool TblPropDialog::GetGlobal()
{
    bool global;
    global = checkbox_1->GetValue();

    return global;
}

int TblPropDialog::GetTblNum()
{
    int tblnum;
    tblnum = spin_ctrl_1->GetValue();

    return tblnum;
}

void TblPropDialog::SetName(wxString a)
{
    wxString name = a;
    text_TblName->SetValue(name);

}

void TblPropDialog::SetMode(wxString b)
{
    wxString mode = b;
    combo_box_1->SetValue(mode);

}

void TblPropDialog::SetGlobal(bool c)
{
    bool global = c;
    checkbox_1->SetValue(global);

    if (checkbox_1->GetValue()==true) {
        label_TblNum->Enable(true);
        spin_ctrl_1->Enable(true);
    }
    else {
        label_TblNum->Disable();
        spin_ctrl_1->Disable();
    }

}

void TblPropDialog::SetTblNum(int d)
{
    int tblnum = d;
    spin_ctrl_1->SetValue(tblnum);

}

void TblPropDialog::OnOk(wxCommandEvent &event)
{    
    EndModal(wxID_OK);
}


void TblPropDialog::OnGlblCheck(wxCommandEvent &event)
{

    if (checkbox_1->GetValue()==true) {
        label_TblNum->Enable(true);
        spin_ctrl_1->Enable(true);
    }
    else {
        label_TblNum->Disable();
        spin_ctrl_1->Disable();
    }
}











/* vim: set sw=4: */
