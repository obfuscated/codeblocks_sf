/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is distributed under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#if CB_PRECOMP
  #include "sdk.h"
#else
  #include <wx/button.h>
  #include <wx/filefn.h>
  #include <wx/filename.h>
  #include <wx/stattext.h>
  #include <wx/xrc/xmlres.h>
  #include "cbeditor.h"
  #include "cbproject.h"
  #include "configmanager.h"
  #include "editormanager.h"
  #include "globals.h"
  #include "licenses.h"
  #include "pluginmanager.h"
  #include "projectmanager.h"
  #include "manager.h"
#endif
#include <wx/listbox.h>
#include <wx/utils.h>
#include "editpairdlg.h"
#include "projectloader.h"
#include "envvars.h"

// Implement the plugin's hooks
CB_IMPLEMENT_PLUGIN(EnvVars, "EnvVars");

BEGIN_EVENT_TABLE(EnvVars, cbPlugin)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(EnvVarsConfigDlg, wxPanel)
  EVT_UPDATE_UI     (XRCID("btnAddEnvVar"),    EnvVarsConfigDlg::OnUpdateUI)
  EVT_UPDATE_UI     (XRCID("btnEditEnvVar"),   EnvVarsConfigDlg::OnUpdateUI)
  EVT_UPDATE_UI     (XRCID("btnDeleteEnvVar"), EnvVarsConfigDlg::OnUpdateUI)
  EVT_UPDATE_UI     (XRCID("btnClearEnvVars"), EnvVarsConfigDlg::OnUpdateUI)

  EVT_LISTBOX_DCLICK(XRCID("lstEnvVars"),      EnvVarsConfigDlg::OnEditEnvVarClick)

  EVT_BUTTON        (XRCID("btnAddEnvVar"),    EnvVarsConfigDlg::OnAddEnvVarClick)
  EVT_BUTTON        (XRCID("btnEditEnvVar"),   EnvVarsConfigDlg::OnEditEnvVarClick)
  EVT_BUTTON        (XRCID("btnDeleteEnvVar"), EnvVarsConfigDlg::OnDeleteEnvVarClick)
  EVT_BUTTON        (XRCID("btnClearEnvVars"), EnvVarsConfigDlg::OnClearEnvVarsClick)
  EVT_BUTTON        (XRCID("btnSetEnvVars"),   EnvVarsConfigDlg::OnSetEnvVarsClick)
END_EVENT_TABLE()

EnvVars::EnvVars()
{
  //ctor
  m_PluginInfo.name = _T("EnvVars");
  m_PluginInfo.title = _("Environment variables");
  m_PluginInfo.version = _T("0.9");
  m_PluginInfo.description = _("Sets up environment variables within the focus of Code::Blocks.");
  m_PluginInfo.author = _T("Martin Halle");
  m_PluginInfo.authorEmail = _T("codeblocks@martin-halle.de");
  m_PluginInfo.authorWebsite = _T("");
  m_PluginInfo.thanksTo = _("Yiannis Mandravellos, Thomas Denk and the whole Code::Blocks team.");
  m_PluginInfo.license = _T("GPL");
}

void EnvVars::OnAttach()
{
  Manager::Get()->Loadxrc(_T("/envvars.zip#zip:envvars.xrc"));
}

void EnvVars::OnRelease(bool appShutDown)
{
}

cbConfigurationPanel* EnvVars::GetConfigurationPanel(wxWindow* parent)
{
  EnvVarsConfigDlg* dlg = new EnvVarsConfigDlg(parent, this);
  // deleted by the caller

  return dlg;
}


int EnvVars::Configure()
{
  return 0;
}

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

EnvVarsConfigDlg::EnvVarsConfigDlg(wxWindow* parent, EnvVars* plug) :
  plugin(plug)
{
  wxXmlResource::Get()->LoadPanel(this, parent, _T("dlgEnvVars"));
  LoadSettings();
}

