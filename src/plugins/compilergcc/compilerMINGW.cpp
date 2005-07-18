/* compilerMINGW.cpp
 * $Id$
 */

#include "compilerMINGW.h"
#include <wx/intl.h>
#include <wx/regex.h>
#include <wx/config.h>
#include <wx/fileconf.h>
#include <wx/msgdlg.h>
#include <wx/log.h>

#include <configmanager.h>

#ifdef __WXMSW__
    #include <wx/msw/registry.h>
#endif

CompilerMINGW::CompilerMINGW()
    : Compiler(_("GNU GCC Compiler"))
{
    Reset();
}

CompilerMINGW::~CompilerMINGW()
{
	//dtor
}

Compiler * CompilerMINGW::CreateCopy()
{
    return new CompilerMINGW(*this);
}

void CompilerMINGW::Reset()
{
#ifdef __WXMSW__
	m_Programs.C = "mingw32-gcc.exe";
	m_Programs.CPP = "mingw32-g++.exe";
	m_Programs.LD = "mingw32-g++.exe";
	m_Programs.DBG = "gdb.exe";
	m_Programs.LIB = "ar.exe";
	m_Programs.WINDRES = "windres.exe";
	m_Programs.MAKE = "mingw32-make.exe";
#else
	m_Programs.C = "gcc";
	m_Programs.CPP = "g++";
	m_Programs.LD = "g++";
	m_Programs.DBG = "gdb";
	m_Programs.LIB = "ar";
	m_Programs.WINDRES = "";
	m_Programs.MAKE = "make";
#endif
	m_Switches.includeDirs = "-I";
	m_Switches.libDirs = "-L";
	m_Switches.linkLibs = "-l";
	m_Switches.defines = "-D";
	m_Switches.genericSwitch = "-";
	m_Switches.objectExtension = "o";
	m_Switches.needDependencies = true;
	m_Switches.forceCompilerUseQuotes = false;
	m_Switches.forceLinkerUseQuotes = false;
	m_Switches.logging = clogSimple;
	m_Switches.libPrefix = "lib";
	m_Switches.libExtension = "a";
	m_Switches.linkerNeedsLibPrefix = false;
	m_Switches.linkerNeedsLibExtension = false;
	m_Switches.buildMethod = cbmDirect;

    // Summary of GCC options: http://gcc.gnu.org/onlinedocs/gcc/Option-Summary.html

    m_Options.ClearOptions();
	m_Options.AddOption(_("Produce debugging symbols"),
				"-g",
				_("Debugging"), 
				"",
				true, 
				"-O -O1 -O2 -O3 -Os", 
				_("You have optimizations enabled. This is Not A Good Thing(tm) when producing debugging symbols..."));
	m_Options.AddOption(_("Profile code when executed"), "-pg", _("Profiling"), "-pg -lgmon");

    wxString category = _("Warnings");

    // warnings
	m_Options.AddOption(_("Enable all compiler warnings (overrides every other setting)"), "-Wall", category);
	m_Options.AddOption(_("Enable standard compiler warnings"), "-W", category);
	m_Options.AddOption(_("Stop compiling after first error"), "-Wfatal-errors", category);
	m_Options.AddOption(_("Inhibit all warning messages"), "-w", category);
	m_Options.AddOption(_("Enable warnings demanded by strict ISO C and ISO C++"), "-pedantic", category);
	m_Options.AddOption(_("Treat as errors the warnings demanded by strict ISO C and ISO C++"), "-pedantic-error", category);
	m_Options.AddOption(_("Warn if main() is not conformant"), "-Wmain", category);
    // optimization
    category = _("Optimization");
	m_Options.AddOption(_("Strip all symbols from binary (minimizes size)"), "", category, "-s", true, "-g -ggdb", "Stripping the binary will strip debugging symbols as well!");
	m_Options.AddOption(_("Optimize generated code (for speed)"), "-O", category);
	m_Options.AddOption(_("Optimize more (for speed)"), "-O1", category);
	m_Options.AddOption(_("Optimize even more (for speed)"), "-O2", category);
	m_Options.AddOption(_("Optimize fully (for speed)"), "-O3", category);
	m_Options.AddOption(_("Optimize generated code (for size)"), "-Os", category);
	m_Options.AddOption(_("Expensive optimizations"), "-fexpensive-optimizations", category);
    // machine dependent options - cpu arch
    category = _("CPU architecture tuning (choose none, or only one of these)");
	m_Options.AddOption(_("i386"), "-march=i386", category);
	m_Options.AddOption(_("i486"), "-march=i486", category);
	m_Options.AddOption(_("Intel Pentium"), "-march=i586", category);
	m_Options.AddOption(_("Intel Pentium (MMX)"), "-march=pentium-mmx", category);
	m_Options.AddOption(_("Intel Pentium PRO"), "-march=i686", category);
	m_Options.AddOption(_("Intel Pentium 2 (MMX)"), "-march=pentium2", category);
	m_Options.AddOption(_("Intel Pentium 3 (MMX, SSE)"), "-march=pentium3", category);
	m_Options.AddOption(_("Intel Pentium 4 (MMX, SSE, SSE2)"), "-march=pentium4", category);
	m_Options.AddOption(_("Intel Pentium 4 Prescott (MMX, SSE, SSE2, SSE3)"), "-march=prescott", category);
	m_Options.AddOption(_("Intel Pentium 4 Nocona (MMX, SSE, SSE2, SSE3, 64bit extensions)"), "-march=nocona", category);
	m_Options.AddOption(_("Intel Pentium M (MMX, SSE, SSE2)"), "-march=pentium-m", category);
	m_Options.AddOption(_("AMD K6 (MMX)"), "-march=k6", category);
	m_Options.AddOption(_("AMD K6-2 (MMX, 3DNow!)"), "-march=k6-2", category);
	m_Options.AddOption(_("AMD K6-3 (MMX, 3DNow!)"), "-march=k6-3", category);
	m_Options.AddOption(_("AMD Athlon (MMX, 3DNow!, enhanced 3DNow!, SSE prefetch)"), "-march=athlon", category);
	m_Options.AddOption(_("AMD Athlon Thunderbird (MMX, 3DNow!, enhanced 3DNow!, SSE prefetch)"), "-march=athlon-tbird", category);
	m_Options.AddOption(_("AMD Athlon 4 (MMX, 3DNow!, enhanced 3DNow!, full SSE)"), "-march=athlon-4", category);
	m_Options.AddOption(_("AMD Athlon XP (MMX, 3DNow!, enhanced 3DNow!, full SSE)"), "-march=athlon-xp", category);
	m_Options.AddOption(_("AMD Athlon MP (MMX, 3DNow!, enhanced 3DNow!, full SSE)"), "-march=athlon-mp", category);
	m_Options.AddOption(_("AMD K8 core (x86-64 instruction set)"), "-march=k8", category);
	m_Options.AddOption(_("AMD Opteron (x86-64 instruction set)"), "-march=opteron", category);
	m_Options.AddOption(_("AMD Athlon64 (x86-64 instruction set)"), "-march=athlon64", category);
	m_Options.AddOption(_("AMD Athlon-FX (x86-64 instruction set)"), "-march=athlon-fx", category);

    m_Commands[(int)ctCompileObjectCmd] = "$compiler $options $includes -c $file -o $object";
    m_Commands[(int)ctGenDependenciesCmd] = "$compiler -MM $options -MF $dep_object -MT $object $includes $file";
    m_Commands[(int)ctCompileResourceCmd] = "$rescomp -i $file -J rc -o $resource_output -O coff $res_includes";
    m_Commands[(int)ctLinkExeCmd] = "$linker $libdirs -o $exe_output $link_objects $link_resobjects $link_options $libs -mwindows";
    m_Commands[(int)ctLinkConsoleExeCmd] = "$linker $libdirs -o $exe_output $link_objects $link_resobjects $link_options $libs";
#ifdef __WXMSW__
    m_Commands[(int)ctLinkDynamicCmd] = "$linker -shared -Wl,--output-def=$def_output -Wl,--out-implib=$static_output -Wl,--dll $libdirs $link_objects $link_resobjects -o $exe_output $link_options $libs";
#else
    m_Commands[(int)ctLinkDynamicCmd] = "$linker -shared -Wl,--output-def=$def_output -Wl,--out-implib=$static_output $libdirs $link_objects $link_resobjects -o $exe_output $link_options $libs";
#endif
    m_Commands[(int)ctLinkStaticCmd] = "$lib_linker -r $static_output $link_objects\n\tranlib $exe_output";

    LoadDefaultRegExArray();

    m_CompilerOptions.Clear();
    m_LinkerOptions.Clear();
    m_LinkLibs.Clear();
    m_CmdsBefore.Clear();
    m_CmdsAfter.Clear();
}

