/*
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

#include "wxsstatictext.h"

namespace
{
    wxsRegisterItem<wxsStaticText> Reg(_T("StaticText"),wxsTWidget,_T("Standard"),50);

    WXS_ST_BEGIN(wxsStaticTextStyles,_T(""))
        WXS_ST_CATEGORY("wxStaticText")
        WXS_ST(wxST_NO_AUTORESIZE)
        WXS_ST(wxALIGN_LEFT)
        WXS_ST(wxALIGN_RIGHT)
        WXS_ST(wxALIGN_CENTRE)
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsStaticTextEvents)
    WXS_EV_END()

}

wxsStaticText::wxsStaticText(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsStaticTextEvents,
        wxsStaticTextStyles),
    Label(_("Label"))

{}


void wxsStaticText::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/stattext.h>"),GetInfo().ClassName,hfInPCH);
            Codef(_T("%C(%W, %I, %t, %P, %S, %T, %N);\n"),Label.wx_str());
            BuildSetupWindowCode();
            return;
        }

        case wxsUnknownLanguage: // fall-through
        default:
        {
            wxsCodeMarks::Unknown(_T("wxsStaticText::OnBuildCreatingCode"),GetLanguage());
        }
    }
}


wxObject* wxsStaticText::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxStaticText* Preview = new wxStaticText(Parent,GetId(),Label,Pos(Parent),Size(Parent),Style());
    return SetupWindow(Preview,Flags);
}


void wxsStaticText::OnEnumWidgetProperties(cb_unused long Flags)
{
    WXS_STRING(wxsStaticText,Label,_("Label"),_T("label"),_T(""),true)
}
