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

#include "wxsgridsizer.h"

namespace
{
    wxsRegisterItem<wxsGridSizer> Reg(_T("GridSizer"),wxsTSizer,_T("Layout"),80);
}

wxsGridSizer::wxsGridSizer(wxsItemResData* Data):
    wxsSizer(Data,&Reg.Info),
    Cols(3),
    Rows(0)
{
}

wxSizer* wxsGridSizer::OnBuildSizerPreview(wxWindow* Parent)
{
	return new wxGridSizer(Rows,Cols,
        VGap.GetPixels(Parent),HGap.GetPixels(Parent));
}

void wxsGridSizer::OnBuildSizerCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code << Codef(Language,_T("%C(%d,%d,%s,%s);\n"),Rows,Cols,
                    VGap.GetPixelsCode(WindowParent,wxsCPP).c_str(),
                    HGap.GetPixelsCode(WindowParent,wxsCPP).c_str());
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsGridSizer::OnBuildSizerCreatingCode"),Language);
        }
    }
}

void wxsGridSizer::OnEnumSizerProperties(long Flags)
{
    WXS_LONG(wxsGridSizer,Cols,0,_("Cols"),_T("cols"),0);
    WXS_LONG(wxsGridSizer,Rows,0,_("Rows"),_T("rows"),0);
    WXS_DIMENSION(wxsGridSizer,VGap,0,_("V-Gap"),_("V-Gap in dialog units"),_T("vgap"),0,false);
    WXS_DIMENSION(wxsGridSizer,HGap,0,_("H-Gap"),_("H,y-Gap in dialog units"),_T("hgap"),0,false);
}

void wxsGridSizer::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/sizer.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsGridSizer::OnEnumDeclFiles"),Language);
    }
}
