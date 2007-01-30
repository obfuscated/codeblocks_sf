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

#include "wxsscrollbar.h"

namespace
{
    wxsRegisterItem<wxsScrollBar> Reg(_T("ScrollBar"),wxsTWidget,_T("Standard"),50);

    WXS_ST_BEGIN(wxsScrollBarStyles,_T(""))
        WXS_ST_CATEGORY("wxScrollBar")
        WXS_ST(wxSB_HORIZONTAL)
        WXS_ST(wxSB_VERTICAL)
    WXS_ST_END()


    WXS_EV_BEGIN(wxsScrollBarEvents)
    /* Extract from the Help
      To process a scroll event, use these event handler macros to direct input
      to member functions that take a wxScrollEvent argument. You can use EVT_COMMAND_SCROLL... macros with window IDs for when intercepting scroll events from controls, or EVT_SCROLL... macros without window IDs for intercepting scroll events from the receiving window -- except for this, the macros behave exactly the same.
    */
        WXS_EVI(EVT_COMMAND_SCROLL,wxEVT_SCROLL_TOP|wxEVT_SCROLL_BOTTOM|wxEVT_SCROLL_LINEUP|wxEVT_SCROLL_LINEDOWN|wxEVT_SCROLL_PAGEUP|wxEVT_SCROLL_PAGEDOWN|wxEVT_SCROLL_THUMBTRACK|wxEVT_SCROLL_THUMBRELEASE|wxEVT_SCROLL_CHANGED,wxScrollEvent,Scroll)
        WXS_EVI(EVT_COMMAND_SCROLL_TOP,wxEVT_SCROLL_TOP,wxScrollEvent,ScrollTop)
        WXS_EVI(EVT_COMMAND_SCROLL_BOTTOM,wxEVT_SCROLL_BOTTOM,wxScrollEvent,ScrollBottom)
        WXS_EVI(EVT_COMMAND_SCROLL_LINEUP,wxEVT_SCROLL_LINEUP,wxScrollEvent,ScrollLineUp)
        WXS_EVI(EVT_COMMAND_SCROLL_LINEDOWN,wxEVT_SCROLL_LINEDOWN,wxScrollEvent,ScrollLineDown)
        WXS_EVI(EVT_COMMAND_SCROLL_PAGEUP,wxEVT_SCROL_PAGEUP,wxScrollEvent,ScrollPageUp)
        WXS_EVI(EVT_COMMAND_SCROLL_PAGEDOWN,wxEVT_SCROLL_PAGEDOWN,wxScrollEvent,ScrollPageDown)
        WXS_EVI(EVT_COMMAND_SCROLL_THUMBTRACK,wxEVT_SCROLL_THUMBTRACK,wxScrollEvent,ScrollThumbTrack)
        WXS_EVI(EVT_COMMAND_SCROLL_THUMBRELEASE,wxEVT_SCROLL_THUMBRELEASE,wxScrollEvent,ScrollThumbRelease)

        // Only in WXMSW
        WXS_EVI(EVT_COMMAND_SCROLL_CHANGED,wxEVT_SCROLL_CHANGED,wxScrollEvent,ScrollChanged)

        WXS_EV_DEFAULTS()

    WXS_EV_END()
}

wxsScrollBar::wxsScrollBar(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsScrollBarEvents,
        wxsScrollBarStyles),
    Value(0),
    ThumbSize(1),
    Range(10),
    PageSize(1)
{}


void wxsScrollBar::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code << Codef(Language,_T("%C(%W,%I,%P,%S,%T,%V,%N);\n"));
            Code << Codef(Language,_T("%ASetScrollbar(%d,%d,%d,%d);\n"),Value,ThumbSize,Range,PageSize);
            SetupWindowCode(Code,Language);
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsScrollBar::OnBuildCreatingCode"),Language);
        }
    }
}


wxObject* wxsScrollBar::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxScrollBar* Preview = new wxScrollBar(Parent,GetId(),Pos(Parent),Size(Parent),Style());
    Preview->SetScrollbar(Value,ThumbSize,Range,PageSize);
    return SetupWindow(Preview,Flags);
}


void wxsScrollBar::OnEnumWidgetProperties(long Flags)
{
   WXS_LONG(wxsScrollBar,Value,0,_("Value"),_T("value"),0)
   WXS_LONG(wxsScrollBar,ThumbSize,0,_("ThumbSize"),_T("thumbsize"),0)
   WXS_LONG(wxsScrollBar,Range,0,_("Range"),_T("range"),0)
   WXS_LONG(wxsScrollBar,PageSize,0,_("PageSize"),_T("pagesize"),0)
}

void wxsScrollBar::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/scrolbar.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsScrollBar::OnEnumDeclFiles"),Language);
    }
}
