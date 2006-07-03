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
WXS_ST_END(wxsDatePickerCtrlStyles)

WXS_EV_BEGIN(wxsDatePickerCtrlEvents)
    WXS_EVI(EVT_DATE_CHANGED,wxDataEvent,Changed)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsDatePickerCtrlEvents)



//wxDatePickerCtrl(wxWindow *parent, wxWindowID id, const wxDateTime& dt = wxDefaultDateTime, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDP_DEFAULT | wxDP_SHOWCENTURY, const wxValidator& validator = wxDefaultValidator, const wxString& name = "datectrl")

wxsDWDefineBegin(wxsDatePickerCtrl,wxDatePickerCtrl,

    WXS_THIS = new wxDatePickerCtrl(WXS_PARENT,WXS_ID,wxDefaultDateTime,WXS_POS,WXS_SIZE,WXS_STYLE,wxDefaultValidator,WXS_NAME);
    )
  // Temporary Commented for further action
  // wxsDWDefDate(label,"Label:","");
wxsDWDefineEnd()
