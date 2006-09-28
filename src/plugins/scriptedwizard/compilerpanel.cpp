#include "compilerpanel.h"

BEGIN_EVENT_TABLE(CompilerPanel,wxPanel)
	//(*EventTable(CompilerPanel)
	EVT_CHECKBOX(ID_CHECKBOX1,CompilerPanel::OnDebugChange)
	EVT_CHECKBOX(ID_CHECKBOX3,CompilerPanel::OnReleaseChange)
	//*)
END_EVENT_TABLE()

CompilerPanel::CompilerPanel(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(CompilerPanel)
	wxBoxSizer* BoxSizer1;
	wxStaticText* StaticText2;
	wxBoxSizer* BoxSizer2;
	wxFlexGridSizer* FlexGridSizer1;
	wxStaticText* StaticText3;
	wxStaticText* StaticText4;
	wxBoxSizer* BoxSizer3;
	wxFlexGridSizer* FlexGridSizer2;
	wxStaticText* StaticText7;
	wxStaticText* StaticText8;
	
	Create(parent,id,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL,_T(""));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	StaticText1 = new wxStaticText(this,ID_STATICTEXT1,_("Please select the compiler to use and which configurations\nyou want enabled in your project."),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT1"));
	StaticText2 = new wxStaticText(this,ID_STATICTEXT2,_("Compiler:"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT2"));
	cmbCompiler = new wxComboBox(this,ID_COMBOBOX1,_T(""),wxDefaultPosition,wxDefaultSize,0,NULL,wxCB_SIMPLE|wxCB_READONLY,wxDefaultValidator,_("ID_COMBOBOX1"));
	BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
	chkConfDebug = new wxCheckBox(this,ID_CHECKBOX1,_("Create \"Debug\" configuration:"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_CHECKBOX1"));
	chkConfDebug->SetValue(true);
	txtDbgName = new wxTextCtrl(this,ID_TEXTCTRL3,_T(""),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_TEXTCTRL3"));
	if ( 0 ) txtDbgName->SetMaxLength(0);
	BoxSizer4->Add(chkConfDebug,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	BoxSizer4->Add(txtDbgName,1,wxLEFT|wxALIGN_CENTER,4);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("\"Debug\" options"));
	FlexGridSizer1 = new wxFlexGridSizer(0,2,5,5);
	FlexGridSizer1->AddGrowableCol(1);
	StaticText3 = new wxStaticText(this,ID_STATICTEXT3,_("Output dir.:"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT3"));
	txtDbgOut = new wxTextCtrl(this,ID_TEXTCTRL1,_T(""),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_TEXTCTRL1"));
	if ( 0 ) txtDbgOut->SetMaxLength(0);
	StaticText4 = new wxStaticText(this,ID_STATICTEXT4,_("Objects output dir.:"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT4"));
	txtDbgObjOut = new wxTextCtrl(this,ID_TEXTCTRL2,_T(""),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_TEXTCTRL2"));
	if ( 0 ) txtDbgObjOut->SetMaxLength(0);
	FlexGridSizer1->Add(StaticText3,1,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	FlexGridSizer1->Add(txtDbgOut,1,wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,5);
	FlexGridSizer1->Add(StaticText4,1,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	FlexGridSizer1->Add(txtDbgObjOut,1,wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,5);
	StaticBoxSizer1->Add(FlexGridSizer1,1,wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer2->Add(16,-1,0);
	BoxSizer2->Add(StaticBoxSizer1,1,wxALIGN_LEFT|wxALIGN_TOP,5);
	BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
	chkConfRelease = new wxCheckBox(this,ID_CHECKBOX3,_("Create \"Release\" configuration:"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_CHECKBOX3"));
	chkConfRelease->SetValue(true);
	txtRelName = new wxTextCtrl(this,ID_TEXTCTRL4,_T(""),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_TEXTCTRL4"));
	if ( 0 ) txtRelName->SetMaxLength(0);
	BoxSizer5->Add(chkConfRelease,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	BoxSizer5->Add(txtRelName,1,wxLEFT|wxALIGN_CENTER,4);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("\"Release\" options"));
	FlexGridSizer2 = new wxFlexGridSizer(0,2,5,5);
	FlexGridSizer2->AddGrowableCol(1);
	StaticText7 = new wxStaticText(this,ID_STATICTEXT7,_("Output dir.:"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT7"));
	txtRelOut = new wxTextCtrl(this,ID_TEXTCTRL5,_T(""),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_TEXTCTRL5"));
	if ( 0 ) txtRelOut->SetMaxLength(0);
	StaticText8 = new wxStaticText(this,ID_STATICTEXT8,_("Objects output dir.:"),wxDefaultPosition,wxDefaultSize,0,_("ID_STATICTEXT8"));
	txtRelObjOut = new wxTextCtrl(this,ID_TEXTCTRL6,_T(""),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_TEXTCTRL6"));
	if ( 0 ) txtRelObjOut->SetMaxLength(0);
	FlexGridSizer2->Add(StaticText7,1,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	FlexGridSizer2->Add(txtRelOut,1,wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,5);
	FlexGridSizer2->Add(StaticText8,1,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	FlexGridSizer2->Add(txtRelObjOut,1,wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,5);
	StaticBoxSizer2->Add(FlexGridSizer2,1,wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer3->Add(16,-1,0);
	BoxSizer3->Add(StaticBoxSizer2,1,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,5);
	BoxSizer1->Add(StaticText1,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(StaticText2,0,wxLEFT|wxRIGHT|wxTOP|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(cmbCompiler,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(BoxSizer4,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(BoxSizer2,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(BoxSizer5,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,8);
	BoxSizer1->Add(BoxSizer3,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,8);
	this->SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	//*)
}

CompilerPanel::~CompilerPanel()
{
}


void CompilerPanel::OnDebugChange(wxCommandEvent& event)
{
    if (!event.IsChecked() && !chkConfRelease->IsChecked())
    {
        cbMessageBox(_("At least one configuration must be set..."), _("Notice"), wxICON_WARNING);
        chkConfDebug->SetValue(true);
        return;
    }
    txtDbgName->Enable(event.IsChecked());
    txtDbgOut->Enable(event.IsChecked());
    txtDbgObjOut->Enable(event.IsChecked());
}

void CompilerPanel::OnReleaseChange(wxCommandEvent& event)
{
    if (!event.IsChecked() && !chkConfDebug->IsChecked())
    {
        cbMessageBox(_("At least one configuration must be set..."), _("Notice"), wxICON_WARNING);
        chkConfRelease->SetValue(true);
        return;
    }
    txtRelName->Enable(event.IsChecked());
    txtRelOut->Enable(event.IsChecked());
    txtRelObjOut->Enable(event.IsChecked());
}

void CompilerPanel::EnableConfigurationTargets(bool en)
{
    chkConfRelease->Show(en);
    txtRelName->Show(en);
    txtRelOut->Show(en);
    txtRelObjOut->Show(en);
    StaticBoxSizer1->Show(en);
    chkConfDebug->Show(en);
    txtDbgName->Show(en);
    txtDbgOut->Show(en);
    txtDbgObjOut->Show(en);
    StaticBoxSizer2->Show(en);

	StaticText1->SetLabel(en
        ? _("Please select the compiler to use and which configurations\nyou want enabled in your project.")
        : _("Please select the compiler to use."));
}
