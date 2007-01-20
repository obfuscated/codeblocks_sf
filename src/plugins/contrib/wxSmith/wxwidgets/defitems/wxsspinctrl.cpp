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

#include "wxsspinctrl.h"

#include <wx/spinctrl.h>

namespace
{
    wxsRegisterItem<wxsSpinCtrl> Reg(_T("SpinCtrl"),wxsTWidget,_T("Standard"),50);

    WXS_ST_BEGIN(wxsSpinCtrlStyles,_T(""))
        WXS_ST_CATEGORY("wxSpinCtrl")

    // NOTE (cyberkoa##): wxSP_HORIZONTAL, wxSP_VERTICAL are not found in HELP but in wxMSW's XRC. Assume same as spinbutton
        WXS_ST_MASK(wxSP_HORIZONTAL,wxsSFAll,wxsSFGTK,true)
        WXS_ST(wxSP_VERTICAL)

        WXS_ST(wxSP_ARROW_KEYS)
        WXS_ST(wxSP_WRAP)
    WXS_ST_END()


    WXS_EV_BEGIN(wxsSpinCtrlEvents)
        WXS_EVI(EVT_SPINCTRL,wxEVT_COMMAND_SPINCTRL_UPDATED,wxSpinEvent,Change)
        WXS_EV_DEFAULTS()
    WXS_EV_END()
}

wxsSpinCtrl::wxsSpinCtrl(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsSpinCtrlEvents,
        wxsSpinCtrlStyles),
    Value(_T("0")),
    Min(0),
    Max(100)
{}

void wxsSpinCtrl::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            if ( GetParent() )
            {
                Code<< GetVarName() << _T(" = new wxSpinCtrl(");
            }
            else
            {
                Code<< _T("Create(");
            }
            long ValueLong = 0;
            Value.ToLong(&ValueLong);
            Code<< WindowParent << _T(",")
                << GetIdName() << _T(",")
                << wxsCodeMarks::WxString(wxsCPP,Value) << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(",")
                << wxString::Format(_T("%d"),Min) << _T(",")
                << wxString::Format(_T("%d"),Max) << _T(",")
                << wxString::Format(_T("%d"),ValueLong) << _T(",")
                << wxsCodeMarks::WxString(wxsCPP,GetIdName(),false) << _T(");\n");

            if ( !Value.empty() )
            {
                Code << GetVarName() << _T("->SetValue(") << wxsCodeMarks::WxString(wxsCPP,Value) << _(");\n");
            }
            SetupWindowCode(Code,Language);
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsSpinCtrl::OnBuildCreatingCode"),Language);
        }
    }
}

wxObject* wxsSpinCtrl::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxSpinCtrl* Preview = new wxSpinCtrl(Parent,GetId(),Value,Pos(Parent),Size(Parent),Style(),Min,Max);
    if ( !Value.empty() ) Preview->SetValue(Value);
    return SetupWindow(Preview,Flags);
}

void wxsSpinCtrl::OnEnumWidgetProperties(long Flags)
{
    WXS_STRING(wxsSpinCtrl,Value,0,_("Value"),_T("value"),_T(""),true,false)
    WXS_LONG(wxsSpinCtrl,Min,0,_("Min"),_T("min"),0)
    WXS_LONG(wxsSpinCtrl,Max,0,_("Max"),_T("max"),100)
}

void wxsSpinCtrl::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/spinctrl.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsSpinCtrl::OnEnumDeclFiles"),Language);
    }
}
