/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/filename.h>

    #include "compiletargetbase.h"
    #include "compilerfactory.h"
    #include "globals.h"
    #include "logmanager.h" // Manager::Get()->GetLogManager()->DebugLog(F())
#endif

#include "filefilters.h"

CompileTargetBase::CompileTargetBase()
    : m_TargetType(ttExecutable),
    m_RunHostApplicationInTerminal(false),
    m_PrefixGenerationPolicy(tgfpPlatformDefault),
    m_ExtensionGenerationPolicy(tgfpPlatformDefault)
{
    //ctor
    for (int i = 0; i < static_cast<int>(ortLast); ++i)
    {
        m_OptionsRelation[i] = orAppendToParentOptions;
    }

    // default "make" commands
    m_MakeCommands[mcBuild]             = _T("$make -f $makefile $target");
    m_MakeCommands[mcCompileFile]       = _T("$make -f $makefile $file");
    m_MakeCommands[mcClean]             = _T("$make -f $makefile clean$target");
    m_MakeCommands[mcDistClean]         = _T("$make -f $makefile distclean$target");
    m_MakeCommands[mcAskRebuildNeeded]  = _T("$make -q -f $makefile $target");
    m_MakeCommands[mcSilentBuild]       = _T("$make -s -f $makefile $target");
    m_MakeCommandsModified = false;
}

CompileTargetBase::~CompileTargetBase()
{
    //dtor
}

void CompileTargetBase::SetTargetFilenameGenerationPolicy(TargetFilenameGenerationPolicy prefix,
                                                        TargetFilenameGenerationPolicy extension)
{
    m_PrefixGenerationPolicy = prefix;
    m_ExtensionGenerationPolicy = extension;
    SetModified(true);
}

void CompileTargetBase::GetTargetFilenameGenerationPolicy(TargetFilenameGenerationPolicy& prefixOut,
                                                        TargetFilenameGenerationPolicy& extensionOut) const
{
    prefixOut = m_PrefixGenerationPolicy;
    extensionOut = m_ExtensionGenerationPolicy;
} // end of GetTargetFilenameGenerationPolicy

const wxString& CompileTargetBase::GetFilename() const
{
    return m_Filename;
}

const wxString& CompileTargetBase::GetTitle() const
{
    return m_Title;
}

void CompileTargetBase::SetTitle(const wxString& title)
{
    if (m_Title == title)
        return;
    m_Title = title;
    SetModified(true);
}

void CompileTargetBase::SetOutputFilename(const wxString& filename)
{
    if (filename.IsEmpty())
    {
        m_OutputFilename = SuggestOutputFilename();
        SetModified(true);
        return;
    }
    else if (m_OutputFilename == filename)
        return;
    m_OutputFilename = UnixFilename(filename);
    GenerateTargetFilename(m_OutputFilename);
    SetModified(true);
}

void CompileTargetBase::SetImportLibraryFilename(const wxString& filename)
{
    if (filename.IsEmpty())
    {
        m_ImportLibraryFilename = _T("$(TARGET_NAME)");
        SetModified(true);
        return;
    }
    else if (m_ImportLibraryFilename == filename)
        return;

    m_ImportLibraryFilename = UnixFilename(filename);
}

void CompileTargetBase::SetDefinitionFileFilename(const wxString& filename)
{
    if (filename.IsEmpty())
    {
        m_DefinitionFileFilename = _T("$(TARGET_NAME)");
        SetModified(true);
        return;
    }
    else if (m_DefinitionFileFilename == filename)
        return;

    m_DefinitionFileFilename = UnixFilename(filename);
}

void CompileTargetBase::SetWorkingDir(const wxString& dirname)
{
    if (m_WorkingDir == dirname)
        return;
    m_WorkingDir = UnixFilename(dirname);
    SetModified(true);
}

void CompileTargetBase::SetObjectOutput(const wxString& dirname)
{
    if (m_ObjectOutput == dirname)
        return;
    m_ObjectOutput = UnixFilename(dirname);
    SetModified(true);
}

void CompileTargetBase::SetDepsOutput(const wxString& dirname)
{
    if (m_DepsOutput == dirname)
        return;
    m_DepsOutput = UnixFilename(dirname);
    SetModified(true);
}

