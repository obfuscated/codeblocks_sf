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
* $Revision: 4504 $
* $Id: wxsmithpluginregistrants.cpp 4504 2007-10-02 21:52:30Z byo $
* $HeadURL: svn+ssh://byo@svn.berlios.de/svnroot/repos/codeblocks/trunk/src/plugins/contrib/wxSmith/plugin/wxsmithpluginregistrants.cpp $
*/

#ifndef LIBRARYRESULT_H
#define LIBRARYRESULT_H

#include <wx/string.h>
#include <wx/arrstr.h>

/** \brief Type of library configuration result */
enum LibraryResultType
{
    rtDetected = 0,     ///< \brief Cnofiguration detected by lib_finder
    rtPredefined,       ///< \brief Predefined configuration
    rtPkgConfig,        ///< \brief Library provided by pkg-config
    ///////
    rtCount,            ///< \brief Here will be the number of result types
    rtUnknown = -1      ///< \brief Used for unknown result types
};

/** \brief Library configuration result
 *
 * This structure contains informations about specific configuration
 * of library. It's usually a result of some detection (at the beginning
 * there were only results of detection) so here's where the name is from.
 */
struct LibraryResult
{
    LibraryResultType Type;
    wxString LibraryName;
    wxString ShortCode;
    wxString BasePath;
    wxString Description;
    wxString PkgConfigVar;
    wxArrayString Categories;
    wxArrayString IncludePath;
    wxArrayString LibPath;
    wxArrayString ObjPath;
    wxArrayString Libs;
    wxArrayString Defines;
    wxArrayString CFlags;
    wxArrayString LFlags;
    wxArrayString Compilers;
    wxArrayString Headers;

    bool operator == ( const LibraryResult& compareWith ) const;
    void SetGlobalVar() const;
};

#endif
