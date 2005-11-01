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

#include "sdk_precomp.h"
#include <wx/menu.h>
#include <wx/regex.h>

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
	UserVariableManager::Free();
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

wxString MacrosManager::ReplaceMacros(const wxString& buffer, bool envVarsToo)
{
    SANITY_CHECK(_T(""));
	wxString tmp = buffer;
	ReplaceMacros(tmp, envVarsToo);
	return tmp;
}

void MacrosManager::ReplaceMacros(wxString& buffer, bool envVarsToo)
{
    SANITY_CHECK();
	/*
		standard macros are:

		${PROJECT_FILENAME} ${PROJECT_FILE} ${PROJECTFILE}
		${PROJECT_NAME}
		${PROJECT_DIR} ${PROJECTDIR} ${PROJECT_DIRECTORY}
		${CODEBLOCKS} ${APP_PATH}  ${APPPATH}
		${DATA_PATH} ${DATAPATH}
		${PLUGINS}
		${ACTIVE_EDITOR_FILENAME}
		${ALL_PROJECT_FILES}
		${MAKEFILE}
		${OUTPUT_FILE} // per target
		${OUTPUT_DIR} // per target

		${AMP} TODO: implement AddMacro() for custom macros (like this)
	*/
	if (buffer.IsEmpty())
		return;

	wxRegEx re[3];
	re[0].Compile(_T("(\\$[({]?)(\\:\\:[A-Za-z_0-9\\.]+)([)}]?)")); // $::VAR.MEMBER, $(::VAR) and ${::VAR}
	re[1].Compile(_T("(\\$[({]?)([A-Za-z_0-9]+)([)}]?)")); // $HOME, $(HOME) and ${HOME}
	re[2].Compile(_T("(%)([A-Za-z_0-9]+)(%)")); // %HOME%

	cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
	EditorBase* editor = Manager::Get()->GetEditorManager()->GetActiveEditor();
	wxFileName prjname;
	if (project)
		prjname.Assign(project->GetFilename());

    int count = 1;
	while (count)
	{
        count = 0;
        for (int i = 0; i < 3; ++i)
        {
            wxString replace;

            if (re[i].Matches(buffer))
            {
                wxString env = re[i].GetMatch(buffer, 2);
                wxString pre = re[i].GetMatch(buffer, 1);
                wxString post = re[i].GetMatch(buffer, 3);
                if (env.Matches(_T("AMP")))
                    replace = _T("&");

                if (env.StartsWith(_T("::")))
                    replace = UnixFilename(Manager::Get()->GetUserVariableManager()->Replace(env));
                else if (env.Matches(_T("PROJECT_FILE*")) || env.Matches(_T("PROJECTFILE*")))
                    replace = project ? UnixFilename(prjname.GetFullName()) : _T("");
                else if (env.Matches(_T("PROJECT_NAME")))
                    replace = project ? project->GetTitle() : _T("");
                else if (env.Matches(_T("PROJECT_DIR*")) || env.Matches(_T("PROJECTDIR*")))
                    replace = project ? UnixFilename(project->GetBasePath()) : _T("");
                else if (env.Matches(_T("MAKEFILE")))
                    replace = project ? UnixFilename(project->GetMakefile()) : _T("");
                else if (env.Matches(_T("CODEBLOCKS")) || env.Matches(_T("APP?PATH")))
                    replace = UnixFilename(ConfigManager::Get()->Read(_T("app_path"), wxEmptyString));
                else if (env.Matches(_T("DATA?PATH")))
                    replace = UnixFilename(ConfigManager::Get()->Read(_T("data_path"), wxEmptyString));
                else if (env.Matches(_T("PLUGINS")))
                    replace = UnixFilename(ConfigManager::Get()->Read(_T("data_path"), wxEmptyString) + _T("/plugins"));
                else if (env.Matches(_T("ALL_PROJECT_FILES")))
                {
                    if (project)
                    {
                        for (int i = 0; i < project->GetFilesCount(); ++i)
                            replace << UnixFilename(project->GetFile(i)->relativeFilename) << _T(" ");
                    }
                }
                else if (env.Matches(_T("ACTIVE_EDITOR_FILENAME")))
                    replace = editor ? UnixFilename(editor->GetFilename()) : _T("");
                else if (project)
                {
                    for (int i = 0; i < project->GetBuildTargetsCount(); ++i)
                    {
                        ProjectBuildTarget* target = project->GetBuildTarget(i);
                        if (!target)
                            continue;
                        wxString title = target->GetTitle().Upper();
                        if (env.Matches(title + _T("_OUTPUT_FILE")))
                            replace = target ? UnixFilename(target->GetOutputFilename()) : _T("");
                        else if (env.Matches(title + _T("_OUTPUT_DIR")))
                            replace = target ? UnixFilename(target->GetBasePath()) : _T("");
                    }
                }

                wxString before = pre + env + post;

                if (!replace.IsEmpty())
                {
                    buffer.Replace(before, replace);
                    ++count;
                }
                else
                {
                    if (envVarsToo)
                    {
                        wxString envactual;
                        wxGetEnv(env, &envactual);
//                        LOGSTREAM << _("Converting ") << before << _(" to ") << envactual << _('\n');
                        buffer.Replace(before, envactual);
                        ++count;
                    }
                }
            }
        }
	}
}

void MacrosManager::ReplaceEnvVars(wxString& buffer)
{
	ReplaceMacros(buffer, true);
}
