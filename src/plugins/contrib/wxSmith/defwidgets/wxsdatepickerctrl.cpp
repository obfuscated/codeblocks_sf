#include "../wxsheaders.h"
#include "wxsdatepickerctrl.h"

#include "wx/datectrl.h"

WXS_ST_BEGIN(wxsDatePickerCtrlStyles)
    WXS_ST_CATEGORY("wxDatePickerCtrl")
    WXS_ST(wxDP_DEFAULT)
    WXS_ST(wxDP_SPIN)
    WXS_ST(wxDP_DROPDOWN)
    WXS_ST(wxDP_ALLOWNONE)
    WXS_ST(wxDP_SHOWCENTURY)
//    WXS_ST_DEFAULTS()
WXS_ST_END(wxsDatePickerCtrlStyles)

WXS_EV_BEGIN(wxsDatePickerCtrlEvents)
    WXS_EVI(EVT_DATE_CHANGED,wxDataEvent,Changed)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsDatePickerCtrlEvents)



//wxDatePickerCtrl(wxWindow *parent, wxWindowID id, const wxDateTime& dt = wxDefaultDateTime, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDP_DEFAULT | wxDP_SHOWCENTURY, const wxValidator& validator = wxDefaultValidator, const wxString& name = "datectrl")

wxsDWDefineBegin(wxsDatePickerCtrl,wxDatePickerCtrl,

    ThisWidget = new wxDatePickerCtrl(parent,id,wxDefaultDateTime,pos,size,style);
    )
  // Temporary Commented for further action
  // wxsDWDefDate(label,"Label:","");
wxsDWDefineEnd()
