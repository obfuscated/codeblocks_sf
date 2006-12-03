#ifndef COMPILERCOMMANDGENERATOR_H
#define COMPILERCOMMANDGENERATOR_H

#include <map>

#include <wx/string.h>
#include <wx/dynarray.h>
#include "settings.h"
#include "compiletargetbase.h"

class cbProject;
class ProjectBuildTarget;
class ProjectFile;
class Compiler;

typedef std::map<ProjectBuildTarget*, wxString> OptionsMap;
typedef std::map<ProjectBuildTarget*, wxArrayString> SearchDirsMap;
typedef std::map<wxString, wxString> BackticksMap;

/** Generate command-lines needed to produce a build.
  * This pre-generates everything when Init() is called.
  *
  * This is used by compilers to generate the needed
  * command lines for a build.
  */
class DLLIMPORT CompilerCommandGenerator
{
    public:
        CompilerCommandGenerator();
        virtual ~CompilerCommandGenerator();

        /** Initialize for use with the specified @c project. */
        virtual void Init(cbProject* project);

        /** Get the command line to compile/link the specific file. */
        virtual void GenerateCommandLine(wxString& macro,
                                        ProjectBuildTarget* target,
                                        ProjectFile* pf,
                                        const wxString& file,
                                        const wxString& object,
                                        const wxString& FlatObject,
                                        const wxString& deps);

		/** @brief Get the full include dirs used in the actuall command line.
		  *
		  * These are the actual include dirs that will be used for building
		  * and might be different than target->GetIncludeDirs(). This is
		  * because it's the sum of target include dirs + project include dirs +
		  * build-script include dirs.
		  * @note This is only valid after Init() has been called.
		  */
		virtual const wxArrayString& GetCompilerSearchDirs(ProjectBuildTarget* target);
    protected:
        virtual void DoBuildScripts(CompileTargetBase* target, const wxString& funcName);
        virtual wxString GetOrderedOptions(const ProjectBuildTarget* target, OptionsRelationType rel, const wxString& project_options, const wxString& target_options);
        virtual wxArrayString GetOrderedOptions(const ProjectBuildTarget* target, OptionsRelationType rel, const wxArrayString& project_options, const wxArrayString& target_options);
        virtual wxString SetupOutputFilenames(Compiler* compiler, ProjectBuildTarget* target);
        virtual wxString SetupIncludeDirs(Compiler* compiler, ProjectBuildTarget* target);
        virtual wxString SetupLibrariesDirs(Compiler* compiler, ProjectBuildTarget* target);
        virtual wxString SetupResourceIncludeDirs(Compiler* compiler, ProjectBuildTarget* target);
        virtual wxString SetupCompilerOptions(Compiler* compiler, ProjectBuildTarget* target);
        virtual wxString SetupLinkerOptions(Compiler* compiler, ProjectBuildTarget* target);
        virtual wxString SetupLinkLibraries(Compiler* compiler, ProjectBuildTarget* target);
        virtual wxString SetupResourceCompilerOptions(Compiler* compiler, ProjectBuildTarget* target);
        virtual wxString FixupLinkLibraries(Compiler* compiler, const wxString& lib);

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

        wxString m_PrjIncPath; ///< directive to add the project's top-level path in compiler search dirs (ready for the command line)
    
		SearchDirsMap m_CompilerSearchDirs; ///< array of final search dirs, per-target
    private:
        void ExpandBackticks(wxString& str);
        BackticksMap m_Backticks;
        wxArrayString m_NotLoadedScripts;
        wxArrayString m_ScriptsWithErrors;
};

#endif // COMPILERCOMMANDGENERATOR_H
