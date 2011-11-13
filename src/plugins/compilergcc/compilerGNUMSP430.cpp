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
#include <wx/msgdlg.h>
#ifdef __WXMSW__
#include <wx/msw/registry.h>
#endif // __WXMSW__
#include "compilerGNUMSP430.h"

CompilerGNUMSP430::CompilerGNUMSP430()
        : Compiler(_("GNU GCC Compiler for MSP430"),_T("msp430-gcc"))
{
    Reset();
}

CompilerGNUMSP430::~CompilerGNUMSP430()
{
    //dtor
}

Compiler * CompilerGNUMSP430::CreateCopy()
{
    Compiler* c = new CompilerGNUMSP430(*this);
    c->SetExtraPaths(m_ExtraPaths); // wxArrayString doesn't seem to be copied with the default copy ctor...
    return c;
}

void CompilerGNUMSP430::Reset()
{
    if (platform::windows)
    {
        m_Programs.C = _T("msp430-gcc.exe");
        m_Programs.CPP = _T("msp430-g++.exe");
        m_Programs.LD = _T("msp430-g++.exe");
        m_Programs.DBG = _T("msp430-insight.exe");
        m_Programs.LIB = _T("msp430-ar.exe");
        m_Programs.WINDRES = _T("");
        m_Programs.MAKE = _T("make.exe");
    }
    else
    {
        m_Programs.C = _T("msp430-gcc");
        m_Programs.CPP = _T("msp430-g++");
        m_Programs.LD = _T("msp430-g++");
        m_Programs.DBG = _T("msp430-insight");
        m_Programs.LIB = _T("msp430-ar");
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
    m_Switches.logging = clogFull;
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
    m_Options.AddOption(_("Enable warnings demanded by strict ISO C and ISO C++"), _T("-pedantic"), category);
    m_Options.AddOption(_("Treat as errors the warnings demanded by strict ISO C and ISO C++"), _T("-pedantic-errors"), category);
    m_Options.AddOption(_("Warn if main() is not conformant"), _T("-Wmain"), category);

    category = _("General Options");

    // general options
    m_Options.AddOption(_("Output an error if same variable is declared without extern in different modules"), _T("-fno-common"), category);
    m_Options.AddOption(_("Save intermediate files in the build directory"), _T("-save-temps"), category);

    // Startup options
    category = _("Linker and startup code");
    m_Options.AddOption(_("do not link against the default crt0.o, so you can add your own startup code (MSP430 specific)"), _T("-nocrt0"), category);
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
    category = _("MSP430 achitecture specific");

    m_Options.AddOption(_("Use subroutine call for function prologue/epilogue when possible"), _T("-msave-prologue"), category);
    m_Options.AddOption(_("Do not perform volatile workaround for bitwise operations"), _T("-mno-volatile-workaround"), category);
    m_Options.AddOption(_("No stack init in main()"), _T("-mno-stack-init"), category);
    m_Options.AddOption(_("Produce IAR assembler syntax"), _T("-mIAR"), category);
    m_Options.AddOption(_("Assume interrupt routine does not do hardware multiply"), _T("-mnoint-hwmul"), category);
    m_Options.AddOption(_("Issue inline multiplication code for 32-bit integers"), _T("-minline-hwmul"), category);
    m_Options.AddOption(_("Disable hardware multiplier"), _T("-mdisable-hwmul"), category);
    m_Options.AddOption(_("Force hardware multiplier"), _T("-mforce-hwmul"), category);
    m_Options.AddOption(_("Strict alignment for all structures"), _T("-mstrict-align"), category);
    m_Options.AddOption(_("Add stack information to profiler"), _T("-mpgr"), category);
    m_Options.AddOption(_("Add library profile information"), _T("-mpgl"), category);
    m_Options.AddOption(_("Add ordinary profile information"), _T("-mpgs"), category);
    m_Options.AddOption(_("Jump to specified routine at the end of main()"), _T("-mendup-at="), category);
    m_Options.AddOption(_("Specify the initial stack address"), _T("-minit-stack="), category);
    m_Options.AddOption(_("enable relaxation at assembly time"), _T("-mQ"), category);
    m_Options.AddOption(_("enable polymorph instructions"), _T("-mP"), category);

    // machine dependent options
    category = _("MSP430 MCU derivatives");

    m_Options.AddOption(_("MSP430 MSP1"), _T("-mmcu=msp1"), category);
    m_Options.AddOption(_("MSP430 MSP2"), _T("-mmcu=msp2"), category);
    m_Options.AddOption(_("MSP430 110"), _T("-mmcu=msp430x110"), category);
    m_Options.AddOption(_("MSP430 112"), _T("-mmcu=msp430x112"), category);
    m_Options.AddOption(_("MSP430 1101"), _T("-mmcu=msp430x1101"), category);
    m_Options.AddOption(_("MSP430 1111"), _T("-mmcu=msp430x1111"), category);
    m_Options.AddOption(_("MSP430 1121"), _T("-mmcu=msp430x1121"), category);
    m_Options.AddOption(_("MSP430 1122"), _T("-mmcu=msp430x1122"), category);
    m_Options.AddOption(_("MSP430 1132"), _T("-mmcu=msp430x1132"), category);
    m_Options.AddOption(_("MSP430 122"), _T("-mmcu=msp430x122"), category);
    m_Options.AddOption(_("MSP430 123"), _T("-mmcu=msp430x123"), category);
    m_Options.AddOption(_("MSP430 1222"), _T("-mmcu=msp430x1222"), category);
    m_Options.AddOption(_("MSP430 1232"), _T("-mmcu=msp430x1232"), category);
    m_Options.AddOption(_("MSP430 133"), _T("-mmcu=msp430x133"), category);
    m_Options.AddOption(_("MSP430 135"), _T("-mmcu=msp430x135"), category);
    m_Options.AddOption(_("MSP430 1331"), _T("-mmcu=msp430x1331"), category);
    m_Options.AddOption(_("MSP430 1351"), _T("-mmcu=msp430x1351"), category);
    m_Options.AddOption(_("MSP430 147"), _T("-mmcu=msp430x147"), category);
    m_Options.AddOption(_("MSP430 148"), _T("-mmcu=msp430x148"), category);
    m_Options.AddOption(_("MSP430 149"), _T("-mmcu=msp430x149"), category);
    m_Options.AddOption(_("MSP430 1471"), _T("-mmcu=msp430x1471"), category);
    m_Options.AddOption(_("MSP430 1481"), _T("-mmcu=msp430x1481"), category);
    m_Options.AddOption(_("MSP430 1491"), _T("-mmcu=msp430x1491"), category);
    m_Options.AddOption(_("MSP430 155"), _T("-mmcu=msp430x155"), category);
    m_Options.AddOption(_("MSP430 156"), _T("-mmcu=msp430x156"), category);
    m_Options.AddOption(_("MSP430 157"), _T("-mmcu=msp430x157"), category);
    m_Options.AddOption(_("MSP430 167"), _T("-mmcu=msp430x167"), category);
    m_Options.AddOption(_("MSP430 168"), _T("-mmcu=msp430x168"), category);
    m_Options.AddOption(_("MSP430 169"), _T("-mmcu=msp430x169"), category);
    m_Options.AddOption(_("MSP430 1610"), _T("-mmcu=msp430x1610"), category);
    m_Options.AddOption(_("MSP430 1611"), _T("-mmcu=msp430x1611"), category);
    m_Options.AddOption(_("MSP430 1612"), _T("-mmcu=msp430x1612"), category);
    m_Options.AddOption(_("MSP430 2001"), _T("-mmcu=msp430x2001"), category);
    m_Options.AddOption(_("MSP430 2011"), _T("-mmcu=msp430x2011"), category);
    m_Options.AddOption(_("MSP430 2002"), _T("-mmcu=msp430x2002"), category);
    m_Options.AddOption(_("MSP430 2012"), _T("-mmcu=msp430x2012"), category);
    m_Options.AddOption(_("MSP430 2003"), _T("-mmcu=msp430x2003"), category);
    m_Options.AddOption(_("MSP430 2013"), _T("-mmcu=msp430x2013"), category);
    m_Options.AddOption(_("MSP430 2101"), _T("-mmcu=msp430x2101"), category);
    m_Options.AddOption(_("MSP430 2111"), _T("-mmcu=msp430x2111"), category);
    m_Options.AddOption(_("MSP430 2121"), _T("-mmcu=msp430x2121"), category);
    m_Options.AddOption(_("MSP430 2131"), _T("-mmcu=msp430x2131"), category);
    m_Options.AddOption(_("MSP430 2234"), _T("-mmcu=msp430x2234"), category);
    m_Options.AddOption(_("MSP430 2254"), _T("-mmcu=msp430x2254"), category);
    m_Options.AddOption(_("MSP430 2274"), _T("-mmcu=msp430x2274"), category);
    m_Options.AddOption(_("MSP430 311"), _T("-mmcu=msp430x311"), category);
    m_Options.AddOption(_("MSP430 312"), _T("-mmcu=msp430x312"), category);
    m_Options.AddOption(_("MSP430 313"), _T("-mmcu=msp430x313"), category);
    m_Options.AddOption(_("MSP430 314"), _T("-mmcu=msp430x314"), category);
    m_Options.AddOption(_("MSP430 315"), _T("-mmcu=msp430x315"), category);
    m_Options.AddOption(_("MSP430 323"), _T("-mmcu=msp430x323"), category);
    m_Options.AddOption(_("MSP430 325"), _T("-mmcu=msp430x325"), category);
    m_Options.AddOption(_("MSP430 336"), _T("-mmcu=msp430x336"), category);
    m_Options.AddOption(_("MSP430 337"), _T("-mmcu=msp430x337"), category);
    m_Options.AddOption(_("MSP430 412"), _T("-mmcu=msp430x412"), category);
    m_Options.AddOption(_("MSP430 413"), _T("-mmcu=msp430x413"), category);
    m_Options.AddOption(_("MSP430 415"), _T("-mmcu=msp430x415"), category);
    m_Options.AddOption(_("MSP430 417"), _T("-mmcu=msp430x417"), category);
    m_Options.AddOption(_("MSP430 423"), _T("-mmcu=msp430x423"), category);
    m_Options.AddOption(_("MSP430 425"), _T("-mmcu=msp430x425"), category);
    m_Options.AddOption(_("MSP430 427"), _T("-mmcu=msp430x427"), category);
    m_Options.AddOption(_("MSP430 4250"), _T("-mmcu=msp430x4250"), category);
    m_Options.AddOption(_("MSP430 4260"), _T("-mmcu=msp430x4260"), category);
    m_Options.AddOption(_("MSP430 4270"), _T("-mmcu=msp430x4270"), category);
    m_Options.AddOption(_("MSP430 E423"), _T("-mmcu=msp430xE423"), category);
    m_Options.AddOption(_("MSP430 E425"), _T("-mmcu=msp430xE425"), category);
    m_Options.AddOption(_("MSP430 E427"), _T("-mmcu=msp430xE427"), category);
    m_Options.AddOption(_("MSP430 W423"), _T("-mmcu=msp430xW423"), category);
    m_Options.AddOption(_("MSP430 W425"), _T("-mmcu=msp430xW425"), category);
    m_Options.AddOption(_("MSP430 W427"), _T("-mmcu=msp430xW427"), category);
    m_Options.AddOption(_("MSP430 G437"), _T("-mmcu=msp430xG437"), category);
    m_Options.AddOption(_("MSP430 G438"), _T("-mmcu=msp430xG438"), category);
    m_Options.AddOption(_("MSP430 G439"), _T("-mmcu=msp430xG439"), category);
    m_Options.AddOption(_("MSP430 435"), _T("-mmcu=msp430x435"), category);
    m_Options.AddOption(_("MSP430 436"), _T("-mmcu=msp430x436"), category);
    m_Options.AddOption(_("MSP430 437"), _T("-mmcu=msp430x437"), category);
    m_Options.AddOption(_("MSP430 447"), _T("-mmcu=msp430x447"), category);
    m_Options.AddOption(_("MSP430 448"), _T("-mmcu=msp430x448"), category);
    m_Options.AddOption(_("MSP430 449"), _T("-mmcu=msp430x449"), category);
    m_Options.AddOption(_("MSP430 4616"), _T("-mmcu=msp430xG4616"), category);
    m_Options.AddOption(_("MSP430 4617"), _T("-mmcu=msp430xG4617"), category);
    m_Options.AddOption(_("MSP430 4618"), _T("-mmcu=msp430xG4618"), category);
    m_Options.AddOption(_("MSP430 4619"), _T("-mmcu=msp430xG4619"), category);

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

void CompilerGNUMSP430::LoadDefaultRegExArray()
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

AutoDetectResult CompilerGNUMSP430::AutoDetectInstallationDir()
{
    wxString sep = wxFileName::GetPathSeparator();
#ifdef __WXMSW__
    m_MasterPath = _T("C:\\HighTec\\Msp430"); // just a guess

    //    wxLogNull ln;
        wxRegKey key; // defaults to HKCR
        key.SetName(_T("HKEY_LOCAL_MACHINE\\Software\\HighTec EDV-Systeme\\Msp430\\"));
        if (key.Exists() && key.Open(wxRegKey::Read))
        {
            // found; read it
            if (key.HasValue(_T("InstallPath")))
            {
                key.QueryValue(_T("InstallPath"), m_MasterPath);
            }
        }
#else
        m_MasterPath = _T("/usr/local/msp430");
#endif // __WXMSW__
    AutoDetectResult ret = wxFileExists(m_MasterPath + sep + _T("bin") + sep + m_Programs.C) ? adrDetected : adrGuessed;
    if (ret == adrDetected)
    {
          AddIncludeDir(m_MasterPath + sep + _T("msp430") + sep + _T("include"));
    }
    return ret;
}
