#include "sdk_precomp.h"
#include "compilercommandgenerator.h"
#include <wx/intl.h>
#include "cbexception.h"
#include "cbproject.h"
#include "compilerfactory.h"
#include "compiler.h"
#include "manager.h"
#include "configmanager.h"
#include "messagemanager.h"
#include "macrosmanager.h"
#include "scriptingmanager.h"
#include "filefilters.h"

#include <sqplus.h>

CompilerCommandGenerator::CompilerCommandGenerator()
{
    //ctor
}

CompilerCommandGenerator::~CompilerCommandGenerator()
{
    //dtor
}

void CompilerCommandGenerator::Init(cbProject* project)
{
    // clear old arrays
    m_Output.clear();
    m_StaticOutput.clear();
    m_DefOutput.clear();
    m_Inc.clear();
    m_Lib.clear();
    m_RC.clear();
    m_CFlags.clear();
    m_LDFlags.clear();
    m_RCFlags.clear();
    m_Backticks.clear();

    // access the default compiler
    Compiler* compiler = CompilerFactory::GetDefaultCompiler();
    if (!compiler)
        cbThrow(_T("Default compiler is invalid!"));

    if (!project)
    {
        m_DefOutput[0] = SetupOutputFilenames(compiler, 0);
        m_Inc[0] = SetupIncludeDirs(compiler, 0);
        m_Lib[0] = SetupLibrariesDirs(compiler, 0);
        m_RC[0] = SetupResourceIncludeDirs(compiler, 0);
        m_CFlags[0] = SetupCompilerOptions(compiler, 0);
        m_LDFlags[0] = SetupLinkerOptions(compiler, 0);
        m_LDAdd[0] = SetupLinkLibraries(compiler, 0);
        m_RCFlags[0] = SetupResourceCompilerOptions(compiler, 0);
        return;
    }
    else
    {
        m_PrjIncPath = project->GetCommonTopLevelPath();
        if (!m_PrjIncPath.IsEmpty())
        {
            QuoteStringIfNeeded(m_PrjIncPath);
            m_PrjIncPath.Prepend(compiler->GetSwitches().includeDirs);
        }
    }

    // project pre-build scripts
    DoBuildScripts(project, _T("SetBuildOptions"));

    // for each target
    for (int i = 0; i < project->GetBuildTargetsCount(); ++i)
    {
        ProjectBuildTarget* target = project->GetBuildTarget(i);

        // for commands-only targets, nothing to setup
        // just add stub entries so that indices keep in sync
        if (target->GetTargetType() == ttCommandsOnly)
        {
            m_Output[target] = wxEmptyString;
            m_StaticOutput[target] = wxEmptyString;
            m_DefOutput[target] = wxEmptyString;
            m_Inc[target] = wxEmptyString;
            m_Lib[target] = wxEmptyString;
            m_RC[target] = wxEmptyString;
            m_CFlags[target] = wxEmptyString;
            m_LDFlags[target] = wxEmptyString;
            m_LDAdd[target] = wxEmptyString;
            m_RCFlags[target] = wxEmptyString;
            // continue with next target
            continue;
        }

        // target pre-build scripts
        DoBuildScripts(target, _T("SetBuildOptions"));

        // access the compiler used for this target
        compiler = CompilerFactory::GetCompiler(target->GetCompilerID());
        if (!compiler)
            cbThrow(_T("Compiler for target '") + target->GetTitle() + _T("' is invalid!"));

        m_DefOutput[target] = SetupOutputFilenames(compiler, target);
        m_Inc[target] = SetupIncludeDirs(compiler, target);
        m_Lib[target] = SetupLibrariesDirs(compiler, target);
        m_RC[target] = SetupResourceIncludeDirs(compiler, target);
        m_CFlags[target] = SetupCompilerOptions(compiler, target);
        m_LDFlags[target] = SetupLinkerOptions(compiler, target);
        m_LDAdd[target] = SetupLinkLibraries(compiler, target);
        m_RCFlags[target] = SetupResourceCompilerOptions(compiler, target);

        // target post-build scripts
        DoBuildScripts(target, _T("UnsetBuildOptions"));
    }

    // project post-build scripts
    DoBuildScripts(project, _T("UnsetBuildOptions"));
}

