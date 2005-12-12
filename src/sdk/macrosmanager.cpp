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
#include "customvars.h"

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
    SANITY_CHECK(wxEmptyString);
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
    m_re.Compile(_T("(\\$[({]?|%)(#?[A-Za-z_0-9]+[\\.]?[A-Za-z_0-9]+)([)}%]?)"));
    uVarMan = Manager::Get()->GetUserVariableManager();
}

void MacrosManager::ClearProjectKeys()
{
    SANITY_CHECK();

    macros.clear();

    macros[_T("AMP")]   = _T("&");
    macros[_T("CODEBLOCKS")] = m_AppPath;
    macros[_T("APP_PATH")]  = m_AppPath;
    macros[_T("APP-PATH")]  = m_AppPath;
    macros[_T("APPPATH")]  = m_AppPath;
    macros[_T("DATA_PATH")]  = m_DataPath;
    macros[_T("DATA-PATH")]  = m_DataPath;
    macros[_T("DATAPATH")]  = m_DataPath;
    macros[_T("PLUGINS")]  = m_Plugins;
}

void MacrosManager::RecalcVars(cbProject* project,EditorBase* editor,ProjectBuildTarget* target)
{
    SANITY_CHECK();

    if(!editor)
    {
        m_ActiveEditorFilename = wxEmptyString;
        m_lastEditor = NULL;
    }
    else if(editor != m_lastEditor)
    {
        m_ActiveEditorFilename = UnixFilename(editor->GetFilename());
        m_lastEditor = editor;
    }
    if(!project)
    {
        m_ProjectFilename = wxEmptyString;
        m_ProjectName = wxEmptyString;
        m_ProjectDir = wxEmptyString;
        m_ProjectFiles = wxEmptyString;
        m_Makefile = wxEmptyString;
        m_lastProject = NULL;
        ClearProjectKeys();
        macros[_T("PROJECT_FILE")]  = wxEmptyString;
        macros[_T("PROJECT_FILENAME")] = wxEmptyString;
        macros[_T("PROJECT_FILE_NAME")] = wxEmptyString;
        macros[_T("PROJECTFILE")]  = wxEmptyString;
        macros[_T("PROJECTFILENAME")] = wxEmptyString;
        macros[_T("PROJECT_NAME")]  = wxEmptyString;
        macros[_T("PROJECTNAME")]  = wxEmptyString;
        macros[_T("PROJECT_DIR")]  = wxEmptyString;
        macros[_T("PROJECT_DIRECTORY")] = wxEmptyString;
        macros[_T("PROJECTDIR")]  = wxEmptyString;
        macros[_T("PROJECTDIRECTORY")] = wxEmptyString;
        macros[_T("MAKEFILE")]   = wxEmptyString;
        macros[_T("ALL_PROJECT_FILES")] = wxEmptyString;
    }
    else if(project != m_lastProject)
    {
        m_lastTarget = NULL; // reset last target when project changes
        m_prjname.Assign(project->GetFilename());
        m_ProjectFilename = UnixFilename(m_prjname.GetFullName());
        m_ProjectName = project->GetTitle();
        m_ProjectDir = UnixFilename(project->GetBasePath());
        m_Makefile = UnixFilename(project->GetMakefile());
        m_ProjectFiles = wxEmptyString;
        for (int i = 0; i < project->GetFilesCount(); ++i)
            m_ProjectFiles << UnixFilename(project->GetFile(i)->relativeFilename) << _T(' ');

        ClearProjectKeys();
        macros[_T("PROJECT_FILE")]  = m_ProjectFilename;
        macros[_T("PROJECT_FILENAME")] = m_ProjectFilename;
        macros[_T("PROJECT_FILE_NAME")] = m_ProjectFilename;
        macros[_T("PROJECTFILE")]  = m_ProjectFilename;
        macros[_T("PROJECTFILENAME")] = m_ProjectFilename;
        macros[_T("PROJECTNAME")]  = m_ProjectName;
        macros[_T("PROJECT_NAME")]  = m_ProjectName;
        macros[_T("PROJECT_DIR")]  = m_ProjectDir;
        macros[_T("PROJECT_DIRECTORY")] = m_ProjectDir;
        macros[_T("PROJECTDIR")]  = m_ProjectDir;
        macros[_T("PROJECTDIRECTORY")] = m_ProjectDir;
        macros[_T("MAKEFILE")]   = m_Makefile;
        macros[_T("ALL_PROJECT_FILES")] = m_ProjectFiles;

        for (int i = 0; i < project->GetBuildTargetsCount(); ++i)
        {
            ProjectBuildTarget* target = project->GetBuildTarget(i);
            if (!target)
                continue;
            wxString title = target->GetTitle().Upper();
            macros[title + _T("_OUTPUT_FILE")] = UnixFilename(target->GetOutputFilename());
            macros[title + _T("_OUTPUT_DIR")] = UnixFilename(target->GetBasePath());
        }
        m_lastProject = project;

        VarsArray vars = project->GetCustomVars().GetVars();

        for(size_t i = 0; i < vars.GetCount(); ++i)
        {
            macros[vars[i].name.Upper()] = vars[i].value.Upper();
        }
    }

    if(!target)
    {
        m_TargetOutputDir = wxEmptyString;
        m_TargetName = wxEmptyString;
        m_lastTarget = NULL;
    }
    else if(target != m_lastTarget)
    {
        m_TargetOutputDir = UnixFilename(target->GetBasePath());
        m_TargetName = UnixFilename(target->GetTitle());
        m_lastTarget = target;
    }

    macros[_T("TARGET_OUTPUT_DIR")]   = m_TargetOutputDir;
    macros[_T("TARGET_NAME")]    = m_TargetName;
    macros[_T("ACTIVE_EDITOR_FILENAME")] = m_ActiveEditorFilename;

    macros[_T("LANGUAGE")]  = wxLocale::GetLanguageName(wxLocale::GetSystemLanguage());
    macros[_T("ENCODING")]  = wxLocale::GetSystemEncodingName();
    macros[_T("TDAY")]   = wxDateTime::Now().Format(_T("%Y%m%d"));
    macros[_T("TODAY")]   = wxDateTime::Now().Format(_T("%Y-%m-%d"));
    macros[_T("NOW")]   = wxDateTime::Now().Format(_T("%Y-%m-%d-%H.%M"));
    macros[_T("NOW_L")]   = wxDateTime::Now().Format(_T("%Y-%m-%d-%H.%M.%S"));
    macros[_T("WEEKDAY")]  = wxDateTime::Now().Format(_T("%A"));
    macros[_T("TDAY_UTC")]  = wxDateTime::Now().ToGMT().Format(_T("%Y%m%d"));
    macros[_T("TODAY_UTC")]  = wxDateTime::Now().ToGMT().Format(_T("%Y-%m-%d"));
    macros[_T("NOW_UTC")]  = wxDateTime::Now().ToGMT().Format(_T("%Y-%m-%d-%H.%M"));
    macros[_T("NOW_L_UTC")]  = wxDateTime::Now().ToGMT().Format(_T("%Y-%m-%d-%H.%M.%S"));
    macros[_T("WEEKDAY_UTC")] = wxDateTime::Now().ToGMT().Format(_T("%A"));
}

