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

#include "wxslistbox.h"

namespace
{
    wxsRegisterItem<wxsListBox> Reg(_T("ListBox"),wxsTWidget,_T("Standard"),70);

    WXS_ST_BEGIN(wxsListBoxStyles,_T(""))
        WXS_ST_CATEGORY("wxListBox")
        WXS_ST_MASK(wxLB_HSCROLL,wxsSFWin,0,true) // Windows ONLY
        WXS_ST(wxLB_SINGLE)
        WXS_ST(wxLB_MULTIPLE)
        WXS_ST(wxLB_EXTENDED)
        WXS_ST(wxLB_ALWAYS_SB)
        WXS_ST(wxLB_NEEDED_SB)
        WXS_ST(wxLB_SORT)
    WXS_ST_END()

    WXS_EV_BEGIN(wxsListBoxEvents)
        WXS_EVI(EVT_LISTBOX,wxEVT_COMMAND_LISTBOX_SELECTED,wxCommandEvent,Select)
        WXS_EVI(EVT_LISTBOX_DCLICK,wxEVT_COMMAND_LISTBOX_DOUBLECLICKED,wxCommandEvent,DClick)
        WXS_EV_DEFAULTS()
    WXS_EV_END()
}

wxsListBox::wxsListBox(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsListBoxEvents,
        wxsListBoxStyles),
    DefaultSelection(-1)
{}

void wxsListBox::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            if ( !IsRootItem() )
            {
                Code<< GetVarName() << _T(" = new wxListBox(");
            }
            else
            {
                Code<< _T("Create(");
            }
            Code<< WindowParent << _T(",")
                << GetIdName() << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << wxString::Format(_T("%d"),0)  << _T(",")
                << wxString::Format(_T("%d"),0)  << _T(",")
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
            wxsCodeMarks::Unknown(_T("wxsListBox::OnBuildCreatingCode"),Language);
        }
    }
}


wxObject* wxsListBox::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxListBox* Preview = new wxListBox(Parent,GetId(),Pos(Parent),Size(Parent),0,0, Style());
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

void wxsListBox::OnEnumWidgetProperties(long Flags)
{
      WXS_ARRAYSTRING(wxsListBox,ArrayChoices,0,_("Choices"),_T("content"),_T("item"))
      WXS_LONG(wxsListBox,DefaultSelection,0,_("Default"),_T("default"),0)
}

void wxsListBox::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/listbox.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsListBox::EnumDeclFiles"),Language);
    }
}
