#include "compilerDMC.h"
#include <wx/log.h>
#include <wx/intl.h>
#include <wx/regex.h>
#include <wx/config.h>

#ifdef __WXMSW__
    #include <wx/msw/registry.h>
#endif

CompilerDMC::CompilerDMC()
    : Compiler(_("Digital Mars Compiler"))
{
	m_Programs.C = "dmc.exe";
	m_Programs.CPP = "dmc.exe";
	m_Programs.LD = "link.exe";
	m_Programs.WINDRES = "rcc.exe";
	m_Programs.MAKE = "mingw32-make.exe";
	
	m_Switches.includeDirs = "-I";
	m_Switches.libDirs = "";
	m_Switches.linkLibs = "";
	m_Switches.defines = "-D";
	m_Switches.genericSwitch = "-";
	m_Switches.objectExtension = "obj";
	m_Switches.needDependencies = false;
	m_Switches.forceCompilerUseQuotes = false;
	m_Switches.forceLinkerUseQuotes = true;
	m_Switches.logging = clogSimple;
	m_Switches.buildMethod = cbmDirect;

	m_Options.AddOption(_("Produce debugging symbols"),
				"-g",
				_("Debugging"),
				"",
				true, 
				"-o -o+space", 
				_("You have optimizations enabled. This is Not A Good Thing(tm) when producing debugging symbols..."));

    //
    // TODO (hd#3#): should be simplified
    //

	//. m_Options.AddOption(_("Alignment of struct members"), "-a[1|2|4|8]", _("Architecture"));
	m_Options.AddOption(_("Enforce strict compliance with ANSI C/C++"), "-A", _("C++ Features"));
	m_Options.AddOption(_("Enable new[] and delete[] overloading"), "-Aa", _("C++ Features"));
	m_Options.AddOption(_("Enable bool"), "-Ab", _("C++ Features"));
	m_Options.AddOption(_("Enable exception handling"), "-Ae", _("C++ Features"));
	m_Options.AddOption(_("Enable RTTI"), "-Ar", _("C++ Features"));
	//. m_Options.AddOption(_("Message language: English,French,German,Japanese"), "-B[e|f|g|j]", _("Others"));
	//. m_Options.AddOption(_("Skip the link, do compile only"), "-c", _("Others"));
	m_Options.AddOption(_("Compile all source files as C++"), "-cpp", _("C++ Features"));
	m_Options.AddOption(_("Generate .cod (assemply) file"), "-cod", _("Others"));
	m_Options.AddOption(_("No inline function expansion"), "-C", _("Debugging"));
	m_Options.AddOption(_("Generate .dep (make dependency) file"), "-d", _("Others"));
	m_Options.AddOption(_("#define DEBUG 1"), "-D", _("Debugging"));
	m_Options.AddOption(_("Show results of preprocessor"), "-e", _("Others"));
	m_Options.AddOption(_("Do not elide comments"), "-EC", _("Others"));
	m_Options.AddOption(_("#line directives not output"), "-EL", _("Others"));
	m_Options.AddOption(_("IEEE 754 inline 8087 code"), "-f", _("Others"));
	m_Options.AddOption(_("Work around FDIV problem"), "-fd", _("Others"));
	m_Options.AddOption(_("Fast inline 8087 code"), "-ff", _("Optimization"));
	m_Options.AddOption(_("Disable debug info optimization"), "-gf", _("Optimization"));
	m_Options.AddOption(_("Make static functions global"), "-gg", _("Optimization"));
	m_Options.AddOption(_("Symbol info for globals"), "-gh", _("C++ Features"));
	m_Options.AddOption(_("Debug line numbers only"), "-gl", _("Debugging"));
	m_Options.AddOption(_("Generate pointer validations"), "-gp", _("Others"));
	m_Options.AddOption(_("Debug symbol info only"), "-gs", _("Debugging"));
	m_Options.AddOption(_("Generate trace prolog/epilog"), "-gt", _("Others"));
	//. m_Options.AddOption(_("Set data threshold to nnnn"), "-GTnnnn", _("Others"));
	m_Options.AddOption(_("Use precompiled headers (ph)"), "-H", _("Others"));
	//. m_Options.AddOption(_("Use ph from directory"), "-HDdirectory", _("Others"));
	//. m_Options.AddOption(_("Generate ph to filename"), "-HF[filename]", _("Others"));
	//. m_Options.AddOption(_("#include \"filename\""), "-HIfilename", _("Others"));
	m_Options.AddOption(_("Include files only once"), "-HO", _("Others"));
	m_Options.AddOption(_("Only search -I directories"), "-HS", _("Others"));
	//. m_Options.AddOption(_("#include file search path"), "-Ipath", _("Others"));
	//. m_Options.AddOption(_("Asian language characters (Japanese)"), "-j0", _("Others"));
	//. m_Options.AddOption(_("Asian language characters (Taiwanese/Chinese)"), "-j1", _("Others"));
	//. m_Options.AddOption(_("Asian language characters (Korean)"), "-j2", _("Others"));
	m_Options.AddOption(_("Relaxed type checking"), "-Jm", _("Others"));
	m_Options.AddOption(_("char==unsigned"), "-Ju", _("Others"));
	m_Options.AddOption(_("No empty base class optimization"), "-Jb", _("Optimization"));
	m_Options.AddOption(_("chars are unsigned"), "-J", _("Others"));
	//. m_Options.AddOption(_("Generate list file"), "-l[listfile]", _("Others"));
	//. m_Options.AddOption(_("Using non-Digital Mars linker"), "-L", _("Others"));
	//. m_Options.AddOption(_("Specify linker to use"), "-Llink", _("Others"));
	//. m_Options.AddOption(_("Pass /switch to linker"), "-L/switch", _("Others"));
	//. m_Options.AddOption(_("Specify assembler to use"), "-Masm", _("Others"));
	//. m_Options.AddOption(_("Pass /switch to assembler"), "-M/switch", _("Others"));
	//. m_Options.AddOption(_("Set memory model (-mn: Windows)"), "-m[tsmclvfnrpxz][do][w][u]", _("Architecture"));
	m_Options.AddOption(_("Perform function level linking"), "-Nc", _("Optimization"));
	m_Options.AddOption(_("No default library"), "-NL", _("Optimization"));
	m_Options.AddOption(_("Place expr strings in code seg"), "-Ns", _("Optimization"));
	m_Options.AddOption(_("New code seg for each function"), "-NS", _("Optimization"));
	//. m_Options.AddOption(_("Set code segment name"), "-NTname", _("Others"));
	m_Options.AddOption(_("vtables in far data"), "-NV", _("Others"));
	//. m_Options.AddOption(_("Run optimizer with flag"), "-o[-+flag]", _("Optimization"));
	m_Options.AddOption(_("Minimize space"), "-o+space", _("Optimization"));
	m_Options.AddOption(_("Maximize speed"), "-o", _("Optimization"));
	//. m_Options.AddOption(_("Output filename"), "-ooutput", _("Others"));
	m_Options.AddOption(_("Turn off function auto-prototyping"), "-p", _("Others"));
	m_Options.AddOption(_("Make Pascal linkage the default"), "-P", _("Linkage"));
	m_Options.AddOption(_("Make stdcall linkage the default"), "-Pz", _("Linkage"));
	m_Options.AddOption(_("Require strict function prototyping"), "-r", _("Others"));
	m_Options.AddOption(_("Put switch tables in code seg"), "-R", _("Others"));
	m_Options.AddOption(_("Stack overflow checking"), "-s", _("Others"));
	m_Options.AddOption(_("Always generate stack frame"), "-S", _("Others"));
	m_Options.AddOption(_("Suppress non-ANSI predefined macros"), "-u", _("C++ Features"));
	//. m_Options.AddOption(_("Verbose compile"), "-v[0|1|2]", _("Warnings"));
	m_Options.AddOption(_("Suppress all warning messages"), "-w", _("Warnings"));
	m_Options.AddOption(_("Warn on C style casts"), "-wc", _("Warnings"));
	//. m_Options.AddOption(_("Suppress warning number n"), "-wn", _("Warnings"));
	m_Options.AddOption(_("Treat warnings as errors"), "-wx", _("Warnings"));
	//. m_Options.AddOption(_("Windows prolog/epilog (-WA exe -WD dll)"), "-W{0123ADabdefmrstuvwx-+}", _("Architecture"));
	m_Options.AddOption(_("Turn off error maximum"), "-x", _("Warnings"));
	m_Options.AddOption(_("Instantiate templates"), "-XD", _("C++ Features"));
	//. m_Options.AddOption(_("Instantiate template class temp<type>"), "-XItemp<type>", _("C++ Features"));
	//. m_Options.AddOption(_("Instantiate template function func(type)"), "-XIfunc(type)", _("C++ Features"));
	//. m_Options.AddOption(_("8088/286/386/486/Pentium/P6 code"), "-[0|2|3|4|5|6]", _("Architecture"));
	m_Options.AddOption(_("Optimize for 80386"), "-3", _("Architecture"));
	m_Options.AddOption(_("Optimize for 80486"), "-4", _("Architecture"));
	m_Options.AddOption(_("Optimize for Pentium"), "-5", _("Architecture"));
	m_Options.AddOption(_("Optimize for Pentium Pro, Pentium II, Pentium III"), "-6", _("Architecture"));

    m_Commands[(int)ctCompileObjectCmd] = "$compiler -mn -c $options $includes -o$object $file";
    m_Commands[(int)ctCompileResourceCmd] = "$rescomp $file -o$resource_output -I$res_includes -32";
    // FIXME (hd#1#): resource_output should be seperated from link_objects
    // m_Commands[(int)ctLinkExeCmd] = "$linker /NOLOGO $link_objects, $exe_output, , $libs, , $resource_output $link_options";
    // NOTE (hd#1#): I can only compiled console programs with this
    m_Commands[(int)ctLinkExeCmd] = "$linker /NOLOGO /subsystem:windows $link_objects, $exe_output, , $link_options, , $link_resobjects";
    m_Commands[(int)ctLinkConsoleExeCmd] = "$linker /NOLOGO $link_objects, $exe_output, , $link_options";
    // FIXME (hd#3#): 
    m_Commands[(int)ctLinkDynamicCmd] = "$linker /NOLOGO $link_objects -WD $libdirs -o$exe_output $libs $link_options";
    // FIXME (hd#2#): 
    m_Commands[(int)ctLinkStaticCmd] = "lib $exe_output /C $link_options , $link_objects";
}

