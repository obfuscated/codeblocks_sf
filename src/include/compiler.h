/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef COMPILER_H
#define COMPILER_H

#include <vector>
#include <map>

#include <wx/string.h>
#include <wx/filename.h>
#include <wx/dynarray.h>
#include <wx/regex.h>
#include "compileoptionsbase.h"
#include "compileroptions.h"

class CompilerCommandGenerator;
class cbProject;
class ProjectBuildTarget;
class ProjectFile;
class wxXmlNode;

/*
    Macros used in command specs:

    Compiler executable: $compiler
    Resource compiler executable: $rescomp
    Linker executable: $linker
    Linker executable for static libs: $lib_linker
    Compiler options: $options
    Linker options: $link_options
    Include dirs: $includes
    Resource include dirs: $res_includes
    Library dirs: $libdirs
    Link libraries: $libs
    Source file: $file
    Object file: $object
    Dependency result: $dep_object
    All *linkable* object files: $link_objects
    All *linkable* flat object files: $link_flat_objects
    Exe output file: $exe_output
    Static lib output file: $static_output
    Dynamic lib output file: $dynamic_output
    Dynamic lib DEF output file: $def_output
    Resources output file: $resource_output
    Objects output dir: $output_dir_objects

    Usual special chars apply: \t, \n, etc.
    The command output should be ready for inclusion
    in a Makefile (properly tab aligned but *no* tabs at the start of the string)
    The macro substitution happens in compiler's MakefileGenerator.
*/

/// Enum categorizing compiler's output line as warning/error/info/normal
enum CompilerLineType
{
    cltNormal = 0,
    cltWarning,
    cltError,
    cltInfo
};

// regexes array declaration
struct RegExStruct
{
    RegExStruct()
        : desc(_("Unknown")), lt(cltError), regex(_T("")), filename(0), line(0)
    {
        memset(msg, 0, sizeof(msg));
        CompileRegEx();
    }
    RegExStruct(const RegExStruct& rhs)
        : desc(rhs.desc), lt(rhs.lt), regex(rhs.regex), filename(rhs.filename), line(rhs.line)
    {
        memcpy(msg, rhs.msg, sizeof(msg));
        CompileRegEx();
    }
    RegExStruct(const wxString&  _desc,
                CompilerLineType _lt,
                const wxString&  _regex,
                int              _msg,
                int              _filename = 0,
                int              _line     = 0,
                int              _msg2     = 0,
                int              _msg3     = 0)
        : desc(_desc), lt(_lt), regex(_regex), filename(_filename), line(_line)
    {
        msg[0] = _msg;
        msg[1] = _msg2;
        msg[2] = _msg3;
        CompileRegEx();
    }
    RegExStruct& operator=(RegExStruct &obj)
    {
        desc=obj.desc;
        lt=obj.lt;
        regex=obj.regex;
        filename=obj.filename;
        line=obj.line;
        memcpy(msg, obj.msg, sizeof(msg));

        CompileRegEx();
        return *this;
    }

    bool operator!=(const RegExStruct& other)
    {
        return !(*this == other);
    }
    bool operator==(const RegExStruct& other)
    {
        return (   desc     == other.desc
                && lt       == other.lt
                && regex    == other.regex
                && msg[0]   == other.msg[0]
                && msg[1]   == other.msg[1]
                && msg[2]   == other.msg[2]
                && filename == other.filename
                && line     == other.line );
    }

    void CompileRegEx()
    {
        if (!regex.empty())
            regexObject.Compile(regex);
    }

    const wxRegEx& GetRegEx() const { return regexObject; }

    wxString         desc;     // title of this regex
    CompilerLineType lt;       // classify the line, if regex matches
    wxString         regex;    // the regex to match
    int              msg[3];   // up-to 3 sub-expression nr for warning/error message
    int              filename; // sub-expression nr for filename
    int              line;     // sub-expression nr for line number
    // if more than one sub-expressions are entered for msg,
    // they are appended to each other, with one space in between.
    // Appending takes place in the same order...
private:
    wxRegEx regexObject;
};
WX_DECLARE_OBJARRAY(RegExStruct, RegExArray);

