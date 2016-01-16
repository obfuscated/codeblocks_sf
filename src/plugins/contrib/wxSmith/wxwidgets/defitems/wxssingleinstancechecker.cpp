/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2008 Ron Collins
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
*/

#include "wxssingleinstancechecker.h"
#include "../wxsitemresdata.h"
#include <wx/snglinst.h>

//------------------------------------------------------------------------------

namespace
{
    wxsRegisterItem<wxsSingleInstanceChecker> Reg(
        _T("SingleInstanceChecker"),  // Class name
        wxsTTool,                     // Item type
        _T("Tools"),                  // Category in palette
        50,                           // Priority in palette
        false);                       // We do not allow this item inside XRC files
}

//------------------------------------------------------------------------------

wxsSingleInstanceChecker::wxsSingleInstanceChecker(wxsItemResData* Data):
    wxsTool(
        Data,
        &Reg.Info,
        0,
        0)
{
}

//------------------------------------------------------------------------------
// as a "tool", the checker is declared directly in the header file
// however, we need to call Create to give it a unique name for the Mutex

void wxsSingleInstanceChecker::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/utils.h>"),   GetInfo().ClassName, 0);
            AddHeader(_T("<wx/snglinst.h>"),GetInfo().ClassName, 0);

            if ( AppName.IsEmpty() )
            {
                AddHeader(_T("<wx/app.h>"), GetInfo().ClassName, 0);
                Codef( _T("%C(wxTheApp->GetAppName() + _T(\"_\") + wxGetUserId() + _T(\"_Guard\"));\n") );
            }
            else
            {
                Codef(_T("%C(%n + wxGetUserId() + _T(\"_Guard\"));\n"), ( AppName + _T("_") ).wx_str() );
            }
            BuildSetupWindowCode();
            break;
        }

        default:
        {
            wxsCodeMarks::Unknown( _T("wxsSingleInstanceChecker::OnBuildCreatingCode"), GetLanguage() );
        }
    }
}

//------------------------------------------------------------------------------

void wxsSingleInstanceChecker::OnEnumToolProperties(long Flags)
{
    WXS_SHORT_STRING( wxsSingleInstanceChecker, AppName, _("Custom app name"), _T("appname"), wxEmptyString, false );
}
