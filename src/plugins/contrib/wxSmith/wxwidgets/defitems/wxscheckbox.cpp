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

#include <wx/checkbox.h>
#include "wxscheckbox.h"

namespace
{
    wxsRegisterItem<wxsCheckBox> Reg(_T("CheckBox"),wxsTWidget,_T("Standard"),330);

    WXS_ST_BEGIN(wxsCheckBoxStyles,_T(""))
        WXS_ST_CATEGORY("wxCheckBox")
        WXS_ST(wxCHK_2STATE)
        WXS_ST(wxCHK_3STATE)
        WXS_ST(wxCHK_ALLOW_3RD_STATE_FOR_USER)
        WXS_ST(wxALIGN_RIGHT)
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsCheckBoxEvents)
        WXS_EVI(EVT_CHECKBOX,wxEVT_COMMAND_CHECKBOX_CLICKED,wxCommandEvent,Click)
    WXS_EV_END()
}

wxsCheckBox::wxsCheckBox(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsCheckBoxEvents,
        wxsCheckBoxStyles),
    Label(_("Label")),
    IsChecked(false)
{}


void wxsCheckBox::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/checkbox.h>"),GetInfo().ClassName,hfInPCH);
            Codef(_T("%C(%W, %I, %t, %P, %S, %T, %V, %N);\n"),Label.wx_str());
            Codef(_T("%ASetValue(%b);\n"),IsChecked);
            BuildSetupWindowCode();
            return;
        }

        case wxsUnknownLanguage: // fall through
        default:
        {
            wxsCodeMarks::Unknown(_T("wxsCheckBox::OnBuildCreatingCode"),GetLanguage());
        }
    }
}


wxObject* wxsCheckBox::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxCheckBox* Preview = new wxCheckBox(Parent,GetId(),Label,Pos(Parent),Size(Parent),Style());
    if ( IsChecked ) Preview->SetValue(IsChecked);
    return SetupWindow(Preview,Flags);
}

void wxsCheckBox::OnEnumWidgetProperties(cb_unused long Flags)
{
    WXS_SHORT_STRING(wxsCheckBox,Label,_("Label"),_T("label"),_T(""),false)
    WXS_BOOL(wxsCheckBox,IsChecked,_("Checked"),_T("checked"),false)
}
