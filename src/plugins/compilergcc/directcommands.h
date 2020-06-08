#ifndef DIRECTCOMMANDS_H
#define DIRECTCOMMANDS_H

#include <wx/string.h>
#include <wx/hashmap.h>

extern const wxString COMPILER_SIMPLE_LOG;
extern const wxString COMPILER_NOTE_LOG;
extern const wxString COMPILER_WARNING_LOG;
extern const wxString COMPILER_ERROR_LOG;
extern const wxString COMPILER_TARGET_CHANGE;
extern const wxString COMPILER_WAIT;
extern const wxString COMPILER_WAIT_LINK;

extern const wxString COMPILER_NOTE_ID_LOG;
/// Print a NOTE log message to the build log, without advancing the progress counter
extern const wxString COMPILER_ONLY_NOTE_ID_LOG;
extern const wxString COMPILER_WARNING_ID_LOG;
extern const wxString COMPILER_ERROR_ID_LOG;


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

        /// \brief Check if the command line is too long for the current running system and create a response file if necessary
        ///
        /// If the command line (executableCmd) used to execute a command is longer than the operating system limit
        /// this function breaks the command line at the limit of the OS and generates a response file in the path folder.
        /// Then it replaces the original command line with a constructed command line with the added response file and a pre-appended @.
        /// This is the common marker for response files for many compilers.
        /// The command is spitted, so that the final command with the response file does not exceed the command line length limit.
        /// The filename of the response file is generated from the executable name (string part until the first space from left)
        /// and the base name. This name has to be unique.
        /// The length limit of the operating system can be overwritten with the CB_COMMAND_LINE_MAX_LENGTH defined during compile time
        /// If it is not defined at compile time reasonable limits for windows and UNIX are used.
        ///
        /// For example:
        ///  mingw32-gcc.exe -i this/is/in/the/lime -i this/is/longer/then/the/operating/system/supports
        ///
        /// gets transformed to
        ///  mingw32-gcc.exe -i this/is/in/the/lime -i @path/to/response/file.respfile
        ///
        /// \param[in,out] executableCmd he command line input to check. Returns the modified command line if it was too long
        /// \param[in,out] outputCommandArray The command queue. Some logging information is added so it can appear in the output log
        /// \param[in] basename A base name, used to create a unique name for the response file
        /// \param[in] path Base path where the response file is created
        void CheckForToLongCommandLine(wxString& executableCmd, wxArrayString& outputCommandArray, const wxString& basename ,const wxString& path) const;

        int                       m_PageIndex;
        CompilerGCC*              m_pCompilerPlugin;
        Compiler*                 m_pCompiler;
        cbProject*                m_pProject;
        CompilerCommandGenerator* m_pGenerator;
    private:
};

#endif // DIRECTCOMMANDS_H
