#include "../wxsheaders.h"
#include "wxstextctrl.h"

WXS_ST_BEGIN(wxsTextCtrlStyles)
    WXS_ST_CATEGORY("wxTextCtrl")
    WXS_ST(wxTE_NO_VSCROLL)
    WXS_ST(wxTE_AUTO_SCROLL)
    WXS_ST(wxTE_PROCESS_ENTER)
    WXS_ST(wxTE_PROCESS_TAB)
    WXS_ST(wxTE_MULTILINE)
    WXS_ST(wxTE_PASSWORD)
    WXS_ST(wxTE_READONLY)
    WXS_ST(wxHSCROLL)
    WXS_ST(wxTE_RICH)
    WXS_ST(wxTE_RICH2)
    WXS_ST(wxTE_NOHIDESEL)
    WXS_ST(wxTE_LEFT)
#ifdef __WXMSW__
    WXS_ST(wxTE_AUTO_URL)
#endif
#ifdef __WXGTK20__
    WXS_ST(wxTE_AUTO_URL)
#endif

#ifdef __WXMSW__
    WXS_ST(wxTE_CENTRE)
#endif
#ifdef __WXGTK20__
    WXS_ST(wxTE_CENTRE)
#endif

#ifdef __WXMSW__
    WXS_ST(wxTE_RIGHT)
#endif
#ifdef __WXGTK20__
    WXS_ST(wxTE_RIGHT)
#endif

#ifdef __WXGTK20__
    WXS_ST(wxTE_CHARWRAP)
#endif
#ifdef ____WXUNIVERSAL__
    WXS_ST(wxTE_CHARWRAP)
#endif

#ifdef __WXGTK20__
    WXS_ST(wxTE_WORDWRAP)
#endif
#ifdef ____WXUNIVERSAL__
    WXS_ST(wxTE_WORDWRAP)
#endif

    WXS_ST(wxTE_DONTWRAP)
    WXS_ST(wxTE_LINEWRAP)
WXS_ST_END(wxsTextCtrlStyles)

WXS_EV_BEGIN(wxsTextCtrlEvents)
    WXS_EVI(EVT_TEXT,wxCommandEvent,Text)
    WXS_EVI(EVT_TEXT_ENTER,wxCommandEvent,TextEnter)
    WXS_EVI(EVT_TEXT_URL,wxTextUrlEvent,TextUrl)
    WXS_EVI(EVT_TEXT_MAXLEN,wxCommandEvent,TextMaxLen)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsTextCtrlEvents)

wxsDWDefineBegin(wxsTextCtrl,wxTextCtrl,
    ThisWidget = new wxTextCtrl(parent,id,value,pos,size,style);
    if ( maxlength ) ThisWidget->SetMaxLength(maxlength);
    )
    wxsDWDefLongStr(value,"Value:","Text");
    wxsDWDefInt(maxlength,"Max length:",0);
wxsDWDefineEnd()

