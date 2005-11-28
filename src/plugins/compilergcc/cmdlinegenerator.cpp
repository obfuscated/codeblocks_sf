#include <sdk.h>
#include "cmdlinegenerator.h"
#include <wx/intl.h>
#include "cbproject.h"
#include "compilerfactory.h"
#include "compiler.h"
#include "manager.h"
#include "messagemanager.h"
#include "macrosmanager.h"

CmdLineGenerator::CmdLineGenerator()
{
    //ctor
}

CmdLineGenerator::~CmdLineGenerator()
{
    //dtor
}

void CmdLineGenerator::Init(cbProject* project)
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

    if (!project)
    {
        // access the default compiler
        Compiler* compiler = CompilerFactory::GetDefaultCompiler();

        SetupOutputFilenames(compiler, 0);
        SetupIncludeDirs(compiler, 0);
        SetupLibrariesDirs(compiler, 0);
        SetupResourceIncludeDirs(compiler, 0);
        SetupCompilerOptions(compiler, 0);
        SetupLinkerOptions(compiler, 0);
        SetupLinkLibraries(compiler, 0);
        SetupResourceCompilerOptions(compiler, 0);
        return;
    }

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

        // access the compiler used for this target
        Compiler* compiler = CompilerFactory::Compilers[target->GetCompilerIndex()];

        SetupOutputFilenames(compiler, target);
        SetupIncludeDirs(compiler, target);
        SetupLibrariesDirs(compiler, target);
        SetupResourceIncludeDirs(compiler, target);
        SetupCompilerOptions(compiler, target);
        SetupLinkerOptions(compiler, target);
        SetupLinkLibraries(compiler, target);
        SetupResourceCompilerOptions(compiler, target);
    }
}

void CmdLineGenerator::CreateSingleFileCompileCmd(wxString& command,
                                                    ProjectBuildTarget* target,
                                                    ProjectFile* pf,
                                                    const wxString& file,
                                                    const wxString& object,
                                                    const wxString& deps)
{
    Compiler* compiler = target
                            ? CompilerFactory::Compilers[target->GetCompilerIndex()]
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

    command.Replace(_T("$compiler"), compilerStr);
    command.Replace(_T("$linker"), compiler->GetPrograms().LD);
    command.Replace(_T("$lib_linker"), compiler->GetPrograms().LIB);
    command.Replace(_T("$rescomp"), compiler->GetPrograms().WINDRES);
    command.Replace(_T("$options"), m_CFlags[target]);
    command.Replace(_T("$link_options"), m_LDFlags[target]);
    command.Replace(_T("$includes"), m_Inc[target]);
    command.Replace(_T("$res_includes"), m_RC[target]);
    command.Replace(_T("$libdirs"), m_Lib[target]);
    command.Replace(_T("$libs"), m_LDAdd[target]);
    command.Replace(_T("$file"), file);
    command.Replace(_T("$dep_object"), deps);
    command.Replace(_T("$object"), object);
    command.Replace(_T("$resource_output"), object);
    if (!target)
    {
        // single file compilation, probably
        wxString object_unquoted(object);
        if (!object_unquoted.IsEmpty() && object_unquoted.GetChar(0) == '"')
            object_unquoted.Replace(_T("\""), _T(""));
        wxFileName fname(object_unquoted);
        fname.SetExt(EXECUTABLE_EXT);
        wxString output = fname.GetFullPath();
        command.Replace(_T("$exe_output"), output);
    }
    else
        command.Replace(_T("$exe_output"), m_Output[target]);
    command.Replace(_T("$link_resobjects"), deps);
    command.Replace(_T("$link_objects"), object);
    // the following were added to support the QUICK HACK
    // at directcommands.cpp:576
    command.Replace(_T("$+link_objects"), object);
    command.Replace(_T("$-link_objects"), object);
    command.Replace(_T("$-+link_objects"), object);
    command.Replace(_T("$+-link_objects"), object);

    if (target && (target->GetTargetType() == ttStaticLib || target->GetTargetType() == ttDynamicLib))
    {
        if (target->GetTargetType() == ttStaticLib || target->GetCreateStaticLib())
            command.Replace(_T("$static_output"), m_StaticOutput[target]);
        else
        {
            command.Replace(_T("-Wl,--out-implib=$static_output"), _T("")); // special gcc case
            command.Replace(_T("$static_output"), _T(""));
        }

        if (target->GetCreateDefFile())
            command.Replace(_T("$def_output"), m_DefOutput[target]);
        else
        {
            command.Replace(_T("-Wl,--output-def=$def_output"), _T("")); // special gcc case
            command.Replace(_T("$def_output"), _T(""));
        }
    }

    // finally, replace all macros in one go
    Manager::Get()->GetMacrosManager()->ReplaceMacros(command, true);
}