void EnvVarsConfigDlg::LoadSettings()
{
//	if (Manager::Get() && Manager::Get()->GetMessageManager());
//    Manager::Get()->GetMessageManager()->DebugLog(_T("LoadSettings"));

  // load configuration
  ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("envvars"));
  wxArrayString list = cfg->EnumerateKeys(_T("/"));
  for (unsigned int i = 0; i < list.GetCount(); ++i)
  {
    wxArrayString array = GetArrayFromString(cfg->Read(list[i]));
    if (array.GetCount() == 2)
    {
      wxString key   = array[0];
      wxString value = array[1];

      key.Trim(true).Trim(false);
      value.Trim(true).Trim(false);
      wxSetEnv(key, value);
      XRCCTRL(*this, "lstEnvVars", wxListBox)->Append(key + _T(" = ") + value);
    }
	}
}

void EnvVarsConfigDlg::SaveSettings()
{
//	if (Manager::Get() && Manager::Get()->GetMessageManager());
//    Manager::Get()->GetMessageManager()->DebugLog(_T("SaveSettings"));

  ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("envvars"));

  wxArrayString list = cfg->EnumerateKeys(_T("/"));
  for (unsigned int i=0; i<list.GetCount(); ++i)
  {
    cfg->UnSet(list[i]);
  }

  wxListBox* lstEnvVars = XRCCTRL(*this, "lstEnvVars", wxListBox);
  if (!lstEnvVars)
    return;

  for (int i=0; i<lstEnvVars->GetCount(); i++)
  {
    wxString key   = lstEnvVars->GetString(i).BeforeFirst(_T('=')).Trim(true);
    wxString value = lstEnvVars->GetString(i).AfterFirst(_T('=')).Trim(true).Trim(false);

    wxString txt;
    txt << key << _T(";") << value;

    wxString cfg_key;
    cfg_key.Printf(_T("EnvVar%d"), i);
		cfg->Write(cfg_key, txt);
	}

  plugin->OnAttach();
}

void EnvVarsConfigDlg::OnAddEnvVarClick(wxCommandEvent& WXUNUSED(event))
{
//	Manager::Get()->GetMessageManager()->DebugLog(_T("OnAddEnvVarClick"));

  wxString key;
  wxString value;
  EditPairDlg dlg(this, key, value, _("Add new variable"),
    EditPairDlg::bmBrowseForDirectory);
  PlaceWindow(&dlg);
  if (dlg.ShowModal() == wxID_OK)
  {
    key.Trim(true).Trim(false);
    value.Trim(true).Trim(false);
    wxSetEnv(key, value);
    XRCCTRL(*this, "lstEnvVars", wxListBox)->Append(key + _T(" = ") + value);
  }
}

void EnvVarsConfigDlg::OnEditEnvVarClick(wxCommandEvent& WXUNUSED(event))
{
//	Manager::Get()->GetMessageManager()->DebugLog(_T("OnEditEnvVarClick"));

  wxListBox* lstEnvVars = XRCCTRL(*this, "lstEnvVars", wxListBox);
  if (!lstEnvVars)
    return;

  int sel = lstEnvVars->GetSelection();
  if (sel == -1)
    return;

  wxString key = lstEnvVars->GetStringSelection().BeforeFirst(_T('=')).Trim(true).Trim(false);
  if (key.IsEmpty())
    return;

  wxString old_key   = key;
  wxString value     = lstEnvVars->GetStringSelection().AfterFirst(_T('=')).Trim(true).Trim(false);
  wxString old_value = value;

  EditPairDlg dlg(this, key, value, _("Edit variable"),
    EditPairDlg::bmBrowseForDirectory);
  PlaceWindow(&dlg);
  if (dlg.ShowModal() == wxID_OK)
  {
    key.Trim(true).Trim(false);
    value.Trim(true).Trim(false);

    if (value != old_value)
    {
      wxSetEnv(key, value);
      lstEnvVars->SetString(sel, key + _T(" = ") + value);
    }
  }
}

