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
  #include <wx/menu.h>
  #include <wx/toolbar.h>

  #include <tinyxml/tinyxml.h>

  #include "cbproject.h"
  #include "globals.h"
  #include "manager.h"
  #include "configmanager.h"
  #include "logmanager.h"
#endif

#include "projectloader_hooks.h"

#include "envvars_common.h"
#include "envvars_cfgdlg.h"
#include "envvars_prjoptdlg.h"
#include "envvars.h"

// Uncomment this for tracing of method calls in C::B's DebugLog:
//#define TRACE_ENVVARS

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

// Register the plugin
namespace
{
  PluginRegistrant<EnvVars> reg(_T("EnvVars"));
};

BEGIN_EVENT_TABLE(EnvVars, cbPlugin)
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
  Manager::Get()->GetLogManager()->DebugLog(F(_T("SetProjectEnvvarSet")));
#endif

  m_ProjectSets[project] = envvar_set;
  EV_DBGLOG(_T("EnvVars: Discarding envvars set '")+nsEnvVars::GetActiveSetName()+_T("'."));
  nsEnvVars::EnvvarSetDiscard(); // remove currently active envvars
  if (envvar_set.IsEmpty())
    EV_DBGLOG(_T("EnvVars: Setting up default envvars set."));
  else
    EV_DBGLOG(_T("EnvVars: Setting up envvars set '")+envvar_set+_T("' for activated project."));
  nsEnvVars::EnvvarSetApply(envvar_set); // apply currently active envvar set for wxEmptyString
}// SetProjectEnvvarSet

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVars::OnProjectLoadingHook(cbProject* project, TiXmlElement* elem,
                                   bool loading)
{
#if TRACE_ENVVARS
  Manager::Get()->GetLogManager()->DebugLog(F(_T("OnProjectLoadingHook")));
#endif

  if (loading)
  {
    TiXmlElement* node = elem->FirstChildElement("envvars");
    if (node)
    {
      m_ProjectSets[project] = cbC2U(node->Attribute("set"));
      if (m_ProjectSets[project].IsEmpty()) // no envvar set to apply setup
        return;

      if (!nsEnvVars::EnvvarSetExists(m_ProjectSets[project]))
        EnvvarSetWarning(m_ProjectSets[project]);
    }
  }
  else
  {
    // Hook called when saving project file.

    // since rev4332, the project keeps a copy of the <Extensions> element
    // and re-uses it when saving the project (so to avoid losing entries in it
    // if plugins that use that element are not loaded atm).
    // so, instead of blindly inserting the element, we must first check it's
    // not already there (and if it is, clear its contents)
    TiXmlElement* node = elem->FirstChildElement("envvars");
    if (!node)
      node = elem->InsertEndChild(TiXmlElement("envvars"))->ToElement();
    node->Clear();
    if (!m_ProjectSets[project].IsEmpty())
      node->SetAttribute("set", cbU2C(m_ProjectSets[project]));
  }
}// OnProjectLoadingHook

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVars::OnProjectActivated(CodeBlocksEvent& event)
{
#if TRACE_ENVVARS
  Manager::Get()->GetLogManager()->DebugLog(F(_T("OnProjectActivated")));
#endif

  if (IsAttached())
  {
    wxString prj_envvar_set = m_ProjectSets[event.GetProject()];
    if (prj_envvar_set.IsEmpty())  // There is no envvar set to apply...
      // Apply default envvar set (but only, if not already active)
      nsEnvVars::EnvvarSetApply(wxEmptyString, false);
    else                           // ...there is an envvar set setup to apply.
    {
      if (nsEnvVars::EnvvarSetExists(prj_envvar_set))
      {
        EV_DBGLOG(_T("EnvVars: Discarding envvars set '")
                 +nsEnvVars::GetActiveSetName()+_T("'."));
        nsEnvVars::EnvvarSetDiscard(); // Remove currently active envvars
        if (prj_envvar_set.IsEmpty())
          EV_DBGLOG(_T("EnvVars: Setting up default envvars set."));
        else
          EV_DBGLOG(_T("EnvVars: Setting up envvars set '")+prj_envvar_set
                   +_T("' for activated project."));
        // Apply envvar set always (as the old one has been discarded above)
        nsEnvVars::EnvvarSetApply(prj_envvar_set);
      }
      else
        EnvvarSetWarning(prj_envvar_set);
    }
  }

  event.Skip(); // propagate the event to other listeners
}// OnProjectActivated

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVars::OnProjectClosed(CodeBlocksEvent& event)
{
#if TRACE_ENVVARS
  Manager::Get()->GetLogManager()->DebugLog(F(_T("OnProjectClosed")));
#endif

  wxString prj_envvar_set = wxEmptyString;

  if (IsAttached())
  {
    prj_envvar_set = m_ProjectSets[event.GetProject()];

    // If there is an envvar set connected to this project...
    if (!prj_envvar_set.IsEmpty())
      // ...make sure it's being discarded
      nsEnvVars::EnvvarSetDiscard(prj_envvar_set);

    m_ProjectSets.erase(event.GetProject());
  }

  // Apply default envvar set (but only, if not already active)
  nsEnvVars::EnvvarSetApply(wxEmptyString,
                            prj_envvar_set.IsEmpty() ? false : true);

  event.Skip(); // Propagate the event to other listeners
}// OnProjectClosed

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVars::OnAttach()
{
#if TRACE_ENVVARS
  Manager::Get()->GetLogManager()->DebugLog(F(_T("OnAttach")));
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

  // register event sink
  Manager::Get()->RegisterEventSink(cbEVT_PROJECT_ACTIVATE, new cbEventFunctor<EnvVars, CodeBlocksEvent>(this, &EnvVars::OnProjectActivated));
  Manager::Get()->RegisterEventSink(cbEVT_PROJECT_CLOSE,    new cbEventFunctor<EnvVars, CodeBlocksEvent>(this, &EnvVars::OnProjectClosed));
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
  Manager::Get()->GetLogManager()->DebugLog(F(_T("Configure")));
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
  Manager::Get()->GetLogManager()->DebugLog(F(_T("EnvvarSetWarning")));
#endif

  wxString warning_msg;
  warning_msg.Printf(_("Warning: The project contained a reference to an envvar set\n"
                       "('%s') that could not be found."), envvar_set.c_str());
  cbMessageBox(warning_msg, _("EnvVars Plugin Warning"), wxICON_WARNING);
}// EnvvarSetWarning
