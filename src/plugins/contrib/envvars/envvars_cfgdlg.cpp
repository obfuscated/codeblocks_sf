/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
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
  #include "logmanager.h"
#endif

#include "editpairdlg.h"

#include "envvars_common.h"
#include "envvars_cfgdlg.h"

// TODO (morten#1#): Save changes if another set is selected (more convenient).

// Uncomment this for tracing of method calls in C::B's DebugLog:
//#define TRACE_ENVVARS


// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

BEGIN_EVENT_TABLE(EnvVarsConfigDlg, wxPanel)
  EVT_CHOICE        (XRCID("choSet"),          EnvVarsConfigDlg::OnSetClick)
  EVT_BUTTON        (XRCID("btnCreateSet"),    EnvVarsConfigDlg::OnCreateSetClick)
  EVT_BUTTON        (XRCID("btnCloneSet"),     EnvVarsConfigDlg::OnCloneSetClick)
  EVT_BUTTON        (XRCID("btnRemoveSet"),    EnvVarsConfigDlg::OnRemoveSetClick)
  EVT_UPDATE_UI     (XRCID("btnRemoveSet"),    EnvVarsConfigDlg::OnUpdateUI)

  EVT_LISTBOX_DCLICK(XRCID("lstEnvVars"),      EnvVarsConfigDlg::OnEditEnvVarClick)
  EVT_CHECKLISTBOX  (XRCID("lstEnvVars"),      EnvVarsConfigDlg::OnToggleEnvVarClick)

  EVT_BUTTON        (XRCID("btnAddEnvVar"),    EnvVarsConfigDlg::OnAddEnvVarClick)
  EVT_BUTTON        (XRCID("btnEditEnvVar"),   EnvVarsConfigDlg::OnEditEnvVarClick)
  EVT_BUTTON        (XRCID("btnDeleteEnvVar"), EnvVarsConfigDlg::OnDeleteEnvVarClick)
  EVT_BUTTON        (XRCID("btnClearEnvVars"), EnvVarsConfigDlg::OnClearEnvVarsClick)
  EVT_BUTTON        (XRCID("btnSetEnvVars"),   EnvVarsConfigDlg::OnSetEnvVarsClick)
  EVT_UPDATE_UI     (XRCID("btnAddEnvVar"),    EnvVarsConfigDlg::OnUpdateUI)
  EVT_UPDATE_UI     (XRCID("btnEditEnvVar"),   EnvVarsConfigDlg::OnUpdateUI)
  EVT_UPDATE_UI     (XRCID("btnDeleteEnvVar"), EnvVarsConfigDlg::OnUpdateUI)
  EVT_UPDATE_UI     (XRCID("btnClearEnvVars"), EnvVarsConfigDlg::OnUpdateUI)
END_EVENT_TABLE()

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

