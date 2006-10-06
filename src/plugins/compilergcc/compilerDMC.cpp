#ifdef __WXMSW__
// this compiler is valid only in windows

#include <sdk.h>
#include "compilerDMC.h"
#include <wx/log.h>
#include <wx/intl.h>
#include <wx/regex.h>
#include <wx/config.h>

#ifdef __WXMSW__
    #include <wx/msw/registry.h>
#endif

CompilerDMC::CompilerDMC()
    : Compiler(_("Digital Mars Compiler"), _T("dmc"))
{
    Reset();
}

CompilerDMC::~CompilerDMC()
{
	//dtor
}

Compiler * CompilerDMC::CreateCopy()
{
    Compiler* c = new CompilerDMC(*this);
    c->SetExtraPaths(m_ExtraPaths); // wxArrayString doesn't seem to be copied with the default copy ctor...
    return c;
}

void CompilerDMC::Reset()
{
	m_Programs.C = _T("dmc.exe");
	m_Programs.CPP = _T("dmc.exe");
	m_Programs.LD = _T("link.exe");
	m_Programs.LIB = _T("lib.exe");
	m_Programs.WINDRES = _T("rcc.exe");
	m_Programs.MAKE = _T("mingw32-make.exe");

	m_Switches.includeDirs = _T("-I");
	m_Switches.libDirs = _T("");
	m_Switches.linkLibs = _T("");
	m_Switches.libPrefix = _T("");
	m_Switches.libExtension = _T("lib");
	m_Switches.defines = _T("-D");
	m_Switches.genericSwitch = _T("-");
	m_Switches.objectExtension = _T("obj");
	m_Switches.needDependencies = false;
	m_Switches.forceCompilerUseQuotes = false;
	m_Switches.forceLinkerUseQuotes = true;
	m_Switches.logging = clogSimple;
	m_Switches.buildMethod = cbmDirect;
	m_Switches.linkerNeedsLibPrefix = false;
	m_Switches.linkerNeedsLibExtension = true;

    m_Options.ClearOptions();
	m_Options.AddOption(_("Produce debugging symbols"),
				_T("-g"),
				_("Debugging"),
				_T(""),
				true,
				_T("-o -o+space"),
				_("You have optimizations enabled. This is Not A Good Thing(tm) when producing debugging symbols..."));

    //
    // TODO (hd#3#): should be simplified
    //

	//. m_Options.AddOption(_("Alignment of struct members"), "-a[1|2|4|8]", _("Architecture"));
	m_Options.AddOption(_("Enforce strict compliance with ANSI C/C++"), _T("-A"), _("C++ Features"));
	m_Options.AddOption(_("Enable new[] and delete[] overloading"), _T("-Aa"), _("C++ Features"));
	m_Options.AddOption(_("Enable bool"), _T("-Ab"), _("C++ Features"));
	m_Options.AddOption(_("Enable exception handling"), _T("-Ae"), _("C++ Features"));
	m_Options.AddOption(_("Enable RTTI"), _T("-Ar"), _("C++ Features"));
	//. m_Options.AddOption(_("Message language: English,French,German,Japanese"), _T("-B[e|f|g|j]"), _("Others"));
	//. m_Options.AddOption(_("Skip the link, do compile only"), _T("-c"), _("Others"));
	m_Options.AddOption(_("Compile all source files as C++"), _T("-cpp"), _("C++ Features"));
	m_Options.AddOption(_("Generate .cod (assemply) file"), _T("-cod"), _("Others"));
	m_Options.AddOption(_("No inline function expansion"), _T("-C"), _("Debugging"));
	m_Options.AddOption(_("Generate .dep (make dependency) file"), _T("-d"), _("Others"));
	m_Options.AddOption(_T("#define DEBUG 1"), _T("-D"), _("Debugging"));
	m_Options.AddOption(_("Show results of preprocessor"), _T("-e"), _("Others"));
	m_Options.AddOption(_("Do not elide comments"), _T("-EC"), _("Others"));
	m_Options.AddOption(_("#line directives not output"), _T("-EL"), _("Others"));
	m_Options.AddOption(_("IEEE 754 inline 8087 code"), _T("-f"), _("Others"));
	m_Options.AddOption(_("Work around FDIV problem"), _T("-fd"), _("Others"));
	m_Options.AddOption(_("Fast inline 8087 code"), _T("-ff"), _("Optimization"));
	m_Options.AddOption(_("Disable debug info optimization"), _T("-gf"), _("Optimization"));
	m_Options.AddOption(_("Make static functions global"), _T("-gg"), _("Optimization"));
	m_Options.AddOption(_("Symbol info for globals"), _T("-gh"), _("C++ Features"));
	m_Options.AddOption(_("Debug line numbers only"), _T("-gl"), _("Debugging"));
	m_Options.AddOption(_("Generate pointer validations"), _T("-gp"), _("Others"));
	m_Options.AddOption(_("Debug symbol info only"), _T("-gs"), _("Debugging"));
	m_Options.AddOption(_("Generate trace prolog/epilog"), _T("-gt"), _("Others"));
	//. m_Options.AddOption(_("Set data threshold to nnnn"), _T("-GTnnnn"), _("Others"));
	m_Options.AddOption(_("Use precompiled headers (ph)"), _T("-H"), _("Others"));
	//. m_Options.AddOption(_("Use ph from directory"), _T("-HDdirectory"), _("Others"));
	//. m_Options.AddOption(_("Generate ph to filename"), _T("-HF[filename]"), _("Others"));
	//. m_Options.AddOption(_("#include \"filename\""), _T("-HIfilename"), _("Others"));
	m_Options.AddOption(_("Include files only once"), _T("-HO"), _("Others"));
	m_Options.AddOption(_("Only search -I directories"), _T("-HS"), _("Others"));
	//. m_Options.AddOption(_("#include file search path"), _T("-Ipath"), _("Others"));
	//. m_Options.AddOption(_("Asian language characters (Japanese)"), _T("-j0"), _("Others"));
	//. m_Options.AddOption(_("Asian language characters (Taiwanese/Chinese)"), _T("-j1"), _("Others"));
	//. m_Options.AddOption(_("Asian language characters (Korean)"), _T("-j2"), _("Others"));
	m_Options.AddOption(_("Relaxed type checking"), _T("-Jm"), _("Others"));
	m_Options.AddOption(_T("char==unsigned"), _T("-Ju"), _("Others"));
	m_Options.AddOption(_("No empty base class optimization"), _T("-Jb"), _("Optimization"));
	m_Options.AddOption(_("chars are unsigned"), _T("-J"), _("Others"));
	//. m_Options.AddOption(_("Generate list file"), _T("-l[listfile]"), _("Others"));
	//. m_Options.AddOption(_("Using non-Digital Mars linker"), _T("-L"), _("Others"));
	//. m_Options.AddOption(_("Specify linker to use"), _T("-Llink"), _("Others"));
	//. m_Options.AddOption(_("Pass /switch to linker"), _T("-L/switch"), _("Others"));
	//. m_Options.AddOption(_("Specify assembler to use"), _T("-Masm"), _("Others"));
	//. m_Options.AddOption(_("Pass /switch to assembler"), _T("-M/switch"), _("Others"));
	//. m_Options.AddOption(_("Set memory model (-mn: Windows)"), _T("-m[tsmclvfnrpxz][do][w][u]"), _("Architecture"));
	m_Options.AddOption(_("Perform function level linking"), _T("-Nc"), _("Optimization"));
	m_Options.AddOption(_("No default library"), _T("-NL"), _("Optimization"));
	m_Options.AddOption(_("Place expr strings in code seg"), _T("-Ns"), _("Optimization"));
	m_Options.AddOption(_("New code seg for each function"), _T("-NS"), _("Optimization"));
	//. m_Options.AddOption(_("Set code segment name"), _T("-NTname"), _("Others"));
	m_Options.AddOption(_("vtables in far data"), _T("-NV"), _("Others"));
	//. m_Options.AddOption(_("Run optimizer with flag"), _T("-o[-+flag]"), _("Optimization"));
	m_Options.AddOption(_("Minimize space"), _T("-o+space"), _("Optimization"));
	m_Options.AddOption(_("Maximize speed"), _T("-o"), _("Optimization"));
	//. m_Options.AddOption(_("Output filename"), _T("-ooutput"), _("Others"));
	m_Options.AddOption(_("Turn off function auto-prototyping"), _T("-p"), _("Others"));
	m_Options.AddOption(_("Make Pascal linkage the default"), _T("-P"), _("Linkage"));
	m_Options.AddOption(_("Make stdcall linkage the default"), _T("-Pz"), _("Linkage"));
	m_Options.AddOption(_("Require strict function prototyping"), _T("-r"), _("Others"));
	m_Options.AddOption(_("Put switch tables in code seg"), _T("-R"), _("Others"));
	m_Options.AddOption(_("Stack overflow checking"), _T("-s"), _("Others"));
	m_Options.AddOption(_("Always generate stack frame"), _T("-S"), _("Others"));
	m_Options.AddOption(_("Suppress non-ANSI predefined macros"), _T("-u"), _("C++ Features"));
	//. m_Options.AddOption(_("Verbose compile"), _T("-v[0|1|2]"), _("Warnings"));
	m_Options.AddOption(_("Suppress all warning messages"), _T("-w"), _("Warnings"));
	m_Options.AddOption(_("Warn on C style casts"), _T("-wc"), _("Warnings"));
	//. m_Options.AddOption(_("Suppress warning number n"), _T("-wn"), _("Warnings"));
	m_Options.AddOption(_("Treat warnings as errors"), _T("-wx"), _("Warnings"));
	//. m_Options.AddOption(_("Windows prolog/epilog (-WA exe -WD dll)"), _T("-W{0123ADabdefmrstuvwx-+}"), _("Architecture"));
	m_Options.AddOption(_("Windows prolog/epilog : Win32 Exe"), _T("-WA"), _("Architecture"));
	m_Options.AddOption(_("Windows prolog/epilog : Win32 Dll"), _T("-WD"), _("Architecture"));
	m_Options.AddOption(_("Turn off error maximum"), _T("-x"), _("Warnings"));
	m_Options.AddOption(_("Instantiate templates"), _T("-XD"), _("C++ Features"));
	//. m_Options.AddOption(_("Instantiate template class temp<type>"), _T("-XItemp<type>"), _("C++ Features"));
	//. m_Options.AddOption(_("Instantiate template function func(type)"), _T("-XIfunc(type)"), _("C++ Features"));
	//. m_Options.AddOption(_("8088/286/386/486/Pentium/P6 code"), _T("-[0|2|3|4|5|6]"), _("Architecture"));
	m_Options.AddOption(_("Optimize for 80386"), _T("-3"), _("Architecture"));
	m_Options.AddOption(_("Optimize for 80486"), _T("-4"), _("Architecture"));
	m_Options.AddOption(_("Optimize for Pentium"), _T("-5"), _("Architecture"));
	m_Options.AddOption(_("Optimize for Pentium Pro, Pentium II, Pentium III"), _T("-6"), _("Architecture"));

    // FIXME (hd#1#): should be work on: we need $res_options
    m_Commands[(int)ctCompileObjectCmd] = _T("$compiler -mn -c $options $includes -o$object $file");
    m_Commands[(int)ctCompileResourceCmd] = _T("$rescomp -32 -I$res_includes -o$resource_output $file");
    m_Commands[(int)ctLinkExeCmd] = _T("$linker /NOLOGO /subsystem:windows $link_objects, $exe_output, , $libs $link_options, , $link_resobjects");
    m_Commands[(int)ctLinkConsoleExeCmd] = _T("$linker /NOLOGO $link_objects, $exe_output, , $libs $link_options");
    m_Commands[(int)ctLinkDynamicCmd] = _T("$linker /NOLOGO /subsystem:windows $link_objects, $exe_output, , $libs $link_options, , $link_resobjects");
    m_Commands[(int)ctLinkStaticCmd] = _T("$lib_linker -c $link_options $static_output $link_objects");

    LoadDefaultRegExArray();

    m_CompilerOptions.Clear();
    m_LinkerOptions.Clear();
    m_LinkLibs.Clear();
    m_CmdsBefore.Clear();
    m_CmdsAfter.Clear();
}

