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

#include <wx/gauge.h>
#include "wxsgauge.h"

namespace
{
    wxsRegisterItem<wxsGauge> Reg(_T("Gauge"),wxsTWidget,_T("Standard"),270);

    WXS_ST_BEGIN(wxsGaugeStyles,_T(""))
        WXS_ST(wxGA_HORIZONTAL)
        WXS_ST(wxGA_VERTICAL)
        WXS_ST(wxGA_SMOOTH)
        WXS_ST_DEFAULTS()
    WXS_ST_END()


    WXS_EV_BEGIN(wxsGaugeEvents)
    WXS_EV_END()
}

wxsGauge::wxsGauge(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsGaugeEvents,
        wxsGaugeStyles),
    Range(100),
    Value(0)
{}



void wxsGauge::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/gauge.h>"),GetInfo().ClassName,hfInPCH);
            Codef(_T("%C(%W, %I, %d, %P, %S, %T, %V, %N);\n"),Range);
            if ( Value )  Codef(_T("%ASetValue(%d);\n"),Value);
            BuildSetupWindowCode();
            return;
        }

        case wxsUnknownLanguage: // fall-through
        default:
        {
            wxsCodeMarks::Unknown(_T("wxsGauge::OnBuildCreatingCode"),GetLanguage());
        }
    }
}


wxObject* wxsGauge::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxGauge* Preview = new wxGauge(Parent,GetId(),Range,Pos(Parent),Size(Parent),Style());
    if ( Value )  Preview->SetValue(Value);
    return SetupWindow(Preview,Flags);
}


void wxsGauge::OnEnumWidgetProperties(cb_unused long Flags)
{
    WXS_LONG(wxsGauge,Value,_("Value"),_T("value"),0)
    WXS_LONG(wxsGauge,Range,_("Range"),_T("range"),100)
}
