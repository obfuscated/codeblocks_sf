#ifdef __WXMSW__
// this compiler is valid only in windows

#include <sdk.h>
#include "compilerBCC.h"
#include <wx/intl.h>
#include <wx/regex.h>
#include <wx/msw/registry.h>

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

    wxString category = _("Optimization");
    m_Options.AddOption(_("Optimizations for size"), _T("-O1"), category);
    m_Options.AddOption(_("Optimizations for speed"), _T("-O2"), category);
    m_Options.AddOption(_("Optimize jumps"), _T("-O"), category);
    m_Options.AddOption(_("Eliminate duplicate expressions"), _T("-Oc"), category);
    m_Options.AddOption(_("Disable optimizations"), _T("-Od"), category);
    m_Options.AddOption(_("Expand intrinsic functions"), _T("-Oi"), category);
    m_Options.AddOption(_("Enable instruction scheduling for Pentium"), _T("-OS"), category);
    m_Options.AddOption(_("Disable instruction scheduling"), _T("-O-S"), category);
    m_Options.AddOption(_("Enable loop induction variable and strength reduction"), _T("-Ov"), category);
    m_Options.AddOption(_("Disable register variables"), _T("-r-"), category);
    m_Options.AddOption(_("Merge duplicate strings"), _T("-d"), category);
    m_Options.AddOption(_("Function stack frame optimization"), _T("-k-"), category);


    category = _("C++ Features");
    m_Options.AddOption(_("Disable runtime type information"), _T("-RT-"), category);
    m_Options.AddOption(_("Disable exception handling"), _T("-x-"), category);
    m_Options.AddOption(_("Enable destructor cleanup"), _T("-xd"), category);
    m_Options.AddOption(_("Use global destructor count"), _T("-xdg"), category);
    m_Options.AddOption(_("Enable fast exception prologs"), _T("-xf"), category);
    m_Options.AddOption(_("Enable exception location information"), _T("-xp"), category);
    m_Options.AddOption(_("Enable slow exception epilogues"), _T("-xs"), category);
    m_Options.AddOption(_("Zero length empty class members"), _T("-Vx"), category);
    m_Options.AddOption(_("Zero-length empty base classes"), _T("-Ve"), category);


    category = _("C Features");
    m_Options.AddOption(_("Emulate floating point"), _T("-f"), category);
    m_Options.AddOption(_("Disable floating point"), _T("-f-"), category);
    m_Options.AddOption(_("Fast floating point"), _T("-ff"), category);
    m_Options.AddOption(_("Pentium FDIV workaround"), _T("-fp"), category);
    m_Options.AddOption(_("Pascal calling convention"), _T("-p"), category);
    m_Options.AddOption(_("C calling convention"), _T("-pc"), category);
    m_Options.AddOption(_("__msfastcall calling convention"), _T("-pm"), category);
    m_Options.AddOption(_("fastcall calling convention"), _T("-pr"), category);
    m_Options.AddOption(_("stdcall calling convention"), _T("-ps"), category);
    m_Options.AddOption(_("Align data by byte"), _T("-a1"), category);
    m_Options.AddOption(_("Align data by word (2 bytes)"), _T("-a2"), category);
    m_Options.AddOption(_("Align data by double word (4 bytes)"), _T("-a4"), category);
    m_Options.AddOption(_("Align data by quad word (8 bytes)"), _T("-a8"), category);
    m_Options.AddOption(_("Align data by paragraph (16 bytes)"), _T("-a16"), category);
    m_Options.AddOption(_("Use minimum sized enums"), _T("-b-"), category);
    m_Options.AddOption(_("Use unsigned char"), _T("-K"), category);


    category = _("Language");
    m_Options.AddOption(_("ANSI keywords and extensions"), _T("-A"), category);
    m_Options.AddOption(_("Kernighan and Ritchie keywords and extensions"), _T("-AK"), category);
    m_Options.AddOption(_("Borland C++ keywords and extensions"), _T("-AT"), category);
    m_Options.AddOption(_("UNIX V keywords and extensions"), _T("-AU"), category);
    m_Options.AddOption(_("Allow nested comments"), _T("-C"), category);


    category = _("Debugging");
    m_Options.AddOption(_("debugging on, inline expansion off"), _T("-v"), category);
    m_Options.AddOption(_("debugging off, inline expansion on"), _T("-v-"), category);
    m_Options.AddOption(_("inline function expansion on"), _T("-vi"), category);
    m_Options.AddOption(_("inline function expansion off"), _T("-vi-"), category);
    m_Options.AddOption(_("Include line numbers"), _T("-y"), category);


    category = _("Architecture");
    m_Options.AddOption(_("Optimize for 80386"), _T("-3"), category);
    m_Options.AddOption(_("Optimize for 80486"), _T("-4"), category);
    m_Options.AddOption(_("Optimize for Pentium"), _T("-5"), category);
    m_Options.AddOption(_("Optimize for Pentium Pro, Pentium II, Pentium III"), _T("-6"), category);


    category = _("Target");
    m_Options.AddOption(_("Windows application"), _T("-tW"), category);
    m_Options.AddOption(_("Console application"), _T("-tWC"), category);
    m_Options.AddOption(_(".DLL executable"), _T("-tWD"), category);
    m_Options.AddOption(_("32-bit multi-threaded"), _T("-tWM"), category);
    m_Options.AddOption(_("Target uses the dynamic RTL"), _T("-tWR"), category);
    m_Options.AddOption(_("Target uses the VCL"), _T("-tWV"), category);


    category = _("Warnings");
    m_Options.AddOption(_("Display all warnings"), _T("-w"), category);


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

    // try to detect Installation dir
    wxRegKey key(_T("HKEY_LOCAL_MACHINE\\SOFTWARE\\Borland\\C++Builder\\5.0"));
    if(key.Exists() && key.Open(wxRegKey::Read))
    {
        wxString dir;
        key.QueryValue(_T("RootDir"), dir);
        if (!dir.IsEmpty() && wxDirExists(dir))
        {
            m_MasterPath=dir;
        }
        key.Close();
    }

    wxString sep = wxString(wxFileName::GetPathSeparator());
    if (!m_MasterPath.IsEmpty())
    {
        AddIncludeDir(m_MasterPath + sep + _T("include"));
        AddLibDir(m_MasterPath + sep + _T("lib"));
    }

    return wxFileExists(m_MasterPath + sep + _T("bin") + sep + m_Programs.C) ? adrDetected : adrGuessed;
}

#endif // __WXMSW__
