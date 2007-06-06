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
  #include <wx/xrc/xmlres.h>
  #include <wx/checkbox.h>
  #include <wx/choice.h>

  #include "cbproject.h"
#endif

#include "envvars.h"
#include "envvars_common.h"
#include "envvars_prjoptdlg.h"

// Uncomment this for tracing of method calls in C::B's DebugLog:
//#define TRACE_ENVVARS

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

BEGIN_EVENT_TABLE(EnvVarsProjectOptionsDlg, wxPanel)
  EVT_UPDATE_UI(-1, EnvVarsProjectOptionsDlg::OnUpdateUI)
END_EVENT_TABLE()

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

EnvVarsProjectOptionsDlg::EnvVarsProjectOptionsDlg(wxWindow* parent, EnvVars* plugin, cbProject* project) :
  m_pPlugin(plugin),
  m_pProject(project)
{
  wxXmlResource::Get()->LoadPanel(this, parent, _T("pnlProjectEnvVarsOptions"));

  wxChoice* choice_control = XRCCTRL(*this, "choEnvvarSets", wxChoice);
  if (!choice_control) return;

  choice_control->Clear();
  wxArrayString envvar_sets = nsEnvVars::GetEnvvarSetNames();
  for (size_t i = 0; i < envvar_sets.GetCount(); ++i)
    choice_control->Append(envvar_sets[i]);

  wxCheckBox* checkbox_control = XRCCTRL(*this, "chkEnvvarSet", wxCheckBox);
  if (checkbox_control && choice_control->GetCount())
  {
    wxString envvar_set = m_pPlugin->GetProjectEnvvarSet(project);
    if (envvar_set.IsEmpty())
    {
      checkbox_control->SetValue(false);
      choice_control->SetSelection(0);
      choice_control->Disable();
    }
    else
    {
      checkbox_control->SetValue(true);
      choice_control->SetStringSelection(envvar_set);
      choice_control->Enable();
    }
  }
}// EnvVarsProjectOptionsDlg

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

EnvVarsProjectOptionsDlg::~EnvVarsProjectOptionsDlg()
{
}// ~EnvVarsProjectOptionsDlg

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVarsProjectOptionsDlg::OnUpdateUI(wxUpdateUIEvent& event)
{
#if TRACE_ENVVARS
  DBGLOG(_T("OnUpdateUI"));
#endif

  wxCheckBox* checkbox_control = XRCCTRL(*this, "chkEnvvarSet", wxCheckBox);
  if (checkbox_control)
  {
    wxChoice* choice_control = XRCCTRL(*this, "choEnvvarSets", wxChoice);
    if (choice_control)
      choice_control->Enable(checkbox_control->IsChecked());
  }
  event.Skip();
}// OnUpdateUI

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void EnvVarsProjectOptionsDlg::OnApply()
{
#if TRACE_ENVVARS
  DBGLOG(_T("OnApply"));
#endif

  wxCheckBox* checkbox_control = XRCCTRL(*this, "chkEnvvarSet", wxCheckBox);
  if (checkbox_control && checkbox_control->IsChecked())
  {
    wxChoice* choice_control = XRCCTRL(*this, "choEnvvarSets", wxChoice);
    if (choice_control)
    {
      wxString envvar_set = choice_control->GetStringSelection();
      if (!envvar_set.IsEmpty())
        m_pPlugin->SetProjectEnvvarSet(m_pProject, envvar_set);
    }
  }
  else
    m_pPlugin->SetProjectEnvvarSet(m_pProject, wxEmptyString);
}// OnApply