OptionsRelation CompileTargetBase::GetOptionRelation(OptionsRelationType type) const
{
    return m_OptionsRelation[type];
}

void CompileTargetBase::SetOptionRelation(OptionsRelationType type, OptionsRelation rel)
{
    if (m_OptionsRelation[type] == rel)
        return;
    m_OptionsRelation[type] = rel;
    SetModified(true);
}

wxString CompileTargetBase::GetOutputFilename()
{
    if (m_TargetType == ttCommandsOnly)
        return wxEmptyString;
    if (m_OutputFilename.IsEmpty())
        m_OutputFilename = SuggestOutputFilename();
    return m_OutputFilename;
}

wxString CompileTargetBase::SuggestOutputFilename()
{
    wxString suggestion;
    switch (m_TargetType)
    {
        case ttConsoleOnly: // fall through
        case ttExecutable:  suggestion = GetExecutableFilename(); break;
        case ttDynamicLib:  suggestion = GetDynamicLibFilename(); break;
        case ttStaticLib:   suggestion = GetStaticLibFilename();  break;
        case ttNative:      suggestion = GetNativeFilename();     break;
        default:
            suggestion.Clear();
            break;
    }
    wxFileName fname(suggestion);
    return UnixFilename(fname.GetFullName());
}

wxString CompileTargetBase::GetWorkingDir()
{
    if (m_TargetType != ttConsoleOnly && m_TargetType != ttExecutable && m_TargetType != ttDynamicLib)
        return wxEmptyString;
    wxString out;
    if (m_WorkingDir.IsEmpty())
    {
        out = GetOutputFilename();
        return wxFileName(out).GetPath(wxPATH_GET_VOLUME);
    }
    return m_WorkingDir;
}

wxString CompileTargetBase::GetObjectOutput() const
{
    if (m_TargetType == ttCommandsOnly)
        return wxEmptyString;
    wxString out;
    if (m_ObjectOutput.IsEmpty())
    {
        out = GetBasePath();
        if (out.IsEmpty() || out.Matches(_T(".")))
             return _T(".objs");
        else
            return out + wxFileName::GetPathSeparator() + _T(".objs");
    }
    return m_ObjectOutput;
}

wxString CompileTargetBase::GetDepsOutput() const
{
    if (m_TargetType == ttCommandsOnly)
        return wxEmptyString;
    wxString out;
    if (m_DepsOutput.IsEmpty())
    {
        out = GetBasePath();
        if (out.IsEmpty() || out.Matches(_T(".")))
             return _T(".deps");
        else
            return out + wxFileName::GetPathSeparator() + _T(".deps");
    }
    return m_DepsOutput;
}

void CompileTargetBase::GenerateTargetFilename(wxString& filename) const
{
    // nothing to do if no auto-generation
    if (   m_PrefixGenerationPolicy    == tgfpNone
        && m_ExtensionGenerationPolicy == tgfpNone )
        return;

    wxFileName fname(filename);
    filename.Clear();
    // path with volume and separator
    filename << fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    // prefix + name + extension
    switch (m_TargetType)
    {
        case ttConsoleOnly:
        case ttExecutable:
        {
            if (m_ExtensionGenerationPolicy == tgfpPlatformDefault)
            {
                filename << fname.GetName();
                filename << FileFilters::EXECUTABLE_DOT_EXT;
            }
            else
                filename << fname.GetFullName();
            break;
        }
        case ttDynamicLib:
        {
            if (m_PrefixGenerationPolicy == tgfpPlatformDefault)
            {
                wxString prefix = wxEmptyString;
                // On linux, "lib" is th common prefix for this platform
                if (platform::linux)
                    prefix = wxT("lib");
                // FIXME (mortenmacfly#5#): What about Mac (Windows is OK)?!

                // avoid adding the prefix, if there is no prefix, or already its there
                if (!prefix.IsEmpty() && !fname.GetName().StartsWith(prefix))
                    filename << prefix;
            }
            if (m_ExtensionGenerationPolicy == tgfpPlatformDefault)
                filename << fname.GetName() << FileFilters::DYNAMICLIB_DOT_EXT;
            else
                filename << fname.GetFullName();
            break;
        }
        case ttNative:
        {
            if (m_ExtensionGenerationPolicy == tgfpPlatformDefault)
                filename << fname.GetName() << FileFilters::NATIVE_DOT_EXT;
            else
                filename << fname.GetFullName();
            break;
        }
        case ttStaticLib:
        {
            if (m_PrefixGenerationPolicy == tgfpPlatformDefault)
            {
                Compiler* compiler = CompilerFactory::GetCompiler(m_CompilerId);
                wxString prefix = compiler ? compiler->GetSwitches().libPrefix : _T("");
                // avoid adding the prefix, if already there
                if (!prefix.IsEmpty() && !fname.GetName().StartsWith(prefix))
                    filename << prefix;
            }
            if (m_ExtensionGenerationPolicy == tgfpPlatformDefault)
            {
                Compiler* compiler = CompilerFactory::GetCompiler(m_CompilerId);
                wxString Ext = compiler ? compiler->GetSwitches().libExtension : FileFilters::STATICLIB_EXT;
                filename << fname.GetName() << _T(".") << Ext;
            }
            else
            {
                filename << fname.GetFullName();
            }
            break;
        }
        default:
            filename.Clear();
            break;
    }

#ifdef command_line_generation
    Manager::Get()->GetLogManager()->DebugLog(F(_T("CompileTargetBase::GenerateTargetFilename got %s and returns: '%s'"), fname.GetFullPath().wx_str(), filename.wx_str()));
#endif
}

