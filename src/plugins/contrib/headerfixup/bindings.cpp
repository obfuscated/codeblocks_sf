/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "bindings.h"

#include <manager.h>
#include <configmanager.h>

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

Bindings::Bindings()
{
  InitialiseBindingsFromConfig();
}// Bindings

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

Bindings::~Bindings()
{
}// ~Bindings

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Bindings::SaveBindings()
{
  ConfigManager* CfgMgr = Manager::Get()->GetConfigManager(_T("HeaderFixup"));
  if (!CfgMgr)
    return;

  CfgMgr->Clear();

  // TODO (Morten#1#): Implement saving to external XML file
//  wxString BindingsFile = CfgMgr->GetConfigFolder() + wxFILE_SEP_PATH + _T("bindings.xml");

  int Cnt = 0;
  for ( GroupsT::iterator i = m_Groups.begin(); i!=m_Groups.end(); ++i )
  {
    wxString Group = i->first;
    MappingsT& Map = i->second;
    for ( MappingsT::iterator j=Map.begin(); j!=Map.end(); ++j )
    {
      wxString Identifier = j->first;
      wxArrayString& Headers = j->second;
      for ( size_t k=0; k<Headers.GetCount(); k++ )
      {
        wxString Key = wxString::Format(_T("binding%05d"),++Cnt);
        CfgMgr->Write(_T("/groups/") + Group + _T("/") + Key + _T("/identifier"),Identifier);
        CfgMgr->Write(_T("/groups/") + Group + _T("/") + Key + _T("/header"),    Headers[k]);
      }
    }
  }
}// SaveBindings

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Bindings::AddBinding(wxString Group,wxString Identifier,wxString Header)
{
  wxArrayString& Headers = (m_Groups[Group])[Identifier];
  if ( Headers.Index(Header) == wxNOT_FOUND )
    Headers.Add(Header);
}// AddBinding

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Bindings::DelBinding(wxString Group,wxString Identifier,wxString Header)
{
  (m_Groups[Group])[Identifier].Remove(Header);
}// DelBinding

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Bindings::GetBindings(wxString Group,wxString Identifier,wxArrayString& DestHeaders)
{
  wxArrayString& Headers = (m_Groups[Group])[Identifier];
  for ( size_t i=0; i<Headers.GetCount(); i++ )
    DestHeaders.Add(Headers[i]);
}// GetBindings

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

wxArrayString Bindings::GetGroups()
{
  wxArrayString Array;
  for ( GroupsT::iterator i = m_Groups.begin(); i!=m_Groups.end(); ++i )
    Array.Add(i->first);
  return Array;
}// GetGroups

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void Bindings::InitialiseBindingsFromConfig()
{
  ConfigManager* CfgMgr = Manager::Get()->GetConfigManager(_T("HeaderFixup"));
  if (!CfgMgr)
    return;

  wxArrayString Groups = CfgMgr->EnumerateSubPaths(_T("/groups"));
  for ( size_t i=0; i<Groups.GetCount(); i++ )
  {
    MappingsT& Map = m_Groups[Groups[i]];

    const wxArrayString TheBindings = CfgMgr->EnumerateSubPaths(_T("/groups/") + Groups[i]);
    for ( size_t j = 0; j < TheBindings.GetCount(); ++j )
    {
      const wxString Identifier = CfgMgr->Read(_T("/groups/") + Groups[i] + _T("/") + TheBindings[j] + _T("/identifier"));
      const wxString Header     = CfgMgr->Read(_T("/groups/") + Groups[i] + _T("/") + TheBindings[j] + _T("/header"));
      if ( Identifier.IsEmpty() || Header.IsEmpty() ) continue;
      wxArrayString& Headers = Map[Identifier];
      if ( Headers.Index(Header) == wxNOT_FOUND )
        Headers.Add(Header);
    }
  }

  if ( m_Groups.empty() )
    SetDefaults();
}// InitialiseFromConfig
