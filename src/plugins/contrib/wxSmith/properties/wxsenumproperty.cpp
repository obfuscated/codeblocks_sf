#include "../wxsheaders.h"
#include "wxsenumproperty.h"

#include <wx/panel.h>
#include <wx/stattext.h>
#include <vector>

wxsEnumProperty::wxsEnumProperty(int &_Value,const wxChar** _Names,const long* _Values):
    Value(_Value),
    Names(_Names),
    Values(_Values),
    PGId(0)
{
}

void wxsEnumProperty::AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name)
{
    PGId = Grid->Append( wxEnumProperty(Name,wxPG_LABEL,Names,Values,0,Value) );
}

bool wxsEnumProperty::PropGridChanged(wxPropertyGrid* Grid,wxPGId Id)
{
    if ( Id == PGId )
    {
        Value = Grid->GetPropertyValue(Id).GetLong();
        return ValueChanged(true);
    }
    return true;
}

void wxsEnumProperty::UpdatePropGrid(wxPropertyGrid* Grid)
{
    Grid->SetPropertyValue(PGId,Value);
}
