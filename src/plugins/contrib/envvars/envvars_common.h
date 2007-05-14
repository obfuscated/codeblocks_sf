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
  extern const wxString EnvVarsSep;
  extern const wxString EnvVarsDefault;

  wxArrayString EnvvarStringTokeniser(const wxString& str);
  wxArrayString GetEnvvarSetNames();
  wxString      GetSetPathByName(const wxString& set_name, bool check_exists = true);
  wxArrayString GetEnvvarsBySetPath(const wxString& set_path);
  bool          EnvvarVeto(wxString &key, wxCheckListBox* lstEnvVars = NULL, int sel = -1);
  bool          EnvvarsClear(wxCheckListBox* lstEnvVars);
  bool          EnvvarDiscard(wxString &key);
  bool          EnvvarApply(wxString &key, wxString& value, wxCheckListBox* lstEnvVars = NULL, int sel = -1);
  bool          EnvvarApply(wxArrayString &envvar, wxCheckListBox* lstEnvVars = NULL);
}// nsEnvVars

#endif // ENVVARS_COMMON_H
