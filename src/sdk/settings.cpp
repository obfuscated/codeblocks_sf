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

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/wx.h>
    #include "settings.h"
#endif


const wxString DEFAULT_WORKSPACE			= _T("default.workspace");

const wxString WORKSPACES_FILES_FILTER      = _("Code::Blocks workspace files (*.workspace)|*.workspace");
const wxString CODEBLOCKS_FILES_FILTER      = _("Code::Blocks project files (*.cbp)|*.cbp");
const wxString DEVCPP_FILES_FILTER          = _("Bloodshed Dev-C++ project files (*.dev)|*.dev");
const wxString MSVC6_FILES_FILTER           = _("MS Visual C++ 6.0 project files (*.dsp)|*.dsp");
const wxString MSVC7_FILES_FILTER           = _("MS Visual Studio 7.0+ project files (*.vcproj)|*.vcproj");
const wxString MSVC6_WORKSPACE_FILES_FILTER = _("MS Visual C++ 6.0 workspace files (*.dsw)|*.dsw");
const wxString MSVC7_WORKSPACE_FILES_FILTER = _("MS Visual Studio 7.0+ solution files (*.sln)|*.sln");
const wxString D_FILES_FILTER               = _("D files (*.d)|*.d");
const wxString C_FILES_FILTER               = _("C/C++ files (*.c;*.cpp;*.cc;*.cxx;*.h;*.hpp;*.hh;*.hxx)|*.c;*.cpp;*.cc;*.cxx;*.h;*.hpp;*.hh;*.hxx");
const wxString SOURCE_FILES_FILTER          = _("C/C++ source files (*.c;*.cpp;*.cc;*.cxx)|*.c;*.cpp;*.cc;*.cxx");
const wxString HEADER_FILES_FILTER          = _("C/C++ header files (*.h;*.hpp;*.hh;*.hxx)|*.h;*.hpp;*.hh;*.hxx");
const wxString RESOURCE_FILES_FILTER        = _("Resource files (*.xrc;*.rc)|*.xrc;*.rc");
const wxString ALL_KNOWN_FILES_FILTER       = _("All known files|*.workspace;*.cbp;*.c;*.cpp;*.cc;*.cxx;*.d;*.h;*.hpp;*.hh;*.hxx;*.xrc;*.rc");

#ifdef __WXMSW__
    const wxString ALL_FILES_FILTER             = _("All files (*.*)|*.*");
#else
    const wxString ALL_FILES_FILTER             = _("All files (*)|*");
#endif

const wxString SOURCE_FILES_DIALOG_FILTER   = WORKSPACES_FILES_FILTER + _T("|") +
                                              CODEBLOCKS_FILES_FILTER + _T("|") +
                                              D_FILES_FILTER          + _T("|") +
                                              C_FILES_FILTER          + _T("|") +
                                              SOURCE_FILES_FILTER     + _T("|") +
                                              HEADER_FILES_FILTER     + _T("|") +
                                              RESOURCE_FILES_FILTER   + _T("|") +
                                              ALL_KNOWN_FILES_FILTER  + _T("|") +
                                              ALL_FILES_FILTER;

const unsigned SOURCE_FILES_FILTER_INDEX    = 7;

const wxString KNOWN_SOURCES_DIALOG_FILTER  = D_FILES_FILTER          + _T("|") +
											  C_FILES_FILTER          + _T("|") +
                                              SOURCE_FILES_FILTER     + _T("|") +
                                              HEADER_FILES_FILTER     + _T("|") +
                                              RESOURCE_FILES_FILTER   + _T("|") +
                                              ALL_FILES_FILTER;

const unsigned KNOWN_SOURCES_FILTER_INDEX   = 5;

const wxString WORKSPACE_EXT                = _T("workspace");
const wxString CODEBLOCKS_EXT               = _T("cbp");
const wxString DEVCPP_EXT                   = _T("dev");
const wxString MSVC6_EXT                    = _T("dsp");
const wxString MSVC7_EXT                    = _T("vcproj");
const wxString MSVC6_WORKSPACE_EXT          = _T("dsw");
const wxString MSVC7_WORKSPACE_EXT          = _T("sln");
const wxString CPP_EXT                      = _T("cpp");
const wxString D_EXT                        = _T("d");
const wxString C_EXT                        = _T("c");
const wxString CC_EXT                       = _T("cc");
const wxString CXX_EXT                      = _T("cxx");
const wxString HPP_EXT                      = _T("hpp");
const wxString H_EXT                        = _T("h");
const wxString HH_EXT                       = _T("hh");
const wxString HXX_EXT                      = _T("hxx");
const wxString OBJECT_EXT                   = _T("o");
const wxString XRCRESOURCE_EXT              = _T("xrc");
const wxString STATICLIB_EXT                = _T("a");
#ifdef __WXMSW__
    const wxString DYNAMICLIB_EXT           = _T("dll");
    const wxString EXECUTABLE_EXT           = _T("exe");
    const wxString RESOURCE_EXT             = _T("rc");
    const wxString RESOURCEBIN_EXT          = _T("res");
#else
    const wxString DYNAMICLIB_EXT	        = _T("so");
    const wxString EXECUTABLE_EXT	        = _T("");
    const wxString RESOURCE_EXT             = _T("");
    const wxString RESOURCEBIN_EXT          = _T("");
#endif