void CompilerDMC::LoadDefaultRegExArray()
{
    m_RegExes.Clear();
    m_RegExes.Add(RegExStruct(_("Linker error"), cltError, _T("([ \tA-Za-z0-9_:\\-\\+/\\.\\(\\)-]*)[ \t]+:[ \t]+(.*error LNK[0-9]+.*)"), 2, 1));
    m_RegExes.Add(RegExStruct(_("Compiler warning"), cltWarning, _T("([ \tA-Za-z0-9_:\\-\\+/\\.-]+)\\(([0-9]+)\\)[ \t]*:[ \t]*[Ww][Aa][Rr][Nn][Ii][Nn][Gg][ \t]*(.*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compiler error"), cltError, _T("([ \tA-Za-z0-9_:\\-\\+/\\.-]+)\\(([0-9]+)\\)[ \t]*:[ \t]*(.*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Fatal error"), cltError, _T("Fatal error:[ \t](.*)"), 1));
}

AutoDetectResult CompilerDMC::AutoDetectInstallationDir()
{
    // just a guess; the default installation dir
	m_MasterPath = _T("C:\\dm");
    wxString sep = wxFileName::GetPathSeparator();

    // NOTE (hd#1#): dmc uses sc.ini for compiler's master directories
    // NOTE (mandrav#1#): which doesn't seem to exist if you don't have the CD version ;)
    if (!m_MasterPath.IsEmpty())
    {
        AddIncludeDir(m_MasterPath + sep + _T("stlport") + sep + _T("stlport"));
        AddIncludeDir(m_MasterPath + sep + _T("include"));
        AddLibDir(m_MasterPath + sep + _T("lib"));
    }

    return wxFileExists(m_MasterPath + sep + _T("bin") + sep + m_Programs.C) ? adrDetected : adrGuessed;
}

#endif // __WXMSW__
