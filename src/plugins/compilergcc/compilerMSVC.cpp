#include "compilerMSVC.h"
#include <wx/intl.h>
#include <wx/regex.h>

CompilerMSVC::CompilerMSVC()
    : Compiler(_("Microsoft Visual C++ Toolkit 2003"))
{
	m_MasterPath = "C:\\Program Files\\Microsoft Visual C++ Toolkit 2003";
	
	m_Programs.C = "cl.exe";
	m_Programs.CPP = "cl.exe";
	m_Programs.LD = "link.exe";
	m_Programs.WINDRES = "rc.exe"; // platform SDK is needed for this
	m_Programs.MAKE = "mingw32-make.exe";
	
	// add default dirs
	m_IncludeDirs.Add("C:\\Program Files\\Microsoft Visual C++ Toolkit 2003\\include");
	m_IncludeDirs.Add("C:\\Program Files\\Microsoft SDK\\include");
	m_LibDirs.Add("C:\\Program Files\\Microsoft Visual C++ Toolkit 2003\\lib");
	m_LibDirs.Add("C:\\Program Files\\Microsoft SDK\\lib");
	
	m_Switches.includeDirs = "/I";
	m_Switches.libDirs = "/LIBPATH:";
	m_Switches.linkLibs = "";
	m_Switches.defines = "/D";
	m_Switches.genericSwitch = "/";
	m_Switches.linkerSwitchForGui = "/subsystem:windows";
	m_Switches.objectExtension = "obj";
	m_Switches.needDependencies = false;
	
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
    m_Commands[(int)ctCompileResourceCmd] = "$rescomp -i $file -J rc -o $resource_output -O coff $res_includes";
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

Compiler::CompilerLineType CompilerMSVC::CheckForWarningsAndErrors(const wxString& line)
{
    Compiler::CompilerLineType ret = Compiler::cltNormal;
	if (line.IsEmpty())
        return ret;

    // quick regex's
    wxRegEx reError(": error ");
    wxRegEx reWarning(": warning ");
    wxRegEx reErrorLine("\\([0-9]+\\) :[ \t].*:");
    wxRegEx reDetailedErrorLine("([A-Za-z0-9_:/\\.]*)\\(([0-9]+)\\) :[ \t](.*)");

    if (reErrorLine.Matches(line))
    {
        // one more check to see it is an actual error line
        if (reDetailedErrorLine.Matches(line))
        {
            if (reError.Matches(line))
                ret = Compiler::cltError;
            else if (reWarning.Matches(line))
                ret = Compiler::cltWarning;
            wxArrayString errors;
            m_ErrorFilename = reDetailedErrorLine.GetMatch(line, 1);
            m_ErrorLine = reDetailedErrorLine.GetMatch(line, 2);
            m_Error = reDetailedErrorLine.GetMatch(line, 3);
        }
    }
    return ret;
}
