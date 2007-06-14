#include <sdk.h>
#include <prep.h>
#ifndef CB_PRECOMP
    #include <wx/intl.h>
    #include <wx/regex.h>
#endif
#include "compilerGNUAVR.h"

CompilerGNUAVR::CompilerGNUAVR()
    : Compiler(_("GNU AVR GCC Compiler"),_T("avr-gcc"))
{
    Reset();
}

CompilerGNUAVR::~CompilerGNUAVR()
{
    //dtor
}

Compiler * CompilerGNUAVR::CreateCopy()
{
    Compiler* c = new CompilerGNUAVR(*this);
    c->SetExtraPaths(m_ExtraPaths); // wxArrayString doesn't seem to be copied with the default copy ctor...
    return c;
} // end of CreateCopy

void CompilerGNUAVR::Reset()
{
    if (platform::windows)
    {
        m_Programs.C = _T("avr-gcc.exe");
        m_Programs.CPP = _T("avr-g++.exe");
        m_Programs.LD = _T("avr-g++.exe");
        m_Programs.DBG = _T("avr-gdb.exe");
        m_Programs.LIB = _T("avr-ar.exe");
        m_Programs.WINDRES = _T("");
        m_Programs.MAKE = _T("make.exe");
    }
    else
    {
        m_Programs.C = _T("avr-gcc");
        m_Programs.CPP = _T("avr-g++");
        m_Programs.LD = _T("avr-g++");
        m_Programs.DBG = _T("avr-gdb");
        m_Programs.LIB = _T("avr-ar");
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
    category = _("AVR CPU architecture specific");
    m_Options.AddOption(_("avr1 (only assembler)"), _T("-mmcu=avr1"), category);
    m_Options.AddOption(_("at90s1200 (only assembler)"), _T("-mmcu=at90s1200"), category);
    m_Options.AddOption(_("attiny10 (only assembler)"), _T("-mmcu=attiny10"), category);
    m_Options.AddOption(_("attiny11 (only assembler)"), _T("-mmcu=attiny11"), category);
    m_Options.AddOption(_("attiny12 (only assembler)"), _T("-mmcu=attiny12"), category);
    m_Options.AddOption(_("attiny15 (only assembler)"), _T("-mmcu=attiny15"), category);
    m_Options.AddOption(_("attiny28 (only assembler)"), _T("-mmcu=attiny28"), category);
    m_Options.AddOption(_("avr2"), _T("-mmcu=avr2"), category);
    m_Options.AddOption(_("at90s2313"), _T("-mmcu=at90s2313"), category);
    m_Options.AddOption(_("at90s2323"), _T("-mmcu=at90s2323"), category);
    m_Options.AddOption(_("attiny22"), _T("-mmcu=attiny22"), category);
    m_Options.AddOption(_("at90s2333"), _T("-mmcu=at90s2333"), category);
    m_Options.AddOption(_("at90s2343"), _T("-mmcu=at90s2343"), category);
    m_Options.AddOption(_("at90s4414"), _T("-mmcu=at90s4414"), category);
    m_Options.AddOption(_("at90s4433"), _T("-mmcu=at90s4433"), category);
    m_Options.AddOption(_("at90s4434"), _T("-mmcu=at90s4434"), category);
    m_Options.AddOption(_("at90s8515"), _T("-mmcu=at90s8515"), category);
    m_Options.AddOption(_("at90c8534"), _T("-mmcu=at90c8534"), category);
    m_Options.AddOption(_("at90s8535"), _T("-mmcu=at90s8535"), category);
    m_Options.AddOption(_("avr3"), _T("-mmcu=avr3"), category);
    m_Options.AddOption(_("atmega103"), _T("-mmcu=atmega103"), category);
    m_Options.AddOption(_("atmega603"), _T("-mmcu=atmega603"), category);
    m_Options.AddOption(_("at43usb320"), _T("-mmcu=at43usb320"), category);
    m_Options.AddOption(_("at76c711"), _T("-mmcu=at76c711"), category);
    m_Options.AddOption(_("avr4"), _T("-mmcu=avr4"), category);
    m_Options.AddOption(_("atmega8"), _T("-mmcu=atmega8"), category);
    m_Options.AddOption(_("atmega83"), _T("-mmcu=atmega83"), category);
    m_Options.AddOption(_("atmega85"), _T("-mmcu=atmega85"), category);
    m_Options.AddOption(_("avr5"), _T("-mmcu=avr5"), category);
    m_Options.AddOption(_("atmega16"), _T("-mmcu=atmega16"), category);
    m_Options.AddOption(_("atmega161"), _T("-mmcu=atmega161"), category);
    m_Options.AddOption(_("atmega163"), _T("-mmcu=atmega163"), category);
    m_Options.AddOption(_("atmega32"), _T("-mmcu=atmega32"), category);
    m_Options.AddOption(_("atmega323"), _T("-mmcu=atmega323"), category);
    m_Options.AddOption(_("atmega64"), _T("-mmcu=atmega64"), category);
    m_Options.AddOption(_("atmega128"), _T("-mmcu=atmega128"), category);
    m_Options.AddOption(_("at43usb355"), _T("-mmcu=at43usb355"), category);
    m_Options.AddOption(_("at94k"), _T("-mmcu=at94k"), category);
    m_Options.AddOption(_("Output instruction sizes to the asm file"), _T("-msize"), category);
    m_Options.AddOption(_("Initial stack address"), _T("-minit-stack=N"), category);
    m_Options.AddOption(_("Disable interrupts"), _T("-mno-interrupts"), category);
    m_Options.AddOption(_("Expand functions prologues/epilogues"), _T("-mcall-prologues"), category);
    m_Options.AddOption(_("Disable tablejump instructions"), _T("-mno-tablejump"), category);
    m_Options.AddOption(_("8 bits stack pointer"), _T("-mtiny-stack"), category);
    m_Options.AddOption(_("int as 8 bit integer"), _T("-mint8"), category);

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

void CompilerGNUAVR::LoadDefaultRegExArray()
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
} // end of LoadDefaultRegExArray

AutoDetectResult CompilerGNUAVR::AutoDetectInstallationDir()
{
    wxString sep = wxFileName::GetPathSeparator();
    if (platform::windows)
        m_MasterPath = _T("C:\\WinAVR");
    else
        m_MasterPath = _T("/usr");

    AutoDetectResult ret = wxFileExists(m_MasterPath + sep + _T("bin") + sep + m_Programs.C) ? adrDetected : adrGuessed;
    if (ret == adrDetected)
    {
        if (platform::windows)
        {
            AddIncludeDir(m_MasterPath + sep + _T("avr\\include"));
            AddLibDir(m_MasterPath + sep + _T("avr\\lib"));
            m_ExtraPaths.Add(m_MasterPath + sep + _T("utils") + sep + _T("bin")); // for make
        }
        else
        {
            AddIncludeDir(m_MasterPath + sep + _T("include"));
            AddLibDir(m_MasterPath + sep + _T("lib"));
        }
    }
    return ret;
} // end of AutoDetectInstallationDir
