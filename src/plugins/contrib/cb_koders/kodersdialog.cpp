
#include <wx/string.h>
#include "kodersdialog.h"

BEGIN_EVENT_TABLE(KodersDialog,wxDialog)
	//(*EventTable(KodersDialog)
	EVT_BUTTON(ID_BTN_SEARCH,KodersDialog::OnBtnSearchClick)
	//*)
END_EVENT_TABLE()

KodersDialog::KodersDialog(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(KodersDialog)
	Create(parent,id,_("Koders query"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE,_T(""));
	bszMain = new wxBoxSizer(wxVERTICAL);
	bszIntro = new wxBoxSizer(wxHORIZONTAL);
	lblIntro = new wxStaticText(this,ID_LBL_INTRO,_("Specify search to query the Koders webpage:"),wxDefaultPosition,wxDefaultSize,0,_("ID_LBL_INTRO"));
	bszIntro->Add(lblIntro,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,5);
	bszSearch = new wxBoxSizer(wxHORIZONTAL);
	txtSearch = new wxTextCtrl(this,ID_TXT_SEARCH,_T(""),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_TXT_SEARCH"));
	if ( 0 ) txtSearch->SetMaxLength(0);
	txtSearch->SetToolTip(_("Enter the search phrase to query the Koders webpage for..."));
	btnSearch = new wxButton(this,ID_BTN_SEARCH,_("Search"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_("ID_BTN_SEARCH"));
	if (true) btnSearch->SetDefault();
	btnSearch->SetToolTip(_("Click to query the search phrase on the Koders webpage..."));
	bszSearch->Add(txtSearch,1,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,5);
	bszSearch->Add(btnSearch,0,wxALL|wxALIGN_RIGHT|wxALIGN_TOP,5);
	bszFilter = new wxBoxSizer(wxHORIZONTAL);
	lblFilter = new wxStaticText(this,ID_LBL_FILTER,_("Filter:"),wxDefaultPosition,wxDefaultSize,0,_("ID_LBL_FILTER"));
	choLanguages = new wxChoice(this,ID_CHO_LANGUAGES,wxDefaultPosition,wxDefaultSize,0,NULL,0,wxDefaultValidator,_("ID_CHO_LANGUAGES"));
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
	choLanguages->SetSelection(0);
	choLanguages->SetToolTip(_("Specify a language filter (limits search)..."));
	choLicenses = new wxChoice(this,ID_CHO_LICENSES,wxDefaultPosition,wxDefaultSize,0,NULL,0,wxDefaultValidator,_("ID_CHO_LICENSES"));
	choLicenses->Append(_("All Licenses"));
	choLicenses->Append(_("AFL"));
	choLicenses->Append(_("AL20"));
	choLicenses->Append(_("ASL"));
	choLicenses->Append(_("APSL"));
	choLicenses->Append(_("BSD"));
	choLicenses->Append(_("CPL"));
	choLicenses->Append(_("GPL"));
	choLicenses->Append(_("LGPL"));
	choLicenses->Append(_("IBMPL"));
	choLicenses->Append(_("IOSL"));
	choLicenses->Append(_("MPL10"));
	choLicenses->Append(_("MPL11"));
	choLicenses->Append(_("NPL10"));
	choLicenses->Append(_("NPL11"));
	choLicenses->Append(_("OSL"));
	choLicenses->Append(_("PSFL"));
	choLicenses->Append(_("SPL"));
	choLicenses->Append(_("W3C"));
	choLicenses->Append(_("ZLL"));
	choLicenses->Append(_("ZPL"));
	choLicenses->SetSelection(0);
	choLicenses->SetToolTip(_("Specify a license filter (limits search)..."));
	bszFilter->Add(lblFilter,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,5);
	bszFilter->Add(choLanguages,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,5);
	bszFilter->Add(choLicenses,0,wxALL|wxALIGN_RIGHT|wxALIGN_TOP|wxEXPAND,5);
	bszMain->Add(bszIntro,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,0);
	bszMain->Add(bszSearch,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,0);
	bszMain->Add(bszFilter,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,0);
	this->SetSizer(bszMain);
	bszMain->Fit(this);
	bszMain->SetSizeHints(this);
	Center();
	//*)
}

KodersDialog::~KodersDialog()
{
}

wxString KodersDialog::GetSearch() const
{
  wxString search(_(""));

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

wxString KodersDialog::GetLicense() const
{
  wxString license(_("*"));

  if (choLicenses && (choLicenses->GetStringSelection() != _("All Licenses")))
    license = choLicenses->GetStringSelection();

  return license;
}

void KodersDialog::OnBtnSearchClick(wxCommandEvent& event)
{
  EndModal(wxID_OK);
}
