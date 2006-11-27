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
    #include <wx/menu.h>

    #include "projectmanager.h"
    #include "editormanager.h"
    #include "messagemanager.h"
    #include "macrosmanager.h"
    #include "manager.h"
    #include "cbproject.h"
    #include "cbeditor.h"
    #include "uservarmanager.h"
    #include "configmanager.h"
    #include "globals.h"
    #include "compilerfactory.h"
    #include "compiler.h"
#endif

#include <cstdlib>

using namespace std;


static const wxString const_COIN(_T("COIN"));
static const wxString const_RANDOM(_T("RANDOM"));


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

MacrosManager::MacrosManager()
{
    Reset();
}

MacrosManager::~MacrosManager()
{
}

void MacrosManager::CreateMenu(wxMenuBar* menuBar)
{
}

void MacrosManager::ReleaseMenu(wxMenuBar* menuBar)
{
}

wxString MacrosManager::ReplaceMacros(const wxString& buffer, ProjectBuildTarget* target)
{
    wxString tmp = buffer;
    ReplaceMacros(tmp, target);
    return tmp;
}

void MacrosManager::Reset()
{
    m_lastProject = 0;
    m_lastTarget = 0;
    m_lastEditor = 0;

    m_AppPath = UnixFilename(ConfigManager::GetExecutableFolder());
    m_Plugins = UnixFilename(ConfigManager::GetPluginsFolder());
    m_DataPath = UnixFilename(ConfigManager::GetDataFolder());
    ClearProjectKeys();
    m_re_unx.Compile(_T("([^$]|^)(\\$[({]?(#?[A-Za-z_0-9.]+)[)} /\\]?)"));
    m_re_dos.Compile(_T("([^%]|^)(%(#?[A-Za-z_0-9.]+)%)"));
    m_re_if.Compile(_T("\\$if\\((.*)\\)[ ]*\\{([^}]*)\\}{1}([ ]*else[ ]*\\{([^}]*)\\})?"));
    m_re_ifsp.Compile(_T("[^=!<>]+|(([^=!<>]+)[ ]*(=|==|!=|>|<|>=|<=)[ ]*([^=!<>]+))"));
    m_re_path.Compile(_T("([^$]|^)(\\$(absolute|relative)\\(([^)]+)\\))"));
    m_uVarMan = Manager::Get()->GetUserVariableManager();
    srand(time(0));
    assert(m_re_unx.IsValid());
    assert(m_re_dos.IsValid());
}

void MacrosManager::ClearProjectKeys()
{
//	Manager::Get()->GetMessageManager()->DebugLog(_T("clear"));
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
	macros[_T("LANGUAGE")]  = wxLocale::GetLanguageName(wxLocale::GetSystemLanguage());
	macros[_T("ENCODING")]  = wxLocale::GetSystemEncodingName();

}

