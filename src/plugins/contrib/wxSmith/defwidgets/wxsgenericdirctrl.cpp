#include "../wxsheaders.h"
#include "wxsgenericdirctrl.h"

#include <wx/dirctrl.h>

WXS_ST_BEGIN(wxsGenericDirCtrlStyles)
    WXS_ST_CATEGORY("wxGenericDirCtrl")
    WXS_ST(wxDIRCTRL_DIR_ONLY)
    WXS_ST(wxDIRCTRL_3D_INTERNAL)
    WXS_ST(wxDIRCTRL_SELECT_FIRST)
    WXS_ST(wxDIRCTRL_SHOW_FILTERS)
    WXS_ST_MASK(wxDIRCTRL_EDIT_LABELS,wxsSFAll,0,false)
WXS_ST_END(wxsGenericDirCtrlStyles)

WXS_EV_BEGIN(wxsGenericDirCtrlEvents)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsGenericDirCtrlEvents)

wxsDWDefineBegin(wxsGenericDirCtrl,wxGenericDirCtrl,
    WXS_THIS = new wxGenericDirCtrl(
        WXS_PARENT,
        WXS_ID,
        defaultfolder,
        WXS_POS,WXS_SIZE,
        WXS_STYLE,
        filter,
        defaultfilter,
        WXS_NAME);
    )
    wxsDWDefStr(defaultfolder,"Default folder:","");
    wxsDWDefStr(filter,"Filter:","");
    wxsDWDefInt(defaultfilter,"Default filter:",0);
wxsDWDefineEnd()
