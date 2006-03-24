#include "wxsslider.h"

#include <wx/slider.h>
#include <messagemanager.h>

/* ************************************************************************** */
/* Styles are processed almost like in older code - using WXS_ST_BEGIN,       */
/* WXS_ST_END, WXS_ST_CATEGORY, WXS_ST ...                                    */
/* ************************************************************************** */

WXS_ST_BEGIN(wxsSliderStyles)
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


/* ************************************************************************** */
/* Events have one additional argument - event type, these can be found       */
/* in WX_DIR/include/event.h                                                  */
/* ************************************************************************** */

WXS_EV_BEGIN(wxsSliderEvents)
//    WXS_EV(EVT_SCROLL,wxScrollEvent,Scroll)
//    WXS_EV(EVT_SCROLL_TOP,wxScrollEvent,ScrollTop)
//    WXS_EV(EVT_SCROLL_BOTTOM,wxScrollEvent,ScrollBottom)
//    WXS_EV(EVT_SCROLL_LINEUP,wxScrollEvent,ScrollLineUp)
//    WXS_EV(EVT_SCROLL_LINEDOWN,wxScrollEvent,ScrollLineDown)
//    WXS_EV(EVT_SCROLL_PAGEUP,wxScrollEvent,ScrollPageUp)
//    WXS_EV(EVT_SCROLL_PAGEDOWN,wxScrollEvent,ScrollPageDown)
//    WXS_EV(EVT_SCROLL_THUMBTRACK,wxScrollEvent,ScrollThumbTrack)
//    WXS_EV(EVT_SCROLL_THUMBRELEASE,wxScrollEvent,ScrollThumbTrackRelease)
//    WXS_EV(EVT_SCROLL_CHANGED,wxScrollEvent,ScrollChanged)
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

/* ************************************************************************** */
/* Widget info                                                                */
/* ************************************************************************** */

wxsItemInfo wxsSlider::Info =
{
    _T("wxSlider"),             // Name of class
    wxsTWidget,                 // Type, always wxsTWidget for widget classes
    _("wxWidgets license"),     // License, any type
    _("wxWidgets team"),        // Author
    _T(""),                     // No default e-mail for standard widgets
    _T("www.wxwidgets.org"),    // Site
    _T("Standard"),             // Groud for widget, note that _T() instead of _() is used
    50,                         // Slider is commonly used widgets - we give it normal priority
    _T("Slider"),               // Standard prefix for variable names and identifiers
    2, 6,                       // Widget version
    NULL,                       // Bitmaps will be loaded later in manager
    NULL,                       // --------------------''-------------------
    0                           // --------------------''-------------------
};


/* ************************************************************************** */
/* Constructur for wxsWidget need style set, event set and widget info, so    */
/* these things should be created before ctor.                                */
/* ************************************************************************** */

wxsSlider::wxsSlider(wxsWindowRes* Resource):
    wxsWidget(
        Resource,                   // Resource is passed to wxsItem's constructor
        wxsBaseProperties::flAll,   // Using all base properties
        &Info,                      // Taking local info
        wxsSliderEvents,            // Pointer to local events
        wxsSliderStyles,            // Pointer to local styles
        _T("")),                     // Default style = 0
        Value(0),                   // Initializing widget with some default (but not necessarily clean) values,
        Min(0),                     // these values will be used when creating new widgets
        Max(100)

{}


/* ************************************************************************** */
/* Function building code - it should append new code generating this widget  */
/* to the end of prevous code. Currently it support only C++ langage but some */
/* other languages may be added in future.                                    */
/* ************************************************************************** */

void wxsSlider::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            // Because c_str() may lead to unspecified behaviour
            // it's better to use << operator instead of wxString::Format.
            // But be carefull when using << to add integers and longs.
            // Because of some wxWidgets bugs use '<< wxString::Format(_T("%d"),Value)'
            // instead of '<< Value'
            Code<< GetVarName() << _T(" = new wxSlider(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(");\n");

            if ( TickFrequency )    Code << GetVarName() << _T("->SetTickFreq(")
                                         << wxString::Format(_T("%d"),TickFrequency) << _T(",0);\n");

            if ( PageSize )         Code << GetVarName() << _T("->SetPageSize(")
                                         << wxString::Format(_T("%d"),PageSize) << _T(");\n");

            if ( LineSize )         Code << GetVarName() << _T("->SetLineSize(")
                                         << wxString::Format(_T("%d"),LineSize) << _T(");\n");

            if ( ThumbLength )      Code << GetVarName() << _T("->SetThumbLength(")
                                         << wxString::Format(_T("%d"),ThumbLength) << _T(");\n");

            if ( Tick )             Code << GetVarName() << _T("->SetTick(")
                                         << wxString::Format(_T("%d"),Tick) << _T(");\n");

            if ( SelMin || SelMax ) Code << GetVarName() << _T("->SetSelection(")
                                         << wxString::Format(_T("%d"),SelMin) << _T(",")
                                         << wxString::Format(_T("%d"),SelMax) << _T(");\n");

            SetupWindowCode(Code,Language);
            return;
        }
    }

    DBGLOG(_T("wxSmith: Unknown coding language when generating slider (id: %d)"),Language);
}

/* ************************************************************************** */
/* Function building preview item. This should simply return previewed object */
/* with all properties set-up.                                                */
/* ************************************************************************** */

wxObject* wxsSlider::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    // Exact argument is not used here because slider preview is
    // exactly the same in editor and in preview window
    wxSlider* Preview = new wxSlider(Parent,GetId(),Value,Min,Max,Pos(Parent),Size(Parent),Style());

    if ( TickFrequency )    Preview->SetTickFreq(TickFrequency,0);
    if ( PageSize )         Preview->SetPageSize(PageSize);
    if ( LineSize )         Preview->SetLineSize(LineSize);
    if ( ThumbLength )      Preview->SetThumbLength(ThumbLength);
    if ( Tick )             Preview->SetTick(Tick);
    if ( SelMin || SelMax ) Preview->SetSelection(SelMin,SelMax);


    return SetupWindow(Preview,Exact);
}


/* ************************************************************************** */
/* Function enumerating all button-specific properties.                       */
/* ************************************************************************** */

void wxsSlider::EnumWidgetProperties(long Flags)
{
   WXS_LONG(wxsSlider,Value,0,_("Value"),_T("Value"),0)
   WXS_LONG(wxsSlider,Min,0,_("Min"),_T("Min"),0)
   WXS_LONG(wxsSlider,Max,0,_("Max"),_T("Max"),100)
   WXS_LONG(wxsSlider,PageSize,0,_("Page Size"),_T("Page Size"),0)
   WXS_LONG(wxsSlider,LineSize,0,_("Line Size"),_T("Line Size"),0)
   WXS_LONG(wxsSlider,ThumbLength,0,_("Thumb Length"),_T("Thumb Length"),0)
   WXS_LONG(wxsSlider,SelMin,0,_("Selection Min"),_T("Selection Min"),0)
   WXS_LONG(wxsSlider,SelMax,0,_("Selection Max"),_T("Selection Max"),0)
   WXS_LONG(wxsSlider,Tick,0,_("Tick"),_T("Tick"),0)
   WXS_LONG(wxsSlider,TickFrequency,0,_("Tick Frequency"),_T("Tick Frequency"),0)

}