/// Setup output filename for build target.
void CmdLineGenerator::SetupOutputFilenames(Compiler* compiler, ProjectBuildTarget* target)
{
    if (!target)
        return;

    // exe file
    wxString result = target->GetOutputFilename();
    QuoteStringIfNeeded(result);
    m_Output[target] = result;

    // static
    wxFileName fname(target->GetOutputFilename());
    if (!fname.GetName().StartsWith(compiler->GetSwitches().libPrefix))
        fname.SetName(compiler->GetSwitches().libPrefix + fname.GetName());
    fname.SetExt(compiler->GetSwitches().libExtension);
    result = UnixFilename(fname.GetFullPath());
    QuoteStringIfNeeded(result);
    m_StaticOutput[target] = result;

    // def
    fname.SetExt(_T("def"));
    result = UnixFilename(fname.GetFullPath());
    QuoteStringIfNeeded(result);
    m_DefOutput[target] = result;
}

/// Setup compiler include dirs for build target.
void CmdLineGenerator::SetupIncludeDirs(Compiler* compiler, ProjectBuildTarget* target)
{
    wxString result;

    // for PCH to work, the very first include dir *must* be the object output dir
    // *only* if PCH is generated in the object output dir
    if (target &&
        target->GetParentProject()->GetModeForPCH() == pchObjectDir)
    {
        wxArrayString includedDirs; // avoid adding duplicate dirs...
        wxString sep = wxFILE_SEP_PATH;
        // find all PCH in project
        int count = target->GetParentProject()->GetFilesCount();
        for (int i = 0; i < count; ++i)
        {
            ProjectFile* f = target->GetParentProject()->GetFile(i);
            if (FileTypeOf(f->relativeFilename) == ftHeader &&
                f->compile)
            {
                // it is a PCH; add it's object dir to includes
                wxString dir = wxFileName(target->GetObjectOutput() + sep + f->GetObjName()).GetPath();
                if (includedDirs.Index(dir) == wxNOT_FOUND)
                {
                    includedDirs.Add(dir);
                    QuoteStringIfNeeded(dir);
                    result << compiler->GetSwitches().includeDirs << dir << _T(" ");
                }
            }
        }
    }

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
    m_Inc[target] = result;
}

/// Setup linker include dirs for build target.
void CmdLineGenerator::SetupLibrariesDirs(Compiler* compiler, ProjectBuildTarget* target)
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
    m_Lib[target] = result;
}

/// Setup resource compiler include dirs for build target.
void CmdLineGenerator::SetupResourceIncludeDirs(Compiler* compiler, ProjectBuildTarget* target)
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
    m_RC[target] = result;
}

/// Setup compiler flags for build target.
void CmdLineGenerator::SetupCompilerOptions(Compiler* compiler, ProjectBuildTarget* target)
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

    // add in array
    m_CFlags[target] = result;
}

/// Setup linker flags for build target.
void CmdLineGenerator::SetupLinkerOptions(Compiler* compiler, ProjectBuildTarget* target)
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

    // add in array
    m_LDFlags[target] = result;
}

/// Fix library name based on advanced compiler settings
wxString CmdLineGenerator::FixupLinkLibraries(Compiler* compiler, const wxString& lib)
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
void CmdLineGenerator::SetupLinkLibraries(Compiler* compiler, ProjectBuildTarget* target)
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
    m_LDAdd[target] = result;
}

/// Setup resource compiler flags for build target.
void CmdLineGenerator::SetupResourceCompilerOptions(Compiler* compiler, ProjectBuildTarget* target)
{
    // resource compiler options are not implemented in C::B yet
    m_RCFlags[target] = wxEmptyString;
}

/** Arrange order of options.
  * Depending on the order defined for the build target, it concatenates
  * @c project_options with @c target_options and returns the result.
  */
wxString CmdLineGenerator::GetOrderedOptions(ProjectBuildTarget* target, OptionsRelationType rel, const wxString& project_options, const wxString& target_options)
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
