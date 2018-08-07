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
#include "prep.h"
#ifndef CB_PRECOMP
  #include <wx/checklst.h>
  #include <wx/utils.h>

  #include "configmanager.h"
  #include "globals.h"
  #include "manager.h"
  #include "macrosmanager.h"
  #include "logmanager.h"
#endif

#include <map>
#include <utility> // std::pair

#include "envvars_common.h"

// Uncomment this for tracing of method calls in C::B's DebugLog:
//#define TRACE_ENVVARS

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

namespace nsEnvVars
{
  const wxString              EnvVarsSep     = _T("|");
  const wxString              EnvVarsDefault = _T("default");
  std::map<wxString,wxString> EnvVarsStack;
}

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void nsEnvVars::EnvVarsDebugLog(const wxChar* msg, ...)
{
  // load and apply configuration (to application only)
  ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("envvars"));
  if (!cfg)
    return;

  // get whether to print debug message to debug log or not
  bool debug_log = cfg->ReadBool(_T("/debug_log"));
  if (!debug_log)
    return;

  wxString log_msg;
  va_list  arg_list;

  va_start(arg_list, msg);
#if wxCHECK_VERSION(3, 0, 0) && wxUSE_UNICODE
// in wx >=  3 unicode-build (default) we need the %ls here, or the strings get
// cut after the first character
    log_msg = msg;
    log_msg.Replace(_T("%s"), _T("%ls"));
    msg = log_msg.wx_str();
#endif

  log_msg = wxString::FormatV(msg, arg_list);
  va_end(arg_list);

  Manager::Get()->GetLogManager()->DebugLog(log_msg);
}// EnvVarsDebugLog

void nsEnvVars::EnvVarsDebugLog(const wxString &msg, ...)
{
  // load and apply configuration (to application only)
  ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("envvars"));
  if (!cfg)
    return;

  // get whether to print debug message to debug log or not
  bool debug_log = cfg->ReadBool(_T("/debug_log"));
  if (!debug_log)
    return;

  wxString log_msg;
  va_list  arg_list;

  va_start(arg_list, msg);
#if wxCHECK_VERSION(2,9,0) && wxUSE_UNICODE
// in wx >=  2.9 unicode-build (default) we need the %ls here, or the strings get
// cut after the first character
    log_msg = msg;
    log_msg.Replace(_T("%s"), _T("%ls"));
    log_msg = wxString::FormatV(msg, arg_list);
#else
    log_msg = wxString::FormatV(msg, arg_list);
#endif

  va_end(arg_list);

  Manager::Get()->GetLogManager()->DebugLog(log_msg);
}// EnvVarsDebugLog

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

wxArrayString nsEnvVars::EnvvarStringTokeniser(const wxString& str)
{
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("EnvvarStringTokeniser")));
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

    if ((current_char.CompareTo(nsEnvVars::EnvVarsSep.wx_str())==0) && (!inside_quot))
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
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("GetEnvvarSetNames")));
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
  EV_DBGLOG(_T("EnvVars: Found %u envvar sets in config."), num_sets);

  if (num_sets==0)
    set_names.Add(nsEnvVars::EnvVarsDefault);
  else
  {
    for (unsigned int i=0; i<num_sets; ++i)
    {
      wxString set_name = sets[i];
      if (set_name.IsEmpty())
        set_name.Printf(_T("Set%u"), i);

      set_names.Add(set_name);
    }// for
  }// if

  return set_names;
}// GetEnvvarSetNames

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

wxString nsEnvVars::GetActiveSetName()
{
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("GetActiveSetName")));
#endif

  wxString active_set = nsEnvVars::EnvVarsDefault;

  // load and apply configuration (to application only)
  ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("envvars"));
  if (!cfg)
    return active_set;

  // try to get the envvar set name of the currently active global envvar set
  wxString active_set_cfg = cfg->Read(_T("/active_set"));
  if (!active_set_cfg.IsEmpty())
    active_set = active_set_cfg;

  EV_DBGLOG(_T("EnvVars: Obtained '%s' as active envvar set from config."), active_set.wx_str());
  return active_set;
}// GetActiveSetName

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

