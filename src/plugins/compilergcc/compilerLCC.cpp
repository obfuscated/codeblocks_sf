/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#ifdef __WXMSW__
// this compiler is valid only in windows

#include <sdk.h>
#include "logmanager.h"
#include "manager.h"
#include "compilerLCC.h"
#include <wx/intl.h>
#include <wx/regex.h>
#include <wx/config.h>
#include <wx/fileconf.h>
#include <wx/msgdlg.h>

#include <wx/msw/registry.h>

CompilerLCC::CompilerLCC() :
    Compiler(_("LCC Compiler"), _T("lcc")),
    m_RegistryUpdated(false)
{
    Reset();
}

CompilerLCC::~CompilerLCC()
{
    //dtor
}

Compiler* CompilerLCC::CreateCopy()
{
    Compiler* c = new CompilerLCC(*this);
    c->SetExtraPaths(m_ExtraPaths); // wxArrayString doesn't seem to be copied with the default copy ctor...
    return c;
}

void CompilerLCC::Reset()
{
    m_RegistryUpdated = false; // Check the registry another time on IsValid()

    m_Programs.C       = _T("lcc.exe");
    m_Programs.CPP     = _T("lcc.exe");
    m_Programs.LD      = _T("lcclnk.exe");
    m_Programs.DBG     = _T("cdb.exe");
    m_Programs.DBGconfig = wxEmptyString;
    m_Programs.LIB     = _T("lcclib.exe");
    m_Programs.WINDRES = _T("lrc.exe");
    m_Programs.MAKE    = _T("make.exe");

    m_Switches.includeDirs     = _T("-I");
    m_Switches.libDirs         = _T("-L");
    m_Switches.linkLibs        = _T("");
    m_Switches.defines         = _T("-D");
    m_Switches.genericSwitch   = _T("-");
    m_Switches.objectExtension = _T("obj");

    m_Switches.needDependencies        = true;
    m_Switches.forceCompilerUseQuotes  = false;
    m_Switches.forceLinkerUseQuotes    = false;
    m_Switches.logging                 = clogSimple; // clogFull;
    m_Switches.libPrefix               = _T("");
    m_Switches.libExtension            = _T("lib");
    m_Switches.linkerNeedsLibPrefix    = false;
    m_Switches.linkerNeedsLibExtension = true;

    m_Options.ClearOptions();

    // General
    wxString category = _("General");
    m_Options.AddOption(_("Generate the debugging information."), _T("-g2"), category, _T(""),
                        true,
                        _T("-O"),
                        _("You have optimizations enabled. This is Not A Good Thing(tm) when producing debugging symbols..."));
    m_Options.AddOption(_("Arrange for function stack tracing. If a trap occurs, the function stack will be displayed."), _T("-g3"), category, _T(""),
                        true,
                        _T("-O"),
                        _("You have optimizations enabled. This is Not A Good Thing(tm) when producing debugging symbols..."));
    m_Options.AddOption(_("Arrange for function stack and line number tracing."), _T("-g4"), category, _T(""),
                        true,
                        _T("-O"),
                        _("You have optimizations enabled. This is Not A Good Thing(tm) when producing debugging symbols..."));
    m_Options.AddOption(_("Arrange for function stack, line number, and return call stack corruption tracing."), _T("-g5"), category, _T(""),
                        true,
                        _T("-O"),
                        _("You have optimizations enabled. This is Not A Good Thing(tm) when producing debugging symbols..."));

    m_Options.AddOption(_("Inject code into the generated program to measure execution time. Incompatible with debug level higher than 2!"), _T("-profile"), category);
    m_Options.AddOption(_("All warnings will be active"), _T("-A"), category);
    m_Options.AddOption(_("Check the given source for errors. No object file is generated."), _T("-check"), category);
    m_Options.AddOption(_("No warnings will be emitted. Errors will be still printed."), _T("-nw"), category);
    m_Options.AddOption(_("Warn when a local variable shadows a global one."), _T("-shadows"), category);
    m_Options.AddOption(_("Warns about unused assignments and suppresses the dead code."), _T("-unused"), category);
    m_Options.AddOption(_("Generate code to test for overflow for all additions, subtractions and multiplications."), _T("-overflowcheck"), category);

    // Optimisation
    category = _("Optimization");
    m_Options.AddOption(_("Optimize the output. This activates the peephole optimizer."), _T("-O"), category);
    m_Options.AddOption(_("Enable Pentium III instructions."), _T("-p6"), category);
    m_Options.AddOption(_("The inline directive is ignored."), _T("-fno-inline"), category);
    m_Options.AddOption(_("Use declarations for lcclibc.dll. Don't forget the -dynamic option for the linker!"), _T("-libcdll"), category);
    m_Options.AddOption(_("Set the default alignment in structures to no alignment at all."), _T("-Zp1"), category);
    m_Options.AddOption(_("Set the default alignment in structures to 2."), _T("-Zp2"), category);
    m_Options.AddOption(_("Set the default alignment in structures to 4."), _T("-Zp4"), category);
    m_Options.AddOption(_("Set the default alignment in structures to 8."), _T("-Zp8"), category);
    m_Options.AddOption(_("Set the default alignment in structures to 16."), _T("-Zp16"), category);

    // Misc.
    category = _("Miscellaneous");
    m_Options.AddOption(_("Disallow the language extensions of lcc-win32."), _T("-ansic"), category);
    m_Options.AddOption(_("Print in standard output each include file recursively."), _T("-M1"), category);
    m_Options.AddOption(_("Do not query the registry for the lib path."), _T("-noregistrylookup"), category);
    m_Options.AddOption(_("Do not use underscores for name mangeling (pass this to the linker!)."), _T("-nounderscore"), category);

    m_Commands[(int)ctCompileObjectCmd].push_back(CompilerTool(_T("$compiler -c $includes $options $file -Fo$object")));
    m_Commands[(int)ctGenDependenciesCmd].push_back(CompilerTool(_T("")));
    m_Commands[(int)ctCompileResourceCmd].push_back(CompilerTool(_T("$rescomp $res_includes $file -fo$resource_output")));
    m_Commands[(int)ctLinkExeCmd].push_back(CompilerTool(_T("$linker $libdirs $link_options $link_objects $link_resobjects $libs -o $exe_output")));
    m_Commands[(int)ctLinkConsoleExeCmd].push_back(CompilerTool(_T("$linker -subsystem console $libdirs $link_options $link_objects $link_resobjects $libs -o $exe_output")));
    m_Commands[(int)ctLinkDynamicCmd].push_back(CompilerTool(_T("$linker -dll $libdirs $link_options $link_objects $link_resobjects $libs -o $exe_output")));
    m_Commands[(int)ctLinkStaticCmd].push_back(CompilerTool(_T("$lib_linker $link_objects $link_resobjects -OUT:$static_output")));
    m_Commands[(int)ctLinkNativeCmd].push_back(CompilerTool(_T("$linker -subsystem console $libdirs $link_options $link_objects $link_resobjects $libs -o $exe_output"))); // unsupported currently

    LoadDefaultRegExArray();

    m_CompilerOptions.Clear();
    m_LinkerOptions.Clear();
    m_LinkLibs.Clear();
    m_CmdsBefore.Clear();
    m_CmdsAfter.Clear();
}

