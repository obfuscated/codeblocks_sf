/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/menu.h>

    #include "cbworkspace.h"
    #include "projectmanager.h"
    #include "editormanager.h"
    #include "logmanager.h"
    #include "macrosmanager.h"
    #include "scriptingmanager.h"
    #include "manager.h"
    #include "cbproject.h"
    #include "cbeditor.h"
    #include "uservarmanager.h"
    #include "configmanager.h"
    #include "globals.h"
    #include "compilerfactory.h"
    #include "compiler.h"
#endif

#include <wx/stdpaths.h> // wxStandardPaths
#include <cstdlib>

#include "scripting/sqplus/sqplus.h"
#include "scripting/bindings/scriptbindings.h"

#include "cbstyledtextctrl.h"

using namespace std;

template<> MacrosManager* Mgr<MacrosManager>::instance = nullptr;
template<> bool  Mgr<MacrosManager>::isShutdown = false;

static const wxString const_COIN(_T("COIN"));
static const wxString const_RANDOM(_T("RANDOM"));
static const wxString toNativePath(_T("$TO_NATIVE_PATH{"));
static const wxString toUnixPath(_T("$TO_UNIX_PATH{"));
static const wxString toWindowsPath(_T("$TO_WINDOWS_PATH{"));

MacrosManager::MacrosManager()
{
    Reset();
}

MacrosManager::~MacrosManager()
{
}

void MacrosManager::CreateMenu(cb_unused wxMenuBar* menuBar)
{
}

void MacrosManager::ReleaseMenu(cb_unused wxMenuBar* menuBar)
{
}

wxString MacrosManager::ReplaceMacros(const wxString& buffer, ProjectBuildTarget* target)
{
    wxString tmp(buffer);
    ReplaceMacros(tmp, target);
    return tmp;
}

void MacrosManager::Reset()
{
    m_LastProject          = nullptr;
    m_LastTarget           = nullptr;
    m_ActiveEditorFilename = wxEmptyString;
    m_ActiveEditorLine     = -1;
    m_ActiveEditorColumn   = -1;

    m_AppPath  = UnixFilename(ConfigManager::GetExecutableFolder());
    m_Plugins  = UnixFilename(ConfigManager::GetPluginsFolder());
    m_DataPath = UnixFilename(ConfigManager::GetDataFolder());
    ClearProjectKeys();
    m_RE_Unix.Compile(_T("([^$]|^)(\\$[({]?(#?[A-Za-z_0-9.]+)[)} /\\]?)"),               wxRE_EXTENDED | wxRE_NEWLINE);
    m_RE_DOS.Compile(_T("([^%]|^)(%(#?[A-Za-z_0-9.]+)%)"),                               wxRE_EXTENDED | wxRE_NEWLINE);
    m_RE_If.Compile(_T("\\$if\\(([^)]*)\\)[::space::]*(\\{([^}]*)\\})(\\{([^}]*)\\})?"), wxRE_EXTENDED | wxRE_NEWLINE);
    m_RE_IfSp.Compile(_T("[^=!<>]+|(([^=!<>]+)[ ]*(=|==|!=|>|<|>=|<=)[ ]*([^=!<>]+))"),  wxRE_EXTENDED | wxRE_NEWLINE);
    m_RE_Script.Compile(_T("(\\[\\[(.*)\\]\\])"),                                        wxRE_EXTENDED | wxRE_NEWLINE);
    m_RE_ToAbsolutePath.Compile(_T("\\$TO_ABSOLUTE_PATH{([^}]*)}"),
#ifndef __WXMAC__
                                wxRE_ADVANCED);
#else
                                wxRE_EXTENDED);
#endif
    m_RE_To83Path.Compile(_T("\\$TO_83_PATH{([^}]*)}"),
#ifndef __WXMAC__
                                wxRE_ADVANCED);
#else
                                wxRE_EXTENDED);
