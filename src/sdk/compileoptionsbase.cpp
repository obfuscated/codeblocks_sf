/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Id$
* $Date$
*/

#include "compileoptionsbase.h"
#include "sdk_events.h"
#include "manager.h"
#include "macrosmanager.h"

CompileOptionsBase::CompileOptionsBase()
	: m_BuildConfiguration(bcDebug),
	m_Modified(false),
	m_Cpp(true),
	m_AlwaysRunPreCmds(false),
	m_AlwaysRunPostCmds(false)
{
	//ctor
}

CompileOptionsBase::~CompileOptionsBase()
{
	//dtor
}

void CompileOptionsBase::SetBuildConfiguration(const BuildConfiguration& bc)
{
	if (m_BuildConfiguration == bc)
		return;
	m_BuildConfiguration = bc;
	SetModified(true);
}

const BuildConfiguration& CompileOptionsBase::GetBuildConfiguration()
{
	return m_BuildConfiguration;
}

void CompileOptionsBase::SetLinkerOptions(const wxArrayString& linkerOpts)
{
	if (m_LinkerOptions == linkerOpts)
		return;
	m_LinkerOptions = linkerOpts;
	SetModified(true);
}

const wxArrayString& CompileOptionsBase::GetLinkerOptions()
{
	return m_LinkerOptions;
}

void CompileOptionsBase::SetLinkLibs(const wxArrayString& linkLibs)
{
    if (m_LinkLibs == linkLibs)
        return;
    m_LinkLibs = linkLibs;
    SetModified(true);
}

const wxArrayString& CompileOptionsBase::GetLinkLibs()
{
    return m_LinkLibs;
}

void CompileOptionsBase::SetCompilerOptions(const wxArrayString& compilerOpts)
{
	if (m_CompilerOptions == compilerOpts)
		return;
	m_CompilerOptions = compilerOpts;
	SetModified(true);
}

const wxArrayString& CompileOptionsBase::GetCompilerOptions()
{
	return m_CompilerOptions;
}

void CompileOptionsBase::SetIncludeDirs(const wxArrayString& includeDirs)
{
	if (m_IncludeDirs == includeDirs)
		return;

    // make sure we don't have duplicate entries
    // that's why we don't assign the array but rather copy it entry by entry...
    bool casesens = true;
#ifdef __WXMSW__
    casesens = false;
#endif
    m_IncludeDirs.Clear();
    for (size_t i = 0; i < includeDirs.GetCount(); ++i)
    {
        wxString entry = UnixFilename(includeDirs[i]);
        if (m_IncludeDirs.Index(entry, casesens) == wxNOT_FOUND)
            m_IncludeDirs.Add(entry);
    }
	SetModified(true);
}

const wxArrayString& CompileOptionsBase::GetIncludeDirs()
{
	return m_IncludeDirs;
}

void CompileOptionsBase::SetResourceIncludeDirs(const wxArrayString& resIncludeDirs)
{
	if (m_ResIncludeDirs == resIncludeDirs)
		return;

    // make sure we don't have duplicate entries
    // that's why we don't assign the array but rather copy it entry by entry...
    bool casesens = true;
#ifdef __WXMSW__
    casesens = false;
#endif
    m_ResIncludeDirs.Clear();
    for (size_t i = 0; i < resIncludeDirs.GetCount(); ++i)
    {
        wxString entry = UnixFilename(resIncludeDirs[i]);
        if (m_ResIncludeDirs.Index(entry, casesens) == wxNOT_FOUND)
            m_ResIncludeDirs.Add(entry);
    }
	SetModified(true);
}

const wxArrayString& CompileOptionsBase::GetResourceIncludeDirs()
{
    return m_ResIncludeDirs;
}

void CompileOptionsBase::SetLibDirs(const wxArrayString& libDirs)
{
	if (m_LibDirs == libDirs)
		return;

    // make sure we don't have duplicate entries
    // that's why we don't assign the array but rather copy it entry by entry...
    bool casesens = true;
#ifdef __WXMSW__
    casesens = false;
#endif
    m_LibDirs.Clear();
    for (size_t i = 0; i < libDirs.GetCount(); ++i)
    {
        wxString entry = UnixFilename(libDirs[i]);
        if (m_LibDirs.Index(entry, casesens) == wxNOT_FOUND)
            m_LibDirs.Add(entry);
    }
	SetModified(true);
}

const wxArrayString& CompileOptionsBase::GetLibDirs()
{
	return m_LibDirs;
}

