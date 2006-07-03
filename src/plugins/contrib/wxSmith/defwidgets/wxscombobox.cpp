#include "../wxsheaders.h"
#include "wxscombobox.h"
#include <wx/combobox.h>
WXS_ST_BEGIN(wxsComboBoxStyles)
    WXS_ST_CATEGORY("wxComboBox")
    WXS_ST_MASK(wxCB_SIMPLE,wxsSFWin,0,true)
    WXS_ST(wxCB_READONLY)
    WXS_ST(wxCB_SORT)
    WXS_ST(wxCB_DROPDOWN)
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
        WXS_THIS = new wxComboBox(WXS_PARENT,WXS_ID,_T(""),WXS_POS,WXS_SIZE,0,0,WXS_STYLE,wxDefaultValidator,WXS_NAME);
	)

    wxsDWDefIntX(defaultChoice,"selection","",-1)
    wxsDWDefStrArrayX(arrayChoices,"content","item","Choices",defaultChoice,wxCB_SORT)

wxsDWDefineEnd()

wxWindow* wxsComboBox::MyCreatePreview(wxWindow* Parent)
{
    wxComboBox* Combo = new wxComboBox(Parent,-1,_T(""),GetPosition(),GetSize(),arrayChoices,GetStyle(),wxDefaultValidator,GetName());
    if ( defaultChoice >= 0 && defaultChoice < (int)arrayChoices.Count() )
    {
        Combo->SetSelection(defaultChoice);
    }
    PreviewApplyDefaults(Combo);
    return Combo;
}

wxString wxsComboBox::GetProducingCode(const wxsCodeParams& Params)
{
    wxString Code;
    Code.Printf(_T("%s = new wxComboBox(%s,%s,_T(\"\"),%s,%s,0,NULL,%s,wxDefaultValidator,%s);\n"),
            Params.VarName.c_str(),
            Params.ParentName.c_str(),
            Params.IdName.c_str(),
            Params.Pos.c_str(),
            Params.Size.c_str(),
            Params.Style.c_str(),
            Params.Name.c_str());
    for ( size_t i = 0; i <  arrayChoices.Count(); ++i )
    {
        Code << wxString::Format(_T("%s->Append(%s);\n"),
            Params.VarName.c_str(),
            wxsGetWxString(arrayChoices[i]).c_str());
    }

    if ( defaultChoice >= 0 && defaultChoice < (int)arrayChoices.Count() )
    {
        Code << wxString::Format(_T("%s->SetSelection(%d);\n"),
            Params.VarName.c_str(),
            defaultChoice);
    }

    Code << Params.InitCode;

    return Code;
}
