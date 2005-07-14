#include "wxslistbox.h"

#include <wx/listbox.h>

WXS_ST_BEGIN(wxsListBoxStyles)
    WXS_ST_CATEGORY("wxListBox")
#ifdef __WIN32__    
    WXS_ST(wxLB_HSCROLL) // Windows ONLY
#endif    
    WXS_ST(wxLB_SINGLE)
    WXS_ST(wxLB_MULTIPLE)
    WXS_ST(wxLB_EXTENDED)
    WXS_ST(wxLB_ALWAYS_SB)
    WXS_ST(wxLB_NEEDED_SB)
    WXS_ST(wxLB_SORT)
    WXS_ST_DEFAULTS()
WXS_ST_END(wxsListBoxStyles)

// default constructor for wxListBox
//wxListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n, const wxString choices[] = NULL, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "listBox")

//wxListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, const wxArrayString& choices, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "listBox")


wxsDWDefineBeginExt(wxsListBox,wxListBox,
        ThisWidget = new wxListBox(parent,id,pos,size,0,0,style);
        wxsDWAddStrings(arrayChoices,ThisWidget);
        wxsDWSelectString(arrayChoices,defaultChoice,ThisWidget);
    , true
    )
   
    wxsDWDefIntX(defaultChoice,"selection","Default",-1)
    wxsDWDefStrArrayX(arrayChoices,"content","item","Choices",defaultChoice)

wxsDWDefineEnd()
