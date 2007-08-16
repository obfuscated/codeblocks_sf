#ifndef COMPILETARGETBASE_H
#define COMPILETARGETBASE_H

#include "compileoptionsbase.h"

/** Enum that defines the option's relation types */
enum OptionsRelationType
{
    ortCompilerOptions = 0, /**< Compiler option */
    ortLinkerOptions, /**< Linker option */
    ortIncludeDirs, /**< Compiler include dir option */
    ortLibDirs, /**< Linker include dir option */
    ortResDirs, /**< Resource compiler include dir option */

    ortLast
};

/** Option's relation */
enum OptionsRelation
{
    orUseParentOptionsOnly = 0, /**< The option uses parent options only */
    orUseTargetOptionsOnly, /**< The option uses target options only */
    orPrependToParentOptions, /**< The option uses parent options appended to target options */
    orAppendToParentOptions /**< The option uses target options appended to parent options */
};

/** Enum to define the type of output the target produces */
enum TargetType
{
    ttExecutable    = 0, /**< Target produces an executable */
    ttConsoleOnly   = 1, /**< Target produces a console executable (without GUI) (distinction between ttExecutable and ttConsoleOnly happens only under Win32) */
    ttStaticLib		= 2, /**< Target produces a static library */
    ttDynamicLib    = 3, /**< Target produces a dynamic library */
    ttCommandsOnly  = 4, /**< Target only runs commands in pre-build and/or post-build steps */
    ttNative        = 5, /**< Target produces a native binary */
};

enum MakeCommand
{
    mcClean = 0,
    mcDistClean,
    mcBuild,
    mcCompileFile,

    /// *Don't* use this. It's only used internally for enumerations...
    mcLast
};

/** A target's filename can either be auto-generated based on the running platform,
  * or completely specified by the user. For more info, see
  * CompileTargetBase::SetTargetFilenameGenerationPolicy.
  */
enum TargetFilenameGenerationPolicy
{
    tgfpPlatformDefault = 0, ///< Generate filename based on running platform defaults.
    tgfpNone ///< No automatic generation; let the user specify the full filename.
};

/**
  * @brief Base class for build target classes
  * Each Code::Blocks project
  * consists of at least one target. Each target has different settings,
  * e.g.:
  * \li Build options,
  * \li Output type,
  * \li Execution parameters, etc.
  * \n\n
  * This class holds the settings of one build target.
 */
class DLLIMPORT CompileTargetBase : public CompileOptionsBase
{
	public:
		CompileTargetBase();
		virtual ~CompileTargetBase();

        /** A target's filename can either be auto-generated based on the running platform,
          * or completely specified by the user. Calling this function sets the
          * filename generation method.
          * @par The filename is divided in 4 parts. Let's see how "Debug/libSomeLib.a"
          * is divided:
          * @li the directory part: @c Debug,
          * @li the filename's prefix: @c lib,
          * @li the base name: @c SomeLib and
          * @li the extension: @c a
          * @par
          * Calling this function defines if the prefix and extension are auto-generated
          * or are left as the user specified. So, if the prefix is set to auto-generated
          * (i.e. @c tgfpPlatformDefault), it would be set depending on the running
          * platform and compiler, e.g.:
          * @li Windows & GCC: @c lib
          * @li Windows & MSVC: @c \<empty\>
          * @li Linux & <*>: @c lib
          * @par
          * The default generation policy is @c tgfpPlatformDefault for both the prefix
          * and the extension.
          * @note The ProjectLoader detects old projects when loaded and, for those, it
          * sets the default generation policy to @c tgfpNone (i.e. no auto-generation)
          * for both the prefix and the extension. This is done so the user doesn't
          * notice any unexpected behaviour...
          */
        virtual void SetTargetFilenameGenerationPolicy(TargetFilenameGenerationPolicy prefix,
                                                        TargetFilenameGenerationPolicy extension);
        virtual void GetTargetFilenameGenerationPolicy(TargetFilenameGenerationPolicy& prefixOut,
                                                        TargetFilenameGenerationPolicy& extensionOut) const;

