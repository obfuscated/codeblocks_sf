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

#include "wxsstatictext.h"

namespace
{
    wxsRegisterItem<wxsStaticText> Reg(_T("StaticText"),wxsTWidget,_T("Standard"),80);

    WXS_ST_BEGIN(wxsStaticTextStyles,_T(""))
        WXS_ST_CATEGORY("wxStaticText")
        WXS_ST(wxST_NO_AUTORESIZE)
        WXS_ST(wxALIGN_LEFT)
        WXS_ST(wxALIGN_RIGHT)
        WXS_ST(wxALIGN_CENTRE)
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsStaticTextEvents)
    WXS_EV_END()

}

wxsStaticText::wxsStaticText(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsStaticTextEvents,
        wxsStaticTextStyles),
    Label(_("Label"))

{}


void wxsStaticText::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code << Codef(Language,_T("%C(%W, %I, %t, %P, %S, %T, %N);\n"),Label.c_str());
            SetupWindowCode(Code,WindowParent,Language);
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsStaticText::OnBuildCreatingCode"),Language);
        }
    }
}


wxObject* wxsStaticText::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxStaticText* Preview = new wxStaticText(Parent,GetId(),Label,Pos(Parent),Size(Parent),Style());
    return SetupWindow(Preview,Flags);
}


void wxsStaticText::OnEnumWidgetProperties(long Flags)
{
    WXS_STRING(wxsStaticText,Label,_("Label"),_T("label"),_T(""),true)
}

void wxsStaticText::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/stattext.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsStaticText::OnEnumDeclFiles"),Language);
    }
}
