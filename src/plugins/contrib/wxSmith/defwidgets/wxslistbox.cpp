#include "../wxsheaders.h"
#include "wxslistbox.h"

#include <wx/listbox.h>

WXS_ST_BEGIN(wxsListBoxStyles)
    WXS_ST_CATEGORY("wxListBox")
    WXS_ST_MASK(wxLB_HSCROLL,wxsSFWin,0,true) // Windows ONLY
    WXS_ST(wxLB_SINGLE)
    WXS_ST(wxLB_MULTIPLE)
    WXS_ST(wxLB_EXTENDED)
    WXS_ST(wxLB_ALWAYS_SB)
    WXS_ST(wxLB_NEEDED_SB)
    WXS_ST(wxLB_SORT)
 //   WXS_ST_DEFAULTS()
WXS_ST_END(wxsListBoxStyles)

WXS_EV_BEGIN(wxsListBoxEvents)
    WXS_EVI(EVT_LISTBOX,wxCommandEvent,Select)
    WXS_EVI(EVT_LISTBOX_DCLICK,wxCommandEvent,DClick)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsListBoxEvents)

// default constructor for wxListBox
//wxListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n, const wxString choices[] = NULL, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "listBox")

//wxListBox(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, const wxArrayString& choices, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "listBox")


wxsDWDefineBegin(wxsListBox,wxListBox,
        WXS_THIS = new wxListBox(WXS_PARENT,WXS_ID,WXS_POS,WXS_SIZE,0,0,WXS_STYLE,wxDefaultValidator,WXS_NAME);
        wxsDWAddStrings(arrayChoices,WXS_THIS);
        wxsDWSelectString(arrayChoices,defaultChoice,WXS_THIS);
    )

    wxsDWDefIntX(defaultChoice,"selection","",-1)
    wxsDWDefStrArrayX(arrayChoices,"content","item","Choices:",defaultChoice,wxLB_SORT)

wxsDWDefineEnd()