void CompilerCommandGenerator::GenerateCommandLine(wxString& macro,
                                                    ProjectBuildTarget* target,
                                                    ProjectFile* pf,
                                                    const wxString& file,
                                                    const wxString& object,
                                                    const wxString& FlatObject,
                                                    const wxString& deps)
{
    Compiler* compiler = target
                            ? CompilerFactory::GetCompiler(target->GetCompilerID())
                            : CompilerFactory::GetDefaultCompiler();
    wxString compilerStr;
    if (pf)
    {
        if (pf->compilerVar.Matches(_T("CPP")))
            compilerStr = compiler->GetPrograms().CPP;
        else if (pf->compilerVar.Matches(_T("CC")))
            compilerStr = compiler->GetPrograms().C;
        else if (pf->compilerVar.Matches(_T("WINDRES")))
            compilerStr = compiler->GetPrograms().WINDRES;
        else
            return;
    }
    else
    {
    	wxFileName fname(file);
    	if (fname.GetExt().Lower().Matches(_T("c")))
            compilerStr = compiler->GetPrograms().C;
        else
            compilerStr = compiler->GetPrograms().CPP;
    }

    wxString fileInc;
    if (Manager::Get()->GetConfigManager(_T("compiler"))->ReadBool(_T("/include_file_cwd"), false))
    {
        // Because C::B doesn't compile each file by running in the same directory with it,
        // it can cause some problems when the file #includes other files relative,
        // e.g. #include "../a_lib/include/a.h"
        //
        // So here we add the currently compiling file's directory to the includes
        // search dir so it works.
        wxFileName fileCwd = file;
        fileInc = fileCwd.GetPath();
        if (!fileInc.IsEmpty()) // only if non-empty! (remember r1813 errors)
        {
            QuoteStringIfNeeded(fileInc);
            fileInc.Prepend(compiler->GetSwitches().includeDirs);
        }
    }
    if (Manager::Get()->GetConfigManager(_T("compiler"))->ReadBool(_T("/include_prj_cwd"), false))
    {
        // Because C::B doesn't compile each file by running in the same directory with it,
        // it can cause some problems when the file #includes other files relative,
        // e.g. #include "../a_lib/include/a.h"
        //
        // So here we add the project's top-level directory (common toplevel path) to the includes
        // search dir so it works.
        fileInc << _T(' ') << m_PrjIncPath;
    }

    if (target)
    {  // this one has to come first, since wxString::Replace, otherwise $object would go first
    	// leaving nothing to replace for this $objects_output_dir
        macro.Replace(_T("$objects_output_dir"), target->GetObjectOutput());
    }
    macro.Replace(_T("$compiler"), compilerStr);
    macro.Replace(_T("$linker"), compiler->GetPrograms().LD);
    macro.Replace(_T("$lib_linker"), compiler->GetPrograms().LIB);
    macro.Replace(_T("$rescomp"), compiler->GetPrograms().WINDRES);
    macro.Replace(_T("$options"), m_CFlags[target]);
    macro.Replace(_T("$link_options"), m_LDFlags[target]);
    macro.Replace(_T("$includes"), m_Inc[target] + fileInc);
    macro.Replace(_T("$res_includes"), m_RC[target] + fileInc);
    macro.Replace(_T("$libdirs"), m_Lib[target]);
    macro.Replace(_T("$libs"), m_LDAdd[target]);
    macro.Replace(_T("$file"), file);
    macro.Replace(_T("$dep_object"), deps);
    macro.Replace(_T("$object"), object);
    macro.Replace(_T("$resource_output"), object);
    if (!target)
    {
        // single file compilation, probably
        wxString object_unquoted(object);
        if (!object_unquoted.IsEmpty() && object_unquoted.GetChar(0) == '"')
            object_unquoted.Replace(_T("\""), _T(""));
        wxFileName fname(object_unquoted);
        fname.SetExt(FileFilters::EXECUTABLE_EXT);
        wxString output = fname.GetFullPath();
        QuoteStringIfNeeded(output);
        macro.Replace(_T("$exe_output"), output);
    }
    else
    {
        macro.Replace(_T("$exe_output"), m_Output[target]);
    }
    macro.Replace(_T("$link_resobjects"), deps);
    macro.Replace(_T("$link_objects"), object);
    macro.Replace(_T("$link_flat_objects"), FlatObject);
    // the following were added to support the QUICK HACK in compiler plugin:
    // DirectCommands::GetTargetLinkCommands()
    macro.Replace(_T("$+link_objects"), object);
    macro.Replace(_T("$-link_objects"), object);
    macro.Replace(_T("$-+link_objects"), object);
    macro.Replace(_T("$+-link_objects"), object);

    if (target && (target->GetTargetType() == ttStaticLib || target->GetTargetType() == ttDynamicLib))
    {
        if (target->GetTargetType() == ttStaticLib || target->GetCreateStaticLib())
            macro.Replace(_T("$static_output"), m_StaticOutput[target]);
        else
        {
            macro.Replace(_T("-Wl,--out-implib=$static_output"), _T("")); // special gcc case
            macro.Replace(_T("$static_output"), _T(""));
        }

        if (target->GetCreateDefFile())
            macro.Replace(_T("$def_output"), m_DefOutput[target]);
        else
        {
            macro.Replace(_T("-Wl,--output-def=$def_output"), _T("")); // special gcc case
            macro.Replace(_T("$def_output"), _T(""));
        }
    }

    // finally, replace all macros in one go
    Manager::Get()->GetMacrosManager()->ReplaceMacros(macro, true, target);
}

