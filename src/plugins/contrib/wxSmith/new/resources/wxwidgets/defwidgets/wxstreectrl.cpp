#include "wxstreectrl.h"

#include <wx/treectrl.h>
#include <messagemanager.h>


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
WXS_ST_END()

WXS_EV_BEGIN(wxsTreeCtrlEvents)
    WXS_EVI(EVT_TREE_BEGIN_DRAG,wxEVT_COMMAND_TREE_BEGIN_DRAG,wxTreeEvent,BeginDrag)
    WXS_EVI(EVT_TREE_BEGIN_RDRAG,wxEVT_COMMAND_TREE_BEGIN_RDRAG,wxTreeEvent,BeginRDrag)
    WXS_EVI(EVT_TREE_END_DRAG,wxEVT_COMMAND_TREE_END_DRAG,wxTreeEvent,EndDrag)
    WXS_EVI(EVT_TREE_BEGIN_LABEL_EDIT,wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT,wxTreeEvent,BeginLabelEdit)
    WXS_EVI(EVT_TREE_END_LABEL_EDIT,wxEVT_COMMAND_TREE_END_LABEL_EDIT,wxTreeEvent,EndLabelEdit)
    WXS_EVI(EVT_TREE_DELETE_ITEM,wxEVT_COMMAND_TREE_DELETE_ITEM,wxTreeEvent,DeleteItem)
    WXS_EVI(EVT_TREE_GET_INFO,wxEVT_COMMAND_TREE_GET_INFO,wxTreeEvent,GetInfo)
    WXS_EVI(EVT_TREE_SET_INFO,wxEVT_COMMAND_TREE_SET_INFO,wxTreeEvent,SetInfo)
    WXS_EVI(EVT_TREE_ITEM_ACTIVATED,wxEVT_COMMAND_TREE_ITEM_ACTIVATED,wxTreeEvent,ItemActivated)
    WXS_EVI(EVT_TREE_ITEM_COLLAPSED,wxEVT_COMMAND_TREE_ITEM_COLLAPSED,wxTreeEvent,ItemCollapsed)
    WXS_EVI(EVT_TREE_ITEM_COLLAPSING,wxEVT_COMMAND_TREE_ITEM_COLLAPSING,wxTreeEvent,ItemCollapsing)
    WXS_EVI(EVT_TREE_ITEM_EXPANDED,wxEVT_COMMAND_TREE_ITEM_EXPANDED,wxTreeEvent,ItemExpanded)
    WXS_EVI(EVT_TREE_ITEM_EXPANDING,wxEVT_COMMAND_TREE_ITEM_EXPANDING,wxTreeEvent,ItemExpanding)
    WXS_EVI(EVT_TREE_ITEM_RIGHT_CLICK,wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK,wxTreeEvent,ItemRightClick)
    WXS_EVI(EVT_TREE_ITEM_MIDDLE_CLICK,wxEVT_COMMAND_TREE_ITEM_MIDDLE_CLICK,wxTreeEvent,ItemMiddleClick)
    WXS_EVI(EVT_TREE_SEL_CHANGED,wxEVT_COMMAND_TREE_SEL_CHANGED,wxTreeEvent,SelectionChanged)
    WXS_EVI(EVT_TREE_SEL_CHANGING,wxEVT_COMMAND_TREE_SEL_CHANGING,wxTreeEvent,SelectionChanging)
    WXS_EVI(EVT_TREE_KEY_DOWN,wxEVT_COMMAND_TREE_KEY_DOWN,wxTreeEvent,KeyDown)
    WXS_EVI(EVT_TREE_ITEM_GETTOOLTIP,wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP,wxTreeEvent,ItemGetToolTip)
    WXS_EVI(EVT_TREE_ITEM_MENU,wxEVT_COMMAND_TREE_ITEM_MENU,wxTreeEvent,ItemMenu)
    WXS_EV_DEFAULTS()
WXS_EV_END()


wxsItemInfo wxsTreeCtrl::Info =
{
    _T("wxTreeCtrl"),
    wxsTWidget,
    _("wxWidgets license"),
    _("wxWidgets team"),
    _T(""),
    _T("www.wxwidgets.org"),
    _T("Standard"),
    70,
    _T("TreeCtrl"),
    2, 6,
    NULL,
    NULL,
    0
};


wxsTreeCtrl::wxsTreeCtrl(wxsWindowRes* Resource):
    wxsWidget(
        Resource,
        wxsBaseProperties::flAll,
        &Info,
        wxsTreeCtrlEvents,
        wxsTreeCtrlStyles,
        _T(""))

{}



void wxsTreeCtrl::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code<< GetVarName() << _T(" = new wxTreeCtrl(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(");\n");

            SetupWindowCode(Code,Language);
            return;
        }
    }

    wxsLANGMSG(wxsTreeCtrl::BuildCreatingCode,Language);
}


wxObject* wxsTreeCtrl::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    wxTreeCtrl* Preview = new wxTreeCtrl(Parent,GetId(),Pos(Parent),Size(Parent),Style());
    return SetupWindow(Preview,Exact);
}



void wxsTreeCtrl::EnumWidgetProperties(long Flags)
{
//    WXS_STRING(wxsTreeCtrl,Label,0,_("Label"),_T("label"),_T(""),true,false)
}

void wxsTreeCtrl::EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/treectrl.h>")); return;
    }

    wxsLANGMSG(wxsTreeCtrl::EnumDeclFiles,Language);
}
