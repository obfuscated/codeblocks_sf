#ifndef DIRECTCOMMANDS_H
#define DIRECTCOMMANDS_H

#include <wx/string.h>
#include <wx/hashmap.h>


#define COMPILER_SIMPLE_LOG     _T("SLOG:")
#define COMPILER_TARGET_CHANGE  _T("TGT:")
#define COMPILER_WAIT           _T("WAIT")
#define COMPILER_WAIT_LINK      _T("LINK")

// forward decls
class CompilerCommandGenerator;
class CompilerGCC;
class Compiler;
class cbProject;
class ProjectBuildTarget;
class ProjectFile;
class pfDetails;

WX_DEFINE_ARRAY(ProjectFile*, MyFilesArray); // keep our own copy, to sort it by file weight (priority)

class DirectCommands
{
        DirectCommands(DirectCommands &);
        DirectCommands& operator=(DirectCommands &);
    public:
        DirectCommands(CompilerGCC* compilerPlugin,
                        Compiler* compiler,
                        cbProject* project,
                        int logPageIndex = 0);
        ~DirectCommands();

        wxArrayString GetPreBuildCommands(ProjectBuildTarget* target);
        wxArrayString GetPostBuildCommands(ProjectBuildTarget* target);
        wxArrayString CompileFile(ProjectBuildTarget* target, ProjectFile* pf, bool force = false);
        wxArrayString GetCompileFileCommand(ProjectBuildTarget* target, ProjectFile* pf);
        wxArrayString GetCompileSingleFileCommand(const wxString& filename);
        wxArrayString GetCompileCommands(ProjectBuildTarget* target, bool force = false);
        wxArrayString GetTargetCompileCommands(ProjectBuildTarget* target, bool force = false);
        wxArrayString GetLinkCommands(ProjectBuildTarget* target, bool force = false);
        wxArrayString GetTargetLinkCommands(ProjectBuildTarget* target, bool force = false);
        wxArrayString GetCleanCommands(ProjectBuildTarget* target, bool distclean = false);
        wxArrayString GetCleanSingleFileCommand(const wxString& filename);
        wxArrayString GetTargetCleanCommands(ProjectBuildTarget* target, bool distclean = false);
        bool m_doYield;
    protected:
        bool AreExternalDepsOutdated(ProjectBuildTarget* target, const wxString& buildOutput, wxArrayString* filesMissing);
        bool IsObjectOutdated(ProjectBuildTarget* target, const pfDetails& pfd, wxString* errorStr = 0);
        void DepsSearchStart(ProjectBuildTarget* target);
        MyFilesArray GetProjectFilesSortedByWeight(ProjectBuildTarget* target, bool compile, bool link);
        void AddCommandsToArray(const wxString& cmds, wxArrayString& array, bool isWaitCmd = false, bool isLinkCmd = false);

        int m_PageIndex;
        CompilerGCC* m_pCompilerPlugin;
        Compiler* m_pCompiler;
        cbProject* m_pProject;
        ProjectBuildTarget* m_pCurrTarget; // temp
        CompilerCommandGenerator *m_pGenerator;
    private:
};

#endif // DIRECTCOMMANDS_H
