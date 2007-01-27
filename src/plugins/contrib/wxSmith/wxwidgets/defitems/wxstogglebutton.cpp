/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006  Bartlomiej Swiecki
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
        WXS_EV_DEFAULTS()

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


void wxsToggleButton::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            if ( !IsRootItem() )
            {
                Code<< GetVarName() << _T(" = new wxToggleButton(");
            }
            else
            {
                Code<< _T("Create(");
            }
            Code<< WindowParent << _T(",")
                << GetIdName() << _T(",")
                << wxsCodeMarks::WxString(wxsCPP,Label) << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(",")
                << _T("wxDefaultValidator") << _T(",")
                << wxsCodeMarks::WxString(wxsCPP,GetIdName(),false) << _T(");\n");

            if ( IsChecked ) Code << GetVarName() << _T("->SetValue(true);\n");

            SetupWindowCode(Code,Language);
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsToggleButton::OnBuildCreatingCode"),Language);
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
    WXS_STRING(wxsToggleButton,Label,0,_("Label"),_T("label"),_T(""),true,false)
    WXS_BOOL  (wxsToggleButton,IsChecked,0,_("Is checked"),_T("checked"),false)
}

void wxsToggleButton::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/tglbtn.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsToggleButton::OnEnumDeclFiles"),Language);
    }
}
