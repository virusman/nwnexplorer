/***************************************************************************
 *   Copyright (C) 2003 by John S. Brown                                   *
 *   malakh@dragoncat dot net                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/




#ifndef WXNWNTREASURE_H
#define WXNWNTREASURE_H


#include "wx/string.h"


//! IDs for various widget events.
enum
{
    ID_New,
    ID_Open = 1,
    ID_Save,
    ID_Create,
    ID_After,
    ID_Table,
    ID_Prop,
    ID_TProp,
    ID_Delete,
    ID_Up,
    ID_Down,
    ID_About = wxID_ABOUT,
    ID_Exit,
    ID_Empty,
    ID_Treasure_Tbl,
    ID_TreeChg,
};



class NwnTFrame: public wxFrame {
    public:

        NwnTFrame(wxWindow* parent, int id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxDEFAULT_FRAME_STYLE);


    private:

        void set_properties();
        void do_layout();

        void ShowItemProp();
        void ShowEncProp();


    protected:

        wxMenuBar* _mb;
        wxStatusBar* _sb;
        wxToolBar* _tb;
        wxTreeCtrl* _tc1;
        wxTreeItemId _tc1_b1;
        wxTreeItemId _tc1_b2;
        wxTreeItemId _tc1_b3;
        wxTreeItemId _tc1_b4;
        wxTreeItemId _tc1_b4_sb1;



        void OnNew(wxCommandEvent& event);
        void OnOpen(wxCommandEvent& event);
        void OnSave(wxCommandEvent& event);
        void OnCreate(wxCommandEvent& event);
        void OnAfter(wxCommandEvent& event);
        void OnTable(wxCommandEvent& event);
        void OnProp(wxCommandEvent& event);
        void OnTProp(wxCommandEvent& event);
        void OnDelete(wxCommandEvent& event);
        void OnUp(wxCommandEvent& event);
        void OnDown(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnExit(wxCommandEvent& event);
        void OnTreeChg(wxCommandEvent& event);

    private:
        DECLARE_EVENT_TABLE()

};















#endif // WXNWNTREASURE_H

/* vim: set sw=4: */
