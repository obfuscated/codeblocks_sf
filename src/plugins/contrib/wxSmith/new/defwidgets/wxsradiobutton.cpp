#include "wxsradiobutton.h"

#include <wx/radiobut.h>
#include <messagemanager.h>

/* ************************************************************************** */
/* Styles are processed almost like in older code - using WXS_ST_BEGIN,       */
/* WXS_ST_END, WXS_ST_CATEGORY, WXS_ST ...                                    */
/* ************************************************************************** */

WXS_ST_BEGIN(wxsRadioButtonStyles)
    WXS_ST_CATEGORY("wxRadioButton")
    WXS_ST(wxRB_GROUP)
    WXS_ST(wxRB_SINGLE)
// NOTE (cyberkoa#1#): wxRB_USE_CHECKBOX is only available under PALMOS, not sure whether in XRC PALMOS port or not
    WXS_ST_MASK(wxRB_USE_CHECKBOX,wxsSFPALMOS,0,true)
WXS_ST_END()


/* ************************************************************************** */
/* Events have one additional argument - event type, these can be found       */
/* in WX_DIR/include/event.h                                                  */
/* ************************************************************************** */

WXS_EV_BEGIN(wxsRadioButtonEvents)
    WXS_EVI(EVT_RADIOBUTTON,wxEVT_COMMAND_RADIOBUTTON_SELECTED,wxCommandEvent,Select)
    WXS_EV_DEFAULTS()
WXS_EV_END()

/* ************************************************************************** */
/* Widget info                                                                */
/* ************************************************************************** */

wxsItemInfo wxsRadioButton::Info =
{
    _T("wxRadioButton"),        // Name of class
    wxsTWidget,                 // Type, always wxsTWidget for widget classes
    _("wxWidgets license"),     // License, any type
    _("wxWidgets team"),        // Author
    _T(""),                     // No default e-mail for standard widgets
    _T("www.wxwidgets.org"),    // Site
    _T("Standard"),             // Groud for widget, note that _T() instead of _() is used
    60,                         // Button is one of most commonly used widgets - we give it high priority
    _T("RadioButton"),          // Standard prefix for variable names and identifiers
    2, 6,                       // Widget version
    NULL,                       // Bitmaps will be loaded later in manager
    NULL,                       // --------------------''-------------------
    0                           // --------------------''-------------------
};


/* ************************************************************************** */
/* Constructur for wxsWidget need style set, event set and widget info, so    */
/* these things should be created before ctor.                                */
/* ************************************************************************** */

wxsRadioButton::wxsRadioButton(wxsWindowRes* Resource):
    wxsWidget(
        Resource,                   // Resource is passed to wxsItem's constructor
        wxsBaseProperties::flAll,   // Using all base properties
        &Info,                      // Taking local info
        wxsRadioButtonEvents,       // Pointer to local events
        wxsRadioButtonStyles,       // Pointer to local styles
        _T("")),                     // Default style = 0
    Label(_("Label")),              // Initializing widget with some default (but not necessarily clean) values,
    IsSelected(false)               // these values will be used when creating new widgets
{}


/* ************************************************************************** */
/* Function building code - it should append new code generating this widget  */
/* to the end of prevous code. Currently it support only C++ langage but some */
/* other languages may be added in future.                                    */
/* ************************************************************************** */

void wxsRadioButton::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
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

    DBGLOG(_T("wxSmith: Unknown coding language when generating radio button (id: %d)"),Language);
}

/* ************************************************************************** */
/* Function building preview item. This should simply return previewed object */
/* with all properties set-up.                                                */
/* ************************************************************************** */

wxObject* wxsRadioButton::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    // Exact argument is not used here because button preview is
    // exactly the same in editor and in preview window
    wxRadioButton* Preview = new wxRadioButton(Parent,GetId(),Label,Pos(Parent),Size(Parent),Style());
    Preview->SetValue(IsSelected);

    return SetupWindow(Preview,Exact);
}


/* ************************************************************************** */
/* Function enumerating all radio button-specific properties.                       */
/* ************************************************************************** */

void wxsRadioButton::EnumWidgetProperties(long Flags)
{
    WXS_STRING(wxsRadioButton,Label,0,_("Label"),_T("label"),_T(""),true,false)
    WXS_BOOL  (wxsRadioButton,IsSelected,0,_("Is Selected"),_("selected"),false)
}
