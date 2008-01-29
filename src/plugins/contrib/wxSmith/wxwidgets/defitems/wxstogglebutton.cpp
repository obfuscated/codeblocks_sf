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

#include "wxstogglebutton.h"

#include <wx/tglbtn.h>

namespace
{
    wxsRegisterItem<wxsToggleButton> Reg(_T("ToggleButton"),wxsTWidget,_T("Standard"),50);

    WXS_ST_BEGIN(wxsToggleButtonStyles,_T(""))
        WXS_ST_CATEGORY("wxToggleButton")
        WXS_ST_DEFAULTS()
    WXS_ST_END()


    WXS_EV_BEGIN(wxsToggleButtonEvents)
        WXS_EVI(EVT_TOGGLEBUTTON,wxEVT_COMMAND_TOGGLEBUTTON_CLICKED,wxCommandEvent,Toggle)
    WXS_EV_END()
}

wxsToggleButton::wxsToggleButton(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsToggleButtonEvents,
        wxsToggleButtonStyles),
   Label(_("Label")),
   IsChecked(false)
{}


void wxsToggleButton::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/tglbtn.h>"),GetInfo().ClassName,0);
            Codef(_T("%C(%W, %I, %t, %P, %S, %T, %V, %N);\n"),Label.c_str());
            if ( IsChecked ) Codef(_T("%ASetValue(%b);\n"),true);
            BuildSetupWindowCode();
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsToggleButton::OnBuildCreatingCode"),GetLanguage());
        }
    }
}

wxObject* wxsToggleButton::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxToggleButton* Preview = new wxToggleButton(Parent,GetId(),Label,Pos(Parent),Size(Parent),Style());
    Preview->SetValue(IsChecked);
    return SetupWindow(Preview,Flags);
}

void wxsToggleButton::OnEnumWidgetProperties(long Flags)
{
    WXS_STRING(wxsToggleButton,Label,_("Label"),_T("label"),_T(""),false)
    WXS_BOOL(wxsToggleButton,IsChecked,_("Is checked"),_T("checked"),false)
}
