#include "wxsradiobutton.h"

#include <wx/radiobut.h>
#include <messagemanager.h>


WXS_ST_BEGIN(wxsRadioButtonStyles)
    WXS_ST_CATEGORY("wxRadioButton")
    WXS_ST(wxRB_GROUP)
    WXS_ST(wxRB_SINGLE)
// NOTE (cyberkoa#1#): wxRB_USE_CHECKBOX is only available under PALMOS, not sure whether in XRC PALMOS port or not
    WXS_ST_MASK(wxRB_USE_CHECKBOX,wxsSFPALMOS,0,true)
WXS_ST_END()


WXS_EV_BEGIN(wxsRadioButtonEvents)
    WXS_EVI(EVT_RADIOBUTTON,wxEVT_COMMAND_RADIOBUTTON_SELECTED,wxCommandEvent,Select)
    WXS_EV_DEFAULTS()
WXS_EV_END()


wxsItemInfo wxsRadioButton::Info =
{
    _T("wxRadioButton"),
    wxsTWidget,
    _("wxWidgets license"),
    _("wxWidgets team"),
    _T(""),
    _T("www.wxwidgets.org"),
    _T("Standard"),
    60,
    _T("RadioButton"),
    2, 6,
    NULL,
    NULL,
    0
};


wxsRadioButton::wxsRadioButton(wxsWindowRes* Resource):
    wxsWidget(
        Resource,
        wxsBaseProperties::flAll,
        &Info,
        wxsRadioButtonEvents,
        wxsRadioButtonStyles,
        _T("")),
    Label(_("Label")),
    IsSelected(false)
{}


void wxsRadioButton::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code<< GetVarName() << _T(" = new wxRadioButton(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << wxsGetWxString(Label) << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(");\n");

            if ( IsSelected )
            {
                Code << GetVarName() << _T("->SetValue(true);\n");
            }
            SetupWindowCode(Code,Language);
            return;
        }
    }

    wxsLANGMSG(wxsRadioButton::BuildCreatingCode,Language);
}


wxObject* wxsRadioButton::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    wxRadioButton* Preview = new wxRadioButton(Parent,GetId(),Label,Pos(Parent),Size(Parent),Style());
    Preview->SetValue(IsSelected);

    return SetupWindow(Preview,Exact);
}



void wxsRadioButton::EnumWidgetProperties(long Flags)
{
    WXS_STRING(wxsRadioButton,Label,0,_("Label"),_T("label"),_T(""),true,false)
    WXS_BOOL  (wxsRadioButton,IsSelected,0,_("Is Selected"),_("selected"),false)
}

void wxsRadioButton::EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/radiobut.h>")); return;
    }

    wxsLANGMSG(wxsRadioButton::EnumDeclFiles,Language);
}
