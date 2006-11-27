#ifdef __WXMSW__
// this compiler is valid only in windows

#include <sdk.h>
#include "compilerMSVC.h"
#include <wx/wx.h>
#include <wx/log.h>
#include <wx/intl.h>
#include <wx/regex.h>
#include <wx/config.h>

#ifdef __WXMSW__
    #include <wx/msw/registry.h>
#endif

CompilerMSVC::CompilerMSVC()
    : Compiler(_("Microsoft Visual C++ Toolkit 2003"), _T("msvctk"))
{
    Reset();
}

CompilerMSVC::~CompilerMSVC()
{
	//dtor
}

Compiler * CompilerMSVC::CreateCopy()
{
    Compiler* c = new CompilerMSVC(*this);
    c->SetExtraPaths(m_ExtraPaths); // wxArrayString doesn't seem to be copied with the default copy ctor...
    return c;
}

void CompilerMSVC::Reset()
{
	m_Programs.C = _T("cl.exe");
	m_Programs.CPP = _T("cl.exe");
	m_Programs.LD = _T("link.exe");
	m_Programs.LIB = _T("link.exe");
	m_Programs.WINDRES = _T("rc.exe"); // platform SDK is needed for this
	m_Programs.MAKE = _T("mingw32-make.exe");
	m_Programs.DBG = _T("cdb.exe");

	m_Switches.includeDirs = _T("/I");
	m_Switches.libDirs = _T("/LIBPATH:");
	m_Switches.linkLibs = _T("");
	m_Switches.defines = _T("/D");
	m_Switches.genericSwitch = _T("/");
	m_Switches.objectExtension = _T("obj");
	m_Switches.needDependencies = false;
	m_Switches.forceCompilerUseQuotes = false;
	m_Switches.forceLinkerUseQuotes = false;
	m_Switches.logging = clogNone;
	m_Switches.buildMethod = cbmDirect;
	m_Switches.libPrefix = _T("");
	m_Switches.libExtension = _T("lib");
	m_Switches.linkerNeedsLibPrefix = false;
	m_Switches.linkerNeedsLibExtension = true;

    m_Options.ClearOptions();
	m_Options.AddOption(_("Produce debugging symbols"),
				_T("/Zi /D_DEBUG"),
				_("Debugging"),
				_T("/DEBUG"),
				true,
				_T("/Og /O1 /O2 /Os /Ot /Ox /NDEBUG"),
				_("You have optimizations enabled. This is Not A Good Thing(tm) when producing debugging symbols..."));
	m_Options.AddOption(_("Enable all compiler warnings"), _T("/Wall"), _("Warnings"));
	m_Options.AddOption(_("Enable warnings level 1"), _T("/W1"), _("Warnings"));
	m_Options.AddOption(_("Enable warnings level 2"), _T("/W2"), _("Warnings"));
	m_Options.AddOption(_("Enable warnings level 3"), _T("/W3"), _("Warnings"));
	m_Options.AddOption(_("Enable warnings level 4"), _T("/W4"), _("Warnings"));
	m_Options.AddOption(_("Enable 64bit porting warnings"), _T("/Wp64"), _("Warnings"));
	m_Options.AddOption(_("Treat warnings as errors"), _T("/WX"), _("Warnings"));
	m_Options.AddOption(_("Enable global optimization"), _T("/Og"), _("Optimization"));
	m_Options.AddOption(_("Maximum optimization (no need for other options)"), _T("/Ox"), _("Optimization"));
	m_Options.AddOption(_("Disable optimizations"), _T("/Od"), _("Optimization")); //added no optimization
	m_Options.AddOption(_("Minimize space"), _T("/O1"), _("Optimization"));
	m_Options.AddOption(_("Maximize speed"), _T("/O2"), _("Optimization"));
	m_Options.AddOption(_("Favor code space"), _T("/Os"), _("Optimization"));
	m_Options.AddOption(_("Favor code speed"), _T("/Ot"), _("Optimization"));
	m_Options.AddOption(_("Enable C++ RTTI"), _T("/GR"), _("C++ Features"));
	m_Options.AddOption(_("Enable C++ exception handling"), _T("/GX"), _("C++ Features"));
	m_Options.AddOption(_("Optimize for 80386"), _T("/G3"), _("Architecture"));
	m_Options.AddOption(_("Optimize for 80486"), _T("/G4"), _("Architecture"));
	m_Options.AddOption(_("Optimize for Pentium"), _T("/G5"), _("Architecture"));
	m_Options.AddOption(_("Optimize for Pentium Pro, Pentium II, Pentium III"), _T("/G6"), _("Architecture"));
	m_Options.AddOption(_("Optimize for Pentium 4 or Athlon"), _T("/G7"), _("Architecture"));
	m_Options.AddOption(_("Enable SSE instruction set"), _T("/arch:SSE"), _("Architecture"));
	m_Options.AddOption(_("Enable SSE2 instruction set"), _T("/arch:SSE2"), _("Architecture"));
	m_Options.AddOption(_("Enable minimal rebuild"), _T("/Gm"), _("Others"));
	m_Options.AddOption(_("Enable link-time code generation"), _T("/GL"), _("Others"), _T(""), true, _T("/Zi /ZI"), _("Link-time code generation is incompatible with debugging info"));
	m_Options.AddOption(_("Optimize for windows application"), _T("/GA"), _("Others"));
	m_Options.AddOption(_("__cdecl calling convention"), _T("/Gd"), _("Others"));
	m_Options.AddOption(_("__fastcall calling convention"), _T("/Gr"), _("Others"));
	m_Options.AddOption(_("__stdcall calling convention"), _T("/Gz"), _("Others"));
    // Added Runtime options for cl.exe, that is the runtime library selection
    m_Options.AddOption(_("Single-threaded Runtime Library"), _T("/ML"), _("Runtime"));
    m_Options.AddOption(_("Single-threaded Debug Runtime Library"), _T("/MLd"), _("Runtime"));
    m_Options.AddOption(_("Multi-threaded Runtime Library"), _T("/MT"), _("Runtime"), _T(""), true);
    m_Options.AddOption(_("Multi-threaded Debug Runtime Library"), _T("/MTd"), _("Runtime"));
    m_Options.AddOption(_("Multi-threaded DLL Runtime Library"), _T("/MD"), _("Runtime"));
    m_Options.AddOption(_("Multi-threaded DLL Debug Runtime Library"), _T("/MDd"), _("Runtime"));


    m_Commands[(int)ctCompileObjectCmd] = _T("$compiler /nologo $options $includes /c $file /Fo$object");
    m_Commands[(int)ctCompileResourceCmd] = _T("$rescomp $res_includes -fo$resource_output $file");
    m_Commands[(int)ctLinkExeCmd] = _T("$linker /nologo /subsystem:windows $libdirs /out:$exe_output $libs $link_objects $link_resobjects $link_options");
    m_Commands[(int)ctLinkConsoleExeCmd] = _T("$linker /nologo $libdirs /out:$exe_output $libs $link_objects $link_resobjects $link_options");
    m_Commands[(int)ctLinkDynamicCmd] = _T("$linker /dll /nologo $libdirs /out:$exe_output $libs $link_objects $link_resobjects $link_options");
    m_Commands[(int)ctLinkStaticCmd] = _T("$lib_linker /lib /nologo $libdirs /out:$static_output $libs $link_objects $link_resobjects $link_options");
    m_Commands[(int)ctLinkNativeCmd] = _T("$linker /nologo /subsystem:native $libdirs /out:$exe_output $libs $link_objects $link_resobjects $link_options");

    LoadDefaultRegExArray();

    m_CompilerOptions.Clear();
    m_LinkerOptions.Clear();
    m_LinkLibs.Clear();
    m_CmdsBefore.Clear();
    m_CmdsAfter.Clear();
}

