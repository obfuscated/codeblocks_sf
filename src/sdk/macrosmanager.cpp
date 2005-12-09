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

#include "projectmanager.h"
#include "editormanager.h"
#include "messagemanager.h"
#include "macrosmanager.h"
#include "manager.h"
#include "cbproject.h"
#include "cbeditor.h"
#include "managerproxy.h"
#include "uservarmanager.h"
#include "configmanager.h"
#include "globals.h"

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
    ${FOO_OUTPUT_FILE} // per target
    ${BAR_OUTPUT_DIR} // per target
    $(TARGET_OUTPUT_DIR) // the current target's out dir
    $(TARGET_NAME)       // the current target's name (title)

    ${AMP} TODO: implement AddMacro() for custom macros (like this)
*/

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
	Reset();
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

void MacrosManager::Reset()
{
    SANITY_CHECK();
    m_lastProject = NULL;
    m_lastTarget = NULL;
    m_lastEditor = NULL;

    m_AppPath = UnixFilename(ConfigManager::GetExecutableFolder());
    m_DataPath = UnixFilename(ConfigManager::GetDataFolder());
    m_Plugins = UnixFilename(ConfigManager::GetDataFolder() + _T("/plugins"));
    ClearProjectKeys();
	m_re[0].Compile(_T("(\\$[({]?)([#]*[A-Za-z_0-9]+[\\.]*[A-Za-z_0-9]+)([)}]?)")); // $HOME, $(HOME) and ${HOME}
	m_re[1].Compile(_T("(%)([#]*[A-Za-z_0-9]+[\\.]*[A-Za-z_0-9]+)(%)")); // %HOME%

}

void MacrosManager::ClearProjectKeys()
{
    SANITY_CHECK();
    m_ProjectKeys.Clear();
    m_ProjectValues.Clear();
    m_ProjectKeys.Add(_T("AMP"));m_ProjectValues.Add(_T("&"));
    m_ProjectKeys.Add(_T("CODEBLOCKS"));m_ProjectValues.Add(m_AppPath);
    m_ProjectKeys.Add(_T("APP?PATH"));m_ProjectValues.Add(m_AppPath);
    m_ProjectKeys.Add(_T("DATA?PATH"));m_ProjectValues.Add(m_DataPath);
    m_ProjectKeys.Add(_T("PLUGINS"));m_ProjectValues.Add(m_Plugins);
}

void MacrosManager::RecalcVars(cbProject* project,EditorBase* editor,ProjectBuildTarget* target)
{
	SANITY_CHECK();
	if(!editor)
	{
        m_ActiveEditorFilename = _T("");
        m_lastEditor = NULL;
	}
    else if(editor != m_lastEditor)
    {
        m_ActiveEditorFilename = UnixFilename(editor->GetFilename());
        m_lastEditor = editor;
    }
	if(!project)
	{
		m_ProjectFilename = _T("");
		m_ProjectName = _T("");
		m_ProjectDir = _T("");
		m_ProjectFiles = _T("");
		m_Makefile = _T("");
		m_lastProject = NULL;
        ClearProjectKeys();
        m_ProjectKeys.Add(_T("PROJECT_FILE*"));m_ProjectValues.Add(_T(""));
        m_ProjectKeys.Add(_T("PROJECTFILE*"));m_ProjectValues.Add(_T(""));
        m_ProjectKeys.Add(_T("PROJECT_NAME"));m_ProjectValues.Add(_T(""));
        m_ProjectKeys.Add(_T("PROJECT_DIR*"));m_ProjectValues.Add(_T(""));
        m_ProjectKeys.Add(_T("PROJECTDIR*"));m_ProjectValues.Add(_T(""));
        m_ProjectKeys.Add(_T("MAKEFILE"));m_ProjectValues.Add(_T(""));
        m_ProjectKeys.Add(_T("ALL_PROJECT_FILES"));m_ProjectValues.Add(_T(""));
	}
	else if(project != m_lastProject)
	{
	    m_lastTarget = NULL; // reset last target when project changes
	    m_prjname.Assign(project->GetFilename());
	    m_ProjectFilename = UnixFilename(m_prjname.GetFullName());
	    m_ProjectName = project->GetTitle();
	    m_ProjectDir = UnixFilename(project->GetBasePath());
	    m_Makefile = UnixFilename(project->GetMakefile());
	    m_ProjectFiles = _T("");
        for (int i = 0; i < project->GetFilesCount(); ++i)
            m_ProjectFiles << UnixFilename(project->GetFile(i)->relativeFilename) << _T(" ");
        ClearProjectKeys();
        m_ProjectKeys.Add(_T("PROJECT_FILE*"));m_ProjectValues.Add(m_ProjectFilename);
        m_ProjectKeys.Add(_T("PROJECTFILE*"));m_ProjectValues.Add(m_ProjectFilename);
        m_ProjectKeys.Add(_T("PROJECT_NAME"));m_ProjectValues.Add(m_ProjectName);
        m_ProjectKeys.Add(_T("PROJECT_DIR*"));m_ProjectValues.Add(m_ProjectDir);
        m_ProjectKeys.Add(_T("PROJECTDIR*"));m_ProjectValues.Add(m_ProjectDir);
        m_ProjectKeys.Add(_T("MAKEFILE"));m_ProjectValues.Add(m_Makefile);
        m_ProjectKeys.Add(_T("ALL_PROJECT_FILES"));m_ProjectValues.Add(m_ProjectFiles);

        for (int i = 0; i < project->GetBuildTargetsCount(); ++i)
        {
            ProjectBuildTarget* target = project->GetBuildTarget(i);
            if (!target)
                continue;
            wxString title = target->GetTitle().Upper();
            m_ProjectKeys.Add(title + _T("_OUTPUT_FILE"));
            m_ProjectValues.Add(UnixFilename(target->GetOutputFilename()));
            m_ProjectKeys.Add(title + _T("_OUTPUT_DIR"));
            m_ProjectValues.Add(UnixFilename(target->GetBasePath()));
        }
        m_lastProject = project;
	}

	if(!target)
	{
	    m_TargetOutputDir = _T("");
	    m_TargetName = _T("");
	    m_lastTarget = NULL;
	}
	else if(target != m_lastTarget)
	{
	    m_TargetOutputDir = UnixFilename(target->GetBasePath());
	    m_TargetName = UnixFilename(target->GetTitle());
	    m_lastTarget = target;
	}
}

