#include "wxsbitmapbutton.h"

#include <wx/bmpbuttn.h>
#include <messagemanager.h>

/* ************************************************************************** */
/* Styles are processed almost like in older code - using WXS_ST_BEGIN,       */
/* WXS_ST_END, WXS_ST_CATEGORY, WXS_ST ...                                    */
/* ************************************************************************** */

WXS_ST_BEGIN(wxsBitmapButtonStyles)
    WXS_ST_CATEGORY("wxBitmapButton")
    WXS_ST_MASK(wxBU_LEFT,wxsSFWin,0,true)
    WXS_ST_MASK(wxBU_TOP,wxsSFWin,0,true)
    WXS_ST_MASK(wxBU_RIGHT,wxsSFWin,0,true)
    WXS_ST_MASK(wxBU_BOTTOM,wxsSFWin,0,true)
    WXS_ST_MASK(wxBU_AUTODRAW,wxsSFWin,0,true)

    // cyberkoa: "Tthe help mentions that wxBU_EXACTFIX is not used but the XRC code yes
    //  WXS_ST(wxBU_EXACTFIX)
WXS_ST_END()


/* ************************************************************************** */
/* Events have one additional argument - event type, these can be found       */
/* in WX_DIR/include/event.h                                                  */
/* ************************************************************************** */

WXS_EV_BEGIN(wxsBitmapButtonEvents)
    WXS_EVI(EVT_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEvent,Click)
    WXS_EV_DEFAULTS()
WXS_EV_END()

/* ************************************************************************** */
/* Widget info                                                                */
/* ************************************************************************** */

wxsItemInfo wxsBitmapButton::Info =
{
    _T("wxBitmapButton"),       // Name of class
    wxsTWidget,                 // Type, always wxsTWidget for widget classes
    _("wxWidgets license"),     // License, any type
    _("wxWidgets team"),        // Author
    _T(""),                     // No default e-mail for standard widgets
    _T("www.wxwidgets.org"),    // Site
    _T("Standard"),             // Groud for widget, note that _T() instead of _() is used
    50,                         // Bitmap Button is commonly used widgets - we give it medium priority
    _T("BitmapButton"),         // Standard previx for variable names and identifiers
    2, 6,                       // Widget version
    NULL,                       // Bitmaps will be loaded later in manager
    NULL,                       // --------------------''-------------------
    0                           // --------------------''-------------------
};


/* ************************************************************************** */
/* Constructur for wxsWidget need style set, event set and widget info, so    */
/* these things should be created before ctor.                                */
/* ************************************************************************** */

wxsBitmapButton::wxsBitmapButton(wxsWindowRes* Resource):
    wxsWidget(
        Resource,                   // Resource is passed to wxsItem's constructor
        wxsBaseProperties::flAll,   // Using all base properties
        &Info,                      // Taking local info
        wxsBitmapButtonEvents,      // Pointer to local events
        wxsBitmapButtonStyles,      // Pointer to local styles
        _T(""))                     // Default style = 0
                                    // Initializing widget with some default (but not necessarily clean) values,
                                    // these values will be used when creating new widgets
{}


/* ************************************************************************** */
/* Function building code - it should append new code generating this widget  */
/* to the end of prevous code. Currently it support only C++ langage but some */
/* other languages may be added in future.                                    */
/* ************************************************************************** */

