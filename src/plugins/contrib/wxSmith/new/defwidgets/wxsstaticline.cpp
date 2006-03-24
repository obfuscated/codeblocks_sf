#include "wxsstaticline.h"

#include <wx/statline.h>
#include <messagemanager.h>

/* ************************************************************************** */
/* Styles are processed almost like in older code - using WXS_ST_BEGIN,       */
/* WXS_ST_END, WXS_ST_CATEGORY, WXS_ST ...                                    */
/* ************************************************************************** */

WXS_ST_BEGIN(wxsStaticLineStyles)
    WXS_ST_CATEGORY("wxStaticLine")
    WXS_ST(wxLI_HORIZONTAL)
    WXS_ST(wxLI_VERTICAL)
WXS_ST_END()


/* ************************************************************************** */
/* Events have one additional argument - event type, these can be found       */
/* in WX_DIR/include/event.h                                                  */
/* ************************************************************************** */

WXS_EV_BEGIN(wxsStaticLineEvents)
    WXS_EV_DEFAULTS()
WXS_EV_END()

/* ************************************************************************** */
/* Widget info                                                                */
/* ************************************************************************** */

wxsItemInfo wxsStaticLine::Info =
{
    _T("wxStaticLine"),         // Name of class
    wxsTWidget,                 // Type, always wxsTWidget for widget classes
    _("wxWidgets license"),     // License, any type
    _("wxWidgets team"),        // Author
    _T(""),                     // No default e-mail for standard widgets
    _T("www.wxwidgets.org"),    // Site
    _T("Standard"),             // Groud for widget, note that _T() instead of _() is used
    60,                         // Button is one of most commonly used widgets - we give it high priority
    _T("StaticLine"),           // Standard prefix for variable names and identifiers
    2, 6,                       // Widget version
    NULL,                       // Bitmaps will be loaded later in manager
    NULL,                       // --------------------''-------------------
    0                           // --------------------''-------------------
};


/* ************************************************************************** */
/* Constructur for wxsWidget need style set, event set and widget info, so    */
/* these things should be created before ctor.                                */
/* ************************************************************************** */

wxsStaticLine::wxsStaticLine(wxsWindowRes* Resource):
    wxsWidget(
        Resource,                   // Resource is passed to wxsItem's constructor
        wxsBaseProperties::flAll,   // Using all base properties
        &Info,                      // Taking local info
        wxsStaticLineEvents,        // Pointer to local events
        wxsStaticLineStyles,        // Pointer to local styles
        _T(""))                      // Default style = 0

{
    // Default the size so that it can be seen in the edit mode
    BaseProps.Size.IsDefault = false;
    BaseProps.Size.X = 10;
    BaseProps.Size.Y = -1;
}


/* ************************************************************************** */
/* Function building code - it should append new code generating this widget  */
/* to the end of prevous code. Currently it support only C++ langage but some */
/* other languages may be added in future.                                    */
/* ************************************************************************** */

void wxsStaticLine::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
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
            Code<< GetVarName() << _T(" = new wxStaticLine(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(");\n");

            SetupWindowCode(Code,Language);
            return;
        }
    }

    DBGLOG(_T("wxSmith: Unknown coding language when generating static line (id: %d)"),Language);
}

/* ************************************************************************** */
/* Function building preview item. This should simply return previewed object */
/* with all properties set-up.                                                */
/* ************************************************************************** */

wxObject* wxsStaticLine::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    // Exact argument is not used here because static line preview is
    // exactly the same in editor and in preview window
    wxStaticLine* Preview = new wxStaticLine(Parent,GetId(),Pos(Parent),Size(Parent),Style());

    return SetupWindow(Preview,Exact);
}


/* ************************************************************************** */
/* Function enumerating all static line -specific properties.                       */
/* ************************************************************************** */

void wxsStaticLine::EnumWidgetProperties(long Flags)
{
//    WXS_STRING(wxsStaticLine,Label,0,_("Label"),_T("label"),_T(""),true,false)
//    WXS_BOOL  (wxsStaticLine,IsDefault,0,_("Is default"),_("default"),false)
}

/*
bool wxsStaticLine::MyPropertiesUpdated(bool Validate,bool Correct)
{
    // Need to additionally check size params
    if ( Style() & wxLI_VERTICAL )
    {
        if ( BaseProps.SizeY == -1 )
        {
            BaseProps.SizeY = BaseProps.SizeX;
        }
        BaseProps.SizeX = -1;
    }
    else
    {
        if ( BaseProps.SizeX == -1 )
        {
            BaseProps.SizeX = BaseProps.SizeY;
        }
        BaseProps.SizeY = -1;
    }

    return true;
}
*/
