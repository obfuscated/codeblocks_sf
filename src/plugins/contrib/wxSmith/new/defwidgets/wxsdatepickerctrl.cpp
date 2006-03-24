#include "wxsdatepickerctrl.h"

#include <wx/datectrl.h>
#include <messagemanager.h>

/* ************************************************************************** */
/* Styles are processed almost like in older code - using WXS_ST_BEGIN,       */
/* WXS_ST_END, WXS_ST_CATEGORY, WXS_ST ...                                    */
/* ************************************************************************** */

WXS_ST_BEGIN(wxsDatePickerCtrlStyles)
    WXS_ST_CATEGORY("wxDatePickerCtrl")
    WXS_ST(wxDP_DEFAULT)
    WXS_ST(wxDP_SPIN)
    WXS_ST(wxDP_DROPDOWN)
    WXS_ST(wxDP_ALLOWNONE)
    WXS_ST(wxDP_SHOWCENTURY)
WXS_ST_END()


/* ************************************************************************** */
/* Events have one additional argument - event type, these can be found       */
/* in WX_DIR/include/event.h                                                  */
/* ************************************************************************** */

WXS_EV_BEGIN(wxsDatePickerCtrlEvents)
    WXS_EVI(EVT_DATE_CHANGED,wxEVT_DATE_CHANGED,wxDataEvent,Changed)
    WXS_EV_DEFAULTS()
WXS_EV_END()

/* ************************************************************************** */
/* Widget info                                                                */
/* ************************************************************************** */

wxsItemInfo wxsDatePickerCtrl::Info =
{
    _T("wxDatePickerCtrl"),             // Name of class
    wxsTWidget,                 // Type, always wxsTWidget for widget classes
    _("wxWidgets license"),     // License, any type
    _("wxWidgets team"),        // Author
    _T(""),                     // No default e-mail for standard widgets
    _T("www.wxwidgets.org"),    // Site
    _T("Standard"),             // Groud for widget, note that _T() instead of _() is used
    30,                         // Button is one of most commonly used widgets - we give it high priority
    _T("DatePickerCtrl"),               // Standard prefix for variable names and identifiers
    2, 6,                       // Widget version
    NULL,                       // Bitmaps will be loaded later in manager
    NULL,                       // --------------------''-------------------
    0                           // --------------------''-------------------
};


/* ************************************************************************** */
/* Constructur for wxsWidget need style set, event set and widget info, so    */
/* these things should be created before ctor.                                */
/* ************************************************************************** */

wxsDatePickerCtrl::wxsDatePickerCtrl(wxsWindowRes* Resource):
    wxsWidget(
        Resource,                   // Resource is passed to wxsItem's constructor
        wxsBaseProperties::flAll,   // Using all base properties
        &Info,                      // Taking local info
        wxsDatePickerCtrlEvents,    // Pointer to local events
        wxsDatePickerCtrlStyles,    // Pointer to local styles
        _T(""))                      // Default style = 0

{}


/* ************************************************************************** */
/* Function building code - it should append new code generating this widget  */
/* to the end of prevous code. Currently it support only C++ langage but some */
/* other languages may be added in future.                                    */
/* ************************************************************************** */

void wxsDatePickerCtrl::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
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
            Code<< GetVarName() << _T(" = new wxDatePickerCtrl(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << _T("wxDefaultDateTime,")    // TODO find a way to get the wxDateTime in code
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(");\n");

            SetupWindowCode(Code,Language);
            return;
        }
    }

    DBGLOG(_T("wxSmith: Unknown coding language when generating date picker control (id: %d)"),Language);
}

/* ************************************************************************** */
/* Function building preview item. This should simply return previewed object */
/* with all properties set-up.                                                */
/* ************************************************************************** */

wxObject* wxsDatePickerCtrl::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    // Exact argument is not used here because date picker control preview is
    // exactly the same in editor and in preview window
    wxDatePickerCtrl* Preview = new wxDatePickerCtrl(Parent,GetId(),wxDefaultDateTime,Pos(Parent),Size(Parent),Style());

    return SetupWindow(Preview,Exact);
}


/* ************************************************************************** */
/* Function enumerating all date picker control - specific properties.                       */
/* ************************************************************************** */

void wxsDatePickerCtrl::EnumWidgetProperties(long Flags)
{
 //  TODO : find
 //   WXS_DATETIME(wxsDatePickerCtrl,DefaultDateTime,0,_("Default"),_T("default"),_T(""),true,false)

}
