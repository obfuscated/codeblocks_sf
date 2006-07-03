#include "../wxsheaders.h"
#include "wxscalendarctrl.h"

#include <wx/calctrl.h>

WXS_ST_BEGIN(wxsCalendarCtrlStyles)
    WXS_ST_CATEGORY("wxCalendarCtrl")
    WXS_ST(wxCAL_SUNDAY_FIRST)
    WXS_ST(wxCAL_MONDAY_FIRST)
    WXS_ST(wxCAL_SHOW_HOLIDAYS)
    WXS_ST(wxCAL_NO_YEAR_CHANGE)
    WXS_ST(wxCAL_NO_MONTH_CHANGE)
    WXS_ST(wxCAL_SHOW_SURROUNDING_WEEKS)
    WXS_ST(wxCAL_SEQUENTIAL_MONTH_SELECTION)
WXS_ST_END(wxsCalendarCtrlStyles)

WXS_EV_BEGIN(wxsCalendarCtrlEvents)
    WXS_EVI(EVT_CALENDAR,wxCalendarEvent,DblClicked)
    WXS_EVI(EVT_CALENDAR_SEL_CHANGED,wxCalendarEvent,Changed)
    WXS_EVI(EVT_CALENDAR_DAY,wxCalendarEvent,DayChanged)
    WXS_EVI(EVT_CALENDAR_MONTH,wxCalendarEvent,MonthChanged)
    WXS_EVI(EVT_CALENDAR_YEAR,wxCalendarEvent,YearChanged)
    WXS_EVI(EVT_CALENDAR_WEEKDAY_CLICKED,wxCalendarEvent,WeekdayClicked)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsCalendarCtrlEvents)

wxsDWDefineBegin(wxsCalendarCtrl,wxCalendarCtrl,
    WXS_THIS = new wxCalendarCtrl(WXS_PARENT,WXS_ID,wxDefaultDateTime,WXS_POS,WXS_SIZE,WXS_STYLE,WXS_NAME);
    )
wxsDWDefineEnd()
