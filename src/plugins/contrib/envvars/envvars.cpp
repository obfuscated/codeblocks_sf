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

  #include <tinyxml.h>

  #include "cbproject.h"
  #include "globals.h"
  #include "manager.h"
  #include "configmanager.h"
  #include "logmanager.h"
#endif

#include "projectloader_hooks.h"
#include <sqplus.h>
#include <sc_base_types.h>

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

wxString EnvVars::ParseProjectEnvvarSet(const cbProject *project)
{
    if (!project)
        return wxString();
    const TiXmlNode *extNode = project->GetExtensionsNode();
    if (!extNode)
        return wxString();
    const TiXmlElement* elem = extNode->ToElement();
    if (!elem)
        return wxString();
    const TiXmlElement* node = elem->FirstChildElement("envvars");
    if (!node)
        return wxString();

    wxString result = cbC2U(node->Attribute("set"));
    if (result.empty()) // no envvar set to apply setup
        return wxString();

    if (!nsEnvVars::EnvvarSetExists(result))
        EnvvarSetWarning(result);
    return result;
}

void EnvVars::SaveProjectEnvvarSet(cbProject &project, const wxString& envvar_set)
{
    TiXmlNode *extNode = project.GetExtensionsNode();
    if (!extNode)
        return;
    TiXmlElement* elem = extNode->ToElement();
    if (!elem)
        return;
    TiXmlElement* node = elem->FirstChildElement("envvars");
    if (!node)
    {
        if (!envvar_set.empty())
            return;
        node = elem->InsertEndChild(TiXmlElement("envvars"))->ToElement();
    }
    if (!envvar_set.empty())
        node->SetAttribute("set", cbU2C(envvar_set));
    else
        elem->RemoveChild(node);
}