/// Helper enum to retrieve compiler commands
enum CommandType
{
    ctCompileObjectCmd = 0, ///< Compile object command, e.g. "$compiler $options $includes -c $file -o $object"
    ctGenDependenciesCmd,   ///< Generate dependencies command
    ctCompileResourceCmd,   ///< Compile Win32 resources command, e.g. "$rescomp -i $file -J rc -o $resource_output -O coff $includes"
    ctLinkExeCmd,           ///< Link executable command, e.g. "$linker $libdirs -o $exe_output $link_objects $libs -mwindows"
    ctLinkConsoleExeCmd,    ///< Link console executable command, e.g. "$linker $libdirs -o $exe_output $link_objects $libs"
    ctLinkDynamicCmd,       ///< Link dynamic (dll) lib command, e.g. "$linker -shared -Wl,--output-def=$def_output -Wl,--out-implib=$static_output -Wl,--dll $libdirs $link_objects $libs -o $dynamic_output"
    ctLinkStaticCmd,        ///< Link static lib command, e.g. "ar -r $output $link_objects\n\tranlib $static_output"
    ctLinkNativeCmd,        ///< Link native binary command

    ctCount                 ///< Do NOT use
};



/// Helper enum for type of compiler logging
enum CompilerLoggingType
{
    clogFull,
    clogSimple,
    clogNone
};

enum AutoDetectResult
{
    adrDetected,
    adrGuessed
};

/// Struct to keep programs
struct CompilerPrograms
{
    wxString C;         // C compiler
    wxString CPP;       // C++ compiler
    wxString LD;        // dynamic libs linker
    wxString LIB;       // static libs linker
    wxString WINDRES;   // resource compiler
    wxString MAKE;      // make
    wxString DBGconfig; // debugger config name = "debugger_settings_name:config_name"
};

/// Struct to keep switches
struct CompilerSwitches
{
    static const CompilerLoggingType defaultLogging = clogFull;
    wxString includeDirs;         // -I
    wxString libDirs;             // -L
    wxString linkLibs;            // -l
    wxString defines;             // -D
    wxString genericSwitch;       // -
    wxString objectExtension;     // o
    bool forceFwdSlashes;         // force use forward slashes in file/path names (used by CompilerCommandGenerator)
    bool forceLinkerUseQuotes;    // use quotes for filenames in linker command line (needed or not)?
    bool forceCompilerUseQuotes;  // use quotes for filenames in compiler command line (needed or not)?
    bool needDependencies;        // true
    CompilerLoggingType logging;  // clogFull
    wxString libPrefix;           // lib
    wxString libExtension;        // a
    bool linkerNeedsLibPrefix;    // when adding a link library, linker needs prefix?
    bool linkerNeedsLibExtension; // when adding a link library, linker needs extension?
    bool linkerNeedsPathResolved; // linker does not support libDirs; C::B must resolve file paths
    bool supportsPCH;             // supports pre-compiled headers?
    wxString PCHExtension;        // pre-compiled headers extension
    bool UseFlatObjects;          // Use Flat object file names (no extra subdirs)?
    bool UseFullSourcePaths;      // This is mainly a workaround for the GDB debugger, apparently I doesn't deal
                                  // well with relative paths, therefore for GCC it is better to specify the source
                                  // full to the compiler in a full path notation, for all other compilers it is
                                  // suggested to keep this switch at false
    bool Use83Paths;              // This is mainly a workaround for the resource compiler under Windows, apparently
                                  // it doesn't deal well with spaces in the (include) path even if the path is quoted,
                                  // therefore use 8.3 notation without spaces on Windows.
                                  // However, this will apply to all include path's as other tools might have the
                                  // same issue and it won't hurt to apply it to all include directories, if enabled.
    wxChar includeDirSeparator;   // space
    wxChar libDirSeparator;       // space
    wxChar objectSeparator;       // space
    int statusSuccess;            // 0    - treat exit-codes >= 0 and <= statusSuccess as success (do not set negative!)

    CompilerSwitches(); // constructor initializing the members, specific compilers should overrule if needed
};

/// Struct for compiler/linker commands
struct CompilerTool
{
    // extensions string will be converted to array by GetArrayFromString using DEFAULT_ARRAY_SEP (;)
    // as separator
    CompilerTool(const wxString& command_in = wxEmptyString, const wxString& extensions_in = wxEmptyString, const wxString& generatedFiles_in = wxEmptyString)
        : command(command_in), extensions(GetArrayFromString(extensions_in)), generatedFiles(GetArrayFromString(generatedFiles_in))
    {}
    CompilerTool(const CompilerTool& rhs)
        : command(rhs.command), extensions(rhs.extensions), generatedFiles(rhs.generatedFiles)
    {}
    bool operator==(const CompilerTool& rhs) const { return command == rhs.command && extensions == rhs.extensions && generatedFiles == rhs.generatedFiles; }
    bool operator!=(const CompilerTool& rhs) const { return !(*this == rhs); }

