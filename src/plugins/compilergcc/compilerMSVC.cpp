#include "compilerMSVC.h"
#include <wx/log.h>
#include <wx/intl.h>
#include <wx/regex.h>
#include <wx/config.h>

#ifdef __WXMSW__
    #include <wx/msw/registry.h>
#endif

CompilerMSVC::CompilerMSVC()
    : Compiler(_("Microsoft Visual C++ Toolkit 2003"))
{
    Reset();
}

CompilerMSVC::~CompilerMSVC()
{
	//dtor
}

Compiler * CompilerMSVC::CreateCopy()
{
    return new CompilerMSVC(*this);
}

void CompilerMSVC::Reset()
{
	m_Programs.C = "cl.exe";
	m_Programs.CPP = "cl.exe";
	m_Programs.LD = "link.exe";
	m_Programs.LIB = "link.exe";
	m_Programs.WINDRES = "rc.exe"; // platform SDK is needed for this
	m_Programs.MAKE = "mingw32-make.exe";
	
	m_Switches.includeDirs = "/I";
	m_Switches.libDirs = "/LIBPATH:";
	m_Switches.linkLibs = "";
	m_Switches.defines = "/D";
	m_Switches.genericSwitch = "/";
	m_Switches.objectExtension = "obj";
	m_Switches.needDependencies = false;
	m_Switches.forceCompilerUseQuotes = false;
	m_Switches.forceLinkerUseQuotes = false;
	m_Switches.logging = clogNone;
	m_Switches.buildMethod = cbmDirect;
	m_Switches.libPrefix = "";
	m_Switches.libExtension = "lib";
	m_Switches.linkerNeedsLibPrefix = false;
	m_Switches.linkerNeedsLibExtension = true;

    m_Options.ClearOptions();
	m_Options.AddOption(_("Produce debugging symbols"),
				"/Zi",
				_("Debugging"),
				"",
				true, 
				"/Og /O1 /O2 /Os /Ot /Ox", 
				_("You have optimizations enabled. This is Not A Good Thing(tm) when producing debugging symbols..."));
	m_Options.AddOption(_("Enable all compiler warnings"), "/Wall", _("Warnings"));
	m_Options.AddOption(_("Enable warnings level 1"), "/W1", _("Warnings"));
	m_Options.AddOption(_("Enable warnings level 2"), "/W2", _("Warnings"));
	m_Options.AddOption(_("Enable warnings level 3"), "/W3", _("Warnings"));
	m_Options.AddOption(_("Enable warnings level 4"), "/W4", _("Warnings"));
	m_Options.AddOption(_("Enable 64bit porting warnings"), "/Wp64", _("Warnings"));
	m_Options.AddOption(_("Treat warnings as errors"), "/WX", _("Warnings"));
	m_Options.AddOption(_("Enable global optimization"), "/Og", _("Optimization"));
	m_Options.AddOption(_("Maximum optimization (no need for other options)"), "/Ox", _("Optimization"));
	m_Options.AddOption(_("Minimize space"), "/O1", _("Optimization"));
	m_Options.AddOption(_("Maximize speed"), "/O2", _("Optimization"));
	m_Options.AddOption(_("Favor code space"), "/Os", _("Optimization"));
	m_Options.AddOption(_("Favor code speed"), "/Ot", _("Optimization"));
	m_Options.AddOption(_("Enable C++ RTTI"), "/GR", _("C++ Features"));
	m_Options.AddOption(_("Enable C++ exception handling"), "/GX", _("C++ Features"));
	m_Options.AddOption(_("Optimize for 80386"), "/G3", _("Architecture"));
	m_Options.AddOption(_("Optimize for 80486"), "/G4", _("Architecture"));
	m_Options.AddOption(_("Optimize for Pentium"), "/G5", _("Architecture"));
	m_Options.AddOption(_("Optimize for Pentium Pro, Pentium II, Pentium III"), "/G6", _("Architecture"));
	m_Options.AddOption(_("Optimize for Pentium 4 or Athlon"), "/G7", _("Architecture"));
	m_Options.AddOption(_("Enable SSE instruction set"), "/arch:SSE", _("Architecture"));
	m_Options.AddOption(_("Enable SSE2 instruction set"), "/arch:SSE2", _("Architecture"));
	m_Options.AddOption(_("Enable minimal rebuild"), "/Gm", _("Others"));
	m_Options.AddOption(_("Enable link-time code generation"), "/GL", _("Others"), "", true, "/Zi /ZI", _("Link-time code generation is incompatible with debugging info"));
	m_Options.AddOption(_("Optimize for windows application"), "/GA", _("Others"));
	m_Options.AddOption(_("__cdecl calling convention"), "/Gd", _("Others"));
	m_Options.AddOption(_("__fastcall calling convention"), "/Gr", _("Others"));
	m_Options.AddOption(_("__stdcall calling convention"), "/Gz", _("Others"));

    m_Commands[(int)ctCompileObjectCmd] = "$compiler /nologo $options $includes /c $file /Fo$object";
    m_Commands[(int)ctCompileResourceCmd] = "$rescomp $res_includes -fo$resource_output $file";
    m_Commands[(int)ctLinkExeCmd] = "$linker /nologo /subsystem:windows $libdirs /out:$exe_output $libs $link_objects $link_resobjects $link_options";
    m_Commands[(int)ctLinkConsoleExeCmd] = "$linker /nologo $libdirs /out:$exe_output $libs $link_objects $link_resobjects $link_options";
    m_Commands[(int)ctLinkDynamicCmd] = "$linker /dll /nologo $libdirs /out:$exe_output $libs $link_objects $link_options";
    m_Commands[(int)ctLinkStaticCmd] = "$lib_linker /lib /nologo $libdirs /out:$static_output $libs $link_objects $link_options";

    m_RegExes.Clear();
    m_RegExes.Add(RegExStruct(_("Compiler warning"), cltWarning, "([ \tA-Za-z0-9_:\\-\\+/\\.]+)\\(([0-9]+)\\) :[ \t]([Ww]arning[ \t].*)", 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compiler error"), cltError, "([ \tA-Za-z0-9_:\\-\\+/\\.]+)\\(([0-9]+)\\) :[ \t](.*[Ee]rror[ \t].*)", 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Linker warning"), cltWarning, "([ \tA-Za-z0-9_:\\-\\+/\\.\\(\\)]*)[ \t]+:[ \t]+(.*warning LNK[0-9]+.*)", 2, 1, 0));
    m_RegExes.Add(RegExStruct(_("Linker error"), cltError, "([ \tA-Za-z0-9_:\\-\\+/\\.\\(\\)]*)[ \t]+:[ \t]+(.*error LNK[0-9]+.*)", 2, 1, 0));

    m_CompilerOptions.Clear();
    m_LinkerOptions.Clear();
    m_LinkLibs.Clear();
    m_CmdsBefore.Clear();
    m_CmdsAfter.Clear();
}

