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
//Help file : No effect under GTK1
    WXS_ST(wxHSCROLL)
    WXS_ST(wxTE_RICH)
    WXS_ST(wxTE_RICH2)
    WXS_ST(wxTE_NOHIDESEL)
    WXS_ST(wxTE_LEFT)
    WXS_ST_MASK(wxTE_AUTO_URL,wxsSFWin|wxsSFGTK20,0,true)
    WXS_ST_MASK(wxTE_CENTRE,wxsSFWin|wxsSFGTK20,0,true)
    WXS_ST_MASK(wxTE_RIGHT,wxsSFWin|wxsSFGTK20,0,true)
    WXS_ST_MASK(wxTE_CENTRE,wxsSFWin|wxsSFGTK20,0,true)
    WXS_ST_MASK(wxTE_CHARWRAP,wxsSFUNIV|wxsSFGTK20,0,true)
    WXS_ST_MASK(wxTE_WORDWRAP,wxsSFUNIV|wxsSFGTK20,0,true)
    WXS_ST(wxTE_BESTWRAP)

// Help file :On PocketPC and Smartphone, causes the first letter to be capitalized
    WXS_ST_MASK(wxTE_CAPITALIZE,wxsSFWinCE,0,true)
// Help file : same as wxTE_HSCROLL, so ignore
//    WXS_ST(wxTE_DONTWRAP)

// Not in Help file but in XRC file, assume applying to all platform
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
    WXS_THIS = new wxTextCtrl(WXS_PARENT,WXS_ID,value,WXS_POS,WXS_SIZE,WXS_STYLE,wxDefaultValidator,WXS_NAME);
    if ( maxlength ) WXS_THIS->SetMaxLength(maxlength);
    )
    wxsDWDefLongStr(value,"Value:","Text");
    wxsDWDefInt(maxlength,"Max length:",0);
wxsDWDefineEnd()

