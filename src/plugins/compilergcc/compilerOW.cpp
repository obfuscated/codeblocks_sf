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
#include "compilerOW.h"
#include <wx/intl.h>
#include <wx/regex.h>
#include <wx/config.h>
#include <wx/utils.h>
#include <wx/msw/registry.h>

#include <logmanager.h>
#include <manager.h>
#include "compilerOWgenerator.h"

#include <wx/utils.h>
#include <wx/filefn.h>

CompilerOW::CompilerOW()
    : Compiler(wxT("OpenWatcom (W32) Compiler"), _T("ow"))
{
    Reset();
}

CompilerOW::~CompilerOW()
{
	//dtor
}

Compiler * CompilerOW::CreateCopy()
{
    Compiler* c = new CompilerOW(*this);
    c->SetExtraPaths(m_ExtraPaths); // wxArrayString doesn't seem to be copied with the default copy ctor...
    return c;
}

CompilerCommandGenerator* CompilerOW::GetCommandGenerator()
{
    return new CompilerOWGenerator;
}

void CompilerOW::Reset()
{
    /*
     *  Define compiler suite programs. I have chosen to use wcl386 for all
     *  to provide as consistent a set of options as possible.
     */

	m_Programs.C                = wxT("wcl386.exe");
	m_Programs.CPP              = wxT("wcl386.exe");
	m_Programs.LD               = wxT("wlink.exe");
	m_Programs.LIB              = wxT("wlib.exe");
	m_Programs.WINDRES          = wxT("wrc.exe");
	m_Programs.MAKE             = wxT("wmake.exe");

	m_Switches.includeDirs      = wxT("-i");
	m_Switches.libDirs          = wxT("LIBP ");
	m_Switches.linkLibs         = wxT("");
	m_Switches.libPrefix        = wxT("");
	m_Switches.libExtension     = wxT("lib");
	m_Switches.defines          = wxT("-d");
	m_Switches.genericSwitch    = wxT("-");
	m_Switches.objectExtension  = wxT("obj");

	m_Switches.needDependencies = false;
	m_Switches.forceCompilerUseQuotes = false;
	m_Switches.forceLinkerUseQuotes = false;
	m_Switches.logging = clogSimple;
	m_Switches.linkerNeedsLibPrefix = false;
	m_Switches.linkerNeedsLibExtension = true;

    m_Options.ClearOptions();
//	m_Options.AddOption(_("Produce debugging symbols"),
//				"-g",
//				_("Debugging"),
//				"",
//				true,
//				"-o -o+space",
//				_("You have optimizations enabled. This is Not A Good Thing(tm) when producing debugging symbols..."));


    wxString   category = wxT("General");

    m_Options.AddOption(wxT("treat source files as C code"), wxT("-cc"), category);
    m_Options.AddOption(wxT("treat source files as C++ code"), wxT("-cc++"), category);
    m_Options.AddOption(wxT("ignore the WCL386 environment variable"), wxT("-y"), category);


    category = wxT("Processor options");
    m_Options.AddOption(wxT("386 register calling conventions"), wxT("-3r"), category);
    m_Options.AddOption(wxT("386 stack calling conventions"), wxT("-3s"), category);
    m_Options.AddOption(wxT("486 register calling conventions"), wxT("-4r"), category);
    m_Options.AddOption(wxT("486 stack calling conventions"), wxT("-4s"), category);
    m_Options.AddOption(wxT("Pentium register calling conventions"), wxT("-5r"), category);
    m_Options.AddOption(wxT("Pentium stack calling conventions"), wxT("-5s"), category);
    m_Options.AddOption(wxT("Pentium Pro register call conventions"), wxT("-6r"), category);
    m_Options.AddOption(wxT("Pentium Pro stack call conventions"), wxT("-6s"), category);

    category = wxT("Floating-point processor options");

    m_Options.AddOption(wxT("calls to floating-point library"), wxT("-fpc"), category);
    m_Options.AddOption(wxT("enable Pentium FDIV check"), wxT("-fpd"), category);
    m_Options.AddOption(wxT("inline 80x87 with emulation"), wxT("-fpi"), category);
    m_Options.AddOption(wxT("inline 80x87"), wxT("-fpi87"), category);
    m_Options.AddOption(wxT("use old floating-point conventions"), wxT("-fpr"), category);
    m_Options.AddOption(wxT("generate 287 floating-point code"), wxT("-fp2"), category);
    m_Options.AddOption(wxT("generate 387 floating-point code"), wxT("-fp3"), category);
    m_Options.AddOption(wxT("optimize floating-point for Pentium"), wxT("-fp5"), category);
    m_Options.AddOption(wxT("optimize floating-point for Pentium Pro"), wxT("-fp6"), category);

    category = wxT("Compiler options");

    m_Options.AddOption(wxT("compile and link for DOS"), wxT("-bcl=dos"), category);
    m_Options.AddOption(wxT("compile and link for Linux"), wxT("-bcl=linux"), category);
    m_Options.AddOption(wxT("compile and link for NT (includes Win32)"), wxT("-bcl=nt"), category);
    m_Options.AddOption(wxT("compile and link for OS/2"), wxT("-bcl=os2"), category);
    m_Options.AddOption(wxT("compile and link for QNX"), wxT("-bcl=qnx"), category);
    m_Options.AddOption(wxT("compile and link for Windows"), wxT("-bcl=windows"), category);

    m_Options.AddOption(wxT("compile for DOS"), wxT("-bt=dos"), category);
    m_Options.AddOption(wxT("compile for Linux"), wxT("-bt=linux"), category);
    m_Options.AddOption(wxT("compile for NetWare"), wxT("-bt=netware"), category);
    m_Options.AddOption(wxT("compile for NetWare 5 and later"), wxT("-bt=netware5"), category);
    m_Options.AddOption(wxT("compile for NT (includes Win32)"), wxT("-bt=nt"), category);
    m_Options.AddOption(wxT("compile for OS/2"), wxT("-bt=os2"), category);
    m_Options.AddOption(wxT("compile for QNX"), wxT("-bt=qnx"), category);
    m_Options.AddOption(wxT("compile for Windows"), wxT("-bt=windows"), category);


    m_Options.AddOption(wxT("generate browsing information"), wxT("-db"), category);
    m_Options.AddOption(wxT("set error limit number (set ERROR_LIMIT in custom variables)"), wxT("-e=$(ERROR_LIMIT)"), category);
    m_Options.AddOption(wxT("call epilogue hook routine"), wxT("-ee"), category);
    m_Options.AddOption(wxT("full paths in messages"), wxT("-ef"), category);
    m_Options.AddOption(wxT("force enums to be type int"), wxT("-ei"), category);
    m_Options.AddOption(wxT("minimum base type for enum is int"), wxT("-em"), category);
    m_Options.AddOption(wxT("emit routine names in the code"), wxT("-en"), category);
    m_Options.AddOption(wxT("call prologue hook routine"), wxT("-ep"), category);
    m_Options.AddOption(wxT("call prologue hook routine with n bytes of stack (set PROLOGUE_STACK in custom variables)"), wxT("-ep=$(PROLOGUE_STACK)"), category);
    m_Options.AddOption(wxT("do not display error messages"), wxT("-eq"), category);
    m_Options.AddOption(wxT("P5 profiling"), wxT("-et"), category);
    m_Options.AddOption(wxT("generate PharLap EZ-OMF object"), wxT("-ez"), category);
    m_Options.AddOption(wxT("generate pre-compiled header (set PCH_FILE in custom variables)"), wxT("-fh=$(PCH_FILE)"), category);
    m_Options.AddOption(wxT("generate pre-compiled header (Quiet) (set PCH_FILE in custom variables)"), wxT("-fhq=$(PCH_FILE)"), category);
    m_Options.AddOption(wxT("(C++) only read PCH"), wxT("-fhr"), category);
    m_Options.AddOption(wxT("(C++) only write PCH"), wxT("-fhw"), category);
    m_Options.AddOption(wxT("(C++) don't count PCH warnings"), wxT("-fhwe"), category);

    // This should be a multiple option. We can define multiple force includes
    m_Options.AddOption(wxT("force include of file (define FORCE_INCLUDE in custom variables)"), wxT("-fi=$(FORCE_INCLUDE)"), category);
    // This one is mandatory in the ctCompileObjectCmd
    //m_Options.AddOption(wxT("set object file name"), wxT("-fo=<file>"), category);
    m_Options.AddOption(wxT("set error file name (define ERROR_FILE in custom variables)"), wxT("-fr=$(ERROR_FILE)"), category);
    m_Options.AddOption(wxT("(C++) check for 8.3 file names"), wxT("-ft"), category);
    m_Options.AddOption(wxT("(C++) no check for 8.3 file names"), wxT("-fx"), category);
    m_Options.AddOption(wxT("set code group name (define CODEGROUP in custom variables)"), wxT("-g=$(CODEGROUP)"), category);
    m_Options.AddOption(wxT("codeview debug format"), wxT("-hc"), category);
    m_Options.AddOption(wxT("dwarf debug format"), wxT("-hd"), category);
    m_Options.AddOption(wxT("watcom debug format"), wxT("-hw"), category);
    m_Options.AddOption(wxT("change char default to signed"), wxT("-j"), category);
    m_Options.AddOption(wxT("memory model flat"), wxT("-mf"), category);
    m_Options.AddOption(wxT("memory model small"), wxT("-ms"), category);
    m_Options.AddOption(wxT("memory model medium"), wxT("-mm"), category);
    m_Options.AddOption(wxT("memory model compact"), wxT("-mc"), category);
    m_Options.AddOption(wxT("memory model large"), wxT("-ml"), category);
    m_Options.AddOption(wxT("memory model huge"), wxT("-mh"), category);
    m_Options.AddOption(wxT("set CODE class name (define CODECLASS in custom variables)"), wxT("-nc=$(CODECLASS)"), category);
    m_Options.AddOption(wxT("set data segment name (define DATANAME in custom variables)"), wxT("-nd=$(DATANAME)"), category);
    m_Options.AddOption(wxT("set module name (define MODULENAME in custom variables)"), wxT("-nm=$(MODULENAME)"), category);
    m_Options.AddOption(wxT("set text segment name (define TEXTNAME in custom variables)"), wxT("-nt=$(TEXTNAME)"), category);
    m_Options.AddOption(wxT("save/restore segregs across calls"), wxT("-r"), category);
    m_Options.AddOption(wxT("promote function args/rets to int"), wxT("-ri"), category);
    m_Options.AddOption(wxT("remove stack overflow checks"), wxT("-s"), category);
    m_Options.AddOption(wxT("generate calls to grow the stack"), wxT("-sg"), category);
    m_Options.AddOption(wxT("touch stack through SS first"), wxT("-st"), category);
    m_Options.AddOption(wxT("output func declarations to .def"), wxT("-v"), category);
    m_Options.AddOption(wxT("VC++ compat: alloca allowed in arg lists"), wxT("-vcap"), category);
    m_Options.AddOption(wxT("set warning level to 0 (suppress warnings)"), wxT("=w=0"), category);
    m_Options.AddOption(wxT("set warning level to 1"), wxT("-w=1"), category);
    m_Options.AddOption(wxT("set warning level to 2"), wxT("-w=2"), category);
    m_Options.AddOption(wxT("set warning level to 3"), wxT("-w=3"), category);
    m_Options.AddOption(wxT("disable warning message (define DIS_WARN in custom variables)"), wxT("-wcd=$(DIS_WARN)"), category);
    m_Options.AddOption(wxT("enable warning message (define ENA_WARN in custom variables)"), wxT("-wce=$(ENA_WARN)"), category);
    m_Options.AddOption(wxT("treat all warnings as errors"), wxT("-we"), category);
    m_Options.AddOption(wxT("set warning level to max"), wxT("-wx"), category);
    m_Options.AddOption(wxT("(C++) enable RTTI"), wxT("-xr"), category);
    m_Options.AddOption(wxT("disable language extensions (ANSI/ISO compliance)"), wxT("-za"), category);
    m_Options.AddOption(wxT("enable language extensions"), wxT("-ze"), category);
    m_Options.AddOption(wxT("place strings in CODE segment"), wxT("-zc"), category);
    m_Options.AddOption(wxT("DS not pegged to DGROUP"), wxT("-zdf"), category);
    m_Options.AddOption(wxT("DS pegged to DGROUP"), wxT("-zdp"), category);
    m_Options.AddOption(wxT("load DS directly from DGROUP"), wxT("-zdl"), category);
    m_Options.AddOption(wxT("Allow code-generator to use FS"), wxT("-zff"), category);
    m_Options.AddOption(wxT("Do not allow code-generator to use FS"), wxT("-zfp"), category);
    m_Options.AddOption(wxT("Allow code-generator to use GS"), wxT("-zgf"), category);
    m_Options.AddOption(wxT("Do not allow code-generator to use GS"), wxT("-zgp"), category);
    m_Options.AddOption(wxT("Allow arithmetic on void derived type"), wxT("-zev"), category);
    m_Options.AddOption(wxT("function prototype using base type"), wxT("-zg"), category);
    // Duplicate to -zk0
    //m_Options.AddOption(wxT("Double byte chars in strings (Japanese DBCS)"), wxT("-zk"), category);
    m_Options.AddOption(wxT("Double byte chars in strings (Japanese DBCS)"), wxT("-zk0"), category);
    m_Options.AddOption(wxT("Double byte chars in strings (Japanese DBCS - translate to Unicode)"), wxT("-zk0u"), category);
    m_Options.AddOption(wxT("Double byte chars in strings (Trad Chinese or Taiwanese DBCS)"), wxT("-zk1"), category);
    m_Options.AddOption(wxT("Double byte chars in strings (Korean Hangeul) DBCS)"), wxT("-zk2"), category);
    m_Options.AddOption(wxT("Double byte chars in strings (Use current code page)"), wxT("-zkl"), category);
    m_Options.AddOption(wxT("Translate characters to Unicode (specify UNI_CP in custom variables)"), wxT("-zku=$(UNI_CP)"), category);
    m_Options.AddOption(wxT("remove default library information"), wxT("-zl"), category);
    m_Options.AddOption(wxT("remove file dependency information"), wxT("-zld"), category);
    m_Options.AddOption(wxT("place functions in separate segments"), wxT("-zm"), category);
    m_Options.AddOption(wxT("(C++) zm with near calls allowed"), wxT("-zmf"), category);
    m_Options.AddOption(wxT("struct packing align 1 byte"), wxT("-zp1"), category);
    m_Options.AddOption(wxT("struct packing align 2 byte"), wxT("-zp2"), category);
    m_Options.AddOption(wxT("struct packing align 4 byte"), wxT("-zp4"), category);
    m_Options.AddOption(wxT("struct packing align 8 byte"), wxT("-zp8"), category);
    m_Options.AddOption(wxT("struct packing align 16 byte"), wxT("-zp16"), category);
    m_Options.AddOption(wxT("warning when padding a struct"), wxT("-zpw"), category);
    m_Options.AddOption(wxT("operate quietly"), wxT("-zq"), category);
    m_Options.AddOption(wxT("check syntax only"), wxT("-zs"), category);
    m_Options.AddOption(wxT("set data threshold (set DATA_THRESHOLD in custom variables)"), wxT("-zt=$(DATA_THRESHOLD)"), category);
    m_Options.AddOption(wxT("Do not assume SS contains DGROUP"), wxT("-zu"), category);
    m_Options.AddOption(wxT("(C++) enable virt. fun. removal opt"), wxT("-zv"), category);
    m_Options.AddOption(wxT("generate code for MS Windows"), wxT("-zw"), category);
    m_Options.AddOption(wxT("remove @size from __stdcall func."), wxT("-zz"), category);

    category = wxT("Debugging options");

    m_Options.AddOption(wxT("no debugging information"), wxT("-d0"), category);
    m_Options.AddOption(wxT("line number debugging information"), wxT("-d1"), category);
    m_Options.AddOption(wxT("(C) line number debugging information plus typing information for global symbols and local structs and arrays"), wxT("-d1+"), category);
    m_Options.AddOption(wxT("full symbolic debugging information"), wxT("-d2"), category);
    m_Options.AddOption(wxT("(C++) d2 and debug inlines; emit inlines as external out-of-line functions"), wxT("-d2i"), category);
    m_Options.AddOption(wxT("(C++) d2 and debug inlines; emit inlines as static out-of-line functions"), wxT("-d2s"), category);
    m_Options.AddOption(wxT("(C++) d2 but without type names"), wxT("-d2t"), category);
    m_Options.AddOption(wxT("full symbolic debugging with unreferenced type names"), wxT("-d3"), category);
    m_Options.AddOption(wxT("(C++) d3 plus debug inlines; emit inlines as external out-of-line functions"), wxT("-d3i"), category);
    m_Options.AddOption(wxT("(C++) d3 plus debug inlines; emit inlines as static out-of-line functions"), wxT("-d3s"), category);

    category = wxT("Optimization options");

    m_Options.AddOption(wxT("relax alias checking"), wxT("-oa"), category);
    m_Options.AddOption(wxT("branch prediction"), wxT("-ob"), category);
    m_Options.AddOption(wxT("disable call/ret optimization"), wxT("-oc"), category);
    m_Options.AddOption(wxT("disable optimizations"), wxT("-od"), category);
    m_Options.AddOption(wxT("expand functions inline (specify INLINE_NUM in custom variables)"), wxT("-oe=$(INLINE_NUM)"), category);
    m_Options.AddOption(wxT("generate traceable stack frames"), wxT("-of"), category);
    m_Options.AddOption(wxT("always generate traceable stack frames"), wxT("-of+"), category);
    m_Options.AddOption(wxT("enable repeated optimizations"), wxT("-oh"), category);
    m_Options.AddOption(wxT("inline intrinsic functions"), wxT("-oi"), category);
    m_Options.AddOption(wxT("(C++) oi with max inlining depth"), wxT("-oi+"), category);
    m_Options.AddOption(wxT("control flow entry/exit sequence"), wxT("-ok"), category);
    m_Options.AddOption(wxT("perform loop optimizations"), wxT("-ol"), category);
    m_Options.AddOption(wxT("ol with loop unrolling"), wxT("-ol+"), category);
    m_Options.AddOption(wxT("generate inline math functions"), wxT("-om"), category);
    m_Options.AddOption(wxT("numerically unstable floating-point"), wxT("-on"), category);
    m_Options.AddOption(wxT("continue compile when low on memory"), wxT("-oo"), category);
    m_Options.AddOption(wxT("improve floating-point consistency"), wxT("-op"), category);
    m_Options.AddOption(wxT("re-order instructions to avoid stalls"), wxT("-or"), category);
    m_Options.AddOption(wxT("optimize for space"), wxT("-os"), category);
    m_Options.AddOption(wxT("optimize for time"), wxT("-ot"), category);
    m_Options.AddOption(wxT("ensure unique addresses for functions"), wxT("-ou"), category);
    m_Options.AddOption(wxT("maximum optimization (-obmiler -s)"), wxT("-ox"), category);

    category = wxT("C++ exception handling options");

    m_Options.AddOption(wxT("no exception handling"), wxT("-xd"), category);
    m_Options.AddOption(wxT("no exception handling: space"), wxT("-xds"), category);
    // duplicate to -xd
    //m_Options.AddOption(wxT("no exception handling"), wxT("-xdt"), category);
    m_Options.AddOption(wxT("exception handling: balanced"), wxT("-xs"), category);
    m_Options.AddOption(wxT("exception handling: space"), wxT("-xss"), category);
    m_Options.AddOption(wxT("exception handling: time"), wxT("-xst"), category);

    category = wxT("Preprocessor options");

    //  defined in m_Switches.defines
    //  m_Options.AddOption(wxT("Define macro"), wxT("-d"), category);
    // difficult to support
    //  m_Options.AddOption(wxT("Extend -d syntax"), wxT(""), category);
    //  This one is mandatory in the ctCompileObjectCmd
    //  m_Options.AddOption(wxT("set object file name"), wxT("-fo=<file>"), category);
    //  Specified by m_Switches.includeDirs
    //  m_Options.AddOption(wxT("include directory"), wxT("-i"), category);
    m_Options.AddOption(wxT("number of spaces in tab stop (set TAB_STOP in custom variables)"), wxT("-t=$(TAB_STOP)"), category);
    // multi-option
    //m_Options.AddOption(wxT("set #pragma on"), wxT("-tp=$(PRAGMA_NAMES)"), category);
    // multi-option
    //m_Options.AddOption(wxT("undefine macro name"), wxT("-u"), category);
    /*
     *  options are -pcelw=n
     */
    m_Options.AddOption(wxT("preprocess source file"), wxT("-p"), category);
    m_Options.AddOption(wxT("preprocess source file (preserve comments)"), wxT("-pc"), category);
    m_Options.AddOption(wxT("preprocess source file (insert #line directives)"), wxT("-pl"), category);
    m_Options.AddOption(wxT("(C++) preprocess file (encrypt identifiers)"), wxT("-pe"), category);
    //

    category = wxT("Linker options");

    m_Options.AddOption(wxT("build Dynamic link library"), wxT("-bd"), category);
    m_Options.AddOption(wxT("build Multi-thread application"), wxT("-bm"), category);
    m_Options.AddOption(wxT("build with dll run-time library"), wxT("-br"), category);
    m_Options.AddOption(wxT("build default Windowing appllication"), wxT("-bw"), category);
    m_Options.AddOption(wxT("write directives"), wxT("-fd"), category);
    m_Options.AddOption(wxT("write directives (define DIRECTIVE_FILE in custom variables)"), wxT("-fd=$(DIRECTIVE_FILE)"), category);
    // mandatory in link commands
    //m_Options.AddOption(wxT("name executable file"), wxT("-fe=<file>"), category);
    m_Options.AddOption(wxT("generate map file"), wxT("-fm"), category);
    m_Options.AddOption(wxT("generate map file (define MAP_FILE in custom variables)"), wxT("-fm=$(MAP_FILE)"), category);
    m_Options.AddOption(wxT("set stack size (define STACK_SIZE in custom variables)"), wxT("-k$(STACK_SIZE)"), category);
    m_Options.AddOption(wxT("link for the specified OS (define TARGET_OS in custom variables)"), wxT("-l=$(TARGET_OS)"), category);
    m_Options.AddOption(wxT("make names case sensitive"), wxT("-x"), category);
    m_Options.AddOption(wxT("additional directive file (specify LINK_DIRECTIVES in custom variables)"), wxT("@$(LINK_VARIABLES)"), category);
    // ????
    // m_Options.AddOption(wxT("following parameters are linker options"), wxT("-"), category);


    m_Commands[(int)ctCompileObjectCmd]
       .push_back( CompilerTool(wxT("$compiler -q -c $options $includes -fo=$object $file")) );
    m_Commands[(int)ctCompileResourceCmd]
       .push_back( CompilerTool(wxT("$rescomp -q -r -fo=$resource_output $res_includes $file")) );
    m_Commands[(int)ctLinkExeCmd]
       .push_back( CompilerTool(wxT("$linker option quiet $link_options $libdirs $link_objects name $exe_output $libs $link_resobjects")) );
    m_Commands[(int)ctLinkConsoleExeCmd]
       .push_back( CompilerTool(wxT("$linker option quiet $link_options $libdirs $link_objects name $exe_output $libs $link_resobjects")) );
    m_Commands[(int)ctLinkDynamicCmd]
       .push_back( CompilerTool(wxT("$linker option quiet $link_options $libdirs name $exe_output $libs $link_objects")) );
    m_Commands[(int)ctLinkStaticCmd]
       .push_back( CompilerTool(wxT("$lib_linker -q $static_output $link_objects")) );
    m_Commands[(int)ctLinkNativeCmd] = m_Commands[(int)ctLinkConsoleExeCmd]; // unsupported currently

    LoadDefaultRegExArray();

    m_CompilerOptions.Clear();
    m_LinkerOptions.Clear();
    m_LinkLibs.Clear();
    m_CmdsBefore.Clear();
    m_CmdsAfter.Clear();
}

