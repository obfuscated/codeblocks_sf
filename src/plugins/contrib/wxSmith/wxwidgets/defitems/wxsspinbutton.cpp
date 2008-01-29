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

#include "wxsspinbutton.h"

#include <wx/spinbutt.h>

namespace
{
    wxsRegisterItem<wxsSpinButton> Reg(_T("SpinButton"),wxsTWidget,_T("Standard"),50);

    WXS_ST_BEGIN(wxsSpinButtonStyles,_T("wxSP_VERTICAL|wxSP_ARROW_KEYS"))
        WXS_ST_CATEGORY("wxsSpinButton")
        WXS_ST(wxSP_HORIZONTAL)
        WXS_ST(wxSP_VERTICAL)
        WXS_ST(wxSP_ARROW_KEYS)
        WXS_ST(wxSP_WRAP)
        WXS_ST_DEFAULTS()
    WXS_ST_END()



    WXS_EV_BEGIN(wxsSpinButtonEvents)
        WXS_EVI(EVT_SPIN,wxEVT_SCROLL_THUMBTRACK,wxSpinEvent,Change)
        WXS_EVI(EVT_SPIN_UP,wxEVT_SCROLL_LINEUP,wxSpinEvent,ChangeUp)
        WXS_EVI(EVT_SPIN_DOWN,wxEVT_SCROLL_LINEDOWN,wxSpinEvent,ChangeDown)
    WXS_EV_END()
}

wxsSpinButton::wxsSpinButton(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsSpinButtonEvents,
        wxsSpinButtonStyles),
    Value(0),
    Min(0),
    Max(100)
{}

void wxsSpinButton::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/spinbutt.h>"),GetInfo().ClassName,0);
            AddHeader(_T("<wx/spinbutt.h>"),_T("wxSpinEvent"),0);
            Codef(_T("%C(%W, %I, %P, %S, %T, %N);\n"));
            if ( Value ) Codef(_T("%ASetValue(%d);\n"), Value);
            if ( Max > Min ) Codef(_T("%ASetRange(%d, %d);\n"), Min, Max);
            BuildSetupWindowCode();
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsSpinButton::OnBuildCreatingCode"),GetLanguage());
        }
    }
}

wxObject* wxsSpinButton::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxSpinButton* Preview = new wxSpinButton(Parent,GetId(),Pos(Parent),Size(Parent),Style());
    if ( Value ) Preview->SetValue(Value);
    if ( Max > Min ) Preview->SetRange(Min,Max);

    return SetupWindow(Preview,Flags);
}

void wxsSpinButton::OnEnumWidgetProperties(long Flags)
{
    WXS_LONG(wxsSpinButton,Value,_("Value"),_T("value"),0)
    WXS_LONG(wxsSpinButton,Min,_("Min Value"),_T("min"),0)
    WXS_LONG(wxsSpinButton,Max,_("Max Value"),_T("max"),0)
}
