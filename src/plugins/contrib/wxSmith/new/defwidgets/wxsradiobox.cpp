#include "wxsradiobox.h"

#include <wx/radiobox.h>
#include <messagemanager.h>

/* ************************************************************************** */
/* Styles are processed almost like in older code - using WXS_ST_BEGIN,       */
/* WXS_ST_END, WXS_ST_CATEGORY, WXS_ST ...                                    */
/* ************************************************************************** */

WXS_ST_BEGIN(wxsRadioBoxStyles)
    WXS_ST_CATEGORY("wxRadioBox")
    WXS_ST(wxRA_SPECIFY_ROWS)
    WXS_ST(wxRA_SPECIFY_COLS)
// NOTE (cyberkoa##): wxRA_HORIZONTAL & wxRA_VERTICAL is not in HELP file but in wxMSW's XRC
    WXS_ST_MASK(wxRA_HORIZONTAL,wxsSFAll,0,false)
    WXS_ST_MASK(wxRA_VERTICAL,wxsSFAll,0,false)
    WXS_ST_MASK(wxRA_USE_CHECKBOX,wxsSFPALMOS,0,true)
WXS_ST_END()


/* ************************************************************************** */
/* Events have one additional argument - event type, these can be found       */
/* in WX_DIR/include/event.h                                                  */
/* ************************************************************************** */

WXS_EV_BEGIN(wxsRadioBoxEvents)
    WXS_EVI(EVT_RADIOBOX,wxEVT_COMMAND_RADIOBOX_SELECTED,wxCommandEvent,Select)
    WXS_EV_DEFAULTS()
WXS_EV_END()

/* ************************************************************************** */
/* Widget info                                                                */
/* ************************************************************************** */

wxsItemInfo wxsRadioBox::Info =
{
    _T("wxRadioBox"),             // Name of class
    wxsTWidget,                 // Type, always wxsTWidget for widget classes
    _("wxWidgets license"),     // License, any type
    _("wxWidgets team"),        // Author
    _T(""),                     // No default e-mail for standard widgets
    _T("www.wxwidgets.org"),    // Site
    _T("Standard"),             // Groud for widget, note that _T() instead of _() is used
    80,                         // Button is one of most commonly used widgets - we give it high priority
    _T("RadioBox"),               // Standard prefix for variable names and identifiers
    2, 6,                       // Widget version
    NULL,                       // Bitmaps will be loaded later in manager
    NULL,                       // --------------------''-------------------
    0                           // --------------------''-------------------
};


/* ************************************************************************** */
/* Constructur for wxsWidget need style set, event set and widget info, so    */
/* these things should be created before ctor.                                */
/* ************************************************************************** */

wxsRadioBox::wxsRadioBox(wxsWindowRes* Resource):
    wxsWidget(
        Resource,                   // Resource is passed to wxsItem's constructor
        wxsBaseProperties::flAll,   // Using all base properties
        &Info,                      // Taking local info
        wxsRadioBoxEvents,          // Pointer to local events
        wxsRadioBoxStyles,          // Pointer to local styles
        _T("")),                     // Default style = 0
    Label(Label),
    DefaultSelection(-1),           // Initializing widget with some default (but not necessarily clean) values,
    Dimension(1)                    // these values will be used when creating new widgets
{}


/* ************************************************************************** */
/* Function building code - it should append new code generating this widget  */
/* to the end of prevous code. Currently it support only C++ langage but some */
/* other languages may be added in future.                                    */
/* ************************************************************************** */

void wxsRadioBox::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
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

            // wxRadioBox does not have Append Function , therefore , have to build a wxString[]
            // to pass in to the ctor
            Code<< _T("wxString wxRadioBoxChoices_") << GetVarName()
                << _T("[") << wxString::Format(_T("%d"),ArrayChoices.GetCount()) << _T("] = \n{\n");
            for ( size_t i = 0; i < ArrayChoices.GetCount(); ++i )
            {
                Code << _T("\t") << wxsGetWxString(ArrayChoices[i]);
                if ( i != ArrayChoices.GetCount()-1 ) Code << _T(",");
                Code << _T("\n");
            }
            Code << _T("};\n");

            if ( Dimension < 1 ) Dimension = 1;
            Code << GetVarName() << _T(" = new wxRadioBox(")
                 << WindowParent << _T(",")
                 << GetIdName() << _T(",")
                 << wxsGetWxString(Label) << _T(",")
                 << PosCode(WindowParent,wxsCPP) << _T(",")
                 << SizeCode(WindowParent,wxsCPP) << _T(",")
                 << wxString::Format(_T("%d"),ArrayChoices.GetCount()) << _T(",")
                 << _T("wxRadioBoxChoices_") << GetVarName() << _T(",")
                 << wxString::Format(_T("%d"),Dimension) << _T(",")
                 << StyleCode(wxsCPP) << _T(");\n");

            if ( DefaultSelection >= 0 && DefaultSelection < (int)ArrayChoices.GetCount() )
            {
                Code << GetVarName() << _T("->SetSelection(")
                     << wxString::Format(_T("%d"),DefaultSelection) << _T(");\n");
            }

            SetupWindowCode(Code,Language);
            return;
        }
    }


    DBGLOG(_T("wxSmith: Unknown coding language when generating radio box (id: %d)"),Language);
}

/* ************************************************************************** */
/* Function building preview item. This should simply return previewed object */
/* with all properties set-up.                                                */
/* ************************************************************************** */

wxObject* wxsRadioBox::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    // Exact argument is not used here because radio box preview is
    // exactly the same in editor and in preview window
/*
    wxString RadioBoxChoices[ArrayChoices.GetCount];

    for ( size_t i = 0; i < ArrayChoices.GetCount(); ++i )
      RadioBoxChoices[i] = ArrayChoices[i].c_str(); */

    wxRadioBox* Preview = new wxRadioBox(Parent,GetId(),Label,Pos(Parent),Size(Parent),ArrayChoices, Dimension, Style());

    if ( DefaultSelection >= 0 && DefaultSelection < (int)ArrayChoices.GetCount() )
    {
        Preview->SetSelection(DefaultSelection);
    }


    return SetupWindow(Preview,Exact);
}


/* ************************************************************************** */
/* Function enumerating all radio box-specific properties.                       */
/* ************************************************************************** */

void wxsRadioBox::EnumWidgetProperties(long Flags)
{
    WXS_STRING(wxsRadioBox,Label,0,_("Label"),_T("label"),_T(""),true,false)
    WXS_ARRAYSTRING(wxsRadioBox,ArrayChoices,0,_("Choices"),_T("content"),_T("item"))
//    WXS_ARRAYSTRINGCHECK(wxsRadioBox,ArrayString2,ArrayBool,0,_("Array string with checks"),_T("array_check"),_T("item"))
    WXS_LONG(wxsRadioBox,DefaultSelection,0x8,_("Default"),_T("default"),0)
    WXS_LONG(wxsRadioBox,Dimension,0x8,_("Dimension"),_T("dimension"),0)

}
