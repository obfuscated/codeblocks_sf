#include "../wxsheaders.h"
#include "wxstreectrl.h"

#include "wx/treectrl.h"

WXS_ST_BEGIN(wxsTreeCtrlStyles)
    WXS_ST_CATEGORY("wxTreeCtrl")
    WXS_ST(wxTR_EDIT_LABELS)
    WXS_ST(wxTR_NO_BUTTONS)
    WXS_ST(wxTR_HAS_BUTTONS)

// NOTE (cyberkoa##): wxTR_TWIST_BUTTONS found in wxMSW's XRC but not in HELP , maybe is new style
    WXS_ST(wxTR_TWIST_BUTTONS)

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
 //   WXS_ST_DEFAULTS()
WXS_ST_END(wxsTreeCtrlStyles)

WXS_EV_BEGIN(wxsTreeCtrlEvents)
    WXS_EVI(EVT_TREE_BEGIN_DRAG,wxTreeEvent,BeginDrag)
    WXS_EVI(EVT_TREE_BEGIN_RDRAG,wxTreeEvent,BeginRDrag)
    WXS_EVI(EVT_TREE_END_DRAG,wxTreeEvent,EndDrag)
    WXS_EVI(EVT_TREE_BEGIN_LABEL_EDIT,wxTreeEvent,BeginLabelEdit)
    WXS_EVI(EVT_TREE_END_LABEL_EDIT,wxTreeEvent,EndLabelEdit)
    WXS_EVI(EVT_TREE_DELETE_ITEM,wxTreeEvent,DeleteItem)
    WXS_EVI(EVT_TREE_GET_INFO,wxTreeEvent,GetInfo)
    WXS_EVI(EVT_TREE_SET_INFO,wxTreeEvent,SetInfo)
    WXS_EVI(EVT_TREE_ITEM_ACTIVATED,wxTreeEvent,ItemActivated)
    WXS_EVI(EVT_TREE_ITEM_COLLAPSED,wxTreeEvent,ItemCollapsed)
    WXS_EVI(EVT_TREE_ITEM_COLLAPSING,wxTreeEvent,ItemCollapsing)
    WXS_EVI(EVT_TREE_ITEM_EXPANDED,wxTreeEvent,ItemExpanded)
    WXS_EVI(EVT_TREE_ITEM_EXPANDING,wxTreeEvent,ItemExpanding)
    WXS_EVI(EVT_TREE_ITEM_RIGHT_CLICK,wxTreeEvent,ItemRightClick)
    WXS_EVI(EVT_TREE_ITEM_MIDDLE_CLICK,wxTreeEvent,ItemMiddleClick)
    WXS_EVI(EVT_TREE_SEL_CHANGED,wxTreeEvent,SelectionChanged)
    WXS_EVI(EVT_TREE_SEL_CHANGING,wxTreeEvent,SelectionChanging)
    WXS_EVI(EVT_TREE_KEY_DOWN,wxTreeEvent,KeyDown)
    WXS_EVI(EVT_TREE_ITEM_GETTOOLTIP,wxTreeEvent,ItemGetToolTip)
    WXS_EVI(EVT_TREE_ITEM_MENU,wxTreeEvent,ItemMenu)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsTreeCtrlEvents)


//wxTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTR_HAS_BUTTONS, const wxValidator& validator = wxDefaultValidator, const wxString& name = "listCtrl")


wxsDWDefineBegin(wxsTreeCtrl,wxTreeCtrl,

    WXS_THIS = new wxTreeCtrl(WXS_PARENT,WXS_ID,WXS_POS,WXS_SIZE,WXS_STYLE,wxDefaultValidator,WXS_NAME);
    )

wxsDWDefineEnd()
