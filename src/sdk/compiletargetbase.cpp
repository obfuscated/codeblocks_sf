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
#include "globals.h"

CompileTargetBase::CompileTargetBase()
    : m_TargetType(ttExecutable)
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
        wxFileName fname;
        fname.Assign(m_Filename);
        switch (m_TargetType)
        {
            case ttConsoleOnly: 
            case ttExecutable: m_OutputFilename = GetExecutableFilename(); break;
            case ttDynamicLib: m_OutputFilename = GetDynamicLibFilename(); break;
            case ttStaticLib: m_OutputFilename = GetStaticLibFilename(); break;
			default: m_OutputFilename = fname.GetFullPath(); break;
        }
    }
    return m_OutputFilename;
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
    wxFileName fname(m_Filename);
    fname.SetName("lib" + fname.GetName());
    fname.SetExt(DYNAMICLIB_EXT); 
    return fname.GetFullPath();
}

wxString CompileTargetBase::GetDynamicLibDefFilename()
{
    wxFileName fname(m_Filename);
    fname.SetName("lib" + fname.GetName());
    fname.SetExt("def");
    return fname.GetFullPath();
}

wxString CompileTargetBase::GetStaticLibFilename()
{
    wxFileName fname(m_Filename);
    fname.SetName("lib" + fname.GetName());
    fname.SetExt(STATICLIB_EXT); 
    return fname.GetFullPath();
}

wxString CompileTargetBase::GetBasePath()
{
    if (m_Filename.IsEmpty())
        return wxEmptyString;
        
    wxFileName basePath(m_Filename);
    return basePath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
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
