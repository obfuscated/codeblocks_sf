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
	m_Programs.C = "cl.exe";
	m_Programs.CPP = "cl.exe";
	m_Programs.LD = "link.exe";
	m_Programs.WINDRES = "rc.exe"; // platform SDK is needed for this
	m_Programs.MAKE = "mingw32-make.exe";
	
	m_Switches.includeDirs = "/I";
	m_Switches.libDirs = "/LIBPATH:";
	m_Switches.linkLibs = "";
	m_Switches.defines = "/D";
	m_Switches.genericSwitch = "/";
	m_Switches.linkerSwitchForGui = "/subsystem:windows";
	m_Switches.objectExtension = "obj";
	m_Switches.needDependencies = false;
	m_Switches.forceCompilerUseQuotes = false;
	m_Switches.forceLinkerUseQuotes = false;
	m_Switches.logging = clogNone;
	m_Switches.buildMethod = cbmDirect;

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
    m_Commands[(int)ctLinkExeCmd] = "$linker /nologo $libdirs /out:$exe_output $libs $link_objects $link_options";
    m_Commands[(int)ctLinkDynamicCmd] = "$linker /dll /nologo $libdirs /out:$exe_output $libs $link_objects $link_options";
    m_Commands[(int)ctLinkStaticCmd] = "$linker /lib /nologo $libdirs /out:$exe_output $libs $link_objects $link_options";
}

CompilerMSVC::~CompilerMSVC()
{
	//dtor
}

Compiler * CompilerMSVC::CreateCopy()
{
    return new CompilerMSVC(*this);
}

AutoDetectResult CompilerMSVC::AutoDetectInstallationDir()
{
    // just a guess; the default installation dir
	m_MasterPath = "C:\\Program Files\\Microsoft Visual C++ Toolkit 2003";
    wxString sep = wxFileName::GetPathSeparator();
    if (!m_MasterPath.IsEmpty())
    {
        m_IncludeDirs.Add(m_MasterPath + sep + "include");
        m_LibDirs.Add(m_MasterPath + sep + "lib");
    
        // add include dirs for MS Platform SDK too
#ifdef __WXMSW__
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
                m_IncludeDirs.Add(dir + "include");
                m_LibDirs.Add(dir + "lib");
            }
        }
#endif
    }

    return wxFileExists(m_MasterPath + sep + "bin" + sep + m_Programs.C) ? adrDetected : adrGuessed;
}

Compiler::CompilerLineType CompilerMSVC::CheckForWarningsAndErrors(const wxString& line)
{
    Compiler::CompilerLineType ret = Compiler::cltNormal;
	if (line.IsEmpty())
        return ret;

    // quick regex's
    wxRegEx reError(": error ");
    wxRegEx reWarning(": warning ");
    wxRegEx reErrorLinker("([ \tA-Za-z0-9_:\\-\\+/\\.\\(\\)]*)[ \t]+:[ \t]+(.*error LNK[0-9]+.*)");
    wxRegEx reErrorLine("\\([0-9]+\\) :[ \t].*:");
    wxRegEx reDetailedErrorLine("([ \tA-Za-z0-9_:\\-\\+/\\.]+)\\(([0-9]+)\\) :[ \t](.*)");

    if (reErrorLine.Matches(line))
    {
        // one more check to see it is an actual error line
        if (reDetailedErrorLine.Matches(line))
        {
            if (reError.Matches(line))
                ret = Compiler::cltError;
            else if (reWarning.Matches(line))
                ret = Compiler::cltWarning;
            m_ErrorFilename = reDetailedErrorLine.GetMatch(line, 1);
            m_ErrorLine = reDetailedErrorLine.GetMatch(line, 2);
            m_Error = reDetailedErrorLine.GetMatch(line, 3);
        }
    }
    else if (reErrorLinker.Matches(line))
    {
        m_ErrorFilename = reErrorLinker.GetMatch(line, 1);
        m_ErrorLine = "";
        m_Error = reErrorLinker.GetMatch(line, 2);
        ret = Compiler::cltError;
    }
    return ret;
}
