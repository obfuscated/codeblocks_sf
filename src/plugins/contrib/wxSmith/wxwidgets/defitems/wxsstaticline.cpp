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

#include "wxsstaticline.h"

#include <wx/statline.h>

namespace
{
    wxsRegisterItem<wxsStaticLine> Reg(_T("StaticLine"),wxsTWidget,_T("Standard"),60);

    WXS_ST_BEGIN(wxsStaticLineStyles,_T("wxLI_HORIZONTAL"))
        WXS_ST_CATEGORY("wxStaticLine")
        WXS_ST(wxLI_HORIZONTAL)
        WXS_ST(wxLI_VERTICAL)
        WXS_ST_DEFAULTS()
    WXS_ST_END()


    WXS_EV_BEGIN(wxsStaticLineEvents)
    WXS_EV_END()
}

wxsStaticLine::wxsStaticLine(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsStaticLineEvents,
        wxsStaticLineStyles)
{
    // Default the size so that it can be seen in the edit mode
    GetBaseProps()->m_Size.IsDefault = false;
    GetBaseProps()->m_Size.X = 10;
    GetBaseProps()->m_Size.Y = -1;
}

void wxsStaticLine::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/statline.h>"),GetInfo().ClassName,0);
            Codef(_T("%C(%W, %I, %P, %S, %T, %N);\n"));
            BuildSetupWindowCode();
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsStaticLine::OnBuildCreatingCode"),GetLanguage());
        }
    }
}


wxObject* wxsStaticLine::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxStaticLine* Preview = new wxStaticLine(Parent,GetId(),Pos(Parent),Size(Parent),Style());
    return SetupWindow(Preview,Flags);
}

void wxsStaticLine::OnEnumWidgetProperties(long Flags)
{
}
