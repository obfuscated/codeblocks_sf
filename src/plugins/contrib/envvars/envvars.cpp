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
  #include <wx/arrstr.h>
  #include <wx/button.h>
  #include <wx/checklst.h>
  #include <wx/utils.h>
  #include <wx/xrc/xmlres.h>
  #include "configmanager.h"
  #include "globals.h"
  #include "licenses.h"
  #include "manager.h"
#endif
#include "editpairdlg.h"
#include "envvars.h"

// Uncomment this for tracing of method calls in C::B's DebugLog:
//#define TRACE_ENVVARS

// Register the plugin
namespace
{
    PluginRegistrant<EnvVars> reg(_T("EnvVars"));
};

BEGIN_EVENT_TABLE(EnvVars, cbPlugin)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(EnvVarsConfigDlg, wxPanel)
  EVT_UPDATE_UI     (XRCID("btnAddEnvVar"),    EnvVarsConfigDlg::OnUpdateUI)
  EVT_UPDATE_UI     (XRCID("btnEditEnvVar"),   EnvVarsConfigDlg::OnUpdateUI)
  EVT_UPDATE_UI     (XRCID("btnDeleteEnvVar"), EnvVarsConfigDlg::OnUpdateUI)
  EVT_UPDATE_UI     (XRCID("btnClearEnvVars"), EnvVarsConfigDlg::OnUpdateUI)

  EVT_LISTBOX_DCLICK(XRCID("lstEnvVars"),      EnvVarsConfigDlg::OnEditEnvVarClick)
  EVT_CHECKLISTBOX  (XRCID("lstEnvVars"),      EnvVarsConfigDlg::OnToggleEnvVarClick)

  EVT_BUTTON        (XRCID("btnAddEnvVar"),    EnvVarsConfigDlg::OnAddEnvVarClick)
  EVT_BUTTON        (XRCID("btnEditEnvVar"),   EnvVarsConfigDlg::OnEditEnvVarClick)
  EVT_BUTTON        (XRCID("btnDeleteEnvVar"), EnvVarsConfigDlg::OnDeleteEnvVarClick)
  EVT_BUTTON        (XRCID("btnClearEnvVars"), EnvVarsConfigDlg::OnClearEnvVarsClick)
  EVT_BUTTON        (XRCID("btnSetEnvVars"),   EnvVarsConfigDlg::OnSetEnvVarsClick)
END_EVENT_TABLE()

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
// Class EnvVars
// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

EnvVars::EnvVars()
{
  //ctor
}// EnvVars

void EnvVars::OnAttach()
{
#if TRACE_ENVVARS
	if (Manager::Get() && Manager::Get()->GetMessageManager());
    Manager::Get()->GetMessageManager()->DebugLog(_T("OnAttach"));
#endif

  if(!Manager::LoadResource(_T("envvars.zip")))
  {
    NotifyMissingFile(_T("envvars.zip"));
  }

  // load and apply configuration (to application only)
  ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("envvars"));
  if (!cfg)
    return;

  wxArrayString list = cfg->EnumerateKeys(_T("/"));
  for (unsigned int i = 0; i < list.GetCount(); ++i)
  {
    // Format: [checked?];[key];[value]
    wxArrayString array = GetArrayFromString(cfg->Read(list[i]));
    if (array.GetCount() == 3)
    {
      wxString check = array[0];
      wxString key   = array[1];
      wxString value = array[2];

      bool bCheck = check.Trim(true).Trim(false).IsSameAs(_T("1"))?true:false;
      key.Trim(true).Trim(false);
      value.Trim(true).Trim(false);

      if (bCheck)
      {
        if (!wxSetEnv(key, value))
        {
          Manager::Get()->GetMessageManager()->Log(_("Setting environment variable '%s' failed."),
            key.c_str());
        }
      }
    }
	}// for
}// OnAttach

void EnvVars::OnRelease(bool appShutDown)
{
  // Nothing to do (so far...)
}// OnRelease

cbConfigurationPanel* EnvVars::GetConfigurationPanel(wxWindow* parent)
{
  EnvVarsConfigDlg* dlg = new EnvVarsConfigDlg(parent, this);
  // deleted by the caller

  return dlg;
}// GetConfigurationPanel


int EnvVars::Configure()
{
#if TRACE_ENVVARS
	if (Manager::Get() && Manager::Get()->GetMessageManager());
    Manager::Get()->GetMessageManager()->DebugLog(_T("Configure"));
#endif

  // Nothing to do (so far...) -> just return success
  return 0;
}// Configure

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
// Class EnvVarsConfigDlg
// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

