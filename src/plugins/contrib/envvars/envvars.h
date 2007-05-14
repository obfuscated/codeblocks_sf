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

#ifndef ENVVARS_H
#define ENVVARS_H

#include <wx/string.h>
#include <wx/window.h>

class wxMenuBar;
class wxToolBar;

#include "cbplugin.h" // the base class we 're inheriting
#include "configurationpanel.h"

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

class EnvVars : public cbPlugin
{
public:
           EnvVars();
  virtual ~EnvVars()
  { };

private:
  int     Configure();

  int     GetConfigurationPriority() const
  { return 50; }

  int     GetConfigurationGroup() const
  { return  cgContribPlugin; }

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

#endif // ENVVARS_H
