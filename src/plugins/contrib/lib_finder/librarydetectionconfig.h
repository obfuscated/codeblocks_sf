/*
* This file is part of lib_finder plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* $Revision$
* $Id$
* $HeadURL$
*/

#ifndef LIBRARYCONFIG_H
#define LIBRARYCONFIG_H

#include <wx/arrstr.h>
#include <wx/string.h>
#include <vector>

/** \brief Requirement which must be meet for this library to be detected */
struct LibraryDetectionFilter
{
    enum FilterType
    {
        None,       ///< \brief Nothing is required by this filter
        File,       ///< \brief Some file is required
        Platform,   ///< \brief Some platform is required
        Exec,       ///< \brief Some executable is required
        PkgConfig,  ///< \brief Configuration in pkg-config is required
        Compiler,   ///< \brief Some compiler is required
    };

    /** \brief Type of filter */
    FilterType Type;

    /** \brief Value required by the filter (f.ex. file name pattern) */
    wxString Value;
};

/** \brief Configuration of the library */
struct LibraryDetectionConfig
{
    /** \brief Description of the library */
    wxString Description;

    /** \brief Pkg-Config variable name */
    wxString PkgConfigVar;

    /** \brief Set of filters required by the library */
    std::vector<LibraryDetectionFilter> Filters;

    /** \brief Set of include directories. If more than one is provided,
     *  first one will be set in global var, other will be added through
     *  cflags.
     *
     *  Include path may use internal variables in form $(VAR_NAME) and
     *  variable $(BASE_DIR) pointing to base directory */
    wxArrayString IncludePaths;

    /** \brief Set of lib directories. If more than one is provided,
     *  first one will be set in global var, other will be added through
     *  lflags.
     *
     *  Lib path may use internal variables in form $(VAR_NAME) and
     *  variable $(BASE_DIR) pointing to base directory */
    wxArrayString LibPaths;

    /** \brief List of libraries which should be added */
    wxArrayString Libs;

    /** \brief List of defines which should be added */
    wxArrayString Defines;

    /** \brief Set of obj directories. If more than one is provided,
     *  first one will be set in global var, other will be added through
     *  lflags.
     *
     *  Obj path may use internal variables in form $(VAR_NAME) and
     *  variable $(BASE_DIR) pointing to base directory */
    wxArrayString ObjPaths;

    /** \brief Global cflags
     *
     * Internal variables and $(BASE_DIR) may be used here. */
    wxArrayString CFlags;

    /** \brief Global lflags
     *
     * Internal variables and $(BASE_DIR) may be used here. */
    wxArrayString LFlags;

    /** \brief List of headers used by this library */
    wxArrayString Headers;

    /** \brief List of other libraries required by this one */
    wxArrayString Require;
};

/** \brief Structure containing set of detection configurations for given library */
struct LibraryDetectionConfigSet
{
    /** \brief Library's shortcode */
    wxString ShortCode;

    /** \brief Name of library, may use internal variables in form $(VAR_NAME) */
    wxString LibraryName;

    /** \brief Category list */
    wxArrayString Categories;

    /** \brief Version of detection settings */
    int Version;

    /** \brief Detection settings */
    std::vector< LibraryDetectionConfig > Configurations;
};

#endif