void EnvVars::DoProjectActivate(cbProject* project)
{
    const wxString prj_envvar_set = ParseProjectEnvvarSet(project);
    if (prj_envvar_set.IsEmpty())  // There is no envvar set to apply...
      // Apply default envvar set (but only, if not already active)
      nsEnvVars::EnvvarSetApply(wxEmptyString, false);
    else                           // ...there is an envvar set setup to apply.
    {
      if (nsEnvVars::EnvvarSetExists(prj_envvar_set))
      {
        EV_DBGLOG(_T("EnvVars: Discarding envvars set '")
                 +nsEnvVars::GetActiveSetName()+_T("'."));
        nsEnvVars::EnvvarSetDiscard(wxEmptyString); // Remove currently active envvars
        if (prj_envvar_set.IsEmpty())
          EV_DBGLOG(_T("EnvVars: Setting up default envvars set."));
        else
          EV_DBGLOG(_T("EnvVars: Setting up envvars set '")+prj_envvar_set
                   +_T("' for activated project."));
        // Apply envvar set always (as the old one has been discarded above)
        nsEnvVars::EnvvarSetApply(prj_envvar_set, true);
      }
      else
        EnvvarSetWarning(prj_envvar_set);
    }
}

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVars::OnProjectActivated(CodeBlocksEvent& event)
{
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("OnProjectActivated")));
#endif

  if (IsAttached())
     DoProjectActivate(event.GetProject());

  event.Skip(); // propagate the event to other listeners
}// OnProjectActivated

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVars::OnProjectClosed(CodeBlocksEvent& event)
{
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("OnProjectClosed")));
#endif

  wxString prj_envvar_set = wxEmptyString;

  if (IsAttached())
  {
    prj_envvar_set = ParseProjectEnvvarSet(event.GetProject());

    // If there is an envvar set connected to this project...
    if (!prj_envvar_set.IsEmpty())
      // ...make sure it's being discarded
      nsEnvVars::EnvvarSetDiscard(prj_envvar_set);
  }

  // Apply default envvar set (but only, if not already active)
  nsEnvVars::EnvvarSetApply(wxEmptyString,
                            prj_envvar_set.IsEmpty() ? false : true);

  event.Skip(); // Propagate the event to other listeners
}// OnProjectClosed

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVars::OnAttach()
{
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("OnAttach")));
#endif

  if (!Manager::LoadResource(_T("envvars.zip")))
    NotifyMissingFile(_T("envvars.zip"));

  // load and apply configuration (to application only)
  ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("envvars"));
  if (!cfg)
    return;

  // will apply the currently active envvar set
  nsEnvVars::EnvvarSetApply(wxEmptyString, true);

  // register event sink
  Manager::Get()->RegisterEventSink(cbEVT_PROJECT_ACTIVATE, new cbEventFunctor<EnvVars, CodeBlocksEvent>(this, &EnvVars::OnProjectActivated));
  Manager::Get()->RegisterEventSink(cbEVT_PROJECT_CLOSE,    new cbEventFunctor<EnvVars, CodeBlocksEvent>(this, &EnvVars::OnProjectClosed));

  // Register scripting
  Manager::Get()->GetScriptingManager(); // make sure the VM is initialised
  if (SquirrelVM::GetVMPtr())
  {
    SqPlus::RegisterGlobal(&nsEnvVars::GetEnvvarSetNames,   "EnvvarGetEnvvarSetNames"  );
    SqPlus::RegisterGlobal(&nsEnvVars::GetEnvvarSetNames,   "EnvvarGetActiveSetName"   );
    SqPlus::RegisterGlobal(&nsEnvVars::GetEnvvarsBySetPath, "EnvVarGetEnvvarsBySetPath");
    SqPlus::RegisterGlobal(&nsEnvVars::EnvvarSetExists,     "EnvvarSetExists"          );
    SqPlus::RegisterGlobal(&nsEnvVars::EnvvarSetApply,      "EnvvarSetApply"           );
    SqPlus::RegisterGlobal(&nsEnvVars::EnvvarSetDiscard,    "EnvvarSetDiscard"         );
    SqPlus::RegisterGlobal(&nsEnvVars::EnvvarApply,         "EnvvarApply"              );
    SqPlus::RegisterGlobal(&nsEnvVars::EnvvarDiscard,       "EnvvarDiscard"            );
  }
}// OnAttach

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVars::OnRelease(bool /*appShutDown*/)
{
  // Unregister scripting
  Manager::Get()->GetScriptingManager(); // make sure the VM is initialised
  HSQUIRRELVM v = SquirrelVM::GetVMPtr();
  if (v)
  {
    // TODO (Morten#5#): Is that the correct way of un-registering? (Seems so weird...)
    sq_pushroottable(v);
    sq_pushstring(v, "EnvvarDiscard", -1);
    sq_deleteslot(v, -2, false);
    sq_poptop(v);

    sq_pushroottable(v);
    sq_pushstring(v, "EnvvarApply", -1);
    sq_deleteslot(v, -2, false);
    sq_poptop(v);

    sq_pushroottable(v);
    sq_pushstring(v, "EnvvarSetDiscard", -1);
    sq_deleteslot(v, -2, false);
    sq_poptop(v);

    sq_pushroottable(v);
    sq_pushstring(v, "EnvvarSetApply", -1);
    sq_deleteslot(v, -2, false);
    sq_poptop(v);

    sq_pushroottable(v);
    sq_pushstring(v, "EnvvarSetExists", -1);
    sq_deleteslot(v, -2, false);
    sq_poptop(v);

    sq_pushroottable(v);
    sq_pushstring(v, "EnvVarGetEnvvarsBySetPath", -1);
    sq_deleteslot(v, -2, false);
    sq_poptop(v);

    sq_pushroottable(v);
    sq_pushstring(v, "EnvvarGetActiveSetName", -1);
    sq_deleteslot(v, -2, false);
    sq_poptop(v);

    sq_pushroottable(v);
    sq_pushstring(v, "EnvvarGetEnvvarSetNames", -1);
    sq_deleteslot(v, -2, false);
    sq_poptop(v);
  }
}// OnRelease

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
  EnvVarsProjectOptionsDlg* dlg = new EnvVarsProjectOptionsDlg(parent, project);
  // deleted by the caller

  return dlg;
}// GetProjectConfigurationPanel

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVars::EnvvarSetWarning(const wxString& envvar_set)
{
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("EnvvarSetWarning")));
#endif

  wxString warning_msg;
  warning_msg.Printf(_("Warning: The project contained a reference to an envvar set\n"
                       "('%s') that could not be found."), envvar_set.wx_str());
  cbMessageBox(warning_msg, _("EnvVars Plugin Warning"), wxICON_WARNING);
}// EnvvarSetWarning