wxString nsEnvVars::GetSetPathByName(const wxString& set_name, bool check_exists,
                                     bool return_default)
{
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("GetSetPathByName")));
#endif

  wxString set_path = _T("/sets/")+nsEnvVars::EnvVarsDefault; // fall back solution
  if (!return_default)
    set_path.Empty();

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
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("GetEnvvarsBySetPath")));
#endif

  wxArrayString envvars;
  EV_DBGLOG(_T("EnvVars: Searching for envvars in path '%s'."), set_path.wx_str());

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
      EV_DBGLOG(_T("EnvVars: Warning: empty envvar detected and skipped."));
  }
  EV_DBGLOG(_T("EnvVars: Read %lu/%u envvars in path '%s'."),
    static_cast<unsigned long>(envvars.GetCount()), num_envvars, set_path.wx_str());

  return envvars;
}// GetEnvvarsBySetPath

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

bool nsEnvVars::EnvvarSetExists(const wxString& set_name)
{
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("EnvvarSetExists")));
#endif

  if (set_name.IsEmpty())
    return false;

  wxString set_path = nsEnvVars::GetSetPathByName(set_name, true, false);
  if (set_name.IsEmpty())
    return false;

  return true;
}// EnvvarSetExists

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

bool nsEnvVars::EnvvarVetoUI(const wxString& key, wxCheckListBox* lstEnvVars, int sel)
{
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("EnvvarVetoUI")));
#endif

  if (wxGetEnv(key, NULL))
  {
    wxString recursion;
    if (platform::windows) recursion = _T("PATH=%PATH%;C:\\NewPath");
    else                   recursion = _T("PATH=$PATH:/new_path");

    wxString warn_exist;
    warn_exist.Printf(_("Warning: Environment variable '%s' is already set.\n"
                        "Continue with updating it's value?\n"
                        "(Recursions like '%s' will be considered.)"),
                        key.wx_str(), recursion.wx_str());

    if (cbMessageBox(warn_exist, _("Confirmation"),
                     wxYES_NO | wxICON_QUESTION) == wxID_NO)
    {
      if (lstEnvVars && (sel>=0))
        lstEnvVars->Check(sel, false); // Unset to visualise it's NOT set
      return true; // User has vetoed the operation
    }
  }// if

  return false;
}// EnvvarVetoUI

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

bool nsEnvVars::EnvvarsClearUI(wxCheckListBox* lstEnvVars)
{
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("EnvvarsClearUI")));
#endif

  if (!lstEnvVars) return false;

  wxString envsNotUnSet(wxEmptyString);

  // Unset all (checked) variables of lstEnvVars
  for (int i=0; i<(int)lstEnvVars->GetCount(); ++i)
  {
    // Note: It's better not to just clear all because wxUnsetEnv would
    //       fail in case an envvar is not set (not checked).
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
                envsNotUnSet.wx_str() );
    cbMessageBox(msg, _("Error"), wxOK | wxCENTRE | wxICON_ERROR);
    return false;
  }

  return true;
}// EnvvarsClearUI

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

bool nsEnvVars::EnvvarIsRecursive(const wxString& key, const wxString& value)
{
  // Replace all macros the user might have setup for the key
  wxString the_key = key;
  Manager::Get()->GetMacrosManager()->ReplaceMacros(the_key);

  wxString recursion;
  if (platform::windows) recursion = _T("%")+the_key+_("%");
  else                   recursion = _T("$")+the_key;

  if (value.Contains(recursion))
    return true;

  return false;
}// EnvvarIsRecursive

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

bool nsEnvVars::EnvvarDiscard(const wxString &key)
{
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("EnvvarDiscard")));
#endif

  // Replace all macros the user might have setup for the key
  wxString the_key = key;
  Manager::Get()->GetMacrosManager()->ReplaceMacros(the_key);
  if (the_key.Trim().IsEmpty()) return false;
  if (!wxGetEnv(the_key, NULL)) return false; // envvar was not set - nothing to do.

  std::map<wxString,wxString>::iterator it = nsEnvVars::EnvVarsStack.find(the_key);
  if (it!=nsEnvVars::EnvVarsStack.end()) // found an old envvar on the stack
    return nsEnvVars::EnvvarApply(the_key, it->second); // restore old value

  if (!wxUnsetEnv(the_key))
  {
    Manager::Get()->GetLogManager()->Log(F(
      _("Unsetting environment variable '%s' failed."), the_key.wx_str())
    );
    EV_DBGLOG(_T("EnvVars: Unsetting environment variable '%s' failed."), the_key.wx_str());
    return false;
  }

  return true;
}// EnvvarDiscard

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

