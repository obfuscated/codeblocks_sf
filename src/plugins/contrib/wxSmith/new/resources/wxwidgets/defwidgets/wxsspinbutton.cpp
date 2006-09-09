#include "wxsspinbutton.h"

#include <wx/spinbutt.h>
#include <messagemanager.h>


WXS_ST_BEGIN(wxsSpinButtonStyles)
    WXS_ST_CATEGORY("wxsSpinButton")

// NOTE (cyberkoa##): wxSP_HORIZONTAL, wxSP_VERTICAL are not found in HELP but in wxMSW's XRC. Assume same as spinbutton
    WXS_ST_MASK(wxSP_HORIZONTAL,wxsSFAll,wxsSFGTK,true)
    WXS_ST(wxSP_VERTICAL)

    WXS_ST(wxSP_ARROW_KEYS)
    WXS_ST(wxSP_WRAP)
WXS_ST_END()



WXS_EV_BEGIN(wxsSpinButtonEvents)
    WXS_EVI(EVT_SPIN,wxEVT_SCROLL_THUMBTRACK,wxSpinEvent,Change)
    WXS_EVI(EVT_SPIN_UP,wxEVT_SCROLL_LINEUP,wxSpinEvent,ChangeUp)
    WXS_EVI(EVT_SPIN_DOWN,wxEVT_SCROLL_LINEDOWN,wxSpinEvent,ChangeDown)
    WXS_EV_DEFAULTS()
WXS_EV_END()


wxsItemInfo wxsSpinButton::Info =
{
    _T("wxSpinButton"),
    wxsTWidget,
    _("wxWidgets license"),
    _("wxWidgets team"),
    _T(""),
    _T("www.wxwidgets.org"),
    _T("Standard"),
    50,
    _T("SpinButton"),
    2, 6,
    NULL,
    NULL,
    0
};



wxsSpinButton::wxsSpinButton(wxsWindowRes* Resource):
    wxsWidget(
        Resource,
        wxsBaseProperties::flAll,
        &Info,
        wxsSpinButtonEvents,
        wxsSpinButtonStyles,
        _T("")),
        Value(Value),
        Min(0),
        Max(100)
{}



void wxsSpinButton::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code<< GetVarName() << _T(" = new wxSpinButton(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(",");

            if ( Value ) Code << GetVarName() << _T("->SetValue(") << wxString::Format(_T("%d"),Value) << _T(");\n");
            if ( Max > Min ) Code << GetVarName() << _T("->SetRange(") << wxString::Format(_T("%d"),Min) << _T(",") << wxString::Format(_T("%d"),Max) << _T(");\n");
            SetupWindowCode(Code,Language);
            return;
        }
    }

    wxsLANGMSG(wxsSpinButton::BuildCreatingCode,Language);
}


wxObject* wxsSpinButton::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    wxSpinButton* Preview = new wxSpinButton(Parent,GetId(),Pos(Parent),Size(Parent),Style());
    if ( Value ) Preview->SetValue(Value);
    if ( Max > Min ) Preview->SetRange(Min,Max);

    return SetupWindow(Preview,Exact);
}


void wxsSpinButton::EnumWidgetProperties(long Flags)
{
    WXS_STRING(wxsSpinButton,Value,0,_("Value"),_T("value"),_T(""),true,false)
    WXS_LONG(wxsSpinButton,Min,0,_("Min Value"),_T("min"),0)
    WXS_LONG(wxsSpinButton,Max,0,_("Max Value"),_T("max"),0)
}

void wxsSpinButton::EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/spinbutt.h>")); return;
    }

    wxsLANGMSG(wxsSpinButton::EnumDeclFiles,Language);
}
