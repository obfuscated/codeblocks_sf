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

#include "wxslistctrl.h"

#include <wx/listctrl.h>

namespace
{
    wxsRegisterItem<wxsListCtrl> Reg(_T("ListCtrl"),wxsTWidget,_T("Standard"),60);

    WXS_ST_BEGIN(wxsListCtrlStyles,_T(""))
        WXS_ST_CATEGORY("wxListCtrl")
        WXS_ST(wxLC_LIST);
        WXS_ST(wxLC_REPORT)
        WXS_ST(wxLC_ICON)
        WXS_ST(wxLC_SMALL_ICON)
        WXS_ST(wxLC_ALIGN_TOP)
        WXS_ST(wxLC_ALIGN_LEFT)
        WXS_ST(wxLC_AUTOARRANGE)
        WXS_ST(wxLC_USER_TEXT)
        WXS_ST(wxLC_EDIT_LABELS)
        WXS_ST(wxLC_NO_HEADER)
        WXS_ST(wxLC_SINGLE_SEL)
        WXS_ST(wxLC_SORT_ASCENDING)
        WXS_ST(wxLC_SORT_DESCENDING)
        WXS_ST(wxLC_VIRTUAL)
        WXS_ST(wxLC_HRULES)
        WXS_ST(wxLC_VRULES)
        WXS_ST(wxLC_NO_SORT_HEADER)
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsListCtrlEvents)
        WXS_EVI(EVT_LIST_BEGIN_DRAG,wxEVT_COMMAND_LIST_BEGIN_DRAG,wxListEvent,BeginDrag)
        WXS_EVI(EVT_LIST_BEGIN_RDRAG,wxEVT_COMMAND_LIST_BEGIN_RDRAG,wxListEvent,BeginRDrag)
        WXS_EVI(EVT_LIST_BEGIN_LABEL_EDIT,wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT,wxListEvent,BeginLabelEdit)
        WXS_EVI(EVT_LIST_END_LABEL_EDIT,wxEVT_COMMAND_LIST_END_LABEL_EDIT,wxListEvent,EndLabelEdit)
        WXS_EVI(EVT_LIST_DELETE_ITEM,wxEVT_COMMAND_LIST_DELETE_ITEM,wxListEvent,DeleteItem)
        WXS_EVI(EVT_LIST_DELETE_ALL_ITEMS,wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS,wxListEvent,DeleteAllItems)
        WXS_EVI(EVT_LIST_ITEM_SELECTED,wxEVT_COMMAND_LIST_ITEM_SELECTED,wxListEvent,ItemSelect)
        WXS_EVI(EVT_LIST_ITEM_DESELECTED,wxEVT_COMMAND_LIST_ITEM_DESELECTED,wxListEvent,ItemDeselect)
        WXS_EVI(EVT_LIST_ITEM_ACTIVATED,wxEVT_COMMAND_LIST_ITEM_ACTIVATED,wxListEvent,ItemActivated)
        WXS_EVI(EVT_LIST_ITEM_FOCUSED,wxEVT_COMMAND_LIST_ITEM_FOCUSED,wxListEvent,ItemFocused)
        WXS_EVI(EVT_LIST_ITEM_MIDDLE_CLICK,wxEVT_COMMAND_LIST_ITEM_MIDDLE_CLICK,wxListEvent,ItemMClick)
        WXS_EVI(EVT_LIST_ITEM_RIGHT_CLICK,wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK,wxListEvent,ItemRClick)
        WXS_EVI(EVT_LIST_KEY_DOWN,wxEVT_COMMAND_LIST_KEY_DOWN,wxListEvent,KeyDown)
        WXS_EVI(EVT_LIST_INSERT_ITEM,wxEVT_COMMAND_LIST_INSERT_ITEM,wxListEvent,InsertItem)
        WXS_EVI(EVT_LIST_COL_CLICK,wxEVT_COMMAND_LIST_COL_CLICK,wxListEvent,ColumnClick)
        WXS_EVI(EVT_LIST_COL_RIGHT_CLICK,wxEVT_COMMAND_LIST_COL_RIGHT_CLICK,wxListEvent,ColumnRClick)
        WXS_EVI(EVT_LIST_COL_BEGIN_DRAG,wxEVT_COMMAND_LIST_COL_BEGIN_DRAG,wxListEvent,ColumnBeginDrag)
        WXS_EVI(EVT_LIST_COL_DRAGGING,wxEVT_COMMAND_LIST_COL_DRAGGING,wxListEvent,ColumnDragging)
        WXS_EVI(EVT_LIST_COL_END_DRAG,wxEVT_COMMAND_LIST_COL_END_DRAG,wxListEvent,ColumnEndDrag)
        WXS_EVI(EVT_LIST_CACHE_HINT,wxEVT_COMMAND_LIST_CACHE_HINT,wxListEvent,CacheHint)
    WXS_EV_END()
}

wxsListCtrl::wxsListCtrl(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsListCtrlEvents,
        wxsListCtrlStyles)
{}

void wxsListCtrl::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code << Codef(Language,_T("%C(%W, %I, %P, %S, %T, %V, %N);\n"));
            SetupWindowCode(Code,WindowParent,Language);
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsListCtrl::OnBuildCreatingCode"),Language);
        }
    }
}


wxObject* wxsListCtrl::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxListCtrl* Preview = new wxListCtrl(Parent,GetId(),Pos(Parent),Size(Parent),Style());
    return SetupWindow(Preview,Flags);
}


void wxsListCtrl::OnEnumWidgetProperties(long Flags)
{
}

void wxsListCtrl::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/listctrl.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsListCtrl::OnEnumDeclFiles"),Language);
    }
}

