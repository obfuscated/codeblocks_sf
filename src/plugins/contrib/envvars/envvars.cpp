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
  #include "scriptingmanager.h"
#endif

#include "projectloader_hooks.h"

#include "sc_utils.h"
#include "sc_typeinfo_all.h"

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

    // If the set is empty we want to remove the node, else we set it.
    if (envvar_set.empty())
    {
        if (node)
            elem->RemoveChild(node);
    }
    else
    {
        if (!node)
            node = elem->InsertEndChild(TiXmlElement("envvars"))->ToElement();
        node->SetAttribute("set", cbU2C(envvar_set));
    }
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

namespace ScriptBindings
{
SQInteger GetEnvvarSetNames(HSQUIRRELVM v)
{
    // env table
    ExtractParams1<SkipParam> extractor(v);
    if (!extractor.Process("EnvvarGetEnvvarSetNames"))
        return extractor.ErrorMessage();

    return ConstructAndReturnInstance(v, nsEnvVars::GetEnvvarSetNames());
}

SQInteger GetActiveSetName(HSQUIRRELVM v)
{
    // env table
    ExtractParams1<SkipParam> extractor(v);
    if (!extractor.Process("EnvvarGetActiveSetName"))
        return extractor.ErrorMessage();

    return ConstructAndReturnInstance(v, nsEnvVars::GetActiveSetName());
}

SQInteger GetEnvvarsBySetPath(HSQUIRRELVM v)
{
    // env table, set_path
    ExtractParams2<SkipParam, const wxString *> extractor(v);
    if (!extractor.Process("EnvvarGetEnvvarsBySetPath"))
        return extractor.ErrorMessage();

    return ConstructAndReturnInstance(v, nsEnvVars::GetEnvvarsBySetPath(*extractor.p1));
}

SQInteger EnvvarSetExists(HSQUIRRELVM v)
{
    // env table, set_path
    ExtractParams2<SkipParam, const wxString *> extractor(v);
    if (!extractor.Process("EnvvarSetExists"))
        return extractor.ErrorMessage();

    sq_pushbool(v, nsEnvVars::EnvvarSetExists(*extractor.p1));
    return 1;
}

SQInteger EnvvarSetApply(HSQUIRRELVM v)
{
    // env table, set_name, even_if_active
    ExtractParams3<SkipParam, const wxString *, bool> extractor(v);
    if (!extractor.Process("EnvvarSetApply"))
        return extractor.ErrorMessage();

    nsEnvVars::EnvvarSetApply(*extractor.p1, extractor.p2);
    return 0;
}

SQInteger EnvvarSetDiscard(HSQUIRRELVM v)
{
    // env table, set_name
    ExtractParams2<SkipParam, const wxString *> extractor(v);
    if (!extractor.Process("EnvvarSetDiscard"))
        return extractor.ErrorMessage();
    nsEnvVars::EnvvarSetDiscard(*extractor.p1);
    return 0;
}

SQInteger EnvvarApply(HSQUIRRELVM v)
{
    // env table, key, value
    ExtractParams3<SkipParam, const wxString *, const wxString *> extractor(v);
    if (!extractor.Process("EnvvarApply"))
        return extractor.ErrorMessage();

    sq_pushbool(v, nsEnvVars::EnvvarApply(*extractor.p1, *extractor.p2));
    return 1;
}

SQInteger EnvvarDiscard(HSQUIRRELVM v)
{
    // env table, key
    ExtractParams2<SkipParam, const wxString *> extractor(v);
    if (!extractor.Process("EnvvarDiscard"))
        return extractor.ErrorMessage();

    sq_pushbool(v, nsEnvVars::EnvvarDiscard(*extractor.p1));
    return 1;
}
} // namespace ScriptBindings

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

  {
    // Register scripting
    ScriptingManager *scriptMgr = Manager::Get()->GetScriptingManager();
    HSQUIRRELVM vm = scriptMgr->GetVM();
    ScriptBindings::PreserveTop preserveTop(vm);

    sq_pushroottable(vm);

    ScriptBindings::BindMethod(vm, _SC("EnvvarGetEnvvarSetNames"), ScriptBindings::GetEnvvarSetNames, nullptr);
    ScriptBindings::BindMethod(vm, _SC("EnvvarGetActiveSetName"), ScriptBindings::GetActiveSetName, nullptr);
    ScriptBindings::BindMethod(vm, _SC("EnvvarGetEnvvarsBySetPath"), ScriptBindings::GetEnvvarsBySetPath, nullptr);
    ScriptBindings::BindMethod(vm, _SC("EnvvarSetExists"), ScriptBindings::EnvvarSetExists, nullptr);
    ScriptBindings::BindMethod(vm, _SC("EnvvarSetApply"), ScriptBindings::EnvvarSetApply, nullptr);
    ScriptBindings::BindMethod(vm, _SC("EnvvarSetDiscard"), ScriptBindings::EnvvarSetDiscard, nullptr);
    ScriptBindings::BindMethod(vm, _SC("EnvvarApply"), ScriptBindings::EnvvarApply, nullptr);
    ScriptBindings::BindMethod(vm, _SC("EnvvarDiscard"), ScriptBindings::EnvvarDiscard, nullptr);

    sq_poptop(vm); // Pop root table.
  }
}// OnAttach

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVars::OnRelease(bool /*appShutDown*/)
{
  // Unregister scripting
  HSQUIRRELVM v = Manager::Get()->GetScriptingManager()->GetVM();
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
