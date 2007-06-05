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

#ifndef ENVVARSPROJECTOPTIONSDLG_H
#define ENVVARSPROJECTOPTIONSDLG_H

#include <wx/event.h>
#include <wx/string.h>

#include "configurationpanel.h"

class EnvVars;
class cbProject;

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

class EnvVarsProjectOptionsDlg : public cbConfigurationPanel
{
public:
  /// Ctor
           EnvVarsProjectOptionsDlg(wxWindow*  parent, EnvVars* plugin,
                                    cbProject* project);
  /// Dtor
  virtual ~EnvVarsProjectOptionsDlg();

  /// returns the title of the plugin configuration panel
  virtual wxString GetTitle() const
  { return _("EnvVars options"); }

  /// returns the title of the plugin's bitmap to use for settings
  virtual wxString GetBitmapBaseName() const
  { return _T("generic-plugin"); }

protected:
  /// Fires if the UI is being updated (wx event)
  void OnUpdateUI(wxUpdateUIEvent& event);

private:
  /// Fires if the "apply" button is pressed inside project settings
  virtual void OnApply();

  /// Fires if the "cancel" button is pressed inside project settings
  virtual void OnCancel()
  { ; }

  EnvVars*   m_pPlugin;  //!< pointer to the EnvVars plugin (the parent)
  cbProject* m_pProject; //!< pointer to the currently active C::B project

  DECLARE_EVENT_TABLE()
};

#endif // ENVVARSPROJECTOPTIONSDLG_H
