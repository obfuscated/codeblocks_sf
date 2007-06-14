/* compilerMINGW.cpp
 * $Id: compilerMINGW.cpp 1429 2005-12-02 23:25:50Z mandrav $
 */

#include <sdk.h>
#include <prep.h>
#ifndef CB_PRECOMP
    #include <wx/intl.h>
    #include <wx/regex.h>
    #include <wx/utils.h> // wxGetOSDirectory, wxGetEnv
#endif
#include <wx/filefn.h> // wxFileExists
#include <wx/fileconf.h> // wxFileConfig
#include "compilerGNUARM.h"

CompilerGNUARM::CompilerGNUARM()
    : Compiler(_("GNU ARM GCC Compiler"),_T("arm-elf-gcc"))
{
    Reset();
}

CompilerGNUARM::~CompilerGNUARM()
{
    //dtor
}

Compiler * CompilerGNUARM::CreateCopy()
{
    Compiler* c = new CompilerGNUARM(*this);
    c->SetExtraPaths(m_ExtraPaths); // wxArrayString doesn't seem to be copied with the default copy ctor...
    return c;
}

void CompilerGNUARM::Reset()
{
    if (platform::windows)
    {
        m_Programs.C = _T("arm-elf-gcc.exe");
        m_Programs.CPP = _T("arm-elf-g++.exe");
        m_Programs.LD = _T("arm-elf-g++.exe");
        m_Programs.DBG = _T("arm-elf-gdb.exe");
        m_Programs.LIB = _T("arm-elf-ar.exe");
        m_Programs.WINDRES = _T("");
        m_Programs.MAKE = _T("make.exe");
    }
    else
    {
        m_Programs.C = _T("arm-elf-gcc");
        m_Programs.CPP = _T("arm-elf-g++");
        m_Programs.LD = _T("arm-elf-g++");
        m_Programs.DBG = _T("arm-elf-gdb");
        m_Programs.LIB = _T("arm-elf-ar");
        m_Programs.WINDRES = _T("");
        m_Programs.MAKE = _T("make");
    }
    m_Switches.includeDirs = _T("-I");
    m_Switches.libDirs = _T("-L");
    m_Switches.linkLibs = _T("-l");
    m_Switches.defines = _T("-D");
    m_Switches.genericSwitch = _T("-");
    m_Switches.objectExtension = _T("o");
    m_Switches.needDependencies = true;
    m_Switches.forceCompilerUseQuotes = false;
    m_Switches.forceLinkerUseQuotes = false;
    m_Switches.logging = clogSimple;
    m_Switches.libPrefix = _T("lib");
    m_Switches.libExtension = _T("a");
    m_Switches.linkerNeedsLibPrefix = false;
    m_Switches.linkerNeedsLibExtension = false;
    m_Switches.buildMethod = cbmDirect;

    // Summary of GCC options: http://gcc.gnu.org/onlinedocs/gcc/Option-Summary.html

    m_Options.ClearOptions();
    m_Options.AddOption(_("Produce debugging symbols"),
                _T("-g"),
                _("Debugging"),
                _T(""),
                true,
                _T("-O -O1 -O2 -O3 -Os"),
                _("You have optimizations enabled. This is Not A Good Thing(tm) when producing debugging symbols..."));

    wxString gprof_link = _T("-pg");
    if (platform::windows)
        gprof_link = _T("-pg -lgmon");
    m_Options.AddOption(_("Profile code when executed"), _T("-pg"), _("Profiling"), gprof_link);

    wxString category = _("Warnings");

    // warnings
    m_Options.AddOption(_("In C mode, support all ISO C90 programs. In C++ mode, remove GNU extensions that conflict with ISO C++"), _T("-ansi"), category);
    m_Options.AddOption(_("Enable all compiler warnings (overrides every other setting)"), _T("-Wall"), category);
    m_Options.AddOption(_("Enable standard compiler warnings"), _T("-W"), category);
    m_Options.AddOption(_("Stop compiling after first error"), _T("-Wfatal-errors"), category);
    m_Options.AddOption(_("Inhibit all warning messages"), _T("-w"), category);
    m_Options.AddOption(_("Enable warnings demanded by strict ISO C and ISO C++"), _T("-pedantic"), category);
    m_Options.AddOption(_("Treat as errors the warnings demanded by strict ISO C and ISO C++"), _T("-pedantic-errors"), category);
    m_Options.AddOption(_("Warn if main() is not conformant"), _T("-Wmain"), category);
    // optimization
    category = _("Optimization");
    m_Options.AddOption(_("Strip all symbols from binary (minimizes size)"), _T(""), category, _T("-s"), true, _T("-g -ggdb"), _("Stripping the binary will strip debugging symbols as well!"));
    m_Options.AddOption(_("Optimize generated code (for speed)"), _T("-O"), category);
    m_Options.AddOption(_("Optimize more (for speed)"), _T("-O1"), category);
    m_Options.AddOption(_("Optimize even more (for speed)"), _T("-O2"), category);
    m_Options.AddOption(_("Optimize fully (for speed)"), _T("-O3"), category);
    m_Options.AddOption(_("Optimize generated code (for size)"), _T("-Os"), category);
    m_Options.AddOption(_("Expensive optimizations"), _T("-fexpensive-optimizations"), category);
    // machine dependent options - cpu arch
    category = _("ARM CPU architecture specific");
    m_Options.AddOption(_("-mapcs-frame"), _T("-mapcs-frame"), category);
    m_Options.AddOption(_("-mno-apcs-frame"), _T("-mno-apcs-frame"), category);
    m_Options.AddOption(_("-mabi=NAME"), _T("-mabi=NAME"), category);
    m_Options.AddOption(_("-mapcs-stack-check"), _T("-mapcs-stack-check"), category);
    m_Options.AddOption(_("-mno-apcs-stack-check"), _T("-mno-apcs-stack-check"), category);
    m_Options.AddOption(_("-mapcs-float"), _T("-mapcs-float"), category);
    m_Options.AddOption(_("-mno-apcs-float"), _T("-mno-apcs-float"), category);
    m_Options.AddOption(_("-mapcs-reentrant"), _T("-mapcs-reentrant"), category);
    m_Options.AddOption(_("-mno-apcs-reentrant"), _T("-mno-apcs-reentrant"), category);
    m_Options.AddOption(_("-msched-prolog"), _T("-msched-prolog"), category);
    m_Options.AddOption(_("-mno-sched-prolog"), _T("-mno-sched-prolog"), category);
    m_Options.AddOption(_("-mlittle-endian"), _T("-mlittle-endian"), category);
    m_Options.AddOption(_("-mbig-endian"), _T("-mbig-endian"), category);
    m_Options.AddOption(_("-mwords-little-endian"), _T("-mwords-little-endian"), category);
    m_Options.AddOption(_("-mfloat-abi=NAME"), _T("-mfloat-abi=NAME"), category);
    m_Options.AddOption(_("-msoft-float"), _T("-msoft-float"), category);
    m_Options.AddOption(_("-mhard-float"), _T("-mhard-float"), category);
    m_Options.AddOption(_("-mfpe"), _T("-mfpe"), category);
    m_Options.AddOption(_("-mthumb-interwork"), _T("-mthumb-interwork"), category);
    m_Options.AddOption(_("-mno-thumb-interwork"), _T("-mno-thumb-interwork"), category);
    m_Options.AddOption(_("-mcpu=NAME"), _T("-mcpu=NAME"), category);
    m_Options.AddOption(_("-march=NAME"), _T("-march=NAME"), category);
    m_Options.AddOption(_("-mfpu=NAME"), _T("-mfpu=NAME"), category);
    m_Options.AddOption(_("-mstructure-size-boundary=N"), _T("-mstructure-size-boundary=N"), category);
    m_Options.AddOption(_("-mabort-on-noreturn"), _T("-mabort-on-noreturn"), category);
    m_Options.AddOption(_("-mlong-calls"), _T("-mlong-calls"), category);
    m_Options.AddOption(_("-mno-long-calls"), _T("-mno-long-calls"), category);
    m_Options.AddOption(_("-msingle-pic-base"), _T("-msingle-pic-base"), category);
    m_Options.AddOption(_("-mno-single-pic-base"), _T("-mno-single-pic-base"), category);
    m_Options.AddOption(_("-mpic-register=REG"), _T("-mpic-register=REG"), category);
    m_Options.AddOption(_("-mnop-fun-dllimport"), _T("-mnop-fun-dllimport"), category);
    m_Options.AddOption(_("-mcirrus-fix-invalid-insns"), _T("-mcirrus-fix-invalid-insns"), category);
    m_Options.AddOption(_("-mno-cirrus-fix-invalid-insns"), _T("-mno-cirrus-fix-invalid-insns"), category);
    m_Options.AddOption(_("-mpoke-function-name"), _T("-mpoke-function-name"), category);
    m_Options.AddOption(_("-mthumb"), _T("-mthumb"), category);
    m_Options.AddOption(_("-marm"), _T("-marm"), category);
    m_Options.AddOption(_("-mtpcs-frame"), _T("-mtpcs-frame"), category);
    m_Options.AddOption(_("-mtpcs-leaf-frame"), _T("-mtpcs-leaf-frame"), category);
    m_Options.AddOption(_("-mcaller-super-interworking"), _T("-mcaller-super-interworking"), category);
    m_Options.AddOption(_("-mcallee-super-interworking"), _T("-mcallee-super-interworking"), category);

    m_Commands[(int)ctCompileObjectCmd] = _T("$compiler $options $includes -c $file -o $object");
    m_Commands[(int)ctGenDependenciesCmd] = _T("$compiler -MM $options -MF $dep_object -MT $object $includes $file");
    m_Commands[(int)ctCompileResourceCmd] = _T("$rescomp -i $file -J rc -o $resource_output -O coff $res_includes");
    m_Commands[(int)ctLinkConsoleExeCmd] = _T("$linker $libdirs -o $exe_output $link_objects $link_resobjects $link_options $libs");
    if (platform::windows)
    {
        m_Commands[(int)ctLinkExeCmd] = _T("$linker $libdirs -o $exe_output $link_objects $link_resobjects $link_options $libs -mwindows");
        m_Commands[(int)ctLinkDynamicCmd] = _T("$linker -shared -Wl,--output-def=$def_output -Wl,--out-implib=$static_output -Wl,--dll $libdirs $link_objects $link_resobjects -o $exe_output $link_options $libs");
    }
    else
    {
        m_Commands[(int)ctLinkExeCmd] = m_Commands[(int)ctLinkConsoleExeCmd]; // no -mwindows
        m_Commands[(int)ctLinkDynamicCmd] = _T("$linker -shared $libdirs $link_objects $link_resobjects -o $exe_output $link_options $libs");
    }
    m_Commands[(int)ctLinkStaticCmd] = _T("$lib_linker -r -s $static_output $link_objects");
    m_Commands[(int)ctLinkNativeCmd] = m_Commands[(int)ctLinkConsoleExeCmd]; // unsupported currently

    LoadDefaultRegExArray();

    m_CompilerOptions.Clear();
    m_LinkerOptions.Clear();
    m_LinkLibs.Clear();
    m_CmdsBefore.Clear();
    m_CmdsAfter.Clear();
} // end of Reset

