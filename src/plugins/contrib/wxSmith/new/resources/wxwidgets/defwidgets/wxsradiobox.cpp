#include "wxsradiobox.h"

#include <wx/radiobox.h>
#include <messagemanager.h>


WXS_ST_BEGIN(wxsRadioBoxStyles)
    WXS_ST_CATEGORY("wxRadioBox")
    WXS_ST(wxRA_SPECIFY_ROWS)
    WXS_ST(wxRA_SPECIFY_COLS)
// NOTE (cyberkoa##): wxRA_HORIZONTAL & wxRA_VERTICAL is not in HELP file but in wxMSW's XRC
    WXS_ST_MASK(wxRA_HORIZONTAL,wxsSFAll,0,false)
    WXS_ST_MASK(wxRA_VERTICAL,wxsSFAll,0,false)
    WXS_ST_MASK(wxRA_USE_CHECKBOX,wxsSFPALMOS,0,true)
WXS_ST_END()


WXS_EV_BEGIN(wxsRadioBoxEvents)
    WXS_EVI(EVT_RADIOBOX,wxEVT_COMMAND_RADIOBOX_SELECTED,wxCommandEvent,Select)
    WXS_EV_DEFAULTS()
WXS_EV_END()


wxsItemInfo wxsRadioBox::Info =
{
    _T("wxRadioBox"),
    wxsTWidget,
    _("wxWidgets license"),
    _("wxWidgets team"),
    _T(""),
    _T("www.wxwidgets.org"),
    _T("Standard"),
    60,
    _T("RadioBox"),
    2, 6,
    NULL,
    NULL,
    0
};



wxsRadioBox::wxsRadioBox(wxsWindowRes* Resource):
    wxsWidget(
        Resource,
        wxsBaseProperties::flAll,
        &Info,
        wxsRadioBoxEvents,
        wxsRadioBoxStyles,
        _T("")),
    Label(Label),
    DefaultSelection(-1),
    Dimension(1)
{}



void wxsRadioBox::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {

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

    wxsLANGMSG(wxsRadioBox::BuildCreatingCode,Language);
}


wxObject* wxsRadioBox::DoBuildPreview(wxWindow* Parent,bool Exact)
{

    wxRadioBox* Preview = new wxRadioBox(Parent,GetId(),Label,Pos(Parent),Size(Parent),ArrayChoices, Dimension, Style());

    if ( DefaultSelection >= 0 && DefaultSelection < (int)ArrayChoices.GetCount() )
    {
        Preview->SetSelection(DefaultSelection);
    }


    return SetupWindow(Preview,Exact);
}



void wxsRadioBox::EnumWidgetProperties(long Flags)
{
    WXS_STRING(wxsRadioBox,Label,0,_("Label"),_T("label"),_T(""),true,false)
    WXS_ARRAYSTRING(wxsRadioBox,ArrayChoices,0,_("Choices"),_T("content"),_T("item"))
//    WXS_ARRAYSTRINGCHECK(wxsRadioBox,ArrayString2,ArrayBool,0,_("Array string with checks"),_T("array_check"),_T("item"))
    WXS_LONG(wxsRadioBox,DefaultSelection,0x8,_("Default"),_T("default"),0)
    WXS_LONG(wxsRadioBox,Dimension,0x8,_("Dimension"),_T("dimension"),0)

}


void wxsRadioBox::EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/radiobox.h>")); return;
    }

    wxsLANGMSG(wxsRadioBox::EnumDeclFiles,Language);
}
