#include "wxsgauge.h"

#include <wx/gauge.h>
#include <messagemanager.h>

/* ************************************************************************** */
/* Styles are processed almost like in older code - using WXS_ST_BEGIN,       */
/* WXS_ST_END, WXS_ST_CATEGORY, WXS_ST ...                                    */
/* ************************************************************************** */

WXS_ST_BEGIN(wxsGaugeStyles)
    WXS_ST(wxGA_HORIZONTAL)
    WXS_ST(wxGA_VERTICAL)
// NOTE (cyberkoa#1#): according to Help file , wxGA_SMOOTH is not support by all platform but does not mention which platform is not support.
    WXS_ST(wxGA_SMOOTH)
// NOTE (cyberkoa##): wxGA_PROGRESSBAR not in HELP file but in XRC code
//    WXS_ST(wxGA_PROGRESSBAR)
WXS_ST_END()


/* ************************************************************************** */
/* Events have one additional argument - event type, these can be found       */
/* in WX_DIR/include/event.h                                                  */
/* ************************************************************************** */

WXS_EV_BEGIN(wxsGaugeEvents)
    WXS_EV_DEFAULTS()
WXS_EV_END()

/* ************************************************************************** */
/* Widget info                                                                */
/* ************************************************************************** */

wxsItemInfo wxsGauge::Info =
{
    _T("wxGauge"),             // Name of class
    wxsTWidget,                 // Type, always wxsTWidget for widget classes
    _("wxWidgets license"),     // License, any type
    _("wxWidgets team"),        // Author
    _T(""),                     // No default e-mail for standard widgets
    _T("www.wxwidgets.org"),    // Site
    _T("Standard"),             // Groud for widget, note that _T() instead of _() is used
    50,                         // Gauge is commonly used widgets - we give it normal priority
    _T("Gauge"),               // Standard prefix for variable names and identifiers
    2, 6,                       // Widget version
    NULL,                       // Bitmaps will be loaded later in manager
    NULL,                       // --------------------''-------------------
    0                           // --------------------''-------------------
};


/* ************************************************************************** */
/* Constructur for wxsWidget need style set, event set and widget info, so    */
/* these things should be created before ctor.                                */
/* ************************************************************************** */

wxsGauge::wxsGauge(wxsWindowRes* Resource):
    wxsWidget(
        Resource,                   // Resource is passed to wxsItem's constructor
        wxsBaseProperties::flAll,   // Using all base properties
        &Info,                      // Taking local info
        wxsGaugeEvents,            // Pointer to local events
        wxsGaugeStyles,            // Pointer to local styles
        _T("")),                     // Default style = 0
    Range(100),              // Initializing widget with some default (but not necessarily clean) values,
    Value(0),                // these values will be used when creating new widgets
    Shadow(0),
    Bezel(0)
{}


/* ************************************************************************** */
/* Function building code - it should append new code generating this widget  */
/* to the end of prevous code. Currently it support only C++ langage but some */
/* other languages may be added in future.                                    */
/* ************************************************************************** */

void wxsGauge::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            // Because c_str() may lead to unspecified behaviour
            // it's better to use << operator instead of wxString::Format.
            // But be carefull when using << to add integers and longs.
            // Because of some wxWidgets bugs use '<< wxString::Format(_T("%d"),Value)'
            // instead of '<< Value'
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

/* ************************************************************************** */
/* Function building preview item. This should simply return previewed object */
/* with all properties set-up.                                                */
/* ************************************************************************** */

wxObject* wxsGauge::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    // Exact argument is not used here because button preview is
    // exactly the same in editor and in preview window
    wxGauge* Preview = new wxGauge(Parent,GetId(),Range,Pos(Parent),Size(Parent),Style());
    if ( Value )  Preview->SetValue(Value);
    if ( Shadow ) Preview->SetShadowWidth(Shadow);
    if ( Bezel )  Preview->SetBezelFace(Bezel);
    return SetupWindow(Preview,Exact);
}


/* ************************************************************************** */
/* Function enumerating all button-specific properties.                       */
/* ************************************************************************** */

void wxsGauge::EnumWidgetProperties(long Flags)
{
    WXS_LONG(wxsGauge,Value,0,_("Value"),_T("value"),0)
    WXS_LONG(wxsGauge,Range,0,_("Range"),_T("range"),100)
    WXS_LONG(wxsGauge,Shadow,0,_("3D Shadow Width"),_T("shadow"),0)
    WXS_LONG(wxsGauge,Bezel,0,_("Bezel Face Width"),_T("bezel"),0)
}
