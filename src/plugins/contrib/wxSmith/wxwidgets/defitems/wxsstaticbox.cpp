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

#include "wxsstaticbox.h"

namespace
{
    wxsRegisterItem<wxsStaticBox> Reg(_T("StaticBox"),wxsTWidget,_T("Standard"),50);

    WXS_ST_BEGIN(wxsStaticBoxStyles,_T(""))
        WXS_ST_CATEGORY("wxStaticBox")
        WXS_ST(wxBU_LEFT)
        WXS_ST(wxBU_TOP)
        WXS_ST(wxBU_RIGHT)
        WXS_ST(wxBU_BOTTOM)
        WXS_ST(wxBU_EXACTFIT)
        WXS_ST_MASK(wxNO_BORDER,wxsSFWin| wxsSFGTK ,0,true)
    WXS_ST_END()

    WXS_EV_BEGIN(wxsStaticBoxEvents)
        WXS_EVI(EVT_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEvent,Click)
        WXS_EV_DEFAULTS()
    WXS_EV_END()
}

wxsStaticBox::wxsStaticBox(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsStaticBoxEvents,
        wxsStaticBoxStyles),
    Label(_("Label"))
{}

void wxsStaticBox::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code << Codef(Language,_T("%C(%W,%I,%t,%P,%S,%T,%N);\n"),Label.c_str());
            SetupWindowCode(Code,WindowParent,Language);
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsStaticBox::OnBuildCreatingCode"),Language);
        }
    }
}

wxObject* wxsStaticBox::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxStaticBox* Preview = new wxStaticBox(Parent,GetId(),Label,Pos(Parent),Size(Parent),Style());
    return SetupWindow(Preview,Flags);
}

void wxsStaticBox::OnEnumWidgetProperties(long Flags)
{
    WXS_SHORT_STRING(wxsStaticBox,Label,_("Label"),_T("label"),_T(""),false)
}

void wxsStaticBox::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/statbox.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsStaticBox::OnEnumDeclFiles"),Language);
    }
}