EnvVarsConfigDlg::EnvVarsConfigDlg(wxWindow* parent)
{
  wxXmlResource::Get()->LoadPanel(this, parent, _T("dlgEnvVars"));
  LoadSettings();
}// EnvVarsConfigDlg

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVarsConfigDlg::OnUpdateUI(wxUpdateUIEvent& WXUNUSED(event))
{
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("OnUpdateUI")));
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
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("LoadSettings")));
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
  EV_DBGLOG(_T("EnvVars: Found %u envvar sets in config."), num_sets);
  unsigned int num_sets_applied = 0;
  for (unsigned int i=0; i<num_sets; ++i)
  {
    choSet->Append(set_names[i]);
    if (active_set.IsSameAs(set_names[i]))
      active_set_idx = i;
    num_sets_applied++;
  }
  EV_DBGLOG(_T("EnvVars: Setup %u/%u envvar sets from config."), num_sets_applied, num_sets);
  if ((int)choSet->GetCount()>active_set_idx) // Select the last active set (from config)
    choSet->SetSelection(active_set_idx);

  // Show currently activated set in debug log (for reference)
  wxString active_set_path = nsEnvVars::GetSetPathByName(active_set);
  EV_DBGLOG(_T("EnvVars: Active envvar set is '%s' at index %d, config path '%s'."),
    active_set.wx_str(), active_set_idx, active_set_path.wx_str());

  // NOTE: Keep this in sync with nsEnvVars::EnvvarSetApply
  // Read and show all envvars from currently active set in listbox
  wxArrayString vars     = nsEnvVars::GetEnvvarsBySetPath(active_set_path);
  size_t envvars_total   = vars.GetCount();
  size_t envvars_applied = 0;
  for (unsigned int i=0; i<envvars_total; ++i)
  {
    // Format: [checked?]|[key]|[value]
    wxArrayString var_array = nsEnvVars::EnvvarStringTokeniser(vars[i]);
    if (nsEnvVars::EnvvarArrayApply(var_array, lstEnvVars))
      envvars_applied++;
    else
    {
      EV_DBGLOG(_T("EnvVars: Invalid envvar in '%s' at position #%u."),
        active_set_path.wx_str(), i);
    }
  }// for

  if (envvars_total>0)
  {
    EV_DBGLOG(_T("EnvVars: %lu/%lu envvars applied within C::B focus."),
      static_cast<unsigned long>(envvars_applied), static_cast<unsigned long>(envvars_total));
  }
}// LoadSettings

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVarsConfigDlg::SaveSettings()
{
#if defined(TRACE_ENVVARS)
  if (Manager::Get() && Manager::Get()->GetLogManager());
    Manager::Get()->GetLogManager()->DebugLog(F(_T("SaveSettings")));
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
    active_set.wx_str(), active_set_path.wx_str());
  cfg->DeleteSubPath(active_set_path);

  EV_DBGLOG(_T("EnvVars: Saving (new) envvar set '%s'."), active_set.wx_str());
  cfg->SetPath(active_set_path);

  for (int i=0; i<(int)lstEnvVars->GetCount(); ++i)
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
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("SaveSettingsActiveSet")));
#endif

  ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("envvars"));
  if (!cfg)
    return;

  if (active_set.IsEmpty())
    active_set = nsEnvVars::EnvVarsDefault;

  EV_DBGLOG(_T("EnvVars: Saving '%s' as active envvar set to config."), active_set.wx_str());
  cfg->Write(_T("/active_set"), active_set);
}// SaveSettingsActiveSet

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVarsConfigDlg::OnSetClick(wxCommandEvent& event)
{
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("OnSetClick")));
#endif

  SaveSettingsActiveSet(event.GetString());
  LoadSettings();
}// OnSetClick

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVarsConfigDlg::OnCreateSetClick(wxCommandEvent& WXUNUSED(event))
{
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("OnCreateSetClick")));
#endif

  wxChoice* choSet = XRCCTRL(*this, "choSet", wxChoice);
  if (!choSet)
    return;

  wxString set = wxGetTextFromUser(_("Enter (lower case) name for new environment variables set:"),
                                   _("Input Set"), nsEnvVars::EnvVarsDefault);
  if (set.IsEmpty() || (!VerifySetUnique(choSet, set)))
    return;

  wxCheckListBox* lstEnvVars = XRCCTRL(*this, "lstEnvVars", wxCheckListBox);
  if (!lstEnvVars)
    return;

  EV_DBGLOG(_T("EnvVars: Unsetting variables of envvar set '%s'."),
    choSet->GetString(choSet->GetCurrentSelection()).wx_str());
  nsEnvVars::EnvvarsClearUI(lstEnvVars); // Don't care about return value
  lstEnvVars->Clear();

  int idx = choSet->Append(set.MakeLower());
  choSet->SetSelection(idx);

  SaveSettings();
  LoadSettings();
}// OnCreateSetClick

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVarsConfigDlg::OnCloneSetClick(wxCommandEvent& WXUNUSED(event))
{
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("OnCloneSetClick")));
#endif

  wxChoice* choSet = XRCCTRL(*this, "choSet", wxChoice);
  if (!choSet)
    return;

  ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("envvars"));
  if (!cfg)
    return;

  wxString set = wxGetTextFromUser(_("Enter (lower case) name for cloned environment variables set:"),
                                   _("Input Set"), nsEnvVars::EnvVarsDefault);
  if (set.IsEmpty() || (!VerifySetUnique(choSet, set)))
    return;

  int idx = choSet->Append(set.MakeLower());
  choSet->SetSelection(idx);

  // Clone envvars set in config
  SaveSettings();
  LoadSettings();
}// OnCloneSetClick

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVarsConfigDlg::OnRemoveSetClick(wxCommandEvent& WXUNUSED(event))
{
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("OnRemoveSetClick")));
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
    EV_DBGLOG(_T("EnvVars: Unsetting variables of envvar set '%s'."), active_set.wx_str());
    nsEnvVars::EnvvarsClearUI(lstEnvVars); // Don't care about return value

    // Remove envvars set from config
    wxString active_set_path = nsEnvVars::GetSetPathByName(active_set, false);
    EV_DBGLOG(_T("EnvVars: Removing envvar set '%s' at path '%s' from config."),
      active_set.wx_str(), active_set_path.wx_str());
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