/// Apply pre-build scripts for @c base.
void CompilerCommandGenerator::DoBuildScripts(CompileOptionsBase* base, const wxString& funcName)
{
    static const wxString clearout_buildscripts = _T("function SetBuildOptions(base){}; function UnsetBuildOptions(base){};");
    const wxArrayString& scripts = base->GetBuildScripts();
    for (size_t i = 0; i < scripts.GetCount(); ++i)
    {
        Manager::Get()->GetScriptingManager()->LoadScript(clearout_buildscripts); // clear previous script's context
        Manager::Get()->GetScriptingManager()->LoadScript(scripts[i]);
        try
        {
            SqPlus::SquirrelFunction<void>(cbU2C(funcName))(base);
        }
        catch (SquirrelError& e)
        {
            Manager::Get()->GetScriptingManager()->DisplayErrors(&e);
        }
    }
}

/// Setup output filename for build target.
wxString CompilerCommandGenerator::SetupOutputFilenames(Compiler* compiler, ProjectBuildTarget* target)
{
    if (!target)
        return wxEmptyString;

    // exe file
    wxString result = target->GetOutputFilename();
    QuoteStringIfNeeded(result);
    m_Output[target] = result;

	// Replace Variables FIRST to address the $(VARIABLE)libfoo.a problem
	// if $(VARIABLE) expands to /bar/ then wxFileName will still consider $(VARIABLE)libfoo.a a filename,
	// not a fully qualified path, so we will prepend lib to /bar/libfoo.a incorrectly
	// NOTE (thomas#1#): A better solution might be to use a regex, but finding an universal regex might not be easy...
    wxString fnameString(target->GetOutputFilename());
    Manager::Get()->GetMacrosManager()->ReplaceMacros(fnameString, true);
    wxFileName fname(fnameString);

    if (!fname.GetName().StartsWith(compiler->GetSwitches().libPrefix))
        fname.SetName(compiler->GetSwitches().libPrefix + fname.GetName());
    fname.SetExt(compiler->GetSwitches().libExtension);
    result = UnixFilename(fname.GetFullPath());
    QuoteStringIfNeeded(result);
    m_StaticOutput[target] = result;

    // def
    fname.SetExt(_T("def"));
    result = UnixFilename(fname.GetFullPath());
    QuoteStringIfNeeded(result); // NOTE (thomas#1#): Do we really need to call QuoteStringIfNeeded that often? ReplaceMacros already does it, and we do it twice again without ever possibly adding whitespace
    return result;
}

/// Setup compiler include dirs for build target.
wxString CompilerCommandGenerator::SetupIncludeDirs(Compiler* compiler, ProjectBuildTarget* target)
{
    wxString result;

    if (target)
    {
        // target dirs
        wxString tstr;
        const wxArrayString& arr = target->GetIncludeDirs();
        for (unsigned int x = 0; x < arr.GetCount(); ++x)
        {
            wxString tmp = arr[x];
            QuoteStringIfNeeded(tmp);
            tstr << compiler->GetSwitches().includeDirs << tmp << _T(' ');
        }

        // project dirs
        wxString pstr;
        const wxArrayString& parr = target->GetParentProject()->GetIncludeDirs();
        for (unsigned int x = 0; x < parr.GetCount(); ++x)
        {
            wxString tmp = parr[x];
            QuoteStringIfNeeded(tmp);
            pstr << compiler->GetSwitches().includeDirs << tmp << _T(' ');
        }

        // decide order
        result << GetOrderedOptions(target, ortIncludeDirs, pstr, tstr);
    }

    // compiler dirs
    const wxArrayString& carr = compiler->GetIncludeDirs();
    for (unsigned int x = 0; x < carr.GetCount(); ++x)
    {
        wxString tmp = carr[x];
        QuoteStringIfNeeded(tmp);
        result << compiler->GetSwitches().includeDirs << tmp << _T(' ');
    }

    // add in array
    return result;
}

