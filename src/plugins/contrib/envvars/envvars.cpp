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

  #include "manager.h"
  #include "configmanager.h"
  #include "messagemanager.h"
#endif

#include "envvars_common.h"
#include "envvars_cfgdlg.h"
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
  //ctor
}// EnvVars

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVars::OnAttach()
{
#if TRACE_ENVVARS
	if (Manager::Get() && Manager::Get()->GetMessageManager());
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

  // Read the currently active envvar set
  wxString active_set = cfg->Read(_T("/active_set"));
  if (active_set.IsEmpty())
    active_set = nsEnvVars::EnvVarsDefault;

  // Show currently activated set in debug log (for reference)
  wxString active_set_path = nsEnvVars::GetSetPathByName(active_set);
  DBGLOG(_T("Active envvar set is '%s', config path '%s'."),
    active_set.c_str(), active_set_path.c_str());

  // Read and show all envvars from currently active set in listbox
  wxArrayString vars     = nsEnvVars::GetEnvvarsBySetPath(active_set_path);
  size_t envvars_total   = vars.GetCount();
  size_t envvars_applied = 0;
  for (unsigned int i=0; i<envvars_total; ++i)
  {
    // Format: [checked?]|[key]|[value]
    wxArrayString var_array = nsEnvVars::EnvvarStringTokeniser(vars[i]);
    if (nsEnvVars::EnvvarApply(var_array))
      envvars_applied++;
    else
      DBGLOG(_("Invalid envvar in '%s' at position #%d."),
        active_set_path.c_str(), i);
	}// for

	if (envvars_total>0)
	{
    DBGLOG(_("%d/%d envvars applied within C::B focus."),
      envvars_applied, envvars_total);
  }
}// OnAttach

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVars::OnRelease(bool appShutDown)
{
  // Nothing to do (so far...)
}// OnRelease

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

cbConfigurationPanel* EnvVars::GetConfigurationPanel(wxWindow* parent)
{
  EnvVarsConfigDlg* dlg = new EnvVarsConfigDlg(parent, this);
  // deleted by the caller

  return dlg;
}// GetConfigurationPanel

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

int EnvVars::Configure()
{
#if TRACE_ENVVARS
	if (Manager::Get() && Manager::Get()->GetMessageManager());
    DBGLOG(_T("Configure"));
#endif

  // Nothing to do (so far...) -> just return success
  return 0;
}// Configure
