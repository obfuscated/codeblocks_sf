#include "compilerMINGW.h"
#include <wx/intl.h>
#include <wx/regex.h>
#include <wx/config.h>
#include <wx/fileconf.h>
#include <wx/msgdlg.h>
#include <wx/log.h>

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
	m_Programs.LIB = "ar.exe";
	m_Programs.WINDRES = "windres.exe";
	m_Programs.MAKE = "mingw32-make.exe";
#else
	m_Programs.C = "gcc";
	m_Programs.CPP = "g++";
	m_Programs.LD = "g++";
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
#ifdef __WXMSW__
	m_Switches.buildMethod = cbmDirect;
#else
	m_Switches.buildMethod = cbmUseMake;
#endif

    m_Options.ClearOptions();
	m_Options.AddOption(_("Produce debugging symbols"),
				"-g",
				_("Debugging"), 
				"",
				true, 
				"-O -O1 -O2 -O3 -Os", 
				_("You have optimizations enabled. This is Not A Good Thing(tm) when producing debugging symbols..."));
	m_Options.AddOption(_("Profile code when executed"), "-pg", _("Profiling"), "-pg -lgmon");
	m_Options.AddOption(_("Enable all compiler warnings"), "-Wall", _("Warnings"));
	m_Options.AddOption(_("Optimize generated code (for speed)"), "-O", _("Optimization"));
	m_Options.AddOption(_("Optimize more (for speed)"), "-O1", _("Optimization"));
	m_Options.AddOption(_("Optimize even more (for speed)"), "-O2", _("Optimization"));
	m_Options.AddOption(_("Optimize generated code (for size)"), "-Os", _("Optimization"));
	m_Options.AddOption(_("Expensive optimizations"), "-fexpensive-optimizations", _("Optimization"));

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

    m_RegExes.Clear();
    m_RegExes.Add(RegExStruct(_("Fatal error"), cltError, "FATAL:[ \t]*(.*)", 1));
    m_RegExes.Add(RegExStruct(_("Preprocessor error"), cltError, "([ \tA-Za-z0-9_\\-\\+/\\.]+):([0-9]+):[0-9]+:[ \t](.*)", 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compiler warning"), cltWarning, "([ \tA-Za-z0-9_\\-\\+/\\.]+):([0-9]+):[ \t][Ww]arning:[ \t](.*)", 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compiler error"), cltError, "([ \tA-Za-z0-9_\\-\\+/\\.]+):([0-9]+):[ \t](.*)", 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Linker error"), cltError, "([ \tA-Za-z0-9_\\-\\+/\\.]+):([0-9]+):[0-9]+:[ \t](.*)", 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Undefined reference"), cltError, "([ \tA-Za-z0-9_\\-\\+/\\.]+):[ \t](undefined reference.*)", 2, 1));

    m_CompilerOptions.Clear();
    m_LinkerOptions.Clear();
    m_LinkLibs.Clear();
    m_CmdsBefore.Clear();
    m_CmdsAfter.Clear();
}

AutoDetectResult CompilerMINGW::AutoDetectInstallationDir()
{
    wxString sep = wxFileName::GetPathSeparator();
#ifdef __WXMSW__
    // search for MinGW installation dir
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
