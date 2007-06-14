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

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

#include "sdk.h"
#ifndef CB_PRECOMP
  #include <wx/arrstr.h>
  #include <wx/button.h>
  #include <wx/checkbox.h>
  #include <wx/checklst.h>
  #include <wx/choice.h>
  #include <wx/panel.h>
  #include <wx/textdlg.h>
  #include <wx/xrc/xmlres.h>

  #include "globals.h"
  #include "manager.h"
  #include "configmanager.h"
  #include "messagemanager.h"
#endif

#include "editpairdlg.h"

#include "envvars_common.h"
#include "envvars_cfgdlg.h"

// Uncomment this for tracing of method calls in C::B's DebugLog:
//#define TRACE_ENVVARS

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

BEGIN_EVENT_TABLE(EnvVarsConfigDlg, wxPanel)
  EVT_UPDATE_UI     (XRCID("btnRemoveSet"),    EnvVarsConfigDlg::OnUpdateUI)
  EVT_UPDATE_UI     (XRCID("btnAddEnvVar"),    EnvVarsConfigDlg::OnUpdateUI)
  EVT_UPDATE_UI     (XRCID("btnEditEnvVar"),   EnvVarsConfigDlg::OnUpdateUI)
  EVT_UPDATE_UI     (XRCID("btnDeleteEnvVar"), EnvVarsConfigDlg::OnUpdateUI)
  EVT_UPDATE_UI     (XRCID("btnClearEnvVars"), EnvVarsConfigDlg::OnUpdateUI)

  EVT_LISTBOX_DCLICK(XRCID("lstEnvVars"),      EnvVarsConfigDlg::OnEditEnvVarClick)
  EVT_CHECKLISTBOX  (XRCID("lstEnvVars"),      EnvVarsConfigDlg::OnToggleEnvVarClick)

  EVT_CHOICE        (XRCID("choSet"),          EnvVarsConfigDlg::OnSetClick)
  EVT_BUTTON        (XRCID("btnCreateSet"),    EnvVarsConfigDlg::OnCreateSetClick)
  EVT_BUTTON        (XRCID("btnRemoveSet"),    EnvVarsConfigDlg::OnRemoveSetClick)

  EVT_BUTTON        (XRCID("btnAddEnvVar"),    EnvVarsConfigDlg::OnAddEnvVarClick)
  EVT_BUTTON        (XRCID("btnEditEnvVar"),   EnvVarsConfigDlg::OnEditEnvVarClick)
  EVT_BUTTON        (XRCID("btnDeleteEnvVar"), EnvVarsConfigDlg::OnDeleteEnvVarClick)
  EVT_BUTTON        (XRCID("btnClearEnvVars"), EnvVarsConfigDlg::OnClearEnvVarsClick)
  EVT_BUTTON        (XRCID("btnSetEnvVars"),   EnvVarsConfigDlg::OnSetEnvVarsClick)
END_EVENT_TABLE()

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