void CompilerLCC::LoadDefaultRegExArray()
{
    m_RegExes.Clear();

    // Syntax: RegExStruct("Type", cltType, RegExp, (RegExp)#Message, (RegExp)#Filename, (RegExp)#Line))

    // Helper:
    // RegExp for file name (Win32):    ([ \tA-Za-z0-9_:+/\\.-]+)
    //                      (C::B):     FilePathWithSpaces (from compiler.h)
    // RegExp for alphanumeric:         ([0-9A-Za-z]+)
    // RegExp for hexa-decimal:         ([0-9A-Fa-fXx]+)
    // RegExp for hexa-decimal (0x...): (0x[0-9A-Fa-f]+)

    // Preprocessor error
    // GCC way:   myapp.c:3:21: stdlib1.h: No such file or directory
    // LCC way:   cpp: myapp.c:3 Could not find include file <stdlib1.h>
    m_RegExes.Add(RegExStruct(_("Preprocessor error"), cltError, _T("cpp:[ \t]\"?(") + FilePathWithSpaces + _T(")\"?:([0-9]+)[ \t](.*)"), 3, 1, 2));

    // Compiler warnings:
    // GCC way:   myapp.c:7: warning: unused variable `i'
    // LCC way:   Warning myapp.c: 7  local 'int i' is not referenced
    //            Warning "main.c": 6 missing prototype for pringtf
    m_RegExes.Add(RegExStruct(_("Compiler warning"), cltWarning, _T("[Ww]arning[ \t]\"?(") + FilePathWithSpaces + _T(")\"?:[ \t]([0-9]+)[ \t]+(.*)"), 3, 1, 2));

    // Compiler errors:
    // GCC way:   myapp.c:7: error: `into' undeclared (first use in this function)
    // LCC way:   Error myapp.c: 7  undeclared identifier 'into'
    m_RegExes.Add(RegExStruct(_("Compiler error"), cltError, _T("[Ee]rror[ \t]\"?(") + FilePathWithSpaces + _T(")\"?:[ \t]([0-9]+)[ \t]+(.*)"), 3, 1, 2));

    // Linker errors:
    // GCC way:   gcc.EXE: mylib.a: No such file or directory
    // LCC way:   cannot open mylib.lib
    m_RegExes.Add(RegExStruct(_("Linker error"), cltError, _T("(cannot open)[ \t]\"?(") + FilePathWithSpaces + _T(")\"?"), 1, 2, 0, 2));

    // Undefined references:
    // GCC way:   myapp.o:myapp.c:(.text+0x4c): undefined reference to `dummy'
    // LCC way:   myapp.obj .text: undefined reference to '_dummy'
    m_RegExes.Add(RegExStruct(_("Undefined reference"), cltError, _T("\"?(") + FilePathWithSpaces + _T(")\"?[ \t].text:[ \t](undefined reference.*)"), 2, 1));

    // Misc.
    m_RegExes.Add(RegExStruct(_("General warning"), cltWarning, _T("([Ww]arning[ \t].*)"), 1));
    m_RegExes.Add(RegExStruct(_("General error"),   cltError,   _T("([Ee]rror[ \t].*)"),   1));
}

