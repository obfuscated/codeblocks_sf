#include "../wxsheaders.h"
#include "wxsradiobutton.h"

WXS_ST_BEGIN(wxsRadioButtonStyles)
    WXS_ST_CATEGORY("wxRadioButton")
    WXS_ST(wxRB_GROUP)
    WXS_ST(wxRB_SINGLE)
// NOTE (cyberkoa#1#): wxRB_USE_CHECKBOX is only available under PALMOS, not sure whether in XRC PALMOS port or not
    WXS_ST_MASK(wxRB_USE_CHECKBOX,wxsSFPALMOS,0,true)

WXS_ST_END(wxsRadioButtonStyles)

WXS_EV_BEGIN(wxsRadioButtonEvents)
    WXS_EVI(EVT_RADIOBUTTON,wxCommandEvent,Change)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsRadioButtonEvents)


// RadioButton Constructor
//wxRadioButton(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "radioButton")



wxsDWDefineBegin(wxsRadioButton,wxRadioButton,

    WXS_THIS = new wxRadioButton(WXS_PARENT,WXS_ID,label,WXS_POS,WXS_SIZE,WXS_STYLE,wxDefaultValidator,WXS_NAME);
    WXS_THIS->SetValue(selected);

    )

    wxsDWDefLongStr(label,"Label:","Label");
    wxsDWDefBoolX(selected,"value","Selected:",false);

wxsDWDefineEnd()
