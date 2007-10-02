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

#include "wxstreectrl.h"

namespace
{
    wxsRegisterItem<wxsTreeCtrl> Reg(_T("TreeCtrl"),wxsTWidget,_T("Standard"),70);

    WXS_ST_BEGIN(wxsTreeCtrlStyles,_T("wxTR_DEFAULT_STYLE"))
        WXS_ST_CATEGORY("wxTreeCtrl")
        WXS_ST(wxTR_EDIT_LABELS)
        WXS_ST(wxTR_NO_BUTTONS)
        WXS_ST(wxTR_HAS_BUTTONS)
        WXS_ST(wxTR_TWIST_BUTTONS)
        WXS_ST(wxTR_NO_LINES)
        WXS_ST(wxTR_FULL_ROW_HIGHLIGHT)
        WXS_ST(wxTR_LINES_AT_ROOT)
        WXS_ST(wxTR_HIDE_ROOT)
        WXS_ST(wxTR_ROW_LINES)
        WXS_ST(wxTR_HAS_VARIABLE_ROW_HEIGHT)
        WXS_ST(wxTR_SINGLE)
        WXS_ST(wxTR_MULTIPLE)
        WXS_ST(wxTR_EXTENDED)
        WXS_ST(wxTR_DEFAULT_STYLE)
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsTreeCtrlEvents)
        WXS_EVI(EVT_TREE_BEGIN_DRAG,wxEVT_COMMAND_TREE_BEGIN_DRAG,wxTreeEvent,BeginDrag)
        WXS_EVI(EVT_TREE_BEGIN_RDRAG,wxEVT_COMMAND_TREE_BEGIN_RDRAG,wxTreeEvent,BeginRDrag)
        WXS_EVI(EVT_TREE_END_DRAG,wxEVT_COMMAND_TREE_END_DRAG,wxTreeEvent,EndDrag)
        WXS_EVI(EVT_TREE_BEGIN_LABEL_EDIT,wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT,wxTreeEvent,BeginLabelEdit)
        WXS_EVI(EVT_TREE_END_LABEL_EDIT,wxEVT_COMMAND_TREE_END_LABEL_EDIT,wxTreeEvent,EndLabelEdit)
        WXS_EVI(EVT_TREE_DELETE_ITEM,wxEVT_COMMAND_TREE_DELETE_ITEM,wxTreeEvent,DeleteItem)
        WXS_EVI(EVT_TREE_GET_INFO,wxEVT_COMMAND_TREE_GET_INFO,wxTreeEvent,GetInfo)
        WXS_EVI(EVT_TREE_SET_INFO,wxEVT_COMMAND_TREE_SET_INFO,wxTreeEvent,SetInfo)
        WXS_EVI(EVT_TREE_ITEM_ACTIVATED,wxEVT_COMMAND_TREE_ITEM_ACTIVATED,wxTreeEvent,ItemActivated)
        WXS_EVI(EVT_TREE_ITEM_COLLAPSED,wxEVT_COMMAND_TREE_ITEM_COLLAPSED,wxTreeEvent,ItemCollapsed)
        WXS_EVI(EVT_TREE_ITEM_COLLAPSING,wxEVT_COMMAND_TREE_ITEM_COLLAPSING,wxTreeEvent,ItemCollapsing)
        WXS_EVI(EVT_TREE_ITEM_EXPANDED,wxEVT_COMMAND_TREE_ITEM_EXPANDED,wxTreeEvent,ItemExpanded)
        WXS_EVI(EVT_TREE_ITEM_EXPANDING,wxEVT_COMMAND_TREE_ITEM_EXPANDING,wxTreeEvent,ItemExpanding)
        WXS_EVI(EVT_TREE_ITEM_RIGHT_CLICK,wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK,wxTreeEvent,ItemRightClick)
        WXS_EVI(EVT_TREE_ITEM_MIDDLE_CLICK,wxEVT_COMMAND_TREE_ITEM_MIDDLE_CLICK,wxTreeEvent,ItemMiddleClick)
        WXS_EVI(EVT_TREE_SEL_CHANGED,wxEVT_COMMAND_TREE_SEL_CHANGED,wxTreeEvent,SelectionChanged)
        WXS_EVI(EVT_TREE_SEL_CHANGING,wxEVT_COMMAND_TREE_SEL_CHANGING,wxTreeEvent,SelectionChanging)
        WXS_EVI(EVT_TREE_KEY_DOWN,wxEVT_COMMAND_TREE_KEY_DOWN,wxTreeEvent,KeyDown)
        WXS_EVI(EVT_TREE_ITEM_GETTOOLTIP,wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP,wxTreeEvent,ItemGetToolTip)
        WXS_EVI(EVT_TREE_ITEM_MENU,wxEVT_COMMAND_TREE_ITEM_MENU,wxTreeEvent,ItemMenu)
    WXS_EV_END()
}

wxsTreeCtrl::wxsTreeCtrl(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsTreeCtrlEvents,
        wxsTreeCtrlStyles)
{}

void wxsTreeCtrl::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/treectrl.h>"),GetInfo().ClassName,0);
            AddHeader(_T("<wx/treectrl.h>"),_T("wxTreeEvent"),0);
            Codef(_T("%C(%W, %I, %P, %S, %T, %V, %N);\n"));
            BuildSetupWindowCode();
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsTreeCtrl::OnBuildCreatingCode"),GetLanguage());
        }
    }
}


wxObject* wxsTreeCtrl::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxTreeCtrl* Preview = new wxTreeCtrl(Parent,GetId(),Pos(Parent),Size(Parent),Style());
    return SetupWindow(Preview,Flags);
}


void wxsTreeCtrl::OnEnumWidgetProperties(long Flags)
{
}
