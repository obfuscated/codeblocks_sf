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

#ifndef ENVVARS_COMMON_H
#define ENVVARS_COMMON_H

#include <wx/arrstr.h>
#include <wx/string.h>

class wxCheckListBox;

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

namespace nsEnvVars
{
  extern const wxString EnvVarsSep;     //!< separator for envvars in config
  extern const wxString EnvVarsDefault; //!< name of default envvar set

  /// Tokenises an envvar string into sub-strings
  wxArrayString EnvvarStringTokeniser(const wxString& str);
  /// Returns all envvars sets available
  wxArrayString GetEnvvarSetNames();
  /// Returns the name of the currently active set (from config, /active_set)
  wxString      GetActiveSetName();
  /// Returns the path to the envvar set in the config by it's name
  wxString      GetSetPathByName(const wxString& set_name,
                                 bool check_exists = true,
                                 bool return_default = true);
  /// Returns the envvars of an envvars set path in the config
  wxArrayString GetEnvvarsBySetPath(const wxString& set_path);
  /// Verifies if an envvars set really exists in the config
  bool          EnvvarSetExists(const wxString& set_name);
  /// Allows the user to veto overwriting an existing envvar with a nrew value
  bool          EnvvarVeto(const wxString& key, wxCheckListBox* lstEnvVars = NULL,
                           int sel = -1);
  /// Clears all envvars of a checklist box
  bool          EnvvarsClear(wxCheckListBox* lstEnvVars);
  /// Discards an envvar
  bool          EnvvarDiscard(const wxString& key);
  /// Applies a specific envvar (and sets the check in a checklist box accordingly)
  bool          EnvvarApply(const wxString& key, const wxString& value,
                            wxCheckListBox* lstEnvVars = NULL, int sel = -1);
  /// Applies a specific envvar (and appends (checks) the envvar in a checklist box accordingly)
  bool          EnvvarApply(const wxArrayString& envvar,
                            wxCheckListBox* lstEnvVars = NULL);
  /// Applies a specific envvar set from the config (without UI interaction)
  void          EnvvarSetApply(const wxString& set_name = wxEmptyString);
  /// Discards a specific envvar set from the config (without UI interaction)
  void          EnvvarSetDiscard(const wxString& set_name = wxEmptyString);
}// nsEnvVars

#endif // ENVVARS_COMMON_H
