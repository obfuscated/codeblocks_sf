
#include <wx/string.h>
#include "kodersdialog.h"

//(*InternalHeaders(KodersDialog)
#include <wx/bitmap.h>
#include <wx/font.h>
#include <wx/fontenum.h>
#include <wx/fontmap.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/settings.h>
//*)

//(*IdInit(KodersDialog)
const long KodersDialog::ID_LBL_INTRO = wxNewId();
const long KodersDialog::ID_TXT_SEARCH = wxNewId();
const long KodersDialog::ID_BTN_SEARCH = wxNewId();
const long KodersDialog::ID_LBL_FILTER = wxNewId();
const long KodersDialog::ID_CHO_LANGUAGES = wxNewId();
const long KodersDialog::ID_CHO_LICENSES = wxNewId();
//*)

BEGIN_EVENT_TABLE(KodersDialog,wxDialog)
	//(*EventTable(KodersDialog)
	//*)
END_EVENT_TABLE()

KodersDialog::KodersDialog(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(KodersDialog)
	Create(parent,id,_("Koders query"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE,_T("wxDialog"));
	bszMain = new wxBoxSizer(wxVERTICAL);
	bszIntro = new wxBoxSizer(wxHORIZONTAL);
	lblIntro = new wxStaticText(this,ID_LBL_INTRO,_("Specify search to query the Koders webpage:"),wxDefaultPosition,wxDefaultSize,0,_T("ID_LBL_INTRO"));
	bszIntro->Add(lblIntro,0,wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP,5);
	bszMain->Add(bszIntro,0,wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP,0);
	bszSearch = new wxBoxSizer(wxHORIZONTAL);
	txtSearch = new wxTextCtrl(this,ID_TXT_SEARCH,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_TXT_SEARCH"));
	txtSearch->SetToolTip(_("Enter keyword to search for (at koders)"));
	bszSearch->Add(txtSearch,1,wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP,5);
	btnSearch = new wxButton(this,ID_BTN_SEARCH,_("Search"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_BTN_SEARCH"));
	btnSearch->SetDefault();
	btnSearch->SetToolTip(_("Click to search at the koders webpage..."));
	bszSearch->Add(btnSearch,0,wxALL|wxALIGN_RIGHT|wxALIGN_TOP,5);
	bszMain->Add(bszSearch,0,wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP,0);
	bszFilter = new wxBoxSizer(wxHORIZONTAL);
	lblFilter = new wxStaticText(this,ID_LBL_FILTER,_("Filter:"),wxDefaultPosition,wxDefaultSize,0,_T("ID_LBL_FILTER"));
	bszFilter->Add(lblFilter,0,wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP,5);
	choLanguages = new wxChoice(this,ID_CHO_LANGUAGES,wxDefaultPosition,wxDefaultSize,0,NULL,0,wxDefaultValidator,_T("ID_CHO_LANGUAGES"));
	choLanguages->SetSelection( choLanguages->Append(_("All Languages")) );
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
	bszFilter->Add(choLanguages,0,wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP,5);
	choLicenses = new wxChoice(this,ID_CHO_LICENSES,wxDefaultPosition,wxDefaultSize,0,NULL,0,wxDefaultValidator,_T("ID_CHO_LICENSES"));
	choLicenses->SetSelection( choLicenses->Append(_("All Licenses")) );
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
	choLicenses->SetToolTip(_("Limit search to a specific license type"));
	bszFilter->Add(choLicenses,0,wxALL|wxEXPAND|wxALIGN_RIGHT|wxALIGN_TOP,5);
	bszMain->Add(bszFilter,0,wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP,0);
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