void CompilerOW::LoadDefaultRegExArray()
{
    m_RegExes.Clear();
    m_RegExes.Add(RegExStruct(_("Note"), cltWarning, wxT("(") + FilePathWithSpaces + _T(")\\(([0-9]+)\\): Note! (.+)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compiler error"), cltError, wxT("(") + FilePathWithSpaces + _T(")\\(([0-9]+)\\): Error! (.+)"), 3, 1, 2));
    m_RegExes.Add(RegExStruct(_("Compiler warning"), cltWarning, wxT("(") + FilePathWithSpaces + _T(")\\(([0-9]+)\\): Warning! (.+)"), 3, 1, 2));
}

AutoDetectResult CompilerOW::AutoDetectInstallationDir()
{
    /* Following code is Not necessary as OpenWatcom does not write to
       Registry anymore */
    /*wxRegKey key; // defaults to HKCR
    key.SetName(wxT("HKEY_LOCAL_MACHINE\\Software\\Open Watcom\\c_1.0"));
    if (key.Open())
        // found; read it
        key.QueryValue(wxT("Install Location"), m_MasterPath);*/

    if (m_MasterPath.IsEmpty())
        // just a guess; the default installation dir
        m_MasterPath = wxT("C:\\watcom");

    if (!m_MasterPath.IsEmpty())
    {
        AddIncludeDir(m_MasterPath + wxFILE_SEP_PATH + wxT("h"));
        AddIncludeDir(m_MasterPath + wxFILE_SEP_PATH + wxT("h") + wxFILE_SEP_PATH + wxT("nt"));
        AddLibDir(m_MasterPath + wxFILE_SEP_PATH + wxT("lib386"));
        AddLibDir(m_MasterPath + wxFILE_SEP_PATH + wxT("lib386") + wxFILE_SEP_PATH + wxT("nt"));
        AddResourceIncludeDir(m_MasterPath + wxFILE_SEP_PATH + wxT("h"));
        AddResourceIncludeDir(m_MasterPath + wxFILE_SEP_PATH + wxT("h") + wxFILE_SEP_PATH + wxT("nt"));
        m_ExtraPaths.Add(m_MasterPath + wxFILE_SEP_PATH + wxT("binnt"));
        m_ExtraPaths.Add(m_MasterPath + wxFILE_SEP_PATH + wxT("binw"));
    }
    wxSetEnv(wxT("WATCOM"), m_MasterPath);

    return wxFileExists(m_MasterPath + wxFILE_SEP_PATH + wxT("binnt") + wxFILE_SEP_PATH + m_Programs.C) ? adrDetected : adrGuessed;
}

void CompilerOW::LoadSettings(const wxString& baseKey)
{
    Compiler::LoadSettings(baseKey);
    wxSetEnv(wxT("WATCOM"), m_MasterPath);
}

void CompilerOW::SetMasterPath(const wxString& path)
{
    Compiler::SetMasterPath(path);
    wxSetEnv(wxT("WATCOM"), m_MasterPath);
}

#endif // __WXMSW__