void MacrosManager::ReplaceMacros(wxString& buffer, bool envVarsToo)
{
    SANITY_CHECK();
	if (buffer.IsEmpty())
		return;

	cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
	EditorBase* editor = Manager::Get()->GetEditorManager()->GetActiveEditor();
	ProjectBuildTarget* target = project ? project->GetCurrentlyCompilingTarget() : 0;

	RecalcVars(project,editor,target);

    int count = 1;
	while (count)
	{
        count = 0;
        for (int i = 0; i < 2; ++i)
        {
            if (!(m_re[i].Matches(buffer)))
                continue;

            wxString replace;
            wxString env = m_re[i].GetMatch(buffer, 2);
            wxString pre = m_re[i].GetMatch(buffer, 1);
            wxString post = m_re[i].GetMatch(buffer, 3);

            if (env.StartsWith(_T("#")))
                replace = UnixFilename(Manager::Get()->GetUserVariableManager()->Replace(env));
            else if (env.Matches(_T("ACTIVE_EDITOR_FILENAME")))
                replace = m_ActiveEditorFilename;
            else if (env.Matches(_T("TARGET_OUTPUT_DIR")))
                replace = m_TargetOutputDir;
            else if (env.Matches(_T("TARGET_NAME")))
                replace = m_TargetName;
            else if (env.Matches(_T("TDAY")))
                replace = wxDateTime::Now().Format(_T("%Y%m%d"));
            else if (env.Matches(_T("TODAY")))
                replace = wxDateTime::Now().Format(_T("%Y-%m-%d"));
            else if (env.Matches(_T("NOW")))
                replace = wxDateTime::Now().Format(_T("%Y-%m-%d-%H.%M"));
            else if (env.Matches(_T("NOW_L")))
                replace = wxDateTime::Now().Format(_T("%Y-%m-%d-%H.%M.%S"));
            else if (env.Matches(_T("WEEKDAY")))
                replace = wxDateTime::Now().Format(_T("%A"));
            else if (env.Matches(_T("TDAY_UTC")))
                replace = wxDateTime::Now().ToGMT().Format(_T("%Y%m%d"));
            else if (env.Matches(_T("TODAY_UTC")))
                replace = wxDateTime::Now().ToGMT().Format(_T("%Y-%m-%d"));
            else if (env.Matches(_T("NOW_UTC")))
                replace = wxDateTime::Now().ToGMT().Format(_T("%Y-%m-%d-%H.%M"));
            else if (env.Matches(_T("NOW_L_UTC")))
                replace = wxDateTime::Now().ToGMT().Format(_T("%Y-%m-%d-%H.%M.%S"));
            else if (env.Matches(_T("WEEKDAY_UTC")))
                replace = wxDateTime::Now().ToGMT().Format(_T("%A"));
			else if (env.Matches(_T("LANGUAGE")))
                replace = wxLocale::GetLanguageName(wxLocale::GetSystemLanguage());
			else if (env.Matches(_T("ENCODING")))
                replace = wxLocale::GetSystemEncodingName();
            else
            {
                for (size_t i = 0; i < m_ProjectKeys.Count(); i++)
                {
                    if (env.Matches(m_ProjectKeys[i]))
                    {
                        replace = m_ProjectValues[i];
                        break;
                    }
                }
            }

			QuoteStringIfNeeded(replace);

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

void MacrosManager::ReplaceEnvVars(wxString& buffer)
{
	ReplaceMacros(buffer, true);
}