CompilerDMC::~CompilerDMC()
{
	//dtor
}

Compiler * CompilerDMC::CreateCopy()
{
    return new CompilerDMC(*this);
}

AutoDetectResult CompilerDMC::AutoDetectInstallationDir()
{
    // just a guess; the default installation dir
	m_MasterPath = "C:\\dm";
    wxString sep = wxFileName::GetPathSeparator();
    //
    // NOTE (hd#1#): dmc uses sc.ini for compiler's master directories
    //
    // if (!m_MasterPath.IsEmpty())
    // {
    //     m_IncludeDirs.Add(m_MasterPath + sep + "include");
    //     m_LibDirs.Add(m_MasterPath + sep + "lib");
    // }
    //

    return wxFileExists(m_MasterPath + sep + "bin" + sep + m_Programs.C) ? adrDetected : adrGuessed;
}

Compiler::CompilerLineType CompilerDMC::CheckForWarningsAndErrors(const wxString& line)
{
    Compiler::CompilerLineType ret = Compiler::cltNormal;
	if (line.IsEmpty())
        return ret;

    //
    // FIXME (hd#1#): should be implemented for dm
    //
    
    // quick regex's
    wxRegEx reError(": Error:");
    wxRegEx reWarning(": Warning:");
    wxRegEx reErrorLinker("([ \tA-Za-z0-9_:\\-\\+/\\.\\(\\)]*)[ \t]+:[ \t]+(.*error LNK[0-9]+.*)");
    wxRegEx reErrorLine("\\([0-9]+\\) :[ \t].*:");
    wxRegEx reDetailedErrorLine("([ \tA-Za-z0-9_:\\-\\+/\\.]+)\\(([0-9]+)\\) :[ \t](.*)");
    wxRegEx reFatalError("Fatal error:[ \t](.*)");

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
    else if (reFatalError.Matches(line))
    {
        m_ErrorFilename = "";
        m_ErrorLine = "";
        m_Error = reFatalError.GetMatch(line, 1);
        ret = Compiler::cltError;
    }
    return ret;
}
