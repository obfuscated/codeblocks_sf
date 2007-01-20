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
    const long    OrientValues[] = { wxHORIZONTAL, wxVERTICAL, 0 };
    const wxChar* OrientNames[]  = { _T("wxHORIZONTAL"), _T("wxVERTICAL"), NULL };

    class OrientProp: public wxsEnumProperty
    {
        public:
            OrientProp(int Offset):
                wxsEnumProperty(
                    _("Orientation"),
                    _T("orient"),
                    Offset,
                    OrientValues,
                    OrientNames,
                    false,
                    wxHORIZONTAL,
                    true)
                {}

            virtual const wxString GetTypeName()
            {
                return _T("Sizer Orientation");
            }
    };

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
            Code << GetVarName() << _T(" = new wxStaticBoxSizer(");
            if ( Orient != wxHORIZONTAL ) Code << _T("wxVERTICAL"); else Code << _T("wxHORIZONTAL");
            Code << _T(",") << WindowParent << _T(",") << wxsCodeMarks::WxString(wxsCPP,Label) << _T(");\n");
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
    WXS_STRING(wxsStaticBoxSizer,Label,0,_("Label"),_T("label"),wxEmptyString,false,false);
    static OrientProp Prop(wxsOFFSET(wxsStaticBoxSizer,Orient));
    Property(Prop,0);
}

void wxsStaticBoxSizer::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/sizer.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsStaticBoxSizer::OnEnumDeclFiles"),Language);
    }
}