void CompileOptionsBase::SetCommandsBeforeBuild(const wxArrayString& commands)
{
	if (m_CmdsBefore == commands)
		return;
	m_CmdsBefore = commands;
	SetModified(true);
}

const wxArrayString& CompileOptionsBase::GetCommandsBeforeBuild()
{
	return m_CmdsBefore;
}

void CompileOptionsBase::SetCommandsAfterBuild(const wxArrayString& commands)
{
	if (m_CmdsAfter == commands)
		return;
	m_CmdsAfter = commands;
	SetModified(true);
}

const wxArrayString& CompileOptionsBase::GetCommandsAfterBuild()
{
	return m_CmdsAfter;
}

bool CompileOptionsBase::GetModified()
{
	return m_Modified;
}

void CompileOptionsBase::SetModified(bool modified)
{
	m_Modified = modified;
}

void CompileOptionsBase::AddLinkerOption(const wxString& option)
{
    bool casesens = true;
#ifdef __WXMSW__
    casesens = false;
#endif
    wxString envopt = option;
    if (m_LinkerOptions.Index(envopt, casesens) == wxNOT_FOUND)
    {
        m_LinkerOptions.Add(envopt);
        SetModified(true);
    }
}

void CompileOptionsBase::AddLinkLib(const wxString& lib)
{
    bool casesens = true;
#ifdef __WXMSW__
    casesens = false;
#endif
    wxString envopt = lib;
    if (m_LinkLibs.Index(envopt, casesens) == wxNOT_FOUND)
    {
        m_LinkLibs.Add(envopt);
        SetModified(true);
    }
}

void CompileOptionsBase::AddCompilerOption(const wxString& option)
{
    bool casesens = true;
#ifdef __WXMSW__
    casesens = false;
#endif
    wxString envopt = option;
    if (m_CompilerOptions.Index(envopt, casesens) == wxNOT_FOUND)
    {
        m_CompilerOptions.Add(envopt);
        SetModified(true);
    }
}

void CompileOptionsBase::AddIncludeDir(const wxString& option)
{
    bool casesens = true;
#ifdef __WXMSW__
    casesens = false;
#endif
    wxString entry = UnixFilename(option);
    if (m_IncludeDirs.Index(entry, casesens) == wxNOT_FOUND)
    {
        m_IncludeDirs.Add(entry);
        SetModified(true);
    }
}

void CompileOptionsBase::AddResourceIncludeDir(const wxString& option)
{
    bool casesens = true;
#ifdef __WXMSW__
    casesens = false;
#endif
    wxString entry = UnixFilename(option);
    if (m_ResIncludeDirs.Index(entry, casesens) == wxNOT_FOUND)
    {
        m_ResIncludeDirs.Add(entry);
        SetModified(true);
    }
}

void CompileOptionsBase::AddLibDir(const wxString& option)
{
    bool casesens = true;
#ifdef __WXMSW__
    casesens = false;
#endif
    wxString entry = UnixFilename(option);
    if (m_LibDirs.Index(entry, casesens) == wxNOT_FOUND)
    {
        m_LibDirs.Add(entry);
        SetModified(true);
    }
}

void CompileOptionsBase::AddCommandsBeforeBuild(const wxString& command)
{
    wxString envopt = command;
	m_CmdsBefore.Add(envopt);
	SetModified(true);
}

void CompileOptionsBase::AddCommandsAfterBuild(const wxString& command)
{
    wxString envopt = command;
	m_CmdsAfter.Add(envopt);
	SetModified(true);
}

bool CompileOptionsBase::GetCpp()
{
	return m_Cpp;
}

void CompileOptionsBase::SetCpp(bool cpp)
{
	if (m_Cpp == cpp)
		return;
	m_Cpp = cpp;
	SetModified(true);
}

bool CompileOptionsBase::GetAlwaysRunPreBuildSteps()
{
    return m_AlwaysRunPreCmds;
}

bool CompileOptionsBase::GetAlwaysRunPostBuildSteps()
{
    return m_AlwaysRunPostCmds;
}

void CompileOptionsBase::SetAlwaysRunPreBuildSteps(bool always)
{
    if (m_AlwaysRunPreCmds == always)
        return;
    m_AlwaysRunPreCmds = always;
    SetModified(true);
}

void CompileOptionsBase::SetAlwaysRunPostBuildSteps(bool always)
{
    if (m_AlwaysRunPostCmds == always)
        return;
    m_AlwaysRunPostCmds = always;
    SetModified(true);
}
