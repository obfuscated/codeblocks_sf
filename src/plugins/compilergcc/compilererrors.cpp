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
#include "sdk.h"
#ifndef CB_PRECOMP
#include "cbeditor.h"
#include "cbproject.h"
#include "editormanager.h"
#include "manager.h"
#include "projectfile.h"
#include "projectmanager.h"
#endif
#include "compilererrors.h"
#include <wx/arrimpl.cpp>

WX_DEFINE_OBJARRAY(ErrorsArray);

CompilerErrors::CompilerErrors()
	: m_ErrorIndex(-1)
{
	//ctor
}

CompilerErrors::~CompilerErrors()
{
	//dtor
}

void CompilerErrors::AddError(CompilerLineType lt, cbProject* project, const wxString& filename, long int line, const wxString& error)
{
	CompileError err;
	err.lineType = lt;
	err.project = project;
	err.filename = filename;
	err.line = line;
	err.errors.Add(error);
	DoAddError(err);
}

void CompilerErrors::GotoError(int nr)
{
	if (m_Errors.GetCount() == 0 || nr < 0 || nr > (int)m_Errors.GetCount() - 1)
		return;
    m_ErrorIndex = nr;
	DoGotoError(m_Errors[m_ErrorIndex]);
}

void CompilerErrors::Next()
{
	if (m_ErrorIndex >= (int)m_Errors.GetCount() - 1)
		return;

    // locate next *error* (not warning), if there is any
    int bkp = ++m_ErrorIndex;
    while (bkp < (int)m_Errors.GetCount())
    {
        if (m_Errors[bkp].lineType == cltError)
        {
            bool isNote =
            	((m_Errors[bkp].errors.GetCount()>0) && m_Errors[bkp].errors[0].StartsWith(_T("note:")));
            if(!isNote)
            {
            	m_ErrorIndex = bkp;
                break;
            }
        }
        ++bkp;
    }

	DoGotoError(m_Errors[m_ErrorIndex]);
}

void CompilerErrors::Previous()
{
	if (m_ErrorIndex <= 0)
        return;

    // locate previous *error* (not warning), if there is any
    int bkp = --m_ErrorIndex;
    while (bkp >= 0)
    {
        if (m_Errors[bkp].lineType == cltError)
        {
            bool isNote =
            	((m_Errors[bkp].errors.GetCount()>0) && m_Errors[bkp].errors[0].StartsWith(_T("note:")));
            if(!isNote)
            {
            	m_ErrorIndex = bkp;
                break;
            }
        }
        --bkp;
    }

    DoGotoError(m_Errors[m_ErrorIndex]);
}

void CompilerErrors::Clear()
{
	DoClearErrorMarkFromAllEditors();
	m_Errors.Clear();
	m_ErrorIndex = -1;
}

void CompilerErrors::DoAddError(const CompileError& error)
{
//	int index = ErrorLineHasMore(error.filename, error.line);
//	if (index != -1)
//	{
//		for (unsigned int i = 0; i < error.errors.GetCount(); ++i)
//			m_Errors[index].errors.Add(error.errors[i]);
//	}
//	else
		m_Errors.Add(error);
}

int CompilerErrors::ErrorLineHasMore(const wxString& filename, long int line) const
{
	for (unsigned int i = 0; i < m_Errors.GetCount(); ++i)
	{
		if (m_Errors[i].filename.Matches(filename) &&
			m_Errors[i].line == line)
			return i;
	}
	return -1;
}

void CompilerErrors::DoGotoError(const CompileError& error)
{
    if (error.line <= 0)
        return;
	DoClearErrorMarkFromAllEditors();
	cbEditor* ed = 0;
	cbProject* project = error.project ? error.project : Manager::Get()->GetProjectManager()->GetActiveProject();
	if (project && Manager::Get()->GetProjectManager()->IsProjectStillOpen(project))
	{
        wxString filename = error.filename;
        bool isAbsolute = (filename.Length() > 1 && filename.GetChar(1) == ':') ||
                           filename.StartsWith(_T("/")) ||
                           filename.StartsWith(_T("\\"));
	    ProjectFile* f = project->GetFileByFilename(error.filename, !isAbsolute, true);
    	if (f)
        {
        	ed = Manager::Get()->GetEditorManager()->Open(f->file.GetFullPath());
            if (ed)
			{
				ed->SetProjectFile(f);
			}
        }
		else
		{
			if(!isAbsolute) // this is always the case, except for system headers
				filename.Prepend(project->GetCommonTopLevelPath());

			ed = Manager::Get()->GetEditorManager()->Open(filename);
		}
	}

	// if we reached here and ed is NULL, either the error file doesn't belong to a project,
	// or can't be found for any other reason.
	// check if we can open it directly...
    if (!ed)
        ed = Manager::Get()->GetEditorManager()->Open(error.filename);

    if (ed)
    {
        ed->Activate();
        ed->UnfoldBlockFromLine(error.line - 1);
        ed->GotoLine(error.line - 1);
        ed->SetErrorLine(error.line - 1);
    }
}

void CompilerErrors::DoClearErrorMarkFromAllEditors()
{
	EditorManager* edMan = Manager::Get()->GetEditorManager();
	for (int i = 0; i < edMan->GetEditorsCount(); ++i)
	{
        cbEditor* ed = edMan->GetBuiltinEditor(i);
        if (ed)
            ed->SetErrorLine(-1);
	}
}

bool CompilerErrors::HasNextError() const
{
	return m_ErrorIndex < (int)m_Errors.GetCount();
}

bool CompilerErrors::HasPreviousError() const
{
	return m_ErrorIndex > 0;
}

wxString CompilerErrors::GetErrorString(int index)
{
	if (m_Errors.GetCount() == 0 || index < 0 || index > (int)m_Errors.GetCount() - 1)
		return wxEmptyString;
    wxArrayString& errors = m_Errors[index].errors;
    wxString error;
    if (errors.GetCount())
        error = errors[0];
    return error;
}

int CompilerErrors::GetFirstError() const
{
	for (unsigned int i = 0; i < m_Errors.GetCount(); ++i)
	{
        if (m_Errors[i].lineType == cltError)
            return i;
	}
	return -1;
}

unsigned int CompilerErrors::GetCount(CompilerLineType lt) const
{
    unsigned int count = 0;
	for (unsigned int i = 0; i < m_Errors.GetCount(); ++i)
	{
        if (m_Errors[i].lineType == lt)
            ++count;
	}
	return count;
}
