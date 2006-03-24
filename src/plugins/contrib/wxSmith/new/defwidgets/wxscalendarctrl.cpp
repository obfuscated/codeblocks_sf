#include "wxscalendarctrl.h"

#include <wx/calctrl.h>
#include <messagemanager.h>

/* ************************************************************************** */
/* Styles are processed almost like in older code - using WXS_ST_BEGIN,       */
/* WXS_ST_END, WXS_ST_CATEGORY, WXS_ST ...                                    */
/* ************************************************************************** */

WXS_ST_BEGIN(wxsCalendarCtrlStyles)
    WXS_ST_CATEGORY("wxCalendarCtrl")
    WXS_ST(wxCAL_SUNDAY_FIRST)
    WXS_ST(wxCAL_MONDAY_FIRST)
    WXS_ST(wxCAL_SHOW_HOLIDAYS)
    WXS_ST(wxCAL_NO_YEAR_CHANGE)
    WXS_ST(wxCAL_NO_MONTH_CHANGE)
    WXS_ST(wxCAL_SHOW_SURROUNDING_WEEKS)
    WXS_ST(wxCAL_SEQUENTIAL_MONTH_SELECTION)
WXS_ST_END()


/* ************************************************************************** */
/* Events have one additional argument - event type, these can be found       */
/* in WX_DIR/include/event.h                                                  */
/* ************************************************************************** */

WXS_EV_BEGIN(wxsCalendarCtrlEvents)
    WXS_EVI(EVT_CALENDAR_SEL_CHANGED,wxEVT_CALENDAR_SEL_CHANGED,wxCalendarEvent,Changed)
    WXS_EVI(EVT_CALENDAR_DAY,wxEVT_CALENDAR_DAY_CHANGED,wxCalendarEvent,DayChanged)
    WXS_EVI(EVT_CALENDAR_MONTH,wxEVT_CALENDAR_MONTH_CHANGED,wxCalendarEvent,MonthChanged)
    WXS_EVI(EVT_CALENDAR_YEAR,wxEVT_CALENDAR_YEAR_CHANGED,wxCalendarEvent,YearChanged)
    WXS_EVI(EVT_CALENDAR,wxCalendarEvent,wxEVT_CALENDAR_DOUBLECLICKED,DblClicked)
    WXS_EVI(EVT_CALENDAR_WEEKDAY_CLICKED,wxEVT_CALENDAR_WEEKDAY_CLICKED,wxCalendarEvent,WeekdayClicked)
    WXS_EV_DEFAULTS()
WXS_EV_END()

/* ************************************************************************** */
/* Widget info                                                                */
/* ************************************************************************** */

wxsItemInfo wxsCalendarCtrl::Info =
{
    _T("wxCalendarCtrl"),       // Name of class
    wxsTWidget,                 // Type, always wxsTWidget for widget classes
    _("wxWidgets license"),     // License, any type
    _("wxWidgets team"),        // Author
    _T(""),                     // No default e-mail for standard widgets
    _T("www.wxwidgets.org"),    // Site
    _T("Standard"),             // Groud for widget, note that _T() instead of _() is used
    30,                         // Button is one of most commonly used widgets - we give it high priority
    _T("CalendarCtrl"),         // Standard prefix for variable names and identifiers
    2, 6,                       // Widget version
    NULL,                       // Bitmaps will be loaded later in manager
    NULL,                       // --------------------''-------------------
    0                           // --------------------''-------------------
};


/* ************************************************************************** */
/* Constructur for wxsWidget need style set, event set and widget info, so    */
/* these things should be created before ctor.                                */
/* ************************************************************************** */

wxsCalendarCtrl::wxsCalendarCtrl(wxsWindowRes* Resource):
    wxsWidget(
        Resource,                   // Resource is passed to wxsItem's constructor
        wxsBaseProperties::flAll,   // Using all base properties
        &Info,                      // Taking local info
        wxsCalendarCtrlEvents,      // Pointer to local events
        wxsCalendarCtrlStyles,      // Pointer to local styles
        _T(""))                     // Default style = 0

{}


/* ************************************************************************** */
/* Function building code - it should append new code generating this widget  */
/* to the end of prevous code. Currently it support only C++ langage but some */
/* other languages may be added in future.                                    */
/* ************************************************************************** */

void wxsCalendarCtrl::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
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
            Code<< GetVarName() << _T(" = new wxCalendarCtrl(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << wxsGetWxString(_T("wxDefaultDateTime")) << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(");\n");

            SetupWindowCode(Code,Language);
            return;
        }
    }

    DBGLOG(_T("wxSmith: Unknown coding language when generating calendar control (id: %d)"),Language);
}

/* ************************************************************************** */
/* Function building preview item. This should simply return previewed object */
/* with all properties set-up.                                                */
/* ************************************************************************** */

wxObject* wxsCalendarCtrl::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    // Exact argument is not used here because calendar preview is
    // exactly the same in editor and in preview window
    wxCalendarCtrl* Preview = new wxCalendarCtrl(Parent,GetId(),wxDefaultDateTime,Pos(Parent),Size(Parent),Style());

    return SetupWindow(Preview,Exact);
}


/* ************************************************************************** */
/* Function enumerating all calendar control-specific properties.                       */
/* ************************************************************************** */

void wxsCalendarCtrl::EnumWidgetProperties(long Flags)
{
  //  WXS_STRING(wxsCalendarCtrl,Label,0,_("Label"),_T("label"),_T(""),true,false)
  //  WXS_BOOL  (wxsCalendarCtrl,IsDefault,0,_("Is default"),_("default"),false)
}
