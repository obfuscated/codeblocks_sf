/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

//(*InternalHeaders(Configuration)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/listbox.h>
#include <wx/intl.h>
#include <wx/button.h>
#include <wx/string.h>
//*)

#include <wx/arrstr.h>
#include <wx/gdicmn.h>
#include <wx/textdlg.h>
#include <wx/tokenzr.h>
#include <wx/utils.h>
#include <wx/window.h>

#include <globals.h>

#include "configuration.h"

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

//(*IdInit(Configuration)
const long Configuration::ID_LST_GROUPS = wxNewId();
const long Configuration::ID_BTN_ADD_GROUP = wxNewId();
const long Configuration::ID_BTN_DELETE_GROUP = wxNewId();
const long Configuration::ID_BTN_RENAME_GROUP = wxNewId();
const long Configuration::ID_BTN_DEFAULTS = wxNewId();
const long Configuration::ID_LST_IDENTIFIERS = wxNewId();
const long Configuration::ID_BTN_ADD_IDENTIFIER = wxNewId();
const long Configuration::ID_BTN_DELETE_IDENTIFIERS = wxNewId();
const long Configuration::ID_BTN_CHANGE_IDENTIFIER = wxNewId();
const long Configuration::ID_TXT_HEADERS = wxNewId();
//*)

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

BEGIN_EVENT_TABLE(Configuration,wxPanel)
  //(*EventTable(Configuration)
  //*)
END_EVENT_TABLE()

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

