#include "../wxsheaders.h"
#include "wxsboolproperty.h"

#include <wx/checkbox.h>


wxsBoolProperty::wxsBoolProperty(wxsProperties* Properties,bool& Bool):
    wxsProperty(Properties), Value(Bool),
    PGId(0)
{
	//ctor
}

wxsBoolProperty::~wxsBoolProperty()
{
	//dtor
}

const wxString& wxsBoolProperty::GetTypeName()
{
    static wxString Name(_T("bool"));
    return Name;
}

void wxsBoolProperty::AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name)
{
    PGId = Grid->Append(Name,wxPG_LABEL,Value);
    Grid->SetPropertyAttribute(PGId,wxPG_BOOL_USE_CHECKBOX,(long)1,wxRECURSE);
}

bool wxsBoolProperty::PropGridChanged(wxPropertyGrid* Grid,wxPGId Id)
{
    if ( Id == PGId )
    {
        Value = Grid->GetPropertyValue(Id).GetBool();
        return ValueChanged(true);
    }
    return true;
}

void wxsBoolProperty::UpdatePropGrid(wxPropertyGrid* Grid)
{
    Grid->SetPropertyValue(PGId,Value);
}
