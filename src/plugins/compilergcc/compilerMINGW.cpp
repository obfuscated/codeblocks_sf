#include "compilerMINGW.h"
#include <wx/intl.h>
#include <wx/regex.h>
#include <wx/config.h>
#include <wx/fileconf.h>
#include <wx/msgdlg.h>

CompilerMINGW::CompilerMINGW()
    : Compiler(_("MinGW Compiler Suite"))
{
#ifdef __WXMSW__
	m_Programs.C = "mingw32-gcc.exe";
	m_Programs.CPP = "mingw32-g++.exe";
	m_Programs.LD = "mingw32-g++.exe";
	m_Programs.WINDRES = "windres.exe";
	m_Programs.MAKE = "mingw32-make.exe";
#else
	m_Programs.C = "gcc";
	m_Programs.CPP = "g++";
	m_Programs.LD = "g++";
	m_Programs.WINDRES = "";
	m_Programs.MAKE = "make";
#endif
	m_Switches.includeDirs = "-I";
	m_Switches.libDirs = "-L";
	m_Switches.linkLibs = "-l";
	m_Switches.defines = "-D";
	m_Switches.genericSwitch = "-";
	m_Switches.linkerSwitchForGui = "-mwindows";
	m_Switches.objectExtension = "o";
	m_Switches.needDependencies = true;
	m_Switches.forceCompilerUseQuotes = false;
	m_Switches.forceLinkerUseQuotes = false;
	m_Switches.logging = clogSimple;
	m_Switches.buildMethod = cbmDirect;

	m_Options.AddOption(_("Produce debugging symbols"),
				"-g",
				_("Debugging"), 
				"",
				true, 
				"-O -O1 -O2 -O3 -Os", 
				_("You have optimizations enabled. This is Not A Good Thing(tm) when producing debugging symbols..."));
	m_Options.AddOption(_("Profile code when executed"), "-pg", _("Profiling"), "-pg -lgmon");
	m_Options.AddOption(_("Enable all compiler warnings"), "-Wall", _("Warnings"));
	m_Options.AddOption(_("Optimize generated code (for speed)"), "-O", _("Optimization"));
	m_Options.AddOption(_("Optimize more (for speed)"), "-O1", _("Optimization"));
	m_Options.AddOption(_("Optimize even more (for speed)"), "-O2", _("Optimization"));
	m_Options.AddOption(_("Optimize generated code (for size)"), "-Os", _("Optimization"));
	m_Options.AddOption(_("Expensive optimizations"), "-fexpensive-optimizations", _("Optimization"));

    m_Commands[(int)ctCompileObjectCmd] = "$compiler $options $includes -c $file -o $object";
    m_Commands[(int)ctGenDependenciesCmd] = "$compiler -MM $options -MF $dep_object -MT $object $includes $file";
    m_Commands[(int)ctCompileResourceCmd] = "$rescomp -i $file -J rc -o $resource_output -O coff $res_includes";
    m_Commands[(int)ctLinkExeCmd] = "$linker $libdirs -o $exe_output $libs $link_objects $link_options";
    m_Commands[(int)ctLinkDynamicCmd] = "$linker -shared -Wl,--output-def=$def_output -Wl,--out-implib=$static_output -Wl,--dll $libdirs $link_objects $libs -o $exe_output $link_options";
    m_Commands[(int)ctLinkStaticCmd] = "ar -r $exe_output $link_objects\n\tranlib $exe_output";
}

CompilerMINGW::~CompilerMINGW()
{
	//dtor
}

Compiler * CompilerMINGW::CreateCopy()
{
    return new CompilerMINGW(*this);
}

AutoDetectResult CompilerMINGW::AutoDetectInstallationDir()
{
#ifdef __WXMSW__
    // search for MinGW installation dir
    wxString windir = wxGetOSDirectory();
    wxFileConfig ini("", "", windir + "/MinGW.ini", "", wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_NO_ESCAPE_CHARACTERS);
	m_MasterPath = ini.Read("/InstallSettings/InstallPath", "C:\\MinGW");
#else
    m_MasterPath = "/usr";
#endif
    wxString sep = wxFileName::GetPathSeparator();
    if (!m_MasterPath.IsEmpty())
    {
        m_IncludeDirs.Add(m_MasterPath + sep + "include");
        m_LibDirs.Add(m_MasterPath + sep + "lib");
    }
    return wxFileExists(m_MasterPath + sep + "bin" + sep + m_Programs.C) ? adrDetected : adrGuessed;
}

Compiler::CompilerLineType CompilerMINGW::CheckForWarningsAndErrors(const wxString& line)
{
    Compiler::CompilerLineType ret = Compiler::cltNormal;
	if (line.IsEmpty())
        return ret;

    // quick regex's
    wxRegEx reWarning("[ \t]warning:[ \t]");
    wxRegEx reFatalErrorLine("FATAL:[ \t]*(.*)");
    wxRegEx reErrorLine(".*:[0-9]+:.*");
    wxRegEx reDetailedErrorLine("([ \tA-Za-z0-9_:\\-\\+/\\.]+):([0-9]+):[ \t](.*)");
    wxRegEx reDetailedPreProcErrorLine("([ \tA-Za-z0-9_:\\-\\+/\\.]+):([0-9]+):[0-9]+:[ \t](.*)");
    wxRegEx reDetailedLinkerErrorLine("([ \tA-Za-z0-9_\\-\\+/\\.]+):[ \t](undefined reference.*)"); // this doesn't expect driver letter...

    if (reErrorLine.Matches(line))
    {
        // one more check to see it is an actual error line
        if (reDetailedErrorLine.Matches(line))
        {
            if (reWarning.Matches(line))
                ret = Compiler::cltWarning;
            else
                ret = Compiler::cltError;
            wxRegEx* actual = 0;
            if (reDetailedPreProcErrorLine.Matches(line))
                actual = &reDetailedPreProcErrorLine;
            else
                actual = &reDetailedErrorLine;
            m_ErrorFilename = actual->GetMatch(line, 1);
            m_ErrorLine = actual->GetMatch(line, 2);
            m_Error = actual->GetMatch(line, 3);
        }
    }
    else if (reFatalErrorLine.Matches(line))
    {
        ret = Compiler::cltError;
        m_ErrorFilename = "";
        m_ErrorLine = "";
        m_Error = reFatalErrorLine.GetMatch(line, 1);
    }
    else if (reDetailedLinkerErrorLine.Matches(line))
    {
        ret = Compiler::cltError;
        m_ErrorFilename = reDetailedLinkerErrorLine.GetMatch(line, 1);
        m_ErrorLine = "";
        m_Error = reDetailedLinkerErrorLine.GetMatch(line, 2);
    }
    return ret;
}
