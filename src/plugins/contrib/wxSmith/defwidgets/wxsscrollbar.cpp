#include "../wxsheaders.h"
#include "wxsscrollbar.h"

WXS_ST_BEGIN(wxsScrollBarStyles)
    WXS_ST_CATEGORY("wxScrollBar")
    WXS_ST(wxSB_HORIZONTAL)
    WXS_ST(wxSB_VERTICAL)
WXS_ST_END(wxsScrollBarStyles)

WXS_EV_BEGIN(wxsScrollBarEvents)
    WXS_EVI(EVT_COMMAND_SCROLL,wxScrollEvent,Scroll)
    WXS_EVI(EVT_COMMAND_SCROLL_TOP,wxScrollEvent,ScrollTop)
    WXS_EVI(EVT_COMMAND_SCROLL_BOTTOM,wxScrollEvent,ScrollBottom)
    WXS_EVI(EVT_COMMAND_SCROLL_LINEUP,wxScrollEvent,ScrollLineUp)
    WXS_EVI(EVT_COMMAND_SCROLL_LINEDOWN,wxScrollEvent,ScrollLineDown)
    WXS_EVI(EVT_COMMAND_SCROLL_PAGEUP,wxScrollEvent,ScrollPageUp)
    WXS_EVI(EVT_COMMAND_SCROLL_PAGEDOWN,wxScrollEvent,ScrollPageDown)
    WXS_EVI(EVT_COMMAND_SCROLL_THUMBTRACK,wxScrollEvent,ScrollThumbTrack)
    WXS_EVI(EVT_COMMAND_SCROLL_THUMBRELEASE,wxScrollEvent,ScrollThumbRelease)
    WXS_EVI(EVT_COMMAND_SCROLL_CHANGED,wxScrollEvent,ScrollChanged)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsScrollBarEvents)

// TODO (SpOoN#1#): Find out if scroll events can be useed for all widgets by default



//wxScrollBar(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxSB_HORIZONTAL, const wxValidator& validator = wxDefaultValidator, const wxString& name = "scrollBar")

wxsDWDefineBegin(wxsScrollBar,wxScrollBar,

    WXS_THIS = new wxScrollBar(WXS_PARENT,WXS_ID,WXS_POS,WXS_SIZE,WXS_STYLE,wxDefaultValidator,WXS_NAME);
    WXS_THIS->SetScrollbar(value,thumbsz,range,pagesz);
    )
    wxsDWDefInt(value,"Value:",0);
    wxsDWDefIntX(thumbsz,"thumbsize","Thumb size:",1);
    wxsDWDefInt(range,"Range:",10);
    wxsDWDefIntX(pagesz,"pagesize","Page size:",1);
wxsDWDefineEnd()
