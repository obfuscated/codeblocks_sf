/**  \file wxsgenericdirctrl.cpp
*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxsgenericdirctrl.h"

#include <wx/dirctrl.h>

namespace
{
    wxsRegisterItem<wxsGenericDirCtrl> Reg(_T("GenericDirCtrl"),wxsTWidget,_T("Advanced"),90);

    WXS_ST_BEGIN(wxsGenericDirCtrlStyles,_T(""))
        WXS_ST_CATEGORY("wxGenericDirCtrl")
        WXS_ST(wxDIRCTRL_DIR_ONLY)
        WXS_ST(wxDIRCTRL_3D_INTERNAL)
        WXS_ST(wxDIRCTRL_SELECT_FIRST)
#if !wxCHECK_VERSION(3, 0, 0)
        WXS_ST(wxDIRCTRL_SHOW_FILTERS)
#endif
        WXS_ST(wxDIRCTRL_EDIT_LABELS)
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsGenericDirCtrlEvents)
        WXS_EVI(EVT_CMD_DIRCTRL_SELECTIONCHANGED,wxEVT_DIRCTRL_SELECTIONCHANGED,wxTreeEvent,SelectionChanged)  // added in 3.0
        WXS_EVI(EVT_CMD_DIRCTRL_FILEACTIVATED,wxEVT_DIRCTRL_FILEACTIVATED,wxTreeEvent,FileActivated)           // added in 3.0
    WXS_EV_END()
}

wxsGenericDirCtrl::wxsGenericDirCtrl(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsGenericDirCtrlEvents,
        wxsGenericDirCtrlStyles),
    DefaultFilter(0)
{}


void wxsGenericDirCtrl::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/dirctrl.h>"),GetInfo().ClassName,0);
            Codef(_T("%C(%W, %I, %n, %P, %S, %T, %n, %d, %N);\n"),DefaultFolder.wx_str(),Filter.wx_str(),DefaultFilter);
            BuildSetupWindowCode();
            return;
        }

        case wxsUnknownLanguage: // fall-through
        default:
        {
            wxsCodeMarks::Unknown(_T("wxsGenericDirCtrl::OnBuildCreatingCode"),GetLanguage());
        }
    }
}

wxObject* wxsGenericDirCtrl::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxGenericDirCtrl* Preview = new wxGenericDirCtrl(Parent,GetId(),DefaultFolder,Pos(Parent),Size(Parent),Style(),Filter,DefaultFilter);
    return SetupWindow(Preview,Flags);
}

void wxsGenericDirCtrl::OnEnumWidgetProperties(cb_unused long Flags)
{
    WXS_SHORT_STRING(wxsGenericDirCtrl,DefaultFolder,_("Default Folder"),_T("defaultfolder"),_T(""),true)
    WXS_SHORT_STRING(wxsGenericDirCtrl,Filter,_("Filter"),_T("filter"),_T(""),true)
    WXS_LONG(wxsGenericDirCtrl,DefaultFilter,_("Default Filter"),_T("defaultfilter"),0)
}
