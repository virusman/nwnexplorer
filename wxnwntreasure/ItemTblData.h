
#include <wx/wx.h>
#include <wx/treectrl.h>

class ItemTblData: public wxTreeItemData {
    wxString action;
    int chance;
    int numdice;
    int die;
    wxString multiplier;
    wxString blueprint;
    int min;
    int max;
    wxString tablename;
    wxString mod;
    wxString specific;

    public:
    ItemTblData (wxString a, int b) 
    { action=a; chance=b; }

    wxString GetAction();
    int GetChance();
    int GetNumDice();
    int GetDie();
    wxString GetMultiplier();
    wxString GetBlueprint();
    int GetMin();
    int GetMax();
    wxString GetTableName();
    wxString GetMod();
    wxString GetSpecific();

    void SetData(wxString, int);
    void SetGold(int, int, wxString);
    void SetItem(wxString, int, int);
    void SetTable(wxString, int, int, wxString, wxString);

};

wxString ItemTblData::GetAction()
{
    return action;
}

int ItemTblData::GetChance()
{
    return chance;
}

int ItemTblData::GetNumDice()
{
    return numdice;
}

int ItemTblData::GetDie()
{
    return die;
}

wxString ItemTblData::GetMultiplier()
{
    return multiplier;
}

wxString ItemTblData::GetBlueprint()
{
    return blueprint;
}

int ItemTblData::GetMin()
{
    return min;
}

int ItemTblData::GetMax()
{
    return max;
}

wxString ItemTblData::GetTableName()
{
    return tablename;
}

wxString ItemTblData::GetMod()
{
    return mod;
}

wxString ItemTblData::GetSpecific()
{
    return specific;
}

void ItemTblData::SetData(wxString a, int b)
{
    action = a; chance = b; 
}

void ItemTblData::SetGold(int a, int b, wxString c)
{
    numdice = a; die = b; multiplier = c;
}

void ItemTblData::SetItem(wxString a, int b, int c)
{
    blueprint = a; min = b; max = c;
}

void ItemTblData::SetTable(wxString a, int b, int c, wxString d, wxString e)
{
    tablename = a; min = b; max = c; mod = d; specific = e;
}

/* vim: set sw=4: */