/// Setup linker include dirs for build target.
wxString CompilerCommandGenerator::SetupLibrariesDirs(Compiler* compiler, ProjectBuildTarget* target)
{
    wxString result;

    if (target)
    {
        // target dirs
        wxString tstr;
        const wxArrayString& arr = target->GetLibDirs();
        for (unsigned int x = 0; x < arr.GetCount(); ++x)
        {
            wxString tmp = arr[x];
            QuoteStringIfNeeded(tmp);
            tstr << compiler->GetSwitches().libDirs << tmp << _T(' ');
        }

        // project dirs
        wxString pstr;
        const wxArrayString& parr = target->GetParentProject()->GetLibDirs();
        for (unsigned int x = 0; x < parr.GetCount(); ++x)
        {
            wxString tmp = parr[x];
            QuoteStringIfNeeded(tmp);
            pstr << compiler->GetSwitches().libDirs << tmp << _T(' ');
        }

        // decide order
        result = GetOrderedOptions(target, ortLibDirs, pstr, tstr);
    }

    // compiler dirs
    const wxArrayString& carr = compiler->GetLibDirs();
    for (unsigned int x = 0; x < carr.GetCount(); ++x)
    {
        wxString cstr = carr[x];
        QuoteStringIfNeeded(cstr);
        result << compiler->GetSwitches().libDirs << cstr << _T(' ');
    }

    // add in array
    return result;
}

/// Setup resource compiler include dirs for build target.
wxString CompilerCommandGenerator::SetupResourceIncludeDirs(Compiler* compiler, ProjectBuildTarget* target)
{
    wxString result;

    if (target)
    {
        // target dirs
        wxString tstr;
        const wxArrayString& arr = target->GetResourceIncludeDirs();
        for (unsigned int x = 0; x < arr.GetCount(); ++x)
        {
            wxString tmp = arr[x];
            QuoteStringIfNeeded(tmp);
            tstr << compiler->GetSwitches().includeDirs << tmp << _T(' ');
        }

        // project dirs
        wxString pstr;
        const wxArrayString& parr = target->GetParentProject()->GetResourceIncludeDirs();
        for (unsigned int x = 0; x < parr.GetCount(); ++x)
        {
            wxString tmp = parr[x];
            QuoteStringIfNeeded(tmp);
            pstr << compiler->GetSwitches().includeDirs << tmp << _T(' ');
        }

        // decide order
        result = GetOrderedOptions(target, ortResDirs, pstr, tstr);
    }

    // compiler dirs
    const wxArrayString& carr = compiler->GetResourceIncludeDirs();
    for (unsigned int x = 0; x < carr.GetCount(); ++x)
    {
        wxString cstr = carr[x];
        QuoteStringIfNeeded(cstr);
        result << compiler->GetSwitches().includeDirs << cstr << _T(' ');
    }

    // add in array
    return result;
}

/// Setup compiler flags for build target.
wxString CompilerCommandGenerator::SetupCompilerOptions(Compiler* compiler, ProjectBuildTarget* target)
{
    wxString result;

    if (target)
    {
        // target options
        wxString tstr = GetStringFromArray(target->GetCompilerOptions(), _T(' '));

        // project options
        wxString pstr = GetStringFromArray(target->GetParentProject()->GetCompilerOptions(), _T(' '));

        // decide order
        result = GetOrderedOptions(target, ortCompilerOptions, pstr, tstr);
    }

    // compiler options
    result << GetStringFromArray(compiler->GetCompilerOptions(), _T(' '));

    ExpandBackticks(result);

    // add in array
    return result;
}

/// Setup linker flags for build target.
wxString CompilerCommandGenerator::SetupLinkerOptions(Compiler* compiler, ProjectBuildTarget* target)
{
    wxString result;

    if (target)
    {
        // target options
        wxString tstr = GetStringFromArray(target->GetLinkerOptions(), _T(' '));

        // project options
        wxString pstr = GetStringFromArray(target->GetParentProject()->GetLinkerOptions(), _T(' '));

        // decide order
        result = GetOrderedOptions(target, ortLinkerOptions, pstr, tstr);
    }

    // linker options
    result << GetStringFromArray(compiler->GetLinkerOptions(), _T(' '));

    ExpandBackticks(result);

    // add in array
    return result;
}