void MacrosManager::RecalcVars(cbProject* project,EditorBase* editor,ProjectBuildTarget* target)
{
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
//		Manager::Get()->GetMessageManager()->DebugLog("project == 0");
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
        macros[_T("PROJECT_TOPDIR")]  = wxEmptyString;
        macros[_T("PROJECT_TOPDIRECTORY")] = wxEmptyString;
        macros[_T("PROJECTTOPDIR")]  = wxEmptyString;
        macros[_T("PROJECTTOPDIRECTORY")] = wxEmptyString;
        macros[_T("MAKEFILE")]   = wxEmptyString;
        macros[_T("ALL_PROJECT_FILES")] = wxEmptyString;
    }
    else if(project != m_lastProject)
    {
//		Manager::Get()->GetMessageManager()->DebugLog("project != m_lastProject");
        m_lastTarget = NULL; // reset last target when project changes
        m_prjname.Assign(project->GetFilename());
        m_ProjectFilename = UnixFilename(m_prjname.GetFullName());
        m_ProjectName = project->GetTitle();
        m_ProjectDir = UnixFilename(project->GetBasePath());
        m_ProjectTopDir = UnixFilename(project->GetCommonTopLevelPath());
        m_Makefile = UnixFilename(project->GetMakefile());
        m_ProjectFiles = wxEmptyString;
        for (int i = 0; i < project->GetFilesCount(); ++i)
        {
            // quote filenames, if they contain spaces
            wxString out = UnixFilename(project->GetFile(i)->relativeFilename);
            QuoteStringIfNeeded(out);
            m_ProjectFiles << out << _T(' ');
        }

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
        macros[_T("PROJECT_TOPDIR")]  = m_ProjectTopDir;
        macros[_T("PROJECT_TOPDIRECTORY")] = m_ProjectTopDir;
        macros[_T("PROJECTTOPDIR")]  = m_ProjectTopDir;
        macros[_T("PROJECTTOPDIRECTORY")] = m_ProjectTopDir;
        macros[_T("MAKEFILE")]   = m_Makefile;
        macros[_T("ALL_PROJECT_FILES")] = m_ProjectFiles;

        for (int i = 0; i < project->GetBuildTargetsCount(); ++i)
        {
            ProjectBuildTarget* target = project->GetBuildTarget(i);
            if (!target)
                continue;
            wxString title = target->GetTitle().Upper();
            while (title.Replace(_T(" "), _T("_")))
                ; // replace spaces with underscores (what about other invalid chars?)
            macros[title + _T("_OUTPUT_FILE")] = UnixFilename(target->GetOutputFilename());
            macros[title + _T("_OUTPUT_DIR")] = UnixFilename(target->GetBasePath());
            macros[title + _T("_OUTPUT_BASENAME")] = wxFileName(target->GetOutputFilename()).GetName();
        }
        m_lastProject = project;
    }

    if(target)
    {
        const Compiler* compiler = CompilerFactory::GetCompiler(target->GetCompilerID());
        if(compiler)
        {
            const StringHash& v = compiler->GetAllVars();
            for (StringHash::const_iterator it = v.begin(); it != v.end(); ++it)
            {
                macros[it->first.Upper()] = it->second;
            }
        }
    }

	if(project)
	{
        const StringHash& v = project->GetAllVars();
        for (StringHash::const_iterator it = v.begin(); it != v.end(); ++it)
        {
            macros[it->first.Upper()] = it->second;
        }
	}

    if(!target)
    {
        m_TargetOutputDir = wxEmptyString;
        m_TargetName = wxEmptyString;
        m_TargetOutputBaseName = wxEmptyString;
        m_TargetFilename = wxEmptyString;
        m_lastTarget = NULL;
    }
    else if(target != m_lastTarget)
    {
        m_TargetOutputDir = UnixFilename(target->GetBasePath());
        m_TargetName = UnixFilename(target->GetTitle());
        m_TargetOutputBaseName = wxFileName(target->GetOutputFilename()).GetName();
        m_TargetFilename = UnixFilename(target->GetOutputFilename());
        m_lastTarget = target;
    }

	if(target)
	{
        const StringHash& v = target->GetAllVars();
        for (StringHash::const_iterator it = v.begin(); it != v.end(); ++it)
        {
            macros[it->first.Upper()] = it->second;
        }
	}

    Compiler *c = CompilerFactory::GetCompiler(target->GetCompilerID());
    macros[_T("TARGET_CC")]   = c->GetPrograms().C;
    macros[_T("TARGET_CPP")]   = c->GetPrograms().CPP;
    macros[_T("TARGET_LD")]   = c->GetPrograms().LD;
    macros[_T("TARGET_LIB")]   = c->GetPrograms().LIB;
    macros[_T("TARGET_OUTPUT_DIR")]   = m_TargetOutputDir;
    macros[_T("TARGET_NAME")]    = m_TargetName;
    macros[_T("TARGET_OUTPUT_BASENAME")]    = m_TargetOutputBaseName;
    macros[_T("TARGET_OUTPUT_FILE")]    = m_TargetFilename;
    macros[_T("ACTIVE_EDITOR_FILENAME")] = m_ActiveEditorFilename;
    wxFileName fn(m_ActiveEditorFilename);
    macros[_T("ACTIVE_EDITOR_DIRNAME")]  = fn.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    macros[_T("ACTIVE_EDITOR_STEM")]  = fn.GetName();
    macros[_T("ACTIVE_EDITOR_EXT")]  = fn.GetExt();
	wxDateTime now(wxDateTime::Now());
	wxDateTime nowGMT(now.ToGMT());

    macros[_T("TDAY")]   = now.Format(_T("%Y%m%d"));
    macros[_T("TODAY")]   = now.Format(_T("%Y-%m-%d"));
    macros[_T("NOW")]   = now.Format(_T("%Y-%m-%d-%H.%M"));
    macros[_T("NOW_L")]   = now.Format(_T("%Y-%m-%d-%H.%M.%S"));
    macros[_T("WEEKDAY")]  = now.Format(_T("%A"));
    macros[_T("TDAY_UTC")]  = nowGMT.Format(_T("%Y%m%d"));
    macros[_T("TODAY_UTC")]  = nowGMT.Format(_T("%Y-%m-%d"));
    macros[_T("NOW_UTC")]  = nowGMT.Format(_T("%Y-%m-%d-%H.%M"));
    macros[_T("NOW_L_UTC")]  = nowGMT.Format(_T("%Y-%m-%d-%H.%M.%S"));
    macros[_T("WEEKDAY_UTC")] = nowGMT.Format(_T("%A"));
}

