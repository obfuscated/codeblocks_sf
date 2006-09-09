#include "wxscombobox.h"

#include <wx/combobox.h>
#include <messagemanager.h>

WXS_ST_BEGIN(wxsComboBoxStyles)
    WXS_ST_CATEGORY("wxComboBox")
    WXS_ST_MASK(wxCB_SIMPLE,wxsSFWin,0,true)
    WXS_ST(wxCB_READONLY)
    WXS_ST(wxCB_SORT)
    WXS_ST(wxCB_DROPDOWN)
WXS_ST_END()

WXS_EV_BEGIN(wxsComboBoxEvents)
    WXS_EVI(EVT_COMBOBOX,wxEVT_COMMAND_COMBOBOX_SELECTED,wxCommandEvent,Select)
    WXS_EVI(EVT_TEXT,wxEVT_COMMAND_TEXT_UPDATED,wxCommandEvent,Text)
    WXS_EVI(EVT_TEXT_ENTER,wxEVT_COMMAND_TEXT_ENTER,wxCommandEvent,TextEnter)
    WXS_EV_DEFAULTS()
WXS_EV_END()

wxsItemInfo wxsComboBox::Info =
{
    _T("wxComboBox"),
    wxsTWidget,
    _("wxWidgets license"),
    _("wxWidgets team"),
    _T(""),
    _T("www.wxwidgets.org"),
    _T("Standard"),
    70,
    _T("ComboBox"),
    2, 6,
    NULL,
    NULL,
    0
};

wxsComboBox::wxsComboBox(wxsWindowRes* Resource):
    wxsWidget(
        Resource,
        wxsBaseProperties::flAll,
        &Info,
        wxsComboBoxEvents,
        wxsComboBoxStyles,
        _T("")),
    DefaultSelection(-1)
{}

void wxsComboBox::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code<< GetVarName() << _T(" = new wxComboBox(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << wxsGetWxString(_T("")) << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(");\n");

            for ( size_t i = 0; i <  ArrayChoices.GetCount(); ++i )
            {
                if ( DefaultSelection == (int)i )
                {
                    Code << GetVarName() << _T("->SetSelection( ");
                }
                Code << GetVarName() << _T("->Append(") << wxsGetWxString(ArrayChoices[i]) << _T(")");
                if ( DefaultSelection == (int)i )
                {
                    Code << _T(" )");
                }
                Code << _T(";\n");
            }

            SetupWindowCode(Code,Language);
            return;
        }
    }

    wxsLANGMSG(wxsComboBox::BuildCreatingCode,Language);
}

wxObject* wxsComboBox::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    wxComboBox* Preview = new wxComboBox(Parent,GetId(),_T(""),Pos(Parent),Size(Parent),ArrayChoices, Style());

    for ( size_t i = 0; i <  ArrayChoices.GetCount(); ++i )
    {
        int Val = Preview->Append(ArrayChoices[i]);
        if ( (int)i == DefaultSelection )
        {
            Preview->SetSelection(Val);
        }
    }

    return SetupWindow(Preview,Exact);
}

void wxsComboBox::EnumWidgetProperties(long Flags)
{
      WXS_ARRAYSTRING(wxsComboBox,ArrayChoices,0,_("Choices"),_T("content"),_T("item"))
      WXS_LONG(wxsComboBox,DefaultSelection,0,_("Default"),_T("default"),0)
}

void wxsComboBox::EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/combobox.h>")); return;
    }

    wxsLANGMSG(wxsComboBox::EnumDeclFiles,Language);
}
