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

#include <wx/radiobut.h>
#include "wxsradiobutton.h"

namespace
{
    wxsRegisterItem<wxsRadioButton> Reg(_T("RadioButton"),wxsTWidget,_T("Standard"),60);

    WXS_ST_BEGIN(wxsRadioButtonStyles,_T(""))
        WXS_ST_CATEGORY("wxRadioButton")
        WXS_ST(wxRB_GROUP)
        WXS_ST(wxRB_SINGLE)
        WXS_ST_DEFAULTS()
    WXS_ST_END()


    WXS_EV_BEGIN(wxsRadioButtonEvents)
        WXS_EVI(EVT_RADIOBUTTON,wxEVT_COMMAND_RADIOBUTTON_SELECTED,wxCommandEvent,Select)
    WXS_EV_END()
}

wxsRadioButton::wxsRadioButton(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsRadioButtonEvents,
        wxsRadioButtonStyles),
    Label(_("Label")),
    IsSelected(false)
{}


void wxsRadioButton::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code << Codef(Language,_T("%C(%W, %I, %t, %P, %S, %T, %V, %N);\n"),Label.c_str());
            if ( IsSelected ) Code << Codef(Language, _T("%ASetValue(%b);\n"), true);
            SetupWindowCode(Code,WindowParent,Language);
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsRadioButton::OnBuildCreatingCode"),Language);
        }
    }
}

wxObject* wxsRadioButton::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxRadioButton* Preview = new wxRadioButton(Parent,GetId(),Label,Pos(Parent),Size(Parent),Style());
    Preview->SetValue(IsSelected);
    return SetupWindow(Preview,Flags);
}

void wxsRadioButton::OnEnumWidgetProperties(long Flags)
{
    WXS_SHORT_STRING(wxsRadioButton,Label,_("Label"),_T("label"),_T(""),true)
    WXS_BOOL(wxsRadioButton,IsSelected,_("Is Selected"),_T("selected"),false)
}

void wxsRadioButton::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/radiobut.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsRadioButton::OnEnumDeclFiles"),Language);
    }
}
