#ifndef DIRECTCOMMANDS_H
#define DIRECTCOMMANDS_H

#include <wx/string.h>

#define COMPILER_SIMPLE_LOG "CB_SLOG:"

// forward decls
class CompilerGCC;
class Compiler;
class cbProject;
class ProjectBuildTarget;
class ProjectFile;
class pfDetails;

class DirectCommands
{
	public:
		DirectCommands(CompilerGCC* compilerPlugin, Compiler* compiler, cbProject* project, int logPageIndex = 0);
		~DirectCommands();
		
		wxArrayString GetCompileFileCommand(ProjectBuildTarget* target, ProjectFile* pf, bool force = false);
		wxArrayString GetCompileCommands(ProjectBuildTarget* target, bool force = false);
		wxArrayString GetTargetCompileCommands(ProjectBuildTarget* target, bool force = false);
		wxArrayString GetLinkCommands(ProjectBuildTarget* target, bool force = false);
		wxArrayString GetTargetLinkCommands(ProjectBuildTarget* target, bool force = false);
		wxArrayString GetCleanCommands(ProjectBuildTarget* target);
		wxArrayString GetTargetCleanCommands(ProjectBuildTarget* target);

        void QuoteStringIfNeeded(wxString& str);
        void AppendArray(const wxArrayString& from, wxArrayString& to);
        bool GetDependenciesOf(const wxString& filename);
	protected:
        friend class pfDetails;
        bool DependsOnChangedFile(const pfDetails& pfd);
        int m_PageIndex;
        CompilerGCC* m_pCompilerPlugin;
        Compiler* m_pCompiler;
        cbProject* m_pProject;
        wxArrayString m_DepFiles; // used in GetDependenciesOf()
	private:
};

class pfDetails
{
    public:
        pfDetails(DirectCommands* cmds, ProjectBuildTarget* target, ProjectFile* pf);
        // all the members below, are set in the constructor
        wxString source_file;
        wxString object_file;
        wxString dep_file;
        wxString object_dir;
        // those below, have no UnixFilename() applied, nor QuoteStringIfNeeded()
        wxString source_file_native;
        wxString object_file_native;
        wxString dep_file_native;
        wxString object_dir_native;
};

#endif // DIRECTCOMMANDS_H
