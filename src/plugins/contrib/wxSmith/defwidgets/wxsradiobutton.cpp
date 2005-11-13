#include "../wxsheaders.h"
#include "wxsradiobutton.h"

WXS_ST_BEGIN(wxsRadioButtonStyles)
    WXS_ST_CATEGORY("wxRadioButton")
    WXS_ST(wxRB_GROUP)
    WXS_ST(wxRB_SINGLE)
#ifdef __WXPALMOS__
    WXS_ST(wxRB_USE_CHECKBOX)
#endif
    WXS_ST_DEFAULTS()
WXS_ST_END(wxsRadioButtonStyles)

WXS_EV_BEGIN(wxsRadioButtonEvents)
    WXS_EVI(EVT_RADIOBUTTON,wxCommandEvent,Change)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsRadioButtonEvents)


// RadioButton Constructor
//wxRadioButton(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "radioButton")



wxsDWDefineBegin(wxsRadioButton,wxRadioButton,

    ThisWidget = new wxRadioButton(parent,id,label,pos,size,style);
    ThisWidget->SetValue(selected);

    )

    wxsDWDefLongStr(label,"Label:","Label");
    wxsDWDefBoolX(selected,"value","Selected:",false);

wxsDWDefineEnd()
