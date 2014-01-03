#include "ProjectOptionsManipulatorResultDlg.h"

//(*InternalHeaders(ProjectOptionsManipulatorResultDlg)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/intl.h>
#include <wx/button.h>
#include <wx/string.h>
//*)

//(*IdInit(ProjectOptionsManipulatorResultDlg)
const long ProjectOptionsManipulatorResultDlg::ID_TXT_RESULT = wxNewId();
//*)

BEGIN_EVENT_TABLE(ProjectOptionsManipulatorResultDlg,wxDialog)
	//(*EventTable(ProjectOptionsManipulatorResultDlg)
	//*)
END_EVENT_TABLE()

ProjectOptionsManipulatorResultDlg::ProjectOptionsManipulatorResultDlg(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(ProjectOptionsManipulatorResultDlg)
	wxStaticText* lblResult;
	wxBoxSizer* bszMainV;
	wxBoxSizer* bszMainH;
	wxStdDialogButtonSizer* sbzOK;

	Create(parent, id, _("Project Options Plugin - Results"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("id"));
	bszMainH = new wxBoxSizer(wxHORIZONTAL);
	bszMainV = new wxBoxSizer(wxVERTICAL);
	lblResult = new wxStaticText(this, wxID_ANY, _("Result of search:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	bszMainV->Add(lblResult, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_TxtResult = new wxTextCtrl(this, ID_TXT_RESULT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY, wxDefaultValidator, _T("ID_TXT_RESULT"));
	m_TxtResult->SetMinSize(wxSize(250,150));
	bszMainV->Add(m_TxtResult, 1, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	sbzOK = new wxStdDialogButtonSizer();
	sbzOK->AddButton(new wxButton(this, wxID_OK, wxEmptyString));
	sbzOK->Realize();
	bszMainV->Add(sbzOK, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	bszMainH->Add(bszMainV, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(bszMainH);
	bszMainH->Fit(this);
	bszMainH->SetSizeHints(this);
	//*)
}

ProjectOptionsManipulatorResultDlg::~ProjectOptionsManipulatorResultDlg()
{
	//(*Destroy(ProjectOptionsManipulatorResultDlg)
	//*)
}

void ProjectOptionsManipulatorResultDlg::ApplyResult(const wxArrayString& result)
{
  m_TxtResult->Clear();
  for (size_t i=0; i<result.Count(); ++i)
    m_TxtResult->AppendText( result[i] + wxT("\n") );
}
