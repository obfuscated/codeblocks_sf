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

#include "wxsstopwatch.h"
#include "../wxsitemresdata.h"
#include <wx/stopwatch.h>

//------------------------------------------------------------------------------

namespace
{

    wxsRegisterItem<wxsStopWatch> Reg(
        _T("StopWatch"),                // Class base name
        wxsTTool,                       // Item type
        _T("Tools"),                    // Category in palette
        30,                             // Priority in palette
        false);                         // We do not allow this item inside XRC files



}

//------------------------------------------------------------------------------

wxsStopWatch::wxsStopWatch(wxsItemResData* Data):
    wxsTool(
        Data,
        &Reg.Info,
        0,
        0)
{
}

//------------------------------------------------------------------------------

void wxsStopWatch::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
            AddHeader(_T("<wx/stopwatch.h>"),GetInfo().ClassName, 0);
            BuildSetupWindowCode();
            break;

        case wxsUnknownLanguage: // fall-through
        default:
            wxsCodeMarks::Unknown(_T("wxsStopWatch::OnBuildCreatingCode"),GetLanguage());
    }
}

//------------------------------------------------------------------------------

void wxsStopWatch::OnEnumToolProperties(cb_unused long Flags)
{
}
