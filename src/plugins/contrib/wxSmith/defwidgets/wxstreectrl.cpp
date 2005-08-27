#include "wxstreectrl.h"

#include "wx/treectrl.h"

WXS_ST_BEGIN(wxsTreeCtrlStyles)
    WXS_ST_CATEGORY("wxTreeCtrl")
    WXS_ST(wxTR_EDIT_LABELS)
    WXS_ST(wxTR_NO_BUTTONS)
    WXS_ST(wxTR_HAS_BUTTONS)
    WXS_ST(wxTR_NO_LINES)
    WXS_ST(wxTR_FULL_ROW_HIGHLIGHT)
    WXS_ST(wxTR_LINES_AT_ROOT)
    WXS_ST(wxTR_HIDE_ROOT)
    WXS_ST(wxTR_ROW_LINES)    
    WXS_ST(wxTR_HAS_VARIABLE_ROW_HEIGHT)
    WXS_ST(wxTR_SINGLE)
    WXS_ST(wxTR_MULTIPLE)
    WXS_ST(wxTR_EXTENDED)
    WXS_ST(wxTR_DEFAULT_STYLE)    
    WXS_ST_DEFAULTS()
WXS_ST_END(wxsTreeCtrlStyles)


//wxTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTR_HAS_BUTTONS, const wxValidator& validator = wxDefaultValidator, const wxString& name = "listCtrl")


wxsDWDefineBegin(wxsTreeCtrl,wxTreeCtrl,

    ThisWidget = new wxTreeCtrl(parent,id,pos,size,style);
    )

wxsDWDefineEnd()