AutoDetectResult CompilerMSVC::AutoDetectInstallationDir()
{
    wxString sep = wxFileName::GetPathSeparator();
#ifdef __WXMSW__
    wxLogNull ln;
    wxRegKey key; // defaults to HKCR
    key.SetName("HKEY_CURRENT_USER\\Environment");
    if (key.Open())
        // found; read it
        key.QueryValue("VCToolkitInstallDir", m_MasterPath);

    if (m_MasterPath.IsEmpty())
        // just a guess; the default installation dir
        m_MasterPath = "C:\\Program Files\\Microsoft Visual C++ Toolkit 2003";

    if (!m_MasterPath.IsEmpty())
    {
        AddIncludeDir(m_MasterPath + sep + "include");
        AddLibDir(m_MasterPath + sep + "lib");
    
        // add include dirs for MS Platform SDK too
        wxLogNull no_log_here;
        wxRegKey key; // defaults to HKCR
        key.SetName("HKEY_CURRENT_USER\\Software\\Microsoft\\Win32SDK\\Directories");
        if (key.Open())
        {
            wxString dir;
            key.QueryValue("Install Dir", dir);
            if (!dir.IsEmpty())
            {
                if (dir.GetChar(dir.Length() - 1) != '\\')
                    dir += sep;
                AddIncludeDir(dir + "include");
                AddLibDir(dir + "lib");
                m_ExtraPaths.Add(dir + "bin");
            }
        }
        
        // add extra paths for "Debugging tools" too
        key.SetName("HKEY_CURRENT_USER\\Software\\Microsoft\\DebuggingTools");
        if (key.Open())
        {
            wxString dir;
            key.QueryValue("WinDbg", dir);
            if (!dir.IsEmpty())
            {
                if (dir.GetChar(dir.Length() - 1) == '\\')
                    dir.Remove(dir.Length() - 1, 1);
                m_ExtraPaths.Add(dir);
            }
        }
    }
#else
    m_MasterPath="."; // doesn't matter under non-win32 platforms...
#endif

    return wxFileExists(m_MasterPath + sep + "bin" + sep + m_Programs.C) ? adrDetected : adrGuessed;
}
