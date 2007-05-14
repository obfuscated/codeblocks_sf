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
  #include <wx/checklst.h>

  #include "globals.h"
  #include "manager.h"
  #include "configmanager.h"
  #include "messagemanager.h"
#endif

#include "envvars_common.h"

// Uncomment this for tracing of method calls in C::B's DebugLog:
//#define TRACE_ENVVARS

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

const wxString nsEnvVars::EnvVarsSep     = _T("|");
const wxString nsEnvVars::EnvVarsDefault = _T("default");

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

wxArrayString nsEnvVars::EnvvarStringTokeniser(const wxString& str)
{
#if TRACE_ENVVARS
  DBGLOG(_T("EnvvarStringTokeniser"));
#endif
  // tokenise string like:
  // C:\Path;"D:\Other Path"

  wxArrayString out;

  wxString search = str;
  search.Trim(true).Trim(false);

  // trivial case: string is empty or consists of blanks only
  if (search.IsEmpty())
    return out;

  wxString token;
  bool     inside_quot = false;
  size_t   pos         = 0;
  while (pos < search.Length())
  {
    wxString current_char = search.GetChar(pos);

    // for e.g. /libpath:"C:\My Folder"
    if (current_char.CompareTo(_T("\""))==0) // equality
      inside_quot = !inside_quot;

    if ((current_char.CompareTo(nsEnvVars::EnvVarsSep)==0) && (!inside_quot))
    {
      if (!token.IsEmpty())
      {
        out.Add(token);
        token.Clear();
      }
    }
    else
      token.Append(current_char);

    pos++;
    // Append final token
    if ((pos==search.Length()) && (!inside_quot) && (!token.IsEmpty()))
      out.Add(token);
  }// while

  return out;
}// EnvvarStringTokeniser

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

wxArrayString nsEnvVars::GetEnvvarSetNames()
{
#if TRACE_ENVVARS
  DBGLOG(_T("GetEnvvarSetNames"));
#endif
  wxArrayString set_names;

  ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("envvars"));
  if (!cfg)
  {
    set_names.Add(nsEnvVars::EnvVarsDefault);
    return set_names;
  }

  // Read all envvar sets available
  wxArrayString sets     = cfg->EnumerateSubPaths(_T("/sets"));
  unsigned int  num_sets = sets.GetCount();
  DBGLOG(_T("Found %d envvar sets in config."), num_sets);

  if (num_sets==0)
    set_names.Add(nsEnvVars::EnvVarsDefault);
  else
  {
    for (unsigned int i=0; i<num_sets; ++i)
    {
      wxString set_name = sets[i];
      if (set_name.IsEmpty())
        set_name.Printf(_T("Set%d"), i);

      set_names.Add(set_name);
    }// for
  }// if

  return set_names;
}// GetEnvvarSetNames

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

wxString nsEnvVars::GetSetPathByName(const wxString& set_name, bool check_exists)
{
#if TRACE_ENVVARS
  DBGLOG(_T("GetSetPathByName"));
#endif

  wxString set_path = _T("/sets/")+nsEnvVars::EnvVarsDefault; // fall back solution

  ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("envvars"));
  if (!cfg || set_path.IsEmpty())
    return set_path;

  if (!check_exists)
    return _T("/sets/")+set_name;

  // Read all envvar sets available
  wxArrayString sets     = cfg->EnumerateSubPaths(_T("/sets"));
  unsigned int  num_sets = sets.GetCount();
  for (unsigned int i=0; i<num_sets; ++i)
  {
    if (set_name.IsSameAs(sets[i]))
    {
      set_path = (_T("/sets/")+set_name);
      break; // Early exit of for-loop
    }
  }

  return set_path;
}// GetSetPathByName

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

wxArrayString nsEnvVars::GetEnvvarsBySetPath(const wxString& set_path)
{
#if TRACE_ENVVARS
  DBGLOG(_T("GetEnvvarsBySetPath"));
#endif
  wxArrayString envvars;
  DBGLOG(_T("Searching for envvars in path '%s'."), set_path.c_str());

  ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("envvars"));
  if (!cfg || set_path.IsEmpty())
    return envvars;

  wxArrayString envvars_keys = cfg->EnumerateKeys(set_path);
  unsigned int  num_envvars  = envvars_keys.GetCount();
  for (unsigned int i=0; i<num_envvars; ++i)
  {
    wxString envvar = cfg->Read(set_path+_T("/")+envvars_keys[i]);
    if (!envvar.IsEmpty())
      envvars.Add(envvar);
    else
      DBGLOG(_T("Warning: empty envvar detected and skipped."));
  }
  DBGLOG(_T("Read %d/%d envvars in path '%s'."),
    envvars.GetCount(), num_envvars, set_path.c_str());

  return envvars;
}// GetEnvvarsBySetPath

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