    wxString      command;        ///< command to execute
    wxArrayString extensions;     ///< file extensions for which the command will be invoked (no leading dot)
    wxArrayString generatedFiles; ///< the native language files this command generates that should be further compiled
};

typedef std::vector<CompilerTool> CompilerToolsVector;

/**
  * @brief Abstract base class for compilers.
  *
  * Create a derived class and add it in compilerfactory.cpp
  */
class DLLIMPORT Compiler : public CompileOptionsBase
{
    public:
        static const wxString FilePathWithSpaces;
        Compiler(const wxString& name, const wxString& ID, const wxString& parentID = wxEmptyString, int weight = 50);
        virtual ~Compiler();

        /** @brief Check if the compiler is actually valid (installed). */
        virtual bool IsValid();

        /** @brief Check if the supplied string is a compiler warning/error */
        virtual CompilerLineType CheckForWarningsAndErrors(const wxString& line);
        /** @brief Returns warning/error filename. Use it after a call to CheckForWarningsAndErrors() */
        virtual wxString GetLastErrorFilename()             { return m_ErrorFilename; }
        /** @brief Returns warning/error line number (as a string). Use it after a call to CheckForWarningsAndErrors() */
        virtual wxString GetLastErrorLine()                 { return m_ErrorLine; }
        /** @brief Returns warning/error actual string. Use it after a call to CheckForWarningsAndErrors() */
        virtual wxString GetLastError()                     { return m_Error; }
        /** @brief Get the compiler's name */
        virtual const wxString& GetName() const             { return m_Name; }
        /** @brief Get the compiler's master path (must contain "bin", "include" and "lib") */
        virtual const wxString& GetMasterPath() const       { return m_MasterPath; }
        /** @brief Get the compiler's extra paths */
        virtual const wxArrayString& GetExtraPaths() const  { return m_ExtraPaths; }
        /** @brief Get the compiler's programs */
        virtual const CompilerPrograms& GetPrograms() const { return m_Programs; }
        /** @brief Get the compiler's generic switches */
        virtual const CompilerSwitches& GetSwitches() const { return m_Switches; }
        /** @brief Get the compiler's options */
        virtual const CompilerOptions& GetOptions() const   { return m_Options; }
        /** @brief Get a command based on CommandType
          * @param ct The command type to process
          * @param fileExtension the file's extension (no leading dot)
          */
        virtual const wxString& GetCommand(CommandType ct, const wxString& fileExtension = wxEmptyString) const;
        /** @brief Get a compiler tool based on CommandType */
        virtual const CompilerTool& GetCompilerTool(CommandType ct, const wxString& fileExtension = wxEmptyString) const;
        /** @brief Get a command tool vector based on CommandType (used by advanced compiler dialog) */
        virtual CompilerToolsVector& GetCommandToolsVector(CommandType ct) { return m_Commands[ct]; }
        /** @brief Get the array of regexes used in errors/warnings recognition */
        virtual const RegExArray& GetRegExArray(){ return m_RegExes; }
        /** @brief Load the default (preset) array of regexes used in errors/warnings recognition */
        virtual void LoadDefaultRegExArray(bool globalPrecedence = false);

        /** @brief Set the compiler's name */
        virtual void SetName(const wxString& name){ m_Name = name; }
        /** @brief Set the compiler's master path (must contain "bin", "include" and "lib") */
        virtual void SetMasterPath(const wxString& path){ m_MasterPath = path; m_NeedValidityCheck = true; }
        /** @brief Set the compiler's extra paths */
        virtual void SetExtraPaths(const wxArrayString& paths){ m_ExtraPaths = paths; m_NeedValidityCheck = true; }
        /** @brief Set the compiler's programs */
        virtual void SetPrograms(const CompilerPrograms& programs){ m_Programs = programs; m_NeedValidityCheck = true; }
        /** @brief Set the compiler's generic switches */
        virtual void SetSwitches(const CompilerSwitches& switches){ m_Switches = switches; }
        /** @brief Set the compiler's options */
        virtual void SetOptions(const CompilerOptions& options){ m_Options = options; }
        /** @brief Set the array of regexes used in errors/warnings recognition */
        virtual void SetRegExArray(const RegExArray& regexes) { m_RegExes = regexes; CompileRegExArray(); }


        /** @brief Save settings */
        virtual void SaveSettings(const wxString& baseKey);
        /** @brief Load settings */
        virtual void LoadSettings(const wxString& baseKey);
        /** @brief Reset settings to defaults.
          * Put initialization code here or leave blank for standard XML loading.
          * Call this from the default constructor.
          */
        virtual void Reset();
        /** @brief Reload option flags (for copied compilers).
          * Override if not using standard XML loading.
          */
        virtual void ReloadOptions();
        /** @brief Try to auto-detect the compiler's installation directory */
        virtual AutoDetectResult AutoDetectInstallationDir() = 0;