#endif
    m_RE_RemoveQuotes.Compile(_T("\\$REMOVE_QUOTES{([^}]*)}"),
#ifndef __WXMAC__
                                wxRE_ADVANCED);
#else
                                wxRE_EXTENDED);
#endif
    m_UserVarMan = Manager::Get()->GetUserVariableManager();
    srand(time(nullptr));
    assert(m_RE_Unix.IsValid());
    assert(m_RE_DOS.IsValid());
}

void MacrosManager::ClearProjectKeys()
{
    m_Macros.clear();

    m_Macros[_T("AMP")]        = _T("&");
    m_Macros[_T("CODEBLOCKS")] = m_AppPath;
    m_Macros[_T("APP_PATH")]   = m_AppPath;
    m_Macros[_T("APP-PATH")]   = m_AppPath;
    m_Macros[_T("APPPATH")]    = m_AppPath;
    m_Macros[_T("DATA_PATH")]  = m_DataPath;
    m_Macros[_T("DATA-PATH")]  = m_DataPath;
    m_Macros[_T("DATAPATH")]   = m_DataPath;
    m_Macros[_T("PLUGINS")]    = m_Plugins;
    m_Macros[_T("LANGUAGE")]   = wxLocale::GetLanguageName(wxLocale::GetSystemLanguage());
    m_Macros[_T("ENCODING")]   = wxLocale::GetSystemEncodingName();

    if (platform::windows)
    {
        m_Macros[_T("CMD_NULL")]  = _T("NUL");

        const wxString cmd(_T("cmd /c "));
        m_Macros[_T("CMD_CP")]    = cmd + _T("copy");
        m_Macros[_T("CMD_RM")]    = cmd + _T("del");
        m_Macros[_T("CMD_MV")]    = cmd + _T("move");
        m_Macros[_T("CMD_MKDIR")] = cmd + _T("md");
        m_Macros[_T("CMD_RMDIR")] = cmd + _T("rd");
    }
    else
    {
        m_Macros[_T("CMD_CP")]     = _T("cp --preserve=timestamps");
        m_Macros[_T("CMD_RM")]     = _T("rm");
        m_Macros[_T("CMD_MV")]     = _T("mv");
        m_Macros[_T("CMD_NULL")]   = _T("/dev/null");
        m_Macros[_T("CMD_MKDIR")]  = _T("mkdir -p");
        m_Macros[_T("CMD_RMDIR")]  = _T("rmdir");
    }

    cbWorkspace* wksp = Manager::Get()->GetProjectManager()->GetWorkspace();
    if (wksp)
    {
        m_WorkspaceWxFileName.Assign(wksp->GetFilename());
        m_WorkspaceFilename = UnixFilename(m_WorkspaceWxFileName.GetFullName());
        m_WorkspaceName = wksp->GetTitle();
        m_WorkspaceDir = UnixFilename(m_WorkspaceWxFileName.GetPath());
    }
    else
    {
        m_WorkspaceWxFileName.Clear();
        m_WorkspaceFilename = wxEmptyString;
        m_WorkspaceName = wxEmptyString;
        m_WorkspaceDir = wxEmptyString;
    }
    m_Macros[_T("WORKSPACE_FILE")]      = m_WorkspaceFilename;
    m_Macros[_T("WORKSPACE_FILENAME")]  = m_WorkspaceFilename;
    m_Macros[_T("WORKSPACE_FILE_NAME")] = m_WorkspaceFilename;
    m_Macros[_T("WORKSPACEFILE")]       = m_WorkspaceFilename;
    m_Macros[_T("WORKSPACEFILENAME")]   = m_WorkspaceFilename;
    m_Macros[_T("WORKSPACENAME")]       = m_WorkspaceName;
    m_Macros[_T("WORKSPACE_NAME")]      = m_WorkspaceName;
    m_Macros[_T("WORKSPACE_DIR")]       = m_WorkspaceDir;
    m_Macros[_T("WORKSPACE_DIRECTORY")] = m_WorkspaceDir;
    m_Macros[_T("WORKSPACEDIR")]        = m_WorkspaceDir;
    m_Macros[_T("WORKSPACEDIRECTORY")]  = m_WorkspaceDir;
}

