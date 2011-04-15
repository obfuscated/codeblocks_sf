/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#include <prep.h>
#include "compilerLDC.h"
#include <wx/intl.h>
#include <wx/regex.h>
#include <wx/config.h>
#include <wx/fileconf.h>
#include <wx/msgdlg.h>
#include "manager.h"
#include "logmanager.h"

#include <configmanager.h>

CompilerLDC::CompilerLDC()
    : Compiler(_("LLVM D Compiler"), _T("ldc"))
{
    Reset();
}

CompilerLDC::~CompilerLDC()
{
    //dtor
}

Compiler * CompilerLDC::CreateCopy()
{
    Compiler* c = new CompilerLDC(*this);
    c->SetExtraPaths(m_ExtraPaths); // wxArrayString doesn't seem to be copied with the default copy ctor...
    return c;
}

void CompilerLDC::Reset()
{
    m_Programs.C = _T("ldc");
    m_Programs.CPP = _T("ldc");
    m_Programs.LD = _T("ldc");
    m_Programs.DBG = _T("gdb");
    m_Programs.LIB = _T("ar");
    m_Programs.WINDRES = _T("");
    m_Programs.MAKE = _T("make");

    m_Switches.includeDirs = _T("-I");
    m_Switches.libDirs = _T("-L-L");
    m_Switches.linkLibs = _T("-L-l");
    m_Switches.defines = _T("-d-version=");
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

    m_Options.ClearOptions();
    m_Options.AddOption(_("Produce debugging symbols"),
                _T("-g"),
                _("Debugging"),
                _T(""),
                true,
                _T("-O1 -O2 -O3"),
                _("You have optimizations enabled. This is Not A Good Thing(tm) when producing debugging symbols..."));

    wxString category = _("Warnings");

    // warnings
    m_Options.AddOption(_("Enable warnings"), _T("-w"), category);
    // D features
    category = _("D");
    m_Options.AddOption(_("allow deprecated features"), _T("-d"), category);
    m_Options.AddOption(_("inline expand functions"), _T("-enable-inlining"), category);
    m_Options.AddOption(_("enable array bounds checks"), _T("-enable-boundscheck"), category);
    m_Options.AddOption(_("enable contracts"), _T("-enable-contracts"), category);
    m_Options.AddOption(_("enable assertions"), _T("-enable-asserts"), category);
    m_Options.AddOption(_("enable invariants"), _T("-enable-invariants"), category);
 
    // optimization
    category = _("Optimization");
    m_Options.AddOption(_("No optimization"), _T("-O0"), category);
    m_Options.AddOption(_("Simple optimizations"), _T("-O1"), category);
    m_Options.AddOption(_("Good optimizations"), _T("-O2"), category);
    m_Options.AddOption(_("Aggressive optimizations"), _T("-O3"), category);
    // machine dependent options - cpu arch
    category = _("CPU architecture tuning (choose none, or only one of these)");
    m_Options.AddOption(_("32-bit X86: Pentium-Pro and above"), _T("-march=x86"), category);
    m_Options.AddOption(_("64-bit X86: EM64T and AMD64"), _T("-march=x86-64"), category);

    m_Commands[(int)ctCompileObjectCmd].push_back(CompilerTool(_T("$compiler $options $includes -c $file -of=$object")));
    m_Commands[(int)ctGenDependenciesCmd].push_back(CompilerTool(_T("$compiler $options -deps=$dep_object $includes $file")));
    m_Commands[(int)ctCompileResourceCmd].push_back(CompilerTool(_T("$rescomp -i $file -J rc -o $resource_output -O coff $res_includes")));
    m_Commands[(int)ctLinkConsoleExeCmd].push_back(CompilerTool(_T("$linker $libdirs -of=$exe_output $link_objects $link_resobjects $link_options $libs")));
    m_Commands[(int)ctLinkExeCmd] = m_Commands[(int)ctLinkConsoleExeCmd]; // no -mwindows
    m_Commands[(int)ctLinkDynamicCmd].push_back(CompilerTool(_T("$linker -shared $libdirs $link_objects $link_resobjects -o $exe_output $link_options $libs")));
    m_Commands[(int)ctLinkStaticCmd].push_back(CompilerTool(_T("$lib_linker -r $static_output $link_objects\nranlib $static_output")));
    m_Commands[(int)ctLinkNativeCmd] = m_Commands[(int)ctLinkConsoleExeCmd]; // unsupported currently

    LoadDefaultRegExArray();

    m_CompilerOptions.Clear();
    m_LinkerOptions.Clear();
    m_LinkLibs.Clear();
    m_CmdsBefore.Clear();
    m_CmdsAfter.Clear();
}

void CompilerLDC::LoadDefaultRegExArray()
{
    m_RegExes.Clear();
    m_RegExes.Add(RegExStruct(_("Fatal error"), cltError, _T("FATAL:[ \t]*(.*)"), 1));
    m_RegExes.Add(RegExStruct(_("'Instantiated from here' info"), cltNormal, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[ \t]+([iI]nstantiated from here.*)"), 3, 1, 2));    m_RegExes.Add(RegExStruct(_("Resource compiler error"), cltError, _T("windres.exe:[ \t](") + FilePathWithSpaces + _T("):([0-9]+):[ \t](.*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Resource compiler error"), cltError, _T("windres.exe:[ \t](") + FilePathWithSpaces + _T("):([0-9]+):[ \t](.*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Resource compiler error (2)"), cltError, _T("windres.exe:[ \t](.*)"), 1));
    m_RegExes.Add(RegExStruct(_("Compiler warning"), cltWarning, _T("(") + FilePathWithSpaces + _T(")\\(([0-9]+)\\):[ \t]([Ww]arning:[ \t].*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compiler error"), cltError, _T("(") + FilePathWithSpaces + _T(")\\(([0-9]+)\\):[ \t](.*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Linker error"), cltError, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[0-9]+:[ \t](.*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Linker error (2)"), cltError, FilePathWithSpaces + _T("\\(.text\\+[0-9A-Za-z]+\\):([ \tA-Za-z0-9_:+/\\.-]+):[ \t](.*)"), 2, 1));
    m_RegExes.Add(RegExStruct(_("Linker error (lib not found)"), cltError, _T(".*(ld.*):[ \t](cannot find.*)"), 2, 1));
    m_RegExes.Add(RegExStruct(_("Undefined reference"), cltError, _T("(") + FilePathWithSpaces + _T("):[ \t](undefined reference.*)"), 2, 1));
    m_RegExes.Add(RegExStruct(_("General warning"), cltWarning, _T("([Ww]arning:[ \t].*)"), 1));
}

AutoDetectResult CompilerLDC::AutoDetectInstallationDir()
{
    wxString sep = wxFileName::GetPathSeparator();
    m_MasterPath = wxFileExists(_T("/usr/local/bin/ldc")) ? _T("/usr/local") : _T("/usr");
    
    AddIncludeDir(m_MasterPath + sep + _T("import"));
    AddLibDir(m_MasterPath + sep + _T("lib"));

    return wxFileExists(m_MasterPath + sep + _T("bin") + sep + m_Programs.C) ? adrDetected : adrGuessed;
}
