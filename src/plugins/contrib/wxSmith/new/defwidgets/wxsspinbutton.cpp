#include "wxsspinbutton.h"

#include <wx/spinbutt.h>
#include <messagemanager.h>

/* ************************************************************************** */
/* Styles are processed almost like in older code - using WXS_ST_BEGIN,       */
/* WXS_ST_END, WXS_ST_CATEGORY, WXS_ST ...                                    */
/* ************************************************************************** */

WXS_ST_BEGIN(wxsSpinButtonStyles)
    WXS_ST_CATEGORY("wxsSpinButton")

// NOTE (cyberkoa##): wxSP_HORIZONTAL, wxSP_VERTICAL are not found in HELP but in wxMSW's XRC. Assume same as spinbutton
    WXS_ST_MASK(wxSP_HORIZONTAL,wxsSFAll,wxsSFGTK,true)
    WXS_ST(wxSP_VERTICAL)

    WXS_ST(wxSP_ARROW_KEYS)
    WXS_ST(wxSP_WRAP)
WXS_ST_END()


/* ************************************************************************** */
/* Events have one additional argument - event type, these can be found       */
/* in WX_DIR/include/event.h                                                  */
/* ************************************************************************** */

WXS_EV_BEGIN(wxsSpinButtonEvents)
    WXS_EVI(EVT_SPIN,wxEVT_SCROLL_THUMBTRACK,wxSpinEvent,Change)
    WXS_EVI(EVT_SPIN_UP,wxEVT_SCROLL_LINEUP,wxSpinEvent,ChangeUp)
    WXS_EVI(EVT_SPIN_DOWN,wxEVT_SCROLL_LINEDOWN,wxSpinEvent,ChangeDown)
    WXS_EV_DEFAULTS()
WXS_EV_END()

/* ************************************************************************** */
/* Widget info                                                                */
/* ************************************************************************** */

wxsItemInfo wxsSpinButton::Info =
{
    _T("wxSpinButton"),         // Name of class
    wxsTWidget,                 // Type, always wxsTWidget for widget classes
    _("wxWidgets license"),     // License, any type
    _("wxWidgets team"),        // Author
    _T(""),                     // No default e-mail for standard widgets
    _T("www.wxwidgets.org"),    // Site
    _T("Standard"),             // Groud for widget, note that _T() instead of _() is used
    50,                         // Button is one of most commonly used widgets - we give it high priority
    _T("SpinButton"),           // Standard prefix for variable names and identifiers
    2, 6,                       // Widget version
    NULL,                       // Bitmaps will be loaded later in manager
    NULL,                       // --------------------''-------------------
    0                           // --------------------''-------------------
};


/* ************************************************************************** */
/* Constructur for wxsWidget need style set, event set and widget info, so    */
/* these things should be created before ctor.                                */
/* ************************************************************************** */

wxsSpinButton::wxsSpinButton(wxsWindowRes* Resource):
    wxsWidget(
        Resource,                   // Resource is passed to wxsItem's constructor
        wxsBaseProperties::flAll,   // Using all base properties
        &Info,                      // Taking local info
        wxsSpinButtonEvents,        // Pointer to local events
        wxsSpinButtonStyles,        // Pointer to local styles
        _T("")),                     // Default style = 0
        Value(Value),               // Initializing widget with some default (but not necessarily clean) values,
        Min(0),                     // these values will be used when creating new widgets
        Max(100)
{}


/* ************************************************************************** */
/* Function building code - it should append new code generating this widget  */
/* to the end of prevous code. Currently it support only C++ langage but some */
/* other languages may be added in future.                                    */
/* ************************************************************************** */

void wxsSpinButton::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
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

/* ************************************************************************** */
/* Function building preview item. This should simply return previewed object */
/* with all properties set-up.                                                */
/* ************************************************************************** */

wxObject* wxsSpinButton::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    // Exact argument is not used here because button preview is
    // exactly the same in editor and in preview window
    wxSpinButton* Preview = new wxSpinButton(Parent,GetId(),Pos(Parent),Size(Parent),Style());
    if ( Value ) Preview->SetValue(Value);
    if ( Max > Min ) Preview->SetRange(Min,Max);

    return SetupWindow(Preview,Exact);
}


/* ************************************************************************** */
/* Function enumerating all button-specific properties.                       */
/* ************************************************************************** */

void wxsSpinButton::EnumWidgetProperties(long Flags)
{
    WXS_STRING(wxsSpinButton,Value,0,_("Value"),_T("value"),_T(""),true,false)
    WXS_LONG(wxsSpinButton,Min,0,_("Min Value"),_T("min"),0)
    WXS_LONG(wxsSpinButton,Max,0,_("Max Value"),_T("max"),0)
}
