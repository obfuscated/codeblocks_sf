#include "../wxsheaders.h"
#include "wxsborderproperty.h"

#include <wx/panel.h>
#include <wx/tglbtn.h>
#include <wx/checkbox.h>

#include "../wxsdefsizer.h"

wxsBorderProperty::wxsBorderProperty(wxsProperties* Properties,int& Flag):
    wxsProperty(Properties),
    BorderFlags(Flag),
    PGId(0)
{
	//ctor
}

wxsBorderProperty::~wxsBorderProperty()
{
	//dtor
}

const wxString& wxsBorderProperty::GetTypeName()
{
    static wxString Type(_T("Widget Border"));
    return Type;
}

void wxsBorderProperty::AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name)
{
    static const wxChar* Borders[] =
    {
        _("Left"),
        _("Right"),
        _("Top"),
        _("Bottom"),
        NULL
    };

    static long Values[] =
    {
        wxsSizerExtraParams::Left,
        wxsSizerExtraParams::Right,
        wxsSizerExtraParams::Top,
        wxsSizerExtraParams::Bottom
    };

    PGId = Grid->Append(wxFlagsProperty(Name,wxPG_LABEL,Borders,Values,0,BorderFlags));
    Grid->SetPropertyAttribute(PGId,wxPG_BOOL_USE_CHECKBOX,(long)1,wxRECURSE);
}

bool wxsBorderProperty::PropGridChanged(wxPropertyGrid* Grid,wxPGId Id)
{
    if ( Id == PGId )
    {
        BorderFlags = Grid->GetPropertyValue(Id).GetLong();
        return ValueChanged(false);
    }
    return true;
}

void wxsBorderProperty::UpdatePropGrid(wxPropertyGrid* Grid)
{
    Grid->SetPropertyValue(PGId,BorderFlags);
}
