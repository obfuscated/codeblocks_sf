/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef BINDINGS_H
#define BINDINGS_H

#include <wx/string.h>
#include <wx/hashmap.h>
#include <wx/arrstr.h>

#define g_alpha_chars         _T("_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz")
#define g_alpha_numeric_chars _T("_0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz")

/** \brief Class containing all bindings between headers and identifiers */
class Bindings
{
public:

  /** \brief Ctor - it will load all bindings from configuration */
  Bindings();

  /** \brief Dctor */
  virtual ~Bindings();

  /** \brief Saving all bindings */
  void SaveBindings();

  /** \brief Adding new binding */
  void AddBinding(wxString Group,wxString Identifier,wxString Header);

  /** \brief Removing given binding */
  void DelBinding(wxString Group,wxString Identifier,wxString Header);

  /** \brief Adding all headers from given group into array */
  void GetBindings(wxString Group,wxString Identifier,wxArrayString& DestHeaders);

  /** \brief Getting array with group names */
  wxArrayString GetGroups();

private:

  /** \brief Initializing Code::Blocks bindings from configuration */
  void InitialiseBindingsFromConfig();
  /** \brief Initializing bindings with default values */
  void SetDefaults();
  /** \brief Initializing Code::Blocks bindings with default values */
  void SetDefaultsCodeBlocks();
  /** \brief Initializing wxWidgets bindings with default values */
  void SetDefaultsWxWidgets();
  /** \brief Initializing STL bindings with default values */
  void SetDefaultsSTL();
  /** \brief Initializing C_Library bindings with default values */
  void SetDefaultsCLibrary();

  WX_DECLARE_STRING_HASH_MAP(wxArrayString,MappingsT);
  WX_DECLARE_STRING_HASH_MAP(MappingsT,GroupsT);

  GroupsT m_Groups; ///< \brief All groups of bindings

  friend class Configuration;
};

#endif
