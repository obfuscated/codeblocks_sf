/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxstimer.h"
#include "../wxsitemresdata.h"
#include <wx/timer.h>

namespace
{
    wxsRegisterItem<wxsTimer> Reg(_T("Timer"),wxsTTool,_T("Tools"),80,false);

    WXS_EV_BEGIN(wxsTimerEvents)
        WXS_EVI(EVT_TIMER,wxEVT_TIMER,wxTimerEvent,Trigger)
    WXS_EV_END()

}

wxsTimer::wxsTimer(wxsItemResData* Data):
    wxsTool(
        Data,
        &Reg.Info,
        wxsTimerEvents,
        0,
        flVariable|flId|flSubclass|flExtraCode)
{
    m_Interval = 0;
    m_OneShoot = false;
}

void wxsTimer::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/timer.h>"),GetInfo().ClassName,hfInPCH);
            Codef(_T("%ASetOwner(this, %I);\n"));
            if ( m_Interval > 0 ) Codef(_T("%AStart(%d, %b);\n"),m_Interval,m_OneShoot);
            BuildSetupWindowCode();
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsTimer::OnBuildCreatingCode"),GetLanguage());
        }
    }
}

void wxsTimer::OnEnumToolProperties(long Flags)
{
    WXS_LONG(wxsTimer,m_Interval,_("Interval"),_T("interval"),0);
    WXS_BOOL(wxsTimer,m_OneShoot,_("One Shoot"),_T("oneshoot"),false);
}

bool wxsTimer::OnCanAddToResource(wxsItemResData* Data,bool ShowMessage)
{
    switch ( Data->GetPropertiesFilter() & (flSource|flMixed|flFile) )
    {
        case flSource:
            return true;

        case flMixed:
            if ( ShowMessage )
            {
                cbMessageBox(_("Can not add timer when using XRC file"),_("wxTimer error"));
            }
            return false;

        case flFile:
            if ( ShowMessage )
            {
                cbMessageBox(_("wxTimer is not supported in XRC"),_("wxTimer error"));
            }
            return false;
    }

    return false;
}