void CompilerMINGW::LoadDefaultRegExArray()
{
    m_RegExes.Clear();
    m_RegExes.Add(RegExStruct(_("Fatal error"), cltError, "FATAL:[ \t]*(.*)", 1));
    m_RegExes.Add(RegExStruct(_("Preprocessor warning"), cltWarning, "([ \tA-Za-z0-9_:+/\\.-]+):([0-9]+):([0-9]+):[ \t]([Ww]arning:[ \t].*)", 4, 1, 2));
    m_RegExes.Add(RegExStruct(_("Preprocessor error"), cltError, "([ \tA-Za-z0-9_:+/\\.-]+):([0-9]+):[0-9]+:[ \t](.*)", 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compiler warning"), cltWarning, "([ \tA-Za-z0-9_:+/\\.-]+):([0-9]+):[ \t]([Ww]arning:[ \t].*)", 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compiler error"), cltError, "([ \tA-Za-z0-9_:+/\\.-]+):([0-9]+):[ \t](.*)", 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Linker error"), cltError, "([ \tA-Za-z0-9_:+/\\.-]+):([0-9]+):[0-9]+:[ \t](.*)", 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Linker error (2)"), cltError, "[ \tA-Za-z0-9_:+/\\.-]+\(.text\+[0-9A-Za-z]+\):([ \tA-Za-z0-9_:+/\\.-]+):[ \t](.*)", 2, 1));
    m_RegExes.Add(RegExStruct(_("Linker error (lib not found)"), cltError, ".*(ld.exe):[ \t](cannot find.*)", 2, 1));
    m_RegExes.Add(RegExStruct(_("Undefined reference"), cltError, "([ \tA-Za-z0-9_:+/\\.-]+):[ \t](undefined reference.*)", 2, 1));
    m_RegExes.Add(RegExStruct(_("Resource compiler error"), cltError, "windres.exe:[ \t](.*)", 1));
}

