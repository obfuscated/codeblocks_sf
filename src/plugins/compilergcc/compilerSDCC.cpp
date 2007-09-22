/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is distributed under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include <sdk.h>
#include <prep.h>
#include "compilerSDCC.h"
#include <wx/intl.h>
#include <wx/regex.h>
#include <wx/config.h>
#include <wx/fileconf.h>
#include <wx/msgdlg.h>

#ifdef __WXMSW__
    #include <wx/msw/registry.h>
#endif

CompilerSDCC::CompilerSDCC()
    : Compiler(_("SDCC Compiler"), _T("sdcc"))
{
    Reset();
}

CompilerSDCC::~CompilerSDCC()
{
    //dtor
}

Compiler * CompilerSDCC::CreateCopy()
{
    Compiler* c = new CompilerSDCC(*this);
    c->SetExtraPaths(m_ExtraPaths); // wxArrayString doesn't seem to be copied with the default copy ctor...
    return c;
}

void CompilerSDCC::Reset()
{
    if (platform::windows)
    {
        m_Programs.C = _T("sdcc.exe");
        m_Programs.CPP = _T("sdcc.exe");
        m_Programs.LD = _T("sdcc.exe");
        m_Programs.DBG = _T("sdcdb.exe");
        m_Programs.LIB = _T("sdcclib.exe");
        m_Programs.WINDRES = _T("");
        m_Programs.MAKE = _T("make.exe");
    }
    else
    {
        m_Programs.C = _T("sdcc");
        m_Programs.CPP = _T("sdcc");
        m_Programs.LD = _T("sdcc");
        m_Programs.DBG = _T("sdcdb");
        m_Programs.LIB = _T("sdcclib");
        m_Programs.WINDRES = _T("");
        m_Programs.MAKE = _T("make");
    }
    m_Switches.includeDirs = _T("-I");
    m_Switches.libDirs = _T("-L");
    m_Switches.linkLibs = _T("-l");
    m_Switches.defines = _T("-D");
    m_Switches.genericSwitch = _T("-");
    m_Switches.objectExtension = _T("rel");

    m_Switches.needDependencies = true;
    m_Switches.forceCompilerUseQuotes = false;
    m_Switches.forceLinkerUseQuotes = false;
    m_Switches.logging = clogSimple; // clogFull;
    m_Switches.libPrefix = _T("lib");
    m_Switches.libExtension = _T("lib");
    m_Switches.linkerNeedsLibPrefix = false;
    m_Switches.linkerNeedsLibExtension = false;

    // Summary of SDCC options: http://sdcc.sourceforge.net

    m_Options.ClearOptions();

    wxString category = _("General");
    m_Options.AddOption(_("Produce debugging symbols"), _T("--debug"), category, _T(""),
                true,
                _T("--opt-code-speed --opt-code-size"),
                _("You have optimizations enabled. This is Not A Good Thing(tm) when producing debugging symbols..."));

    m_Options.AddOption(_("All functions will be compiled as reentrant"), _T("--stack-auto"), category);
    m_Options.AddOption(_("Be verbose"), _T("--verbose"), category);
    m_Options.AddOption(_("Generate extra profiling information"), _T("--profile"), category);
    m_Options.AddOption(_("Callee will always save registers used"), _T("--all-callee-saves"), category);
    m_Options.AddOption(_("Leave out the frame pointer"), _T("--fommit-frame-pointer"), category);
    m_Options.AddOption(_("[MCS51/DS390] - use Bank1 for parameter passing"), _T("--parms-in-bank1"), category);

    // optimization
    category = _("Optimization");
    m_Options.AddOption(_("Optimize generated code (for speed)"), _T("--opt-code-speed"), category);
    m_Options.AddOption(_("Optimize generated code (for size)"), _T("--opt-code-size"), category);

    // machine dependent options - cpu arch
    category = _("CPU architecture (choose none, or only one of these)");
    m_Options.AddOption(_("[CPU] Intel MCS51 (default)"), _T("-mmcs51"), category);
    m_Options.AddOption(_("[CPU] Dallas DS80C390"), _T("-mds390"), category);
    m_Options.AddOption(_("[CPU] Dallas DS80C400"), _T("-mds400"), category);
    m_Options.AddOption(_("[CPU] Freescale/Motorola HC08"), _T("-mhc08"), category);
    m_Options.AddOption(_("[CPU] Zilog Z80"), _T("-mz80"), category);
    m_Options.AddOption(_("[CPU] GameBoy Z80 (Not actively maintained)."), _T("-mgbz80"), category);
    m_Options.AddOption(_("[CPU] Atmel AVR (In development, not complete)"), _T("-mavr"), category);
    m_Options.AddOption(_("[CPU] Microchip PIC 14-bit (p16f84 and variants. In development, not complete)"), _T("-mpic14"), category);
    m_Options.AddOption(_("[CPU] PIC 16-bit (p18f452 and variants. In development, not complete)"), _T("-mpic16"), category);

    // MCS51 dependent options
    category = _("MCS51 Options");
    m_Options.AddOption(_("[MCS51] Large model programs (default is Small)"), _T("--model-large"), category);
    m_Options.AddOption(_("[MCS51] Use a pseudo stack in the first 256 bytes in the external ram"), _T("--xstack"), category);
    m_Options.AddOption(_("[MCS51] Linker use old style for allocating memory areas."), _T("--no-pack-iram"), category);

    // DS390 / DS400 Options
    category = _("DS390 / DS400 Options");
    m_Options.AddOption(_("[DS390 / DS400] Generate 24-bit flat mode code"), _T("--model-flat24"), category);
    m_Options.AddOption(_("[DS390 / DS400] Disable interrupts during ESP:SP updates"), _T("--protect-sp-update"), category);
    m_Options.AddOption(_("[DS390 / DS400] Insert call to function __stack_probe at each function prologue"), _T("--stack-probe"), category);
    m_Options.AddOption(_("[DS390 / DS400] Generate code for DS390 Arithmetic Accelerator"), _T("--use-accelerator"), category);

    // Z80 Options
    category = _("Z80 Options");
    m_Options.AddOption(_("[Z80] Force a called function to always save BC"), _T("--callee-saves-bc"), category);
    m_Options.AddOption(_("[Z80] When linking, skip the standard crt0.o object file"), _T("--no-std-crt0"), category);

    // Linker output format options
    category = _("Linker output format (choose none, or only one of these)");
    m_Options.AddOption(_("Output Intel Hex (default)"), _T("--out-fmt-ihx"), category);
    m_Options.AddOption(_("Output Motorola S19"), _T("--out-fmt-s19"), category);
    m_Options.AddOption(_("Output ELF (Currently only supported for the HC08 processors)"), _T("--out-fmt-elf"), category);

    m_Commands[(int)ctCompileObjectCmd].push_back(CompilerTool(_T("$compiler $options $includes -c $file -o $object")));
    m_Commands[(int)ctGenDependenciesCmd].push_back(CompilerTool(_T("$compiler -MM $options -MF $dep_object -MT $object $includes $file")));
    m_Commands[(int)ctLinkExeCmd].push_back(CompilerTool(_T("$linker $libdirs -o $exe_output $options $link_options $libs $link_objects")));
    m_Commands[(int)ctLinkConsoleExeCmd].push_back(CompilerTool(_T("$linker $libdirs -o $exe_output $options $link_options $libs $link_objects")));
    //m_Commands[(int)ctLinkStaticCmd].push_back(CompilerTool(_T("$lib_linker -r $static_output $link_objects\n\tranlib $exe_output")));
    m_Commands[(int)ctLinkNativeCmd] = m_Commands[(int)ctLinkConsoleExeCmd]; // unsupported currently

    LoadDefaultRegExArray();

    m_CompilerOptions.Clear();
    m_LinkerOptions.Clear();
    m_LinkLibs.Clear();
    m_CmdsBefore.Clear();
    m_CmdsAfter.Clear();
}

