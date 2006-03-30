#include "wxsscrollbar.h"

#include <wx/scrolbar.h>
#include <messagemanager.h>

/* ************************************************************************** */
/* Styles are processed almost like in older code - using WXS_ST_BEGIN,       */
/* WXS_ST_END, WXS_ST_CATEGORY, WXS_ST ...                                    */
/* ************************************************************************** */

WXS_ST_BEGIN(wxsScrollBarStyles)
    WXS_ST_CATEGORY("wxScrollBar")
    WXS_ST(wxSB_HORIZONTAL)
    WXS_ST(wxSB_VERTICAL)
WXS_ST_END()


/* ************************************************************************** */
/* Events have one additional argument - event type, these can be found       */
/* in WX_DIR/include/event.h                                                  */
/* ************************************************************************** */

WXS_EV_BEGIN(wxsScrollBarEvents)
 /* Extract from the Help
  To process a scroll event, use these event handler macros to direct input to member functions that take a wxScrollEvent argument. You can use EVT_COMMAND_SCROLL... macros with window IDs for when intercepting scroll events from controls, or EVT_SCROLL... macros without window IDs for intercepting scroll events from the receiving window -- except for this, the macros behave exactly the same.
*/
    WXS_EVI(EVT_COMMAND_SCROLL,wxEVT_SCROLL_TOP|wxEVT_SCROLL_BOTTOM|wxEVT_SCROLL_LINEUP|wxEVT_SCROLL_LINEDOWN|wxEVT_SCROLL_PAGEUP|wxEVT_SCROLL_PAGEDOWN|wxEVT_SCROLL_THUMBTRACK|wxEVT_SCROLL_THUMBRELEASE|wxEVT_SCROLL_CHANGED,wxScrollEvent,Scroll)
    WXS_EVI(EVT_COMMAND_SCROLL_TOP,wxEVT_SCROLL_TOP,wxScrollEvent,ScrollTop)
    WXS_EVI(EVT_COMMAND_SCROLL_BOTTOM,wxEVT_SCROLL_BOTTOM,wxScrollEvent,ScrollBottom)
    WXS_EVI(EVT_COMMAND_SCROLL_LINEUP,wxEVT_SCROLL_LINEUP,wxScrollEvent,ScrollLineUp)
    WXS_EVI(EVT_COMMAND_SCROLL_LINEDOWN,wxEVT_SCROLL_LINEDOWN,wxScrollEvent,ScrollLineDown)
    WXS_EVI(EVT_COMMAND_SCROLL_PAGEUP,wxEVT_SCROL_PAGEUP,wxScrollEvent,ScrollPageUp)
    WXS_EVI(EVT_COMMAND_SCROLL_PAGEDOWN,wxEVT_SCROLL_PAGEDOWN,wxScrollEvent,ScrollPageDown)
    WXS_EVI(EVT_COMMAND_SCROLL_THUMBTRACK,wxEVT_SCROLL_THUMBTRACK,wxScrollEvent,ScrollThumbTrack)
    WXS_EVI(EVT_COMMAND_SCROLL_THUMBRELEASE,wxEVT_SCROLL_THUMBRELEASE,wxScrollEvent,ScrollThumbRelease)

    // Only in WXMSW
    WXS_EVI(EVT_COMMAND_SCROLL_CHANGED,wxEVT_SCROLL_CHANGED,wxScrollEvent,ScrollChanged)

    WXS_EV_DEFAULTS()

WXS_EV_END()

/* ************************************************************************** */
/* Widget info                                                                */
/* ************************************************************************** */

wxsItemInfo wxsScrollBar::Info =
{
    _T("wxScrollBar"),          // Name of class
    wxsTWidget,                 // Type, always wxsTWidget for widget classes
    _("wxWidgets license"),     // License, any type
    _("wxWidgets team"),        // Author
    _T(""),                     // No default e-mail for standard widgets
    _T("www.wxwidgets.org"),    // Site
    _T("Standard"),             // Groud for widget, note that _T() instead of _() is used
    50,                         // Scroll bar is one of most commonly used widgets - we give it high priority
    _T("ScrollBar"),            // Standard prefix for variable names and identifiers
    2, 6,                       // Widget version
    NULL,                       // Bitmaps will be loaded later in manager
    NULL,                       // --------------------''-------------------
    0                           // --------------------''-------------------
};


/* ************************************************************************** */
/* Constructur for wxsWidget need style set, event set and widget info, so    */
/* these things should be created before ctor.                                */
/* ************************************************************************** */

wxsScrollBar::wxsScrollBar(wxsWindowRes* Resource):
    wxsWidget(
        Resource,                   // Resource is passed to wxsItem's constructor
        wxsBaseProperties::flAll,   // Using all base properties
        &Info,                      // Taking local info
        wxsScrollBarEvents,         // Pointer to local events
        wxsScrollBarStyles,         // Pointer to local styles
        _T(""))                      // Default style = 0

{}


/* ************************************************************************** */
/* Function building code - it should append new code generating this widget  */
/* to the end of prevous code. Currently it support only C++ langage but some */
/* other languages may be added in future.                                    */
/* ************************************************************************** */

void wxsScrollBar::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
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
            Code << GetVarName() << _T(" = new wxScrollBar(")
                 << WindowParent << _T(",")
                 << GetIdName() << _T(",")
                 << PosCode(WindowParent,wxsCPP) << _T(",")
                 << SizeCode(WindowParent,wxsCPP) << _T(",")
                 << StyleCode(wxsCPP) << _T(");\n");

            Code << GetVarName() << _T("->SetScrollbar(")
                 << wxString::Format(_T("%d"),Value)  << _T(",")
                 << wxString::Format(_T("%d"),ThumbSize)  << _T(",")
                 << wxString::Format(_T("%d"),Range)  << _T(",")
                 << wxString::Format(_T("%d"),PageSize)  << _T(");\n");

            SetupWindowCode(Code,Language);
            return;
        }
    }

    wxsLANGMSG(wxsScrollBar::BuildCreatingCode,Language);
}

/* ************************************************************************** */
/* Function building preview item. This should simply return previewed object */
/* with all properties set-up.                                                */
/* ************************************************************************** */

wxObject* wxsScrollBar::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    // Exact argument is not used here because button preview is
    // exactly the same in editor and in preview window
    wxScrollBar* Preview = new wxScrollBar(Parent,GetId(),Pos(Parent),Size(Parent),Style());

    Preview->SetScrollbar(Value,ThumbSize,Range,PageSize);

    return SetupWindow(Preview,Exact);
}


/* ************************************************************************** */
/* Function enumerating all scroll bar-specific properties.                       */
/* ************************************************************************** */

void wxsScrollBar::EnumWidgetProperties(long Flags)
{
   WXS_LONG(wxsScrollBar,Value,0,_("Value"),_T("Value"),0)
   WXS_LONG(wxsScrollBar,ThumbSize,0,_("ThumbSize"),_T("Thumb Size"),0)
   WXS_LONG(wxsScrollBar,Range,0,_("Range"),_T("Range"),0)
   WXS_LONG(wxsScrollBar,PageSize,0,_("PageSize"),_T("Page Size"),0)
}
