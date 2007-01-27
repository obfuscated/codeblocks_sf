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

#include "wxsradiobutton.h"

namespace
{
    wxsRegisterItem<wxsRadioButton> Reg(_T("RadioButton"),wxsTWidget,_T("Standard"),60);

    WXS_ST_BEGIN(wxsRadioButtonStyles,_T(""))
        WXS_ST_CATEGORY("wxRadioButton")
        WXS_ST(wxRB_GROUP)
        WXS_ST(wxRB_SINGLE)
    // NOTE (cyberkoa#1#): wxRB_USE_CHECKBOX is only available under PALMOS, not sure whether in XRC PALMOS port or not
        WXS_ST_MASK(wxRB_USE_CHECKBOX,wxsSFPALMOS,0,true)
    WXS_ST_END()


    WXS_EV_BEGIN(wxsRadioButtonEvents)
        WXS_EVI(EVT_RADIOBUTTON,wxEVT_COMMAND_RADIOBUTTON_SELECTED,wxCommandEvent,Select)
        WXS_EV_DEFAULTS()
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
            if ( !IsRootItem() )
            {
                Code<< GetVarName() << _T(" = new wxRadioButton(");
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

            if ( IsSelected )
            {
                Code << GetVarName() << _T("->SetValue(true);\n");
            }
            SetupWindowCode(Code,Language);
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
    WXS_STRING(wxsRadioButton,Label,0,_("Label"),_T("label"),_T(""),true,false)
    WXS_BOOL  (wxsRadioButton,IsSelected,0,_("Is Selected"),_T("selected"),false)
}

void wxsRadioButton::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/radiobut.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsRadioButton::OnEnumDeclFiles"),Language);
    }
}
