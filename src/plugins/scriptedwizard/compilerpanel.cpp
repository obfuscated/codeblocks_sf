/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk.h"
#ifndef CB_PRECOMP
#include <wx/event.h>
//(*InternalHeadersPCH(CompilerPanel)
#include <wx/string.h>
#include <wx/intl.h>
//*)
#include "globals.h"
#endif
#include "compilerpanel.h"

//(*IdInit(CompilerPanel)
const long CompilerPanel::ID_STATICTEXT1 = wxNewId();
const long CompilerPanel::ID_STATICTEXT2 = wxNewId();
const long CompilerPanel::ID_COMBOBOX1 = wxNewId();
const long CompilerPanel::ID_CHECKBOX1 = wxNewId();
const long CompilerPanel::ID_TEXTCTRL3 = wxNewId();
const long CompilerPanel::ID_STATICTEXT3 = wxNewId();
const long CompilerPanel::ID_TEXTCTRL1 = wxNewId();
const long CompilerPanel::ID_STATICTEXT4 = wxNewId();
const long CompilerPanel::ID_TEXTCTRL2 = wxNewId();
const long CompilerPanel::ID_CHECKBOX3 = wxNewId();
const long CompilerPanel::ID_TEXTCTRL4 = wxNewId();
const long CompilerPanel::ID_STATICTEXT7 = wxNewId();
const long CompilerPanel::ID_TEXTCTRL5 = wxNewId();
const long CompilerPanel::ID_STATICTEXT8 = wxNewId();
const long CompilerPanel::ID_TEXTCTRL6 = wxNewId();
//*)

BEGIN_EVENT_TABLE(CompilerPanel,wxPanel)
	//(*EventTable(CompilerPanel)
	//*)
END_EVENT_TABLE()

CompilerPanel::CompilerPanel(wxWindow* parent, wxWindow* parentDialog) :
    m_parentDialog(parentDialog)
{
	//(*Initialize(CompilerPanel)
	wxFlexGridSizer* FlexGridSizer1;
	wxFlexGridSizer* FlexGridSizer2;
	wxBoxSizer* BoxSizer3;
	wxBoxSizer* BoxSizer2;
	wxStaticText* StaticText3;
	wxStaticText* StaticText8;
	wxStaticText* StaticText7;
	wxStaticText* StaticText4;
	wxBoxSizer* BoxSizer1;
	wxStaticText* StaticText2;

	Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Please select the compiler to use and which configurations\nyou want enabled in your project."), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	BoxSizer1->Add(StaticText1, 0, wxALL|wxEXPAND, 8);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Compiler:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	BoxSizer1->Add(StaticText2, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 8);
	cmbCompiler = new wxComboBox(this, ID_COMBOBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_SIMPLE|wxCB_READONLY, wxDefaultValidator, _T("ID_COMBOBOX1"));
	BoxSizer1->Add(cmbCompiler, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 8);
	BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
	chkConfDebug = new wxCheckBox(this, ID_CHECKBOX1, _("Create \"Debug\" configuration:"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	chkConfDebug->SetValue(true);
	BoxSizer4->Add(chkConfDebug, 0, wxALIGN_CENTER_VERTICAL, 5);
	txtDbgName = new wxTextCtrl(this, ID_TEXTCTRL3, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL3"));
	BoxSizer4->Add(txtDbgName, 1, wxLEFT|wxALIGN_CENTER_VERTICAL, 4);
	BoxSizer1->Add(BoxSizer4, 0, wxALL|wxEXPAND, 8);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	BoxSizer2->Add(16,-1,0, wxALIGN_TOP, 5);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("\"Debug\" options"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 2, 5, 5);
	FlexGridSizer1->AddGrowableCol(1);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Output dir.:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer1->Add(StaticText3, 1, wxALIGN_CENTER_VERTICAL, 5);
	txtDbgOut = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	FlexGridSizer1->Add(txtDbgOut, 1, wxEXPAND, 5);
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Objects output dir.:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	FlexGridSizer1->Add(StaticText4, 1, wxALIGN_CENTER_VERTICAL, 5);
	txtDbgObjOut = new wxTextCtrl(this, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	FlexGridSizer1->Add(txtDbgObjOut, 1, wxEXPAND, 5);
	StaticBoxSizer1->Add(FlexGridSizer1, 1, wxEXPAND, 5);
	BoxSizer2->Add(StaticBoxSizer1, 1, wxALIGN_TOP, 5);
	BoxSizer1->Add(BoxSizer2, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 8);
	BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
	chkConfRelease = new wxCheckBox(this, ID_CHECKBOX3, _("Create \"Release\" configuration:"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX3"));
	chkConfRelease->SetValue(true);
	BoxSizer5->Add(chkConfRelease, 0, wxALIGN_CENTER_VERTICAL, 5);
	txtRelName = new wxTextCtrl(this, ID_TEXTCTRL4, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL4"));
	BoxSizer5->Add(txtRelName, 1, wxLEFT|wxALIGN_CENTER_VERTICAL, 4);
	BoxSizer1->Add(BoxSizer5, 0, wxALL|wxEXPAND, 8);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	BoxSizer3->Add(16,-1,0, wxALIGN_TOP, 5);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("\"Release\" options"));
	FlexGridSizer2 = new wxFlexGridSizer(0, 2, 5, 5);
	FlexGridSizer2->AddGrowableCol(1);
	StaticText7 = new wxStaticText(this, ID_STATICTEXT7, _("Output dir.:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
	FlexGridSizer2->Add(StaticText7, 1, wxALIGN_CENTER_VERTICAL, 5);
	txtRelOut = new wxTextCtrl(this, ID_TEXTCTRL5, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL5"));
	FlexGridSizer2->Add(txtRelOut, 1, wxEXPAND, 5);
	StaticText8 = new wxStaticText(this, ID_STATICTEXT8, _("Objects output dir.:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT8"));
	FlexGridSizer2->Add(StaticText8, 1, wxALIGN_CENTER_VERTICAL, 5);
	txtRelObjOut = new wxTextCtrl(this, ID_TEXTCTRL6, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL6"));
	FlexGridSizer2->Add(txtRelObjOut, 1, wxEXPAND, 5);
	StaticBoxSizer2->Add(FlexGridSizer2, 1, wxEXPAND, 5);
	BoxSizer3->Add(StaticBoxSizer2, 1, wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(BoxSizer3, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 8);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);

	Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerPanel::OnDebugChange);
	Connect(ID_CHECKBOX3,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerPanel::OnReleaseChange);
	//*)
}

CompilerPanel::~CompilerPanel()
{
}


void CompilerPanel::OnDebugChange(wxCommandEvent& event)
{
    if (!event.IsChecked() && !chkConfRelease->IsChecked())
    {
        cbMessageBox(_("At least one configuration must be set..."), _("Notice"), wxICON_WARNING, m_parentDialog);
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
        cbMessageBox(_("At least one configuration must be set..."), _("Notice"), wxICON_WARNING, m_parentDialog);
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