bool nsEnvVars::EnvvarApply(const wxString& key, const wxString& value)
{
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("EnvvarApply")));
#endif

  // Replace all macros the user might have setup for the key
  wxString the_key = key;
  Manager::Get()->GetMacrosManager()->ReplaceMacros(the_key);
  if (the_key.Trim().IsEmpty()) return false;

  // Value: First, expand stuff like:
  //        set PATH=%PATH%;C:\NewPath OR export PATH=$PATH:/new_path
  //        After, replace all macros the user might have used in addition
  wxString value_set;
  bool     is_set    = wxGetEnv(the_key, &value_set);
  wxString the_value = value;
  if (is_set)
  {
    std::map<wxString,wxString>::iterator it = nsEnvVars::EnvVarsStack.find(the_key);
    if (it==nsEnvVars::EnvVarsStack.end()) // envvar not already on the stack
      nsEnvVars::EnvVarsStack[the_key] = value_set; // remember the old value

    // Avoid endless recursion if the value set contains e.g. $PATH, too
    if (nsEnvVars::EnvvarIsRecursive(the_key,the_value))
    {
      if (nsEnvVars::EnvvarIsRecursive(the_key,value_set))
      {
        EV_DBGLOG(_T("EnvVars: Setting environment variable '%s' failed "
                     "due to unresolvable recursion."), the_key.wx_str());
        return false;
      }
      // Restore original value in case of recursion before
      if (it!=nsEnvVars::EnvVarsStack.end())
        value_set = nsEnvVars::EnvVarsStack[the_key];
      // Resolve recursion now (if any)
      wxString recursion;
      if (platform::windows) recursion = _T("%")+the_key+_("%");
      else                   recursion = _T("$")+the_key;
      the_value.Replace(recursion.wx_str(), value_set.wx_str());
    }
  }
  // Replace all macros the user might have setup for the value
  Manager::Get()->GetMacrosManager()->ReplaceMacros(the_value);

  EV_DBGLOG(_T("EnvVars: Trying to set environment variable '%s' to value '%s'..."), the_key.wx_str(), the_value.wx_str());
  if (!wxSetEnv(the_key, the_value)) // set the envvar as computed
  {
    EV_DBGLOG(_T("EnvVars: Setting environment variable '%s' failed."), the_key.wx_str());
    return false;
  }

  return true;
}// EnvvarApply

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

