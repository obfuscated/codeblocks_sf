#include "../wxsheaders.h"
#include "wxsstaticbox.h"

#include "wxsstdmanager.h"
#include "../wxspropertiesman.h"

WXS_ST_BEGIN(wxsStaticBoxStyles)
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
WXS_ST_END(wxsStaticBoxStyles)

WXS_EV_BEGIN(wxsStaticBoxEvents)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsStaticBoxEvents)

wxsDWDefineBegin(wxsStaticBox,wxStaticBox,
    WXS_THIS = new wxStaticBox(WXS_PARENT,WXS_ID,label,WXS_POS,WXS_SIZE,WXS_STYLE,WXS_NAME);
    )
    wxsDWDefLongStr(label,"Label:","Label");
wxsDWDefineEnd()
