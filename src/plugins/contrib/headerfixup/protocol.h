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
#include <wx/sizer.h>
#include <wx/button.h>
#include "scrollingdialog.h"
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)


class wxCommandEvent;
class wxWindow;
class wxString;

class Protocol: public wxScrollingDialog
{
public:

  Protocol(wxWindow* parent,wxWindowID id = -1);

  void SetProtocol(const wxArrayString& Protocol);

  //(*Identifiers(Protocol)
  static const long ID_LBL_PROTOCOL;
  static const long ID_TXT_PROTOCOL;
  //*)

protected:

  //(*Handlers(Protocol)
  void OnBtnOKClick(wxCommandEvent& event);
  //*)

  //(*Declarations(Protocol)
  wxBoxSizer* sizMain;
  wxButton* m_OK;
  wxStaticText* lblProtocol;
  wxStaticBoxSizer* sizProtocol;
  wxTextCtrl* m_Protocol;
  //*)

private:

  DECLARE_EVENT_TABLE()
};

#endif // PROTOCOL_H
