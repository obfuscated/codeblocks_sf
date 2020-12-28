/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "tidycmtsettings.h"

//(*InternalHeaders(TidyCmtSettings)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(TidyCmtSettings)
const long TidyCmtSettings::ID_CHK_ENABLE = wxNewId();
const long TidyCmtSettings::ID_SPN_LENGTH = wxNewId();
const long TidyCmtSettings::ID_TXT_SINGLE_LINE_CMT = wxNewId();
const long TidyCmtSettings::ID_TXT_MULTI_LINE_CMT = wxNewId();
//*)

BEGIN_EVENT_TABLE(TidyCmtSettings,wxPanel)
	//(*EventTable(TidyCmtSettings)
	//*)
END_EVENT_TABLE()

TidyCmtSettings::TidyCmtSettings(wxWindow* parent, const TidyCmtConfig& tcc)
{
	//(*Initialize(TidyCmtSettings)
	wxGridSizer* grdMain;
	wxStaticText* lblEnable;
	wxStaticText* lblLength;
	wxStaticText* lblMultiLineCmt;
	wxStaticText* lblSingleLineCmt;

	Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("wxID_ANY"));
	grdMain = new wxGridSizer(4, 2, 5, 5);
	lblEnable = new wxStaticText(this, wxID_ANY, _("Enable tidying of comments:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	grdMain->Add(lblEnable, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	chkEnable = new wxCheckBox(this, ID_CHK_ENABLE, _("OK (enable)."), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHK_ENABLE"));
	chkEnable->SetValue(false);
	grdMain->Add(chkEnable, 0, wxEXPAND, 5);
	lblLength = new wxStaticText(this, wxID_ANY, _("Select length of comment in file:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	grdMain->Add(lblLength, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	spnLength = new wxSpinCtrl(this, ID_SPN_LENGTH, _T("80"), wxDefaultPosition, wxDefaultSize, 0, 16, 255, 80, _T("ID_SPN_LENGTH"));
	spnLength->SetValue(_T("80"));
	grdMain->Add(spnLength, 0, wxEXPAND, 5);
	lblSingleLineCmt = new wxStaticText(this, wxID_ANY, _("Single-line comment trigger:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	grdMain->Add(lblSingleLineCmt, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	txtSingleLineCmt = new wxTextCtrl(this, ID_TXT_SINGLE_LINE_CMT, _("//--"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TXT_SINGLE_LINE_CMT"));
	grdMain->Add(txtSingleLineCmt, 0, wxEXPAND, 5);
	lblMultiLineCmt = new wxStaticText(this, wxID_ANY, _("Multi-line comment trigger:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	grdMain->Add(lblMultiLineCmt, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	txtMultiLineCmt = new wxTextCtrl(this, ID_TXT_MULTI_LINE_CMT, _("/*--"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TXT_MULTI_LINE_CMT"));
	grdMain->Add(txtMultiLineCmt, 0, wxEXPAND, 5);
	SetSizer(grdMain);
	grdMain->Fit(this);
	grdMain->SetSizeHints(this);

	Connect(ID_CHK_ENABLE,wxEVT_COMMAND_CHECKBOX_CLICKED,wxCommandEventHandler(TidyCmtSettings::OnEnableClick));
	//*)

  chkEnable->SetValue(tcc.enabled);
  spnLength->SetValue(tcc.length);
  txtSingleLineCmt->SetValue(tcc.single_line_cmt);
  txtMultiLineCmt->SetValue(tcc.multi_line_cmt);

  DoEnable(tcc.enabled);
}

TidyCmtSettings::~TidyCmtSettings()
{
  //(*Destroy(TidyCmtSettings)
  //*)
}

TidyCmtConfig TidyCmtSettings::GetTidyCmtConfig()
{
  TidyCmtConfig tcc;

  tcc.enabled         = chkEnable->IsChecked();
  tcc.length          = spnLength->GetValue();
  tcc.single_line_cmt = txtSingleLineCmt->GetValue();
  tcc.multi_line_cmt  = txtMultiLineCmt->GetValue();

  return tcc;
}

void TidyCmtSettings::OnEnableClick(wxCommandEvent& event)
{
  DoEnable(event.IsChecked());
}

void TidyCmtSettings::DoEnable(bool en)
{
  spnLength->Enable(en);
  txtSingleLineCmt->Enable(en);
  txtMultiLineCmt->Enable(en);
}
