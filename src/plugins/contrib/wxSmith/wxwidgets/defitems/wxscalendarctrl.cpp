/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxscalendarctrl.h"

#include <wx/calctrl.h>

namespace
{
    wxsRegisterItem<wxsCalendarCtrl> Reg(_T("CalendarCtrl"),wxsTWidget,_T("Standard"),30);

    WXS_ST_BEGIN(wxsCalendarCtrlStyles,_T(""))
        WXS_ST_CATEGORY("wxCalendarCtrl")
        WXS_ST(wxCAL_SUNDAY_FIRST)
        WXS_ST(wxCAL_MONDAY_FIRST)
        WXS_ST(wxCAL_SHOW_HOLIDAYS)
        WXS_ST(wxCAL_NO_YEAR_CHANGE)
        WXS_ST(wxCAL_NO_MONTH_CHANGE)
        WXS_ST(wxCAL_SHOW_SURROUNDING_WEEKS)
        WXS_ST(wxCAL_SEQUENTIAL_MONTH_SELECTION)
        WXS_ST_DEFAULTS()
    WXS_ST_END()


    WXS_EV_BEGIN(wxsCalendarCtrlEvents)
        WXS_EVI(EVT_CALENDAR_SEL_CHANGED,wxEVT_CALENDAR_SEL_CHANGED,wxCalendarEvent,Changed)
        WXS_EVI(EVT_CALENDAR_DAY,wxEVT_CALENDAR_DAY_CHANGED,wxCalendarEvent,DayChanged)
        WXS_EVI(EVT_CALENDAR_MONTH,wxEVT_CALENDAR_MONTH_CHANGED,wxCalendarEvent,MonthChanged)
        WXS_EVI(EVT_CALENDAR_YEAR,wxEVT_CALENDAR_YEAR_CHANGED,wxCalendarEvent,YearChanged)
        WXS_EVI(EVT_CALENDAR,wxEVT_CALENDAR_DOUBLECLICKED,wxCalendarEvent,DblClicked)
        WXS_EVI(EVT_CALENDAR_WEEKDAY_CLICKED,wxEVT_CALENDAR_WEEKDAY_CLICKED,wxCalendarEvent,WeekdayClicked)
    WXS_EV_END()
}


wxsCalendarCtrl::wxsCalendarCtrl(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsCalendarCtrlEvents,
        wxsCalendarCtrlStyles)
{}

void wxsCalendarCtrl::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/calctrl.h>"),GetInfo().ClassName,0);
            AddHeader(_T("<wx/calctrl.h>"),_T("wxCalendarEvent"),0);
            Codef(_T("%C(%W, %I, wxDefaultDateTime, %P, %S, %T, %N);\n"));
            BuildSetupWindowCode();
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsCalendarCtrl::OnBuildCreatingCode"),GetLanguage());
        }
    }
}

wxObject* wxsCalendarCtrl::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxCalendarCtrl* Preview = new wxCalendarCtrl(Parent,GetId(),wxDefaultDateTime,Pos(Parent),Size(Parent),Style());
    return SetupWindow(Preview,Flags);
}

void wxsCalendarCtrl::OnEnumWidgetProperties(long Flags)
{
}