wxString CompileTargetBase::GetExecutableFilename() const
{
    if (m_TargetType == ttCommandsOnly)
        return wxEmptyString;

    if (m_PrefixGenerationPolicy != tgfpNone || m_ExtensionGenerationPolicy != tgfpNone)
    {
        wxString out = m_Filename;
        GenerateTargetFilename(out);
        return out;
    }

    wxFileName fname(m_Filename);
#ifdef __WXMSW__
    fname.SetExt(FileFilters::EXECUTABLE_EXT);
#else
    fname.SetExt(_T(""));
#endif
    return fname.GetFullPath();
}

wxString CompileTargetBase::GetNativeFilename()
{
    if (m_TargetType == ttCommandsOnly)
        return wxEmptyString;

    if (m_Filename.IsEmpty())
        m_Filename = m_OutputFilename;

    if (m_PrefixGenerationPolicy != tgfpNone || m_ExtensionGenerationPolicy != tgfpNone)
    {
        wxString out = m_Filename;
        GenerateTargetFilename(out);
        return out;
    }

    wxFileName fname(m_Filename);
    fname.SetName(fname.GetName());
    fname.SetExt(FileFilters::NATIVE_EXT);
    return fname.GetFullPath();
}

wxString CompileTargetBase::GetDynamicLibFilename()
{
    if (m_TargetType == ttCommandsOnly)
        return wxEmptyString;

    if (m_Filename.IsEmpty())
        m_Filename = m_OutputFilename;

    if (m_PrefixGenerationPolicy != tgfpNone || m_ExtensionGenerationPolicy != tgfpNone)
    {
        wxString out = m_Filename;
        GenerateTargetFilename(out);
#ifdef command_line_generation
        Manager::Get()->GetLogManager()->DebugLog(F(_T("CompileTargetBase::GetDynamicLibFilename [0] returns: '%s'"), out.wx_str()));
#endif
        return out;
    }

    wxFileName fname(m_Filename);
    fname.SetName(fname.GetName());
    fname.SetExt(FileFilters::DYNAMICLIB_EXT);

#ifdef command_line_generation
    Manager::Get()->GetLogManager()->DebugLog(F(_T("CompileTargetBase::GetDynamicLibFilename [1] returns: '%s'"), fname.GetFullPath().wx_str()));
#endif
    return fname.GetFullPath();
}

wxString CompileTargetBase::GetDynamicLibImportFilename()
{
    if (m_TargetType == ttCommandsOnly)
        return wxEmptyString;

    if (m_ImportLibraryFilename.IsEmpty())
        m_ImportLibraryFilename = _T("$(TARGET_OUTPUT_DIR)$(TARGET_OUTPUT_BASENAME)");

    wxFileName fname(m_ImportLibraryFilename);

#ifdef command_line_generation
    Manager::Get()->GetLogManager()->DebugLog(F(_T("CompileTargetBase::GetDynamicLibImportFilename returns: '%s'"), fname.GetFullPath().wx_str()));
#endif
    return fname.GetFullPath();
}

