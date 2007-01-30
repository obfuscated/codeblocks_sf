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

#include "wxsslider.h"


// TODO: Add some flag like "Using Selection range" to note when using selmin / selmax (current comparision to 0 isn't enough)
namespace
{
    wxsRegisterItem<wxsSlider> Reg(_T("Slider"),wxsTWidget,_T("Standard"),50);

    WXS_ST_BEGIN(wxsSliderStyles,_T(""))
        WXS_ST_CATEGORY("wxSlider")
        WXS_ST(wxSL_HORIZONTAL)
        WXS_ST(wxSL_VERTICAL)
        WXS_ST(wxSL_AUTOTICKS)
        WXS_ST(wxSL_LABELS)
        WXS_ST(wxSL_LEFT)
        WXS_ST(wxSL_TOP)
        WXS_ST(wxSL_RIGHT)
        WXS_ST(wxSL_BOTTOM)
        WXS_ST(wxSL_BOTH)
        WXS_ST_MASK(wxSL_SELRANGE,wxsSFWin,0,true)
        WXS_ST(wxSL_INVERSE)
        WXS_ST_CATEGORY("wxWindow")
        WXS_ST_MASK(wxCLIP_CHILDREN,wxsSFWin,0,true)
        WXS_ST(wxSIMPLE_BORDER)
        WXS_ST(wxSUNKEN_BORDER)
        WXS_ST_MASK(wxDOUBLE_BORDER,wxsSFWin|wxsSFOSX|wxsSFCOCOA|wxsSFCARBON,0,true)
        WXS_ST(wxRAISED_BORDER)
        WXS_ST_MASK(wxSTATIC_BORDER,wxsSFWin,0,true)
        WXS_ST(wxNO_BORDER)
        WXS_ST_MASK(wxTRANSPARENT_WINDOW,wxsSFWin,0,true)
        WXS_ST(wxWANTS_CHARS)
        WXS_ST(wxTAB_TRAVERSAL)
        WXS_ST(wxNO_FULL_REPAINT_ON_RESIZE)
        WXS_ST(wxFULL_REPAINT_ON_RESIZE)
        WXS_EXST(wxWS_EX_BLOCK_EVENTS)
        WXS_EXST(wxWS_EX_VALIDATE_RECURSIVELY)
    WXS_ST_END()


    WXS_EV_BEGIN(wxsSliderEvents)
        WXS_EVI(EVT_COMMAND_SCROLL,wxEVT_SCROLL_TOP|wxEVT_SCROLL_BOTTOM|wxEVT_SCROLL_LINEUP|wxEVT_SCROLL_LINEDOWN|wxEVT_SCROLL_PAGEUP|wxEVT_SCROLL_PAGEDOWN|wxEVT_SCROLL_THUMBTRACK|wxEVT_SCROLL_THUMBRELEASE|wxEVT_SCROLL_CHANGED,wxScrollEvent,CmdScroll)
        WXS_EVI(EVT_COMMAND_SCROLL_TOP,wxEVT_SCROLL_TOP,wxScrollEvent,ScrollTop)
        WXS_EVI(EVT_COMMAND_SCROLL_BOTTOM,wxEVT_SCROLL_BOTTOM,wxScrollEvent,CmdScrollBottom)
        WXS_EVI(EVT_COMMAND_SCROLL_LINEUP,wxEVT_SCROLL_LINEUP,wxScrollEvent,CmdScrollLineUp)
        WXS_EVI(EVT_COMMAND_SCROLL_LINEDOWN,wxEVT_SCROLL_LINEDOWN,wxScrollEvent,CmdScrollLineDown)
        WXS_EVI(EVT_COMMAND_SCROLL_PAGEUP,wxEVT_SCROL_PAGEUP,wxScrollEvent,CmdScrollPageUp)
        WXS_EVI(EVT_COMMAND_SCROLL_PAGEDOWN,wxEVT_SCROLL_PAGEDOWN,wxScrollEvent,CmdScrollPageDown)
        WXS_EVI(EVT_COMMAND_SCROLL_THUMBTRACK,wxEVT_SCROLL_THUMBTRACK,wxScrollEvent,CmdScrollThumbTrack)
        WXS_EVI(EVT_COMMAND_SCROLL_THUMBRELEASE,wxEVT_SCROLL_THUMBRELEASE,wxScrollEvent,CmdScrollThumbRelease)

