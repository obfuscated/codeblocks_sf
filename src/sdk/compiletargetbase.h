#ifndef COMPILETARGETBASE_H
#define COMPILETARGETBASE_H

#include "compileoptionsbase.h"

/** Enum that defines the option's relation types */
enum OptionsRelationType
{
    ortCompilerOptions = 0, /**< Compiler option */
    ortLinkerOptions, /**< Linker option */
    ortIncludeDirs, /**< Compiler include dir option */
    ortLibDirs /**< Linker include dir option */
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
    ttExecutable = 0, /**< Target produces an executable */
    ttConsoleOnly, /**< Target produces a console executable (without GUI) (distinction between ttExecutable and ttConsoleOnly happens only under Win32) */
    ttStaticLib, /**< Target produces a static library */
    ttDynamicLib /**< Target produces a dynamic library */
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

        virtual const wxString& GetFilename(); 
        virtual const wxString& GetTitle(); ///< Read the target's title
        virtual void SetTitle(const wxString& title); ///< Set the target's title
        virtual void SetOutputFilename(const wxString& filename); ///< Set the target's output filename
        virtual void SetObjectOutput(const wxString& dirname); ///< Set the target's objects output dir
        virtual void SetDepsOutput(const wxString& dirname); ///< Set the target's dependencies output dir
        virtual OptionsRelation GetOptionRelation(OptionsRelationType type); ///< Read the target's options relation for \c type
        virtual void SetOptionRelation(OptionsRelationType type, OptionsRelation rel); ///< Set the target's options relation for \c type to \c rel
        virtual wxString GetObjectOutput(); ///< Read the target's objects output dir
        virtual wxString GetDepsOutput(); ///< Read the target's dependencies output dir
        virtual wxString GetOutputFilename(); ///< Read the target's output filename
        virtual wxString SuggestOutputFilename(); ///< Suggest a filename based on the target's type
        virtual wxString GetExecutableFilename(); ///< Read the target's executable filename (produced if target type is ttExecutable)
        virtual wxString GetDynamicLibFilename(); ///< Read the target's dynamic library filename (produced if target type is ttDynamicLib)
        virtual wxString GetDynamicLibDefFilename(); ///< Read the target's dynamic library definition file filename (produced if target type is ttDynamicLib)
        virtual wxString GetStaticLibFilename(); ///< Read the target's static library filename (produced if target type is ttStaticLib)
        virtual wxString GetBasePath(); ///< Read the target's base path, e.g. if GetOutputFilename() returns "/usr/local/bin/xxx", base path will return "/usr/local/bin"
        virtual void SetTargetType(const TargetType& pt); ///< Set the target's type to \c pt
        virtual const TargetType& GetTargetType(); ///< Read the target's type
        virtual const wxString& GetExecutionParameters(); ///< Read the target's execution parameters
        virtual void SetExecutionParameters(const wxString& params); ///< Set the target's execution parameters to \c params
		virtual const wxString& GetHostApplication(); ///< Read the target's host application
		virtual void SetHostApplication(const wxString& app); ///< Set the target's host application to \c app
        virtual void SetCompilerIndex(int compilerIdx);
        virtual int GetCompilerIndex(){ return m_CompilerIdx; }
	protected:
        friend class cbProject;
        wxString m_Filename;
        wxString m_Title;
        wxString m_OutputFilename;
        wxString m_ObjectOutput;
        wxString m_DepsOutput;
        wxString m_ExecutionParameters;
        wxString m_HostApplication;
        OptionsRelation m_OptionsRelation[4];
        TargetType m_TargetType;
        int m_CompilerIdx;
	private:
};

#endif // COMPILETARGETBASE_H