void EnvVarsConfigDlg::OnToggleEnvVarClick(wxCommandEvent& event)
{
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("OnToggleEnvVarClick")));
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
    if (!nsEnvVars::EnvvarApply(key, value))
      lstEnvVars->Check(sel, false); // Unset on UI to mark it's NOT set
  }
  else
  {
    // Is has been toggled OFF -> unsset envvar now
    nsEnvVars::EnvvarDiscard(key); // Don't care about return value
  }
}// OnToggleEnvVarClick

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVarsConfigDlg::OnAddEnvVarClick(wxCommandEvent& WXUNUSED(event))
{
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("OnAddEnvVarClick")));
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

    if (nsEnvVars::EnvvarVetoUI(key, NULL, -1))
      return;

    int  sel     = lstEnvVars->Append(key + _T(" = ") + value, new nsEnvVars::EnvVariableListClientData(key, value));
    bool success = nsEnvVars::EnvvarApply(key, value);
    if (sel>=0)
      lstEnvVars->Check(sel, success);
  }
}// OnAddEnvVarClick

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVarsConfigDlg::OnEditEnvVarClick(wxCommandEvent& WXUNUSED(event))
{
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("OnEditEnvVarClick")));
#endif

  wxCheckListBox* lstEnvVars = XRCCTRL(*this, "lstEnvVars", wxCheckListBox);
  if (!lstEnvVars)
    return;

  int sel = lstEnvVars->GetSelection();
  if (sel == -1)
    return;

  nsEnvVars::EnvVariableListClientData *data;
  data = static_cast<nsEnvVars::EnvVariableListClientData*>(lstEnvVars->GetClientObject(sel));
  wxString key = data->key;
  if (key.IsEmpty())
    return;
  bool was_checked = lstEnvVars->IsChecked(sel);
  wxString value = data->value;

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
  bool bDoSet = (   ((key != data->key) || (value != data->value))
                 && lstEnvVars->IsChecked(sel) );
  if (bDoSet)
  {
    // unset the old envvar if it's key name has changed
    if (key != data->key)
    {
      nsEnvVars::EnvvarDiscard(data->key); // Don't care about return value
      if (nsEnvVars::EnvvarVetoUI(key, lstEnvVars, sel))
        return;
    }

    // set the new envvar
    if (!nsEnvVars::EnvvarApply(key, value))
    {
      lstEnvVars->Check(sel, false); // Unset on UI to mark it's NOT set
      was_checked = false;
    }
  }

  // update the GUI to the (new/updated/same) key/value pair anyway
  lstEnvVars->SetString(sel, key + _T(" = ") + value);
  lstEnvVars->Check(sel, was_checked);
  data->key = key;
  data->value = value;
}// OnEditEnvVarClick

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVarsConfigDlg::OnDeleteEnvVarClick(wxCommandEvent& WXUNUSED(event))
{
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("OnDeleteEnvVarClick")));
#endif

  wxCheckListBox* lstEnvVars = XRCCTRL(*this, "lstEnvVars", wxCheckListBox);
  if (!lstEnvVars)
    return;

  int sel = lstEnvVars->GetSelection();
  if (sel == -1)
    return;

  const wxString &key = static_cast<nsEnvVars::EnvVariableListClientData*>(lstEnvVars->GetClientObject(sel))->key;
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
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("OnClearEnvVarsClick")));
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

  nsEnvVars::EnvvarsClearUI(lstEnvVars); // Don't care about return value
}// OnClearEnvVarsClick

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVarsConfigDlg::OnSetEnvVarsClick(wxCommandEvent& WXUNUSED(event))
{
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("OnSetEnvVarsClick")));
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
  for (int i=0; i<(int)lstEnvVars->GetCount(); ++i)
  {
    if (lstEnvVars->IsChecked(i))
    {
      nsEnvVars::EnvVariableListClientData *data;
      data = static_cast<nsEnvVars::EnvVariableListClientData*>(lstEnvVars->GetClientObject(i));
      if (!data->key.IsEmpty())
      {
        if (!nsEnvVars::EnvvarApply(data->key, data->value))
        {
          lstEnvVars->Check(i, false); // Unset on UI to mark it's NOT set

          // Setting envvar failed. Remember this key to report later.
          if (envsNotSet.IsEmpty())
            envsNotSet << data->key;
          else
            envsNotSet << _T(", ") << data->key;
        }
      }
    }
  }// for

  if (!envsNotSet.IsEmpty())
  {
    wxString msg;
    msg.Printf( _("There was an error setting the following environment variables:\n%s"),
                envsNotSet.wx_str() );
    cbMessageBox(msg, _("Error"), wxOK | wxCENTRE | wxICON_ERROR);
  }
}// OnSetEnvVarsClick

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

bool EnvVarsConfigDlg::VerifySetUnique(const wxChoice* choSet, wxString set)
{
  for (int i=0; i<(int)choSet->GetCount(); ++i)
  {
    if (set.MakeLower().IsSameAs(choSet->GetString(i).MakeLower()))
    {
      cbMessageBox(_("This set already exists."), _("Error"),
                   wxOK | wxCENTRE | wxICON_EXCLAMATION);
      return false;
    }
  }

  return true;
}// VerifySetUnique
