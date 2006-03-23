#include "wxsspinctrl.h"

#include <wx/spinctrl.h>
#include <messagemanager.h>

/* ************************************************************************** */
/* Styles are processed almost like in older code - using WXS_ST_BEGIN,       */
/* WXS_ST_END, WXS_ST_CATEGORY, WXS_ST ...                                    */
/* ************************************************************************** */

WXS_ST_BEGIN(wxsSpinCtrlStyles)
    WXS_ST_CATEGORY("wxSpinCtrl")

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

WXS_EV_BEGIN(wxsSpinCtrlEvents)
    WXS_EVI(EVT_SPINCTRL,wxEVT_COMMAND_SPINCTRL_UPDATED,wxSpinEvent,Change)
    WXS_EV_DEFAULTS()
WXS_EV_END()

/* ************************************************************************** */
/* Widget info                                                                */
/* ************************************************************************** */

wxsItemInfo wxsSpinCtrl::Info =
{
    _T("wxSpinCtrl"),           // Name of class
    wxsTWidget,                 // Type, always wxsTWidget for widget classes
    _("wxWidgets license"),     // License, any type
    _("wxWidgets team"),        // Author
    _T(""),                     // No default e-mail for standard widgets
    _T("www.wxwidgets.org"),    // Site
    _T("Standard"),             // Groud for widget, note that _T() instead of _() is used
    80,                         // Button is one of most commonly used widgets - we give it high priority
    _T("SpinCtrl"),             // Standard prefix for variable names and identifiers
    2, 6,                       // Widget version
    NULL,                       // Bitmaps will be loaded later in manager
    NULL,                       // --------------------''-------------------
    0                           // --------------------''-------------------
};


/* ************************************************************************** */
/* Constructur for wxsWidget need style set, event set and widget info, so    */
/* these things should be created before ctor.                                */
/* ************************************************************************** */

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


/* ************************************************************************** */
/* Function building code - it should append new code generating this widget  */
/* to the end of prevous code. Currently it support only C++ langage but some */
/* other languages may be added in future.                                    */
/* ************************************************************************** */

void wxsSpinCtrl::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
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

    DBGLOG(_T("wxSmith: Unknown coding language when generating spin control (id: %d)"),Language);
}

/* ************************************************************************** */
/* Function building preview item. This should simply return previewed object */
/* with all properties set-up.                                                */
/* ************************************************************************** */

wxObject* wxsSpinCtrl::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    // Exact argument is not used here because button preview is
    // exactly the same in editor and in preview window
    wxSpinCtrl* Preview = new wxSpinCtrl(Parent,GetId(),Value,Pos(Parent),Size(Parent),Style(),Min,Max);
    if ( !Value.empty() ) Preview->SetValue(Value);
    return SetupWindow(Preview,Exact);
}


/* ************************************************************************** */
/* Function enumerating all spin ctrl-specific properties.                       */
/* ************************************************************************** */

void wxsSpinCtrl::EnumWidgetProperties(long Flags)
{
    WXS_STRING(wxsSpinCtrl,Value,0,_("Value"),_T("value"),_T(""),true,false)
    WXS_LONG(wxsSpinCtrl,Min,0,_("Min"),_T("Min"),0)
    WXS_LONG(wxsSpinCtrl,Max,0,_("Max"),_T("Max"),0)
}
