
#include "sdk.h"
#ifndef CB_PRECOMP
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#endif

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
	cboLanguages = new wxChoice(this,ID_CBO_LANGUAGES,wxDefaultPosition,wxDefaultSize,0,NULL,wxCB_READONLY,wxDefaultValidator,_("ID_CBO_LANGUAGES"));
	cboLanguages->Append(_("All Languages"));
	cboLanguages->Append(_("Ada"));
	cboLanguages->Append(_("ASP"));
	cboLanguages->Append(_("Assembler"));
	cboLanguages->Append(_("C"));
	cboLanguages->Append(_("C#"));
	cboLanguages->Append(_("Cpp"));
	cboLanguages->Append(_("ColdFusion"));
	cboLanguages->Append(_("Delphi"));
	cboLanguages->Append(_("Eiffel"));
	cboLanguages->Append(_("Erlang"));
	cboLanguages->Append(_("Fortran"));
	cboLanguages->Append(_("Java"));
	cboLanguages->Append(_("JavaScript"));
	cboLanguages->Append(_("JSP"));
	cboLanguages->Append(_("Lisp"));
	cboLanguages->Append(_("Lua"));
	cboLanguages->Append(_("Mathematica"));
	cboLanguages->Append(_("Matlab"));
	cboLanguages->Append(_("ObjectiveC"));
	cboLanguages->Append(_("Perl"));
	cboLanguages->Append(_("PHP"));
	cboLanguages->Append(_("Prolog"));
	cboLanguages->Append(_("Python"));
	cboLanguages->Append(_("Ruby"));
	cboLanguages->Append(_("Scheme"));
	cboLanguages->Append(_("Smalltalk"));
	cboLanguages->Append(_("SQL"));
	cboLanguages->Append(_("Tcl"));
	cboLanguages->Append(_("VB"));
	cboLanguages->Append(_("VB.NET"));
	cboLanguages->SetSelection(0);
	cboLanguages->SetToolTip(_("Specify a language filter (limits search)..."));
	cboLicenses = new wxChoice(this,ID_CBO_LICENSES,wxDefaultPosition,wxDefaultSize,0,NULL,wxCB_READONLY,wxDefaultValidator,_("ID_CBO_LICENSES"));
	cboLicenses->Append(_("All Licenses"));
	cboLicenses->Append(_("AFL"));
	cboLicenses->Append(_("AL20"));
	cboLicenses->Append(_("ASL"));
	cboLicenses->Append(_("APSL"));
	cboLicenses->Append(_("BSD"));
	cboLicenses->Append(_("CPL"));
	cboLicenses->Append(_("GPL"));
	cboLicenses->Append(_("LGPL"));
	cboLicenses->Append(_("IBMPL"));
	cboLicenses->Append(_("IOSL"));
	cboLicenses->Append(_("MPL10"));
	cboLicenses->Append(_("MPL11"));
	cboLicenses->Append(_("NPL10"));
	cboLicenses->Append(_("NPL11"));
	cboLicenses->Append(_("OSL"));
	cboLicenses->Append(_("PSFL"));
	cboLicenses->Append(_("SPL"));
	cboLicenses->Append(_("W3C"));
	cboLicenses->Append(_("ZLL"));
	cboLicenses->Append(_("ZPL"));
	cboLicenses->SetSelection(0);
	cboLicenses->SetToolTip(_("Specify a license filter (limits search)..."));
	bszFilter->Add(lblFilter,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,5);
	bszFilter->Add(cboLanguages,0,wxALL|wxALIGN_LEFT|wxALIGN_TOP|wxEXPAND,5);
	bszFilter->Add(cboLicenses,0,wxALL|wxALIGN_RIGHT|wxALIGN_TOP|wxEXPAND,5);
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

  if (cboLanguages && (cboLanguages->GetStringSelection() != _("All Languages")))
    language = cboLanguages->GetStringSelection();

  return language;
}

wxString KodersDialog::GetLicense() const
{
  wxString license(_("*"));

  if (cboLicenses && (cboLicenses->GetStringSelection() != _("All Licenses")))
    license = cboLicenses->GetStringSelection();

  return license;
}

void KodersDialog::OnBtnSearchClick(wxCommandEvent& /*event*/)
{
  EndModal(wxID_OK);
}
