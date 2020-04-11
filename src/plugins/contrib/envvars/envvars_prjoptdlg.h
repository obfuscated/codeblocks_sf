/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

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
  EnvVarsProjectOptionsDlg(wxWindow*  parent, cbProject* project);
  /// Dtor
  ~EnvVarsProjectOptionsDlg() override;

  /// returns the title of the plugin configuration panel
  wxString GetTitle() const override
  { return _("EnvVars options"); }

  /// returns the title of the plugin's bitmap to use for settings
  wxString GetBitmapBaseName() const override
  { return _T("generic-plugin"); }

protected:
  /// Fires if the UI is being updated (wx event)
  void OnUpdateUI(wxUpdateUIEvent& event);

private:
  /// Fires if the "apply" button is pressed inside project settings
  void OnApply() override;

  /// Fires if the "cancel" button is pressed inside project settings
  void OnCancel() override {}

  EnvVars*   m_pPlugin;  //!< pointer to the EnvVars plugin (the parent)
  cbProject* m_pProject; //!< pointer to the currently active C::B project

  DECLARE_EVENT_TABLE()
};

#endif // ENVVARSPROJECTOPTIONSDLG_H
