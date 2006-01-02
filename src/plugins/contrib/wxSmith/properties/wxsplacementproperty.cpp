#include "../wxsheaders.h"
#include "wxsplacementproperty.h"

#include <wx/panel.h>
#include <wx/tglbtn.h>
#include <wx/checkbox.h>

#include "../wxsdefsizer.h"

wxsPlacementProperty::wxsPlacementProperty(
    int& Placement,
    bool& _Expand,
    bool& _Shaped,
    const wxString& ExpandPropName,
    const wxString& ShapedPropName):
        PlacementType(Placement),
        Expand(_Expand),
        Shaped(_Shaped),
        ExpandName(ExpandPropName),
        ShapedName(ShapedPropName),
        PGId(0),
        ExpandId(0),
        ShapedId(0)
{
	//ctor
}

void wxsPlacementProperty::AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name)
{
    static const wxChar* Placements[] =
    {
        _("Left - Top"),
        _("Top"),
        _("Right - Top"),
        _("Right"),
        _("Right - Bottom"),
        _("Bottom"),
        _("Left - Bottom"),
        _("Left"),
        _("Center"),
        NULL
    };

    static long Values[] =
    {
        wxsSizerExtraParams::LeftTop,
        wxsSizerExtraParams::CenterTop,
        wxsSizerExtraParams::RightTop,
        wxsSizerExtraParams::RightCenter,
        wxsSizerExtraParams::RightBottom,
        wxsSizerExtraParams::CenterBottom,
        wxsSizerExtraParams::LeftBottom,
        wxsSizerExtraParams::LeftCenter,
        wxsSizerExtraParams::Center
    };

    PGId = Grid->Append( wxEnumProperty(Name,wxPG_LABEL,Placements,Values,0,PlacementType) );
    ExpandId = Grid->Append( wxBoolProperty(ExpandName, wxPG_LABEL, Expand ) );
    ShapedId = Grid->Append( wxBoolProperty(ShapedName, wxPG_LABEL, Shaped ) );
    Grid->SetPropertyAttribute(ExpandId,wxPG_BOOL_USE_CHECKBOX,(long)1,wxRECURSE);
    Grid->SetPropertyAttribute(ShapedId,wxPG_BOOL_USE_CHECKBOX,(long)1,wxRECURSE);
}

bool wxsPlacementProperty::PropGridChanged(wxPropertyGrid* Grid,wxPGId Id)
{
    if ( Id == PGId || Id == ExpandId || Id == ShapedId )
    {
        PlacementType = Grid->GetPropertyValue(PGId).GetLong();
        Expand = Grid->GetPropertyValue(ExpandId).GetBool();
        Shaped = Grid->GetPropertyValue(ShapedId).GetBool();
        return ValueChanged(true);
    }
    return true;
}

void wxsPlacementProperty::UpdatePropGrid(wxPropertyGrid* Grid)
{
    Grid->SetPropertyValue(PGId,PlacementType);
    Grid->SetPropertyValue(ExpandId,Expand);
    Grid->SetPropertyValue(ShapedId,Shaped);
}