EnvVarsConfigDlg::EnvVarsConfigDlg(wxWindow* parent, EnvVars* plugin):
  m_pPlugin(plugin)
{
  wxXmlResource::Get()->LoadPanel(this, parent, _T("dlgEnvVars"));
  LoadSettings();
}// EnvVarsConfigDlg

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVarsConfigDlg::OnUpdateUI(wxUpdateUIEvent& WXUNUSED(event))
{
#if TRACE_ENVVARS
  DBGLOG(_T("OnUpdateUI"));
#endif

  bool en;

  // toggle remove envvar set button
  wxChoice* choSet = XRCCTRL(*this, "choSet", wxChoice);
  if (!choSet)
    return;
  en = (choSet->GetCount() > 1);
  XRCCTRL(*this, "btnRemoveSet",    wxButton)->Enable(en);

  // toggle edit/delete/clear/set env vars buttons
  wxCheckListBox* lstEnvVars = XRCCTRL(*this, "lstEnvVars", wxCheckListBox);
  if (!lstEnvVars)
    return;
  if (lstEnvVars->IsEmpty())
    return;

  en = (lstEnvVars->GetSelection() >= 0);
  XRCCTRL(*this, "btnEditEnvVar",   wxButton)->Enable(en);
  XRCCTRL(*this, "btnDeleteEnvVar", wxButton)->Enable(en);

  en = (lstEnvVars->GetCount() != 0);
  XRCCTRL(*this, "btnClearEnvVars", wxButton)->Enable(en);
  XRCCTRL(*this, "btnSetEnvVars",   wxButton)->Enable(en);
}// OnUpdateUI

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVarsConfigDlg::LoadSettings()
{
#if TRACE_ENVVARS
  DBGLOG(_T("LoadSettings"));
#endif

  wxChoice* choSet = XRCCTRL(*this, "choSet", wxChoice);
  if (!choSet)
    return;

  wxCheckListBox* lstEnvVars = XRCCTRL(*this, "lstEnvVars", wxCheckListBox);
  if (!lstEnvVars)
    return;

  wxCheckBox* chkDebugLog = XRCCTRL(*this, "chkDebugLog", wxCheckBox);
  if (!chkDebugLog)
    return;

  // load and apply configuration (to application and GUI)
  ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("envvars"));
  if (!cfg)
    return;

  choSet->Clear();
  lstEnvVars->Clear();
  chkDebugLog->SetValue(cfg->ReadBool(_T("/debug_log")));

  // Read the currently active envvar set
  wxString active_set     = nsEnvVars::GetActiveSetName();
  int      active_set_idx = 0;

  // Read all envvar sets available
  wxArrayString set_names = nsEnvVars::GetEnvvarSetNames();
  unsigned int  num_sets  = set_names.GetCount();
  EV_DBGLOG(_T("EnvVars: Found %d envvar sets in config."), num_sets);
  unsigned int num_sets_applied = 0;
  for (unsigned int i=0; i<num_sets; ++i)
  {
    choSet->Append(set_names[i]);
    if (active_set.IsSameAs(set_names[i]))
      active_set_idx = i;
    num_sets_applied++;
  }
  EV_DBGLOG(_T("EnvVars: Setup %d/%d envvar sets from config."), num_sets_applied, num_sets);
  if (choSet->GetCount()>active_set_idx) // Select the last active set (from config)
    choSet->SetSelection(active_set_idx);

  // Show currently activated set in debug log (for reference)
  wxString active_set_path = nsEnvVars::GetSetPathByName(active_set);
  EV_DBGLOG(_T("EnvVars: Active envvar set is '%s' at index %d, config path '%s'."),
    active_set.c_str(), active_set_idx, active_set_path.c_str());

  // Read and show all envvars from currently active set in listbox
  wxArrayString vars     = nsEnvVars::GetEnvvarsBySetPath(active_set_path);
  size_t envvars_total   = vars.GetCount();
  size_t envvars_applied = 0;
  for (unsigned int i=0; i<envvars_total; ++i)
  {
    // Format: [checked?]|[key]|[value]
    wxArrayString var_array = nsEnvVars::EnvvarStringTokeniser(vars[i]);
    if (nsEnvVars::EnvvarApply(var_array, lstEnvVars))
      envvars_applied++;
    else
      EV_DBGLOG(_T("EnvVars: Invalid envvar in '%s' at position #%d."),
        active_set_path.c_str(), i);
	}// for

	if (envvars_total>0)
    EV_DBGLOG(_T("EnvVars: %d/%d envvars applied within C::B focus."),
      envvars_applied, envvars_total);
}// LoadSettings

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVarsConfigDlg::SaveSettings()
{
#if TRACE_ENVVARS
	if (Manager::Get() && Manager::Get()->GetMessageManager());
    DBGLOG(_T("SaveSettings"));
#endif

  wxChoice* choSet = XRCCTRL(*this, "choSet", wxChoice);
  if (!choSet)
    return;

  wxCheckListBox* lstEnvVars = XRCCTRL(*this, "lstEnvVars", wxCheckListBox);
  if (!lstEnvVars)
    return;

  wxCheckBox* chkDebugLog = XRCCTRL(*this, "chkDebugLog", wxCheckBox);
  if (!chkDebugLog)
    return;

  ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("envvars"));
  if (!cfg)
    return;

  wxString active_set = choSet->GetString(choSet->GetCurrentSelection());
  if (active_set.IsEmpty())
    active_set = nsEnvVars::EnvVarsDefault;

  SaveSettingsActiveSet(active_set);

  wxString active_set_path = nsEnvVars::GetSetPathByName(active_set, false);
  EV_DBGLOG(_T("EnvVars: Removing (old) envvar set '%s' at path '%s' from config."),
    active_set.c_str(), active_set_path.c_str());
  cfg->DeleteSubPath(active_set_path);

	EV_DBGLOG(_T("EnvVars: Saving (new) envvar set '%s'."), active_set.c_str());
  cfg->SetPath(active_set_path);

  for (int i=0; i<lstEnvVars->GetCount(); ++i)
  {
    // Format: [checked?]|[key]|[value]
    wxString check = (lstEnvVars->IsChecked(i))?_T("1"):_T("0");
    wxString key   = lstEnvVars->GetString(i).BeforeFirst(_T('=')).Trim(true).Trim(false);
    wxString value = lstEnvVars->GetString(i).AfterFirst(_T('=')).Trim(true).Trim(false);

    wxString txt;
    txt << check << nsEnvVars::EnvVarsSep << key
                 << nsEnvVars::EnvVarsSep << value;

    wxString cfg_key;
    cfg_key.Printf(_T("EnvVar%d"), i);
		cfg->Write(cfg_key, txt);
	}// for

  cfg->Write(_T("/debug_log"), chkDebugLog->GetValue());
}// SaveSettings

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVarsConfigDlg::SaveSettingsActiveSet(wxString active_set)
{
#if TRACE_ENVVARS
  DBGLOG(_T("SaveSettingsActiveSet"));
#endif

  ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("envvars"));
  if (!cfg)
    return;

  if (active_set.IsEmpty())
    active_set = nsEnvVars::EnvVarsDefault;

  EV_DBGLOG(_T("EnvVars: Saving '%s' as active envvar set to config."), active_set.c_str());
  cfg->Write(_T("/active_set"), active_set);
}// SaveSettingsActiveSet

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVarsConfigDlg::OnAddEnvVarClick(wxCommandEvent& WXUNUSED(event))
{
#if TRACE_ENVVARS
	DBGLOG(_T("OnAddEnvVarClick"));
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

    if (nsEnvVars::EnvvarVeto(key))
      return;

    int sel = lstEnvVars->Append(key + _T(" = ") + value);
    if (nsEnvVars::EnvvarApply(key, value, lstEnvVars, sel))
      lstEnvVars->Check(sel, true);
  }
}// OnAddEnvVarClick

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVarsConfigDlg::OnEditEnvVarClick(wxCommandEvent& WXUNUSED(event))
{
#if TRACE_ENVVARS
	DBGLOG(_T("OnEditEnvVarClick"));
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
  if (dlg.ShowModal() != wxID_OK)
    return;

  key.Trim(true).Trim(false);
  value.Trim(true).Trim(false);

  // filter illegal envvars with no key
  if (key.IsEmpty())
  {
    cbMessageBox(_("Cannot set an empty environment variable key."),
                 _("Error"), wxOK | wxCENTRE | wxICON_ERROR);
    return;
  }

  // is this envvar to be set?
  bool bDoSet = (   ((key != old_key) || (value != old_value))
                 && lstEnvVars->IsChecked(sel) );
  if (bDoSet)
  {
    // unset the old envvar if it's key name has changed
    if (key != old_key)
    {
      nsEnvVars::EnvvarDiscard(old_key); // Don't care about return value
      if (nsEnvVars::EnvvarVeto(key, lstEnvVars, sel))
        return;
    }

    // set the new envvar
    nsEnvVars::EnvvarApply(key, value, lstEnvVars, sel); // Don't care about return value
  }

  // update the GUI to the (new/updated/same) key/value pair anyway
  lstEnvVars->SetString(sel, key + _T(" = ") + value);
}// OnEditEnvVarClick

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVarsConfigDlg::OnToggleEnvVarClick(wxCommandEvent& event)
{
#if TRACE_ENVVARS
	DBGLOG(_T("OnToggleEnvVarClick"));
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
    // Is has been toggled ON -> set envvar now
    wxString value = lstEnvVars->GetString(sel).AfterFirst(_T('=')).Trim(true).Trim(false);
    nsEnvVars::EnvvarApply(key, value, lstEnvVars, sel); // Don't care about return value
  }
  else
  {
    // Is has been toggled OFF -> unsset envvar now
    nsEnvVars::EnvvarDiscard(key); // Don't care about return value
  }
}// OnToggleEnvVarClick

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVarsConfigDlg::OnSetClick(wxCommandEvent& event)
{
#if TRACE_ENVVARS
	DBGLOG(_T("OnSetClick"));
#endif

  SaveSettingsActiveSet(event.GetString());
  LoadSettings();
}// OnSetClick

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVarsConfigDlg::OnCreateSetClick(wxCommandEvent& WXUNUSED(event))
{
#if TRACE_ENVVARS
	DBGLOG(_T("OnCreateSetClick"));
#endif

  wxString set = wxGetTextFromUser(_("Enter (lower case) name for new environment variables set:"),
                                   _("Input Set"), nsEnvVars::EnvVarsDefault);
  if (set.IsEmpty())
    return;

  wxChoice* choSet = XRCCTRL(*this, "choSet", wxChoice);
  if (!choSet)
    return;

  for (int i=0; i<choSet->GetCount(); ++i)
  {
    if (set.MakeLower().IsSameAs(choSet->GetString(i).MakeLower()))
    {
      cbMessageBox(_("This set already exists."), _("Error"),
                   wxOK | wxCENTRE | wxICON_EXCLAMATION);
      return;
    }
  }

  wxCheckListBox* lstEnvVars = XRCCTRL(*this, "lstEnvVars", wxCheckListBox);
  if (!lstEnvVars)
    return;

  EV_DBGLOG(_T("EnvVars: Unsetting variables of envvar set '%s'."),
    choSet->GetString(choSet->GetCurrentSelection()).c_str());
  nsEnvVars::EnvvarsClear(lstEnvVars); // Don't care about return value
  lstEnvVars->Clear();

  int idx = choSet->Append(set.MakeLower());
  choSet->SetSelection(idx);

	SaveSettings();
  LoadSettings();
}// OnCreateSetClick

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVarsConfigDlg::OnRemoveSetClick(wxCommandEvent& WXUNUSED(event))
{
#if TRACE_ENVVARS
	DBGLOG(_T("OnRemoveSetClick"));
#endif

  wxChoice* choSet = XRCCTRL(*this, "choSet", wxChoice);
  if (!choSet)
    return;

  if (choSet->GetCount()<2)
  {
    cbMessageBox(_("Must have at least one set active (can be empty)."),
                 _("Information"), wxICON_INFORMATION);
    return;
  }

  wxCheckListBox* lstEnvVars = XRCCTRL(*this, "lstEnvVars", wxCheckListBox);
  if (!lstEnvVars)
    return;

  ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("envvars"));
  if (!cfg)
    return;

  if (cbMessageBox(_("Are you sure you want to delete the set?"),
                   _("Confirmation"),
                   wxYES | wxNO | wxICON_QUESTION) == wxID_YES)
  {
    // Obtain active set
    int      active_set_idx = choSet->GetCurrentSelection();
    wxString active_set     = choSet->GetString(active_set_idx);

    // Remove envvars from C::B focus (and listbox)
    EV_DBGLOG(_T("EnvVars: Unsetting variables of envvar set '%s'."), active_set.c_str());
    nsEnvVars::EnvvarsClear(lstEnvVars); // Don't care about return value

    // Remove envvars set from config
    wxString active_set_path = nsEnvVars::GetSetPathByName(active_set, false);
    EV_DBGLOG(_T("EnvVars: Removing envvar set '%s' at path '%s' from config."),
      active_set.c_str(), active_set_path.c_str());
    cfg->DeleteSubPath(active_set_path);

    // Remove envvars set from choicebox
    choSet->Delete(active_set_idx);

    if (active_set_idx>0)
      choSet->SetSelection(active_set_idx-1);
    else
      choSet->SetSelection(0);
  }// if

  SaveSettingsActiveSet(choSet->GetString(choSet->GetCurrentSelection()));
  LoadSettings();
}// OnRemoveSetClick

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVarsConfigDlg::OnDeleteEnvVarClick(wxCommandEvent& WXUNUSED(event))
{
#if TRACE_ENVVARS
	DBGLOG(_T("OnDeleteEnvVarClick"));
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
    nsEnvVars::EnvvarDiscard(key); // Don't care about return value
    lstEnvVars->Delete(sel);
  }
}// OnDeleteEnvVarClick

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVarsConfigDlg::OnClearEnvVarsClick(wxCommandEvent& WXUNUSED(event))
{
#if TRACE_ENVVARS
	DBGLOG(_T("OnClearEnvVarsClick"));
#endif

  wxCheckListBox* lstEnvVars = XRCCTRL(*this, "lstEnvVars", wxCheckListBox);
  if (!lstEnvVars)
    return;
  if (lstEnvVars->IsEmpty())
    return;

  if (cbMessageBox(_("Are you sure you want to clear and unset all variables?"),
                   _("Confirmation"),
                   wxYES | wxNO | wxICON_QUESTION) != wxID_YES)
    return;

  nsEnvVars::EnvvarsClear(lstEnvVars); // Don't care about return value
}// OnClearEnvVarsClick

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVarsConfigDlg::OnSetEnvVarsClick(wxCommandEvent& WXUNUSED(event))
{
#if TRACE_ENVVARS
	DBGLOG(_T("OnSetEnvVarsClick"));
#endif

  wxCheckListBox* lstEnvVars = XRCCTRL(*this, "lstEnvVars", wxCheckListBox);
  if (!lstEnvVars)
    return;
  if (lstEnvVars->IsEmpty())
    return;

  if (cbMessageBox(_("Are you sure you want to set all variables?"),
                   _("Confirmation"),
                   wxYES | wxNO | wxICON_QUESTION) != wxID_YES)
    return;

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
        if (!nsEnvVars::EnvvarApply(key, value))
        {
          // Setting envvar failed. Remember this key to report later.
          if (envsNotSet.IsEmpty())
            envsNotSet << key;
          else
            envsNotSet << _T(", ") << key;
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
}// OnSetEnvVarsClick
