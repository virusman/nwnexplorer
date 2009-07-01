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
#include <wx/spinctrl.h>


#ifndef ENCITEMPROPDLG_H
#define ENCITEMPROPDLG_H

enum
{
    ID_EncChoice,
};


class EncItemPropDlg: public wxDialog {
    public:



        EncItemPropDlg(wxWindow* parent, int id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxDEFAULT_DIALOG_STYLE);
        wxString GetTblName();
        int GetIcon();

        wxString GetAction();
        int GetChance();
        wxString GetBlueprint();
        int GetMin();
        int GetMax();
        wxString GetTableName();
        wxString GetMod();
        wxString GetSpecific();

        void SetAction(wxString);
        void SetChance(int);

        void SetItem(wxString, int, int);
        void SetTable(wxString, int, int, wxString, wxString);

        void ShowNothing();
        void ShowItem();
        void ShowTable();

    private:

        void set_properties();
        void do_layout();


        void OnOk(wxCommandEvent &event);
        void OnChoiceSel(wxCommandEvent &event);
        int icon;


    protected:

        wxStaticText* label_1;
        wxStaticText* label_2;
        wxStaticText* label_3;
        wxStaticText* label_4;
        wxStaticText* label_5; 
        wxStaticText* label_6;
        wxStaticText* label_7;
        wxStaticText* label_8; 
        wxStaticText* label_9;    
        wxStaticText* label_10;
        wxStaticText* label_11;
        wxStaticText* label_12;
        wxStaticText* label_13;
        wxStaticText* label_14;
        wxStaticText* label_15;
        wxTextCtrl* text_1;
        wxTextCtrl* text_2;
        wxChoice* choice_1;
        wxChoice* choice_2;
        wxChoice* choice_3;
        wxChoice* choice_4;
        wxSpinCtrl* spin_1;
        wxSpinCtrl* spin_2;
        wxSpinCtrl* spin_3;
        wxSpinCtrl* spin_4;
        wxSpinCtrl* spin_5;
        wxSpinCtrl* spin_6;
        wxSpinCtrl* spin_7;
        wxButton* button_1;
        wxButton* button_2;
        wxButton* button_3;
        wxBoxSizer* main_sizer;
        wxBoxSizer* text_sizer;    
        wxGridSizer* gridsizer_top;
        wxGridSizer* gridsizer_mid1;
        wxGridSizer* gridsizer_mid2;
        wxGridSizer* mid1_sizer1;
        wxGridSizer* mid1_sizer2;
        wxGridSizer* mid2_sizer1;
        wxGridSizer* mid2_sizer2;
        wxGridSizer* button_sizer;

        int current;





    private:
        DECLARE_EVENT_TABLE()

};



#endif // ENCITEMPROPDLG_H

/* vim: set sw=4: */
