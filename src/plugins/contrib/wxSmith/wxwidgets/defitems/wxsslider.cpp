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

#include <wx/slider.h>
#include "wxsslider.h"


// TODO: Add some flag like "Using Selection range" to note when using selmin / selmax (current comparision to 0 isn't enough)
namespace
{
    wxsRegisterItem<wxsSlider> Reg(_T("Slider"),wxsTWidget,_T("Standard"),110);

    WXS_ST_BEGIN(wxsSliderStyles,_T(""))
        WXS_ST(wxSL_HORIZONTAL)
        WXS_ST(wxSL_VERTICAL)
        WXS_ST(wxSL_AUTOTICKS)
        WXS_ST(wxSL_LABELS)
        WXS_ST(wxSL_LEFT)
        WXS_ST(wxSL_TOP)
        WXS_ST(wxSL_RIGHT)
        WXS_ST(wxSL_BOTTOM)
        WXS_ST(wxSL_BOTH)
        WXS_ST(wxSL_SELRANGE)
        WXS_ST(wxSL_INVERSE)
        WXS_ST_DEFAULTS()
    WXS_ST_END()


    WXS_EV_BEGIN(wxsSliderEvents)
        WXS_EVI(EVT_COMMAND_SCROLL,wxEVT_SCROLL_TOP|wxEVT_SCROLL_BOTTOM|wxEVT_SCROLL_LINEUP|wxEVT_SCROLL_LINEDOWN|wxEVT_SCROLL_PAGEUP|wxEVT_SCROLL_PAGEDOWN|wxEVT_SCROLL_THUMBTRACK|wxEVT_SCROLL_THUMBRELEASE|wxEVT_SCROLL_CHANGED,wxScrollEvent,CmdScroll)
        WXS_EVI(EVT_COMMAND_SCROLL_TOP,wxEVT_SCROLL_TOP,wxScrollEvent,ScrollTop)
        WXS_EVI(EVT_COMMAND_SCROLL_BOTTOM,wxEVT_SCROLL_BOTTOM,wxScrollEvent,CmdScrollBottom)
        WXS_EVI(EVT_COMMAND_SCROLL_LINEUP,wxEVT_SCROLL_LINEUP,wxScrollEvent,CmdScrollLineUp)
        WXS_EVI(EVT_COMMAND_SCROLL_LINEDOWN,wxEVT_SCROLL_LINEDOWN,wxScrollEvent,CmdScrollLineDown)
        WXS_EVI(EVT_COMMAND_SCROLL_PAGEUP,wxEVT_SCROLL_PAGEUP,wxScrollEvent,CmdScrollPageUp)
        WXS_EVI(EVT_COMMAND_SCROLL_PAGEDOWN,wxEVT_SCROLL_PAGEDOWN,wxScrollEvent,CmdScrollPageDown)
        WXS_EVI(EVT_COMMAND_SCROLL_THUMBTRACK,wxEVT_SCROLL_THUMBTRACK,wxScrollEvent,CmdScrollThumbTrack)
        WXS_EVI(EVT_COMMAND_SCROLL_THUMBRELEASE,wxEVT_SCROLL_THUMBRELEASE,wxScrollEvent,CmdScrollThumbRelease)
        WXS_EVI(EVT_COMMAND_SCROLL_CHANGED,wxEVT_SCROLL_CHANGED,wxScrollEvent,CmdScrollChanged)
        WXS_EVI(EVT_COMMAND_SLIDER_UPDATED,wxEVT_COMMAND_SLIDER_UPDATED,wxScrollEvent,CmdSliderUpdated)
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

void wxsSlider::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/slider.h>"),GetInfo().ClassName,hfInPCH);
            Codef(_T("%C(%W, %I, %d, %d, %d, %P, %S, %T, %V, %N);\n"),Value,Min,Max);

            if ( TickFrequency )    Codef( _T("%ASetTickFreq(%d);\n"), TickFrequency);
            if ( PageSize )         Codef( _T("%ASetPageSize(%d);\n"), PageSize);
            if ( LineSize )         Codef( _T("%ASetLineSize(%d);\n"), LineSize);
            if ( ThumbLength )      Codef( _T("%ASetThumbLength(%d);\n"), ThumbLength);
            if ( Tick )             Codef( _T("%ASetTick(%d);\n"), Tick);
            if ( SelMin || SelMax ) Codef( _T("%ASetSelection(%d, %d);\n"), SelMin, SelMax);
            BuildSetupWindowCode();
            return;
        }

        case wxsUnknownLanguage: // fall-through
        default:
        {
            wxsCodeMarks::Unknown(_T("wxsSlider::OnBuildCreatingCode"),GetLanguage());
        }
    }
}

wxObject* wxsSlider::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxSlider* Preview = new wxSlider(Parent,GetId(),Value,Min,Max,Pos(Parent),Size(Parent),Style());
#if wxCHECK_VERSION(2, 9, 0)
    if ( TickFrequency )    Preview->SetTickFreq(TickFrequency);
#else
    if ( TickFrequency )    Preview->SetTickFreq(TickFrequency,0);
#endif
    if ( PageSize )         Preview->SetPageSize(PageSize);
    if ( LineSize )         Preview->SetLineSize(LineSize);
    if ( ThumbLength )      Preview->SetThumbLength(ThumbLength);
    if ( Tick )             Preview->SetTick(Tick);
    if ( SelMin || SelMax ) Preview->SetSelection(SelMin,SelMax);
    return SetupWindow(Preview,Flags);
}

void wxsSlider::OnEnumWidgetProperties(cb_unused long Flags)
{
   WXS_LONG(wxsSlider,Value,_("Value"),_T("value"),0)
   WXS_LONG(wxsSlider,Min,_("Min"),_T("min"),0)
   WXS_LONG(wxsSlider,Max,_("Max"),_T("max"),100)
   WXS_LONG(wxsSlider,TickFrequency,_("Tick Frequency"),_T("tickfreq"),0)
   WXS_LONG(wxsSlider,PageSize,_("Page Size"),_T("pagesize"),0)
   WXS_LONG(wxsSlider,LineSize,_("Line Size"),_T("linesize"),0)
   WXS_LONG(wxsSlider,ThumbLength,_("Thumb Length"),_T("thumb"),0)
   WXS_LONG(wxsSlider,Tick,_("Tick"),_T("tick"),0)
   WXS_LONG(wxsSlider,SelMin,_("Selection Min"),_T("selmin"),0)
   WXS_LONG(wxsSlider,SelMax,_("Selection Max"),_T("selmax"),0)
}
