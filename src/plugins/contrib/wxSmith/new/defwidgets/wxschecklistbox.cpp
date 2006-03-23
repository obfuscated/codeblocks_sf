#include "wxschecklistbox.h"

#include <wx/checklst.h>
#include <messagemanager.h>

/* ************************************************************************** */
/* Styles are processed almost like in older code - using WXS_ST_BEGIN,       */
/* WXS_ST_END, WXS_ST_CATEGORY, WXS_ST ...                                    */
/* ************************************************************************** */

WXS_ST_BEGIN(wxsCheckListBoxStyles)
    WXS_ST_CATEGORY("wxCheckListBox")
    WXS_ST_MASK(wxLB_HSCROLL,wxsSFWin,0,true) // Windows ONLY
    WXS_ST(wxLB_SINGLE)
    WXS_ST(wxLB_MULTIPLE)
    WXS_ST(wxLB_EXTENDED)
    WXS_ST(wxLB_ALWAYS_SB)
    WXS_ST(wxLB_NEEDED_SB)
    WXS_ST(wxLB_SORT)
WXS_ST_END()


/* ************************************************************************** */
/* Events have one additional argument - event type, these can be found       */
/* in WX_DIR/include/event.h                                                  */
/* ************************************************************************** */

WXS_EV_BEGIN(wxsCheckListBoxEvents)
    WXS_EVI(EVT_CHECKLISTBOX,wxEVT_COMMAND_CHECKLISTBOX_TOGGLED,wxCommandEvent,Toggled)
    WXS_EV_DEFAULTS()
WXS_EV_END()

/* ************************************************************************** */
/* Widget info                                                                */
/* ************************************************************************** */

wxsItemInfo wxsCheckListBox::Info =
{
    _T("wxCheckListBox"),       // Name of class
    wxsTWidget,                 // Type, always wxsTWidget for widget classes
    _("wxWidgets license"),     // License, any type
    _("wxWidgets team"),        // Author
    _T(""),                     // No default e-mail for standard widgets
    _T("www.wxwidgets.org"),    // Site
    _T("Standard"),             // Groud for widget, note that _T() instead of _() is used
    80,                         // Button is one of most commonly used widgets - we give it high priority
    _T("CheckListBox"),         // Standard prefix for variable names and identifiers
    2, 6,                       // Widget version
    NULL,                       // Bitmaps will be loaded later in manager
    NULL,                       // --------------------''-------------------
    0                           // --------------------''-------------------
};


/* ************************************************************************** */
/* Constructur for wxsWidget need style set, event set and widget info, so    */
/* these things should be created before ctor.                                */
/* ************************************************************************** */

wxsCheckListBox::wxsCheckListBox(wxsWindowRes* Resource):
    wxsWidget(
        Resource,                   // Resource is passed to wxsItem's constructor
        wxsBaseProperties::flAll,   // Using all base properties
        &Info,                      // Taking local info
        wxsCheckListBoxEvents,      // Pointer to local events
        wxsCheckListBoxStyles,      // Pointer to local styles
        _T(""))                      // Default style = 0
                                    // Initializing widget with some default (but not necessarily clean) values,
                                    // these values will be used when creating new widgets
{}


/* ************************************************************************** */
/* Function building code - it should append new code generating this widget  */
/* to the end of prevous code. Currently it support only C++ langage but some */
/* other languages may be added in future.                                    */
/* ************************************************************************** */

void wxsCheckListBox::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
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
            Code<< GetVarName() << _T(" = new wxCheckListBox(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(");\n");

            for ( size_t i = 0; i <  ArrayChoices.GetCount(); ++i )
            {
                if ( ArrayChecks[i] )
                {
                    Code << GetVarName() << _T("->Check( ");
                }
                Code << GetVarName() << _T("->Append(") << wxsGetWxString(ArrayChoices[i]) << _T(")");
                if ( ArrayChecks[i] )
                {
                    Code << GetVarName() << _T(" )");
                }
                Code << _T(";\n");
            }
            SetupWindowCode(Code,Language);
            return;
        }
    }

    DBGLOG(_T("wxSmith: Unknown coding language when generating check listbox (id: %d)"),Language);
}

/* ************************************************************************** */
/* Function building preview item. This should simply return previewed object */
/* with all properties set-up.                                                */
/* ************************************************************************** */

wxObject* wxsCheckListBox::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    // Exact argument is not used here because checklistbox preview is
    // exactly the same in editor and in preview window
    wxCheckListBox* Preview = new wxCheckListBox(Parent,GetId(),Pos(Parent),Size(Parent),0,NULL,Style());

    for ( size_t i = 0; i < ArrayChoices.GetCount(); ++i )
    {
        int Val = Preview->Append(ArrayChoices[i]);
        if ( ArrayChecks[i] )
        {
            Preview->Check(Val);
        }
    }
    return SetupWindow(Preview,Exact);
}


/* ************************************************************************** */
/* Function enumerating all checklistbox-specific properties.                       */
/* ************************************************************************** */

void wxsCheckListBox::EnumWidgetProperties(long Flags)
{
    WXS_ARRAYSTRINGCHECK(wxsCheckListBox,ArrayChoices,ArrayChecks,0,_("Choices"),_T("content"),_T("item"));
}
