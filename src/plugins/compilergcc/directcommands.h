#ifndef DIRECTCOMMANDS_H
#define DIRECTCOMMANDS_H

#include <wx/string.h>
#include <wx/hashmap.h>

#define COMPILER_SIMPLE_LOG _T("CB_SLOG:")
#define COMPILER_TARGET_CHANGE  _T("TARGET:")

// forward decls
class CompilerGCC;
class Compiler;
class cbProject;
class ProjectBuildTarget;
class ProjectFile;
class pfDetails;
class CmdLineGenerator;

WX_DEFINE_ARRAY(ProjectFile*, MyFilesArray); // keep our own copy, to sort it by file weight (priority)

class DirectCommands
{
	public:
		DirectCommands(CompilerGCC* compilerPlugin,
                        CmdLineGenerator* generator,
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
        bool AreExternalDepsOutdated(const wxString& buildOutput, const wxString& additionalFiles, const wxString& externalDeps);
        bool IsObjectOutdated(const pfDetails& pfd);
        void DepsSearchStart(ProjectBuildTarget* target);
        MyFilesArray GetProjectFilesSortedByWeight(ProjectBuildTarget* target, bool compile, bool link);
        void AddCommandsToArray(const wxString& cmds, wxArrayString& array);

        int m_PageIndex;
        CompilerGCC* m_pCompilerPlugin;
        CmdLineGenerator* m_pGenerator;
        Compiler* m_pCompiler;
        cbProject* m_pProject;
        ProjectBuildTarget* m_pCurrTarget; // temp
	private:
};

#endif // DIRECTCOMMANDS_H
