#include "wxstextctrl.h"

//#include <wx/textctrl.h>
#include <messagemanager.h>

/* ************************************************************************** */
/* Styles are processed almost like in older code - using WXS_ST_BEGIN,       */
/* WXS_ST_END, WXS_ST_CATEGORY, WXS_ST ...                                    */
/* ************************************************************************** */

WXS_ST_BEGIN(wxsTextCtrlStyles)
    WXS_ST_CATEGORY("wxTextCtrl")
    WXS_ST(wxTE_NO_VSCROLL)
    WXS_ST(wxTE_AUTO_SCROLL)
    WXS_ST(wxTE_PROCESS_ENTER)
    WXS_ST(wxTE_PROCESS_TAB)
    WXS_ST(wxTE_MULTILINE)
    WXS_ST(wxTE_PASSWORD)
    WXS_ST(wxTE_READONLY)
//Help file : No effect under GTK1
    WXS_ST(wxHSCROLL)
    WXS_ST(wxTE_RICH)
    WXS_ST(wxTE_RICH2)
    WXS_ST(wxTE_NOHIDESEL)
    WXS_ST(wxTE_LEFT)
    WXS_ST_MASK(wxTE_AUTO_URL,wxsSFWin|wxsSFGTK20,0,true)
    WXS_ST_MASK(wxTE_CENTRE,wxsSFWin|wxsSFGTK20,0,true)
    WXS_ST_MASK(wxTE_RIGHT,wxsSFWin|wxsSFGTK20,0,true)
    WXS_ST_MASK(wxTE_CENTRE,wxsSFWin|wxsSFGTK20,0,true)
    WXS_ST_MASK(wxTE_CHARWRAP,wxsSFUNIV|wxsSFGTK20,0,true)
    WXS_ST_MASK(wxTE_WORDWRAP,wxsSFUNIV|wxsSFGTK20,0,true)
    WXS_ST(wxTE_BESTWRAP)

// Help file :On PocketPC and Smartphone, causes the first letter to be capitalized
    WXS_ST_MASK(wxTE_CAPITALIZE,wxsSFWinCE,0,true)
// Help file : same as wxTE_HSCROLL, so ignore
//    WXS_ST(wxTE_DONTWRAP)

// Not in Help file but in XRC file, assume applying to all platform
    WXS_ST(wxTE_LINEWRAP)
WXS_ST_END()


/* ************************************************************************** */
/* Events have one additional argument - event type, these can be found       */
/* in WX_DIR/include/event.h                                                  */
/* ************************************************************************** */

WXS_EV_BEGIN(wxsTextCtrlEvents)
    WXS_EVI(EVT_TEXT,wxEVT_COMMAND_TEXT_UPDATED,wxCommandEvent,Text)
    WXS_EVI(EVT_TEXT_ENTER,wxEVT_COMMAND_TEXT_ENTER,wxCommandEvent,TextEnter)
    WXS_EVI(EVT_TEXT_URL,wxEVT_COMMAND_TEXT_URL,wxTextUrlEvent,TextUrl)
    WXS_EVI(EVT_TEXT_MAXLEN,wxEVT_COMMAND_TEXT_MAXLEN,wxCommandEvent,TextMaxLen)
    WXS_EV_DEFAULTS()
WXS_EV_END()

/* ************************************************************************** */
/* Widget info                                                                */
/* ************************************************************************** */

wxsItemInfo wxsTextCtrl::Info =
{
    _T("wxTextCtrl"),           // Name of class
    wxsTWidget,                 // Type, always wxsTWidget for widget classes
    _("wxWidgets license"),     // License, any type
    _("wxWidgets team"),        // Author
    _T(""),                     // No default e-mail for standard widgets
    _T("www.wxwidgets.org"),    // Site
    _T("Standard"),             // Groud for widget, note that _T() instead of _() is used
    75,                         // Button is one of most commonly used widgets - we give it high priority
    _T("TextCtrl"),             // Standard prefix for variable names and identifiers
    2, 6,                       // Widget version
    NULL,                       // Bitmaps will be loaded later in manager
    NULL,                       // --------------------''-------------------
    0                           // --------------------''-------------------
};


/* ************************************************************************** */
/* Constructur for wxsWidget need style set, event set and widget info, so    */
/* these things should be created before ctor.                                */
/* ************************************************************************** */

wxsTextCtrl::wxsTextCtrl(wxsWindowRes* Resource):
    wxsWidget(
        Resource,                   // Resource is passed to wxsItem's constructor
        wxsBaseProperties::flAll,   // Using all base properties
        &Info,                      // Taking local info
        wxsTextCtrlEvents,          // Pointer to local events
        wxsTextCtrlStyles,          // Pointer to local styles
        _T("")),                     // Default style = 0
        Text(_("Text")),            // Initializing widget with some default (but not necessarily clean) values,
        MaxLength(0)                // these values will be used when creating new widgets
{}


/* ************************************************************************** */
/* Function building code - it should append new code generating this widget  */
/* to the end of prevous code. Currently it support only C++ langage but some */
/* other languages may be added in future.                                    */
/* ************************************************************************** */

void wxsTextCtrl::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
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
            Code<< GetVarName() << _T(" = new wxTextCtrl(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << wxsGetWxString(Text) << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(");\n");

            if ( MaxLength > 0 )
            {
                Code << GetVarName() << _T("->SetMaxLength(") << wxString::Format(_T("%d"),MaxLength) << _T(");\n");
            }
            SetupWindowCode(Code,Language);
            return;
        }
    }

    wxsLANGMSG(wxsTextCtrl::BuildCreatingCode,Language);
}

/* ************************************************************************** */
/* Function building preview item. This should simply return previewed object */
/* with all properties set-up.                                                */
/* ************************************************************************** */

wxObject* wxsTextCtrl::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    // Exact argument is not used here because button preview is
    // exactly the same in editor and in preview window
    wxTextCtrl* Preview = new wxTextCtrl(Parent,GetId(),Text,Pos(Parent),Size(Parent),Style());

    return SetupWindow(Preview,Exact);
}


/* ************************************************************************** */
/* Function enumerating all text control-specific properties.                       */
/* ************************************************************************** */

void wxsTextCtrl::EnumWidgetProperties(long Flags)
{
    WXS_STRING(wxsTextCtrl,Text,0,_("Text"),_T("text"),_T(""),true,false)
    WXS_LONG(wxsTextCtrl,MaxLength,0x8,_("Max Length"),_T("max length"),0)
}
