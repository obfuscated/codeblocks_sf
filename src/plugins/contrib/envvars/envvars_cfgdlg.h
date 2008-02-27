/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef ENVVARS_CFGDLG_H
#define ENVVARS_CFGDLG_H

#include <wx/event.h>
#include <wx/string.h>
#include <wx/window.h>

#include "configurationpanel.h"

class EnvVars;
class wxChoice;

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

class EnvVarsConfigDlg : public cbConfigurationPanel
{
public:
  /// Ctor
           EnvVarsConfigDlg(wxWindow* parent, EnvVars* plugin);
  /// Dtor
  virtual ~EnvVarsConfigDlg()
  { };

  /// returns the title of the plugin configuration panel
  virtual wxString GetTitle() const
  { return _T("Environment variables"); }

  /// returns the title of the plugin's bitmap to use for configuration dialog
  virtual wxString GetBitmapBaseName() const
  { return _T("envvars"); }

protected:
  /// Fires if the UI is being updated (wx event)
  void OnUpdateUI         (wxUpdateUIEvent& event);

private:
  /// Fires if the "apply" button is pressed inside C::B settings
  virtual void OnApply()
  { SaveSettings(); }

  /// Fires if the "cancel" button is pressed inside C::B settings
  virtual void OnCancel()
  { ; }

  /// Load all settings (envvar sets) from global C::B config
  void LoadSettings();
  /// Save all settings (envvar sets) to global C::B config
  void SaveSettings();
  /// Save settings (of specific active envvar set) to global C::B config
  void SaveSettingsActiveSet(wxString active_set);

  /// Fires when a (new) envvar set is selected
  void OnSetClick         (wxCommandEvent&  event);
  /// Fires when the button to create an envvar set is pressed
  void OnCreateSetClick   (wxCommandEvent&  event);
  /// Fires when the button to clone an envvar set is pressed
  void OnCloneSetClick    (wxCommandEvent&  event);
  /// Fires when the button to remove an envvar set is pressed
  void OnRemoveSetClick   (wxCommandEvent&  event);

  /// Fires when the checkbox to toggle an envvar is changed
  void OnToggleEnvVarClick(wxCommandEvent&  event);
  /// Fires when the button to add an envvar is pressed
  void OnAddEnvVarClick   (wxCommandEvent&  event);
  /// Fires when the button to edit an envvar is pressed
  void OnEditEnvVarClick  (wxCommandEvent&  event);
  /// Fires when the button to delete an envvar is pressed
  void OnDeleteEnvVarClick(wxCommandEvent&  event);
  /// Fires when the button to clear all envvars is pressed
  void OnClearEnvVarsClick(wxCommandEvent&  event);
  /// Fires when the button to set all envvars is pressed
  void OnSetEnvVarsClick  (wxCommandEvent&  event);

  /// Verifies that an envvar set is unique
  bool VerifySetUnique(const wxChoice* choSet, wxString set);

  EnvVars* m_pPlugin; //!< pointer to the EnvVars plugin (the parent)

  DECLARE_EVENT_TABLE()
};

#endif // ENVVARS_CFGDLG_H
