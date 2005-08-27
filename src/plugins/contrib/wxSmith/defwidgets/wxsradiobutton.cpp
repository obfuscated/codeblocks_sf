#include "wxsradiobutton.h"

WXS_ST_BEGIN(wxsRadioButtonStyles)
    WXS_ST_CATEGORY("wxRadioButton")
    WXS_ST(wxBU_LEFT)
    WXS_ST(wxBU_TOP)
    WXS_ST(wxBU_RIGHT)
    WXS_ST(wxBU_BOTTOM)
//    WXS_ST(wxBU_EXACTFIT)
    WXS_ST(wxNO_BORDER)
    
    WXS_ST(wxRB_GROUP)
#ifdef __WXMSW__
    WXS_ST(wxRB_SINGLE)
#endif
#ifdef __PALMOS__    
    WXS_ST(wxRB_USE_CHECKBOX)
#endif     
    WXS_ST_DEFAULTS()
WXS_ST_END(wxsRadioButtonStyles)


// RadioButton Constructor
//wxRadioButton(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "radioButton")



wxsDWDefineBegin(wxsRadioButton,wxRadioButton,

    ThisWidget = new wxRadioButton(parent,id,label,pos,size,style);
    ThisWidget->SetValue(selected);
    
    )

    wxsDWDefStr(label,"Label:","Label");
    wxsDWDefBoolX(selected,"value","Selected:",false);

wxsDWDefineEnd()
