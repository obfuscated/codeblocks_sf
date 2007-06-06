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
  #include <wx/menu.h>
  #include <wx/toolbar.h>

  #include <tinyxml/tinyxml.h>

  #include "cbproject.h"
  #include "globals.h"
  #include "manager.h"
  #include "configmanager.h"
  #include "messagemanager.h"
#endif

#include "projectloader_hooks.h"

#include "envvars_common.h"
#include "envvars_cfgdlg.h"
#include "envvars_prjoptdlg.h"
#include "envvars.h"

// Uncomment this for tracing of method calls in C::B's DebugLog:
//#define TRACE_ENVVARS

// TODO (Morten#5#): remove envvars of currently active set upon ProjectActivated (is that a good way?)
// TODO (Morten#5#): apply default envvar set (is still stored in /active_set from config) on ProjectClosed

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

// Register the plugin
namespace
{
  PluginRegistrant<EnvVars> reg(_T("EnvVars"));
};

BEGIN_EVENT_TABLE(EnvVars, cbPlugin)
  EVT_PROJECT_ACTIVATE(EnvVars::OnProjectActivated)
END_EVENT_TABLE()

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

EnvVars::EnvVars()
{
  // hook to project loading procedure
  ProjectLoaderHooks::HookFunctorBase* envvar_hook =
    new ProjectLoaderHooks::HookFunctor<EnvVars>(this, &EnvVars::OnProjectLoadingHook);

  m_EnvVarHookID = ProjectLoaderHooks::RegisterHook(envvar_hook);
}// EnvVars

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

EnvVars::~EnvVars()
{
  ProjectLoaderHooks::UnregisterHook(m_EnvVarHookID, true);
}// ~EnvVars

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVars::SetProjectEnvvarSet(cbProject* project, const wxString& envvar_set)
{
#if TRACE_ENVVARS
  DBGLOG(_T("SetProjectEnvvarSet"));
#endif

  m_ProjectSets[project] = envvar_set;
  nsEnvVars::EnvvarSetDiscard();
  nsEnvVars::EnvvarSetApply(envvar_set); // apply currently active envvar set for wxEmptyString
}// SetProjectEnvvarSet

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVars::OnProjectLoadingHook(cbProject* project, TiXmlElement* elem,
                                   bool loading)
{
#if TRACE_ENVVARS
  DBGLOG(_T("OnProjectLoadingHook"));
#endif

  if (loading)
  {
    TiXmlElement* CCConf = elem->FirstChildElement("envvars");
    if (CCConf)
    {
      m_ProjectSets[project] = cbC2U(CCConf->Attribute("set"));
      if (m_ProjectSets[project].IsEmpty()) // no envvar set to apply setup
        return;

      if (!nsEnvVars::EnvvarSetExists(m_ProjectSets[project]))
        EnvvarSetWarning(m_ProjectSets[project]);
    }
  }
  else
  {
    // Hook called when saving project file.
    TiXmlElement* node = elem->InsertEndChild(TiXmlElement("envvars"))->ToElement();
    if (!m_ProjectSets[project].IsEmpty())
      node->SetAttribute("set",  cbU2C(m_ProjectSets[project]));
  }
}// OnProjectLoadingHook

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVars::OnProjectActivated(CodeBlocksEvent& event)
{
#if TRACE_ENVVARS
  DBGLOG(_T("OnProjectActivated"));
#endif

  if (IsAttached())
  {
    wxString envvar_set = m_ProjectSets[event.GetProject()];
    if (envvar_set.IsEmpty()) // there is no envvar set to apply
      nsEnvVars::EnvvarSetApply(); // apply currently active envvar set
    else                      // there is an envvar set setup to apply
    {
      if (nsEnvVars::EnvvarSetExists(envvar_set))
      {
        DBGLOG(_T("EnvVars: Setting up envvars set '")+envvar_set+_T("' for activated project."));
        nsEnvVars::EnvvarSetDiscard(); // remove currently active envvars
        nsEnvVars::EnvvarSetApply(envvar_set);
      }
      else
        EnvvarSetWarning(envvar_set);
    }
  }

  event.Skip(); // propagate the event to other listeners
}// OnProjectActivated

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVars::OnProjectClosed(CodeBlocksEvent& event)
{
#if TRACE_ENVVARS
  DBGLOG(_T("OnProjectClosed"));
#endif

  if (IsAttached())
    m_ProjectSets.erase(event.GetProject());

  nsEnvVars::EnvvarSetApply(); // apply currently active envvar set

  event.Skip(); // propagate the event to other listeners
}// OnProjectClosed

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVars::OnAttach()
{
#if TRACE_ENVVARS
  DBGLOG(_T("OnAttach"));
#endif

  if(!Manager::LoadResource(_T("envvars.zip")))
  {
    NotifyMissingFile(_T("envvars.zip"));
  }

  // load and apply configuration (to application only)
  ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("envvars"));
  if (!cfg)
    return;

  nsEnvVars::EnvvarSetApply(); // will apply the currently active envvar set
}// OnAttach

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVars::OnRelease(bool appShutDown)
{
  // Nothing to do (so far...)
}// OnRelease

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

int EnvVars::Configure()
{
#if TRACE_ENVVARS
  DBGLOG(_T("Configure"));
#endif

  // Nothing to do (so far...) -> just return success
  return 0;
}// Configure

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

cbConfigurationPanel* EnvVars::GetConfigurationPanel(wxWindow* parent)
{
  EnvVarsConfigDlg* dlg = new EnvVarsConfigDlg(parent, this);
  // deleted by the caller

  return dlg;
}// GetConfigurationPanel

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

cbConfigurationPanel* EnvVars::GetProjectConfigurationPanel(wxWindow* parent,
                                                            cbProject* project)
{
  EnvVarsProjectOptionsDlg* dlg = new EnvVarsProjectOptionsDlg(parent, this, project);
  // deleted by the caller

  return dlg;
}// GetProjectConfigurationPanel

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVars::EnvvarSetWarning(const wxString& envvar_set)
{
#if TRACE_ENVVARS
  DBGLOG(_T("EnvvarSetWarning"));
#endif

  wxString warning_msg;
  warning_msg.Printf(_("Warning: The project contained a reference to an envvar set\n"
                       "('%s') that could not be found."), envvar_set.c_str());
  cbMessageBox(warning_msg, _("EnvVars Plugin Warning"), wxICON_WARNING);
}// EnvvarSetWarning
