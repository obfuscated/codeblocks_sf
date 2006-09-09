#include "wxscalendarctrl.h"

#include <wx/calctrl.h>
#include <messagemanager.h>


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


WXS_EV_BEGIN(wxsCalendarCtrlEvents)
    WXS_EVI(EVT_CALENDAR_SEL_CHANGED,wxEVT_CALENDAR_SEL_CHANGED,wxCalendarEvent,Changed)
    WXS_EVI(EVT_CALENDAR_DAY,wxEVT_CALENDAR_DAY_CHANGED,wxCalendarEvent,DayChanged)
    WXS_EVI(EVT_CALENDAR_MONTH,wxEVT_CALENDAR_MONTH_CHANGED,wxCalendarEvent,MonthChanged)
    WXS_EVI(EVT_CALENDAR_YEAR,wxEVT_CALENDAR_YEAR_CHANGED,wxCalendarEvent,YearChanged)
    WXS_EVI(EVT_CALENDAR,wxCalendarEvent,wxEVT_CALENDAR_DOUBLECLICKED,DblClicked)
    WXS_EVI(EVT_CALENDAR_WEEKDAY_CLICKED,wxEVT_CALENDAR_WEEKDAY_CLICKED,wxCalendarEvent,WeekdayClicked)
    WXS_EV_DEFAULTS()
WXS_EV_END()

wxsItemInfo wxsCalendarCtrl::Info =
{
    _T("wxCalendarCtrl"),
    wxsTWidget,
    _("wxWidgets license"),
    _("wxWidgets team"),
    _T(""),
    _T("www.wxwidgets.org"),
    _T("Standard"),
    30,
    _T("CalendarCtrl"),
    2, 6,
    NULL,
    NULL,
    0
};


wxsCalendarCtrl::wxsCalendarCtrl(wxsWindowRes* Resource):
    wxsWidget(
        Resource,
        wxsBaseProperties::flAll,
        &Info,
        wxsCalendarCtrlEvents,
        wxsCalendarCtrlStyles,
        _T(""))

{}


void wxsCalendarCtrl::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code<< GetVarName() << _T(" = new wxCalendarCtrl(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << _T("wxDefaultDateTime") << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(");\n");

            SetupWindowCode(Code,Language);
            return;
        }
    }

    wxsLANGMSG(wxsCalendarCtrl::BuildCreatingCode,Language);
}

wxObject* wxsCalendarCtrl::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    wxCalendarCtrl* Preview = new wxCalendarCtrl(Parent,GetId(),wxDefaultDateTime,Pos(Parent),Size(Parent),Style());

    return SetupWindow(Preview,Exact);
}


void wxsCalendarCtrl::EnumWidgetProperties(long Flags)
{

}

void wxsCalendarCtrl::EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/calctrl.h>")); return;
    }

    wxsLANGMSG(wxsCalendarCtrl::EnumDeclFiles,Language);
}
