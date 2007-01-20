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

#include "wxscombobox.h"

namespace
{
    wxsRegisterItem<wxsComboBox> Reg(_T("ComboBox"),wxsTWidget,_T("Standard"),70);


    WXS_ST_BEGIN(wxsComboBoxStyles,_T(""))
        WXS_ST_CATEGORY("wxComboBox")
        WXS_ST_MASK(wxCB_SIMPLE,wxsSFWin,0,true)
        WXS_ST(wxCB_READONLY)
        WXS_ST(wxCB_SORT)
        WXS_ST(wxCB_DROPDOWN)
    WXS_ST_END()

    WXS_EV_BEGIN(wxsComboBoxEvents)
        WXS_EVI(EVT_COMBOBOX,wxEVT_COMMAND_COMBOBOX_SELECTED,wxCommandEvent,Select)
        WXS_EVI(EVT_TEXT,wxEVT_COMMAND_TEXT_UPDATED,wxCommandEvent,Text)
        WXS_EVI(EVT_TEXT_ENTER,wxEVT_COMMAND_TEXT_ENTER,wxCommandEvent,TextEnter)
        WXS_EV_DEFAULTS()
    WXS_EV_END()
}

wxsComboBox::wxsComboBox(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsComboBoxEvents,
        wxsComboBoxStyles),
    DefaultSelection(-1)
{}

void wxsComboBox::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            if ( GetParent() )
            {
                Code<< GetVarName() << _T(" = new wxComboBox(");
            }
            else
            {
                Code<< _T("Create(");
            }
            Code<< WindowParent << _T(",")
                << GetIdName() << _T(",")
                << wxsCodeMarks::WxString(wxsCPP,_T("")) << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << _T("0,NULL,")
                << StyleCode(wxsCPP) << _T(",")
                << _T("wxDefaultValidator") << _T(",")
                << wxsCodeMarks::WxString(wxsCPP,GetIdName(),false) << _T(");\n");

            for ( size_t i = 0; i <  ArrayChoices.GetCount(); ++i )
            {
                if ( DefaultSelection == (int)i )
                {
                    Code << GetVarName() << _T("->SetSelection( ");
                }
                Code << GetVarName() << _T("->Append(") << wxsCodeMarks::WxString(wxsCPP,ArrayChoices[i]) << _T(")");
                if ( DefaultSelection == (int)i )
                {
                    Code << _T(" )");
                }
                Code << _T(";\n");
            }

            SetupWindowCode(Code,Language);
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsComboBox::OnBuildCreatingCode"),Language);
        }
    }
}

wxObject* wxsComboBox::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxComboBox* Preview = new wxComboBox(Parent,GetId(),_T(""),Pos(Parent),Size(Parent),ArrayChoices, Style());

    for ( size_t i = 0; i <  ArrayChoices.GetCount(); ++i )
    {
        int Val = Preview->Append(ArrayChoices[i]);
        if ( (int)i == DefaultSelection )
        {
            Preview->SetSelection(Val);
        }
    }

    return SetupWindow(Preview,Flags);
}

void wxsComboBox::OnEnumWidgetProperties(long Flags)
{
      WXS_ARRAYSTRING(wxsComboBox,ArrayChoices,0,_("Choices"),_T("content"),_T("item"))
      WXS_LONG(wxsComboBox,DefaultSelection,0,_("Default"),_T("default"),0)
}

void wxsComboBox::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/combobox.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsComboBox::OnEnumDeclFiles"),Language);
    }
}
