#ifndef COMPILER_H
#define COMPILER_H

#include <wx/string.h>
#include <wx/filename.h>
#include "compileoptionsbase.h"
#include "compileroptions.h"

/*
    Macros used in command specs:
    
    Compiler executable: $compiler
    Resource compiler executable: $rescomp
    Linker executable: $linker
    Compiler options: $options
    Linker options: $link_options
    Include dirs: $includes
    Rsource include dirs: $res_includes
    Library dirs: $libdirs
    Link libraries: $libs
    Source file: $file
    Object file: $object
    Dependency result: $dep_object
    All object files: $objects
    All *linkable* object files: $link_objects
    Exe output file: $exe_output
    Static lib output file: $static_output
    Dynamic lib output file: $dynamic_output
    Dynamic lib DEF output file: $def_output
    Resources output file: $resource_output
    
    Usual special chars apply: \t, \n, etc.
    The command output should be ready for inclusion
    in a Makefile (properly tab aligned but *no* tabs at the start of the string)
    The macro substitution happens in compiler's MakefileGenerator.
*/

/// Helper enum to retrieve compiler commands
enum CommandType
{
    ctCompileObjectCmd = 0, ///< Compile object command, e.g. "$compiler $options $includes -c $file -o $object"
    ctGenDependenciesCmd,   ///< Generate dependencies command
    ctCompileResourceCmd,   ///< Compile Win32 resources command, e.g. "$rescomp -i $file -J rc -o $resource_output -O coff $includes"
    ctLinkExeCmd,           ///< Link executable command, e.g. "$linker $libdirs -o $exe_output $link_objects $libs"
    ctLinkDynamicCmd,       ///< Link dynamic (dll) lib command, e.g. "$linker -shared -Wl,--output-def=$def_output -Wl,--out-implib=$static_output -Wl,--dll $libdirs $link_objects $libs -o $dynamic_output"
    ctLinkStaticCmd         ///< Link static lib command, e.g. "ar -r $output $link_objects\n\tranlib $static_output"
};
#define COMPILER_COMMAND_TYPES_COUNT 6 // change this to reflect the above enumerators count

/// Helper enum for type of compiler logging
enum CompilerLoggingType
{
    clogFull,
    clogSimple,
    clogNone
};

// Helper enum for compiler build method
enum CompilerBuildMethod
{
    cbmUseMake,
    cbmDirect
};

enum AutoDetectResult
{
    adrDetected,
    adrGuessed
};

/// Struct to keep programs
struct CompilerPrograms
{
    wxString C;
    wxString CPP;
    wxString LD;
    wxString WINDRES;
    wxString MAKE;
};

/// Struct to keep switches
struct CompilerSwitches
{
    wxString includeDirs;   // -I
    wxString libDirs;       // -L
    wxString linkLibs;      // -l
    wxString defines;       // -D
    wxString genericSwitch; // -
    wxString linkerSwitchForGui; // -mwindows
    wxString objectExtension; // o
    bool forceLinkerUseQuotes; // use quotes for filenames in linker command line (needed or not)?
    bool forceCompilerUseQuotes; // use quotes for filenames in compiler command line (needed or not)?
    bool needDependencies; // true
    CompilerLoggingType logging;
    CompilerBuildMethod buildMethod;
};

/**
  * @brief Abstract base class for compilers.
  *
  * Create a derived class and add it in compilerfactory.cpp
  */
class DLLIMPORT Compiler : public CompileOptionsBase
{
	public:
        /// Enum categorizing compiler's output line as warning/error/normal
        enum CompilerLineType
        {
            cltNormal,
            cltWarning,
            cltError
        };

		Compiler(const wxString& name);
		Compiler(const Compiler& other); // copy ctor to copy everything but update m_ID
		virtual ~Compiler();
		
		/** @brief Check if the supplied string is a compiler warning/error */
		virtual CompilerLineType CheckForWarningsAndErrors(const wxString& line) = 0;
		/** @brief Returns warning/error filename. Use it after a call to CheckForWarningsAndErrors() */
		virtual wxString GetLastErrorFilename(){ return m_ErrorFilename; }
		/** @brief Returns warning/error line number (as a string). Use it after a call to CheckForWarningsAndErrors() */
		virtual wxString GetLastErrorLine(){ return m_ErrorLine; }
		/** @brief Returns warning/error actual string. Use it after a call to CheckForWarningsAndErrors() */
		virtual wxString GetLastError(){ return m_Error; }
        /** @brief Get the compiler's name */
		virtual const wxString& GetName() const { return m_Name; }
        /** @brief Get the compiler's master path (must contain "bin", "include" and "lib") */
		virtual const wxString& GetMasterPath() const { return m_MasterPath; }
        /** @brief Get the compiler's programs */
		virtual const CompilerPrograms& GetPrograms() const { return m_Programs; }
        /** @brief Get the compiler's generic switches */
		virtual const CompilerSwitches& GetSwitches() const { return m_Switches; }
        /** @brief Get the compiler's options */
		virtual const CompilerOptions& GetOptions() const { return m_Options; }
        /** @brief Get a command based on CommandType */
		virtual const wxString& GetCommand(CommandType ct) const { return m_Commands[(int)ct]; }

        /** @brief Set the compiler's name */
		virtual void SetName(const wxString& name){ m_Name = name; }
        /** @brief Set the compiler's master path (must contain "bin", "include" and "lib") */
		virtual void SetMasterPath(const wxString& path){ m_MasterPath = path; }
        /** @brief Set the compiler's programs */
		virtual void SetPrograms(const CompilerPrograms& programs){ m_Programs = programs; }
        /** @brief Set the compiler's generic switches */
		virtual void SetSwitches(const CompilerSwitches& switches){ m_Switches = switches; }
        /** @brief Set the compiler's options */
		virtual void SetOptions(const CompilerOptions& options){ m_Options = options; }
        /** @brief Set a command based on CommandType */
		virtual void SetCommand(CommandType ct, const wxString& cmd){ m_Commands[(int)ct] = cmd; }

        void SaveSettings(const wxString& baseKey);
        void LoadSettings(const wxString& baseKey);
        virtual AutoDetectResult AutoDetectInstallationDir() = 0;

        long GetID() const { return m_ID; }
        long GetParentID() const { return m_ParentID; }

        static wxString CommandTypeDescriptions[COMPILER_COMMAND_TYPES_COUNT];
	protected:
        friend class CompilerFactory;
        virtual Compiler* CreateCopy() = 0; // override in new compilers, to return a new copy
        // set the following members in your class
        wxString m_Name;
        wxString m_MasterPath;
        wxString m_Commands[COMPILER_COMMAND_TYPES_COUNT];
        CompilerPrograms m_Programs;
        CompilerSwitches m_Switches;
        CompilerOptions m_Options;
        wxString m_ErrorFilename;
        wxString m_ErrorLine;
        wxString m_Error;
	private:
        long m_ID;
        long m_ParentID; // -1 for builtin compilers, the builtin compiler's ID to derive from for user compilers...
        static long CompilerIDCounter; // for builtin compilers
        static long UserCompilerIDCounter; // for user compilers
};

#endif // COMPILER_H
