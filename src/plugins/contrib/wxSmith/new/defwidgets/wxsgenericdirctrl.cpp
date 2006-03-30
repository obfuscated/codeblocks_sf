#include "wxsgenericdirctrl.h"

#include <wx/dirctrl.h>
#include <messagemanager.h>

/* ************************************************************************** */
/* Styles are processed almost like in older code - using WXS_ST_BEGIN,       */
/* WXS_ST_END, WXS_ST_CATEGORY, WXS_ST ...                                    */
/* ************************************************************************** */

WXS_ST_BEGIN(wxsGenericDirCtrlStyles)
    WXS_ST_CATEGORY("wxGenericDirCtrl")
    WXS_ST(wxDIRCTRL_DIR_ONLY)
    WXS_ST(wxDIRCTRL_3D_INTERNAL)
    WXS_ST(wxDIRCTRL_SELECT_FIRST)
    WXS_ST(wxDIRCTRL_SHOW_FILTERS)
    WXS_ST_MASK(wxDIRCTRL_EDIT_LABELS,wxsSFAll,0,false)
WXS_ST_END()


/* ************************************************************************** */
/* Events have one additional argument - event type, these can be found       */
/* in WX_DIR/include/event.h                                                  */
/* ************************************************************************** */

WXS_EV_BEGIN(wxsGenericDirCtrlEvents)
    WXS_EV_DEFAULTS()
WXS_EV_END()

/* ************************************************************************** */
/* Widget info                                                                */
/* ************************************************************************** */

wxsItemInfo wxsGenericDirCtrl::Info =
{
    _T("wxGenericDirCtrl"),             // Name of class
    wxsTWidget,                 // Type, always wxsTWidget for widget classes
    _("wxWidgets license"),     // License, any type
    _("wxWidgets team"),        // Author
    _T(""),                     // No default e-mail for standard widgets
    _T("www.wxwidgets.org"),    // Site
    _T("Standard"),             // Groud for widget, note that _T() instead of _() is used
    30,                         // GenericDirCtrl is not commonly used widgets - we give lower priority
    _T("GenDirCtrl"),           // Standard prefix for variable names and identifiers
    2, 6,                       // Widget version
    NULL,                       // Bitmaps will be loaded later in manager
    NULL,                       // --------------------''-------------------
    0                           // --------------------''-------------------
};


/* ************************************************************************** */
/* Constructur for wxsWidget need style set, event set and widget info, so    */
/* these things should be created before ctor.                                */
/* ************************************************************************** */

wxsGenericDirCtrl::wxsGenericDirCtrl(wxsWindowRes* Resource):
    wxsWidget(
        Resource,
        wxsBaseProperties::flAll,
        &Info,
        wxsGenericDirCtrlEvents,
        wxsGenericDirCtrlStyles,
        _T("")),
        DefaultFilter(0)

{}


/* ************************************************************************** */
/* Function building code - it should append new code generating this widget  */
/* to the end of prevous code. Currently it support only C++ langage but some */
/* other languages may be added in future.                                    */
/* ************************************************************************** */

void wxsGenericDirCtrl::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
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
            Code<< GetVarName() << _T(" = new wxGenericDirCtrl(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << wxsGetWxString(DefaultFolder) << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(",")
                << wxsGetWxString(Filter) << _T(",")
                << wxString::Format(_T("%d"),DefaultFilter) << _T(");\n");

            SetupWindowCode(Code,Language);
            return;
        }
    }

    wxsLANGMSG(wxsGenericDirCtrl::BuildCreatingCode,Language);
}

/* ************************************************************************** */
/* Function building preview item. This should simply return previewed object */
/* with all properties set-up.                                                */
/* ************************************************************************** */

wxObject* wxsGenericDirCtrl::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    // Exact argument is not used here because generic dir control preview is
    // exactly the same in editor and in preview window
    wxGenericDirCtrl* Preview = new wxGenericDirCtrl(Parent,GetId(),DefaultFolder,Pos(Parent),Size(Parent),Style(),Filter,DefaultFilter);
    return SetupWindow(Preview,Exact);
}


/* ************************************************************************** */
/* Function enumerating all button-specific properties.                       */
/* ************************************************************************** */

void wxsGenericDirCtrl::EnumWidgetProperties(long Flags)
{
    WXS_STRING(wxsGenericDirCtrl,DefaultFolder,0,_("Default Folder"),_T("defaultfolder"),_T(""),true,false)
    WXS_STRING(wxsGenericDirCtrl,Filter,0,_("Filter"),_T("filter"),_T(""),true,false)
    WXS_LONG(wxsGenericDirCtrl,DefaultFilter,0,_("Default Filter"),_T("defaultfilter"),0)
}
