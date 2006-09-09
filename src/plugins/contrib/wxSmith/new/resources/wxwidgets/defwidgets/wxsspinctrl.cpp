#include "wxsspinctrl.h"

#include <wx/spinctrl.h>
#include <messagemanager.h>


WXS_ST_BEGIN(wxsSpinCtrlStyles)
    WXS_ST_CATEGORY("wxSpinCtrl")

// NOTE (cyberkoa##): wxSP_HORIZONTAL, wxSP_VERTICAL are not found in HELP but in wxMSW's XRC. Assume same as spinbutton
    WXS_ST_MASK(wxSP_HORIZONTAL,wxsSFAll,wxsSFGTK,true)
    WXS_ST(wxSP_VERTICAL)

    WXS_ST(wxSP_ARROW_KEYS)
    WXS_ST(wxSP_WRAP)
WXS_ST_END()


WXS_EV_BEGIN(wxsSpinCtrlEvents)
    WXS_EVI(EVT_SPINCTRL,wxEVT_COMMAND_SPINCTRL_UPDATED,wxSpinEvent,Change)
    WXS_EV_DEFAULTS()
WXS_EV_END()


wxsItemInfo wxsSpinCtrl::Info =
{
    _T("wxSpinCtrl"),
    wxsTWidget,
    _("wxWidgets license"),
    _("wxWidgets team"),
    _T(""),
    _T("www.wxwidgets.org"),
    _T("Standard"),
    50,
    _T("SpinCtrl"),
    2, 6,
    NULL,
    NULL,
    0
};



wxsSpinCtrl::wxsSpinCtrl(wxsWindowRes* Resource):
    wxsWidget(
        Resource,
        wxsBaseProperties::flAll,
        &Info,
        wxsSpinCtrlEvents,
        wxsSpinCtrlStyles,
        _T("")),
        Value(_T("0")),
        Min(0),
        Max(100)
{}


void wxsSpinCtrl::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code<< GetVarName() << _T(" = new wxSpinCtrl(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << wxsGetWxString(Value) << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(",")
                << wxString::Format(_T("%d"),Min) << _T(",")
                << wxString::Format(_T("%d"),Max) << _T(");\n");

            if ( !Value.empty() )
            {
                Code << GetVarName() << _T("->SetValue(") << wxsGetWxString(Value) << _(");\n");
            }
            SetupWindowCode(Code,Language);
            return;
        }
    }

    wxsLANGMSG(wxsSpinCtrl::BuildCreatingCode,Language);
}


wxObject* wxsSpinCtrl::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    wxSpinCtrl* Preview = new wxSpinCtrl(Parent,GetId(),Value,Pos(Parent),Size(Parent),Style(),Min,Max);
    if ( !Value.empty() ) Preview->SetValue(Value);
    return SetupWindow(Preview,Exact);
}


void wxsSpinCtrl::EnumWidgetProperties(long Flags)
{
    WXS_STRING(wxsSpinCtrl,Value,0,_("Value"),_T("value"),_T(""),true,false)
    WXS_LONG(wxsSpinCtrl,Min,0,_("Min"),_T("Min"),0)
    WXS_LONG(wxsSpinCtrl,Max,0,_("Max"),_T("Max"),0)
}

void wxsSpinCtrl::EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/spinctrl.h>")); return;
    }

    wxsLANGMSG(wxsSpinCtrl::EnumDeclFiles,Language);
}