void EnvVarsConfigDlg::OnDeleteEnvVarClick(wxCommandEvent& WXUNUSED(event))
{
//	Manager::Get()->GetMessageManager()->DebugLog(_T("OnDeleteEnvVarClick"));

  wxListBox* lstEnvVars = XRCCTRL(*this, "lstEnvVars", wxListBox);
  if (!lstEnvVars)
    return;

  int sel = lstEnvVars->GetSelection();
  if (sel == -1)
    return;

  wxString key = lstEnvVars->GetStringSelection().BeforeFirst(_T('=')).Trim(true);
  if (key.IsEmpty())
    return;

  if (cbMessageBox(_("Are you sure you want to delete this variable?"),
                   _("Confirmation"),
                   wxYES_NO | wxICON_QUESTION) == wxID_YES)
  {
    wxUnsetEnv(key);
    lstEnvVars->Delete(sel);
  }
}

void EnvVarsConfigDlg::OnClearEnvVarsClick(wxCommandEvent& WXUNUSED(event))
{
//	Manager::Get()->GetMessageManager()->DebugLog(_T("OnClearEnvVarsClick"));

  wxListBox* lstEnvVars = XRCCTRL(*this, "lstEnvVars", wxListBox);
  if (lstEnvVars->IsEmpty())
    return;

  if (cbMessageBox(_("Are you sure you want to clear all variables?"),
                   _("Confirmation"),
                   wxYES | wxNO | wxICON_QUESTION) == wxID_YES)
  {
    // Unset all variables of lstVars
    for (int i=0; i<lstEnvVars->GetCount(); i++)
    {
      wxString key   = lstEnvVars->GetString(i).BeforeFirst(_T('=')).Trim(true);
      if (!key.IsEmpty())
      {
        wxUnsetEnv(key);
      }
    }

    lstEnvVars->Clear();
  }
}

void EnvVarsConfigDlg::OnSetEnvVarsClick(wxCommandEvent& WXUNUSED(event))
{
//	Manager::Get()->GetMessageManager()->DebugLog(_T("OnSetEnvVarsClick"));

  wxListBox* lstEnvVars = XRCCTRL(*this, "lstEnvVars", wxListBox);
  if (lstEnvVars->IsEmpty())
    return;

  if (cbMessageBox(_("Are you sure you want to set all variables?"),
                   _("Confirmation"),
                   wxYES | wxNO | wxICON_QUESTION) == wxID_YES)
  {
    // Set all variables of lstEnvVars
    for (int i=0; i<lstEnvVars->GetCount(); i++)
    {
      wxString value = lstEnvVars->GetStringSelection().AfterFirst(_T('=')).Trim(true).Trim(false);
      wxString key   = lstEnvVars->GetString(i).BeforeFirst(_T('=')).Trim(true);
      if (!key.IsEmpty())
      {
        wxSetEnv(key, value);
      }
    }
  }
}

void EnvVarsConfigDlg::OnUpdateUI(wxUpdateUIEvent& WXUNUSED(event))
{
//	if (Manager::Get() && Manager::Get()->GetMessageManager());
//    Manager::Get()->GetMessageManager()->DebugLog(_T("OnUpdateUI"));

  wxListBox* lstEnvVars = XRCCTRL(*this, "lstEnvVars", wxListBox);
  if (lstEnvVars->IsEmpty())
    return;

  // add/edit/delete/clear env vars
  bool en;
  en = ( lstEnvVars->GetSelection() >= 0 );
  XRCCTRL(*this, "btnEditEnvVar",   wxButton)->Enable(en);
  XRCCTRL(*this, "btnDeleteEnvVar", wxButton)->Enable(en);

  en = ( lstEnvVars->GetCount() != 0 );
  XRCCTRL(*this, "btnClearEnvVars", wxButton)->Enable(en);
  XRCCTRL(*this, "btnSetEnvVars",   wxButton)->Enable(en);
}
