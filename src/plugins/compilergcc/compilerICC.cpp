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
#include "compilerICC.h"
#include <wx/intl.h>
#include <wx/regex.h>
#include <wx/config.h>
#include <wx/fileconf.h>
#include <wx/msgdlg.h>
#include "manager.h"
#include "logmanager.h"

CompilerICC::CompilerICC()
    : Compiler(_("Intel C/C++ Compiler"), _T("icc"))
{
    Reset();
}

CompilerICC::~CompilerICC()
{
    //dtor
}

Compiler * CompilerICC::CreateCopy()
{
    Compiler* c = new CompilerICC(*this);
    c->SetExtraPaths(m_ExtraPaths); // wxArrayString doesn't seem to be copied with the default copy ctor...
    return c;
}

void CompilerICC::Reset()
{
    if (platform::windows)
    {
        // Looks alot like the msvc compiler. Needs sdk as the msvc does
        m_Programs.C = _T("icl.exe");
        m_Programs.CPP = _T("icl.exe");
        m_Programs.LD = _T("xilink.exe"); //Runs Microsoft's link.exe
        m_Programs.DBG = _T("idb.exe");
        m_Programs.LIB = _T("xilink.exe");
        m_Programs.WINDRES = _T("rc.exe"); // platform SDK is needed for this
        m_Programs.MAKE = _T("mingw32-make.exe");//it works with nmake as well but cb doesn't

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
        m_Switches.libPrefix = _T("");
        m_Switches.libExtension = _T("lib");
        m_Switches.linkerNeedsLibPrefix = false;
        m_Switches.linkerNeedsLibExtension = true;

        // Intel Compiler Options from the supplied with the compiler documentation (man pages and docs)
        // NOTE(yop): I have been as descriptive as possible, producing very long descriptions for
        //            each option. Should this change to something more compact but less informative?
        //            These are not the most common options but they are from my point of view some
        //            of the most usefull ones. Some of them should be removed to conform with the
        //            rest of the supported compilers layout of options (that are significantly less).
        m_Options.ClearOptions();
        // Debug and Profile options
        wxString category = _("Output and Debug");
        m_Options.AddOption(_("Trap uninitialized variables"), _T("/Qtrapuv"), category);
        // This is a tricky one. I see precompiled headers enabled by default in RC2 but each compiler
        // takes different flags to generate them. Shouldn't the functionality of precompiled headers
        // move here? This option produces a precompiled header during first compilation and uses the
        // produced one for the next ones.
        m_Options.AddOption(_("Enable automatic precompiled header file creation/usage"), _T("/YX"), category);
        m_Options.AddOption(_("Perform syntax and semantic checking only (no object file produced)"), _T("/Zs"), category);
        m_Options.AddOption(_("Produce symbolic debug information in object files (you should avoid using any optimizations)"), _T("/Zi"), category);

        // Compiler Diagnostics
        category = _("Compiler Diagnostics (some options overide each other)");
        m_Options.AddOption(_("Only display errors"), _T("-W0"), category);
        m_Options.AddOption(_("Enable more strict diagnostics"), _T("-Wcheck"), category);
        m_Options.AddOption(_("Force warnings to be reported as errors"), _T("-WX"), category);
        m_Options.AddOption(_("Print brief one-line diagnostics"), _T("-WL"), category);
        m_Options.AddOption(_("Enable all compiler diagnostics"), _T("-Wall"), category);
        m_Options.AddOption(_("Issue portability diagnostics"), _T("-Wport"), category);
        m_Options.AddOption(_("Print diagnostics for 64-bit porting"), _T("-Wp64"), category);

        // Performance
        category = _("Performance");
        m_Options.AddOption(_("Disable optimizations"), _T("/Od"), category);
        m_Options.AddOption(_("Optimize for maximum speed, but disable some optimizations which increase code size for a small speed benefit."), _T("/O1"), category);
        m_Options.AddOption(_("Enable optimizations"), _T("/O2"), category);
        m_Options.AddOption(_("Enable -O2 plus more aggressive optimizations that may not improve performance for all programs"), _T("/O3"), category);
        m_Options.AddOption(_("Enable speed optimizations, but disable some optimizations which increase code size for small speed benefit"), _T("/Os"), category);
        m_Options.AddOption(_("Enable -xP -O3 -ipo -no-prec-div -static"), _T("/fast"), category);
        m_Options.AddOption(_("Disable inlining"), _T("/Ob0"), category);
        m_Options.AddOption(_("Inline functions declared with __inline, and perform C++ inlining"), _T("/Ob1"), category);
        m_Options.AddOption(_("Inline any function, at the compiler's discretion"), _T("/Ob2"), category);
        m_Options.AddOption(_("Assume no aliasing in program"), _T("/Oa"), category);
        m_Options.AddOption(_("Assume no aliasing within functions, but assume aliasing across calls"), _T("/Ow"), category);
        m_Options.AddOption(_("Maintain floating point precision (disables some optimizations)"), _T("/Op"), category);
        m_Options.AddOption(_("Improve floating-point precision (speed impact is less than -mp)"), _T("/Qprec"), category);
        m_Options.AddOption(_("Disable using EBP as general purpose register"), _T("-Oy"), category);
        m_Options.AddOption(_("Improve precision of floating-point divides (some speed impact)"), _T("/Qprec-div"), category);
        m_Options.AddOption(_("Determine if certain square root optimizations are enabled"), _T("/Qprec-sqrt"), category);
        m_Options.AddOption(_("Round fp results at assignments & casts (some speed impact)"), _T("/Qfp-port"), category);
        m_Options.AddOption(_("Enable fp stack checking after every function/procedure call"), _T("/Qfpstkchk"), category);
        m_Options.AddOption(_("Rounding mode to enable fast float-to-int conversions"), _T("/Qrcd"), category);
        m_Options.AddOption(_("Optimize specificly for Pentium processor"), _T("/G5"), category);
        m_Options.AddOption(_("Optimize specificly for Pentium Pro, Pentium II and Pentium III processors"), _T("/G6"), category);
        m_Options.AddOption(_("Code is optimized for Intel Pentium III"), _T("/QxK"), category);
        m_Options.AddOption(_("Code is optimized for Intel Pentium 4"), _T("/QxW"), category);
        m_Options.AddOption(_("Code is optimized for Intel Pentium 4 and enables new optimizations"), _T("/QxN"), category);
        m_Options.AddOption(_("Code is optimized for Intel Pentium M"), _T("/QxB"), category);
        m_Options.AddOption(_("Code is optimized for Intel Pentium 4 with SSE3 support"), _T("/QxP"), category);

        // Language
        category = _("Language");
        m_Options.AddOption(_("Enable the 'restrict' keyword for disambiguating pointers"), _T("/Qrestrict"), category);
        m_Options.AddOption(_("Strict ANSI conformance dialects"), _T("/Za"), category);
        m_Options.AddOption(_("Compile all source or unrecognized file types as C++ source files"), _T("/Qc++"), category);
        m_Options.AddOption(_("Disable RTTI support"), _T("/GR-"), category);
        m_Options.AddOption(_T("Process OpenMP directives"), _T("/Qopenmp"), category);
        m_Options.AddOption(_("Specify alignment constraint for structures to 1"), _T("/Zp1"), category);
        m_Options.AddOption(_("Specify alignment constraint for structures to 2"), _T("/Zp2"), category);
        m_Options.AddOption(_("Specify alignment constraint for structures to 4"), _T("/Zp4"), category);
        m_Options.AddOption(_("Specify alignment constraint for structures to 8"), _T("/Zp8"), category);
        m_Options.AddOption(_("Specify alignment constraint for structures to 16"), _T("/Zp16"), category);
        m_Options.AddOption(_("Change default char type to unsigned"), _T("/J"), category);

        m_Commands[(int)ctCompileObjectCmd].push_back(CompilerTool(_T("$compiler /nologo $options $includes /c $file /Fo$object")));
        //The rest are part of the microsoft sdk. The xilink.exe calls link.exe eventually.
        m_Commands[(int)ctCompileResourceCmd].push_back(CompilerTool(_T("$rescomp $res_includes -fo$resource_output $file")));
        m_Commands[(int)ctLinkExeCmd].push_back(CompilerTool(_T("$linker /nologo /subsystem:windows $libdirs /out:$exe_output $libs $link_objects $link_resobjects $link_options")));
        m_Commands[(int)ctLinkConsoleExeCmd].push_back(CompilerTool(_T("$linker /nologo $libdirs /out:$exe_output $libs $link_objects $link_resobjects $link_options")));
        m_Commands[(int)ctLinkDynamicCmd].push_back(CompilerTool(_T("$linker /dll /nologo $libdirs /out:$exe_output $libs $link_objects $link_resobjects $link_options")));
        m_Commands[(int)ctLinkStaticCmd].push_back(CompilerTool(_T("$lib_linker /lib /nologo $libdirs /out:$static_output $libs $link_objects $link_resobjects $link_options")));
        m_Commands[(int)ctLinkNativeCmd] = m_Commands[(int)ctLinkConsoleExeCmd]; // unsupported currently
    }
    else
    {
        m_Programs.C = _T("icc");
        m_Programs.CPP = _T("icpc");
        m_Programs.LD = _T("icpc");
        m_Programs.DBG = _T("idb");
        m_Programs.LIB = _T("ar");
        m_Programs.WINDRES = _T("");
        m_Programs.MAKE = _T("make");

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

        m_Options.ClearOptions();
        // Debug and Profile options
        wxString category = _("Output, Debug and Profile");
        m_Options.AddOption(_("Trap uninitialized variables"), _T("-ftrapuv"), category);
        m_Options.AddOption(_("Enable automatic precompiled header file creation/usage"), _T("-pch"), category);
        m_Options.AddOption(_("Perform syntax and semantic checking only (no object file produced)"), _T("-fsyntax-only"), category);
        m_Options.AddOption(_("Produce symbolic debug information in object files (you should avoid using any optimizations)"), _T("-g"), category);
        m_Options.AddOption(_("Compile and link for function profiling with UNIX gprof tool"), _T("-p"), category);

        // Compiler Diagnostics
        category = _("Compiler Diagnostics (some options overide each other)");
        m_Options.AddOption(_("Only display errors"), _T("-w0"), category);
        m_Options.AddOption(_("Display remarks, warnings, and errors"), _T("-w2"), category);
        m_Options.AddOption(_("Enable more strict diagnostics"), _T("-Wcheck"), category);
        m_Options.AddOption(_("Force warnings to be reported as errors"), _T("-Werror"), category);
        m_Options.AddOption(_("Print brief one-line diagnostics"), _T("-Wbrief"), category);
        m_Options.AddOption(_("Enable all compiler diagnostics"), _T("-Wall"), category);
        m_Options.AddOption(_("Warn for missing prototypes"), _T("-Wmissing-prototypes"), category);
        m_Options.AddOption(_("Warn for questionable pointer arithmetic"), _T("-Wpointer-arith"), category);
        m_Options.AddOption(_("Warn if a variable is used before being initialized"), _T("-Wuninitialized"), category);
        m_Options.AddOption(_("Enable inline diagnostics"), _T("-Winline"), category);
        m_Options.AddOption(_("Print warnings related to deprecated features"), _T("-Wdeprecated"), category);
        m_Options.AddOption(_("Warn if declared function is not used"), _T("-Wunused-function"), category);
        m_Options.AddOption(_("Don't warn if an unknown #pragma directive is used"), _T("-Wno-unknown-pragmas"), category);
        m_Options.AddOption(_("Warn if return type of main is not expected"), _T("-Wmain"), category);
        m_Options.AddOption(_("Warn when /* appears in the middle of a /* */ comment"), _T("-Wcomment"), category);
        m_Options.AddOption(_("Warn when a function uses the default int return type and warn when a return statement is used in a void function"), _T("-Wreturn-type"), category);
        m_Options.AddOption(_("Print diagnostics for 64-bit porting"), _T("-Wp64"), category);

        // Performance
        category = _("Performance");
        m_Options.AddOption(_("Disable optimizations"), _T("-O0"), category);
        m_Options.AddOption(_("Optimize for maximum speed, but disable some optimizations which increase code size for a small speed benefit."), _T("-O1"), category);
        m_Options.AddOption(_("Enable optimizations"), _T("-O2"), category);
        m_Options.AddOption(_("Enable -O2 plus more aggressive optimizations that may not improve performance for all programs"), _T("-O3"), category);
        m_Options.AddOption(_("Enable speed optimizations, but disable some optimizations which increase code size for small speed benefit"), _T("-Os"), category);
        m_Options.AddOption(_("Enable -xP -O3 -ipo -no-prec-div -static"), _T("-fast"), category);
        m_Options.AddOption(_("Disable inlining"), _T("-Ob0"), category);
        m_Options.AddOption(_("Inline functions declared with __inline, and perform C++ inlining"), _T("-Ob1"), category);
        m_Options.AddOption(_("Inline any function, at the compiler's discretion"), _T("-Ob2"), category);
        m_Options.AddOption(_("Assume no aliasing in program"), _T("-fno-alias"), category);
        m_Options.AddOption(_("Assume no aliasing within functions, but assume aliasing across calls"), _T("-fno-fnalias"), category);
        m_Options.AddOption(_("Maintain floating point precision (disables some optimizations)"), _T("-mp"), category);
        m_Options.AddOption(_("Improve floating-point precision (speed impact is less than -mp)"), _T("-mp1"), category);
        m_Options.AddOption(_("Disable using EBP as general purpose register"), _T("-fp"), category);
        m_Options.AddOption(_("Improve precision of floating-point divides (some speed impact)"), _T("-prec-div"), category);
        m_Options.AddOption(_("Determine if certain square root optimizations are enabled"), _T("-prec-sqrt"), category);
        m_Options.AddOption(_("Round fp results at assignments & casts (some speed impact)"), _T("-fp-port"), category);
        m_Options.AddOption(_("Enable fp stack checking after every function/procedure call"), _T("-fpstkchk"), category);
        m_Options.AddOption(_("Rounding mode to enable fast float-to-int conversions"), _T("-rcd"), category);
        m_Options.AddOption(_("Optimize specificly for Pentium processor"), _T("-mtune=pentium"), category);
        m_Options.AddOption(_("Optimize specificly for Pentium Pro, Pentium II and Pentium III processors"), _T("-mtune=pentiumpro"), category);
        m_Options.AddOption(_("Generate code excusively for Pentium Pro and Pentium II processor instructions"), _T("-march=pentiumpro"), category);
        m_Options.AddOption(_("Generate code excusively for MMX instructions"), _T("-march=pentiumii"), category);
        m_Options.AddOption(_("Generate code excusively for streaming SIMD extensions"), _T("-march=pentiumiii"), category);
        m_Options.AddOption(_("Generate code excusively for Pentium 4 New Instructions"), _T("-march=pentium4"), category);
        m_Options.AddOption(_("Generate code for Intel Pentium III and compatible Intel processors"), _T("-msse"), category);
        m_Options.AddOption(_("Generate code for Intel Pentium 4 and compatible Intel processors"), _T("-msse2"), category);
        m_Options.AddOption(_("Generate code for Intel Pentium 4 processors with SSE3 extensions"), _T("-msse3"), category);

        // Language
        category = _("Language");
        m_Options.AddOption(_("Enable the 'restrict' keyword for disambiguating pointers"), _T("-restrict"), category);
        m_Options.AddOption(_("Equivalent to GNU -ansi"), _T("-ansi"), category);
        m_Options.AddOption(_("Strict ANSI conformance dialects"), _T("-strict-ansi"), category);
        m_Options.AddOption(_("Compile all source or unrecognized file types as C++ source files"), _T("-Kc++"), category);
        m_Options.AddOption(_("Disable RTTI support"), _T("-fno-rtti"), category);
        m_Options.AddOption(_T("Process OpenMP directives"), _T("-openmp"), category);
        m_Options.AddOption(_("Analyze and reorder memory layout for variables and arrays"), _T("-align"), category);
        m_Options.AddOption(_("Specify alignment constraint for structures to 1"), _T("-Zp1"), category);
        m_Options.AddOption(_("Specify alignment constraint for structures to 2"), _T("-Zp2"), category);
        m_Options.AddOption(_("Specify alignment constraint for structures to 4"), _T("-Zp4"), category);
        m_Options.AddOption(_("Specify alignment constraint for structures to 8"), _T("-Zp8"), category);
        m_Options.AddOption(_("Specify alignment constraint for structures to 16"), _T("-Zp16"), category);
        m_Options.AddOption(_("Allocate as many bytes as needed for enumerated types"), _T("-fshort-enums"), category);
        m_Options.AddOption(_("Change default char type to unsigned"), _T("-funsigned-char"), category);
        m_Options.AddOption(_("Change default bitfield type to unsigned"), _T("-funsigned-bitfields"), category);
        m_Options.AddOption(_("Disable support for operator name keywords"), _T("-fno-operator-names"), category);
        m_Options.AddOption(_("Do not recognize 'typeof' as a keyword"), _T("-fno-gnu-keywords"), category);
        m_Options.AddOption(_("Allow for non-conformant code"), _T("-fpermissive"), category);

        m_Commands[(int)ctCompileObjectCmd].push_back(CompilerTool(_T("$compiler $options $includes -c $file -o $object")));
        m_Commands[(int)ctGenDependenciesCmd].push_back(CompilerTool(_T("$compiler -MM $options -MF $dep_object -MT $object $includes $file")));
        m_Commands[(int)ctCompileResourceCmd].push_back(CompilerTool(_T("$rescomp -i $file -J rc -o $resource_output -O coff $res_includes")));
        m_Commands[(int)ctLinkConsoleExeCmd].push_back(CompilerTool(_T("$linker $libdirs -o $exe_output $link_objects $link_resobjects $link_options $libs")));
        m_Commands[(int)ctLinkExeCmd] = m_Commands[(int)ctLinkConsoleExeCmd];
        m_Commands[(int)ctLinkDynamicCmd].push_back(CompilerTool(_T("$linker -shared $libdirs $link_objects $link_resobjects -o $exe_output $link_options $libs")));
        m_Commands[(int)ctLinkStaticCmd].push_back(CompilerTool(_T("$lib_linker -r $static_output $link_objects\n\tranlib $exe_output")));
        m_Commands[(int)ctLinkNativeCmd] = m_Commands[(int)ctLinkConsoleExeCmd]; // unsupported currently
    }

    LoadDefaultRegExArray();

    m_CompilerOptions.Clear();
    m_LinkerOptions.Clear();
    m_LinkLibs.Clear();
    m_CmdsBefore.Clear();
    m_CmdsAfter.Clear();
}