void CompilerMSVC::LoadDefaultRegExArray()
{
    m_RegExes.Clear();
    m_RegExes.Add(RegExStruct(_("Compiler warning"), cltWarning, _T("([ \tA-Za-z0-9_:\\-\\+/\\.-]+)\\(([0-9]+)\\) :[ \t]([Ww]arning[ \t].*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compiler error"), cltError, _T("([ \tA-Za-z0-9_:\\-\\+/\\.-]+)\\(([0-9]+)\\) :[ \t](.*[Ee]rror[ \t].*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Linker warning"), cltWarning, _T("([ \tA-Za-z0-9_:\\-\\+/\\.\\(\\)-]*)[ \t]+:[ \t]+(.*warning LNK[0-9]+.*)"), 2, 1, 0));
    m_RegExes.Add(RegExStruct(_("Linker error"), cltError, _T("([ \tA-Za-z0-9_:\\-\\+/\\.\\(\\)-]*)[ \t]+:[ \t]+(.*error LNK[0-9]+.*)"), 2, 1, 0));
}

AutoDetectResult CompilerMSVC::AutoDetectInstallationDir()
{
    wxString sep = wxFileName::GetPathSeparator();
#ifdef __WXMSW__
    wxLogNull ln;

    // Read the VCToolkitInstallDir environment variable
    wxGetEnv(_T("VCToolkitInstallDir"), &m_MasterPath);

    if (m_MasterPath.IsEmpty())
    {
        // just a guess; the default installation dir
        wxString Programs = _T("C:\\Program Files");
        // what's the "Program Files" location
        // TO DO : support 64 bit ->    32 bit apps are in "ProgramFiles(x86)"
        //                              64 bit apps are in "ProgramFiles"
        wxGetEnv(_T("ProgramFiles"), &Programs);
        m_MasterPath = Programs + _T("\\Microsoft Visual C++ Toolkit 2003");
    }
    if (!m_MasterPath.IsEmpty())
    {
        AddIncludeDir(m_MasterPath + sep + _T("include"));
        AddLibDir(m_MasterPath + sep + _T("lib"));

        // add include dirs for MS Platform SDK too
        wxLogNull no_log_here;
        wxRegKey key; // defaults to HKCR
        key.SetName(_T("HKEY_CURRENT_USER\\Software\\Microsoft\\Win32SDK\\Directories"));
        if (key.Open())
        {
            wxString dir;
            key.QueryValue(_T("Install Dir"), dir);
            if (!dir.IsEmpty())
            {
                if (dir.GetChar(dir.Length() - 1) != '\\')
                    dir += sep;
                AddIncludeDir(dir + _T("include"));
                AddLibDir(dir + _T("lib"));
                m_ExtraPaths.Add(dir + _T("bin"));
            }
        }

        // add extra paths for "Debugging tools" too
        key.SetName(_T("HKEY_CURRENT_USER\\Software\\Microsoft\\DebuggingTools"));
        if (key.Open())
        {
            wxString dir;
            key.QueryValue(_T("WinDbg"), dir);
            if (!dir.IsEmpty())
            {
                if (dir.GetChar(dir.Length() - 1) == '\\')
                    dir.Remove(dir.Length() - 1, 1);
                m_ExtraPaths.Add(dir);
            }
        }
    }
#else
    m_MasterPath=_T("."); // doesn't matter under non-win32 platforms...
#endif

    return wxFileExists(m_MasterPath + sep + _T("bin") + sep + m_Programs.C) ? adrDetected : adrGuessed;
}

#endif // __WXMSW__
