#include "wxsstaticbitmap.h"

#include <wx/statbmp.h>
#include <messagemanager.h>

/* ************************************************************************** */
/* Styles are processed almost like in older code - using WXS_ST_BEGIN,       */
/* WXS_ST_END, WXS_ST_CATEGORY, WXS_ST ...                                    */
/* ************************************************************************** */

WXS_ST_BEGIN(wxsStaticBitmapStyles)
    WXS_ST_CATEGORY("wxStaticBitmap")
WXS_ST_END()


/* ************************************************************************** */
/* Events have one additional argument - event type, these can be found       */
/* in WX_DIR/include/event.h                                                  */
/* ************************************************************************** */

WXS_EV_BEGIN(wxsStaticBitmapEvents)
    WXS_EV_DEFAULTS()
WXS_EV_END()

/* ************************************************************************** */
/* Widget info                                                                */
/* ************************************************************************** */

wxsItemInfo wxsStaticBitmap::Info =
{
    _T("wxStaticBitmap"),       // Name of class
    wxsTWidget,                 // Type, always wxsTWidget for widget classes
    _("wxWidgets license"),     // License, any type
    _("wxWidgets team"),        // Author
    _T(""),                     // No default e-mail for standard widgets
    _T("www.wxwidgets.org"),    // Site
    _T("Standard"),             // Groud for widget, note that _T() instead of _() is used
    70,                         // Button is one of most commonly used widgets - we give it high priority
    _T("StaticBitmap"),         // Standard prefix for variable names and identifiers
    2, 6,                       // Widget version
    NULL,                       // Bitmaps will be loaded later in manager
    NULL,                       // --------------------''-------------------
    0                           // --------------------''-------------------
};


/* ************************************************************************** */
/* Constructur for wxsWidget need style set, event set and widget info, so    */
/* these things should be created before ctor.                                */
/* ************************************************************************** */

wxsStaticBitmap::wxsStaticBitmap(wxsWindowRes* Resource):
    wxsWidget(
        Resource,                   // Resource is passed to wxsItem's constructor
        wxsBaseProperties::flAll,   // Using all base properties
        &Info,                      // Taking local info
        wxsStaticBitmapEvents,      // Pointer to local events
        wxsStaticBitmapStyles,      // Pointer to local styles
        _T(""))                      // Default style = 0
                                    // Initializing widget with some default (but not necessarily clean) values,
                                    // these values will be used when creating new widgets
{}


/* ************************************************************************** */
/* Function building code - it should append new code generating this widget  */
/* to the end of prevous code. Currently it support only C++ langage but some */
/* other languages may be added in future.                                    */
/* ************************************************************************** */

void wxsStaticBitmap::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            wxString SizeName = GetVarName() + _T("_Size");
            Code << _T("wxSize ") << SizeName << _T(" = ") << SizeCode(WindowParent,wxsCPP) << _T(";\n");
            wxString BmpCode = Bitmap.BuildCode(BaseProps.Size.IsDefault,SizeName,wxsCPP,wxART_OTHER);
            Code<< GetVarName() << _T(" = new wxStaticBitmap(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << BmpCode.empty() ? _T("wxNullBitmap") : BmpCode << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeName << _T(",")
                << StyleCode(wxsCPP) << _T(");\n");

            SetupWindowCode(Code,Language);
            return;
        }
    }

    DBGLOG(_T("wxSmith: Unknown coding language when generating static bitmap (id: %d)"),Language);
}

/* ************************************************************************** */
/* Function building preview item. This should simply return previewed object */
/* with all properties set-up.                                                */
/* ************************************************************************** */

wxObject* wxsStaticBitmap::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    // Exact argument is not used here because static bitmap preview is
    // exactly the same in editor and in preview window
    wxStaticBitmap* Preview = new wxStaticBitmap(Parent,GetId(),Bitmap.GetPreview(Size(Parent)),Pos(Parent),Size(Parent),Style());
    return SetupWindow(Preview,Exact);
}


/* ************************************************************************** */
/* Function enumerating all static bitmap-specific properties.                       */
/* ************************************************************************** */

void wxsStaticBitmap::EnumWidgetProperties(long Flags)
{
   WXS_BITMAP(wxsStaticBitmap,Bitmap,0,_("Bitmap"),_T("bitmap"),_T("wxART_OTHER"))
}
