#include "buildtargetpanel.h"

#include <wx/intl.h>

//(*IdInit(BuildTargetPanel)
const long BuildTargetPanel::ID_STATICTEXT1 = wxNewId();
const long BuildTargetPanel::ID_STATICTEXT3 = wxNewId();
const long BuildTargetPanel::ID_TEXTCTRL1 = wxNewId();
const long BuildTargetPanel::ID_STATICTEXT2 = wxNewId();
const long BuildTargetPanel::ID_COMBOBOX1 = wxNewId();
const long BuildTargetPanel::ID_STATICTEXT4 = wxNewId();
const long BuildTargetPanel::ID_TEXTCTRL2 = wxNewId();
const long BuildTargetPanel::ID_STATICTEXT5 = wxNewId();
const long BuildTargetPanel::ID_TEXTCTRL3 = wxNewId();
const long BuildTargetPanel::ID_CHECKBOX1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(BuildTargetPanel,wxPanel)
    //(*EventTable(BuildTargetPanel)
    //*)
END_EVENT_TABLE()

BuildTargetPanel::BuildTargetPanel(wxWindow* parent,wxWindowID id)
    : lblCompiler(0),
    cmbCompiler(0),
    txtOut(0),
    txtObjOut(0),
    chkEnableDebug(0)
{
    //(*Initialize(BuildTargetPanel)
    wxStaticText* StaticText1;
    wxStaticBoxSizer* StaticBoxSizer1;
    wxFlexGridSizer* FlexGridSizer1;
    wxStaticText* StaticText4;
    wxStaticText* StaticText5;

    Create(parent,id,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL,_T("wxPanel"));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    StaticText1 = new wxStaticText(this,ID_STATICTEXT1,_("Please setup the options for the new build target."),wxDefaultPosition,wxDefaultSize,0,_T("ID_STATICTEXT1"));
    BoxSizer1->Add(StaticText1,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,8);
    StaticText3 = new wxStaticText(this,ID_STATICTEXT3,_("Build target name:"),wxDefaultPosition,wxDefaultSize,0,_T("ID_STATICTEXT3"));
    BoxSizer1->Add(StaticText3,0,wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_TOP,8);
    txtName = new wxTextCtrl(this,ID_TEXTCTRL1,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_TEXTCTRL1"));
    BoxSizer1->Add(txtName,0,wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP,8);
    lblCompiler = new wxStaticText(this,ID_STATICTEXT2,_("Compiler:"),wxDefaultPosition,wxDefaultSize,0,_T("ID_STATICTEXT2"));
    BoxSizer1->Add(lblCompiler,0,wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_TOP,8);
    cmbCompiler = new wxComboBox(this,ID_COMBOBOX1,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,NULL,wxCB_SIMPLE|wxCB_READONLY,wxDefaultValidator,_T("ID_COMBOBOX1"));
    BoxSizer1->Add(cmbCompiler,0,wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP,8);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL,this,_("Options"));
    FlexGridSizer1 = new wxFlexGridSizer(0,2,5,5);
    FlexGridSizer1->AddGrowableCol(1);
    StaticText4 = new wxStaticText(this,ID_STATICTEXT4,_("Output dir.:"),wxDefaultPosition,wxDefaultSize,0,_T("ID_STATICTEXT4"));
    FlexGridSizer1->Add(StaticText4,0,wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
    txtOut = new wxTextCtrl(this,ID_TEXTCTRL2,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_TEXTCTRL2"));
    FlexGridSizer1->Add(txtOut,0,wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP,5);
    StaticText5 = new wxStaticText(this,ID_STATICTEXT5,_("Objects output dir.:"),wxDefaultPosition,wxDefaultSize,0,_T("ID_STATICTEXT5"));
    FlexGridSizer1->Add(StaticText5,0,wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
    txtObjOut = new wxTextCtrl(this,ID_TEXTCTRL3,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_TEXTCTRL3"));
    FlexGridSizer1->Add(txtObjOut,0,wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP,5);
    StaticBoxSizer1->Add(FlexGridSizer1,1,wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,8);
    chkEnableDebug = new wxCheckBox(this,ID_CHECKBOX1,_("Enable debugging symbols for this target"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_CHECKBOX1"));
    chkEnableDebug->SetValue(false);
    StaticBoxSizer1->Add(chkEnableDebug,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,8);
    BoxSizer1->Add(StaticBoxSizer1,0,wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP,8);
    SetSizer(BoxSizer1);
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);
    Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&BuildTargetPanel::OntxtNameText);
    //*)
}

BuildTargetPanel::~BuildTargetPanel()
{
}


void BuildTargetPanel::OntxtNameText(wxCommandEvent& event)
{
    // update output dirs based on target name
    txtOut->SetValue(_T("bin") + wxString(wxFILE_SEP_PATH) + txtName->GetValue());
    txtObjOut->SetValue(_T("obj") + wxString(wxFILE_SEP_PATH) + txtName->GetValue());
}
