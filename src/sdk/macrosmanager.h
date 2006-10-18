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
    void ReplaceMacros(wxString& buffer, bool envVarsToo = false, ProjectBuildTarget* target = 0);
    wxString ReplaceMacros(const wxString& buffer, bool envVarsToo = false, ProjectBuildTarget* target = 0);
    void ReplaceEnvVars(wxString& buffer);
    void RecalcVars(cbProject* project,EditorBase* editor,ProjectBuildTarget* target);
    void ClearProjectKeys();
protected:
    ProjectBuildTarget* m_lastTarget;
    cbProject* m_lastProject;
    EditorBase* m_lastEditor;
    wxFileName m_prjname;
    wxString m_AppPath, m_DataPath, m_Plugins, m_ActiveEditorFilename,
    m_ProjectFilename, m_ProjectName, m_ProjectDir, m_ProjectTopDir,
    m_ProjectFiles, m_Makefile, m_TargetOutputDir, m_TargetName,
    m_TargetOutputBaseName, m_TargetFilename;
	MacrosMap macros;
    wxRegEx m_re;
    UserVariableManager *m_uVarMan;
public:
    void Reset();
private:
    MacrosManager();
    ~MacrosManager();
};

#endif // MACROSMANAGER_H