void MacrosManager::ReplaceMacros(wxString& buffer, bool envVarsToo)
{
    SANITY_CHECK();

    if (buffer.IsEmpty())
        return;

    cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
    EditorBase* editor = Manager::Get()->GetEditorManager()->GetActiveEditor();
    ProjectBuildTarget* target = project ? project->GetCurrentlyCompilingTarget() : 0;

    if(project != m_lastProject || editor != m_lastEditor || target != m_lastTarget)
        RecalcVars(project, editor, target);

    wxString replace;
    wxString before;

    while(m_re.Matches(buffer))
    {
        replace.Empty();

        wxString env = m_re.GetMatch(buffer, 2).Upper();
        before = m_re.GetMatch(buffer, 0);

        if (env[0] == _T('#'))
            replace = UnixFilename(uVarMan->Replace(env));
        else
        {
            MacrosMap::iterator it;
            if((it = macros.find(env)) != macros.end())
                replace = it->second;
        }

        QuoteStringIfNeeded(replace);

        if (!replace.IsEmpty())
        {
            buffer.Replace(before, replace);
        }
        else
        {
            if (envVarsToo)
            {
                wxString envactual;
                wxGetEnv(env, &envactual);
                buffer.Replace(before, envactual);
            }
        }
    }
}

void MacrosManager::ReplaceEnvVars(wxString& buffer)
{
    ReplaceMacros(buffer, true);
}
