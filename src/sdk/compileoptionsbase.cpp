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
* $Revision$
* $Id$
* $HeadURL$
*/

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include "compileoptionsbase.h"
    #include "sdk_events.h"
    #include "manager.h"
    #include "macrosmanager.h"
#endif

CompileOptionsBase::CompileOptionsBase()
	: m_Platform(spAll),
	m_Modified(false),
	m_AlwaysRunPostCmds(false)
{
	//ctor
}

CompileOptionsBase::~CompileOptionsBase()
{
	//dtor
}

void CompileOptionsBase::AddPlatform(int platform)
{
	if (m_Platform & platform)
		return;
	m_Platform |= platform;
	SetModified(true);
}

void CompileOptionsBase::RemovePlatform(int platform)
{
	if (!(m_Platform & platform))
		return;
	m_Platform &= ~platform;
	SetModified(true);
}

void CompileOptionsBase::SetPlatforms(int platforms)
{
	if (m_Platform == platforms)
		return;
	m_Platform = platforms;
	SetModified(true);
}

int CompileOptionsBase::GetPlatforms() const
{
	return m_Platform;
}

bool CompileOptionsBase::SupportsCurrentPlatform() const
{
	#ifdef __WXMSW__
	return m_Platform & spWindows;
	#elif __WXGTK__
	return m_Platform & spUnix;
	#else
	return m_Platform & spMac;
	#endif
}

void CompileOptionsBase::SetLinkerOptions(const wxArrayString& linkerOpts)
{
	if (m_LinkerOptions == linkerOpts)
		return;
	m_LinkerOptions = linkerOpts;
	SetModified(true);
}

const wxArrayString& CompileOptionsBase::GetLinkerOptions() const
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

const wxArrayString& CompileOptionsBase::GetLinkLibs() const
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

const wxArrayString& CompileOptionsBase::GetCompilerOptions() const
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

const wxArrayString& CompileOptionsBase::GetIncludeDirs() const
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

const wxArrayString& CompileOptionsBase::GetResourceIncludeDirs() const
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

const wxArrayString& CompileOptionsBase::GetLibDirs() const
{
	return m_LibDirs;
}

void CompileOptionsBase::SetBuildScripts(const wxArrayString& scripts)
{
	if (m_Scripts == scripts)
		return;

    // make sure we don't have duplicate entries
    // that's why we don't assign the array but rather copy it entry by entry...
    bool casesens = true;
#ifdef __WXMSW__
    casesens = false;
#endif
    m_Scripts.Clear();
    for (size_t i = 0; i < scripts.GetCount(); ++i)
    {
        wxString entry = UnixFilename(scripts[i]);
        if (m_Scripts.Index(entry, casesens) == wxNOT_FOUND)
            m_Scripts.Add(entry);
    }
	SetModified(true);
}

const wxArrayString& CompileOptionsBase::GetBuildScripts() const
{
    return m_Scripts;
}

void CompileOptionsBase::SetCommandsBeforeBuild(const wxArrayString& commands)
{
	if (m_CmdsBefore == commands)
		return;
	m_CmdsBefore = commands;
	SetModified(true);
}

const wxArrayString& CompileOptionsBase::GetCommandsBeforeBuild() const
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

const wxArrayString& CompileOptionsBase::GetCommandsAfterBuild() const
{
	return m_CmdsAfter;
}

bool CompileOptionsBase::GetAlwaysRunPostBuildSteps() const
{
    return m_AlwaysRunPostCmds;
}

void CompileOptionsBase::SetAlwaysRunPostBuildSteps(bool always)
{
    if (m_AlwaysRunPostCmds == always)
        return;
    m_AlwaysRunPostCmds = always;
    SetModified(true);
}

bool CompileOptionsBase::GetModified() const
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

void CompileOptionsBase::AddBuildScript(const wxString& script)
{
    bool casesens = true;
#ifdef __WXMSW__
    casesens = false;
#endif
    wxString envopt = UnixFilename(script);
    if (m_Scripts.Index(envopt, casesens) == wxNOT_FOUND)
    {
        m_Scripts.Add(envopt);
        SetModified(true);
    }
}