        // Only in WXMSW
        WXS_EVI(EVT_COMMAND_SCROLL_CHANGED,wxEVT_SCROLL_CHANGED,wxScrollEvent,CmdScrollChanged)
        WXS_EV_DEFAULTS()

    WXS_EV_END()

}


wxsSlider::wxsSlider(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsSliderEvents,
        wxsSliderStyles),
    Value(0),
    Min(0),
    Max(100),
    TickFrequency(0),
    PageSize(0),
    LineSize(0),
    ThumbLength(0),
    Tick(0),
    SelMin(0),
    SelMax(0)
{}

void wxsSlider::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code << Codef(Language,_T("%C(%W,%I,%d,%d,%d,%P,%S,%T,%V,%N);\n"),Value,Min,Max);

            if ( TickFrequency )    Code << Codef(Language,_T("%ASetTickFreq(%d);\n"),TickFrequency);
            if ( PageSize )         Code << Codef(Language,_T("%ASetPageSize(%d);\n"),PageSize);
            if ( LineSize )         Code << Codef(Language,_T("%ASetLineSize(%d);\n"),LineSize);
            if ( ThumbLength )      Code << Codef(Language,_T("%ASetThumbLength(%d);\n"),ThumbLength);
            if ( Tick )             Code << Codef(Language,_T("%ASetTick(%d);\n"),Tick);
            if ( SelMin || SelMax ) Code << Codef(Language,_T("%ASetSelection(%d,%d);\n"),SelMin,SelMax);
            SetupWindowCode(Code,Language);
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsSlider::OnBuildCreatingCode"),Language);
        }
    }
}

wxObject* wxsSlider::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxSlider* Preview = new wxSlider(Parent,GetId(),Value,Min,Max,Pos(Parent),Size(Parent),Style());
    if ( TickFrequency )    Preview->SetTickFreq(TickFrequency,0);
    if ( PageSize )         Preview->SetPageSize(PageSize);
    if ( LineSize )         Preview->SetLineSize(LineSize);
    if ( ThumbLength )      Preview->SetThumbLength(ThumbLength);
    if ( Tick )             Preview->SetTick(Tick);
    if ( SelMin || SelMax ) Preview->SetSelection(SelMin,SelMax);
    return SetupWindow(Preview,Flags);
}

void wxsSlider::OnEnumWidgetProperties(long Flags)
{
   WXS_LONG(wxsSlider,Value,0,_("Value"),_T("value"),0)
   WXS_LONG(wxsSlider,Min,0,_("Min"),_T("min"),0)
   WXS_LONG(wxsSlider,Max,0,_("Max"),_T("max"),100)
   WXS_LONG(wxsSlider,TickFrequency,0,_("Tick Frequency"),_T("tickfreq"),0)
   WXS_LONG(wxsSlider,PageSize,0,_("Page Size"),_T("pagesize"),0)
   WXS_LONG(wxsSlider,LineSize,0,_("Line Size"),_T("linesize"),0)
   WXS_LONG(wxsSlider,ThumbLength,0,_("Thumb Length"),_T("thumb"),0)
   WXS_LONG(wxsSlider,Tick,0,_("Tick"),_T("tick"),0)
   WXS_LONG(wxsSlider,SelMin,0,_("Selection Min"),_T("selmin"),0)
   WXS_LONG(wxsSlider,SelMax,0,_("Selection Max"),_T("selmax"),0)
}

void wxsSlider::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/slider.h>")); return;
        default: wxsCodeMarks::Unknown(_T("wxsSlider::OnEnumDeclFiles"),Language);
    }
}
