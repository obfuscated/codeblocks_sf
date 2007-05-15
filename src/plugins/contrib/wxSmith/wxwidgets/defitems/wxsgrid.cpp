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

#include "wxsgrid.h"
#include <wx/grid.h>

namespace
{
    wxsRegisterItem<wxsGrid> Reg(_T("Grid"),wxsTWidget,_T("Standard"),50);

    WXS_ST_BEGIN(wxsGridStyles,_T(""))
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsGridEvents)
        WXS_EVI(wxEVT_GRID_CELL_LEFT_CLICK,EVT_GRID_CMD_CELL_LEFT_CLICK,wxGridEvent,CellLeftClick)
        WXS_EVI(wxEVT_GRID_CELL_RIGHT_CLICK,EVT_GRID_CMD_CELL_RIGHT_CLICK,wxGridEvent,CellRightClick)
        WXS_EVI(wxEVT_GRID_CELL_LEFT_DCLICK,EVT_GRID_CMD_CELL_LEFT_DCLICK,wxGridEvent,CellLeftDClick)
        WXS_EVI(wxEVT_GRID_CELL_RIGHT_DCLICK,EVT_GRID_CMD_CELL_RIGHT_DCLICK,wxGridEvent,CellRightDClick)
        WXS_EVI(wxEVT_GRID_LABEL_LEFT_CLICK,EVT_GRID_CMD_LABEL_LEFT_CLICK,wxGridEvent,LabelLeftClick)
        WXS_EVI(wxEVT_GRID_LABEL_RIGHT_CLICK,EVT_GRID_CMD_LABEL_RIGHT_CLICK,wxGridEvent,LabelRightClick)
        WXS_EVI(wxEVT_GRID_LABEL_LEFT_DCLICK,EVT_GRID_CMD_LABEL_LEFT_DCLICK,wxGridEvent,LabelLeftDClick)
        WXS_EVI(wxEVT_GRID_LABEL_RIGHT_DCLICK,EVT_GRID_CMD_LABEL_RIGHT_DCLICK,wxGridEvent,LabelRightDClick)
        WXS_EVI(wxEVT_GRID_CELL_CHANGE,EVT_GRID_CMD_CELL_CHANGE,wxGridEvent,CellChange)
        WXS_EVI(wxEVT_GRID_SELECT_CELL,EVT_GRID_CMD_SELECT_CELL,wxGridEvent,CellSelect)
        WXS_EVI(wxEVT_GRID_EDITOR_HIDDEN,EVT_GRID_CMD_EDITOR_HIDDEN,wxGridEvent,EditorHidden)
        WXS_EVI(wxEVT_GRID_EDITOR_SHOWN,EVT_GRID_CMD_EDITOR_SHOWN,wxGridEvent,EditorShown)

        WXS_EV_DEFAULTS()
    WXS_EV_END()

}

wxsGrid::wxsGrid(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsGridEvents,
        wxsGridStyles)
{}


void wxsGrid::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code << Codef(Language,_T("%C(%W,%I,%P,%S,%T,%N);\n"));
            SetupWindowCode(Code,WindowParent,Language);
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsGrid::OnBuildCreatingCode"),Language);
        }
    }
}


wxObject* wxsGrid::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxGrid* Preview = new wxGrid(Parent,GetId(),Pos(Parent),Size(Parent),Style());
    return SetupWindow(Preview,Flags);
}


void wxsGrid::OnEnumWidgetProperties(long Flags)
{
}

void wxsGrid::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/grid.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsGrid::EnumDeclFiles"),Language);
    }
}
