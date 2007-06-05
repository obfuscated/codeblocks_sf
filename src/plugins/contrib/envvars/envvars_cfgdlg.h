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

#ifndef ENVVARS_CFGDLG_H
#define ENVVARS_CFGDLG_H

#include <wx/event.h>
#include <wx/string.h>
#include <wx/window.h>

#include "configurationpanel.h"

class EnvVars;

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

  /// returns the title of the plugin's bitmap to use for settings
  virtual wxString GetBitmapBaseName() const
  { return _T("generic-plugin"); }

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

  void OnAddEnvVarClick   (wxCommandEvent&  event);
  void OnEditEnvVarClick  (wxCommandEvent&  event);
  void OnToggleEnvVarClick(wxCommandEvent&  event);
  void OnSetClick         (wxCommandEvent&  event);
  void OnCreateSetClick   (wxCommandEvent&  event);
  void OnRemoveSetClick   (wxCommandEvent&  event);
  void OnDeleteEnvVarClick(wxCommandEvent&  event);
  void OnClearEnvVarsClick(wxCommandEvent&  event);
  void OnSetEnvVarsClick  (wxCommandEvent&  event);

  EnvVars* m_pPlugin; //!< pointer to the EnvVars plugin (the parent)

  DECLARE_EVENT_TABLE()
};

#endif // ENVVARS_CFGDLG_H
