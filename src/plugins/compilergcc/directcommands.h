#ifndef DIRECTCOMMANDS_H
#define DIRECTCOMMANDS_H

#include <wx/string.h>

#define COMPILER_SIMPLE_LOG "CB_SLOG:"

WX_DEFINE_ARRAY(ProjectFile*, MyFilesArray); // keep our own copy, to sort it by file weight (priority)

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
		
		wxArrayString CompileFile(ProjectBuildTarget* target, ProjectFile* pf, bool force = false);
		wxArrayString GetCompileFileCommand(ProjectBuildTarget* target, ProjectFile* pf);
		wxArrayString GetCompileCommands(ProjectBuildTarget* target, bool force = false);
		wxArrayString GetTargetCompileCommands(ProjectBuildTarget* target, bool force = false);
		wxArrayString GetLinkCommands(ProjectBuildTarget* target, bool force = false);
		wxArrayString GetTargetLinkCommands(ProjectBuildTarget* target, bool force = false);
		wxArrayString GetCleanCommands(ProjectBuildTarget* target, bool distclean = false);
		wxArrayString GetTargetCleanCommands(ProjectBuildTarget* target, bool distclean = false);

        static void QuoteStringIfNeeded(wxString& str);
        static void AppendArray(const wxArrayString& from, wxArrayString& to);
	protected:
        friend class pfDetails;
        bool IsObjectOutdated(const pfDetails& pfd);
        wxArrayString GetPreBuildCommands(ProjectBuildTarget* target);
        wxArrayString GetPostBuildCommands(ProjectBuildTarget* target);
        MyFilesArray GetProjectFilesSortedByWeight();
        void AddCommandsToArray(const wxString& cmds, wxArrayString& array);
        int m_PageIndex;
        CompilerGCC* m_pCompilerPlugin;
        Compiler* m_pCompiler;
        cbProject* m_pProject;
        ProjectBuildTarget* m_pCurrTarget; // temp
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
        wxString dep_dir;
        // those below, have no UnixFilename() applied, nor QuoteStringIfNeeded()
        wxString source_file_native;
        wxString object_file_native;
        wxString dep_file_native;
        wxString object_dir_native;
        wxString dep_dir_native;
        wxString source_file_absolute_native;
        wxString object_file_absolute_native;
        wxString dep_file_absolute_native;
};

#endif // DIRECTCOMMANDS_H