void CompilerSDCC::LoadDefaultRegExArray()
{
    m_RegExes.Clear();
    m_RegExes.Add(RegExStruct(_("Fatal error"), cltError, _T("FATAL:[ \t]*(.*)"), 1));
    m_RegExes.Add(RegExStruct(_("Compiler warning (.h)"), cltWarning, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[0-9:]+[ \t]([Ww]arning[: \t].*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compiler warning"), cltWarning, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[ \t]([Ww]arning[: \t].*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compiler error"), cltError, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[ \t](.*[Ee]rror[: \t].*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compiler error (2)"), cltError, _T("(") + FilePathWithSpaces + _T("):([0-9]+):[0-9:]+ (.*: No such .*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Linker warning"), cltWarning, _T("(ASlink-Warning-.*)"), 1));
}

AutoDetectResult CompilerSDCC::AutoDetectInstallationDir()
{
    if (platform::windows)
    {
#ifdef __WXMSW__ // for wxRegKey
        wxRegKey key;   // defaults to HKCR
        key.SetName(wxT("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\SDCC"));
        if (key.Open(wxRegKey::Read)) // found; read it
            key.QueryValue(wxT("UninstallString"), m_MasterPath);
#endif

        if (m_MasterPath.IsEmpty())
            // just a guess; the default installation dir
            m_MasterPath = wxT("C:\\sdcc");
        else {
            wxFileName fn(m_MasterPath);
            m_MasterPath = fn.GetPath();
        }

        if (!m_MasterPath.IsEmpty())
        {
            AddIncludeDir(m_MasterPath + wxFILE_SEP_PATH + wxT("include"));
            AddLibDir(m_MasterPath + wxFILE_SEP_PATH + wxT("lib"));
            m_ExtraPaths.Add(m_MasterPath + wxFILE_SEP_PATH + wxT("bin"));
        }
    }
    else
        m_MasterPath=_T("/usr/local/bin"); // default

    return wxFileExists(m_MasterPath + wxFILE_SEP_PATH + wxT("bin") + wxFILE_SEP_PATH + m_Programs.C) ? adrDetected : adrGuessed;
}
