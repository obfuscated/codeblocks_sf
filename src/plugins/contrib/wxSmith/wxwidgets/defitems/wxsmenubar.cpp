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

#include "wxsmenubar.h"
#include "../wxsitemresdata.h"
#include <wx/menu.h>

namespace
{
    wxsRegisterItem<wxsMenuBar> Reg(_T("MenuBar"),wxsTTool,/*_T("Tools")*/_T(""),90);
}

wxsMenuBar::wxsMenuBar(wxsItemResData* Data):
    wxsTool(
        Data,
        &Reg.Info,
        NULL,
        NULL,
        flVariable)
{
}

void wxsMenuBar::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
}

void wxsMenuBar::OnEnumToolProperties(long Flags)
{
}

void wxsMenuBar::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/menu.h>")); break;
        default: wxsCodeMarks::Unknown(_T("wxsMenuBar::OnEnumDeclFiles"),Language);
    }
}

bool wxsMenuBar::OnCanAddToResource(wxsItemResData* Data,bool ShowMessage)
{
    if ( Data->GetClassType() != _T("wxFrame") )
    {
        if ( ShowMessage )
        {
            cbMessageBox(_T("wxMenuBar can be added to wxFrame only"));
        }
        return false;
    }

    for ( int i=0; i<Data->GetToolsCount(); i++ )
    {
        if ( Data->GetTool(i)->GetClassName() == _T("wxMenuBar") )
        {
            if ( ShowMessage )
            {
                cbMessageBox(_T("Can not add two or more wxMenuBar classes"));
            }
            return false;
        }
    }

    return true;
}

void wxsMenuBar::ShowMenuEditor()
{
}