AutoDetectResult CompilerLCC::AutoDetectInstallationDir()
{
    wxRegKey key; // defaults to HKCR
    wxString mpHKLM     = wxEmptyString;
    wxString mpHKCU     = wxEmptyString;
    wxString mpLccRoot  = wxEmptyString;
    wxString mpLccLnk   = wxEmptyString;
    wxString mpCompiler = wxEmptyString;

    // Query uninstall information if installed with admin rights:
    key.SetName(_T("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\lcc-win32 (base system)_is1"));
    if (key.Exists() && key.Open(wxRegKey::Read))
        key.QueryValue(_T("Inno Setup: App Path"), mpHKLM);

    // Query uninstall information if installed *without* admin rights:
    key.SetName(_T("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\lcc-win32 (base system)_is1"));
    if (key.Exists() && key.Open(wxRegKey::Read))
        key.QueryValue(_T("Inno Setup: App Path"), mpHKCU);

    // Check the LCC lccroot path
    key.SetName(_T("HKEY_CURRENT_USER\\Software\\lcc"));
    if (key.Exists() && key.Open(wxRegKey::Read))
        key.QueryValue(_T("lccroot"), mpLccRoot);
    if (mpLccRoot.IsEmpty())
    {
        // Check the LCC lccroot path
        key.SetName(_T("HKEY_CURRENT_USER\\Software\\lcc\\lccroot"));
        if (key.Exists() && key.Open(wxRegKey::Read))
            key.QueryValue(_T("path"), mpLccRoot);
    }

    // Check the LCC lcclnk path
    key.SetName(_T("HKEY_CURRENT_USER\\Software\\lcc\\lcclnk"));
    if (key.Exists() && key.Open(wxRegKey::Read))
    {
        key.QueryValue(_T("libpath"), mpLccLnk);
        wxString lib_path = _T("\\lib");
        if (   !mpLccLnk.IsEmpty()
            && (mpLccLnk.Length()>lib_path.Length())
            && (mpLccLnk.Lower().EndsWith(lib_path)) )
        {
            // Remove the lib path to point to the LCC root folder
            mpLccLnk.Remove( (mpLccLnk.Length()-lib_path.Length()), lib_path.Length() );
        }
    }

    // Check the LCC compiler path
    key.SetName(_T("HKEY_CURRENT_USER\\Software\\lcc\\compiler"));
    if (key.Exists() && key.Open(wxRegKey::Read))
    {
        key.QueryValue(_T("includepath"), mpCompiler);
        wxString inc_path = _T("\\include");
        if (   !mpCompiler.IsEmpty()
            && (mpCompiler.Length()>inc_path.Length())
            && (mpCompiler.Lower().EndsWith(inc_path)) )
        {
            // Remove the include path to point to the LCC root folder
            mpCompiler.Remove( (mpCompiler.Length()-inc_path.Length()), inc_path.Length() );
        }
    }

    // Verify all path's obtained
    wxString compiler; compiler << wxFILE_SEP_PATH << _T("bin") << wxFILE_SEP_PATH << m_Programs.C;

    if      (wxFileExists(mpHKLM     + compiler))
        m_MasterPath = mpHKLM;
    else if (wxFileExists(mpHKCU     + compiler))
        m_MasterPath = mpHKCU;
    else if (wxFileExists(mpLccRoot  + compiler))
        m_MasterPath = mpLccRoot;
    else if (wxFileExists(mpLccLnk   + compiler))
        m_MasterPath = mpLccLnk;
    else if (wxFileExists(mpCompiler + compiler))
        m_MasterPath = mpCompiler;
    else
        m_MasterPath = _T("C:\\lcc"); // just a guess; the default installation dir

    if (!m_MasterPath.IsEmpty())
    {
        AddIncludeDir   (m_MasterPath + wxFILE_SEP_PATH + _T("include"));
        AddLibDir       (m_MasterPath + wxFILE_SEP_PATH + _T("lib"));
        m_ExtraPaths.Add(m_MasterPath + wxFILE_SEP_PATH + _T("bin"));
    }

    m_RegistryUpdated = false; // Check the registry another time on IsValid()

    return wxFileExists(m_MasterPath+compiler) ? adrDetected : adrGuessed;
}

