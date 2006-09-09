#include "wxstogglebutton.h"

#include <wx/tglbtn.h>
#include <messagemanager.h>


WXS_ST_BEGIN(wxsToggleButtonStyles)
    WXS_ST_CATEGORY("wxToggleButton")
    WXS_ST_DEFAULTS()
WXS_ST_END()


WXS_EV_BEGIN(wxsToggleButtonEvents)
    WXS_EVI(EVT_TOGGLEBUTTON,wxEVT_COMMAND_TOGGLEBUTTON_CLICKED,wxCommandEvent,Toggle)
    WXS_EV_DEFAULTS()
WXS_EV_END()


wxsItemInfo wxsToggleButton::Info =
{
    _T("wxToggleButton"),
    wxsTWidget,
    _("wxWidgets license"),
    _("wxWidgets team"),
    _T(""),
    _T("www.wxwidgets.org"),
    _T("Standard"),
    50,
    _T("ToggleButton"),
    2, 6,
    NULL,
    NULL,
    0
};


wxsToggleButton::wxsToggleButton(wxsWindowRes* Resource):
    wxsWidget(
        Resource,
        wxsBaseProperties::flAll,
        &Info,
        wxsToggleButtonEvents,
        wxsToggleButtonStyles,
        _T("")),
       Label(_("Label")),
       IsChecked(false)
{}


void wxsToggleButton::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code<< GetVarName() << _T(" = new wxToggleButton(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << wxsGetWxString(Label) << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(");\n");

            if ( IsChecked ) Code << GetVarName() << _T("->SetValue(true);\n");

            SetupWindowCode(Code,Language);
            return;
        }
    }

    wxsLANGMSG(wxsToggleButton::BuildCreatingCode,Language);
}


wxObject* wxsToggleButton::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    wxToggleButton* Preview = new wxToggleButton(Parent,GetId(),Label,Pos(Parent),Size(Parent),Style());

    Preview->SetValue(IsChecked);

    return SetupWindow(Preview,Exact);
}


void wxsToggleButton::EnumWidgetProperties(long Flags)
{
    WXS_STRING(wxsToggleButton,Label,0,_("Label"),_T("label"),_T(""),true,false)
    WXS_BOOL  (wxsToggleButton,IsChecked,0,_("Is checked"),_("checked"),false)
}

void wxsToggleButton::EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/tglbtn.h>")); return;
    }

    wxsLANGMSG(wxsToggleButton::EnumDeclFiles,Language);
}
