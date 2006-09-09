#include "wxscheckbox.h"

#include <wx/checkbox.h>
#include <messagemanager.h>


WXS_ST_BEGIN(wxsCheckBoxStyles)
    WXS_ST_CATEGORY("wxCheckBox")
    WXS_ST(wxCHK_2STATE)
    WXS_ST(wxALIGN_RIGHT)
    WXS_ST_MASK(wxCHK_3STATE,wxsSFAll,wxsSFMGL |wxsSFGTK12 | wxsSFOS2,true)
    WXS_ST_MASK(wxCHK_ALLOW_3RD_STATE_FOR_USER,wxsSFAll,wxsSFMGL |wxsSFGTK12 | wxsSFOS2,true)
WXS_ST_END()


WXS_EV_BEGIN(wxsCheckBoxEvents)
    WXS_EVI(EVT_CHECKBOX,wxEVT_COMMAND_CHECKBOX_CLICKED,wxCommandEvent,Click)
    WXS_EV_DEFAULTS()
WXS_EV_END()


wxsItemInfo wxsCheckBox::Info =
{
    _T("wxCheckBox"),
    wxsTWidget,
    _("wxWidgets license"),
    _("wxWidgets team"),
    _T(""),
    _T("www.wxwidgets.org"),
    _T("Standard"),
    60,
    _T("CheckBox"),
    2, 6,
    NULL,
    NULL,
    0
};


wxsCheckBox::wxsCheckBox(wxsWindowRes* Resource):
    wxsWidget(
        Resource,
        wxsBaseProperties::flAll,
        &Info,
        wxsCheckBoxEvents,
        wxsCheckBoxStyles,
        _T("")),
    Label(_("Label")),
    IsChecked(false)
{}


void wxsCheckBox::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code<< GetVarName() << _T(" = new wxCheckBox(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << wxsGetWxString(Label) << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(");\n");

            if ( IsChecked ) Code << GetVarName() << _T("->SetValue(")
                             << wxString::Format(_T("%d"),IsChecked) << _T(";\n");
            SetupWindowCode(Code,Language);
            return;
        }
    }

    wxsLANGMSG(wxsCheckBox::BuildCreatingCode,Language);
}


wxObject* wxsCheckBox::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    wxCheckBox* Preview = new wxCheckBox(Parent,GetId(),Label,Pos(Parent),Size(Parent),Style());
    if ( IsChecked ) Preview->SetValue(IsChecked);
    return SetupWindow(Preview,Exact);
}


void wxsCheckBox::EnumWidgetProperties(long Flags)
{
    WXS_STRING(wxsCheckBox,Label,0,_("Label"),_T("label"),_T(""),true,false)
    WXS_BOOL  (wxsCheckBox,IsChecked,0,_("Checked"),_("checked"),false)
}

void wxsCheckBox::EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/checkbox.h>")); return;
    }

    wxsLANGMSG(wxsCheckBox::EnumDeclFiles,Language);
}
