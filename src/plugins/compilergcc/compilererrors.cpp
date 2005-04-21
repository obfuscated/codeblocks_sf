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

#include "compilererrors.h"
#include <cbeditor.h>
#include <cbproject.h>
#include <projectmanager.h>
#include <editormanager.h>
#include <manager.h>
#include <wx/regex.h>
#include <wx/tipwin.h>
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

void CompilerErrors::AddError(const wxString& filename, long int line, const wxString& error, bool isWarning)
{
	CompileError err;
	err.isWarning = isWarning;
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
        if (!m_Errors[bkp].isWarning)
        {
            m_ErrorIndex = bkp;
            break;
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
        if (!m_Errors[bkp].isWarning)
        {
            m_ErrorIndex = bkp;
            break;
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

int CompilerErrors::ErrorLineHasMore(const wxString& filename, long int line)
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
	cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
	if (project)
	{
        wxString filename = error.filename;
        bool isAbsolute = (filename.Length() > 1 && filename.GetChar(1) == ':') ||
                           filename.StartsWith("/") ||
                           filename.StartsWith("\\");
	    ProjectFile* f = project->GetFileByFilename(error.filename, !isAbsolute, true);
    	if (f)
        {
        	cbEditor* ed = Manager::Get()->GetEditorManager()->Open(f->file.GetFullPath());
            if (ed)
			{
				ed->SetProjectFile(f);
				ed->Activate();
				// make sure we can see some context...
				ed->GetControl()->GotoLine(error.line - 10);
				ed->GetControl()->GotoLine(error.line + 10);
				ed->GetControl()->GotoLine(error.line - 1);
				ed->MarkLine(ERROR_LINE, error.line - 1);
			}
        }
	}
}

void CompilerErrors::DoClearErrorMarkFromAllEditors()
{
	EditorManager* edMan = Manager::Get()->GetEditorManager();
	for (int i = 0; i < edMan->GetEditorsCount(); ++i)
	{
        cbEditor* ed = edMan->GetBuiltinEditor(i);
        if (ed)
            ed->MarkLine(ERROR_LINE, -1);
	}
}

bool CompilerErrors::HasNextError()
{
	return m_ErrorIndex < (int)m_Errors.GetCount();
}

bool CompilerErrors::HasPreviousError()
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

unsigned int CompilerErrors::GetErrorsCount()
{
    unsigned int count = 0;
	for (unsigned int i = 0; i < m_Errors.GetCount(); ++i)
	{
        if (!m_Errors[i].isWarning)
            ++count;
	}
	return count;
}

unsigned int CompilerErrors::GetWarningsCount()
{
    unsigned int count = 0;
	for (unsigned int i = 0; i < m_Errors.GetCount(); ++i)
	{
        if (m_Errors[i].isWarning)
            ++count;
	}
	return count;
}
