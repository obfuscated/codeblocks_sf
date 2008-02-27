/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef ENVVARS_H
#define ENVVARS_H

#include <wx/string.h>
#include <wx/window.h>

#include <map>

class wxMenu;
class wxMenuBar;
class wxToolBar;

class TiXmlElement;

class cbProject;

#include "cbplugin.h" // the base class we 're inheriting
#include "configurationpanel.h"
#include "sdk_events.h"

typedef std::map<cbProject*, wxString> ProjectEnvvarMap;

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

class EnvVars : public cbPlugin
{
public:
  /// Ctor
           EnvVars();
  /// Dtor
  virtual ~EnvVars();

  wxString GetProjectEnvvarSet(cbProject* project)
  { return m_ProjectSets[project]; }

  void     SetProjectEnvvarSet(cbProject* project, const wxString& envvar_set);

protected:
  /// fires when a project is being loaded / saved
  void     OnProjectLoadingHook(cbProject* project, TiXmlElement* elem,
                                bool loading);

  /// fires when a project is being activated
  void     OnProjectActivated(CodeBlocksEvent& event);

  /// fires when a project is being closed
  void     OnProjectClosed(CodeBlocksEvent& event);

private:
  /// fires when the plugin is attached to the application:
  void     OnAttach();

  /// fires when the plugin is released from the application:
  void     OnRelease(bool appShutDown);

  /// configures the plugin
  int      Configure();

  /// returns the configuration priority (when to setup the plugin)
  int      GetConfigurationPriority() const
  { return 50; }

  /// returns the configuration group this plugin belongs to
  int      GetConfigurationGroup() const
  { return  cgContribPlugin; }

  /// returns the configuration panel for the plugin (if any)
  virtual  cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);

  /// returns the project configuration panel for the plugin (if any)
  virtual  cbConfigurationPanel* GetProjectConfigurationPanel(wxWindow* parent,
                                                              cbProject* project);

  /// hooks into the menu build process to allow the plugin to add menu entries
  void     BuildMenu(wxMenuBar* menuBar)
  { return; }

  /// hooks into the module menu build process to allow the plugin to add menu entries
  void     BuildModuleMenu(const ModuleType type, wxMenu* menu,
                           const FileTreeData* data = 0)
  { return; }

  /// hooks into the toolbar build process to allow the plugin to add an own toolbar
  bool     BuildToolBar(wxToolBar* toolBar)
  { return false; }

  /// issues a warning if an activated project has a reference to an envvar set that does not exist
  void     EnvvarSetWarning(const wxString& envvar_set);

  int              m_EnvVarHookID; //!< project loader hook ID
  ProjectEnvvarMap m_ProjectSets;  //!< preferred envvar sets for all projects

  DECLARE_EVENT_TABLE()
};

#endif // ENVVARS_H
