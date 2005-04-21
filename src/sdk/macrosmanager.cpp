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

#include <wx/menu.h>

#include "projectmanager.h"
#include "editormanager.h"
#include "messagemanager.h"
#include "macrosmanager.h"
#include "manager.h"
#include "cbproject.h"
#include "cbeditor.h"
#include "managerproxy.h"

MacrosManager* MacrosManager::Get()
{
    if(Manager::isappShuttingDown()) // The mother of all sanity checks
        MacrosManager::Free();
    else 
    if (!MacrosManagerProxy::Get())
	{
        MacrosManagerProxy::Set( new MacrosManager() );
		Manager::Get()->GetMessageManager()->Log(_("MacrosManager initialized"));
	}
    return MacrosManagerProxy::Get();
}

void MacrosManager::Free()
{
	if (MacrosManagerProxy::Get())
	{
		delete MacrosManagerProxy::Get();
		MacrosManagerProxy::Set( 0L );
	}
}

MacrosManager::MacrosManager()
{
	//ctor
	SC_CONSTRUCTOR_BEGIN
}

MacrosManager::~MacrosManager()
{
	//dtor
	SC_DESTRUCTOR_BEGIN
	SC_DESTRUCTOR_END
}

void MacrosManager::CreateMenu(wxMenuBar* menuBar)
{
    SANITY_CHECK();
}

void MacrosManager::ReleaseMenu(wxMenuBar* menuBar)
{
    SANITY_CHECK();
}

wxString MacrosManager::ReplaceMacros(const wxString& buffer)
{
    SANITY_CHECK("");
	wxString tmp = buffer;
	ReplaceMacros(tmp);
	return tmp;
}

void MacrosManager::ReplaceMacros(wxString& buffer)
{
    SANITY_CHECK();
	/*
		standard macros are:
		
		${PROJECT_FILENAME}
		${PROJECT_NAME}
		${PROJECT_DIR}
		${ACTIVE_EDITOR_FILENAME}
		${ALL_PROJECT_FILES}
		${MAKEFILE}
		
		${AMP} TODO: implement AddMacro() for custom macros (like this)
	*/
	if (buffer.IsEmpty())
		return;
	
	buffer.Replace("${AMP}", "&");

	cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
	if (project)
	{
		wxFileName fname(project->GetFilename());
		buffer.Replace("${PROJECT_FILENAME}", UnixFilename(fname.GetFullName()));
		buffer.Replace("${PROJECT_NAME}", project->GetTitle());
		buffer.Replace("${PROJECT_DIR}", UnixFilename(project->GetBasePath()));
		buffer.Replace("${MAKEFILE}", UnixFilename(project->GetMakefile()));
		
		wxString files;
		for (int i = 0; i < project->GetFilesCount(); ++i)
			files << UnixFilename(project->GetFile(i)->relativeFilename) << " ";
		buffer.Replace("${ALL_PROJECT_FILES}", files);
	}
	else
	{
		buffer.Replace("${PROJECT_FILENAME}", wxEmptyString);
		buffer.Replace("${PROJECT_NAME}", wxEmptyString);
		buffer.Replace("${PROJECT_DIR}", wxEmptyString);
		buffer.Replace("${MAKEFILE}", wxEmptyString);
		buffer.Replace("${ALL_PROJECT_FILES}", wxEmptyString);
	}

	EditorBase* editor = Manager::Get()->GetEditorManager()->GetActiveEditor();
	if (editor)
		buffer.Replace("${ACTIVE_EDITOR_FILENAME}", UnixFilename(editor->GetFilename()));
	else
		buffer.Replace("${ACTIVE_EDITOR_FILENAME}", wxEmptyString);
}
