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

#include "wxsgenericdirctrl.h"

#include <wx/dirctrl.h>

namespace
{
    wxsRegisterItem<wxsGenericDirCtrl> Reg(_T("GenericDirCtrl"),wxsTWidget,_T("Standard"),30);

    WXS_ST_BEGIN(wxsGenericDirCtrlStyles,_T(""))
        WXS_ST_CATEGORY("wxGenericDirCtrl")
        WXS_ST(wxDIRCTRL_DIR_ONLY)
        WXS_ST(wxDIRCTRL_3D_INTERNAL)
        WXS_ST(wxDIRCTRL_SELECT_FIRST)
        WXS_ST(wxDIRCTRL_SHOW_FILTERS)
        WXS_ST_MASK(wxDIRCTRL_EDIT_LABELS,wxsSFAll,0,false)
    WXS_ST_END()


    WXS_EV_BEGIN(wxsGenericDirCtrlEvents)
        WXS_EV_DEFAULTS()
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


void wxsGenericDirCtrl::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            if ( GetParent() )
            {
                Code<< GetVarName() << _T(" = new wxGenericDirCtrl(");
            }
            else
            {
                Code<< _T("Create(");
            }
            Code<< WindowParent << _T(",")
                << GetIdName() << _T(",")
                << wxsCodeMarks::WxString(wxsCPP,DefaultFolder) << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(",")
                << wxsCodeMarks::WxString(wxsCPP,Filter) << _T(",")
                << wxString::Format(_T("%d"),DefaultFilter) << _T(",")
                << wxsCodeMarks::WxString(wxsCPP,GetIdName(),false) << _T(");\n");

            SetupWindowCode(Code,Language);
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsGenericDirCtrl::OnBuildCreatingCode"),Language);
        }
    }
}

wxObject* wxsGenericDirCtrl::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxGenericDirCtrl* Preview = new wxGenericDirCtrl(Parent,GetId(),DefaultFolder,Pos(Parent),Size(Parent),Style(),Filter,DefaultFilter);
    return SetupWindow(Preview,Flags);
}

void wxsGenericDirCtrl::OnEnumWidgetProperties(long Flags)
{
    WXS_STRING(wxsGenericDirCtrl,DefaultFolder,0,_("Default Folder"),_T("defaultfolder"),_T(""),true,false)
    WXS_STRING(wxsGenericDirCtrl,Filter,0,_("Filter"),_T("filter"),_T(""),true,false)
    WXS_LONG(wxsGenericDirCtrl,DefaultFilter,0,_("Default Filter"),_T("defaultfilter"),0)
}

void wxsGenericDirCtrl::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/dirctrl.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsGenericDirCtrl::OnEnumDeclFiles"),Language);
    }
}