wxString CompileTargetBase::GetDynamicLibDefFilename()
{
    if (m_TargetType == ttCommandsOnly)
        return wxEmptyString;

    if (m_DefinitionFileFilename.IsEmpty())
        m_DefinitionFileFilename = _T("$(TARGET_OUTPUT_DIR)$(TARGET_OUTPUT_BASENAME)");

    wxFileName fname(m_DefinitionFileFilename);

#ifdef command_line_generation
    Manager::Get()->GetLogManager()->DebugLog(F(_T("CompileTargetBase::GetDynamicLibDefFilename returns: '%s'"), fname.GetFullPath().wx_str()));
#endif
    return fname.GetFullPath();
}

wxString CompileTargetBase::GetStaticLibFilename()
{
    if (m_TargetType == ttCommandsOnly)
        return wxEmptyString;

    if (m_Filename.IsEmpty())
        m_Filename = m_OutputFilename;

    /* NOTE: There is no need to check for Generation policy for import library
       if target type is ttDynamicLib. */
    if (   (m_TargetType == ttStaticLib)
        && (   m_PrefixGenerationPolicy    != tgfpNone
            || m_ExtensionGenerationPolicy != tgfpNone) )
    {
        wxString out = m_Filename;
        GenerateTargetFilename(out);
#ifdef command_line_generation
        Manager::Get()->GetLogManager()->DebugLog(F(_T("CompileTargetBase::GetStaticLibFilename [0] returns: '%s'"), out.wx_str()));
#endif
        return out;
    }

    wxFileName fname(m_Filename);

    wxString prefix = _T("lib");
    wxString suffix = FileFilters::STATICLIB_EXT;
    Compiler* compiler = CompilerFactory::GetCompiler(m_CompilerId);
    if (compiler)
    {
        prefix = compiler->GetSwitches().libPrefix;
        suffix = compiler->GetSwitches().libExtension;
    }
    if (!fname.GetName().StartsWith(prefix))
        fname.SetName(prefix + fname.GetName());
    fname.SetExt(suffix);

#ifdef command_line_generation
    Manager::Get()->GetLogManager()->DebugLog(F(_T("CompileTargetBase::GetStaticLibFilename [1] returns: '%s'"), fname.GetFullPath().wx_str()));
#endif
    return fname.GetFullPath();
}

wxString CompileTargetBase::GetBasePath() const
{
    if (m_Filename.IsEmpty())
        return _T(".");

    wxFileName basePath(m_Filename);
    wxString base = basePath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    return !base.IsEmpty() ? base : _T(".");
}

void CompileTargetBase::SetTargetType(TargetType pt)
{
    if (m_TargetType == pt)
        return;

    m_TargetType = pt;
    m_OutputFilename = SuggestOutputFilename();
    SetModified(true);
}

TargetType CompileTargetBase::GetTargetType() const
{
    return m_TargetType;
}

const wxString& CompileTargetBase::GetExecutionParameters() const
{
    return m_ExecutionParameters;
}

void CompileTargetBase::SetExecutionParameters(const wxString& params)
{
    if (m_ExecutionParameters == params)
        return;

    m_ExecutionParameters = params;
    SetModified(true);
}

const wxString& CompileTargetBase::GetHostApplication() const
{
    return m_HostApplication;
}

void CompileTargetBase::SetHostApplication(const wxString& app)
{
    if (m_HostApplication == app)
        return;

    m_HostApplication = app;
    SetModified(true);
}

bool CompileTargetBase::GetRunHostApplicationInTerminal() const
{
    return m_RunHostApplicationInTerminal;
}

void CompileTargetBase::SetRunHostApplicationInTerminal(bool in_terminal)
{
    if (m_RunHostApplicationInTerminal == in_terminal)
        return;
    m_RunHostApplicationInTerminal = in_terminal;
    SetModified(true);
}

void CompileTargetBase::SetCompilerID(const wxString& id)
{
    if (id == m_CompilerId)
        return;

    m_CompilerId = id;
    SetModified(true);
}

void CompileTargetBase::SetMakeCommandFor(MakeCommand cmd, const wxString& make)
{
    if (m_MakeCommands[cmd] == make)
        return;

    m_MakeCommands[cmd] = make;
    m_MakeCommandsModified = true;
    SetModified(true);
}
