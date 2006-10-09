#ifndef LIBRARYCONFIG_H
#define LIBRARYCONFIG_H

#include <wx/arrstr.h>
#include <wx/string.h>

struct LibraryConfig
{
    /** Name of library, may use internal variables in form $(VAR_NAME) */
    wxString LibraryName;

    /** Name of global variable used with this library */
    wxString GlobalVar;

    /** Set of files required by this library
     *
     * Each file name will be threated as relative name inside
     * root library path. It may point to file or directory.
     * When directory is in form:
     *  @code *$(VAR_NAME) @endcode
     * It will be threated as ANY directory and internal variable named
     * VAR_NAME will be set to it's name.
     * Top-level file can not be variable name.
     */
    wxArrayString FileNames;

    /** Set of include directories. If more than one is provided,
     *  first one will be set in global var, other will be added through
     *  cflags.
     *
     *  Include path may use internal variables in form $(VAR_NAME) and
     *  variable $(BASE_DIR) pointing to base directory */
    wxArrayString IncludePaths;

    /** Set of lib directories. If more than one is provided,
     *  first one will be set in global var, other will be added through
     *  lflags.
     *
     *  Lib path may use internal variables in form $(VAR_NAME) and
     *  variable $(BASE_DIR) pointing to base directory */
    wxArrayString LibPaths;

    /** Set of obj directories. If more than one is provided,
     *  first one will be set in global var, other will be added through
     *  lflags.
     *
     *  Obj path may use internal variables in form $(VAR_NAME) and
     *  variable $(BASE_DIR) pointing to base directory */
    wxArrayString ObjPaths;

    /** Global cflags
     *
     * Internal variables and $(BASE_DIR) may be used here. */
    wxString CFlags;

    /** Global lflags
     *
     * Internal variables and $(BASE_DIR) may be used here. */
    wxString LFlags;
};

#endif //LIBRARYCONFIG_H
