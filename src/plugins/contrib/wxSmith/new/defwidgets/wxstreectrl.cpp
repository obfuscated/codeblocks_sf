#include "wxstreectrl.h"

#include <wx/treectrl.h>
#include <messagemanager.h>

/* ************************************************************************** */
/* Styles are processed almost like in older code - using WXS_ST_BEGIN,       */
/* WXS_ST_END, WXS_ST_CATEGORY, WXS_ST ...                                    */
/* ************************************************************************** */

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


/* ************************************************************************** */
/* Events have one additional argument - event type, these can be found       */
/* in WX_DIR/include/event.h                                                  */
/* ************************************************************************** */

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

/* ************************************************************************** */
/* Widget info                                                                */
/* ************************************************************************** */

wxsItemInfo wxsTreeCtrl::Info =
{
    _T("wxTreeCtrl"),             // Name of class
    wxsTWidget,                 // Type, always wxsTWidget for widget classes
    _("wxWidgets license"),     // License, any type
    _("wxWidgets team"),        // Author
    _T(""),                     // No default e-mail for standard widgets
    _T("www.wxwidgets.org"),    // Site
    _T("Standard"),             // Groud for widget, note that _T() instead of _() is used
    80,                         // Button is one of most commonly used widgets - we give it high priority
    _T("TreeCtrl"),               // Standard prefix for variable names and identifiers
    2, 6,                       // Widget version
    NULL,                       // Bitmaps will be loaded later in manager
    NULL,                       // --------------------''-------------------
    0                           // --------------------''-------------------
};


/* ************************************************************************** */
/* Constructur for wxsWidget need style set, event set and widget info, so    */
/* these things should be created before ctor.                                */
/* ************************************************************************** */

wxsTreeCtrl::wxsTreeCtrl(wxsWindowRes* Resource):
    wxsWidget(
        Resource,                   // Resource is passed to wxsItem's constructor
        wxsBaseProperties::flAll,   // Using all base properties
        &Info,                      // Taking local info
        wxsTreeCtrlEvents,          // Pointer to local events
        wxsTreeCtrlStyles,          // Pointer to local styles
        _T(""))                      // Default style = 0
                                    // these values will be used when creating new widgets
{}


/* ************************************************************************** */
/* Function building code - it should append new code generating this widget  */
/* to the end of prevous code. Currently it support only C++ langage but some */
/* other languages may be added in future.                                    */
/* ************************************************************************** */

void wxsTreeCtrl::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            // Because c_str() may lead to unspecified behaviour
            // it's better to use << operator instead of wxString::Format.
            // But be carefull when using << to add integers and longs.
            // Because of some wxWidgets bugs use '<< wxString::Format(_T("%d"),Value)'
            // instead of '<< Value'
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

    DBGLOG(_T("wxSmith: Unknown coding language when generating tree control (id: %d)"),Language);
}

/* ************************************************************************** */
/* Function building preview item. This should simply return previewed object */
/* with all properties set-up.                                                */
/* ************************************************************************** */

wxObject* wxsTreeCtrl::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    // Exact argument is not used here because tree control preview is
    // exactly the same in editor and in preview window
    wxTreeCtrl* Preview = new wxTreeCtrl(Parent,GetId(),Pos(Parent),Size(Parent),Style());
    return SetupWindow(Preview,Exact);
}


/* ************************************************************************** */
/* Function enumerating all tree control-specific properties.                       */
/* ************************************************************************** */

void wxsTreeCtrl::EnumWidgetProperties(long Flags)
{
/*
    WXS_STRING(wxsTreeCtrl,Label,0,_("Label"),_T("label"),_T(""),true,false)
    WXS_BOOL  (wxsTreeCtrl,IsDefault,0,_("Is default"),_("default"),false)
*/
}