wxString GetSelectedText()
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
    {
        cbStyledTextCtrl* stc = ed->GetControl();
        if (stc)
        {
            wxString text = stc->GetSelectedText();
            if ( text.size() )
                return text;
            else
            {
                int iCurrentPos = stc->GetCurrentPos();
                return stc->GetTextRange(stc->WordStartPosition(iCurrentPos, true),
                                         stc->WordEndPosition(iCurrentPos, true));
            }
        }
    }

    return wxEmptyString;
}

namespace
{
template<typename T>
void ReadMacros(MacrosMap &macros, T *object)
{
    if (object)
    {
        const StringHash& v = object->GetAllVars();
        for (StringHash::const_iterator it = v.begin(); it != v.end(); ++it)
            macros[it->first.Upper()] = it->second;
    }
}
} // namespace

void MacrosManager::RecalcVars(cbProject* project, EditorBase* editor, ProjectBuildTarget* target)
{
    m_ActiveEditorFilename = wxEmptyString; // invalidate
    m_ActiveEditorLine     = -1;            // invalidate
    m_ActiveEditorColumn   = -1;            // invalidate

    if (editor)
    {
      // don't use pointer to editor here, because this might be the same,
      // even after closing one file and opening a new one
      if (editor->GetFilename() != m_ActiveEditorFilename)
          m_ActiveEditorFilename = editor->GetFilename();

      // (re-) compute column and line but only in case it's a builtin-editor
      if (editor->IsBuiltinEditor())
      {
          cbEditor*         cbEd  = static_cast<cbEditor*>(editor);
          cbStyledTextCtrl* cbSTC = cbEd->GetControl();
          if (cbSTC)
          {
              m_ActiveEditorLine = cbSTC->GetCurrentLine() + 1;
              int pos = cbSTC->GetCurrentPos();
              if (pos!=-1)
                  m_ActiveEditorColumn = cbSTC->GetColumn(pos) + 1;
          }
      }
    }

    if (!project)
    {
        m_ProjectFilename = wxEmptyString;
        m_ProjectName     = wxEmptyString;
        m_ProjectDir      = wxEmptyString;
        m_ProjectFiles    = wxEmptyString;
        m_Makefile        = wxEmptyString;
        m_LastProject     = nullptr;
        ClearProjectKeys();
        m_Macros[_T("PROJECTFILE")]          = wxEmptyString;
        m_Macros[_T("PROJECT_FILE")]         = wxEmptyString;
        m_Macros[_T("PROJECTFILENAME")]      = wxEmptyString;
        m_Macros[_T("PROJECT_FILENAME")]     = wxEmptyString;
        m_Macros[_T("PROJECT_FILE_NAME")]    = wxEmptyString;
        m_Macros[_T("PROJECTNAME")]          = wxEmptyString;
        m_Macros[_T("PROJECT_NAME")]         = wxEmptyString;
        m_Macros[_T("PROJECTDIR")]           = wxEmptyString;
        m_Macros[_T("PROJECT_DIR")]          = wxEmptyString;
        m_Macros[_T("PROJECTDIRECTORY")]     = wxEmptyString;
        m_Macros[_T("PROJECT_DIRECTORY")]    = wxEmptyString;
        m_Macros[_T("PROJECTTOPDIR")]        = wxEmptyString;
        m_Macros[_T("PROJECT_TOPDIR")]       = wxEmptyString;
        m_Macros[_T("PROJECTTOPDIRECTORY")]  = wxEmptyString;
        m_Macros[_T("PROJECT_TOPDIRECTORY")] = wxEmptyString;
        m_Macros[_T("MAKEFILE")]             = wxEmptyString;
        m_Macros[_T("ALL_PROJECT_FILES")]    = wxEmptyString;
    }
    else if ( (project != m_LastProject) || (project->GetTitle() != m_ProjectName)
                || (UnixFilename(project->GetBasePath()) != m_ProjectDir)
                || (UnixFilename(m_ProjectWxFileName.GetFullName()) != m_ProjectFilename)
             )
    {
        m_LastTarget      = nullptr; // reset last target when project changes
        m_ProjectWxFileName.Assign(project->GetFilename());
        m_ProjectFilename = UnixFilename(m_ProjectWxFileName.GetFullName());
        m_ProjectName     = project->GetTitle();
        m_ProjectDir      = UnixFilename(project->GetBasePath());
        m_ProjectTopDir   = UnixFilename(project->GetCommonTopLevelPath());
        m_Makefile        = UnixFilename(project->GetMakefile());
        m_ProjectFiles    = wxEmptyString;
        for (FilesList::iterator it = project->GetFilesList().begin(); it != project->GetFilesList().end(); ++it)
        {
            // quote filenames, if they contain spaces
            wxString out = UnixFilename(((ProjectFile*)*it)->relativeFilename);
            QuoteStringIfNeeded(out);
            m_ProjectFiles << out << _T(' ');
        }

        ClearProjectKeys();
        m_Macros[_T("PROJECTFILE")]          = m_ProjectFilename;
        m_Macros[_T("PROJECT_FILE")]         = m_ProjectFilename;
        m_Macros[_T("PROJECTFILENAME")]      = m_ProjectFilename;
        m_Macros[_T("PROJECT_FILENAME")]     = m_ProjectFilename;
        m_Macros[_T("PROJECT_FILE_NAME")]    = m_ProjectFilename;
        m_Macros[_T("PROJECTNAME")]          = m_ProjectName;
        m_Macros[_T("PROJECT_NAME")]         = m_ProjectName;
        m_Macros[_T("PROJECTDIR")]           = m_ProjectDir;
        m_Macros[_T("PROJECT_DIR")]          = m_ProjectDir;
        m_Macros[_T("PROJECTDIRECTORY")]     = m_ProjectDir;
        m_Macros[_T("PROJECT_DIRECTORY")]    = m_ProjectDir;
        m_Macros[_T("PROJECTTOPDIR")]        = m_ProjectTopDir;
        m_Macros[_T("PROJECT_TOPDIR")]       = m_ProjectTopDir;
        m_Macros[_T("PROJECTTOPDIRECTORY")]  = m_ProjectTopDir;
        m_Macros[_T("PROJECT_TOPDIRECTORY")] = m_ProjectTopDir;
        m_Macros[_T("MAKEFILE")]             = m_Makefile;
        m_Macros[_T("ALL_PROJECT_FILES")]    = m_ProjectFiles;

        for (int i = 0; i < project->GetBuildTargetsCount(); ++i)
        {
            ProjectBuildTarget* it_target = project->GetBuildTarget(i);
            if (!it_target)
                continue;
            wxString title = it_target->GetTitle().Upper();
            while (title.Replace(_T(" "), _T("_")))
                ; // replace spaces with underscores (what about other invalid chars?)
            m_Macros[title + _T("_OUTPUT_FILE")]     = UnixFilename(it_target->GetOutputFilename());
            m_Macros[title + _T("_OUTPUT_DIR")]      = UnixFilename(it_target->GetBasePath());
            m_Macros[title + _T("_OUTPUT_BASENAME")] = wxFileName(it_target->GetOutputFilename()).GetName();
            m_Macros[title + _T("_PARAMETERS")]      = it_target->GetExecutionParameters();
        }
        m_LastProject = project;
    }

    // get compiler variables
    if (target)
    {
        const Compiler* compiler = CompilerFactory::GetCompiler(target->GetCompilerID());
        ReadMacros(m_Macros, compiler);
    }

    ReadMacros(m_Macros, project);

    if (!target)
    {
        m_TargetOutputDir      = wxEmptyString;
        m_TargetName           = wxEmptyString;
        m_TargetOutputBaseName = wxEmptyString;
        m_TargetOutputFilename = wxEmptyString;
        m_TargetFilename       = wxEmptyString;
        m_LastTarget           = nullptr;
    }
    else if ( (target != m_LastTarget) or (target->GetTitle() != m_TargetName) )
    {
        wxFileName tod(target->GetOutputFilename());
        m_TargetOutputDir      = UnixFilename(tod.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR));
        m_TargetName           = UnixFilename(target->GetTitle());
        m_TargetOutputBaseName = wxFileName(target->GetOutputFilename()).GetName();
        m_TargetOutputFilename = wxFileName(target->GetOutputFilename()).GetFullName();
        m_TargetFilename       = UnixFilename(target->GetOutputFilename());
        m_LastTarget           = target;
    }

    if (target)
    {
        ReadMacros(m_Macros, target);

        if (Compiler* c = CompilerFactory::GetCompiler(target->GetCompilerID()))
        {
            m_Macros[_T("TARGET_CC")]           = c->GetPrograms().C;
            m_Macros[_T("TARGET_CPP")]          = c->GetPrograms().CPP;
            m_Macros[_T("TARGET_LD")]           = c->GetPrograms().LD;
            m_Macros[_T("TARGET_LIB")]          = c->GetPrograms().LIB;
            wxFileName MasterPath;
            MasterPath.SetPath(c->GetMasterPath(), wxPATH_NATIVE);
            m_Macros[_T("TARGET_COMPILER_DIR")] = MasterPath.GetPathWithSep(wxPATH_NATIVE);
        }
        m_Macros[_T("TARGET_OBJECT_DIR")] = target->GetObjectOutput();
    }

    m_Macros[_T("TARGET_OUTPUT_DIR")]      = m_TargetOutputDir;
    m_Macros[_T("TARGET_NAME")]            = m_TargetName;
    m_Macros[_T("TARGET_OUTPUT_BASENAME")] = m_TargetOutputBaseName;
    m_Macros[_T("TARGET_OUTPUT_FILENAME")] = m_TargetOutputFilename;
    m_Macros[_T("TARGET_OUTPUT_FILE")]     = m_TargetFilename;

    m_Macros[_T("ACTIVE_EDITOR_FILENAME")] = UnixFilename(m_ActiveEditorFilename);
    wxFileName fn(m_Macros[_T("ACTIVE_EDITOR_FILENAME")]);
    m_Macros[_T("ACTIVE_EDITOR_DIRNAME")]  = fn.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    m_Macros[_T("ACTIVE_EDITOR_STEM")]     = fn.GetName();
    m_Macros[_T("ACTIVE_EDITOR_EXT")]      = fn.GetExt();

    wxString temp_str;
    temp_str.Printf(_T("%d"), m_ActiveEditorLine);
    m_Macros[_T("ACTIVE_EDITOR_LINE")]     = temp_str;
    temp_str.Printf(_T("%d"), (m_ActiveEditorLine>0) ? m_ActiveEditorLine-1 : -1); // zero based, but keep value for "invalid"
    m_Macros[_T("ACTIVE_EDITOR_LINE_0")]   = temp_str;
    temp_str.Printf(_T("%d"), m_ActiveEditorColumn);
    m_Macros[_T("ACTIVE_EDITOR_COLUMN")]   = temp_str;
    temp_str.Printf(_T("%d"), (m_ActiveEditorColumn>0) ? m_ActiveEditorColumn-1 : -1); // zero based, but keep value for "invalid"
    m_Macros[_T("ACTIVE_EDITOR_COLUMN_0")] = temp_str;
    m_Macros[_T("ACTIVE_EDITOR_SELECTION")] = GetSelectedText();

    // Wrapper for WX standard path's methods:

    // Unix: prefix/share/appname   Windows: EXE path
    m_Macros[_T("GET_DATA_DIR")]            = ((const wxStandardPaths&)wxStandardPaths::Get()).GetDataDir();
    // Unix: /etc/appname   Windows: EXE path
    m_Macros[_T("GET_LOCAL_DATA_DIR")]      = ((const wxStandardPaths&)wxStandardPaths::Get()).GetLocalDataDir();
    // Unix: ~   Windows: C:\Documents and Settings\username\Documents
    m_Macros[_T("GET_DOCUMENTS_DIR")]       = ((const wxStandardPaths&)wxStandardPaths::Get()).GetDocumentsDir();
    // Unix: /etc   Windows: C:\Documents and Settings\All Users\Application Data
    m_Macros[_T("GET_CONFIG_DIR")]          = ((const wxStandardPaths&)wxStandardPaths::Get()).GetConfigDir();
    // Unix: ~   Windows: C:\Documents and Settings\username\Application Data\appname
    m_Macros[_T("GET_USER_CONFIG_DIR")]     = ((const wxStandardPaths&)wxStandardPaths::Get()).GetUserConfigDir();
    // Unix: ~/.appname   Windows: C:\Documents and Settings\username\Application Data
    m_Macros[_T("GET_USER_DATA_DIR")]       = ((const wxStandardPaths&)wxStandardPaths::Get()).GetUserDataDir();
    // Unix: ~/.appname   Windows: C:\Documents and Settings\username\Local Settings\Application Data\appname
    m_Macros[_T("GET_USER_LOCAL_DATA_DIR")] = ((const wxStandardPaths&)wxStandardPaths::Get()).GetUserLocalDataDir();
    // ALl platforms: A writable, temporary directory
    m_Macros[_T("GET_TEMP_DIR")]            = ((const wxStandardPaths&)wxStandardPaths::Get()).GetTempDir();

    wxDateTime now(wxDateTime::Now());
    m_Macros[_T("TDAY")]        = now.Format(_T("%Y%m%d"));
    m_Macros[_T("TODAY")]       = now.Format(_T("%Y-%m-%d"));
    m_Macros[_T("NOW")]         = now.Format(_T("%Y-%m-%d-%H.%M"));
    m_Macros[_T("NOW_L")]       = now.Format(_T("%Y-%m-%d-%H.%M.%S"));
    m_Macros[_T("WEEKDAY")]     = now.Format(_T("%A"));

    wxDateTime nowGMT(now.ToGMT());
    m_Macros[_T("TDAY_UTC")]    = nowGMT.Format(_T("%Y%m%d"));
    m_Macros[_T("TODAY_UTC")]   = nowGMT.Format(_T("%Y-%m-%d"));
    m_Macros[_T("NOW_UTC")]     = nowGMT.Format(_T("%Y-%m-%d-%H.%M"));
    m_Macros[_T("NOW_L_UTC")]   = nowGMT.Format(_T("%Y-%m-%d-%H.%M.%S"));
    m_Macros[_T("WEEKDAY_UTC")] = nowGMT.Format(_T("%A"));

    wxDateTime january_1_2010(1, wxDateTime::Jan, 2010, 0, 0, 0, 0);
    wxTimeSpan ts = now.Subtract(january_1_2010);
    m_Macros[_T("DAYCOUNT")] = wxString::Format(_T("%d"), ts.GetDays());
}