bool nsEnvVars::EnvvarArrayApply(const wxArrayString& envvar,
                                 wxCheckListBox*      lstEnvVars)
{
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("EnvvarArrayApply")));
#endif

  if (envvar.GetCount() == 3)
  {
    wxString check = envvar[0];
    wxString key   = envvar[1];
    wxString value = envvar[2];

    bool bCheck = check.Trim(true).Trim(false).IsSameAs(_T("1"))?true:false;
    key.Trim(true).Trim(false);
    value.Trim(true).Trim(false);

    int sel = -1;
    if (lstEnvVars)
    {
#if CHECK_LIST_BOX_CLIENT_DATA==1
      sel = lstEnvVars->Append(key + _T(" = ") + value, new EnvVariableListClientData(key, value));
#else
      sel = lstEnvVars->Append(key + _T(" = ") + value);
#endif
      lstEnvVars->Check(sel, bCheck);
    }

    if (bCheck)
    {
      bool success = EnvvarApply(key, value);
      if (!success && lstEnvVars && sel>=0)
        lstEnvVars->Check(sel, false); // Unset on UI to mark it's NOT set

      return success;
    }
    else
      return true; // No need to apply -> success, too.
  }// if

  return false;
}// EnvvarArrayApply

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void nsEnvVars::EnvvarSetApply(const wxString& set_name, bool even_if_active)
{
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("EnvvarSetApply")));
#endif

  // Load and apply envvar set from config (to application only)
  ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("envvars"));
  if (!cfg)
    return;

  // Stores the currently active envar set that has been successfully applied at last
  static wxString last_set_applied = wxEmptyString;

  wxString set_to_apply = set_name;
  if (set_to_apply.IsEmpty())
    set_to_apply = nsEnvVars::GetActiveSetName();

  // Early exit for a special case requested by even_if_active parameter
  if (!even_if_active && set_to_apply.IsSameAs(last_set_applied))
  {
    EV_DBGLOG(_T("EnvVars: Set '%s' will not be applied (already active)."),
              set_to_apply.wx_str());
    return;
  }

  // Show currently activated set in debug log (for reference)
  wxString set_path = nsEnvVars::GetSetPathByName(set_to_apply);
  EV_DBGLOG(_T("EnvVars: Active envvar set is '%s', config path '%s'."),
    set_to_apply.wx_str(), set_path.wx_str());

  // NOTE: Keep this in sync with EnvVarsConfigDlg::LoadSettings
  // Read and apply all envvars from currently active set in config
  wxArrayString vars     = nsEnvVars::GetEnvvarsBySetPath(set_path);
  size_t envvars_total   = vars.GetCount();
  size_t envvars_applied = 0;
  for (unsigned int i=0; i<envvars_total; ++i)
  {
    // Format: [checked?]|[key]|[value]
    wxArrayString var_array = nsEnvVars::EnvvarStringTokeniser(vars[i]);
    if (nsEnvVars::EnvvarArrayApply(var_array))
      envvars_applied++;
    else
    {
      EV_DBGLOG(_T("EnvVars: Invalid envvar in '%s' at position #%u."),
        set_path.wx_str(), i);
    }
  }// for

  if (envvars_total>0)
  {
    last_set_applied = set_to_apply;
    EV_DBGLOG(_T("EnvVars: %lu/%lu envvars applied within C::B focus."),
      static_cast<unsigned long>(envvars_applied), static_cast<unsigned long>(envvars_total));
  }
}// EnvvarSetApply

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

void nsEnvVars::EnvvarSetDiscard(const wxString& set_name)
{
#if defined(TRACE_ENVVARS)
  Manager::Get()->GetLogManager()->DebugLog(F(_T("EnvvarSetDiscard")));
#endif

  // load and apply envvar set from config (to application only)
  ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("envvars"));
  if (!cfg)
    return;

  wxString set_to_discard = set_name;
  if (set_to_discard.IsEmpty())
    set_to_discard = nsEnvVars::GetActiveSetName();

  // Show currently activated set in debug log (for reference)
  wxString set_path = nsEnvVars::GetSetPathByName(set_to_discard);
  EV_DBGLOG(_T("EnvVars: Active envvar set is '%s', config path '%s'."),
    set_to_discard.wx_str(), set_path.wx_str());

  // Read and apply all envvars from currently active set in config
  wxArrayString vars       = nsEnvVars::GetEnvvarsBySetPath(set_path);
  size_t envvars_total     = vars.GetCount();
  size_t envvars_discarded = 0;
  for (unsigned int i=0; i<envvars_total; ++i)
  {
    // Format: [checked?]|[key]|[value]
    wxArrayString var_array = nsEnvVars::EnvvarStringTokeniser(vars[i]);
    if (var_array.GetCount()==3)
    {
      wxString check = var_array[0];
      bool bCheck = check.Trim(true).Trim(false).IsSameAs(_T("1"))?true:false;
      // Do not unset envvars that are not activated (checked)
      if (!bCheck) continue; // next for-loop

      // unset the old envvar
      if (nsEnvVars::EnvvarDiscard(var_array[1]))
        envvars_discarded++;
    }
    else
    {
      EV_DBGLOG(_T("EnvVars: Invalid envvar in '%s' at position #%u."),
        set_path.wx_str(), i);
    }
  }// for

  if (envvars_total>0)
  {
    EV_DBGLOG(_T("EnvVars: %lu/%lu envvars discarded within C::B focus."),
      static_cast<unsigned long>(envvars_discarded), static_cast<unsigned long>(envvars_total));
  }
}// EnvvarSetDiscard