void CompilerICC::LoadDefaultRegExArray()
{
    m_RegExes.Clear();
    m_RegExes.Add(RegExStruct(_("Compilation remark"), cltWarning, _T("(") + FilePathWithSpaces + _T(")\\(([0-9]+).:[ \t]([Rr]emark[ \t]#[0-9]+:[ \t].*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("OpenMP remark"), cltInfo, _T("(") + FilePathWithSpaces + _T(")\\(([0-9]+)\\):[ \\t]\\(col. ([0-9]+)\\)[ \\t]([Rr]emark:[ \\t].*)"), 4, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compilation warning"), cltWarning, _T("(") + FilePathWithSpaces + _T(")\\(([0-9]+).:[ \t]([Ww]arning[ \t]#[0-9]+:[ \t].*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compilation error"), cltError, _T("(") + FilePathWithSpaces + _T(")\\(([0-9]+).:[ \t](.*)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("General warning"), cltWarning, _T("([Ww]arning:[ \t].*)"), 1));
    m_RegExes.Add(RegExStruct(_("General error"), cltError, _T("([Ee]rror:[ \t].*)"), 1));
}

AutoDetectResult CompilerICC::AutoDetectInstallationDir()
{
    wxString sep = wxFileName::GetPathSeparator();
    wxString extraDir = _T("");
    if (platform::windows)
    {
        // Read the ICPP_COMPILER90 environment variable
        wxGetEnv(_T("ICPP_COMPILER90"), &m_MasterPath);
        extraDir = sep + _T("Ia32");// Intel also provides compiler for Itanium processors

        if (m_MasterPath.IsEmpty())
        {
            // just a guess the default installation dir
            wxString Programs = _T("C:\\Program Files");
            // what's the "Program Files" location
            // TO DO : support 64 bit ->    32 bit apps are in "ProgramFiles(x86)"
            //                              64 bit apps are in "ProgramFiles"
            wxGetEnv(_T("ProgramFiles"), &Programs);
            m_MasterPath = Programs + _T("\\Intel\\Compiler\\C++\\9.0");
        }
    }
    else
    {
        m_MasterPath = _T("/opt/intel/cc/9.0");
        if (wxDirExists(_T("/opt/intel")))
        {
            wxDir icc_dir(_T("/opt/intel/cc"));
            if (icc_dir.IsOpened())
            {
                wxArrayString dirs;
                wxIccDirTraverser IccDirTraverser(dirs);
                icc_dir.Traverse(IccDirTraverser);
                if (!dirs.IsEmpty())
                {
                    // Now sort the array in reverse order to get the latest version's path
                    dirs.Sort(true);
                    m_MasterPath = dirs[0];
                }
            }
        }
    }

    AutoDetectResult ret = wxFileExists(m_MasterPath + extraDir + sep + _T("bin") + sep + m_Programs.C) ? adrDetected : adrGuessed;
    if (ret == adrDetected)
    {
        AddIncludeDir(m_MasterPath + extraDir + sep + _T("Include"));
        AddLibDir(m_MasterPath + extraDir + sep + _T("Lib"));
    }
    // Try to detect the debugger. If not detected succesfully the debugger plugin will
    // complain, so only the autodetection of compiler is considered in return value
    wxString path;
    if (platform::windows)
    {
        wxGetEnv(_T("IDB_PATH"), &path);
        path += _T("IDB\\9.0\\IA32");
    }
    else
    {
        path= _T("/opt/intel/idb/9.0");
        if (wxDirExists(_T("/opt/intel")))
        {
            wxDir icc_debug_dir(_T("/opt/intel/idb"));
            if (icc_debug_dir.IsOpened())
            {
                wxArrayString debug_dirs;
                wxIccDirTraverser IccDebugDirTraverser(debug_dirs);
                icc_debug_dir.Traverse(IccDebugDirTraverser);
                if (!debug_dirs.IsEmpty())
                {
                    // Now sort the array in reverse order to get the latest version's path
                    debug_dirs.Sort(true);
                    path = debug_dirs[0];
                }
            }
        }
    }

    if (wxFileExists(path + sep + _T("bin") + sep + m_Programs.DBG))
        m_ExtraPaths.Add(path);

    return ret;
}
