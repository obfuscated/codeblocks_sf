#include "wxsradiobox.h"

WXS_ST_BEGIN(wxsRadioBoxStyles)
    WXS_ST_CATEGORY("wxRadioBox")
    WXS_ST(wxRA_SPECIFY_ROWS)
    WXS_ST(wxRA_SPECIFY_COLS)    
#ifdef __PALMOS__    
    WXS_ST(wxRA_USE_CHECKBOX)
#endif    
    WXS_ST_DEFAULTS()
WXS_ST_END(wxsRadioBoxStyles)

// Constructor
//wxRadioBox(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& point = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, int majorDimension = 0, long style = wxRA_SPECIFY_COLS, const wxValidator& validator = wxDefaultValidator, const wxString& name = "radioBox")

wxsDWDefineBegin(wxsRadioBox,wxRadioBox,
    ThisWidget = new wxRadioBox(parent,id,label,pos,size,arrayChoices,0,style);
          if(defaultChoice>=0)   ThisWidget->SetSelection(defaultChoice);
    )

    #ifdef __NO_PROPGRID
        wxsDWDefIntX(defaultChoice,"selection","Default",-1)
    #else
        wxsDWDefIntX(defaultChoice,"selection","",-1)
    #endif
      wxsDWDefStrArrayX(arrayChoices,"content","item","Choices",defaultChoice,0)
wxsDWDefineEnd()
