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

#include <wx/filename.h>
#include "compiletargetbase.h"
#include "compilerfactory.h"
#include "globals.h"

CompileTargetBase::CompileTargetBase()
    : m_TargetType(ttExecutable),
    m_CompilerIdx(0)
{
	//ctor
    for (int i = 0; i < 4; ++i)
        m_OptionsRelation[i] = orAppendToParentOptions;
}

CompileTargetBase::~CompileTargetBase()
{
	//dtor
}

const wxString& CompileTargetBase::GetFilename()
{
	return m_Filename;
}

const wxString& CompileTargetBase::GetTitle()
{
	return m_Title;
}

void CompileTargetBase::SetTitle(const wxString& title)
{
	if (m_Title == title)
		return;
	m_Title = title;
	SetModified(true);
}

void CompileTargetBase::SetOutputFilename(const wxString& filename)
{
	if (m_OutputFilename == filename)
		return;
	m_OutputFilename = filename;
	SetModified(true);
}

void CompileTargetBase::SetObjectOutput(const wxString& dirname)
{
	if (m_ObjectOutput == dirname)
		return;
	m_ObjectOutput = dirname;
	SetModified(true);
}

void CompileTargetBase::SetDepsOutput(const wxString& dirname)
{
	if (m_DepsOutput == dirname)
		return;
	m_DepsOutput = dirname;
	SetModified(true);
}

OptionsRelation CompileTargetBase::GetOptionRelation(OptionsRelationType type)
{
	return m_OptionsRelation[type];
}

void CompileTargetBase::SetOptionRelation(OptionsRelationType type, OptionsRelation rel)
{
	if (m_OptionsRelation[type] == rel)
		return;
	m_OptionsRelation[type] = rel;
	SetModified(true);
}

wxString CompileTargetBase::GetOutputFilename()
{
    if (m_OutputFilename.IsEmpty())
    {
        switch (m_TargetType)
        {
            case ttConsoleOnly: 
            case ttExecutable: m_OutputFilename = GetExecutableFilename(); break;
            case ttDynamicLib: m_OutputFilename = GetDynamicLibFilename(); break;
            case ttStaticLib: m_OutputFilename = GetStaticLibFilename(); break;
			default:
                wxFileName fname;
                fname.Assign(m_Filename);
                m_OutputFilename = fname.GetFullPath();
                break;
        }
    }
    return m_OutputFilename;
}

wxString CompileTargetBase::GetObjectOutput()
{
    wxString out;
    if (m_ObjectOutput.IsEmpty())
    {
        out = GetBasePath();
        if (out.IsEmpty() || out.Matches("."))
             return ".objs";
        else
            return out + wxFileName::GetPathSeparator() + ".objs";
    }
    return m_ObjectOutput;
}

wxString CompileTargetBase::GetDepsOutput()
{
    wxString out;
    if (m_DepsOutput.IsEmpty())
    {
        out = GetBasePath();
        if (out.IsEmpty() || out.Matches("."))
             return ".deps";
        else
            return out + wxFileName::GetPathSeparator() + ".deps";
    }
    return m_DepsOutput;
}

wxString CompileTargetBase::GetExecutableFilename()
{
    wxFileName fname(m_Filename);
#ifdef __WXMSW__
    fname.SetExt(EXECUTABLE_EXT); 
#else
    fname.SetExt("");
#endif
    return fname.GetFullPath();
}

wxString CompileTargetBase::GetDynamicLibFilename()
{
    if (m_Filename.IsEmpty())
        m_Filename = m_OutputFilename;
    wxFileName fname(m_Filename);
    fname.SetName(fname.GetName());
    fname.SetExt(DYNAMICLIB_EXT);
    return fname.GetFullPath();
}

wxString CompileTargetBase::GetDynamicLibDefFilename()
{
    if (m_Filename.IsEmpty())
        m_Filename = m_OutputFilename;
    wxFileName fname(m_Filename);
    
    wxString prefix = "lib";
    if (CompilerFactory::CompilerIndexOK(m_CompilerIdx))
    {
        Compiler* compiler = CompilerFactory::Compilers[m_CompilerIdx];
        prefix = compiler->GetSwitches().libPrefix;
    }
    fname.SetName(prefix + fname.GetName());
    fname.SetExt("def");
    return fname.GetFullPath();
}

wxString CompileTargetBase::GetStaticLibFilename()
{
    if (m_Filename.IsEmpty())
        m_Filename = m_OutputFilename;
    wxFileName fname(m_Filename);

    wxString prefix = "lib";
    wxString suffix = STATICLIB_EXT;
    if (CompilerFactory::CompilerIndexOK(m_CompilerIdx))
    {
        Compiler* compiler = CompilerFactory::Compilers[m_CompilerIdx];
        prefix = compiler->GetSwitches().libPrefix;
        suffix = compiler->GetSwitches().libExtension;
    }
    if (!fname.GetName().StartsWith(prefix))
        fname.SetName(prefix + fname.GetName());
    fname.SetExt(suffix);
    return fname.GetFullPath();
}

wxString CompileTargetBase::GetBasePath()
{
    if (m_Filename.IsEmpty())
        return ".";
        
    wxFileName basePath(m_Filename);
    wxString base = basePath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    return !base.IsEmpty() ? base : ".";
}

void CompileTargetBase::SetTargetType(const TargetType& pt)
{
	if (m_TargetType == pt)
		return;
	m_TargetType = pt;
	SetModified(true);
}

const TargetType& CompileTargetBase::GetTargetType()
{
	return m_TargetType;
}

const wxString& CompileTargetBase::GetExecutionParameters()
{
	return m_ExecutionParameters;
}

void CompileTargetBase::SetExecutionParameters(const wxString& params)
{
	if (m_ExecutionParameters == params)
		return;
	m_ExecutionParameters = params;
	SetModified(true);
}

const wxString& CompileTargetBase::GetHostApplication()
{
	return m_HostApplication;
}

void CompileTargetBase::SetHostApplication(const wxString& app)
{
	if (m_HostApplication == app)
		return;
	m_HostApplication = app;
	SetModified(true);
}

void CompileTargetBase::SetCompilerIndex(int compilerIdx)
{
    if (compilerIdx == m_CompilerIdx)
        return;
    m_CompilerIdx = compilerIdx;
    SetModified(true);
}