void MacrosManager::ReplaceMacros(wxString& buffer, ProjectBuildTarget* target, bool subrequest)
{
    if (buffer.IsEmpty())
        return;
    if( buffer.find(_T('$')) == wxString::npos && buffer.find(_T('%')) == wxString::npos )
        return;

    cbProject* project = target
                        ? target->GetParentProject()
                        : Manager::Get()->GetProjectManager()->GetActiveProject();
    EditorBase* editor = Manager::Get()->GetEditorManager()->GetActiveEditor();

    if(!target)
    {
        if (project)
        {
            // use the currently compiling target
            target = project->GetCurrentlyCompilingTarget();
            // if none,
            if (!target)
                // use the last known active target
                target = project->GetBuildTarget(project->GetActiveBuildTarget());
        }
    }

    if(project != m_lastProject || target != m_lastTarget || editor != m_lastEditor)
        RecalcVars(project, editor, target);

    wxString search;
    wxString replace;

    if(buffer.find(_T("$if")) != wxString::npos)
    while(m_re_if.Matches(buffer))
    {
        search = m_re_if.GetMatch(buffer, 0);
        replace = EvalCondition(m_re_if.GetMatch(buffer, 1), m_re_if.GetMatch(buffer, 2), m_re_if.GetMatch(buffer, 4), target);
        buffer.Replace(search, replace, false);
    }

    while(m_re_path.Matches(buffer))
    {
        search = m_re_path.GetMatch(buffer, 2);
        replace = m_re_path.GetMatch(buffer, 4);
        wxString abs_rel(m_re_path.GetMatch(buffer, 3).Lower());
        ReplaceMacros(replace, target, true);

        {
        wxLogNull shit;
        wxFileName f(replace);
        if(abs_rel.IsSameAs(_T("relative")))
            f.MakeRelativeTo(m_ProjectDir);
        else
            f.MakeAbsolute();
        replace = f.GetFullPath();
        }

        buffer.Replace(search, replace, false);
    }

    while(m_re_unx.Matches(buffer))
    {
        replace.Empty();

        wxString search = m_re_unx.GetMatch(buffer, 2);
        wxString var = m_re_unx.GetMatch(buffer, 3).Upper();

        if (var.GetChar(0) == _T('#'))
        {
            replace = UnixFilename(m_uVarMan->Replace(var));
        }
        else
        {
            if(var.compare(const_COIN) == 0)
                replace.assign(1u, rand() & 1 ? _T('1') : _T('0'));
            else if(var.compare(const_RANDOM) == 0)
                replace = wxString::Format(_T("%d"), rand() & 0xffff);
            else
            {
                MacrosMap::iterator it;
                if((it = macros.find(var)) != macros.end())
                    replace = it->second;
            }
        }

        const wxChar l = search.Last(); // make non-braced variables work
        if(l == _T('/') || l == _T('\\') || l == _T('$') || l == _T(' '))
            replace.append(l);

        if (replace.IsEmpty())
            wxGetEnv(var, &replace);

        buffer.Replace(search, replace, false);
    }

    while(m_re_dos.Matches(buffer))
    {
        replace.Empty();

        wxString search = m_re_dos.GetMatch(buffer, 1);
        wxString var = m_re_dos.GetMatch(buffer, 2).Upper();

        if (var.GetChar(0) == _T('#'))
        {
            replace = UnixFilename(m_uVarMan->Replace(var));
        }
        else
        {
            if(var.compare(const_COIN) == 0)
                replace.assign(1u, rand() & 1 ? _T('1') : _T('0'));
            else if(var.compare(const_RANDOM) == 0)
                replace = wxString::Format(_T("%d"), rand() & 0xffff);
            else
            {
                MacrosMap::iterator it;
                if((it = macros.find(var)) != macros.end())
                    replace = it->second;
            }
        }

        if (replace.IsEmpty())
            wxGetEnv(var, &replace);

        buffer.Replace(search, replace, false);
    }


    if(!subrequest)
    {
        buffer.Replace(_T("%%"), _T("%"));
        buffer.Replace(_T("$$"), _T("$"));
    }
}

wxString MacrosManager::EvalCondition(const wxString& in_cond, const wxString& true_clause, const wxString& false_clause, ProjectBuildTarget* target)
{
    enum condition_codes {EQ = 1, LT = 2, GT = 4, NE = 8};

    wxString cond(in_cond);
    wxString result;

    ReplaceMacros(cond, target, true);

    if(!m_re_ifsp.Matches(in_cond))
        return false_clause;


    wxString cmpToken(m_re_ifsp.GetMatch(in_cond, 3).Strip(wxString::both));
    wxString left(m_re_ifsp.GetMatch(in_cond, 2).Strip(wxString::both));
    wxString right(m_re_ifsp.GetMatch(in_cond, 4).Strip(wxString::both));


    int compare = left.Cmp(right);
    if(compare == 0)
        compare = EQ;
    else if(compare < 0)
        compare = LT | NE;
    else if(compare > 0)
        compare = GT | NE;


    if(cmpToken.IsEmpty())
        {
        wxString s(m_re_ifsp.GetMatch(in_cond, 0));
        if(s.IsEmpty() || s.IsSameAs(_T("0")) || s.IsSameAs(_T("false")))
            return false_clause;
        return true_clause;
        }

    int condCode = 0;

    if(cmpToken.IsSameAs(_T("==")) || cmpToken.IsSameAs(_T("=")))
        condCode = EQ;
    if(cmpToken.IsSameAs(_T("<")))
        condCode = LT;
    if(cmpToken.IsSameAs(_T(">")))
        condCode = GT;
    if(cmpToken.IsSameAs(_T("<=")))
        condCode = EQ | LT;
    if(cmpToken.IsSameAs(_T(">=")))
        condCode = EQ | GT;
    if(cmpToken.IsSameAs(_T("!=")))
        condCode = NE;

    return condCode & compare ? true_clause : false_clause;
}
