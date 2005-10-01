#include "wxsstaticline.h"

#include "wx/statline.h"

WXS_ST_BEGIN(wxsStaticLineStyles)
    WXS_ST_CATEGORY("wxStaticLine")
    WXS_ST(wxLI_HORIZONTAL)
    WXS_ST(wxLI_VERTICAL)
WXS_ST_END(wxsStaticLineStyles)

WXS_EV_BEGIN(wxsStaticLineEvents)
//    WXS_EVI(EVT_BUTTON,wxCommandEvent,Click)
//    WXS_EV_DEFAULTS()
WXS_EV_END(wxsStaticLineEvents)


wxsDWDefineBegin(wxsStaticLine,wxStaticLine,
   //  size.Set(10,2);
     ThisWidget = new wxStaticLine(parent,id,pos,size,style);
    //if (def) ThisWidget->SetDefault();
    
    )

 //   wxsDWDefLongStr(label,"Label:","Label");
 //   wxsDWDefBoolX(def,"default","Default:",false);

wxsDWDefineEnd()
