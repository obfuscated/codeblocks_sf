#include "../wxsheaders.h"
#include "wxsfontproperty.h"

#include <wx/intl.h>
#include <wx/propgrid/advprops.h>

wxsFontProperty::wxsFontProperty(wxsProperties* Properties,bool& _Use,wxFont& _Font):
    wxsProperty(Properties),
    PGId(0),
    UseId(0),
    Use(_Use),
    Font(_Font)
{
}

wxsFontProperty::~wxsFontProperty()
{
}

const wxString & wxsFontProperty::GetTypeName()
{
    static wxString Type(_T("wxFont"));
    return Type;
}

void wxsFontProperty::AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name)
{
    PGId = Grid->Append( wxParentProperty(Name,wxPG_LABEL) );

    UseId = Grid->AppendIn(
        PGId,
        wxBoolProperty(_("Use font:"),wxPG_LABEL,Use ) );

    FontId = Grid->AppendIn(
        PGId,
        wxFontProperty(_("Font:"), wxPG_LABEL, Font ));

    Grid->SetPropertyAttribute(UseId,wxPG_BOOL_USE_CHECKBOX,(long)1,wxRECURSE);
}

bool wxsFontProperty::PropGridChanged(wxPropertyGrid* Grid,wxPGId Id)
{
    if ( Id == PGId || Id == UseId || Id == FontId )
    {
    	Use = Grid->GetPropertyValue(UseId).GetBool();
    	Font = *wxDynamicCast(Grid->GetPropertyValueAsWxObjectPtr(FontId),wxFont);
        Grid->EnableProperty(FontId,Use);
    	return ValueChanged(true);
    }
    return true;
}

void wxsFontProperty::UpdatePropGrid(wxPropertyGrid* Grid)
{
    Grid->SetPropertyValue(UseId,Use);
    Grid->SetPropertyValue(FontId,Font);
    Grid->EnableProperty(FontId,Use);
}