        virtual const wxString& GetFilename() const;
        virtual const wxString& GetTitle() const; ///< Read the target's title
        virtual void SetTitle(const wxString& title); ///< Set the target's title
        virtual void SetOutputFilename(const wxString& filename); ///< Set the target's output filename
        virtual void SetWorkingDir(const wxString& dirname); ///< Set the target's working dir on execution (valid only for executable targets)
        virtual void SetObjectOutput(const wxString& dirname); ///< Set the target's objects output dir
        virtual void SetDepsOutput(const wxString& dirname); ///< Set the target's dependencies output dir
        virtual OptionsRelation GetOptionRelation(OptionsRelationType type) const; ///< Read the target's options relation for \c type
        virtual void SetOptionRelation(OptionsRelationType type, OptionsRelation rel); ///< Set the target's options relation for \c type to \c rel
        virtual wxString GetWorkingDir(); ///< Read the target's working dir for execution (valid only for executable targets)
        virtual wxString GetObjectOutput() const; ///< Read the target's objects output dir
        virtual wxString GetDepsOutput() const; ///< Read the target's dependencies output dir
        virtual wxString GetOutputFilename() ; ///< Read the target's output filename
        virtual wxString SuggestOutputFilename(); ///< Suggest a filename based on the target's type
        virtual wxString GetExecutableFilename() const; ///< Read the target's executable filename (produced if target type is ttExecutable)
        virtual wxString GetDynamicLibFilename(); ///< Read the target's dynamic library filename (produced if target type is ttDynamicLib)
        virtual wxString GetDynamicLibDefFilename(); ///< Read the target's dynamic library definition file filename (produced if target type is ttDynamicLib)
        virtual wxString GetStaticLibFilename(); ///< Read the target's static library filename (produced if target type is ttStaticLib)
        virtual wxString GetNativeFilename(); ///< Read the target's native filename (produced if target type is ttNative)
        virtual wxString GetBasePath() const; ///< Read the target's base path, e.g. if GetFilename() returns "/usr/local/bin/xxx", base path will return "/usr/local/bin"
        virtual void SetTargetType(TargetType pt); ///< Set the target's type to \c pt
        virtual TargetType GetTargetType() const; ///< Read the target's type
        virtual const wxString& GetExecutionParameters() const; ///< Read the target's execution parameters
        virtual void SetExecutionParameters(const wxString& params); ///< Set the target's execution parameters to \c params
		virtual const wxString& GetHostApplication() const; ///< Read the target's host application
		virtual void SetHostApplication(const wxString& app); ///< Set the target's host application to \c app
        virtual void SetCompilerID(const wxString& id); ///< Set the target's compiler
        virtual const wxString& GetCompilerID() const { return m_CompilerId; } ///< Read the target's compiler
        virtual wxString GetMakeCommandFor(MakeCommand cmd) const { return m_MakeCommands[cmd]; } ///< Get the "make" command used for @c cmd
        virtual void SetMakeCommandFor(MakeCommand cmd, const wxString& make); ///< Set the "make" command used for @c cmd
        virtual bool MakeCommandsModified() const { return m_MakeCommandsModified; } ///< True if any of the "make" commands is modified.
	protected:
        friend class cbProject;

        wxString m_Filename;
        wxString m_Title;
        wxString m_OutputFilename;
        wxString m_WorkingDir;
        wxString m_ObjectOutput;
        wxString m_DepsOutput;
        wxString m_ExecutionParameters;
        wxString m_HostApplication;
        OptionsRelation m_OptionsRelation[ortLast];
        TargetType m_TargetType;
        wxString m_CompilerId;
        wxString m_MakeCommands[mcLast];
        bool m_MakeCommandsModified;
        TargetFilenameGenerationPolicy m_PrefixGenerationPolicy;
        TargetFilenameGenerationPolicy m_ExtensionGenerationPolicy;
	private:
        void GenerateTargetFilename(wxString& filename) const;
};

#endif // COMPILETARGETBASE_H
