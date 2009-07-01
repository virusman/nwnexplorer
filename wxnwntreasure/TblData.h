
#include <wx/wx.h>
#include <wx/treectrl.h>

class TblData: public wxTreeItemData {
    wxString name;
    wxString mode;
    bool global;
    int tblnum;

    public:
    TblData (wxString a, wxString b, bool c, int d) {name=a; mode=b; global=c; tblnum=d;}

    wxString GetName();
    wxString GetMode();
    bool GetGlobal();
    int GetTblNum();

    void SetName(wxString);
    void SetMode(wxString);
    void SetGlobal(bool);
    void SetTblNum(int);
};

wxString TblData::GetName()
{
    return name;
}

wxString TblData::GetMode()
{
    return mode;
}

bool TblData::GetGlobal()
{
    return global;
}

int TblData::GetTblNum()
{
    return tblnum;
}

void TblData::SetName(wxString w)
{
    name = w;
}

void TblData::SetMode(wxString x)
{
    mode = x;
}

void TblData::SetGlobal(bool y)
{
    global = y;
}

void TblData::SetTblNum(int z)
{
    tblnum = z;
}


/* vim: set sw=4: */
