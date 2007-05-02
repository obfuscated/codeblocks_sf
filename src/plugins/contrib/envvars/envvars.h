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

#ifndef ENVVARS_H
#define ENVVARS_H

#include <wx/arrstr.h>
#include <wx/intl.h>
#include <wx/string.h>

#include "cbplugin.h" // the base class we 're inheriting
#include "configurationpanel.h"

class EnvVars : public cbPlugin
{
public:
           EnvVars();
  virtual ~EnvVars()
  { };

  int     Configure();

  int     GetConfigurationPriority() const
  { return 50; }

  int     GetConfigurationGroup() const
  { return  cgUnknown; }

  void    BuildMenu(wxMenuBar* menuBar)
  { return; }

  void    BuildModuleMenu(const ModuleType type, wxMenu* menu,
                          const FileTreeData* data = 0)
  { return; }

  bool    BuildToolBar(wxToolBar* toolBar)
  { return false; }

  // fires when the plugin is attached to the application:
  void    OnAttach();

  // fires when the plugin is released from the application:
  void    OnRelease(bool appShutDown);

  virtual cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);

  DECLARE_EVENT_TABLE()
};


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
  virtual void OnApply()
  { SaveSettings(); }
  virtual void OnCancel()
  { }

  void LoadSettings();
  void SaveSettings();

  void OnAddEnvVarClick   (wxCommandEvent&  event);
  void OnEditEnvVarClick  (wxCommandEvent&  event);
  void OnToggleEnvVarClick(wxCommandEvent&  event);
  void OnDeleteEnvVarClick(wxCommandEvent&  event);
  void OnClearEnvVarsClick(wxCommandEvent&  event);
  void OnSetEnvVarsClick  (wxCommandEvent&  event);

  void OnUpdateUI         (wxUpdateUIEvent& event);

  DECLARE_EVENT_TABLE()
};

#endif // ENVVARS_H
