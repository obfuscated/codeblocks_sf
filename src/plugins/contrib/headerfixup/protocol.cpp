/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

//(*InternalHeaders(Protocol)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/intl.h>
#include <wx/button.h>
#include <wx/string.h>
//*)

#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/utils.h>
#include <wx/window.h>

#include "protocol.h"

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

//(*IdInit(Protocol)
const long Protocol::ID_LBL_PROTOCOL = wxNewId();
const long Protocol::ID_TXT_PROTOCOL = wxNewId();
//*)

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

BEGIN_EVENT_TABLE(Protocol,wxDialog)
  //(*EventTable(Protocol)
  //*)
END_EVENT_TABLE()

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

Protocol::Protocol(wxWindow* parent,wxWindowID id)
{
  //(*Initialize(Protocol)
  wxStaticBoxSizer* sizProtocol;
  wxFlexGridSizer* flsMain;
  wxFlexGridSizer* flsProtocol;

  Create(parent, wxID_ANY, _("Header Fixup - Protocol"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("wxID_ANY"));
  flsMain = new wxFlexGridSizer(2, 1, 0, 0);
  flsMain->AddGrowableCol(0);
  flsMain->AddGrowableRow(0);
  sizProtocol = new wxStaticBoxSizer(wxVERTICAL, this, _("Protocol"));
  flsProtocol = new wxFlexGridSizer(2, 1, 0, 0);
  flsProtocol->AddGrowableCol(0);
  flsProtocol->AddGrowableRow(1);
  lblProtocol = new wxStaticText(this, ID_LBL_PROTOCOL, _("Protocol for last operation:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_LBL_PROTOCOL"));
  flsProtocol->Add(lblProtocol, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
  m_Protocol = new wxTextCtrl(this, ID_TXT_PROTOCOL, wxEmptyString, wxDefaultPosition, wxSize(480,240), wxTE_MULTILINE|wxTE_READONLY, wxDefaultValidator, _T("ID_TXT_PROTOCOL"));
  m_Protocol->SetToolTip(_("This is the full log of the parser operations."));
  flsProtocol->Add(m_Protocol, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  sizProtocol->Add(flsProtocol, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  flsMain->Add(sizProtocol, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  m_OK = new wxButton(this, wxID_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("wxID_OK"));
  m_OK->SetDefault();
  m_OK->SetToolTip(_("Click to exit the protocol and return to C::B."));
  flsMain->Add(m_OK, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  SetSizer(flsMain);
  flsMain->Fit(this);
  flsMain->SetSizeHints(this);
  Center();

  Connect(wxID_OK,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&Protocol::OnBtnOKClick);
  //*)
}// Protocol

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

Protocol::~Protocol()
{
  //(*Destroy(Protocol)
  //*)
}// ~Protocol

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Protocol::OnBtnOKClick(wxCommandEvent& event)
{
  EndModal(wxID_OK);
}// OnBtnOkClick