void wxsBitmapButton::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
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
            wxString SizeName = GetVarName() + _T("_Size");
            Code << _T("wxSize ") << SizeName << _T(" = ") << _T("wxSize(")
                 << wxString::Format(_T("%d"),(BaseProps.Size.X > 2)? (BaseProps.Size.X - 2):BaseProps.Size.X) << _T(",")
                 << wxString::Format(_T("%d"),(BaseProps.Size.Y > 2)? (BaseProps.Size.Y - 2):BaseProps.Size.Y) << _T(");\n");

            wxString BmpCode = BitmapLabel.BuildCode(BaseProps.Size.IsDefault,SizeName,wxsCPP,wxART_OTHER);
            Code<< GetVarName() << _T(" = new wxBitmapButton(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << BmpCode.empty() ? _T("wxNullBitmap") : BmpCode << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << wxsGetWxString(_T("wxDefaultSize")) << _T(",")
                //SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(");\n");
/*
            if ( BitmapLabel )
            {
                Code << GetVarName() << _T("->SetBitmapLabel(") << BmpCode << _T(");\n");
            }
*/
            if ( !(BitmapDisabled.Id.empty()||BitmapDisabled.Client.empty())|| !(BitmapDisabled.FileName.empty()) )
            {
                BmpCode = BitmapDisabled.BuildCode(BaseProps.Size.IsDefault,SizeName,wxsCPP,wxART_OTHER);
                Code << GetVarName() << _T("->SetBitmapDisabled(") << BmpCode << _T(");\n");
            }
            if ( !(BitmapSelected.Id.empty()||BitmapSelected.Client.empty())|| !(BitmapSelected.FileName.empty()) )
            {
                BmpCode = BitmapSelected.BuildCode(BaseProps.Size.IsDefault,SizeName,wxsCPP,wxART_OTHER);
                Code << GetVarName() << _T("->SetBitmapSelected(") << BmpCode << _T(");\n");
            }
            if ( !(BitmapFocus.Id.empty()||BitmapFocus.Client.empty())|| !(BitmapFocus.FileName.empty()) )
            {
                BmpCode = BitmapFocus.BuildCode(BaseProps.Size.IsDefault,SizeName,wxsCPP,wxART_OTHER);
                Code << GetVarName() << _T("->SetBitmapFocus(") << BmpCode << _T(");\n");
            }

            if ( IsDefault ) Code << GetVarName() << _T("->SetDefault();\n");
            SetupWindowCode(Code,Language);
            return;
        }
    }

    DBGLOG(_T("wxSmith: Unknown coding language when generating bitmap button (id: %d)"),Language);
}

/* ************************************************************************** */
/* Function building preview item. This should simply return previewed object */
/* with all properties set-up.                                                */
/* ************************************************************************** */

wxObject* wxsBitmapButton::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    // Exact argument is not used here because bitmap button preview is
    // exactly the same in editor and in preview window

    // Reduce the size of the bitmap by 2 pixel for width and height of the bitmap button
    wxSize BitmapSize = wxSize((BaseProps.Size.X > 2)? (BaseProps.Size.X - 2):BaseProps.Size.X,(BaseProps.Size.Y > 2)? (BaseProps.Size.Y - 2):BaseProps.Size.Y);

    wxBitmapButton* Preview = new wxBitmapButton(Parent,GetId(),BitmapLabel.GetPreview(BitmapSize),Pos(Parent),BitmapSize,Style());

    if ( !(BitmapDisabled.Id.empty()||BitmapDisabled.Client.empty())|| !(BitmapDisabled.FileName.empty()) )
        Preview->SetBitmapDisabled(BitmapDisabled.GetPreview(BitmapSize));

    if ( !(BitmapSelected.Id.empty()||BitmapSelected.Client.empty())|| !(BitmapSelected.FileName.empty()) )
        Preview->SetBitmapSelected(BitmapSelected.GetPreview(BitmapSize));

    if ( !(BitmapFocus.Id.empty()||BitmapFocus.Client.empty())|| !(BitmapFocus.FileName.empty()) )
        Preview->SetBitmapFocus(BitmapFocus.GetPreview(BitmapSize));

    if ( IsDefault ) Preview->SetDefault();
    return SetupWindow(Preview,Exact);
}


/* ************************************************************************** */
/* Function enumerating all bitmap button-specific properties.                       */
/* ************************************************************************** */

void wxsBitmapButton::EnumWidgetProperties(long Flags)
{
    WXS_BITMAP(wxsBitmapButton,BitmapLabel,0,_("Bitmap"),_T("bitmap"),_T("wxART_OTHER"))
    WXS_BITMAP(wxsBitmapButton,BitmapDisabled,0,_("Bitmap"),_T("disabled"),_T("wxART_OTHER"))
    WXS_BITMAP(wxsBitmapButton,BitmapSelected,0,_("Bitmap"),_T("selected"),_T("wxART_OTHER"))
    WXS_BITMAP(wxsBitmapButton,BitmapFocus,0,_("Bitmap"),_T("focus"),_T("wxART_OTHER"))
    WXS_BOOL  (wxsBitmapButton,IsDefault,0,_("Is default"),_("default"),false)
}
