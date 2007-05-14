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
  EnvVars* plugin;

public:
           EnvVarsConfigDlg(wxWindow* parent, EnvVars* plug);
  virtual ~EnvVarsConfigDlg()
  { };

  virtual wxString GetTitle() const
  { return _T("Environment variables"); }
  virtual wxString GetBitmapBaseName() const
  { return _T("generic-plugin"); }

private:
  virtual void OnApply()
  { SaveSettings(); }
  virtual void OnCancel()
  { }

  void LoadSettings();
  void SaveSettings();
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

  void OnUpdateUI         (wxUpdateUIEvent& event);

  DECLARE_EVENT_TABLE()
};

#endif // ENVVARS_CFGDLG_H
