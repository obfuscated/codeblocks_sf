#ifndef CMDLINEGENERATOR_H
#define CMDLINEGENERATOR_H

#include <wx/string.h>
#include <wx/hashmap.h>
#include <compiletargetbase.h>

class cbProject;
class ProjectBuildTarget;
class ProjectFile;
class Compiler;

WX_DECLARE_HASH_MAP(ProjectBuildTarget*, wxString, wxPointerHash, wxPointerEqual, OptionsMap);

/** Generate command-lines needed to produce a build.
  *
  * This is used by DirectCommands to generate the needed
  * command lines for a build.
  */
class CmdLineGenerator
{
    public:
        CmdLineGenerator();
        ~CmdLineGenerator();

        /** Initialize for use with the specified @c project. */
        void Init(cbProject* project);

        /** Get the command line to compile/link the specific file. */
        void CreateSingleFileCompileCmd(wxString& command,
                                        ProjectBuildTarget* target,
                                        ProjectFile* pf,
                                        const wxString& file,
                                        const wxString& object,
                                        const wxString& deps);
    protected:
        wxString GetOrderedOptions(ProjectBuildTarget* target, OptionsRelationType rel, const wxString& project_options, const wxString& target_options);
        void SetupOutputFilenames(Compiler* compiler, ProjectBuildTarget* target);
        void SetupIncludeDirs(Compiler* compiler, ProjectBuildTarget* target);
        void SetupLibrariesDirs(Compiler* compiler, ProjectBuildTarget* target);
        void SetupResourceIncludeDirs(Compiler* compiler, ProjectBuildTarget* target);
        void SetupCompilerOptions(Compiler* compiler, ProjectBuildTarget* target);
        void SetupLinkerOptions(Compiler* compiler, ProjectBuildTarget* target);
        void SetupLinkLibraries(Compiler* compiler, ProjectBuildTarget* target);
        void SetupResourceCompilerOptions(Compiler* compiler, ProjectBuildTarget* target);
        wxString FixupLinkLibraries(Compiler* compiler, const wxString& lib);

        OptionsMap m_Output; ///< output filenames, per-target
        OptionsMap m_StaticOutput; ///< static output filenames, per-target
        OptionsMap m_DefOutput; ///< def output filenames, per-target
        OptionsMap m_Inc; ///< compiler 'include' dirs, per-target
        OptionsMap m_Lib; ///< linker 'include' dirs, per-target
        OptionsMap m_RC; ///< resource compiler 'include' dirs, per-target
        OptionsMap m_CFlags; ///< compiler flags, per-target
        OptionsMap m_LDFlags; ///< linker flags, per-target
        OptionsMap m_LDAdd; ///< link libraries, per-target
        OptionsMap m_RCFlags; ///< resource compiler flags, per-target
    private:
};

#endif // CMDLINEGENERATOR_H
