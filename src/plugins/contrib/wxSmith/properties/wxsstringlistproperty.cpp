#include "../wxsheaders.h"
#include "wxsstringlistproperty.h"

#include <wx/button.h>
#include <wx/tokenzr.h>

wxsStringListProperty::wxsStringListProperty(wxsProperties* Properties,wxArrayString& _Array):
	wxsProperty(Properties),
	Array(_Array),
	Selected(NULL),
	SortedFlag(0), PGId(0), SelId(0)
{}

wxsStringListProperty::wxsStringListProperty(wxsProperties* Properties,wxArrayString& _Array,int& _Selected,int _SortedFlag):
	wxsProperty(Properties),
	Array(_Array),
	Selected(&_Selected),
	SortedFlag(_SortedFlag), PGId(0), SelId(0)
{}

wxsStringListProperty::~wxsStringListProperty()
{}

const wxString& wxsStringListProperty::GetTypeName()
{
    static wxString Name(_T("wxArrayString"));
    return Name;
}

void wxsStringListProperty::AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name)
{
    PGId = Grid->Append(wxArrayStringProperty(Name,wxPG_LABEL,Array));
    if ( Selected )
    {
        SelId = Grid->Append(wxEnumProperty(_("Selection"),wxPG_LABEL));
        BuildChoices(Grid);
        Grid->SetPropertyValue(SelId,*Selected);
    }
}

bool wxsStringListProperty::PropGridChanged(wxPropertyGrid* Grid,wxPGId Id)
{
    if ( Id == PGId )
    {
        Array = Grid->GetPropertyValue(Id).GetArrayString();
        if ( Selected )
        {
            RebuildChoices(Grid);
        }
        return ValueChanged(true);
    }
    if ( Selected && Id == SelId )
    {
        *Selected = Grid->GetPropertyValue(SelId).GetLong();
        return ValueChanged(true);
    }
    return true;
}

void wxsStringListProperty::UpdatePropGrid(wxPropertyGrid* Grid)
{
    Grid->SetPropertyValue(PGId,Array);
    if ( Selected )
    {
        BuildChoices(Grid);
        Grid->SetPropertyValue(SelId,*Selected);
    }
}

void wxsStringListProperty::BuildChoices(wxPropertyGrid* Grid)
{
    wxPGConstants& consts = Grid->GetPropertyChoices(SelId);
    static const wxChar* None[] = { _("-- None --"), NULL };
    static long NoneValue[] = { -1 };
    consts.Set(None,NoneValue);
    wxArrayString Items = Array;
    wxArrayInt Values;
    for ( size_t i = 0; i < Items.Count(); i++ )
    {
        Values.Add(i);
    }
    if ( IsSorted() )
    {
        Items.Sort();
    }
    consts.Add(Items,Values);
}

void wxsStringListProperty::RebuildChoices(wxPropertyGrid* Grid)
{
    long SelectedNum = Grid->GetPropertyValue(SelId).GetLong();
    if ( SelectedNum != -1 )
    {
        wxString SelectedStr = Grid->GetPropertyValue(SelId);
        BuildChoices(Grid);
        long Index = -1;
        if ( IsSorted() )
        {
            wxArrayString Items = Array;
            Items.Sort();
            Index = Items.Index(SelectedStr);
        }
        else
        {
            Index = Array.Index(SelectedStr);
        }

        if ( Index < 0 || Index >= (long)Array.Count() )
        {
            Index = -1;
        }

        *Selected = Index;
        Grid->SetPropertyValue(SelId,Index);
    }
    else
    {
        BuildChoices(Grid);
        Grid->SetPropertyValue(SelId,-1);
    }
}

bool wxsStringListProperty::IsSorted()
{
    assert ( GetProperties() );
    assert ( GetProperties()->GetWidget() );
    return ( GetProperties()->GetWidget()->GetStyle() & SortedFlag ) != 0;
}
