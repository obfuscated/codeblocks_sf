#include "buildtargetpanel.h"

BEGIN_EVENT_TABLE(BuildTargetPanel,wxPanel)
	//(*EventTable(BuildTargetPanel)
	EVT_TEXT(ID_TEXTCTRL1,BuildTargetPanel::OntxtNameText)
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

	Create(parent,id,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL);
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	StaticText1 = new wxStaticText(this,ID_STATICTEXT1,_("Please setup the options for the new build target."),wxDefaultPosition,wxDefaultSize,0);
	StaticText3 = new wxStaticText(this,ID_STATICTEXT3,_("Build target name:"),wxDefaultPosition,wxDefaultSize,0);
	txtName = new wxTextCtrl(this,ID_TEXTCTRL1,_T(""),wxDefaultPosition,wxDefaultSize,0);
	if ( 0 ) txtName->SetMaxLength(0);
	lblCompiler = new wxStaticText(this,ID_STATICTEXT2,_("Compiler:"),wxDefaultPosition,wxDefaultSize,0);
	cmbCompiler = new wxComboBox(this,ID_COMBOBOX1,_T(""),wxDefaultPosition,wxDefaultSize,0,NULL,wxCB_SIMPLE|wxCB_READONLY);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL,this,_("Options"));
	FlexGridSizer1 = new wxFlexGridSizer(0,2,5,5);
	FlexGridSizer1->AddGrowableCol(1);
	StaticText4 = new wxStaticText(this,ID_STATICTEXT4,_("Output dir.:"),wxDefaultPosition,wxDefaultSize,0);
	txtOut = new wxTextCtrl(this,ID_TEXTCTRL2,_T(""),wxDefaultPosition,wxDefaultSize,0);
	if ( 0 ) txtOut->SetMaxLength(0);
	StaticText5 = new wxStaticText(this,ID_STATICTEXT5,_("Objects output dir.:"),wxDefaultPosition,wxDefaultSize,0);
	txtObjOut = new wxTextCtrl(this,ID_TEXTCTRL3,_T(""),wxDefaultPosition,wxDefaultSize,0);
	if ( 0 ) txtObjOut->SetMaxLength(0);
	FlexGridSizer1->Add(StaticText4,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxEXPAND,5);
	FlexGridSizer1->Add(txtOut,0,wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,5);
	FlexGridSizer1->Add(StaticText5,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxEXPAND,5);
	FlexGridSizer1->Add(txtObjOut,0,wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,5);
	chkEnableDebug = new wxCheckBox(this,ID_CHECKBOX1,_("Enable debugging symbols for this target"),wxDefaultPosition,wxDefaultSize,0);
	chkEnableDebug->SetValue(false);
	StaticBoxSizer1->Add(FlexGridSizer1,1,wxALL|wxALIGN_CENTER|wxEXPAND,8);
	StaticBoxSizer1->Add(chkEnableDebug,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,8);
	BoxSizer1->Add(StaticText1,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP,8);
	BoxSizer1->Add(StaticText3,0,wxLEFT|wxRIGHT|wxTOP|wxALIGN_LEFT|wxALIGN_TOP,8);
	BoxSizer1->Add(txtName,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(lblCompiler,0,wxLEFT|wxRIGHT|wxTOP|wxALIGN_LEFT|wxALIGN_TOP,8);
	BoxSizer1->Add(cmbCompiler,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(StaticBoxSizer1,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	this->SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
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