void MacrosManager::ReplaceMacros(wxString& buffer, ProjectBuildTarget* target, bool subrequest)
{
    if (buffer.IsEmpty())
        return;

    static const wxString delim(_T("$%["));
    if ( buffer.find_first_of(delim) == wxString::npos )
        return;

    cbProject* project = target
                        ? target->GetParentProject()
                        : Manager::Get()->GetProjectManager()->GetActiveProject();
    EditorBase* editor = Manager::Get()->GetEditorManager()->GetActiveEditor();

    if (!target)
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
    if (project != m_LastProject || target != m_LastTarget || (editor && (editor->GetFilename() != m_ActiveEditorFilename))
                    || (project && (UnixFilename(project->GetBasePath()) != m_ProjectDir))
                    || (UnixFilename(m_ProjectWxFileName.GetFullName()) != m_ProjectFilename)
                    || (target && (target->GetTitle() != m_TargetName)) )
        RecalcVars(project, editor, target);

    wxString search;
    wxString replace;

    if (buffer.find(_T("$if")) != wxString::npos)
    {
        while (m_RE_If.Matches(buffer))
        {
            search = m_RE_If.GetMatch(buffer, 0);
            replace = EvalCondition(m_RE_If.GetMatch(buffer, 1), m_RE_If.GetMatch(buffer, 3), m_RE_If.GetMatch(buffer, 5), target);
            buffer.Replace(search, replace, false);
        }
    }

    while (m_RE_Script.Matches(buffer))
    {
        search = m_RE_Script.GetMatch(buffer, 1);
        replace = Manager::Get()->GetScriptingManager()->LoadBufferRedirectOutput(m_RE_Script.GetMatch(buffer, 2));
        buffer.Replace(search, replace, false);
    }

    while (m_RE_ToAbsolutePath.Matches(buffer))
    {
        search = m_RE_ToAbsolutePath.GetMatch(buffer, 0);
        const wxString relativePath = m_RE_ToAbsolutePath.GetMatch(buffer, 1);
        wxFileName fn(relativePath);
        fn.MakeAbsolute();
        replace = fn.GetFullPath();
        buffer.Replace(search, replace, false);
    }

    while (m_RE_To83Path.Matches(buffer))
    {
        search = m_RE_To83Path.GetMatch(buffer, 0);
        const wxString path = m_RE_To83Path.GetMatch(buffer, 1);
        wxFileName fn(path);
        fn.MakeAbsolute(); // make absolute before translating to 8.3 notation
        replace = fn.GetShortPath();
        buffer.Replace(search, replace, false);
    }

    int index = wxNOT_FOUND;
    while ((index = buffer.Index(toNativePath.wx_str())) != wxNOT_FOUND)
    {
        int end = MatchBrace(buffer, index + toNativePath.Length() - 1);
        wxString content = buffer.Mid(index + toNativePath.Length(), end - index - toNativePath.Length());
        ReplaceMacros(content, target, true);
        buffer.Replace(buffer.Mid(index, end - index + 1), UnixFilename(content), false);
    }

    while ((index = buffer.Index(toUnixPath.wx_str())) != wxNOT_FOUND)
    {
        int end = MatchBrace(buffer, index + toUnixPath.Length() - 1);
        wxString content = buffer.Mid(index + toUnixPath.Length(), end - index - toUnixPath.Length());
        ReplaceMacros(content, target, true);
        buffer.Replace(buffer.Mid(index, end - index + 1), UnixFilename(content, wxPATH_UNIX), false);
    }

    while ((index = buffer.Index(toWindowsPath.wx_str())) != wxNOT_FOUND)
    {
        int end = MatchBrace(buffer, index + toWindowsPath.Length() - 1);
        wxString content = buffer.Mid(index + toWindowsPath.Length(), end - index - toWindowsPath.Length());
        ReplaceMacros(content, target, true);
        buffer.Replace(buffer.Mid(index, end - index + 1), UnixFilename(content, wxPATH_WIN), false);
    }

    while (m_RE_RemoveQuotes.Matches(buffer))
    {
        search = m_RE_RemoveQuotes.GetMatch(buffer, 0);
        wxString content = m_RE_RemoveQuotes.GetMatch(buffer, 1).Trim().Trim(false);
        if (content.StartsWith(wxT("$")))
            ReplaceMacros(content, target, subrequest);
        if (content.Len()>2 && content.StartsWith(wxT("\"")) && content.EndsWith(wxT("\"")))
        {
            replace = content.Mid(1,content.Len()-2); // with first and last char (the quotes) removed
            buffer.Replace(search, replace, false);
        }
        else
            buffer.Replace(search, content, false);
    }

    while (m_RE_Unix.Matches(buffer))
    {
        replace.Empty();
        search = m_RE_Unix.GetMatch(buffer, 2);

        wxString var = m_RE_Unix.GetMatch(buffer, 3).Upper();

        if (var.GetChar(0) == _T('#'))
            replace = UnixFilename(m_UserVarMan->Replace(var));
        else
        {
            if (var.compare(const_COIN) == 0)
                replace.assign(1u, rand() & 1 ? _T('1') : _T('0'));
            else if (var.compare(const_RANDOM) == 0)
                replace = wxString::Format(_T("%d"), rand() & 0xffff);
            else
            {
                MacrosMap::iterator it;
                if ((it = m_Macros.find(var)) != m_Macros.end())
                    replace = it->second;
            }
        }

        const wxChar l = search.Last(); // make non-braced variables work
        if (l == _T('/') || l == _T('\\') || l == _T('$') || l == _T(' '))
            replace.append(l);

        if (replace.IsEmpty())
            wxGetEnv(var, &replace);

        buffer.Replace(search, replace, false);
    }

    while (m_RE_DOS.Matches(buffer))
    {
        replace.Empty();
        search = m_RE_DOS.GetMatch(buffer, 2);

        wxString var = m_RE_DOS.GetMatch(buffer, 3).Upper();

        if (var.GetChar(0) == _T('#'))
            replace = UnixFilename(m_UserVarMan->Replace(var));
        else
        {
            if (var.compare(const_COIN) == 0)
                replace.assign(1u, rand() & 1 ? _T('1') : _T('0'));
            else if (var.compare(const_RANDOM) == 0)
                replace = wxString::Format(_T("%d"), rand() & 0xffff);
            else
            {
                MacrosMap::iterator it;
                if ((it = m_Macros.find(var)) != m_Macros.end())
                    replace = it->second;
            }
        }

        if (replace.IsEmpty())
            wxGetEnv(var, &replace);

        buffer.Replace(search, replace, false);
    }

    if (!subrequest)
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

    if (!m_RE_IfSp.Matches(in_cond))
        return false_clause;


    wxString cmpToken(m_RE_IfSp.GetMatch(in_cond, 3).Strip(wxString::both));
    wxString left(m_RE_IfSp.GetMatch(in_cond, 2).Strip(wxString::both));
    wxString right(m_RE_IfSp.GetMatch(in_cond, 4).Strip(wxString::both));


    int compare = left.Cmp(right);
    if (compare == 0)
        compare = EQ;
    else if (compare < 0)
        compare = LT | NE;
    else if (compare > 0)
        compare = GT | NE;


    if (cmpToken.IsEmpty())
    {
        if (cond.IsEmpty() || cond.IsSameAs(_T("0")) || cond.IsSameAs(_T("false")))
            return false_clause;
        return true_clause;
    }

    int condCode = 0;

    if (cmpToken.IsSameAs(_T("==")) || cmpToken.IsSameAs(_T("=")))
        condCode = EQ;
    if (cmpToken.IsSameAs(_T("<")))
        condCode = LT;
    if (cmpToken.IsSameAs(_T(">")))
        condCode = GT;
    if (cmpToken.IsSameAs(_T("<=")))
        condCode = EQ | LT;
    if (cmpToken.IsSameAs(_T(">=")))
        condCode = EQ | GT;
    if (cmpToken.IsSameAs(_T("!=")))
        condCode = NE;

    return condCode & compare ? true_clause : false_clause;
}

int MacrosManager::MatchBrace(const wxString& buffer, int index)
{
    int depth = 0;
    while (index < (int)buffer.Length())
    {
        if (buffer[index] == wxT('{'))
            ++depth;
        else if (buffer[index] == wxT('}'))
            --depth;
        if (depth == 0)
            break;
        ++index;
    }
    return index;
}
