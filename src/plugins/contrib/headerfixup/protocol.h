/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

//(*Headers(Protocol)
#include <wx/dialog.h>
class wxTextCtrl;
class wxStaticText;
class wxFlexGridSizer;
class wxButton;
class wxStaticBoxSizer;
//*)

class wxCommandEvent;
class wxWindow;
#include <wx/string.h>

class Protocol: public wxDialog
{
public:

  Protocol(wxWindow* parent,wxWindowID id = -1);
  virtual ~Protocol();

  void SetProtocol(const wxString & Protocol)
  { if (m_Protocol) m_Protocol->SetValue(Protocol); }

  //(*Identifiers(Protocol)
  static const long ID_LBL_PROTOCOL;
  static const long ID_TXT_PROTOCOL;
  //*)

protected:

  //(*Handlers(Protocol)
  void OnBtnOKClick(wxCommandEvent& event);
  //*)

  //(*Declarations(Protocol)
  wxTextCtrl* m_Protocol;
  wxStaticText* lblProtocol;
  wxButton* m_OK;
  //*)

private:

  DECLARE_EVENT_TABLE()
};

#endif // PROTOCOL_H
