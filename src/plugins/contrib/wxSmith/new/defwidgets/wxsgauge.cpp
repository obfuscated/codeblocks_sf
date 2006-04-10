#include "wxsgauge.h"

#include <wx/gauge.h>
#include <messagemanager.h>


WXS_ST_BEGIN(wxsGaugeStyles)
    WXS_ST(wxGA_HORIZONTAL)
    WXS_ST(wxGA_VERTICAL)
// NOTE (cyberkoa#1#): according to Help file , wxGA_SMOOTH is not support by all platform but does not mention which platform is not support.
    WXS_ST(wxGA_SMOOTH)
// NOTE (cyberkoa##): wxGA_PROGRESSBAR not in HELP file but in XRC code
//    WXS_ST(wxGA_PROGRESSBAR)
WXS_ST_END()


WXS_EV_BEGIN(wxsGaugeEvents)
    WXS_EV_DEFAULTS()
WXS_EV_END()


wxsItemInfo wxsGauge::Info =
{
    _T("wxGauge"),
    wxsTWidget,
    _("wxWidgets license"),
    _("wxWidgets team"),
    _T(""),
    _T("www.wxwidgets.org"),
    _T("Standard"),
    50,
    _T("Gauge"),
    2, 6,
    NULL,
    NULL,
    0
};

wxsGauge::wxsGauge(wxsWindowRes* Resource):
    wxsWidget(
        Resource,
        wxsBaseProperties::flAll,
        &Info,
        wxsGaugeEvents,
        wxsGaugeStyles,
        _T("")),
    Range(100),
    Value(0),
    Shadow(0),
    Bezel(0)
{}



void wxsGauge::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code<< GetVarName() << _T(" = new wxGauge(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << wxString::Format(_T("%d"),Range) << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(");\n");
            if ( Value )  Code << GetVarName() << _T("->SetValue(") << wxString::Format(_T("%d"),Value) << _T(");\n");
            if ( Shadow ) Code << GetVarName() << _T("->SetShadowWidth(") << wxString::Format(_T("%d"),Shadow) << _T(");\n");
            if ( Bezel )  Code << GetVarName() << _T("->SetBezelFace(") << wxString::Format(_T("%d"),Bezel) << _T(");\n");


            SetupWindowCode(Code,Language);
            return;
        }
    }

    wxsLANGMSG(wxsGauge::BuildCreatingCode,Language);
}


wxObject* wxsGauge::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    wxGauge* Preview = new wxGauge(Parent,GetId(),Range,Pos(Parent),Size(Parent),Style());
    if ( Value )  Preview->SetValue(Value);
    if ( Shadow ) Preview->SetShadowWidth(Shadow);
    if ( Bezel )  Preview->SetBezelFace(Bezel);
    return SetupWindow(Preview,Exact);
}


void wxsGauge::EnumWidgetProperties(long Flags)
{
    WXS_LONG(wxsGauge,Value,0,_("Value"),_T("value"),0)
    WXS_LONG(wxsGauge,Range,0,_("Range"),_T("range"),100)
    WXS_LONG(wxsGauge,Shadow,0,_("3D Shadow Width"),_T("shadow"),0)
    WXS_LONG(wxsGauge,Bezel,0,_("Bezel Face Width"),_T("bezel"),0)
}

void wxsGauge::EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/gauge.h>")); return;
    }

    wxsLANGMSG(wxsGauge::EnumDeclFiles,Language);
}
