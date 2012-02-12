/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

//(*Headers(Configuration)
#include <wx/panel.h>
class wxTextCtrl;
class wxListBox;
class wxStaticText;
class wxBoxSizer;
class wxButton;
class wxStaticBoxSizer;
//*)

#include <wx/event.h>
#include <wx/string.h>

#include <configurationpanel.h>
#include "bindings.h"

class wxWindow;

class Configuration: public cbConfigurationPanel
{
public:

  Configuration(wxWindow* parent,wxWindowID id = -1);
  virtual ~Configuration();

  //(*Identifiers(Configuration)
  static const long ID_LST_GROUPS;
  static const long ID_BTN_ADD_GROUP;
  static const long ID_BTN_DELETE_GROUP;
  static const long ID_BTN_RENAME_GROUP;
  static const long ID_BTN_DEFAULTS;
  static const long ID_LBL_IDENTIFIERS;
  static const long ID_LST_IDENTIFIERS;
  static const long ID_BTN_ADD_IDENTIFIER;
  static const long ID_BTN_DELETE_IDENTIFIERS;
  static const long ID_BTN_CHANGE_IDENTIFIER;
  static const long ID_LBL_HEADERS;
  static const long ID_TXT_HEADERS;
  //*)

protected:

  //(*Handlers(Configuration)
  void OnBtnAddGroupClick(wxCommandEvent& event);
  void OnBtnDeleteGroupClick(wxCommandEvent& event);
  void OnRenameGroup(wxCommandEvent& event);
  void OnGroupsSelect(wxCommandEvent& event);
  void OnBtnAddIdentifierClick(wxCommandEvent& event);
  void OnBtnDeleteIdentifierClick(wxCommandEvent& event);
  void OnChangeIdentifier(wxCommandEvent& event);
  void OnIdentifiersSelect(wxCommandEvent& event);
  void OnHeadersText(wxCommandEvent& event);
  void OnBtnDefaultsClick(wxCommandEvent& event);
  //*)

  //(*Declarations(Configuration)
  wxButton* m_DeleteIdentifier;
  wxBoxSizer* sizMain;
  wxListBox* m_Groups;
  wxListBox* m_Identifiers;
  wxBoxSizer* sizHeaders;
  wxBoxSizer* sizAddDeleteChange;
  wxTextCtrl* m_Headers;
  wxButton* m_ChangeIdentifier;
  wxButton* m_DeleteGroup;
  wxButton* m_RenameGroup;
  wxButton* m_AddIdentifier;
  wxButton* m_AddGroup;
  wxButton* m_Defaults;
  wxStaticBoxSizer* sizGroups;
  wxStaticText* lblHeaders;
  wxBoxSizer* sizIdentifiers;
  wxBoxSizer* sizAddDeleteRename;
  wxBoxSizer* sizIdentifiersVert;
  wxStaticBoxSizer* sizBindings;
  wxBoxSizer* sizIdentifiersHor;
  wxStaticText* lblIdentifiers;
  //*)

private:

  void ShowGroups();
  void SelectGroup(int Number);
  void SelectIdentifier(int Number);
  virtual wxString GetTitle() const
  { return _("HeaderFixup configuration"); }
  virtual wxString GetBitmapBaseName() const
  { return _T("generic-plugin"); }
  virtual void OnApply();
  virtual void OnCancel()
  { }

  Bindings m_Bindings;
  bool     m_BlockHeadersText;
  bool     m_Dirty;

  DECLARE_EVENT_TABLE()
};

#endif
