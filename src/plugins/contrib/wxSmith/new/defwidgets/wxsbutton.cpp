#include "wxsbutton.h"

#include <wx/button.h>
#include <messagemanager.h>

/* ************************************************************************** */
/* Styles are processed almost like in older code - using WXS_ST_BEGIN,       */
/* WXS_ST_END, WXS_ST_CATEGORY, WXS_ST ...                                    */
/* ************************************************************************** */

WXS_ST_BEGIN(wxsButtonStyles)
    WXS_ST_CATEGORY("wxButton")
    WXS_ST(wxBU_LEFT)
    WXS_ST(wxBU_TOP)
    WXS_ST(wxBU_RIGHT)
    WXS_ST(wxBU_BOTTOM)
    WXS_ST(wxBU_EXACTFIT)
    WXS_ST_MASK(wxNO_BORDER,wxsSFWin| wxsSFGTK ,0,true)
WXS_ST_END()


/* ************************************************************************** */
/* Events have one additional argument - event type, these can be found       */
/* in WX_DIR/include/event.h                                                  */
/* ************************************************************************** */

WXS_EV_BEGIN(wxsButtonEvents)
    WXS_EVI(EVT_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEvent,Click)
    WXS_EV_DEFAULTS()
WXS_EV_END()

/* ************************************************************************** */
/* Widget info                                                                */
/* ************************************************************************** */

wxsItemInfo wxsButton::Info =
{
    _T("wxButton"),             // Name of class
    wxsTWidget,                 // Type, always wxsTWidget for widget classes
    _("wxWidgets license"),     // License, any type
    _("wxWidgets team"),        // Author
    _T(""),                     // No default e-mail for standard widgets
    _T("www.wxwidgets.org"),    // Site
    _T("Standard"),             // Groud for widget, note that _T() instead of _() is used
    80,                         // Button is one of most commonly used widgets - we give it high priority
    _T("Button"),               // Standard previx for variable names and identifiers
    2, 6,                       // Widget version
    NULL,                       // Bitmaps will be loaded later in manager
    NULL,                       // --------------------''-------------------
    0                           // --------------------''-------------------
};


/* ************************************************************************** */
/* Constructur for wxsWidget need style set, event set and widget info, so    */
/* these things should be created before ctor.                                */
/* ************************************************************************** */

wxsButton::wxsButton(wxsWindowRes* Resource):
    wxsWidget(
        Resource,                   // Resource is passed to wxsItem's constructor
        wxsBaseProperties::flAll,   // Using all base properties
        &Info,                      // Taking local info
        wxsButtonEvents,            // Pointer to local events
        wxsButtonStyles,            // Pointer to local styles
        _T("")),                    // Default style = 0
    Label(_("Label")),              // Initializing widget with some default (but not necessarily clean) values,
    IsDefault(false)                // these values will be used when creating new widgets
{}


/* ************************************************************************** */
/* Function building code - it should append new code generating this widget  */
/* to the end of prevous code. Currently it support only C++ langage but some */
/* other languages may be added in future.                                    */
/* ************************************************************************** */

void wxsButton::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
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
            Code<< GetVarName() << _T(" = new wxButton(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << wxsGetWxString(Label) << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(");\n");
            if ( IsDefault ) Code << GetVarName() << _T("->SetDefault();\n");
            SetupWindowCode(Code,Language);
            return;
        }
    }

    wxsLANGMSG(wxsButton::BuildCreatingCode,Language);
}

/* ************************************************************************** */
/* Function building preview item. This should simply return previewed object */
/* with all properties set-up.                                                */
/* ************************************************************************** */

wxObject* wxsButton::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    // Exact argument is not used here because button preview is
    // exactly the same in editor and in preview window
    wxButton* Preview = new wxButton(Parent,GetId(),Label,Pos(Parent),Size(Parent),Style());
    if ( IsDefault ) Preview->SetDefault();
    return SetupWindow(Preview,Exact);
}


/* ************************************************************************** */
/* Function enumerating all button-specific properties.                       */
/* ************************************************************************** */

void wxsButton::EnumWidgetProperties(long Flags)
{
    WXS_STRING(wxsButton,Label,0,_("Label"),_T("label"),_T(""),true,false)
    WXS_BOOL  (wxsButton,IsDefault,0,_("Is default"),_("default"),false)
}

/* ************************************************************************** */
/* Function enumerating all required declaration files (header files in case  */
/* of c++)                                                                    */
/* ************************************************************************** */
void wxsButton::EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/button.h>")); return;
    }

    wxsLANGMSG(wxsButton::EnumDeclFiles,Language);
}