EnvVarsConfigDlg::EnvVarsConfigDlg(wxWindow* parent, EnvVars* plug) :
  plugin(plug)
{
  wxXmlResource::Get()->LoadPanel(this, parent, _T("dlgEnvVars"));
  LoadSettings();
}// EnvVarsConfigDlg

void EnvVarsConfigDlg::LoadSettings()
{
#if TRACE_ENVVARS
	if (Manager::Get() && Manager::Get()->GetMessageManager());
    Manager::Get()->GetMessageManager()->DebugLog(_T("LoadSettings"));
#endif

  wxCheckListBox* lstEnvVars = XRCCTRL(*this, "lstEnvVars", wxCheckListBox);
  if (!lstEnvVars)
    return;

  // load and apply configuration (to application and GUI)
  ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("envvars"));
  if (!cfg)
    return;

  wxArrayString list = cfg->EnumerateKeys(_T("/"));
  for (unsigned int i = 0; i < list.GetCount(); ++i)
  {
    // Format: [checked?];[key];[value]
    wxArrayString array = GetArrayFromString(cfg->Read(list[i]));
    if (array.GetCount() == 3)
    {
      wxString check = array[0];
      wxString key   = array[1];
      wxString value = array[2];

      bool bCheck = check.Trim(true).Trim(false).IsSameAs(_T("1"))?true:false;
      key.Trim(true).Trim(false);
      value.Trim(true).Trim(false);

      int sel = lstEnvVars->Append(key + _T(" = ") + value);
      lstEnvVars->Check(sel, bCheck);

      if (bCheck)
      {
        if (!wxSetEnv(key, value))
        {
          Manager::Get()->GetMessageManager()->Log(_("Setting environment variable '%s' failed."),
            key.c_str());
          lstEnvVars->Check(sel, false); // Unset to visualise it's NOT set
        }
      }
    }
	}// for
}// EnvVarsConfigDlg

void EnvVarsConfigDlg::SaveSettings()
{
#if TRACE_ENVVARS
	if (Manager::Get() && Manager::Get()->GetMessageManager());
    Manager::Get()->GetMessageManager()->DebugLog(_T("SaveSettings"));
#endif

  ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("envvars"));
  if (!cfg)
    return;

  wxArrayString list = cfg->EnumerateKeys(_T("/"));
  for (unsigned int i=0; i<list.GetCount(); ++i)
  {
    cfg->UnSet(list[i]);
  }// for

  wxCheckListBox* lstEnvVars = XRCCTRL(*this, "lstEnvVars", wxCheckListBox);
  if (!lstEnvVars)
    return;

  for (int i=0; i<lstEnvVars->GetCount(); ++i)
  {
    // Format: [checked?];[key];[value]
    wxString check = (lstEnvVars->IsChecked(i))?_T("1"):_T("0");
    wxString key   = lstEnvVars->GetString(i).BeforeFirst(_T('=')).Trim(true).Trim(false);
    wxString value = lstEnvVars->GetString(i).AfterFirst(_T('=')).Trim(true).Trim(false);

    wxString txt;
    txt << check << _T(";") << key << _T(";") << value;

    wxString cfg_key;
    cfg_key.Printf(_T("EnvVar%d"), i);
		cfg->Write(cfg_key, txt);
	}// for
}// SaveSettings

void EnvVarsConfigDlg::OnAddEnvVarClick(wxCommandEvent& WXUNUSED(event))
{
#if TRACE_ENVVARS
	Manager::Get()->GetMessageManager()->DebugLog(_T("OnAddEnvVarClick"));
#endif

  wxCheckListBox* lstEnvVars = XRCCTRL(*this, "lstEnvVars", wxCheckListBox);
  if (!lstEnvVars)
    return;

  wxString key;
  wxString value;
  EditPairDlg dlg(this, key, value, _("Add new variable"),
    EditPairDlg::bmBrowseForDirectory);
  PlaceWindow(&dlg);
  if (dlg.ShowModal() == wxID_OK)
  {
    key.Trim(true).Trim(false);
    value.Trim(true).Trim(false);
    int sel = lstEnvVars->Append(key + _T(" = ") + value);

    if (!wxSetEnv(key, value))
    {
      Manager::Get()->GetMessageManager()->Log(_("Setting environment variable '%s' failed."),
        key.c_str());
      lstEnvVars->Check(sel, false);
    }
    else
    {
      lstEnvVars->Check(sel, true);
    }
  }
}// OnAddEnvVarClick