bool nsEnvVars::EnvvarVeto(wxString &key, wxCheckListBox* lstEnvVars, int sel)
{
#if TRACE_ENVVARS
  DBGLOG(_T("EnvvarVeto"));
#endif

  if (wxGetEnv(key, NULL))
  {
    wxString warn_exist;
    warn_exist.Printf(_("Warning: Environment variable '%s' exists.\n"
                        "Continue with updating it's value?"), key.c_str());
    if (cbMessageBox(warn_exist, _("Confirmation"),
                     wxYES_NO | wxICON_QUESTION) == wxID_NO)
    {
      if (lstEnvVars && (sel>=0))
        lstEnvVars->Check(sel, false); // Unset to visualise it's NOT set
      return true; // User has vetoed the operation
    }
  }// if

  return false;
}// EnvvarVeto

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

bool nsEnvVars::EnvvarsClear(wxCheckListBox* lstEnvVars)
{
#if TRACE_ENVVARS
  DBGLOG(_T("EnvvarsClear"));
#endif

  wxString envsNotUnSet(wxEmptyString);

  // Unset all (checked) variables of lstEnvVars
  for (int i=0; i<lstEnvVars->GetCount(); ++i)
  {
    // Note: It's better not to just clear all because wxUnsetEnv would
    //       fail in case an environment variable is not set (not checked).
    if (lstEnvVars->IsChecked(i))
    {
      wxString key = lstEnvVars->GetString(i).BeforeFirst(_T('=')).Trim(true).Trim(false);
      if (!key.IsEmpty())
      {
        if (!nsEnvVars::EnvvarDiscard(key))
        {
          // Setting env.-variable failed. Remember this key to report later.
          if (envsNotUnSet.IsEmpty())
            envsNotUnSet << key;
          else
            envsNotUnSet << _T(", ") << key;
        }
      }
    }
  }// for

  lstEnvVars->Clear();

  if (!envsNotUnSet.IsEmpty())
  {
    wxString msg;
    msg.Printf( _("There was an error unsetting the following environment variables:\n%s"),
                envsNotUnSet.c_str() );
    cbMessageBox(msg, _("Error"), wxOK | wxCENTRE | wxICON_ERROR);
    return false;
  }

  return true;
}// EnvvarsClear

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

bool nsEnvVars::EnvvarDiscard(wxString &key)
{
#if TRACE_ENVVARS
  DBGLOG(_T("EnvvarDiscard"));
#endif

  if (!wxUnsetEnv(key))
  {
    Manager::Get()->GetMessageManager()->Log(
      _("Unsetting environment variable '%s' failed."), key.c_str());
    return false;
  }

  return true;
}// EnvvarDiscard

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

bool nsEnvVars::EnvvarApply(wxString &key, wxString& value, wxCheckListBox* lstEnvVars, int sel)
{
#if TRACE_ENVVARS
  DBGLOG(_T("EnvvarApply"));
#endif

  if (!wxSetEnv(key, value))
  {
    DBGLOG(_("Setting environment variable '%s' failed."), key.c_str());
    if (lstEnvVars && (sel>=0))
      lstEnvVars->Check(sel, false); // Unset to visualise it's NOT set
    return false;
  }

  return true;
}// EnvvarApply

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

bool nsEnvVars::EnvvarApply(wxArrayString &envvar, wxCheckListBox* lstEnvVars)
{
#if TRACE_ENVVARS
  DBGLOG(_T("EnvvarApply"));
#endif

  if (envvar.GetCount() == 3)
  {
    wxString check = envvar[0];
    wxString key   = envvar[1];
    wxString value = envvar[2];

    bool bCheck = check.Trim(true).Trim(false).IsSameAs(_T("1"))?true:false;
    key.Trim(true).Trim(false);
    value.Trim(true).Trim(false);

    int sel = 0;
    if (lstEnvVars)
    {
      sel = lstEnvVars->Append(key + _T(" = ") + value);
      lstEnvVars->Check(sel, bCheck);
    }

    if (bCheck)
    {
      if (EnvvarApply(key, value, lstEnvVars, sel))
        return true;
    }
    else
      return true; // No need to apply -> success also.
  }// if

  return false;
}// EnvvarApply