void CompileOptionsBase::RemoveLinkerOption(const wxString& option)
{
    bool casesens = true;
#ifdef __WXMSW__
    casesens = false;
#endif
    wxString envopt = option;
    int idx = m_LinkerOptions.Index(envopt, casesens);
    if (idx != wxNOT_FOUND)
    {
        m_LinkerOptions.RemoveAt(idx);
        SetModified(true);
    }
}

void CompileOptionsBase::RemoveLinkLib(const wxString& lib)
{
    bool casesens = true;
#ifdef __WXMSW__
    casesens = false;
#endif
    wxString envopt = lib;
    int idx = m_LinkLibs.Index(envopt, casesens);
    if (idx != wxNOT_FOUND)
    {
        m_LinkLibs.RemoveAt(idx);
        SetModified(true);
    }
}

void CompileOptionsBase::RemoveCompilerOption(const wxString& option)
{
    bool casesens = true;
#ifdef __WXMSW__
    casesens = false;
#endif
    wxString envopt = option;
    int idx = m_CompilerOptions.Index(envopt, casesens);
    if (idx != wxNOT_FOUND)
    {
        m_CompilerOptions.RemoveAt(idx);
        SetModified(true);
    }
}

void CompileOptionsBase::RemoveIncludeDir(const wxString& option)
{
    bool casesens = true;
#ifdef __WXMSW__
    casesens = false;
#endif
    wxString entry = UnixFilename(option);
    int idx = m_IncludeDirs.Index(entry, casesens);
    if (idx != wxNOT_FOUND)
    {
        m_IncludeDirs.RemoveAt(idx);
        SetModified(true);
    }
}

void CompileOptionsBase::RemoveResourceIncludeDir(const wxString& option)
{
    bool casesens = true;
#ifdef __WXMSW__
    casesens = false;
#endif
    wxString entry = UnixFilename(option);
    int idx = m_ResIncludeDirs.Index(entry, casesens);
    if (idx != wxNOT_FOUND)
    {
        m_ResIncludeDirs.RemoveAt(idx);
        SetModified(true);
    }
}

void CompileOptionsBase::RemoveLibDir(const wxString& option)
{
    bool casesens = true;
#ifdef __WXMSW__
    casesens = false;
#endif
    wxString entry = UnixFilename(option);
    int idx = m_LibDirs.Index(entry, casesens);
    if (idx != wxNOT_FOUND)
    {
        m_LibDirs.RemoveAt(idx);
        SetModified(true);
    }
}

void CompileOptionsBase::RemoveCommandsBeforeBuild(const wxString& command)
{
    wxString envopt = command;
	m_CmdsBefore.Remove(envopt);
	SetModified(true);
}

void CompileOptionsBase::RemoveCommandsAfterBuild(const wxString& command)
{
    wxString envopt = command;
	m_CmdsAfter.Remove(envopt);
	SetModified(true);
}

void CompileOptionsBase::RemoveBuildScript(const wxString& script)
{
    bool casesens = true;
#ifdef __WXMSW__
    casesens = false;
#endif
    wxString envopt = UnixFilename(script);
    int idx = m_Scripts.Index(envopt, casesens);
    if (idx != wxNOT_FOUND)
    {
        m_Scripts.RemoveAt(idx);
        SetModified(true);
    }
}

bool CompileOptionsBase::SetVar(const wxString& key, const wxString& value, bool onlyIfExists)
{
    if (onlyIfExists)
    {
        StringHash::iterator it = m_Vars.find(key);
        if (it == m_Vars.end())
            return false;
        it->second = value;
        return true;
    }

    m_Vars[key] = value;
    SetModified(true);
    return true;
}

bool CompileOptionsBase::UnsetVar(const wxString& key)
{
    StringHash::iterator it = m_Vars.find(key);
    if (it != m_Vars.end())
    {
        m_Vars.erase(it);
        SetModified(true);
        return true;
    }
    return false;
}

void CompileOptionsBase::UnsetAllVars()
{
    m_Vars.clear();
}

const wxString& CompileOptionsBase::GetVar(const wxString& key) const
{
    StringHash::const_iterator it = m_Vars.find(key);
    if (it != m_Vars.end())
        return it->second;

    static wxString emptystring = wxEmptyString;
    return emptystring;
}

const StringHash& CompileOptionsBase::GetAllVars() const
{
    return m_Vars;
}
