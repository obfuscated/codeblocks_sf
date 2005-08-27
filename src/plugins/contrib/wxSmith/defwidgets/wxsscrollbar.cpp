#include "wxsscrollbar.h"

WXS_ST_BEGIN(wxsScrollBarStyles)
    WXS_ST_CATEGORY("wxScrollBar")
    WXS_ST(wxSB_HORIZONTAL)    
    WXS_ST(wxSB_VERTICAL)
//    WXS_ST_DEFAULTS()
WXS_ST_END(wxsScrollBarStyles)



//wxScrollBar(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxSB_HORIZONTAL, const wxValidator& validator = wxDefaultValidator, const wxString& name = "scrollBar")

wxsDWDefineBegin(wxsScrollBar,wxScrollBar,

    ThisWidget = new wxScrollBar(parent,id,pos,size,style);
    ThisWidget->SetScrollbar(value,thumbsize,range,pagesize);
    )
    wxsDWDefInt(value,"Value:",0);
    wxsDWDefInt(thumbsize,"Thumb size:",1);
    wxsDWDefInt(range,"Range:",10);
    wxsDWDefInt(pagesize,"Page size:",1);
wxsDWDefineEnd()
