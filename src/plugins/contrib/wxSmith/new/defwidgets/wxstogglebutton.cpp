#include "wxstogglebutton.h"

#include <wx/tglbtn.h>
#include <messagemanager.h>

/* ************************************************************************** */
/* Styles are processed almost like in older code - using WXS_ST_BEGIN,       */
/* WXS_ST_END, WXS_ST_CATEGORY, WXS_ST ...                                    */
/* ************************************************************************** */

WXS_ST_BEGIN(wxsToggleButtonStyles)
    WXS_ST_CATEGORY("wxToggleButton")
    WXS_ST_DEFAULTS()
WXS_ST_END()


/* ************************************************************************** */
/* Events have one additional argument - event type, these can be found       */
/* in WX_DIR/include/event.h                                                  */
/* ************************************************************************** */

WXS_EV_BEGIN(wxsToggleButtonEvents)
    WXS_EVI(EVT_TOGGLEBUTTON,wxEVT_COMMAND_TOGGLEBUTTON_CLICKED,wxCommandEvent,Toggle)
    WXS_EV_DEFAULTS()
WXS_EV_END()

/* ************************************************************************** */
/* Widget info                                                                */
/* ************************************************************************** */

wxsItemInfo wxsToggleButton::Info =
{
    _T("wxToggleButton"),       // Name of class
    wxsTWidget,                 // Type, always wxsTWidget for widget classes
    _("wxWidgets license"),     // License, any type
    _("wxWidgets team"),        // Author
    _T(""),                     // No default e-mail for standard widgets
    _T("www.wxwidgets.org"),    // Site
    _T("Standard"),             // Groud for widget, note that _T() instead of _() is used
    80,                         // Button is one of most commonly used widgets - we give it high priority
    _T("ToggleButton"),         // Standard prefix for variable names and identifiers
    2, 6,                       // Widget version
    NULL,                       // Bitmaps will be loaded later in manager
    NULL,                       // --------------------''-------------------
    0                           // --------------------''-------------------
};


/* ************************************************************************** */
/* Constructur for wxsWidget need style set, event set and widget info, so    */
/* these things should be created before ctor.                                */
/* ************************************************************************** */

wxsToggleButton::wxsToggleButton(wxsWindowRes* Resource):
    wxsWidget(
        Resource,                   // Resource is passed to wxsItem's constructor
        wxsBaseProperties::flAll,   // Using all base properties
        &Info,                      // Taking local info
        wxsToggleButtonEvents,      // Pointer to local events
        wxsToggleButtonStyles,      // Pointer to local styles
        _T("")),                     // Default style = 0
       Label(_("Label")),           // Initializing widget with some default (but not necessarily clean) values,
       IsChecked(false)             // these values will be used when creating new widgets
{}


/* ************************************************************************** */
/* Function building code - it should append new code generating this widget  */
/* to the end of prevous code. Currently it support only C++ langage but some */
/* other languages may be added in future.                                    */
/* ************************************************************************** */

void wxsToggleButton::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
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

    DBGLOG(_T("wxSmith: Unknown coding language when generating toggle button (id: %d)"),Language);
}

/* ************************************************************************** */
/* Function building preview item. This should simply return previewed object */
/* with all properties set-up.                                                */
/* ************************************************************************** */

wxObject* wxsToggleButton::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    // Exact argument is not used here because toggle button preview is
    // exactly the same in editor and in preview window
    wxToggleButton* Preview = new wxToggleButton(Parent,GetId(),Label,Pos(Parent),Size(Parent),Style());

    Preview->SetValue(IsChecked);

    return SetupWindow(Preview,Exact);
}


/* ************************************************************************** */
/* Function enumerating all toggle button-specific properties.                       */
/* ************************************************************************** */

void wxsToggleButton::EnumWidgetProperties(long Flags)
{
    WXS_STRING(wxsToggleButton,Label,0,_("Label"),_T("label"),_T(""),true,false)
    WXS_BOOL  (wxsToggleButton,IsChecked,0,_("Is checked"),_("checked"),false)
}
