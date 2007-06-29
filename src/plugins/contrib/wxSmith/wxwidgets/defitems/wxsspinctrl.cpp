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
        WXS_ST(wxSP_HORIZONTAL)
        WXS_ST(wxSP_VERTICAL)
        WXS_ST(wxSP_ARROW_KEYS)
        WXS_ST(wxSP_WRAP)
        WXS_ST_DEFAULTS()
    WXS_ST_END()


    WXS_EV_BEGIN(wxsSpinCtrlEvents)
        WXS_EVI(EVT_SPINCTRL,wxEVT_COMMAND_SPINCTRL_UPDATED,wxSpinEvent,Change)
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
            long ValueLong = 0;
            Value.ToLong(&ValueLong);
            Code << Codef(Language,_T("%C(%W,%I,%n,%P,%S,%T,%d,%d,%d,%N);\n"),Value.c_str(),Min,Max,ValueLong);
            if ( !Value.empty() ) Code << Codef(Language,_T("%ASetValue(%n);\n"),Value.c_str());
            SetupWindowCode(Code,WindowParent,Language);
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
    WXS_SHORT_STRING(wxsSpinCtrl,Value,_("Value"),_T("value"),_T(""),true)
    WXS_LONG(wxsSpinCtrl,Min,_("Min"),_T("min"),0)
    WXS_LONG(wxsSpinCtrl,Max,_("Max"),_T("max"),100)
}

void wxsSpinCtrl::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/spinctrl.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsSpinCtrl::OnEnumDeclFiles"),Language);
    }
}
