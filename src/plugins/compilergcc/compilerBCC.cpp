#include "compilerBCC.h"
#include <wx/intl.h>
#include <wx/regex.h>

CompilerBCC::CompilerBCC()
    : Compiler(_("Borland C++ Compiler 5.5"))
{
	m_Programs.C = "bcc32.exe";
	m_Programs.CPP = "bcc32.exe";
	m_Programs.LD = "ilink32.exe";
	m_Programs.WINDRES = "brcc32.exe"; // platform SDK is needed for this
	m_Programs.MAKE = "mingw32-make.exe";
	
	m_Switches.includeDirs = "-I";
	m_Switches.libDirs = "-L";
	m_Switches.linkLibs = "";
	m_Switches.defines = "-D";
	m_Switches.genericSwitch = "-";
	m_Switches.linkerSwitchForGui = "";
	m_Switches.objectExtension = "obj";
	m_Switches.needDependencies = false;
	m_Switches.forceCompilerUseQuotes = false;
	m_Switches.forceLinkerUseQuotes = true;
	m_Switches.logging = clogSimple;
	m_Switches.buildMethod = cbmDirect;

//	m_Options.AddOption(_("Enable all compiler warnings"), "/Wall", _("Warnings"));
	m_Options.AddOption(_("Optimizations level 1"), "-O1", _("Optimization"));
	m_Options.AddOption(_("Optimizations level 2"), "-O2", _("Optimization"));
	m_Options.AddOption(_("Optimizations level 3"), "-O3", _("Optimization"));
	m_Options.AddOption(_("Disable C++ RTTI"), "-RT-", _("C++ Features"));
//	m_Options.AddOption(_("Enable C++ exception handling"), "/GX", _("C++ Features"));
	m_Options.AddOption(_("Optimize for 80386"), "-3", _("Architecture"));
	m_Options.AddOption(_("Optimize for 80486"), "-4", _("Architecture"));
	m_Options.AddOption(_("Optimize for Pentium"), "-5", _("Architecture"));
	m_Options.AddOption(_("Optimize for Pentium Pro, Pentium II, Pentium III"), "-6", _("Architecture"));

    m_Commands[(int)ctCompileObjectCmd] = "$compiler $options $includes -o$object -c $file";
    m_Commands[(int)ctCompileResourceCmd] = "$rescomp -32 -fo$resource_output $res_includes $file";
    m_Commands[(int)ctLinkExeCmd] = "$linker -aa  $libs $libdirs c0w32 $link_objects,$exe_output,,$link_options";
    m_Commands[(int)ctLinkDynamicCmd] = "$linker $libdirs -o $exe_output $libs $link_objects $link_options";
    m_Commands[(int)ctLinkStaticCmd] = "$linker $libdirs -o $exe_output $libs $link_objects $link_options";
}

CompilerBCC::~CompilerBCC()
{
	//dtor
}

Compiler * CompilerBCC::CreateCopy()
{
    return new CompilerBCC(*this);
}

AutoDetectResult CompilerBCC::AutoDetectInstallationDir()
{
    // just a guess; the default installation dir
	m_MasterPath = "C:\\Borland\\BCC55";
    wxString sep = wxFileName::GetPathSeparator();
    if (!m_MasterPath.IsEmpty())
    {
        m_IncludeDirs.Add(m_MasterPath + sep + "include");
        m_LibDirs.Add(m_MasterPath + sep + "lib");
    }

    return wxFileExists(m_MasterPath + sep + "bin" + sep + m_Programs.C) ? adrDetected : adrGuessed;
}

Compiler::CompilerLineType CompilerBCC::CheckForWarningsAndErrors(const wxString& line)
{
    Compiler::CompilerLineType ret = Compiler::cltNormal;
	if (line.IsEmpty())
        return ret;

    // quick regex's
    wxRegEx reFatalError("Fatal:[ \t]+(.*)");
    wxRegEx reError("^Error[ \t]+.*");
    wxRegEx reWarning("^Warning[ \t]+.*");
    wxRegEx reErrorLine("[ \t]+[WE]+[0-9]+.*");
    wxRegEx reDetailedWarningLine("(^Warning[ \t]W[0-9]+)[ \t]([A-Za-z0-9_:/\\.]+)[ \t]([0-9]+)(:[ \t].*)");
    wxRegEx reDetailedErrorLine("(^Error[ \t]E[0-9]+)[ \t]([A-Za-z0-9_:/\\.]+)[ \t]([0-9]+)(:[ \t].*)");
    wxRegEx reDetailedLinkerErrorLine("^Error[ \t]+E[0-9]+:.*");

    if (reErrorLine.Matches(line))
    {
        wxRegEx* actual = 0;
        if (reDetailedErrorLine.Matches(line))
            actual = &reDetailedErrorLine;
        else if (reDetailedWarningLine.Matches(line))
            actual = &reDetailedWarningLine;
        if (actual)
        {
            if (reError.Matches(line))
                ret = Compiler::cltError;
            else if (reWarning.Matches(line))
                ret = Compiler::cltWarning;
            wxArrayString errors;
            m_ErrorFilename = actual->GetMatch(line, 2);
            m_ErrorLine = actual->GetMatch(line, 3);
            m_Error = actual->GetMatch(line, 1) + actual->GetMatch(line, 4);
        }
        else if (reDetailedLinkerErrorLine.Matches(line))
        {
            // linker error
            m_ErrorFilename = "";
            m_ErrorLine = "";
            m_Error = line;
            ret = Compiler::cltError;
        }
    }
    else if (reFatalError.Matches(line))
    {
        // linker error
        m_ErrorFilename = "";
        m_ErrorLine = "";
        m_Error = reFatalError.GetMatch(line, 1);
        ret = Compiler::cltError;
    }

    return ret;
}
