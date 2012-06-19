/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
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
#ifdef __WXMSW__
#include <wx/msw/registry.h>
#endif // __WXMSW__
#include "compilerGNUPOWERPC.h"

CompilerGNUPOWERPC::CompilerGNUPOWERPC()
        : Compiler(_("GNU GCC Compiler for PowerPC"),_T("ppc-gcc"))
{
    Reset();
}

CompilerGNUPOWERPC::~CompilerGNUPOWERPC()
{
    //dtor
}

Compiler * CompilerGNUPOWERPC::CreateCopy()
{
    Compiler* c = new CompilerGNUPOWERPC(*this);
    c->SetExtraPaths(m_ExtraPaths); // wxArrayString doesn't seem to be copied with the default copy ctor...
    return c;
}

void CompilerGNUPOWERPC::Reset()
{
    if (platform::windows)
    {
        m_Programs.C = _T("ppc-gcc.exe");
        m_Programs.CPP = _T("ppc-g++.exe");
        m_Programs.LD = _T("ppc-g++.exe");
        m_Programs.DBG = _T("ppc-insight.exe");
        m_Programs.DBGconfig = wxEmptyString;
        m_Programs.LIB = _T("ppc-ar.exe");
        m_Programs.WINDRES = _T("");
        m_Programs.MAKE = _T("make.exe");
    }
    else
    {
        m_Programs.C = _T("ppc-gcc");
        m_Programs.CPP = _T("ppc-g++");
        m_Programs.LD = _T("ppc-g++");
        m_Programs.DBG = _T("ppc-insight");
        m_Programs.DBGconfig = wxEmptyString;
        m_Programs.LIB = _T("ppc-ar");
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
    m_Switches.logging = CompilerSwitches::defaultLogging;
    m_Switches.libPrefix = _T("lib");
    m_Switches.libExtension = _T("a");
    m_Switches.linkerNeedsLibPrefix = false;
    m_Switches.linkerNeedsLibExtension = false;

    // Summary of GCC options: http://gcc.gnu.org/onlinedocs/gcc/Option-Summary.html

    m_Options.ClearOptions();
    m_Options.AddOption(_("Produce debugging symbols"),
                        _T("-g"),
                        _("Debugging"),
                        _T(""),
                        true,
                        _T("-O -O1 -O2 -O3 -Os"),
                        _("You have optimizations enabled. This will make debugging difficult because variables may be optimized away etc."));
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
    m_Options.AddOption(_("Have g++ follow the 1998 ISO C++ language standard"), _T("-std=c++98"), category);
    m_Options.AddOption(_("Have g++ follow the coming C++0x ISO C++ language standard"), _T("-std=c++0x"), category);
    m_Options.AddOption(_("Have g++ follow the C++11 ISO C++ language standard"), _T("-std=c++11"), category);
    m_Options.AddOption(_("Enable warnings demanded by strict ISO C and ISO C++"), _T("-pedantic"), category);
    m_Options.AddOption(_("Treat as errors the warnings demanded by strict ISO C and ISO C++"), _T("-pedantic-errors"), category);
    m_Options.AddOption(_("Warn if main() is not conformant"), _T("-Wmain"), category);
    m_Options.AddOption(_("zero as null pointer constant"), _T("-Wzero-as-null-pointer-constant"), category);

    category = _("General Options");

    // general options
    m_Options.AddOption(_("Output an error if same variable is declared without extern in different modules"), _T("-fno-common"), category);
    m_Options.AddOption(_("Save intermediate files in the build directory"), _T("-save-temps"), category);

    // Startup options
    category = _("Linker and startup code");
    m_Options.AddOption(_("do not link against the default crt0.o, so you can add your own startup code (PowerPC specific)"), _T("-nocrt0"), category);
    m_Options.AddOption(_("do not link against standard system startup files"), _T("-nostartfiles"), category);
    m_Options.AddOption(_("only search library directories explicitly specified on the command line"), _T("-nostdlib"), category);

    // optimization
    category = _("Optimization");
    m_Options.AddOption(_("Strip all symbols from binary (minimizes size)"), _T(""), category, _T("-s"), true, _T("-g -ggdb"), _("Stripping the binary will strip debugging symbols as well!"));
    m_Options.AddOption(_("Optimize generated code (for speed)"), _T("-O"), category);
    m_Options.AddOption(_("Optimize more (for speed)"), _T("-O1"), category);
    m_Options.AddOption(_("Optimize even more (for speed)"), _T("-O2"), category);
    m_Options.AddOption(_("Optimize fully (for speed)"), _T("-O3"), category);
    m_Options.AddOption(_("Optimize generated code (for size)"), _T("-Os"), category);
    m_Options.AddOption(_("Expensive optimizations"), _T("-fexpensive-optimizations"), category);
    m_Options.AddOption(_("No instruction scheduling before reload"), _T("-fno-schedule-insns"), category);
    m_Options.AddOption(_("No instruction scheduling after reload"), _T("-fno-schedule-insns2"), category);

    // machine dependent options
    category = _("PowerPC achitecture specific");

    m_Options.AddOption(_("Generate 32-bit code"), _T("-m32"), category);
    m_Options.AddOption(_("Use EABI"), _T("-meabi"), category);
    m_Options.AddOption(_("Produce big endian code"), _T("-mbig-endian"), category);
    m_Options.AddOption(_("Produce little endian code"), _T("-mlittle-endian"), category);
    m_Options.AddOption(_("Do not allow bit-fields to cross word boundaries"), _T("-mno-bit-word"), category);
    m_Options.AddOption(_("Align to the base type of the bit-field"), _T("-mbit-align"), category);
    m_Options.AddOption(_("Do not generate single field mfcr instruction"), _T("-mno-mfcrf"), category);
    m_Options.AddOption(_("Generate single field mfcr instruction"), _T("-mmfcrf"), category);
    m_Options.AddOption(_("Generate load/store with update instructions"), _T("-mupdate"), category);
    m_Options.AddOption(_("Generate load/store multiple instructions"), _T("-mmultiple"), category);
    m_Options.AddOption(_("Do not use hardware floating point"), _T("-msoft-float"), category);
    m_Options.AddOption(_("Use hardware floating point"), _T("-mhard-float"), category);
    m_Options.AddOption(_("Select method for sdata handling"), _T("-msdata="), category);
    m_Options.AddOption(_("Specify alignment of structure fields default/natural"), _T("-malign="), category);
    m_Options.AddOption(_("Avoid all range limits on call instructions"), _T("-mlongcall"), category);
    m_Options.AddOption(_("Using floating point in the GPRs"), _T("-mfloat-gprs=yes"), category);
    m_Options.AddOption(_("Not using floating point in the GPRs"), _T("-mfloat-gprs=no"), category);
//    m_Options.AddOption(_("Specify size of long double (64 or 128 bits)"), _T("-mlong-double="), category);
    m_Options.AddOption(_("Enable debug output"), _T("-mdebug"), category);
    m_Options.AddOption(_("Schedule code for given CPU"), _T("-mtune="), category);
    m_Options.AddOption(_("Allow symbolic names for registers"), _T("-mregnames"), category);
    m_Options.AddOption(_("Do not allow symbolic names for registers"), _T("-mno-regnames"), category);
    m_Options.AddOption(_("Support for GCC's -mrelocatble option"), _T("-mrelocatable"), category);
    m_Options.AddOption(_("Support for GCC's -mrelocatble-lib option"), _T("-mrelocatable-lib"), category);

    // machine dependent options
    category = _("PowerPC MCU derivatives");

    m_Options.AddOption(_("Select CPU PowerPC 5xx"), _T("-mcpu=505"), category);
    m_Options.AddOption(_("Select CPU PowerPC 823"), _T("-mcpu=823"), category);
    m_Options.AddOption(_("Select PowerPC 5200"), _T("-mcpu=603e -msoft-float"), category);

    m_Commands[(int)ctCompileObjectCmd].push_back(CompilerTool(_T("$compiler $options $includes -c $file -o $object")));
    m_Commands[(int)ctGenDependenciesCmd].push_back(CompilerTool(_T("$compiler -MM $options -MF $dep_object -MT $object $includes $file")));
    m_Commands[(int)ctCompileResourceCmd].push_back(CompilerTool(_T("$rescomp -i $file -J rc -o $resource_output -O coff $res_includes")));
    m_Commands[(int)ctLinkConsoleExeCmd].push_back(CompilerTool(_T("$linker $libdirs -o $exe_output $link_objects $link_resobjects $link_options $libs")));
    if (platform::windows)
    {
        m_Commands[(int)ctLinkExeCmd].push_back(CompilerTool(_T("$linker $libdirs -o $exe_output $link_objects $link_resobjects $link_options $libs -mwindows")));
        m_Commands[(int)ctLinkDynamicCmd].push_back(CompilerTool(_T("$linker -shared -Wl,--output-def=$def_output -Wl,--out-implib=$static_output -Wl,--dll $libdirs $link_objects $link_resobjects -o $exe_output $link_options $libs")));
    }
    else
    {
        m_Commands[(int)ctLinkExeCmd] = m_Commands[(int)ctLinkConsoleExeCmd]; // no -mwindows
        m_Commands[(int)ctLinkDynamicCmd].push_back(CompilerTool(_T("$linker -shared $libdirs $link_objects $link_resobjects -o $exe_output $link_options $libs")));
    }
    m_Commands[(int)ctLinkStaticCmd].push_back(CompilerTool(_T("$lib_linker -rs $static_output $link_objects")));
    m_Commands[(int)ctLinkNativeCmd] = m_Commands[(int)ctLinkConsoleExeCmd]; // unsupported currently

    LoadDefaultRegExArray();

    m_CompilerOptions.Clear();
    m_LinkerOptions.Clear();
    m_LinkLibs.Clear();
    m_CmdsBefore.Clear();
    m_CmdsAfter.Clear();
}

void CompilerGNUPOWERPC::LoadDefaultRegExArray()
{
    m_RegExes.Clear();
    m_RegExes.Add(RegExStruct(_("Fatal error"), cltError, _T("FATAL:[ \t]*(.*)"), 1));
    m_RegExes.Add(RegExStruct(_("'In function...' info"), cltInfo, _T("(") + FilePathWithSpaces + _T("):[ \t]+") + _T("([iI]n ([cC]lass|[cC]onstructor|[dD]estructor|[fF]unction|[mM]ember [fF]unction).*)"), 2, 1));
    m_RegExes.Add(RegExStruct(_("'Instantiated from' info"), cltInfo, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[ \t]+([iI]nstantiated from .*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Resource compiler error"), cltError, _T("windres.exe:[ \t](") + FilePathWithSpaces + _T("):([0-9]+):[ \t](.*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Resource compiler error (2)"), cltError, _T("windres.exe:[ \t](.*)"), 1));
    m_RegExes.Add(RegExStruct(_("Preprocessor warning"), cltWarning, _T("(") + FilePathWithSpaces + _T("):([0-9]+):([0-9]+):[ \t]([Ww]arning:[ \t].*)"), 4, 1, 2));
    m_RegExes.Add(RegExStruct(_("Preprocessor error"), cltError, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[0-9]+:[ \t](.*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compiler warning"), cltWarning, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[ \t]([Ww]arning:[ \t].*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compiler note"), cltInfo, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[ \t]([Nn]ote:[ \t].*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("General note"), cltInfo, _T("([Nn]ote:[ \t].*)"), 1));
    m_RegExes.Add(RegExStruct(_("Compiler error"), cltError, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[ \t](.*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Linker error"), cltError, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[0-9]+:[ \t](.*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Linker error (2)"), cltError, FilePathWithSpaces + _T("\\(.text\\+[0-9A-Za-z]+\\):([ \tA-Za-z0-9_:+/\\.-]+):[ \t](.*)"), 2, 1));
    m_RegExes.Add(RegExStruct(_("Linker error (lib not found)"), cltError, _T(".*(ld.*):[ \t](cannot find.*)"), 2, 1));
    m_RegExes.Add(RegExStruct(_("Undefined reference"), cltError, _T("(") + FilePathWithSpaces + _T("):[ \t](undefined reference.*)"), 2, 1));
    m_RegExes.Add(RegExStruct(_("General warning"), cltWarning, _T("([Ww]arning:[ \t].*)"), 1));
    m_RegExes.Add(RegExStruct(_("Auto-import info"), cltInfo, _T("([Ii]nfo:[ \t].*)\\(auto-import\\)"), 1));
}

AutoDetectResult CompilerGNUPOWERPC::AutoDetectInstallationDir()
{
    wxString sep = wxFileName::GetPathSeparator();
#ifdef __WXMSW__
     m_MasterPath = _T("C:\\HighTec\\PowerPC"); // just a guess

    //    wxLogNull ln;
    wxRegKey key; // defaults to HKCR
     key.SetName(_T("HKEY_LOCAL_MACHINE\\Software\\HighTec EDV-Systeme\\PowerPC\\"));
     if (key.Exists() && key.Open(wxRegKey::Read))
     {
     // found; read it
    if (key.HasValue(_T("InstallPath")))
        {
            key.QueryValue(_T("InstallPath"), m_MasterPath);
        }
      }
#else
     m_MasterPath = _T("/usr/local/ppc");
#endif // __WXMSW__
    AutoDetectResult ret = wxFileExists(m_MasterPath + sep + _T("bin") + sep + m_Programs.C) ? adrDetected : adrGuessed;
//    if (ret == adrDetected)
//    {
//          AddIncludeDir(m_MasterPath + sep + _T("ppc-ht-eabi") + sep + _T("include"));
//    }
    return ret;
}
