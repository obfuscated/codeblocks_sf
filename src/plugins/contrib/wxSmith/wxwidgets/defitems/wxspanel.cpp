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

#include "wxspanel.h"

namespace
{
    wxsRegisterItem<wxsPanel> Reg(_T("Panel"),wxsTContainer, _T("Standard"), 75);

    WXS_ST_BEGIN(wxsPanelStyles,_T("wxTAB_TRAVERSAL"))
        WXS_ST_CATEGORY("wxPanel")
        WXS_ST(wxNO_3D)
        WXS_ST(wxTAB_TRAVERSAL)
        WXS_EXST(wxWS_EX_VALIDATE_RECURSIVELY)
    WXS_ST_END()

    WXS_EV_BEGIN(wxsPanelEvents)
        WXS_EV_DEFAULTS()
    WXS_EV_END()
}

wxsPanel::wxsPanel(wxsItemResData* Data):
    wxsContainer(
        Data,
        &Reg.Info,
        wxsPanelEvents,
        wxsPanelStyles)
{}

void wxsPanel::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            if ( !IsRootItem() )
            {
                Code<< GetVarName() << _T(" = new wxPanel(");
            }
            else
            {
                Code<< _T("Create(");
            }
            Code<< WindowParent << _T(",")
                << GetIdName() << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(",")
                << wxsCodeMarks::WxString(wxsCPP,GetIdName(),false) << _T(");\n");

            SetupWindowCode(Code,wxsCPP);
            AddChildrenCode(Code,wxsCPP);
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsPanel::OnBuildCreatingCode"),Language);
        }
    }
}

wxObject* wxsPanel::OnBuildPreview(wxWindow* Parent,long Flags)
{
    // TODO: Use grid-viewing panel when not in exact mode
    wxWindow* NewItem = new wxPanel( Parent,GetId(),wxDefaultPosition,wxDefaultSize,Style());
    NewItem->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE ));
    SetupWindow(NewItem,Flags);
    AddChildrenPreview(NewItem,Flags);
    return NewItem;
}

void wxsPanel::OnEnumContainerProperties(long Flags)
{
}

void wxsPanel::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/panel.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsPanel::OnEnumDeclFiles"),Language);
    }
}
