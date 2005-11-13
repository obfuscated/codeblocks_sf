#include "../wxsheaders.h"
#include "wxscombobox.h"
#include <wx/combobox.h>
WXS_ST_BEGIN(wxsComboBoxStyles)
    WXS_ST_CATEGORY("wxComboBox")
#ifdef __WXMSW__
    WXS_ST(wxCB_SIMPLE) // Windows ONLY
#endif
    WXS_ST(wxCB_READONLY)
    WXS_ST(wxCB_SORT)
    WXS_ST(wxCB_DROPDOWN)
    WXS_ST_DEFAULTS()
WXS_ST_END(wxsComboBoxStyles)

WXS_EV_BEGIN(wxsComboBoxEvents)
    WXS_EVI(EVT_COMBOBOX,wxCommandEvent,Select)
    WXS_EVI(EVT_TEXT,wxCommandEvent,Text)
    WXS_EVI(EVT_TEXT_ENTER,wxCommandEvent,TextEnter)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsComboBoxEvents)

// default constructor for wxCombobox
//wxComboBox(wxWindow* parent, wxWindowID id, const wxString& value = "", const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n, const wxString choices[], long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "comboBox")

// Constructor 2 for wxCombobox, available only in wxwidgets > 2.5
//wxComboBox(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size, const wxArrayString& choices, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "comboBox")


wxsDWDefineBegin(wxsComboBoxBase,wxComboBox,
        ThisWidget = new wxComboBox(parent,id,_T(""),pos,size,0,0,style);
	)

    #ifdef __NO_PROPGRID
        wxsDWDefIntX(defaultChoice,"selection","Default:",-1)
    #else
        wxsDWDefIntX(defaultChoice,"selection","",-1)
    #endif
    wxsDWDefStrArrayX(arrayChoices,"content","item","Choices",defaultChoice,wxCB_SORT)

wxsDWDefineEnd()

wxWindow* wxsComboBox::MyCreatePreview(wxWindow* Parent)
{
    wxComboBox* Combo = new wxComboBox(Parent,-1,_T(""),GetPosition(),GetSize(),arrayChoices,GetStyle());
    if ( defaultChoice >= 0 && defaultChoice < (int)arrayChoices.Count() )
    {
        Combo->SetSelection(defaultChoice);
    }
    PreviewApplyDefaults(Combo);
    return Combo;
}

wxString wxsComboBox::GetProducingCode(wxsCodeParams& Params)
{
    wxString Code;
    const CodeDefines& CDefs = GetCodeDefines();
    Code.Printf(_T("%s = new wxComboBox(%s,%s,_T(\"\"),%s,%s,0,NULL,%s);\n"),
            GetBaseProperties().VarName.c_str(),
            Params.ParentName.c_str(),
            GetBaseProperties().IdName.c_str(),
            CDefs.Pos.c_str(),
            CDefs.Size.c_str(),
            CDefs.Style.c_str());
    for ( size_t i = 0; i <  arrayChoices.Count(); ++i )
    {
        Code << wxString::Format(_T("%s->Append(%s);\n"),
            GetBaseProperties().VarName.c_str(),
            GetWxString(arrayChoices[i]).c_str());
    }

    if ( defaultChoice >= 0 && defaultChoice < (int)arrayChoices.Count() )
    {
        Code << wxString::Format(_T("%s->SetSelection(%d);\n"),
            GetBaseProperties().VarName.c_str(),
            defaultChoice);
    }

    Code << CDefs.InitCode;

    return Code;
}
