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

#include "wxstextctrl.h"

namespace
{
    wxsRegisterItem<wxsTextCtrl> Reg(_T("TextCtrl"),wxsTWidget,_T("Standard"),75);

    WXS_ST_BEGIN(wxsTextCtrlStyles,_T(""))
        WXS_ST(wxTE_NO_VSCROLL)
        WXS_ST(wxTE_AUTO_SCROLL)
        WXS_ST(wxTE_PROCESS_ENTER)
        WXS_ST(wxTE_PROCESS_TAB)
        WXS_ST(wxTE_MULTILINE)
        WXS_ST(wxTE_PASSWORD)
        WXS_ST(wxTE_READONLY)
        WXS_ST(wxHSCROLL)
        WXS_ST(wxTE_RICH)
        WXS_ST(wxTE_RICH2)
        WXS_ST(wxTE_AUTO_URL)
        WXS_ST(wxTE_NOHIDESEL)
        WXS_ST(wxTE_LEFT)
        WXS_ST(wxTE_CENTRE)
        WXS_ST(wxTE_RIGHT)
        WXS_ST(wxTE_DONTWRAP)
        WXS_ST(wxTE_CHARWRAP)
        WXS_ST(wxTE_WORDWRAP)
        WXS_ST_DEFAULTS()
    WXS_ST_END()



    WXS_EV_BEGIN(wxsTextCtrlEvents)
        WXS_EVI(EVT_TEXT,wxEVT_COMMAND_TEXT_UPDATED,wxCommandEvent,Text)
        WXS_EVI(EVT_TEXT_ENTER,wxEVT_COMMAND_TEXT_ENTER,wxCommandEvent,TextEnter)
        WXS_EVI(EVT_TEXT_URL,wxEVT_COMMAND_TEXT_URL,wxTextUrlEvent,TextUrl)
        WXS_EVI(EVT_TEXT_MAXLEN,wxEVT_COMMAND_TEXT_MAXLEN,wxCommandEvent,TextMaxLen)
    WXS_EV_END()

}

wxsTextCtrl::wxsTextCtrl(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsTextCtrlEvents,
        wxsTextCtrlStyles),
    Text(_("Text")),
    MaxLength(0)
{}


void wxsTextCtrl::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code << Codef(Language,_T("%C(%W, %I, %t, %P, %S, %T, %V, %N);\n"),Text.c_str());
            if ( MaxLength > 0 ) Code << Codef(Language,_T("%ASetMaxLength(%d);\n"),MaxLength);
            SetupWindowCode(Code,WindowParent,Language);
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsTextCtrl::OnBuildCreatingCode"),Language);
        }
    }
}


wxObject* wxsTextCtrl::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxTextCtrl* Preview = new wxTextCtrl(Parent,GetId(),Text,Pos(Parent),Size(Parent),Style());
    return SetupWindow(Preview,Flags);
}


void wxsTextCtrl::OnEnumWidgetProperties(long Flags)
{
    WXS_STRING(wxsTextCtrl,Text,_("Text"),_T("value"),_T(""),false)
    WXS_LONG(wxsTextCtrl,MaxLength,_("Max Length"),_T("maxlength"),0)
}

void wxsTextCtrl::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/textctrl.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsTextCtrl::EnumDeclFiles"),Language);
    }
}
