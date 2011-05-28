/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef MACROSMANAGER_H
#define MACROSMANAGER_H

#include "settings.h"
#include "manager.h"
#include <wx/regex.h>
#include <wx/filename.h>

// forward decls;
class wxMenuBar;
class cbProject;
class ProjectBuildTarget;
class EditorBase;
class UserVariableManager;

WX_DECLARE_STRING_HASH_MAP( wxString, MacrosMap );

class DLLIMPORT MacrosManager : public Mgr<MacrosManager>
{
public:
    friend class Manager;
    friend class Mgr<MacrosManager>;
    void CreateMenu(wxMenuBar* menuBar);
    void ReleaseMenu(wxMenuBar* menuBar);
    void ReplaceMacros(wxString& buffer, ProjectBuildTarget* target = 0, bool subrequest = false);
    wxString ReplaceMacros(const wxString& buffer, ProjectBuildTarget* target = 0);
    void ReplaceEnvVars(wxString& buffer) { ReplaceMacros(buffer); }  /* misnomer, should be ReplaceVariables */;
    void RecalcVars(cbProject* project, EditorBase* editor, ProjectBuildTarget* target);
    void ClearProjectKeys();
    void Reset();
protected:
    ProjectBuildTarget* m_LastTarget;
    cbProject*          m_LastProject;

    wxFileName          m_ProjectWxFileName;
    wxFileName          m_WorkspaceWxFileName;

    wxString            m_AppPath;
    wxString            m_DataPath;
    wxString            m_Plugins;
    wxString            m_ActiveEditorFilename;
    int                 m_ActiveEditorLine;
    int                 m_ActiveEditorColumn;
    wxString            m_WorkspaceFilename;
    wxString            m_WorkspaceName;
    wxString            m_WorkspaceDir;
    wxString            m_ProjectFilename;
    wxString            m_ProjectName;
    wxString            m_ProjectDir;
    wxString            m_ProjectTopDir;
    wxString            m_ProjectFiles;
    wxString            m_Makefile;
    wxString            m_TargetOutputDir;
    wxString            m_TargetName;
    wxString            m_TargetOutputBaseName;
    wxString            m_TargetOutputFilename;
    wxString            m_TargetFilename;

    MacrosMap           m_Macros;
    wxRegEx             m_RE_Unix;
    wxRegEx             m_RE_DOS;
    wxRegEx             m_RE_If;
    wxRegEx             m_RE_IfSp;
    wxRegEx             m_RE_Script;
    wxRegEx             m_RE_ToFullPath;
    UserVariableManager *m_UserVarMan;

private:
    MacrosManager();
    ~MacrosManager();
    wxString EvalCondition(const wxString& cond, const wxString& true_clause, const wxString& false_clause, ProjectBuildTarget* target);
};

#endif // MACROSMANAGER_H

