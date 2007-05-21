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

#include "wxsdatepickerctrl.h"

#include <wx/datectrl.h>

namespace
{
    wxsRegisterItem<wxsDatePickerCtrl> Reg(_T("DatePickerCtrl"),wxsTWidget,_T("Standard"),30);

    WXS_ST_BEGIN(wxsDatePickerCtrlStyles,_T("wxDP_DEFAULT|wxDP_SHOWCENTURY"))
        WXS_ST_CATEGORY("wxDatePickerCtrl")
        WXS_ST(wxDP_DEFAULT)
        WXS_ST(wxDP_SPIN)
        WXS_ST(wxDP_DROPDOWN)
        WXS_ST(wxDP_ALLOWNONE)
        WXS_ST(wxDP_SHOWCENTURY)
    WXS_ST_END()


    WXS_EV_BEGIN(wxsDatePickerCtrlEvents)
        WXS_EVI(EVT_DATE_CHANGED,wxEVT_DATE_CHANGED,wxDataEvent,Changed)
    WXS_EV_END()
}

wxsDatePickerCtrl::wxsDatePickerCtrl(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsDatePickerCtrlEvents,
        wxsDatePickerCtrlStyles)
{}

void wxsDatePickerCtrl::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code << Codef(Language,_T("%C(%W,%I,wxDefaultDateTime,%P,%S,%T,%V,%N);\n"));
            SetupWindowCode(Code,WindowParent,Language);
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsDatePickerCtrl::OnBuildCreatingCode"),Language);
        }
    }
}


wxObject* wxsDatePickerCtrl::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxDatePickerCtrl* Preview = new wxDatePickerCtrl(Parent,GetId(),wxDefaultDateTime,Pos(Parent),Size(Parent),Style());
    return SetupWindow(Preview,Flags);
}

void wxsDatePickerCtrl::OnEnumWidgetProperties(long Flags)
{
}

void wxsDatePickerCtrl::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/datectrl.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsDatePickerCtrl::OnEnumDeclFiles"),Language);
    }
}
