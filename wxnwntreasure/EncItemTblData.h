
#include <wx/wx.h>
#include <wx/treectrl.h>

class EncItemTblData: public wxTreeItemData {
    wxString action;
    int chance;

    wxString blueprint;
    int min;
    int max;
    wxString tablename;
    wxString mod;
    wxString specific;

    public:
    EncItemTblData (wxString a, int b) 
    { action=a; chance=b; }

    wxString GetAction();
    int GetChance();

    wxString GetBlueprint();
    int GetMin();
    int GetMax();
    wxString GetTableName();
    wxString GetMod();
    wxString GetSpecific();

    void SetData(wxString, int);

    void SetItem(wxString, int, int);
    void SetTable(wxString, int, int, wxString, wxString);

};

wxString EncItemTblData::GetAction()
{
    return action;
}

int EncItemTblData::GetChance()
{
    return chance;
}

wxString EncItemTblData::GetBlueprint()
{
    return blueprint;
}

int EncItemTblData::GetMin()
{
    return min;
}

int EncItemTblData::GetMax()
{
    return max;
}

wxString EncItemTblData::GetTableName()
{
    return tablename;
}

wxString EncItemTblData::GetMod()
{
    return mod;
}

wxString EncItemTblData::GetSpecific()
{
    return specific;
}

void EncItemTblData::SetData(wxString a, int b)
{
    action = a; chance = b; 
}

void EncItemTblData::SetItem(wxString a, int b, int c)
{
    blueprint = a; min = b; max = c;
}

void EncItemTblData::SetTable(wxString a, int b, int c, wxString d, wxString e)
{
    tablename = a; min = b; max = c; mod = d; specific = e;
}

/* vim: set sw=4: */
