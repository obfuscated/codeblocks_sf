#include "../wxsheaders.h"
#include "wxsstringlistcheckproperty.h"

#include <wx/button.h>
#include <wx/tokenzr.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

#include "wxscheckstringseditor.h"

class wxsStringListCheckPropClass: public wxCustomPropertyClass
{
    public:
        wxsStringListCheckPropClass(const wxString& label, const wxString& name,
            wxsStringListCheckProperty* _Prop):
            wxCustomPropertyClass(label,name),
            Prop(_Prop)
        {}
        wxsStringListCheckProperty* Prop;
};

wxsStringListCheckProperty::wxsStringListCheckProperty(
    wxArrayString& _Array,
    wxsArrayBool& _Checks,
    int _SortedFlag):
        Array(_Array),
        Checks(_Checks),
        SortedFlag(_SortedFlag),
        PGId(0)
{}

void wxsStringListCheckProperty::AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name)
{
    PGId = Grid->Append(new wxsStringListCheckPropClass(Name,wxPG_LABEL,this));
    Grid->SetPropertyAttribute(PGId,wxPG_CUSTOM_EDITOR,wxPGEditor_TextCtrlAndButton);
    Grid->SetPropertyAttribute(PGId,wxPG_CUSTOM_CALLBACK,(void*)PropertyGridCallback);
    RebuildData(Grid);
}

bool wxsStringListCheckProperty::PropGridChanged(wxPropertyGrid* Grid,wxPGId Id)
{
    if ( Id == PGId )
    {
        RebuildData(Grid);
    }
    return true;
}

void wxsStringListCheckProperty::UpdatePropGrid(wxPropertyGrid* Grid)
{
    RebuildData(Grid);
}

bool wxsStringListCheckProperty::PropertyGridCallback(wxPropertyGrid* propGrid,wxPGProperty* property,wxPGCtrlClass* ctrl,int data)
{
    return ((wxsStringListCheckPropClass*)property)->Prop->EditValues(propGrid);
}

bool wxsStringListCheckProperty::EditValues(wxPropertyGrid* propGrid)
{
    wxsCheckStringsEditor Dlg(0L,Array,Checks,IsSorted());
    if ( Dlg.ShowModal() != wxID_OK ) return true;
    Array = Dlg.Strings;
    Checks = Dlg.Bools;
    RebuildData(propGrid);
    ValueChanged(true);
    return true;
}

bool wxsStringListCheckProperty::IsSorted()
{
    assert ( GetProperties() );
    assert ( GetProperties()->GetWidget() );
    return ( GetProperties()->GetWidget()->GetStyle() & SortedFlag ) != 0;
}

void wxsStringListCheckProperty::RebuildData(wxPropertyGrid* propGrid)
{
    wxString Content;
    for ( size_t i = 0; i < Array.Count(); ++i )
    {
        if ( i > 0 )     Content.Append(_T(" "));
        if ( Checks[i] ) Content.Append(_T("*["));
        else             Content.Append(_T("["));
        Content.Append(Array[i]);
        Content.Append(_T("]"));
    }
    
    propGrid->SetPropertyValue(PGId,Content);
}
