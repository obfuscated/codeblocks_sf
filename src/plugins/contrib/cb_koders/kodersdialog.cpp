/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <wx/string.h>
#include "kodersdialog.h"

//(*InternalHeaders(KodersDialog)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(KodersDialog)
const long KodersDialog::ID_LBL_INTRO = wxNewId();
const long KodersDialog::ID_TXT_SEARCH = wxNewId();
const long KodersDialog::ID_BTN_SEARCH = wxNewId();
const long KodersDialog::ID_LBL_FILTER = wxNewId();
const long KodersDialog::ID_CHO_LANGUAGES = wxNewId();
//*)

BEGIN_EVENT_TABLE(KodersDialog,wxScrollingDialog)
	//(*EventTable(KodersDialog)
	//*)
END_EVENT_TABLE()

KodersDialog::KodersDialog(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(KodersDialog)
	Create(parent, id, _("OLOHO query"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	bszMain = new wxBoxSizer(wxVERTICAL);
	bszIntro = new wxBoxSizer(wxHORIZONTAL);
	lblIntro = new wxStaticText(this, ID_LBL_INTRO, _("Specify search to query the OLOHO webpage:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_LBL_INTRO"));
	bszIntro->Add(lblIntro, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	bszMain->Add(bszIntro, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
	bszSearch = new wxBoxSizer(wxHORIZONTAL);
	txtSearch = new wxTextCtrl(this, ID_TXT_SEARCH, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TXT_SEARCH"));
	txtSearch->SetToolTip(_("Enter keyword to search for (at koders)"));
	bszSearch->Add(txtSearch, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	btnSearch = new wxButton(this, ID_BTN_SEARCH, _("Search"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BTN_SEARCH"));
	btnSearch->SetDefault();
	btnSearch->SetToolTip(_("Click to search at the koders webpage..."));
	bszSearch->Add(btnSearch, 0, wxLEFT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	bszMain->Add(bszSearch, 0, wxTOP|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	bszFilter = new wxBoxSizer(wxHORIZONTAL);
	lblFilter = new wxStaticText(this, ID_LBL_FILTER, _("Filter:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_LBL_FILTER"));
	bszFilter->Add(lblFilter, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	choLanguages = new wxChoice(this, ID_CHO_LANGUAGES, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHO_LANGUAGES"));
	choLanguages->Append(_("All Languages"));
	choLanguages->Append(_("Ada"));
	choLanguages->Append(_("ASP"));
	choLanguages->Append(_("Assembler"));
	choLanguages->Append(_("C"));
	choLanguages->Append(_("C#"));
	choLanguages->Append(_("Cpp"));
	choLanguages->Append(_("ColdFusion"));
	choLanguages->Append(_("Delphi"));
	choLanguages->Append(_("Eiffel"));
	choLanguages->Append(_("Erlang"));
	choLanguages->Append(_("Fortran"));
	choLanguages->Append(_("Java"));
	choLanguages->Append(_("JavaScript"));
	choLanguages->Append(_("JSP"));
	choLanguages->Append(_("Lisp"));
	choLanguages->Append(_("Lua"));
	choLanguages->Append(_("Mathematica"));
	choLanguages->Append(_("Matlab"));
	choLanguages->Append(_("ObjectiveC"));
	choLanguages->Append(_("Perl"));
	choLanguages->Append(_("PHP"));
	choLanguages->Append(_("Prolog"));
	choLanguages->Append(_("Python"));
	choLanguages->Append(_("Ruby"));
	choLanguages->Append(_("Scheme"));
	choLanguages->Append(_("Smalltalk"));
	choLanguages->Append(_("SQL"));
	choLanguages->Append(_("Tcl"));
	choLanguages->Append(_("VB"));
	choLanguages->Append(_("VB.NET"));
	choLanguages->SetToolTip(_("Limit search to a specific programming language"));
	bszFilter->Add(choLanguages, 1, wxLEFT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	bszMain->Add(bszFilter, 0, wxTOP|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(bszMain);
	bszMain->Fit(this);
	bszMain->SetSizeHints(this);
	Center();

	Connect(ID_BTN_SEARCH,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&KodersDialog::OnBtnSearchClick);
	//*)
}

KodersDialog::~KodersDialog()
{
}

void KodersDialog::SetSearch(const wxString &search)
{
  if (txtSearch)
    txtSearch->SetValue(search);
}

wxString KodersDialog::GetSearch() const
{
  wxString search(_T(""));

  if (txtSearch)
    search = txtSearch->GetValue();

  return search;
}

wxString KodersDialog::GetLanguage() const
{
  wxString language(_("*"));

  if (choLanguages && (choLanguages->GetStringSelection() != _("All Languages")))
    language = choLanguages->GetStringSelection();

  return language;
}

void KodersDialog::OnBtnSearchClick(wxCommandEvent& /*event*/)
{
  EndModal(wxID_OK);
}