bool CompilerLCC::IsValid()
{
    if (!m_RegistryUpdated)
    {
        wxString compiler = m_MasterPath + wxFILE_SEP_PATH
                          + _T("bin") + wxFILE_SEP_PATH + m_Programs.C;

        if (wxFileExists(compiler))
        {
            Manager::Get()->GetLogManager()->DebugLog(_T("LCC: Updating registry..."));

            // Make sure the registry is setup as it should be after an installation.
            // This avoids the "smart and clever" LCC compiler asking for the
            // <include> header and lcc libraries path's on the command line (huh?!).
            // Note: A compiler *never ever* should ask on std::cin anything!!!
            wxRegKey key; // defaults to HKCR

            key.SetName(_T("HKEY_CURRENT_USER\\Software\\lcc"));
            if (!key.Exists() && key.Create())
                key.SetValue(_T("lccroot"),     m_MasterPath                 );

            key.SetName(_T("HKEY_CURRENT_USER\\Software\\lcc\\compiler"));
            if (!key.Exists() && key.Create())
                key.SetValue(_T("includepath"), m_MasterPath+_T("\\include") );

            key.SetName(_T("HKEY_CURRENT_USER\\Software\\lcc\\lcclnk"));
            if (!key.Exists() && key.Create())
                key.SetValue(_T("libpath"),     m_MasterPath+_T("\\lib")     );

            key.SetName(_T("HKEY_CURRENT_USER\\Software\\lcc\\lccroot"));
            if (!key.Exists() && key.Create())
                key.SetValue(_T("path"),        m_MasterPath                 );

            m_RegistryUpdated = true;
        }
    }

    return Compiler::IsValid();
}

#endif // __WXMSW__
