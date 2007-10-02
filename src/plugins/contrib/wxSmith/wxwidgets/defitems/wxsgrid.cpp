/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
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
        WXS_EVI(EVT_CMD_GRID_CELL_LEFT_CLICK,wxEVT_GRID_CELL_LEFT_CLICK,wxGridEvent,CellLeftClick)
        WXS_EVI(EVT_CMD_GRID_CELL_RIGHT_CLICK,wxEVT_GRID_CELL_RIGHT_CLICK,wxGridEvent,CellRightClick)
        WXS_EVI(EVT_CMD_GRID_CELL_LEFT_DCLICK,wxEVT_GRID_CELL_LEFT_DCLICK,wxGridEvent,CellLeftDClick)
        WXS_EVI(EVT_CMD_GRID_CELL_RIGHT_DCLICK,wxEVT_GRID_CELL_RIGHT_DCLICK,wxGridEvent,CellRightDClick)
        WXS_EVI(EVT_CMD_GRID_LABEL_LEFT_CLICK,wxEVT_GRID_LABEL_LEFT_CLICK,wxGridEvent,LabelLeftClick)
        WXS_EVI(EVT_CMD_GRID_LABEL_RIGHT_CLICK,wxEVT_GRID_LABEL_RIGHT_CLICK,wxGridEvent,LabelRightClick)
        WXS_EVI(EVT_CMD_GRID_LABEL_LEFT_DCLICK,wxEVT_GRID_LABEL_LEFT_DCLICK,wxGridEvent,LabelLeftDClick)
        WXS_EVI(EVT_CMD_GRID_LABEL_RIGHT_DCLICK,wxEVT_GRID_LABEL_RIGHT_DCLICK,wxGridEvent,LabelRightDClick)
        WXS_EVI(EVT_CMD_GRID_CELL_CHANGE,wxEVT_GRID_CELL_CHANGE,wxGridEvent,CellChange)
        WXS_EVI(EVT_CMD_GRID_SELECT_CELL,wxEVT_GRID_SELECT_CELL,wxGridEvent,CellSelect)
        WXS_EVI(EVT_CMD_GRID_EDITOR_HIDDEN,wxEVT_GRID_EDITOR_HIDDEN,wxGridEvent,EditorHidden)
        WXS_EVI(EVT_CMD_GRID_EDITOR_SHOWN,wxEVT_GRID_EDITOR_SHOWN,wxGridEvent,EditorShown)

        WXS_EV_DEFAULTS()
    WXS_EV_END()

}

wxsGrid::wxsGrid(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsGridEvents,
        wxsGridStyles),
    Cols(0),
    Rows(0)
{}


void wxsGrid::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/grid.h>"),GetInfo().ClassName,0);
            AddHeader(_T("<wx/grid.h>"),_T("wxGridEvent"),0);
            Codef(_T("%C(%W, %I, %P, %S, %T, %N);\n"));
            if ( GetPropertiesFlags() & flSource )
            {
                if ( Cols>=0 && Rows>=0 && (Cols>0 || Rows>0) )
                {
                    Codef(_T("%ACreateGrid(%d,%d);\n"),Rows,Cols);
                }
            }
            BuildSetupWindowCode();
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsGrid::OnBuildCreatingCode"),GetLanguage());
        }
    }
}

wxObject* wxsGrid::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxGrid* Preview = new wxGrid(Parent,GetId(),Pos(Parent),Size(Parent),Style());
    if ( GetPropertiesFlags() & flSource )
    {
        if ( Cols>=0 && Rows>=0 && (Cols>0 || Rows>0) )
        {
            Preview->CreateGrid(Rows,Cols);
        }
    }
    return SetupWindow(Preview,Flags);
}

void wxsGrid::OnEnumWidgetProperties(long Flags)
{
    if ( Flags & flSource )
    {
        WXS_LONG(wxsGrid,Cols,_("Number of columns"),_T("cols"),0);
        WXS_LONG(wxsGrid,Rows,_("Number of rows"),_T("rows"),0);
    }
}
