/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is distributed under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
*
* $Revision: 3167 $
* $Id: compilerTcc.cpp 3167 2006-11-02 09:07:52Z killerbot $
* $HeadURL: svn+ssh://killerbot@svn.berlios.de/svnroot/repos/codeblocks/trunk/src/plugins/compilergcc/compilerTcc.cpp $
*/

#include "sdk.h"
#ifndef CB_PRECOMP
#include <wx/intl.h>
#include <wx/log.h>
#include <wx/regex.h>
#include <wx/string.h>
#endif
#include <wx/filefn.h> // wxFileExists
#include "compilerTcc.h"

CompilerTcc::CompilerTcc()
    : Compiler(_("Tiny C Compiler"), _T("tcc"))
{
    Reset();
}

CompilerTcc::~CompilerTcc()
{
	//dtor
}

Compiler * CompilerTcc::CreateCopy()
{
    Compiler* c = new CompilerTcc(*this);
    c->SetExtraPaths(m_ExtraPaths); // wxArrayString doesn't seem to be copied with the default copy ctor...
    return c;
} // end of CreateCopy

void CompilerTcc::Reset()
{
#ifdef __WXMSW__
	m_Programs.C = _T("tcc.exe");
	m_Programs.CPP = _T("");
	m_Programs.LD = _T("tcc.exe");
	m_Programs.DBG = _T("gdb.exe");
	m_Programs.LIB = _T("tcc.exe");
	m_Programs.WINDRES = _T("windres.exe");
	m_Programs.MAKE = _T("mingw32-make.exe");
#else
	m_Programs.C = _T("tcc");
	m_Programs.CPP = _T("");
	m_Programs.LD = _T("tcc");
	m_Programs.DBG = _T("gdb");
	m_Programs.LIB = _T("tcc");
	m_Programs.WINDRES = _T("");
	m_Programs.MAKE = _T("make");
#endif
	m_Switches.includeDirs = _T("-I");
	m_Switches.libDirs = _T("-L");
	m_Switches.linkLibs = _T("-l");
	m_Switches.defines = _T("-D");
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
	m_Switches.buildMethod = cbmDirect;
    m_Switches.supportsPCH = false;
    m_Switches.PCHExtension = _T("");

    // Summary of tcc options

    m_Options.ClearOptions();
	m_Options.AddOption(_("Produce debugging symbols"),
				_T("-g"),
				_("Debugging"));

    wxString category = _("Warnings");

    // warnings
	m_Options.AddOption(_("Enable all compiler warnings (overrides every other setting)"), _T("-Wall"), category);
	m_Options.AddOption(_("Disable all warnings"), _T("-w"), category);
	m_Options.AddOption(_("Abort compilation if warnings are issued"), _T("-Werror"), category);
	m_Options.AddOption(_("Inhibit all warning messages"), _T("-w"), category);


    m_Commands[(int)ctCompileObjectCmd] = _T("$compiler $options $includes -c $file -o $object");
    m_Commands[(int)ctGenDependenciesCmd] = _T("");
    m_Commands[(int)ctCompileResourceCmd] = _T("");
    m_Commands[(int)ctLinkConsoleExeCmd] = _T("$linker $libdirs -o $exe_output $link_objects $link_resobjects $link_options $libs");
#ifdef __WXMSW__
    m_Commands[(int)ctLinkExeCmd] = _T("$linker $libdirs -o $exe_output $link_objects $link_resobjects $link_options $libs -mwindows");
    m_Commands[(int)ctLinkDynamicCmd] = _T("$linker -shared -Wl,--output-def=$def_output -Wl,--out-implib=$static_output -Wl,--dll $libdirs $link_objects $link_resobjects -o $exe_output $link_options $libs");
#else
    m_Commands[(int)ctLinkExeCmd] = m_Commands[(int)ctLinkConsoleExeCmd]; // no -mwindows
    m_Commands[(int)ctLinkDynamicCmd] = _T("$linker -shared $libdirs $link_objects $link_resobjects -o $exe_output $link_options $libs");
#endif
    m_Commands[(int)ctLinkStaticCmd] = _T("$lib_linker -r -static -o $static_output $link_objects");
    m_Commands[(int)ctLinkNativeCmd] = m_Commands[(int)ctLinkConsoleExeCmd]; // unsupported currently

    LoadDefaultRegExArray();

    m_CompilerOptions.Clear();
    m_LinkerOptions.Clear();
    m_LinkLibs.Clear();
    m_CmdsBefore.Clear();
    m_CmdsAfter.Clear();
}

void CompilerTcc::LoadDefaultRegExArray()
{
    m_RegExes.Clear();
    m_RegExes.Add(RegExStruct(_("Fatal error"), cltError, _T("FATAL:[ \t]*(.*)"), 1));
    m_RegExes.Add(RegExStruct(_("Resource compiler error"), cltError, _T("windres.exe:[ \t](") + FilePathWithSpaces + _T("):([0-9]+):[ \t](.*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Resource compiler error (2)"), cltError, _T("windres.exe:[ \t](.*)"), 1));
    m_RegExes.Add(RegExStruct(_("Preprocessor warning"), cltWarning, _T("(") + FilePathWithSpaces + _T("):([0-9]+):([0-9]+):[ \t]([Ww]arning:[ \t].*)"), 4, 1, 2));
    m_RegExes.Add(RegExStruct(_("Preprocessor error"), cltError, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[0-9]+:[ \t](.*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compiler warning"), cltWarning, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[ \t]([Ww]arning:[ \t].*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compiler error"), cltError, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[ \t](.*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Linker error"), cltError, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[0-9]+:[ \t](.*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Linker error (2)"), cltError, FilePathWithSpaces + _T("\\(.text\\+[0-9A-Za-z]+\\):([ \tA-Za-z0-9_:+/\\.-]+):[ \t](.*)"), 2, 1));
    m_RegExes.Add(RegExStruct(_("Linker error (lib not found)"), cltError, _T(".*(ld.exe):[ \t](cannot find.*)"), 2, 1));
    m_RegExes.Add(RegExStruct(_("Undefined reference"), cltError, _T("(") + FilePathWithSpaces + _T("):[ \t](undefined reference.*)"), 2, 1));
    m_RegExes.Add(RegExStruct(_("General warning"), cltWarning, _T("([Ww]arning:[ \t].*)"), 1));
}

AutoDetectResult CompilerTcc::AutoDetectInstallationDir()
{
    wxString sep = wxFileName::GetPathSeparator();
#ifdef __WXMSW__
    m_MasterPath = _T("C:\\tcc");
#else
    m_MasterPath = _T("/usr");
#endif
    wxString BinPath = m_MasterPath + sep + wxT("tcc");
    AutoDetectResult ret = wxFileExists(BinPath + sep + m_Programs.C) ? adrDetected : adrGuessed;
    if (ret == adrDetected)
    {
        AddIncludeDir(m_MasterPath + sep + _T("include"));
        AddLibDir(m_MasterPath + sep + _T("lib"));
        m_ExtraPaths.Add(BinPath);
    }
    return ret;
} // end of AutoDetectInstallationDir