void EnvVarsConfigDlg::OnEditEnvVarClick(wxCommandEvent& WXUNUSED(event))
{
#if TRACE_ENVVARS
	Manager::Get()->GetMessageManager()->DebugLog(_T("OnEditEnvVarClick"));
#endif

  wxCheckListBox* lstEnvVars = XRCCTRL(*this, "lstEnvVars", wxCheckListBox);
  if (!lstEnvVars)
    return;

  int sel = lstEnvVars->GetSelection();
  if (sel == -1)
    return;

  wxString key = lstEnvVars->GetStringSelection().BeforeFirst(_T('=')).Trim(true).Trim(false);
  if (key.IsEmpty())
    return;

  wxString value     = lstEnvVars->GetStringSelection().AfterFirst(_T('=')).Trim(true).Trim(false);
  wxString old_key   = key;
  wxString old_value = value;

  EditPairDlg dlg(this, key, value, _("Edit variable"),
    EditPairDlg::bmBrowseForDirectory);
  PlaceWindow(&dlg);
  if (dlg.ShowModal() == wxID_OK)
  {
    key.Trim(true).Trim(false);
    value.Trim(true).Trim(false);

    // filter illegal environment variables with no key
    if (key.IsEmpty())
    {
      cbMessageBox(_("Cannot set an empty environment variable key."),
                   _("Error"), wxOK | wxCENTRE | wxICON_ERROR);
      return;
    }

    // is this environment variable to be set?
    bool bSet = (   ((key != old_key) || (value != old_value))
                 && lstEnvVars->IsChecked(sel) );

    if (bSet)
    {
      // unset the old environment variable if it's key name has changed
      if (key != old_key)
      {
        if (!wxUnsetEnv(old_key))
          Manager::Get()->GetMessageManager()->Log(_("Unsetting environment variable '%s' failed."),
            old_key.c_str());
      }

      // set the new environment
      if (!wxSetEnv(key, value))
        Manager::Get()->GetMessageManager()->Log(_("Setting environment variable '%s' failed."),
          key.c_str());
    }

    // update the GUI to the (new/updated/same) key/value pair anyway
    lstEnvVars->SetString(sel, key + _T(" = ") + value);
  }
}// OnEditEnvVarClick

void EnvVarsConfigDlg::OnToggleEnvVarClick(wxCommandEvent& event)
{
#if TRACE_ENVVARS
	Manager::Get()->GetMessageManager()->DebugLog(_T("OnToggleEnvVarClick"));
#endif

  wxCheckListBox* lstEnvVars = XRCCTRL(*this, "lstEnvVars", wxCheckListBox);
  if (!lstEnvVars)
    return;

  int sel = event.GetInt();
  if(sel < 0)
    return;

  bool bCheck = lstEnvVars->IsChecked(sel);

  wxString key = lstEnvVars->GetString(sel).BeforeFirst(_T('=')).Trim(true).Trim(false);
  if (key.IsEmpty())
    return;

  if (bCheck)
  {
    // Is has been toggled ON -> set environment variable now
    wxString value = lstEnvVars->GetString(sel).AfterFirst(_T('=')).Trim(true).Trim(false);
    if (!wxSetEnv(key, value))
      Manager::Get()->GetMessageManager()->Log(_("Setting environment variable '%s' failed."),
        key.c_str());
  }
  else
  {
    // Is has been toggled OFF -> unsset environment variable now
    if (!wxUnsetEnv(key))
      Manager::Get()->GetMessageManager()->Log(_("Unsetting environment variable '%s' failed."),
        key.c_str());
  }
}// OnToggleEnvVarClick

void EnvVarsConfigDlg::OnDeleteEnvVarClick(wxCommandEvent& WXUNUSED(event))
{
#if TRACE_ENVVARS
	Manager::Get()->GetMessageManager()->DebugLog(_T("OnDeleteEnvVarClick"));
#endif

  wxCheckListBox* lstEnvVars = XRCCTRL(*this, "lstEnvVars", wxCheckListBox);
  if (!lstEnvVars)
    return;

  int sel = lstEnvVars->GetSelection();
  if (sel == -1)
    return;

  wxString key = lstEnvVars->GetStringSelection().BeforeFirst(_T('=')).Trim(true).Trim(false);
  if (key.IsEmpty())
    return;

  if (cbMessageBox(_("Are you sure you want to delete this variable?"),
                   _("Confirmation"),
                   wxYES_NO | wxICON_QUESTION) == wxID_YES)
  {
    if (!wxUnsetEnv(key))
      Manager::Get()->GetMessageManager()->Log(_("Unsetting environment variable '%s' failed."),
        key.c_str());

    lstEnvVars->Delete(sel);
  }
}// OnDeleteEnvVarClick

