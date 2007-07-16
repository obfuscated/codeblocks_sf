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

#include "wxsstaticboxsizer.h"

namespace
{
    wxsRegisterItem<wxsStaticBoxSizer> Reg(_T("StaticBoxSizer"),wxsTSizer,_T("Layout"),40);
}


wxsStaticBoxSizer::wxsStaticBoxSizer(wxsItemResData* Data):
    wxsSizer(Data,&Reg.Info),
    Orient(wxHORIZONTAL),
    Label(_("Label"))
{
}


wxSizer* wxsStaticBoxSizer::OnBuildSizerPreview(wxWindow* Parent)
{
    return new wxStaticBoxSizer(Orient,Parent,Label);
}

void wxsStaticBoxSizer::OnBuildSizerCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code << Codef(Language,_T("%C(%s, %W, %t);\n"),
                    (Orient!=wxHORIZONTAL)?_T("wxVERTICAL"):_T("wxHORIZONTAL"),
                    Label.c_str());
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsStaticBoxSizer::OnBuildSizerCreatingCode"),Language);
        }
    }
}

void wxsStaticBoxSizer::OnEnumSizerProperties(long Flags)
{
    static const long    OrientValues[] = { wxHORIZONTAL, wxVERTICAL, 0 };
    static const wxChar* OrientNames[]  = { _T("wxHORIZONTAL"), _T("wxVERTICAL"), 0 };

    WXS_SHORT_STRING(wxsStaticBoxSizer,Label,_("Label"),_T("label"),_T(""),false);
    WXS_ENUM(wxsStaticBoxSizer,Orient,_("Orientation"),_T("orient"),OrientValues,OrientNames,wxHORIZONTAL);
}

void wxsStaticBoxSizer::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/sizer.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsStaticBoxSizer::OnEnumDeclFiles"),Language);
    }
}
