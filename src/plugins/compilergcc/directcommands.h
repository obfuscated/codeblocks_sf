#ifndef DIRECTCOMMANDS_H
#define DIRECTCOMMANDS_H

#include <wx/string.h>
#include <wx/hashmap.h>

extern const wxString COMPILER_SIMPLE_LOG;
extern const wxString COMPILER_NOTE_LOG;
extern const wxString COMPILER_WARNING_LOG;
extern const wxString COMPILER_TARGET_CHANGE;
extern const wxString COMPILER_WAIT;
extern const wxString COMPILER_WAIT_LINK;

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
                       Compiler*    compiler,
                       cbProject*   project,
                       int          logPageIndex = 0);
        ~DirectCommands();

        wxArrayString GetPreBuildCommands(ProjectBuildTarget* target) const;
        wxArrayString GetPostBuildCommands(ProjectBuildTarget* target) const;
        wxArrayString CompileFile(ProjectBuildTarget* target, ProjectFile* pf, bool force = false) const;
        wxArrayString GetCompileFileCommand(ProjectBuildTarget* target, ProjectFile* pf) const;
        wxArrayString GetCompileSingleFileCommand(const wxString& filename) const;
        wxArrayString GetCompileCommands(ProjectBuildTarget* target, bool force = false) const;
        wxArrayString GetTargetCompileCommands(ProjectBuildTarget* target, bool force = false) const;
        wxArrayString GetLinkCommands(ProjectBuildTarget* target, bool force = false) const;
        wxArrayString GetTargetLinkCommands(ProjectBuildTarget* target, bool force = false) const;
        wxArrayString GetCleanCommands(ProjectBuildTarget* target, bool distclean = false) const;
        wxArrayString GetCleanSingleFileCommand(const wxString& filename) const;
        wxArrayString GetTargetCleanCommands(ProjectBuildTarget* target, bool distclean = false) const;

        bool m_doYield;
    protected:
        bool         AreExternalDepsOutdated(ProjectBuildTarget* target, const wxString& buildOutput, wxArrayString* filesMissing) const;
        bool         IsObjectOutdated(ProjectBuildTarget* target, const pfDetails& pfd, wxString* errorStr = 0) const;
        void         DepsSearchStart(ProjectBuildTarget* target) const;
        MyFilesArray GetProjectFilesSortedByWeight(ProjectBuildTarget* target, bool compile, bool link) const;
        void         AddCommandsToArray(const wxString& cmds, wxArrayString& array, bool isWaitCmd = false, bool isLinkCmd = false) const;

        int                       m_PageIndex;
        CompilerGCC*              m_pCompilerPlugin;
        Compiler*                 m_pCompiler;
        cbProject*                m_pProject;
        CompilerCommandGenerator* m_pGenerator;
    private:
};

#endif // DIRECTCOMMANDS_H
