#include "compilerOW.h"
#include <wx/log.h>
#include <wx/intl.h>
#include <wx/regex.h>
#include <wx/config.h>
#include <wx/utils.h>

#ifdef __WXMSW__
    #include <wx/msw/registry.h>
#endif

CompilerOW::CompilerOW()
    : Compiler(_("OpenWatcom Compiler"))
{
    Reset();
}

CompilerOW::~CompilerOW()
{
	//dtor
}

Compiler * CompilerOW::CreateCopy()
{
    return new CompilerOW(*this);
}

void CompilerOW::Reset()
{
	m_Programs.C = "wcc386.exe";
	m_Programs.CPP = "wpp386.exe";
	m_Programs.LD = "wcl386.exe";
	m_Programs.LIB = "wlib_wrap.exe";
	m_Programs.WINDRES = "wrc.exe";
	m_Programs.MAKE = "mingw32-make.exe";
	
	m_Switches.includeDirs = "-I";
	m_Switches.libDirs = "-L";
	m_Switches.linkLibs = "";
	m_Switches.libPrefix = "";
	m_Switches.libExtension = "lib";
	m_Switches.defines = "-d";
	m_Switches.genericSwitch = "-";
	m_Switches.objectExtension = "obj";
	m_Switches.needDependencies = false;
	m_Switches.forceCompilerUseQuotes = false;
	m_Switches.forceLinkerUseQuotes = false;
	m_Switches.logging = clogSimple;
	m_Switches.buildMethod = cbmDirect;
	m_Switches.linkerNeedsLibPrefix = false;
	m_Switches.linkerNeedsLibExtension = true;

    m_Options.ClearOptions();
//	m_Options.AddOption(_("Produce debugging symbols"),
//				"-g",
//				_("Debugging"),
//				"",
//				true, 
//				"-o -o+space", 
//				_("You have optimizations enabled. This is Not A Good Thing(tm) when producing debugging symbols..."));

    m_Commands[(int)ctCompileObjectCmd] = "$compiler -zq $options $includes -fo=$object $file";
    m_Commands[(int)ctCompileResourceCmd] = "$rescomp -zq -fo=$resource_output $res_includes $file";
    m_Commands[(int)ctLinkExeCmd] = "$linker -zq -l=nt_win  $link_options $libdirs $link_objects -fe=$exe_output $libs $link_resobjects";
    m_Commands[(int)ctLinkConsoleExeCmd] = "$linker -zq -l=nt  $link_options $libdirs $link_objects -fe=$exe_output $libs $link_resobjects";
    m_Commands[(int)ctLinkDynamicCmd] = "$linker -zq -l=nt_dll $libdirs -fe=$exe_output $libs $link_objects $link_options";
    m_Commands[(int)ctLinkStaticCmd] = "$lib_linker $static_output $link_objects";

    m_RegExes.Clear();
    m_RegExes.Add(RegExStruct(_("Note"), cltError, "([A-Za-z0-9_:/\\.]+)\\(([0-9]+)\\): Note! (.+)", 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compiler error"), cltError, "([A-Za-z0-9_:/\\.]+)\\(([0-9]+)\\): Error! (.+)", 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compiler warning"), cltWarning, "([A-Za-z0-9_:/\\.]+)\\(([0-9]+)\\): Warning! (.+)", 3, 1, 2));

    m_CompilerOptions.Clear();
    m_LinkerOptions.Clear();
    m_LinkLibs.Clear();
    m_CmdsBefore.Clear();
    m_CmdsAfter.Clear();
}

AutoDetectResult CompilerOW::AutoDetectInstallationDir()
{
#ifdef __WXMSW__
    wxLogNull ln;
    wxRegKey key; // defaults to HKCR
    key.SetName("HKEY_LOCAL_MACHINE\\Software\\Open Watcom\\c_1.0");
    if (key.Open())
        // found; read it
        key.QueryValue("Install Location", m_MasterPath);

    if (m_MasterPath.IsEmpty())
        // just a guess; the default installation dir
        m_MasterPath = "C:\\watcom";

    if (!m_MasterPath.IsEmpty())
    {
        AddIncludeDir(m_MasterPath + wxFILE_SEP_PATH + "h");
        AddIncludeDir(m_MasterPath + wxFILE_SEP_PATH + "h" + wxFILE_SEP_PATH + "nt");
        m_ExtraPaths.Add(m_MasterPath + wxFILE_SEP_PATH + "binnt");
        m_ExtraPaths.Add(m_MasterPath + wxFILE_SEP_PATH + "binw");
    }
#endif
    wxSetEnv("WATCOM", m_MasterPath);

    return wxFileExists(m_MasterPath + wxFILE_SEP_PATH + "binnt" + wxFILE_SEP_PATH + m_Programs.C) ? adrDetected : adrGuessed;
}

void CompilerOW::LoadSettings(const wxString& baseKey)
{
    Compiler::LoadSettings(baseKey);
    wxSetEnv("WATCOM", m_MasterPath);
}

void CompilerOW::SetMasterPath(const wxString& path)
{
    Compiler::SetMasterPath(path);
    wxSetEnv("WATCOM", m_MasterPath);
}