void EnvVarsConfigDlg::OnClearEnvVarsClick(wxCommandEvent& WXUNUSED(event))
{
#if TRACE_ENVVARS
	Manager::Get()->GetMessageManager()->DebugLog(_T("OnClearEnvVarsClick"));
#endif

  wxCheckListBox* lstEnvVars = XRCCTRL(*this, "lstEnvVars", wxCheckListBox);
  if (lstEnvVars->IsEmpty())
    return;

  if (cbMessageBox(_("Are you sure you want to clear all variables?"),
                   _("Confirmation"),
                   wxYES | wxNO | wxICON_QUESTION) == wxID_YES)
  {
    wxString envsNotUnSet(wxEmptyString);

    // Unset all (checked) variables of lstVars
    for (int i=0; i<lstEnvVars->GetCount(); ++i)
    {
      // Note: It's better nnot to just clear all because wxUnsetEnv would
      //       fail in case an environment variable is not set (not chekcd).
      if (lstEnvVars->IsChecked(i))
      {
        wxString key = lstEnvVars->GetString(i).BeforeFirst(_T('=')).Trim(true).Trim(false);
        if (!key.IsEmpty())
        {
          if (!wxUnsetEnv(key))
          {
            // Setting env.-variable failed. Remember this key to report later.
            if (envsNotUnSet.IsEmpty())
              envsNotUnSet << key;
            else
              envsNotUnSet << _T(";") << key;
          }
        }
      }
    }// for

    lstEnvVars->Clear();

    if (!envsNotUnSet.IsEmpty())
    {
      wxString msg;
      msg.Printf( _("There was an error unsetting the following environment variables:\n%s"),
                  envsNotUnSet.c_str() );
      cbMessageBox(msg, _("Error"), wxOK | wxCENTRE | wxICON_ERROR);
    }
  }
}// OnClearEnvVarsClick

void EnvVarsConfigDlg::OnSetEnvVarsClick(wxCommandEvent& WXUNUSED(event))
{
#if TRACE_ENVVARS
	Manager::Get()->GetMessageManager()->DebugLog(_T("OnSetEnvVarsClick"));
#endif

  wxCheckListBox* lstEnvVars = XRCCTRL(*this, "lstEnvVars", wxCheckListBox);
  if (lstEnvVars->IsEmpty())
    return;

  if (cbMessageBox(_("Are you sure you want to set all variables?"),
                   _("Confirmation"),
                   wxYES | wxNO | wxICON_QUESTION) == wxID_YES)
  {
    wxString envsNotSet(wxEmptyString);

    // Set all (checked) variables of lstEnvVars
    for (int i=0; i<lstEnvVars->GetCount(); ++i)
    {
      if (lstEnvVars->IsChecked(i))
      {
        wxString key   = lstEnvVars->GetString(i).BeforeFirst(_T('=')).Trim(true).Trim(false);
        wxString value = lstEnvVars->GetString(i).AfterFirst(_T('=')).Trim(true).Trim(false);
        if (!key.IsEmpty())
        {
          if (!wxSetEnv(key, value))
          {
            // Setting env.-variable failed. Remember this key to report later.
            if (envsNotSet.IsEmpty())
              envsNotSet << key;
            else
              envsNotSet << _T(";") << key;
          }
        }
      }
    }// for

    if (!envsNotSet.IsEmpty())
    {
      wxString msg;
      msg.Printf( _("There was an error setting the following environment variables:\n%s"),
                  envsNotSet.c_str() );
      cbMessageBox(msg, _("Error"), wxOK | wxCENTRE | wxICON_ERROR);
    }
  }
}// OnSetEnvVarsClick

void EnvVarsConfigDlg::OnUpdateUI(wxUpdateUIEvent& WXUNUSED(event))
{
#if TRACE_ENVVARS
	if (Manager::Get() && Manager::Get()->GetMessageManager());
    Manager::Get()->GetMessageManager()->DebugLog(_T("OnUpdateUI"));
#endif

  wxCheckListBox* lstEnvVars = XRCCTRL(*this, "lstEnvVars", wxCheckListBox);
  if (lstEnvVars->IsEmpty())
    return;

  // edit/delete/clear/set env vars
  bool en;
  en = ( lstEnvVars->GetSelection() >= 0 );
  XRCCTRL(*this, "btnEditEnvVar",   wxButton)->Enable(en);
  XRCCTRL(*this, "btnDeleteEnvVar", wxButton)->Enable(en);

  en = ( lstEnvVars->GetCount() != 0 );
  XRCCTRL(*this, "btnClearEnvVars", wxButton)->Enable(en);
  XRCCTRL(*this, "btnSetEnvVars",   wxButton)->Enable(en);
}// OnUpdateUI
