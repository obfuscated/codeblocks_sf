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

#include "wxsscrolledwindow.h"

namespace
{
    wxsRegisterItem<wxsScrolledWindow> Reg(_T("ScrolledWindow"),wxsTContainer, _T("Standard"), 75);

    WXS_ST_BEGIN(wxsScrolledWindowStyles,_T("wxHSCROLL|wxVSCROLL"))
        WXS_ST_CATEGORY("wxScrolledWindow")
        WXS_ST(wxHSCROLL)
        WXS_ST(wxVSCROLL)
        WXS_ST(wxNO_3D)
        WXS_ST(wxTAB_TRAVERSAL)
        WXS_EXST(wxWS_EX_VALIDATE_RECURSIVELY)
    WXS_ST_END()

    WXS_EV_BEGIN(wxsScrolledWindowEvents)
        WXS_EV_DEFAULTS()
    WXS_EV_END()
}

wxsScrolledWindow::wxsScrolledWindow(wxsItemResData* Data):
    wxsContainer(
        Data,
        &Reg.Info,
        wxsScrolledWindowEvents,
        wxsScrolledWindowStyles)
{}

void wxsScrolledWindow::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            if ( GetParent() )
            {
                Code<< GetVarName() << _T(" = new wxScrolledWindow(");
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
            wxsCodeMarks::Unknown(_T("wxsScrolledWindow::OnBuildCreatingCode"),Language);
        }
    }
}

wxObject* wxsScrolledWindow::OnBuildPreview(wxWindow* Parent,long Flags)
{
    // TODO: Use grid-viewing panel when not in exact mode
    wxWindow* NewItem = new wxScrolledWindow( Parent,GetId(),wxDefaultPosition,wxDefaultSize,Style());
    SetupWindow(NewItem,Flags);
    AddChildrenPreview(NewItem,Flags);
    return NewItem;
}

void wxsScrolledWindow::OnEnumContainerProperties(long Flags)
{
}

void wxsScrolledWindow::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/panel.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsScrolledWindow::OnEnumDeclFiles"),Language);
    }
}
