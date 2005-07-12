#include "wxscombobox.h"

#include <wx/combobox.h>

WXS_ST_BEGIN(wxsComboBoxStyles)
    WXS_ST_CATEGORY("wxComboBox")
#ifdef __WIN32__    
    WXS_ST(wxCB_SIMPLE) // Windows ONLY
#endif    
    WXS_ST(wxCB_READONLY)
    WXS_ST(wxCB_SORT)
    WXS_ST_DEFAULTS()
WXS_ST_END(wxsComboBoxStyles)

// default constructor for wxCombobox
//wxComboBox(wxWindow* parent, wxWindowID id, const wxString& value = "", const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n, const wxString choices[], long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "comboBox")

// Constructor 2 for wxCombobox, available only in wxwidgets > 2.5
//wxComboBox(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size, const wxArrayString& choices, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "comboBox")


wxsDWDefineBegin(wxsComboBox,wxComboBox,
	ThisWidget = new wxComboBox(parent,id,_(""),pos,size,0,NULL,style);
    wxsDWAddStrings(arrayChoices,ThisWidget->Append);
    wxsDWSelectString(arrayChoices,defaultChoice,ThisWidget->SetValue);
	)
   
    wxsDWDefIntX(defaultChoice,"selection","Default",-1)
    wxsDWDefStrArrayX(arrayChoices,"content","item","Choices",defaultChoice)

wxsDWDefineEnd()