void CompilerGNUARM::LoadDefaultRegExArray()
{
    m_RegExes.Clear();
    m_RegExes.Add(RegExStruct(_("Fatal error"), cltError, _T("FATAL:[ \t]*(.*)"), 1));
    m_RegExes.Add(RegExStruct(_("'Instantiated from here' info"), cltNormal, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[ \t]+([iI]nstantiated from here.*)"), 3, 1, 2));    m_RegExes.Add(RegExStruct(_("Resource compiler error"), cltError, _T("windres.exe:[ \t](") + FilePathWithSpaces + _T("):([0-9]+):[ \t](.*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Resource compiler error"), cltError, _T("windres.exe:[ \t](") + FilePathWithSpaces + _T("):([0-9]+):[ \t](.*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Resource compiler error (2)"), cltError, _T("windres.exe:[ \t](.*)"), 1));
    m_RegExes.Add(RegExStruct(_("Preprocessor warning"), cltWarning, _T("(") + FilePathWithSpaces + _T("):([0-9]+):([0-9]+):[ \t]([Ww]arning:[ \t].*)"), 4, 1, 2));
    m_RegExes.Add(RegExStruct(_("Preprocessor error"), cltError, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[0-9]+:[ \t](.*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compiler warning"), cltWarning, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[ \t]([Ww]arning:[ \t].*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compiler error"), cltError, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[ \t](.*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Linker error"), cltError, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[0-9]+:[ \t](.*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Linker error (2)"), cltError, FilePathWithSpaces + _T("\\(.text\\+[0-9A-Za-z]+\\):([ \tA-Za-z0-9_:+/\\.-]+):[ \t](.*)"), 2, 1));
    m_RegExes.Add(RegExStruct(_("Linker error (lib not found)"), cltError, _T(".*(ld.*):[ \t](cannot find.*)"), 2, 1));
    m_RegExes.Add(RegExStruct(_("Undefined reference"), cltError, _T("(") + FilePathWithSpaces + _T("):[ \t](undefined reference.*)"), 2, 1));
    m_RegExes.Add(RegExStruct(_("General warning"), cltWarning, _T("([Ww]arning:[ \t].*)"), 1));
    m_RegExes.Add(RegExStruct(_("Auto-import info"), cltWarning, _T("([Ii]nfo:[ \t].*)\\(auto-import\\)"), 1));
}

AutoDetectResult CompilerGNUARM::AutoDetectInstallationDir()
{
    wxString sep = wxFileName::GetPathSeparator();
    if (platform::windows)
    {
        // Search for GNUARM installation dir
        wxString windir = wxGetOSDirectory();
        wxFileConfig ini(_T(""), _T(""), windir + _T("/GnuARM.ini"), _T(""), wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_NO_ESCAPE_CHARACTERS);
        // need it as const , so correct overloaded method will be selected
        wxString Programs = _T("C:\\Program Files");
        // what's the "Program Files" location
        // TO DO : support 64 bit ->    32 bit apps are in "ProgramFiles(x86)"
        //                              64 bit apps are in "ProgramFiles"
        wxGetEnv(_T("ProgramFiles"), &Programs);
        // need it as const , so correct overloaded method will be selected
        const wxString ProgramsConst = Programs + _T("\\GNUARM");
        m_MasterPath = ini.Read(_T("/InstallSettings/InstallPath"), ProgramsConst);

        if (wxFileExists(m_MasterPath + sep + _T("bin") + sep + m_Programs.C))
        {
            m_Programs.MAKE = _T("make.exe"); // we distribute "make" not "mingw32-make"
        }
    }
    else
        m_MasterPath = _T("/usr");

    AutoDetectResult ret = wxFileExists(m_MasterPath + sep + _T("bin") + sep + m_Programs.C) ? adrDetected : adrGuessed;
    if (ret == adrDetected)
    {
        AddIncludeDir(m_MasterPath + sep + _T("include"));
        AddLibDir(m_MasterPath + sep + _T("lib"));
    }
    return ret;
} // end of AutoDetectInstallationDir