Configuration::Configuration(wxWindow* parent)
{
  //(*Initialize(Configuration)
  wxBoxSizer* sizMain;
  wxBoxSizer* sizHeaders;
  wxBoxSizer* sizAddDeleteChange;
  wxStaticBoxSizer* sizGroups;
  wxStaticText* lblHeaders;
  wxBoxSizer* sizIdentifiers;
  wxBoxSizer* sizAddDeleteRename;
  wxStaticBoxSizer* sizBindings;
  wxBoxSizer* sizIdentifiersMain;
  wxStaticText* lblIdentifiers;

  Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("wxID_ANY"));
  sizMain = new wxBoxSizer(wxHORIZONTAL);
  sizGroups = new wxStaticBoxSizer(wxVERTICAL, this, _("Groups"));
  m_Groups = new wxListBox(this, ID_LST_GROUPS, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_LST_GROUPS"));
  m_Groups->SetToolTip(_("This is a list of all groups of bindings available to the plugin."));
  sizGroups->Add(m_Groups, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  sizAddDeleteRename = new wxBoxSizer(wxHORIZONTAL);
  m_AddGroup = new wxButton(this, ID_BTN_ADD_GROUP, _("Add"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BTN_ADD_GROUP"));
  m_AddGroup->SetToolTip(_("Add a new group..."));
  sizAddDeleteRename->Add(m_AddGroup, 1, wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
  m_DeleteGroup = new wxButton(this, ID_BTN_DELETE_GROUP, _("Delete"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BTN_DELETE_GROUP"));
  m_DeleteGroup->SetToolTip(_("Delete the selected group..."));
  sizAddDeleteRename->Add(m_DeleteGroup, 1, wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
  m_RenameGroup = new wxButton(this, ID_BTN_RENAME_GROUP, _("Rename"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BTN_RENAME_GROUP"));
  m_RenameGroup->SetToolTip(_("Rename the selected group..."));
  sizAddDeleteRename->Add(m_RenameGroup, 1, wxLEFT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
  sizGroups->Add(sizAddDeleteRename, 0, wxTOP|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  m_Defaults = new wxButton(this, ID_BTN_DEFAULTS, _("Defaults"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BTN_DEFAULTS"));
  sizGroups->Add(m_Defaults, 0, wxTOP|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  sizMain->Add(sizGroups, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  sizBindings = new wxStaticBoxSizer(wxVERTICAL, this, _("Bindings"));
  sizIdentifiersMain = new wxBoxSizer(wxVERTICAL);
  lblIdentifiers = new wxStaticText(this, wxID_ANY, _("Identifiers:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
  sizIdentifiersMain->Add(lblIdentifiers, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  sizIdentifiers = new wxBoxSizer(wxHORIZONTAL);
  m_Identifiers = new wxListBox(this, ID_LST_IDENTIFIERS, wxDefaultPosition, wxSize(188,115), 0, 0, 0, wxDefaultValidator, _T("ID_LST_IDENTIFIERS"));
  m_Identifiers->SetToolTip(_("This is a list of all identifiers (tokens) available within the selected group."));
  sizIdentifiers->Add(m_Identifiers, 1, wxTOP|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  sizAddDeleteChange = new wxBoxSizer(wxVERTICAL);
  m_AddIdentifier = new wxButton(this, ID_BTN_ADD_IDENTIFIER, _("Add"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BTN_ADD_IDENTIFIER"));
  m_AddIdentifier->SetToolTip(_("Add a new identifier..."));
  sizAddDeleteChange->Add(m_AddIdentifier, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  m_DeleteIdentifier = new wxButton(this, ID_BTN_DELETE_IDENTIFIERS, _("Delete"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BTN_DELETE_IDENTIFIERS"));
  m_DeleteIdentifier->SetToolTip(_("Delete the selected identifier..."));
  sizAddDeleteChange->Add(m_DeleteIdentifier, 0, wxTOP|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  m_ChangeIdentifier = new wxButton(this, ID_BTN_CHANGE_IDENTIFIER, _("Change"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BTN_CHANGE_IDENTIFIER"));
  m_ChangeIdentifier->SetToolTip(_("Change (rename) the selected identifier..."));
  sizAddDeleteChange->Add(m_ChangeIdentifier, 0, wxTOP|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  sizIdentifiers->Add(sizAddDeleteChange, 0, wxTOP|wxLEFT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  sizIdentifiersMain->Add(sizIdentifiers, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  sizBindings->Add(sizIdentifiersMain, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  sizHeaders = new wxBoxSizer(wxVERTICAL);
  lblHeaders = new wxStaticText(this, wxID_ANY, _("Headers:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
  sizHeaders->Add(lblHeaders, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  m_Headers = new wxTextCtrl(this, ID_TXT_HEADERS, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER|wxTE_MULTILINE, wxDefaultValidator, _T("ID_TXT_HEADERS"));
  m_Headers->SetToolTip(_("This is a list of all header files required for the selected identifier (token) within the selected group."));
  sizHeaders->Add(m_Headers, 1, wxTOP|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  sizBindings->Add(sizHeaders, 1, wxTOP|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  sizMain->Add(sizBindings, 1, wxTOP|wxBOTTOM|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
  SetSizer(sizMain);
  sizMain->Fit(this);
  sizMain->SetSizeHints(this);

  Connect(ID_LST_GROUPS,wxEVT_COMMAND_LISTBOX_SELECTED,(wxObjectEventFunction)&Configuration::OnGroupsSelect);
  Connect(ID_LST_GROUPS,wxEVT_COMMAND_LISTBOX_DOUBLECLICKED,(wxObjectEventFunction)&Configuration::OnRenameGroup);
  Connect(ID_BTN_ADD_GROUP,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&Configuration::OnBtnAddGroupClick);
  Connect(ID_BTN_DELETE_GROUP,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&Configuration::OnBtnDeleteGroupClick);
  Connect(ID_BTN_RENAME_GROUP,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&Configuration::OnRenameGroup);
  Connect(ID_BTN_DEFAULTS,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&Configuration::OnBtnDefaultsClick);
  Connect(ID_LST_IDENTIFIERS,wxEVT_COMMAND_LISTBOX_SELECTED,(wxObjectEventFunction)&Configuration::OnIdentifiersSelect);
  Connect(ID_LST_IDENTIFIERS,wxEVT_COMMAND_LISTBOX_DOUBLECLICKED,(wxObjectEventFunction)&Configuration::OnChangeIdentifier);
  Connect(ID_BTN_ADD_IDENTIFIER,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&Configuration::OnBtnAddIdentifierClick);
  Connect(ID_BTN_DELETE_IDENTIFIERS,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&Configuration::OnBtnDeleteIdentifierClick);
  Connect(ID_BTN_CHANGE_IDENTIFIER,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&Configuration::OnChangeIdentifier);
  Connect(ID_TXT_HEADERS,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&Configuration::OnHeadersText);
  //*)

  m_BlockHeadersText = false;
  m_Dirty            = false;

  ShowGroups();
}// Configuration

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

Configuration::~Configuration()
{
  //(*Destroy(Configuration)
  //*)
}// ~Configuration

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

bool Configuration::IdentifierOK(const wxString& Identifier)
{
  if ( wxString(g_alpha_chars).Find(Identifier.GetChar(0)) == wxNOT_FOUND )
  {
    cbMessageBox(_("Please enter valid C++ identifier."),_T("Header Fixup"));
    return false;
  }

  for ( size_t i=1; i<Identifier.Length(); i++ )
  {
    if ( wxString(g_alpha_numeric_chars).Find(Identifier.GetChar(i)) == wxNOT_FOUND )
    {
      cbMessageBox(_("Please enter valid C++ identifier."),_T("Header Fixup"));
      return false;
    }
  }

  return true;
}// IdentifierOK

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Configuration::ShowGroups()
{
  m_Groups->Clear();
  for ( Bindings::GroupsT::iterator i = m_Bindings.m_Groups.begin(); i != m_Bindings.m_Groups.end(); ++i )
    m_Groups->Append(i->first, (void*) &(i->second) );

  SelectGroup(0);
}// ShowGroups

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Configuration::SelectGroup(int Number)
{
  if ( m_Groups->GetSelection() != Number )
    m_Groups->SetSelection(Number);

  if ( Number<0 || Number >= static_cast<int>(m_Groups->GetCount()))
  {
    m_RenameGroup->Disable();
    m_DeleteGroup->Disable();
    m_Identifiers->Clear();
    m_Identifiers->Disable();
    SelectIdentifier(-1);
    m_AddIdentifier->Disable();
  }
  else
  {
    m_AddIdentifier->Enable();
    m_RenameGroup->Enable();
    m_DeleteGroup->Enable();
    m_Identifiers->Clear();
    m_Identifiers->Enable();

    Bindings::MappingsT* Map = (Bindings::MappingsT*)(m_Groups->GetClientData(Number));
    for ( Bindings::MappingsT::iterator i=Map->begin(); i!=Map->end(); ++i )
      m_Identifiers->Append(i->first, (void*) &(i->second) );
    SelectIdentifier(0);
  }
}// SelectGroup

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Configuration::SelectIdentifier(int Number)
{
  if ( m_Identifiers->GetSelection() != Number )
    m_Identifiers->SetSelection(Number);

  m_BlockHeadersText = true;
  if ( Number<0 || Number>= static_cast<int>(m_Identifiers->GetCount()))
  {
    m_ChangeIdentifier->Disable();
    m_DeleteIdentifier->Disable();
    m_Headers->Disable();
    m_Headers->Clear();
  }
  else
  {
    m_ChangeIdentifier->Enable();
    m_DeleteIdentifier->Enable();
    m_Headers->Enable();
    wxArrayString& Headers = *((wxArrayString*)(m_Identifiers->GetClientData(Number)));
    wxString Content;
    for ( size_t i=0; i<Headers.GetCount(); i++ )
      Content << Headers[i] << _T("\n");
    m_Headers->SetValue(Content);
  }
  m_BlockHeadersText = false;
}// SelectIdentifier

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Configuration::OnApply()
{
  if (m_Dirty)
    m_Bindings.SaveBindings();
}// OnApply

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Configuration::OnBtnAddGroupClick(wxCommandEvent& /*event*/)
{
  wxString GroupName = ::wxGetTextFromUser(_("Enter name for new group"));
  if ( GroupName.IsEmpty() )
    return;

  if ( m_Groups->FindString(GroupName) != wxNOT_FOUND )
  {
    cbMessageBox(_("Group with this name already exists."),_T("Header Fixup"));
    return;
  }

  for ( size_t i=0; i<GroupName.Length(); i++ )
  {
    if ( wxString(g_alpha_numeric_chars).Find(GroupName.GetChar(i)) == wxNOT_FOUND )
    {
      cbMessageBox(_("Invalid group name, please use only alphanumeric characters or '_'."),_T("Header Fixup"));
      return;
    }
  }

  Bindings::MappingsT& Map = m_Bindings.m_Groups[GroupName];
  SelectGroup(m_Groups->Append(GroupName,(void*)&Map));

  m_Dirty = true;
}// OnBtnAddGroupClick

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Configuration::OnBtnDeleteGroupClick(wxCommandEvent& /*event*/)
{
  if ( cbMessageBox(_("Are you sure?"),_("Deleting group"),wxYES|wxNO) != wxID_YES )
    return;

  wxString GroupName = m_Groups->GetStringSelection();
  if ( GroupName.IsEmpty() )
    return;

  m_Groups->Delete(m_Groups->GetSelection());
  m_Bindings.m_Groups.erase(GroupName);
  SelectGroup(m_Groups->GetSelection());

  m_Dirty = true;
}// OnBtnDeleteGroupClick

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Configuration::OnRenameGroup(wxCommandEvent& /*event*/)
{
  wxString GroupName = m_Groups->GetStringSelection();
  wxString OldName = GroupName;
  if ( GroupName.IsEmpty() )
    return;

  GroupName = ::wxGetTextFromUser(_("Enter new group name"),_("Change group name"),GroupName);
  if ( GroupName.IsEmpty() )
    return;

  int Index = m_Groups->FindString(GroupName);
  if ( Index != wxNOT_FOUND && Index != m_Groups->GetSelection() )
  {
    cbMessageBox(_("Group with this name already exists."),_T("Header Fixup"));
    return;
  }

  for ( size_t i=0; i<GroupName.Length(); i++ )
  {
    if ( wxString(g_alpha_numeric_chars).Find(GroupName.GetChar(i)) == wxNOT_FOUND )
    {
      cbMessageBox(_("Invalid group name, please use only alphanumeric characters or '_'."),_T("Header Fixup"));
      return;
    }
  }

  m_Groups->SetString(m_Groups->GetSelection(),GroupName);
  m_Bindings.m_Groups[GroupName] = m_Bindings.m_Groups[OldName];
  m_Bindings.m_Groups.erase(OldName);
  m_Groups->SetClientData(m_Groups->GetSelection(), (void*) &(m_Bindings.m_Groups[GroupName]) );
  SelectGroup(m_Groups->GetSelection());

  m_Dirty = true;
}// OnRenameGroup

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Configuration::OnBtnDefaultsClick(wxCommandEvent& /*event*/)
{
  if ( cbMessageBox(_("Are you really sure?"),_("Setting defaults"),wxYES|wxNO) != wxID_YES )
    return;

  // Clean up the bindings and start from scratch...
  m_Bindings.m_Groups.clear();
  m_Bindings.SetDefaults();

  ShowGroups();

  m_Dirty = false;
}// OnBtnDefaultsClick

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Configuration::OnGroupsSelect(wxCommandEvent& /*event*/)
{
  SelectGroup(m_Groups->GetSelection());
}// OnGroupsSelect

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Configuration::OnBtnAddIdentifierClick(wxCommandEvent& /*event*/)
{
  wxString Identifier = ::wxGetTextFromUser(_("Enter new identifier"));
  if ( Identifier.IsEmpty() )
    return;

  if ( m_Identifiers->FindString(Identifier) != wxNOT_FOUND )
  {
    cbMessageBox(_("Such identifier already exists."),_T("Header Fixup"));
    return;
  }

  if ( !IdentifierOK(Identifier) )
    return;

  Bindings::MappingsT& Map = * ((Bindings::MappingsT*)m_Groups->GetClientData(m_Groups->GetSelection()));
  wxArrayString& Headers = Map[Identifier];
  SelectIdentifier(m_Identifiers->Append(Identifier,(void*)&Headers));

  m_Dirty = true;
}// OnBtnAddIdentifierClick

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Configuration::OnBtnDeleteIdentifierClick(wxCommandEvent& /*event*/)
{
  if ( cbMessageBox(_("Are you sure?"),_("Deleting identifier"),wxYES|wxNO) != wxID_YES )
    return;

  wxString Identifier = m_Identifiers->GetStringSelection();
  if ( Identifier.IsEmpty() )
    return;

  m_Identifiers->Delete(m_Identifiers->GetSelection());

  Bindings::MappingsT& Map = * ((Bindings::MappingsT*)m_Groups->GetClientData(m_Groups->GetSelection()));
  Map.erase(Identifier);
  SelectIdentifier(m_Identifiers->GetSelection());

  m_Dirty = true;
}// OnBtnDeleteIdentifierClick

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Configuration::OnChangeIdentifier(wxCommandEvent& /*event*/)
{
  wxString Identifier = m_Identifiers->GetStringSelection();
  wxString OldIdent = Identifier;
  if ( Identifier.IsEmpty() )
    return;

  Identifier = ::wxGetTextFromUser(_("Enter new identifier"),_("Change identifier"),Identifier);
  if ( Identifier.IsEmpty() )
    return;

  int Index = m_Identifiers->FindString(Identifier);
  if ( Index != wxNOT_FOUND && Index != m_Identifiers->GetSelection() )
  {
    cbMessageBox(_("Such identifier already exists."),_T("Header Fixup"));
    return;
  }

  if ( !IdentifierOK(Identifier) )
    return;

  m_Identifiers->SetString(m_Identifiers->GetSelection(),Identifier);
  Bindings::MappingsT& Map = * ((Bindings::MappingsT*)m_Groups->GetClientData(m_Groups->GetSelection()));

  Map[Identifier] = Map[OldIdent];
  Map.erase(OldIdent);
  m_Identifiers->SetClientData(m_Identifiers->GetSelection(), (void*) &(Map[Identifier]) );
  SelectIdentifier(m_Identifiers->GetSelection());

  m_Dirty = true;
}// OnChangeIdentifier

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Configuration::OnIdentifiersSelect(wxCommandEvent& /*event*/)
{
  SelectIdentifier(m_Identifiers->GetSelection());
}// OnIdentifiersSelect

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Configuration::OnHeadersText(wxCommandEvent& /*event*/)
{
  if ( m_BlockHeadersText )
    return;

  // Updating headers arrays after each text change
  wxStringTokenizer Tokenizer(m_Headers->GetValue(),_T("\n"));
  wxArrayString* Headers = (wxArrayString*)m_Identifiers->GetClientData(m_Identifiers->GetSelection());
  if ( !Headers )
    return;

  Headers->Clear();
  while ( Tokenizer.HasMoreTokens() )
    Headers->Add(Tokenizer.GetNextToken());

  m_Dirty = true;
}// OnHeadersText
