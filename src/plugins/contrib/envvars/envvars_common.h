/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef ENVVARS_COMMON_H
#define ENVVARS_COMMON_H

#include <wx/arrstr.h>
#include <wx/string.h>

class wxCheckListBox;


#if (defined (__WIN32__) || defined (_WIN64)) && !wxCHECK_VERSION(3, 0, 0)
    #define CHECK_LIST_BOX_CLIENT_DATA 0
#else
    #define CHECK_LIST_BOX_CLIENT_DATA 1
#endif

#define EV_DBGLOG nsEnvVars::EnvVarsDebugLog

// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

namespace nsEnvVars
{
  extern const wxString EnvVarsSep;     //!< separator for envvars in config
  extern const wxString EnvVarsDefault; //!< name of default envvar set

  /** Prints a message to C::B's debug log depending on debug activated or not
    * \param msg Message to print at C::B's debug log
    */
  void          EnvVarsDebugLog(const wxChar* msg, ...);
  void          EnvVarsDebugLog(const wxString& msg, ...);

  /** Tokenises an envvar string into sub-strings
    * \param str String to tokenise (envvars set format to array string)
    * \return Tokenised envars as array string
    */
  wxArrayString EnvvarStringTokeniser(const wxString& str);
  /** Returns all envvars sets available
    * \return All envvars sets available in the config
    */
  wxArrayString GetEnvvarSetNames();
  /** Returns the name of the currently active set (from config, /active_set)
    * \return Name of the currently active set (from config, /active_set)
    */
  wxString      GetActiveSetName();
  /** Returns the path to the envvar set in the config by it's name
    * \param set_name Name of the set to query/find in the config
    * \param check_exists Check if the set really exists in the config (error otherwise)
    * \param return_default Return the default envvar set if the set to query has not been found in the config
    * \return Name of the envvar set
    */
  wxString      GetSetPathByName(const wxString& set_name,
                                 bool check_exists = true,
                                 bool return_default = true);
  /** Returns the envvars of an envvars set path in the config
    * \param set_path Path to the envvars set in the config to query
    * \return All envvars that belong to the envvar set in the config
    */
  wxArrayString GetEnvvarsBySetPath(const wxString& set_path);
  /** Verifies if an envvars set really exists in the config
    * \param set_name Name of the set to query/find in the config
    * \return Has the set been found / does it exist in the config?
    */
  bool          EnvvarSetExists(const wxString& set_name);
  /** Allows the user to veto overwriting an existing envvar with a new value
    * \param key envvar key to set (overwrite)
    * \param lstEnvVars Pointer to a check-listbox to update (envvar settings dialog)
    * \param sel Selection in the check listbox to update
    * \return Has the user vetoed to update the envvar?
    */
  bool          EnvvarVetoUI(const wxString& key, wxCheckListBox* lstEnvVars, int sel);
  /** Clears all envvars of a checklist box
    * \param lstEnvVars Pointer to a check-listbox to update (envvar settings dialog)
    * \return Has the check listbox been updated successfully?
    */
  bool          EnvvarsClearUI(wxCheckListBox* lstEnvVars);
  /** Checks, if an envvar is recursive, like PATH=%PATH%;C:\Folder
    * \param key   envvar to check
    * \param value value of the envvar to check for recursion
    * \return Is this a recursive envvar?
    */
  bool          EnvvarIsRecursive(const wxString& key, const wxString& value);
  /** Discards an envvar
    * \param key envvar to discard (erase)
    * \return Has the envvar been discarded successfully?
    */
  bool          EnvvarDiscard(const wxString& key);
  /** Applies a specific envvar
    * \param key envvar key to set
    * \param value envvar value to set (value of the key)
    * \return Has the envvar been applied successfully?
    */
  bool          EnvvarApply(const wxString& key, const wxString& value);
  /** Applies a specific envvar array (and appends (checks) the envvar in a checklist box accordingly)
    * \param envvar Set of 1..n envvars to apply
    * \param lstEnvVars Pointer to a check-listbox to update (envvar settings dialog)
    * \return Have all envvars been applied successfully?
    */
  bool          EnvvarArrayApply(const wxArrayString& envvar,
                                 wxCheckListBox*      lstEnvVars = NULL);
  /** Applies a specific envvar set from the config (without UI interaction)
    * \param set_name Name of the set to apply (maps to a path in the config)
    * \param even_if_active Apply the envvar set even if it is active (it might have changed!)
    */
  void          EnvvarSetApply(const wxString& set_name, bool even_if_active);
  /** Discards a specific envvar set from the config (without UI interaction)
    * \param set_name envvar set to discard completely
    */
  void          EnvvarSetDiscard(const wxString& set_name);


#if CHECK_LIST_BOX_CLIENT_DATA==1
  struct EnvVariableListClientData : wxClientData
  {
      EnvVariableListClientData(const wxString &_key, const wxString &_value) : key(_key), value(_value) {}
      wxString key, value;
  };
#endif

}// nsEnvVars

#endif // ENVVARS_COMMON_H
