#include "../wxsheaders.h"
#include "wxsslider.h"

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
WXS_ST_END(wxsSliderStyles)

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
    WXS_EVI(EVT_COMMAND_SCROLL,wxScrollEvent,CmdScroll)
    WXS_EVI(EVT_COMMAND_SCROLL_TOP,wxScrollEvent,CmdScrollTop)
    WXS_EVI(EVT_COMMAND_SCROLL_BOTTOM,wxScrollEvent,CmdScrollBottom)
    WXS_EVI(EVT_COMMAND_SCROLL_LINEUP,wxScrollEvent,CmdScrollLineUp)
    WXS_EVI(EVT_COMMAND_SCROLL_LINEDOWN,wxScrollEvent,CmdScrollLineDown)
    WXS_EVI(EVT_COMMAND_SCROLL_PAGEUP,wxScrollEvent,CmdScrollPageUp)
    WXS_EVI(EVT_COMMAND_SCROLL_PAGEDOWN,wxScrollEvent,CmdScrollPageDown)
    WXS_EVI(EVT_COMMAND_SCROLL_THUMBTRACK,wxScrollEvent,CmdScrollThumbTrack)
    WXS_EVI(EVT_COMMAND_SCROLL_THUMBRELEASE,wxScrollEvent,CmdScrollThumbRelease)
    WXS_EVI(EVT_COMMAND_SCROLL_CHANGED,wxScrollEvent,CmdScrollChanged)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsSliderEvents)

wxsDWDefineBegin(wxsSlider,wxSlider,
    WXS_THIS = new wxSlider(WXS_PARENT,WXS_ID,value,min,max,WXS_POS,WXS_SIZE,WXS_STYLE,wxDefaultValidator,WXS_NAME);
    if ( tickfreq ) WXS_THIS->SetTickFreq(tickfreq,0);
    if ( pagesize ) WXS_THIS->SetPageSize(pagesize);
    if ( linesize ) WXS_THIS->SetLineSize(linesize);
    if ( thumb    ) WXS_THIS->SetThumbLength(thumb);
    if ( tick     ) WXS_THIS->SetTick(tick);
    if ( selmin || selmax ) WXS_THIS->SetSelection(selmin,selmax);
    )
    wxsDWDefInt(value,"Value:",0);
    wxsDWDefInt(min,"Min value:",0);
    wxsDWDefInt(max,"Max value:",100);
    wxsDWDefInt(pagesize,"Page size:",0);
    wxsDWDefInt(linesize,"Line size:",0);
    wxsDWDefInt(thumb,"Thumb length:",0);
    wxsDWDefInt(selmin,"Sel. min:",0);
    wxsDWDefInt(selmax,"Sel. max:",0);
    wxsDWDefInt(tick,"Tick:",0)
    wxsDWDefInt(tickfreq,"Tick frequency:",0);
wxsDWDefineEnd()
