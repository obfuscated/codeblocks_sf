#include "../wxsheaders.h"
#include "wxscolourproperty.h"

#include "../wxsglobals.h"
#include <wx/intl.h>

wxsColourProperty::wxsColourProperty(
    wxUint32& _Type,
    wxColour& _Colour,
    const wxString& UseSubName,
    const wxString& ColSubName):
        Type(_Type),
        Colour(_Colour),
        UseName(UseSubName),
        ColName(ColSubName),
        PGId(0),
        UseColId(0),
        ColId(0)
{
}

void wxsColourProperty::AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name)
{
    PGId = Grid->Append( wxParentProperty(Name,wxPG_LABEL) );

    UseColId = Grid->AppendIn(
        PGId,
        wxBoolProperty(UseName,wxPG_LABEL,Type != wxsNO_COLOUR ) );

    wxColour Col = wxColour(0,0,0);

    if ( Type == wxPG_COLOUR_CUSTOM )
    {
    	Col = Colour;
    }
    else if ( Type != wxsNO_COLOUR )
    {
    	Col = wxSystemSettings::GetColour((wxSystemColour)Type);
    }

    ColId = Grid->AppendIn(
        PGId,
        wxSystemColourProperty(ColName, wxPG_LABEL, Col ));

    Grid->SetPropertyAttribute(UseColId,wxPG_BOOL_USE_CHECKBOX,(long)1,wxRECURSE);
}

bool wxsColourProperty::PropGridChanged(wxPropertyGrid* Grid,wxPGId Id)
{
    if ( ( Id == PGId ) || ( Id == UseColId ) || ( Id == ColId ) )
    {
    	bool Use = Grid->GetPropertyValue(UseColId).GetBool();
    	if ( !Use )
    	{
    		Grid->EnableProperty(ColId,false);
    		Type = wxsNO_COLOUR;
    	}
    	else
    	{
    		Grid->EnableProperty(ColId,true);
            wxColourPropertyValue* ColVal =
                wxDynamicCast(Grid->GetPropertyValueAsWxObjectPtr(ColId),wxColourPropertyValue);
            Type = ColVal->m_type;
            Colour = wxColour(ColVal->m_colour);
    	}

    	return ValueChanged(true);
    }
    return true;
}

void wxsColourProperty::UpdatePropGrid(wxPropertyGrid* Grid)
{
	wxUint32 InternalType = wxPG_COLOUR_CUSTOM;
    if ( Type == wxsNO_COLOUR )
    {
    	Grid->SetPropertyValue(UseColId,false);
    	Grid->EnableProperty(ColId,false);
    }
    else
    {
    	Grid->SetPropertyValue(UseColId,true);
    	Grid->EnableProperty(ColId,true);
    	if ( Type != wxPG_COLOUR_CUSTOM )
    	{
    		Colour = wxSystemSettings::GetColour((wxSystemColour)Type);
    		InternalType = Type;
    	}
    }

    wxColourPropertyValue Value(InternalType,Colour);
    Grid->SetPropertyValue(ColId,Value);
}