/// Fix library name based on advanced compiler settings
wxString CompilerCommandGenerator::FixupLinkLibraries(Compiler* compiler, const wxString& lib)
{
    if (lib.IsEmpty())
        return wxEmptyString;

    wxString result = lib;

    // construct linker option for each result, based on compiler's settings
    wxString libPrefix = compiler->GetSwitches().libPrefix;
    wxString libExt = compiler->GetSwitches().libExtension;
    QuoteStringIfNeeded(result);
    // run replacements on libs only if no slashes in name (which means it's a relative or absolute path)
    if (result.Find('/') == -1 && result.Find('\\') == -1)
    {
        // 'result' prefix
        bool hadLibPrefix = false;
        if (!compiler->GetSwitches().linkerNeedsLibPrefix &&
            !libPrefix.IsEmpty() &&
            result.StartsWith(libPrefix))
        {
            result.Remove(0, libPrefix.Length());
            hadLibPrefix = true;
        }
        // extension
        if (!compiler->GetSwitches().linkerNeedsLibExtension &&
            result.Length() > libExt.Length() &&
            result.Right(libExt.Length() + 1) == _T(".") + libExt)
        {
            // remove the extension only if we had a result prefix
            if (hadLibPrefix)
                result.RemoveLast(libExt.Length() + 1);
        }
        else if (compiler->GetSwitches().linkerNeedsLibExtension &&
                !libExt.IsEmpty())
        {
            if (result.Length() <= libExt.Length() ||
                result.Right(libExt.Length() + 1) != _T(".") + libExt)
            {
                result << _T(".") << libExt;
            }
        }
        result = compiler->GetSwitches().linkLibs + result;
    }
    return result;
}

/// Setup link libraries for build target.
wxString CompilerCommandGenerator::SetupLinkLibraries(Compiler* compiler, ProjectBuildTarget* target)
{
    wxString result;

    if (target)
    {
        // target options
        wxString tstr;
        const wxArrayString& arr = target->GetLinkLibs();
        for (unsigned int x = 0; x < arr.GetCount(); ++x)
            tstr << FixupLinkLibraries(compiler, arr[x]) << _T(' ');

        // project options
        wxString pstr;
        const wxArrayString& parr = target->GetParentProject()->GetLinkLibs();
        for (unsigned int x = 0; x < parr.GetCount(); ++x)
            pstr << FixupLinkLibraries(compiler, parr[x]) << _T(' ');

        // decide order
        result = GetOrderedOptions(target, ortLinkerOptions, pstr, tstr);
    }

    // compiler link libraries
    result << GetStringFromArray(compiler->GetLinkLibs(), _T(' '));

    // add in array
    return result;
}

/// Setup resource compiler flags for build target.
wxString CompilerCommandGenerator::SetupResourceCompilerOptions(Compiler* compiler, ProjectBuildTarget* target)
{
    // resource compiler options are not implemented in C::B yet
    return wxEmptyString;
}

/** Arrange order of options.
  * Depending on the order defined for the build target, it concatenates
  * @c project_options with @c target_options and returns the result.
  */
wxString CompilerCommandGenerator::GetOrderedOptions(ProjectBuildTarget* target, OptionsRelationType rel, const wxString& project_options, const wxString& target_options)
{
    wxString result;
    OptionsRelation relation = target->GetOptionRelation(rel);
    switch (relation)
    {
        case orUseParentOptionsOnly:
            result << project_options;
            break;
        case orUseTargetOptionsOnly:
            result << target_options;
            break;
        case orPrependToParentOptions:
            result << target_options << project_options;
            break;
        case orAppendToParentOptions:
            result << project_options << target_options;
            break;
    }
    return result;
}

/** Adds support for backtick'd expressions under windows. */
void CompilerCommandGenerator::ExpandBackticks(wxString& str)
{
    // only for windows...
    // real OSes support this natively ;)
#ifdef __WXMSW__
    size_t start = str.find(_T('`'));
    if (start == wxString::npos)
        return; // no backticks here
    size_t end = str.find(_T('`'), start + 1);
    if (end == wxString::npos)
        return; // no ending backtick; error?

    while (start != wxString::npos && end != wxString::npos)
    {
        wxString cmd = str.substr(start + 1, end - start - 1);
        if (cmd.IsEmpty())
            break;

        wxString bt;
        BackticksMap::iterator it = m_Backticks.find(cmd);
        if (it != m_Backticks.end())
        {
            // in cache :)
            bt = it->second;
        }
        else
        {
            wxArrayString output;
            if (wxGetOsVersion() == wxWINDOWS_NT)
                wxExecute(_T("cmd /c ") + cmd, output, wxEXEC_NODISABLE);
            else
                wxExecute(cmd, output, wxEXEC_NODISABLE);
            bt = GetStringFromArray(output, _T(" "));
            // add it in the cache
            m_Backticks[cmd] = bt;
        }
        str = str.substr(0, start) + bt + str.substr(end + 1, wxString::npos);

        // find next occurrence
        start = str.find(_T('`'));
        end = str.find(_T('`'), start + 1);
    }
#endif
}