AutoDetectResult CompilerMINGW::AutoDetectInstallationDir()
{
    wxString sep = wxFileName::GetPathSeparator();
#ifdef __WXMSW__
    // look first if MinGW was installed with Code::Blocks (new in beta6)
    m_MasterPath = ConfigManager::Get()->Read("app_path", wxEmptyString);
	if (!wxFileExists(m_MasterPath + sep + "bin" + sep + m_Programs.C))
    {
        // no... search for MinGW installation dir
        wxString windir = wxGetOSDirectory();
        wxFileConfig ini("", "", windir + "/MinGW.ini", "", wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_NO_ESCAPE_CHARACTERS);
        m_MasterPath = ini.Read("/InstallSettings/InstallPath", "C:\\MinGW");
        if (!wxFileExists(m_MasterPath + sep + "bin" + sep + m_Programs.C))
        {
            // not found...
            // look for dev-cpp installation
            wxLogNull ln;
            wxRegKey key; // defaults to HKCR
            key.SetName("HKEY_LOCAL_MACHINE\\Software\\Dev-C++");
            if (key.Open())
                // found; read it
                key.QueryValue("Install_Dir", m_MasterPath);
        }
    }
    else
        m_Programs.MAKE = "make.exe"; // we distribute "make" not "mingw32-make"
#else
    m_MasterPath = "/usr";
#endif
    AutoDetectResult ret = wxFileExists(m_MasterPath + sep + "bin" + sep + m_Programs.C) ? adrDetected : adrGuessed;
    if (ret == adrDetected)
    {
        AddIncludeDir(m_MasterPath + sep + "include");
        AddLibDir(m_MasterPath + sep + "lib");
    }
    return ret;
}
