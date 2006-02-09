#ifdef __WXMSW__
// this compiler is valid only in windows

#include <sdk.h>
#include "compilerBCC.h"
#include <wx/intl.h>
#include <wx/regex.h>

CompilerBCC::CompilerBCC()
    : Compiler(_("Borland C++ Compiler 5.5"), _T("bcc"))
{
    Reset();
}

CompilerBCC::~CompilerBCC()
{
	//dtor
}

Compiler * CompilerBCC::CreateCopy()
{
    return new CompilerBCC(*this);
}

void CompilerBCC::Reset()
{
	m_Programs.C = _T("bcc32.exe");
	m_Programs.CPP = _T("bcc32.exe");
	m_Programs.LD = _T("ilink32.exe");
	m_Programs.LIB = _T("tlib.exe");
	m_Programs.WINDRES = _T("brcc32.exe"); // platform SDK is needed for this
	m_Programs.MAKE = _T("mingw32-make.exe");

	m_Switches.includeDirs = _T("-I");
	m_Switches.libDirs = _T("-L");
	m_Switches.linkLibs = _T("");
	m_Switches.defines = _T("-D");
	m_Switches.genericSwitch = _T("-");
	m_Switches.objectExtension = _T("obj");
	m_Switches.needDependencies = false;
	m_Switches.forceCompilerUseQuotes = false;
	m_Switches.forceLinkerUseQuotes = true;
	m_Switches.logging = clogSimple;
	m_Switches.buildMethod = cbmDirect;
	m_Switches.libPrefix = _T("");
	m_Switches.libExtension = _T("lib");
	m_Switches.linkerNeedsLibPrefix = false;
	m_Switches.linkerNeedsLibExtension = true;

    m_Options.ClearOptions();
//	m_Options.AddOption(_("Enable all compiler warnings"), "/Wall", _("Warnings"));
	m_Options.AddOption(_("Optimizations level 1"), _T("-O1"), _("Optimization"));
	m_Options.AddOption(_("Optimizations level 2"), _T("-O2"), _("Optimization"));
	m_Options.AddOption(_("Optimizations level 3"), _T("-O3"), _("Optimization"));
	m_Options.AddOption(_("Disable C++ RTTI"), _T("-RT-"), _("C++ Features"));
//	m_Options.AddOption(_("Enable C++ exception handling"), _T("/GX"), _("C++ Features"));
	m_Options.AddOption(_("Optimize for 80386"), _T("-3"), _("Architecture"));
	m_Options.AddOption(_("Optimize for 80486"), _T("-4"), _("Architecture"));
	m_Options.AddOption(_("Optimize for Pentium"), _T("-5"), _("Architecture"));
	m_Options.AddOption(_("Optimize for Pentium Pro, Pentium II, Pentium III"), _T("-6"), _("Architecture"));

    m_Commands[(int)ctCompileObjectCmd] = _T("$compiler -q $options $includes -o$object -c $file");
    m_Commands[(int)ctCompileResourceCmd] = _T("$rescomp -32 -fo$resource_output $res_includes $file");
    m_Commands[(int)ctLinkExeCmd] = _T("$linker -q -aa  $link_options $libdirs c0w32 $link_objects,$exe_output,,$libs,,$link_resobjects");
    m_Commands[(int)ctLinkConsoleExeCmd] = _T("$linker -q -ap  $link_options $libdirs c0x32 $link_objects,$exe_output,,$libs,,$link_resobjects");
    m_Commands[(int)ctLinkDynamicCmd] = _T("$linker -q $libdirs -o $exe_output $libs $link_objects $link_options");
    m_Commands[(int)ctLinkStaticCmd] = _T("$lib_linker /C $static_output $+-link_objects,$def_output");

    LoadDefaultRegExArray();

    m_CompilerOptions.Clear();
    m_LinkerOptions.Clear();
    m_LinkLibs.Clear();
    m_CmdsBefore.Clear();
    m_CmdsAfter.Clear();
}

void CompilerBCC::LoadDefaultRegExArray()
{
    m_RegExes.Clear();
    m_RegExes.Add(RegExStruct(_("Compiler warning"), cltWarning, _T("(^Warning[ \t]W[0-9]+)[ \t]([A-Za-z0-9_:/\\.-]+)[ \t]([0-9]+)(:[ \t].*)"), 1, 2, 3, 4));
    m_RegExes.Add(RegExStruct(_("Compiler error"), cltError, _T("(^Error[ \t]E[0-9]+)[ \t]([A-Za-z0-9_:/\\.-]+)[ \t]([0-9]+)(:[ \t].*)"), 1, 2, 3, 4));
    m_RegExes.Add(RegExStruct(_("Unknown error"), cltError, _T("(^Error[ \t]+E[0-9]+:.*)"), 1));
    m_RegExes.Add(RegExStruct(_("Fatal error"), cltError, _T("Fatal:[ \t]+(.*)"), 1));
}

AutoDetectResult CompilerBCC::AutoDetectInstallationDir()
{
    // just a guess; the default installation dir
	m_MasterPath = _T("C:\\Borland\\BCC55");
    wxString sep = wxString(wxFileName::GetPathSeparator());
    if (!m_MasterPath.IsEmpty())
    {
        AddIncludeDir(m_MasterPath + sep + _T("include"));
        AddLibDir(m_MasterPath + sep + _T("lib"));
    }

    return wxFileExists(m_MasterPath + sep + _T("bin") + sep + m_Programs.C) ? adrDetected : adrGuessed;
}

#endif // __WXMSW__
