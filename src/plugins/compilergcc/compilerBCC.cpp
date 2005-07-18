#include "compilerBCC.h"
#include <wx/intl.h>
#include <wx/regex.h>

CompilerBCC::CompilerBCC()
    : Compiler(_("Borland C++ Compiler 5.5"))
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
	m_Programs.C = "bcc32.exe";
	m_Programs.CPP = "bcc32.exe";
	m_Programs.LD = "ilink32.exe";
	m_Programs.LIB = "tlib.exe";
	m_Programs.WINDRES = "brcc32.exe"; // platform SDK is needed for this
	m_Programs.MAKE = "mingw32-make.exe";
	
	m_Switches.includeDirs = "-I";
	m_Switches.libDirs = "-L";
	m_Switches.linkLibs = "";
	m_Switches.defines = "-D";
	m_Switches.genericSwitch = "-";
	m_Switches.objectExtension = "obj";
	m_Switches.needDependencies = false;
	m_Switches.forceCompilerUseQuotes = false;
	m_Switches.forceLinkerUseQuotes = true;
	m_Switches.logging = clogSimple;
	m_Switches.buildMethod = cbmDirect;
	m_Switches.libPrefix = "";
	m_Switches.libExtension = "lib";
	m_Switches.linkerNeedsLibPrefix = false;
	m_Switches.linkerNeedsLibExtension = true;

    m_Options.ClearOptions();
//	m_Options.AddOption(_("Enable all compiler warnings"), "/Wall", _("Warnings"));
	m_Options.AddOption(_("Optimizations level 1"), "-O1", _("Optimization"));
	m_Options.AddOption(_("Optimizations level 2"), "-O2", _("Optimization"));
	m_Options.AddOption(_("Optimizations level 3"), "-O3", _("Optimization"));
	m_Options.AddOption(_("Disable C++ RTTI"), "-RT-", _("C++ Features"));
//	m_Options.AddOption(_("Enable C++ exception handling"), "/GX", _("C++ Features"));
	m_Options.AddOption(_("Optimize for 80386"), "-3", _("Architecture"));
	m_Options.AddOption(_("Optimize for 80486"), "-4", _("Architecture"));
	m_Options.AddOption(_("Optimize for Pentium"), "-5", _("Architecture"));
	m_Options.AddOption(_("Optimize for Pentium Pro, Pentium II, Pentium III"), "-6", _("Architecture"));

    m_Commands[(int)ctCompileObjectCmd] = "$compiler $options $includes -o$object -c $file";
    m_Commands[(int)ctCompileResourceCmd] = "$rescomp -32 -fo$resource_output $res_includes $file";
    m_Commands[(int)ctLinkExeCmd] = "$linker -aa  $link_options $libdirs c0w32 $link_objects,$exe_output,,$libs,,$link_resobjects";
    m_Commands[(int)ctLinkConsoleExeCmd] = "$linker -ap  $link_options $libdirs c0x32 $link_objects,$exe_output,,$libs,,$link_resobjects";
    m_Commands[(int)ctLinkDynamicCmd] = "$linker $libdirs -o $exe_output $libs $link_objects $link_options";
    m_Commands[(int)ctLinkStaticCmd] = "$lib_linker /C $static_output +$link_objects,$def_output";

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
    m_RegExes.Add(RegExStruct(_("Compiler warning"), cltWarning, "(^Warning[ \t]W[0-9]+)[ \t]([A-Za-z0-9_:/\\.-]+)[ \t]([0-9]+)(:[ \t].*)", 1, 2, 3, 4));
    m_RegExes.Add(RegExStruct(_("Compiler error"), cltError, "(^Error[ \t]E[0-9]+)[ \t]([A-Za-z0-9_:/\\.-]+)[ \t]([0-9]+)(:[ \t].*)", 1, 2, 3, 4));
    m_RegExes.Add(RegExStruct(_("Unknown error"), cltError, "(^Error[ \t]+E[0-9]+:.*)", 1));
    m_RegExes.Add(RegExStruct(_("Fatal error"), cltError, "Fatal:[ \t]+(.*)", 1));
}

AutoDetectResult CompilerBCC::AutoDetectInstallationDir()
{
    // just a guess; the default installation dir
	m_MasterPath = "C:\\Borland\\BCC55";
    wxString sep = wxFileName::GetPathSeparator();
    if (!m_MasterPath.IsEmpty())
    {
        AddIncludeDir(m_MasterPath + sep + "include");
        AddLibDir(m_MasterPath + sep + "lib");
    }

    return wxFileExists(m_MasterPath + sep + "bin" + sep + m_Programs.C) ? adrDetected : adrGuessed;
}