        /** @brief Get this compiler's unique ID */
        const wxString& GetID() const { return m_ID; }
        /** @brief Get this compiler's parent's unique ID */
        const wxString& GetParentID() const { return m_ParentID; }

        /** @brief Get the command type descriptions (used in advanced compiler options) */
        static wxString CommandTypeDescriptions[ctCount];

        /** @brief Set the compiler version string. Please override this virtual function
          * with your own compiler-version detection code if you want to use this.
          *
          * By default this function does nothing. */
        virtual void SetVersionString() { return; };

        /** @brief Get the compiler version string */
        const wxString GetVersionString() const { return m_VersionString; };

        /** This is to be overridden, if compiler needs to alter the default
          * command line generation.
          */
        virtual CompilerCommandGenerator* GetCommandGenerator(cbProject *project);

        void SetCOnlyFlags(const wxString& flags)   { m_SortOptions[0] = flags; };
        void SetCPPOnlyFlags(const wxString& flags) { m_SortOptions[1] = flags; };

        const wxString& GetCOnlyFlags()   { return m_SortOptions[0]; };
        const wxString& GetCPPOnlyFlags() { return m_SortOptions[1]; };

    protected:
        friend class CompilerFactory;
        Compiler(const Compiler& other); // copy ctor to copy everything but update m_ID

        /** @brief Implement this in new compilers, to return a new copy */
        virtual Compiler* CreateCopy() = 0;

        // purposely non-virtual
        bool IsUniqueID(const wxString& ID){ return m_CompilerIDs.Index(ID) == wxNOT_FOUND; }
        // converts, if needed, m_ID to something that is valid
        void MakeValidID();

        // load options from the corresponding options_<name>.xml
        void LoadDefaultOptions(const wxString& name, int recursion = 0);
        // load array of regexes from the corresponding options_<name>.xml
        void LoadRegExArray(const wxString& name, bool globalPrecedence = false, int recursion = 0);

        bool EvalXMLCondition(const wxXmlNode* node);
        wxString GetExecName(const wxString& name);

        // keeps a copy of current settings (works only the first time it's called)
        void MirrorCurrentSettings();

        void CompileRegExArray();

        // set the following members in your class
        wxString            m_Name;
        wxString            m_MasterPath;
        wxArrayString       m_ExtraPaths;
        CompilerToolsVector m_Commands[ctCount];
        CompilerPrograms    m_Programs;
        CompilerSwitches    m_Switches;
        CompilerOptions     m_Options;
        RegExArray          m_RegExes;
        wxString            m_ErrorFilename;
        wxString            m_ErrorLine;
        wxString            m_Error;
        wxString            m_VersionString;
        wxString            m_SortOptions[2]; // m_SortOptions[0] == C-only flags; m_SortOptions[1] == C++-only flags

        int m_Weight; // lower means listed sooner (try to keep between 0 and 100)
    private:
        wxString m_ID;
        wxString m_ParentID; // -1 for builtin compilers, the builtin compiler's ID to derive from for user compilers...
        static wxArrayString m_CompilerIDs; // map to guarantee unique IDs
        bool m_Valid; // 'valid' flag
        bool m_NeedValidityCheck; // flag to re-check validity (raised when changing compiler paths)

        // "mirror" default settings for comparing when saving (to save only those that differ from defaults)
        struct MirrorSettings
        {
            wxString         Name;
            wxString         MasterPath;
            wxArrayString    ExtraPaths;
            CompilerPrograms Programs;

            // these are the CompileOptionsBase settings that each compiler keeps on a global level
            wxArrayString CompilerOptions_;
            wxArrayString LinkerOptions;
            wxArrayString IncludeDirs;
            wxArrayString ResIncludeDirs;
            wxArrayString LibDirs;
            wxArrayString LinkLibs;
            wxArrayString CmdsBefore;
            wxArrayString CmdsAfter;

            // below are the settings that the user is asked to revert to defaults (if defaults have changed)
            CompilerToolsVector Commands[ctCount];
            CompilerSwitches    Switches;
            CompilerOptions     Options;
            RegExArray          RegExes;

            wxString SortOptions[2];
        };
        MirrorSettings m_Mirror;
        bool           m_Mirrored; // flag to only mirror the settings once
};

#endif // COMPILER_H
