/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef GDB_DEBUGGER_COMMANDS_H
#define GDB_DEBUGGER_COMMANDS_H

// get rid of wxWidgets debug ugliness
#ifdef new
    #undef new
#endif

#include <map>

#include <wx/string.h>
#include <wx/regex.h>
#include <wx/tipwin.h>
#include <wx/tokenzr.h>

#include <cbeditor.h>
#include <cbdebugger_interfaces.h>
#include <configmanager.h>
#include <globals.h>
#include <manager.h>
#include <editormanager.h>
#include <infowindow.h>
#include <logmanager.h>
#include <macrosmanager.h>
#include <scriptingmanager.h>
#include <sqplus.h>
#include <scripting/bindings/sc_base_types.h>

#include "debugger_defs.h"
#include "debuggergdb.h"
#include "gdb_driver.h"
#include "remotedebugging.h"
#include "parsewatchvalue.h"

namespace
{
  template <typename T>
  T wxStrHexTo(const wxString &str)
  {
    T ret = 0; // return
    std::size_t count = 0; // how many characters we've converted
    std::size_t pos = 0; // string position

    // if it begins with 0x or 0X, just ignore it
    if (str[pos] == _T('0'))
    {
      ++pos;

      if (str[pos] == _T('x') || str[pos] == _T('X'))
      {
        ++pos; // start after the x or X
      }

      while (str[pos] == _T('0')) // skip all zeros
      {
        ++pos;
      }
    }

    while (count < sizeof(T) * 2) // be sure we don't keep adding more to ret
    {
      #if wxCHECK_VERSION(3, 0, 0)
      switch (str[pos].GetValue())
      #else
      switch (str[pos])
      #endif
      {
        case _T('0'):
        case _T('1'):
        case _T('2'):
        case _T('3'):
        case _T('4'):
        case _T('5'):
        case _T('6'):
        case _T('7'):
        case _T('8'):
        case _T('9'):
          ret <<= 4;
          ret |= str[pos] - _T('0');
          ++count;
          break;

        case _T('a'):
        case _T('b'):
        case _T('c'):
        case _T('d'):
        case _T('e'):
        case _T('f'):
          ret <<= 4;
          ret |= str[pos] - _T('a') + 10;
          ++count;
          break;

        case _T('A'):
        case _T('B'):
        case _T('C'):
        case _T('D'):
        case _T('E'):
        case _T('F'):
          ret <<= 4;
          ret |= str[pos] - _T('A') + 10;
          ++count;
          break;

        default: // whatever we find that doesn't match ends the conversion
          return ret;
      }

      ++pos;
    }

    return ret;
  }
}

//#0 wxEntry () at main.cpp:5
//#8  0x77d48734 in USER32!GetDC () from C:\WINDOWS\system32\user32.dll
//#9  0x001b04fe in ?? ()
//#30 0x00403c0a in WinMain (hInstance=0x400000, hPrevInstance=0x0, lpCmdLine=0x241ef9 "", nCmdShow=10) at C:/Devel/wxSmithTest/app.cpp:297
//#31 0x004076ca in main () at C:/Devel/wxWidgets-2.6.1/include/wx/intl.h:555
//#50  0x00410c8c in one::~one() (this=0x3d24c8) at main.cpp:14
//#11  0x00406810 in main ()
static wxRegEx reBT0(_T("#([0-9]+)[ \t]+(.+)[ \t]at[ \t](.+):([0-9]+)")); // case #0
static wxRegEx reBT1(_T("#([0-9]+)[ \t]+0x([A-Fa-f0-9]+)[ \t]+in[ \t]+(.+)[ \t]+(\\([^)]*\\))[ \t]")); // all other cases (gdb 6.3)
static wxRegEx reBTX(_T("#([0-9]+)[ \t]+0x([A-Fa-f0-9]+)[ \t]+in[ \t]+([^(]+)[ \t]*(\\([^)]*\\)[ \t]*\\([^)]*\\))")); // all other cases (gdb 5.2)
static wxRegEx reBT2(_T("\\)[ \t]+[atfrom]+[ \t]+(.*):([0-9]+)"));
static wxRegEx reBT3(_T("\\)[ \t]+[atfrom]+[ \t]+(.*)"));
static wxRegEx reBT4(_T("#([0-9]+)[ \\t]+(.+)[ \\t]in[ \\t](.+)")); // case #11
// Breakpoint 1 at 0x4013d6: file main.cpp, line 8.
static wxRegEx reBreakpoint(_T("Breakpoint ([0-9]+) at (0x[0-9A-Fa-f]+)"));
// GDB7.4 and before will return:
// Breakpoint 1 ("/home/jens/codeblocks-build/codeblocks-1.0svn/src/plugins/debuggergdb/gdb_commands.h:125) pending.
// GDB7.5 and later will return:
// Breakpoint 4 ("E:/code/cb/test_code/DebugDLLTest/TestDLL/dllmain.cpp:29") pending.
static wxRegEx rePendingBreakpoint(_T("Breakpoint ([0-9]+)[ \t]\\(\"(.+):([0-9]+)(\"?)\\)[ \t]pending\\."));
// Hardware assisted breakpoint 1 at 0x4013d6: file main.cpp, line 8.
static wxRegEx reHWBreakpoint(_T("Hardware assisted breakpoint ([0-9]+) at (0x[0-9A-Fa-f]+)"));
// Hardware watchpoint 1: expr
static wxRegEx reDataBreakpoint(_T("Hardware watchpoint ([0-9]+):.*"));
// Temporary breakpoint 2 at 0x401203: file /home/obfuscated/projects/tests/_cb_dbg/watches/main.cpp, line 115.
static wxRegEx reTemporaryBreakpoint(wxT("^[Tt]emporary[ \t]breakpoint[ \t]([0-9]+)[ \t]at.*"));
// eax            0x40e66666       1088841318
static wxRegEx reRegisters(_T("([A-z0-9]+)[ \t]+(0x[0-9A-Fa-f]+)[ \t]+(.*)"));
// wayne registers
//static wxRegEx reRegisters(_T("(R[0-9]+)[ \t]+(0x[0-9A-Fa-f]+)"));
// 0x00401390 <main+0>:    push   ebp
static wxRegEx reDisassembly(_T("(0x[0-9A-Za-z]+)[ \t]+<.*>:[ \t]+(.*)"));
// 9           if(argc > 1)
// 10              strcpy(filename, argv[1]) ;
// 11          else
// 12          strcpy(filename, "c:\\dev\\wxwidgets\\wxWidgets-2.8.10\\build\\msw\\../../src/something.c") ;
static wxRegEx reDisassemblySource(_T("([0-9]+)[ \t](.*)"));
//Stack level 0, frame at 0x22ff80:
// eip = 0x401497 in main (main.cpp:16); saved eip 0x4011e7
// source language c++.
// Arglist at 0x22ff78, args: argc=1, argv=0x3e3cb0
// Locals at 0x22ff78, Previous frame's sp is 0x22ff80
// Saved registers:
//  ebx at 0x22ff6c, ebp at 0x22ff78, esi at 0x22ff70, edi at 0x22ff74, eip at 0x22ff7c
static wxRegEx reDisassemblyInit(_T("^[ \t]*Stack level [0-9]+, frame at (0x[A-Fa-f0-9]+):"));
//  rip = 0x400931 in Bugtest<int> (/src/_cb_dbg/disassembly/main.cpp:6);
static wxRegEx reDisassemblyInitSymbol(_T("[ \t]*[er]ip[ \t]+=[ \t]+0x[0-9a-f]+[ \t]+in[ \t]+(.+)\\((.+):([0-9]+)\\);"));
static wxRegEx reDisassemblyInitFunc(_T("eip = (0x[A-Fa-f0-9]+) in ([^;]*)"));
// or32 variant
#ifdef __WXMSW__
static wxRegEx reDisassemblyInitFuncOR32(_T("PC = (0x[A-Fa-f0-9]+) in ([^;]*)"));
#else
// not used on linux, but make sure it exists otherwise compilation fails on linux
// if(platform::windows && m_disassemblyFlavor == _T("set disassembly-flavor or32")) blabla
static wxRegEx reDisassemblyInitFuncOR32(_T("PC = (0x[A-Fa-f0-9]+) in ([^;]*)"));
#endif
static wxRegEx reDisassemblyCurPC(_T("=>[ \t]+(0x[A-Fa-f0-9]+)"));
//    Using the running image of child Thread 46912568064384 (LWP 7051).
static wxRegEx reInfoProgramThread(_T("\\(LWP[ \t]([0-9]+)\\)"));
//    Using the running image of child process 10011.
static wxRegEx reInfoProgramProcess(_T("child process ([0-9]+)"));
//  2 Thread 1082132832 (LWP 8017)  0x00002aaaac5a2aca in pthread_cond_wait@@GLIBC_2.3.2 () from /lib/libpthread.so.0
//* 1 Thread 46912568064384 (LWP 7926)  0x00002aaaac76e612 in poll () from /lib/libc.so.6
static wxRegEx reInfoThreads(_T("(\\**)[ \t]*([0-9]+)[ \t](.*)"));
static wxRegEx reGenericHexAddress(_T("(0x[A-Fa-f0-9]+)"));

static wxRegEx reExamineMemoryLine(wxT("[ \t]*(0x[0-9a-f]+)[ \t]<.+>:[ \t]+(.+)"));

//mi output from 'nexti' is:
//"^Z^Zc:\dev\cb_exp\cb_debugviz\panels.cpp:409:12533:middle:0x4044f5"
//That's tough - guessing that path may/not always be full, i.e. might be
//only relative path sometimes(?), windows has possibility of
//the ':' in drive specifier...  The "middle" can also be "beg"...

//how to handle path that may include alternate data streams - appears easy with MI
//interface, not with (now current) CLI interface!!!
//This might handle the (windows only) leading drive prefix, but not embedded colons ini path!!!
//"\x1a\x1a(([a-zA-Z]:)?.*?):([0-9]*):([0-9]*):(.*?):(.*)"
//I've seen 'middle' and 'beg', but what if there's not line number info available?Hmm, doesn't
//appear to be another option of any sort for that (gdb annotate_source()
//"\x1a\x1a(([a-zA-Z]:)?.*?):([0-9]*):([0-9]*):(middle|beg|):(.*)"
//static wxRegEx reStepI(_T("\x1a\x1a.*?:([0-9]*):([0-9]*):(.*?):(.*)"));
//static wxRegEx reStepI(_T("\x1a\x1a(([a-zA-Z]:)?.*?):([0-9]*):([0-9]*):(middle|beg):(.*)"));
static wxRegEx reStepI(wxT("(((.*)[a-zA-Z]:)?.*)?:(\\d+):(middle|beg):(.*)"),
#ifndef __WXMAC__
                       wxRE_ADVANCED);
#else
                       wxRE_EXTENDED);
#endif
static wxRegEx reStepI2(_T("\\A(0x[A-Fa-f0-9]+)\\s+(\\d+)\\s+in (.*)"),
#ifndef __WXMAC__
                       wxRE_ADVANCED);
#else
                       wxRE_EXTENDED);
#endif
static wxRegEx reStepI3(_T("^(0x[A-Fa-f0-9]+) in (.*)? from (.*)"));
static wxRegEx reStepI4(_T("^(0x[A-Fa-f0-9]+) in (.*)? at (.*)"));

static wxRegEx reNextI(_T("\x1a\x1a(([a-zA-Z]:)?.*?):([0-9]*):([0-9]*):(middle|beg):(.*)"),
#ifndef __WXMAC__
                       wxRE_ADVANCED);
#else
                       wxRE_EXTENDED);
#endif

/**
  * Command to add a search directory for source files in debugger's paths.
  */
class GdbCmd_AddSourceDir : public DebuggerCmd
{
    public:
        /** If @c dir is empty, resets all search dirs to $cdir:$cwd, the default. */
        GdbCmd_AddSourceDir(DebuggerDriver* driver, const wxString& dir)
            : DebuggerCmd(driver)
        {
            m_Cmd << _T("directory ") << dir;
        }
        void ParseOutput(const wxString& output)
        {
            // Output:
            // Warning: C:\Devel\tmp\console\111: No such file or directory.
            // Source directories searched: <dir>;$cdir;$cwd
            if (output.StartsWith(_T("Warning: ")))
                m_pDriver->Log(output.BeforeFirst(_T('\n')));
        }
};

/**
  * Command to the set the file to be debugged.
  */
class GdbCmd_SetDebuggee : public DebuggerCmd
{
    public:
        /** @param file The file to debug. */
        GdbCmd_SetDebuggee(DebuggerDriver* driver, const wxString& file)
            : DebuggerCmd(driver)
        {
            m_Cmd << _T("file ") << file;
        }
        void ParseOutput(const wxString& output)
        {
            // Output:
            // Reading symbols from C:\Devel\tmp\console/console.exe...done.
            // or if it doesn't exist:
            // console.exe: No such file or directory.

            // just log everything before the prompt
            m_pDriver->Log(output.BeforeFirst(_T('\n')));
        }
};

/**
  * Command to the add symbol files.
  */
class GdbCmd_AddSymbolFile : public DebuggerCmd
{
    public:
        /** @param file The file which contains the symbols. */
        GdbCmd_AddSymbolFile(DebuggerDriver* driver, const wxString& file)
            : DebuggerCmd(driver)
        {
            m_Cmd << _T("add-symbol-file ") << file;
        }
        void ParseOutput(const wxString& output)
        {
            // Output:
            //
            // add symbol table from file "console.exe" at
            // Reading symbols from C:\Devel\tmp\console/console.exe...done.
            //
            // or if it doesn't exist:
            // add symbol table from file "console.exe" at
            // console.exe: No such file or directory.

            // just ignore the "add symbol" line and log the rest before the prompt
            m_pDriver->Log(output.AfterFirst(_T('\n')).BeforeLast(_T('\n')));
        }
};

/**
  * Command to set the arguments to the debuggee.
  */
class GdbCmd_SetArguments : public DebuggerCmd
{
    public:
        /** @param file The file which contains the symbols. */
        GdbCmd_SetArguments(DebuggerDriver* driver, const wxString& args)
            : DebuggerCmd(driver)
        {
            m_Cmd << _T("set args ") << args;
        }
        void ParseOutput(cb_unused const wxString& output)
        {
            // No output
        }
};

/**
  * Command to the attach to a process.
  */
class GdbCmd_AttachToProcess : public DebuggerCmd
{
    public:
        /** @param file The file to debug. */
        GdbCmd_AttachToProcess(DebuggerDriver* driver, int pid)
            : DebuggerCmd(driver)
        {
            m_Cmd << _T("attach ") << wxString::Format(_T("%d"), pid);
            m_pDriver->Log(wxString::Format(_("Attaching to program with pid: %d"), pid));
        }
        void ParseOutput(const wxString& output)
        {
            // Output:
            // Attaching to process <pid>
            // or,
            // Can't attach to process.
            wxArrayString lines = GetArrayFromString(output, _T('\n'));
            for (unsigned int i = 0; i < lines.GetCount(); ++i)
            {
                if (lines[i].StartsWith(_T("Attaching")))
                    m_pDriver->Log(lines[i]);
                else if (lines[i].StartsWith(wxT("Can't "))
                         || lines[i].StartsWith(wxT("Could not attach to process"))
                         || lines[i].StartsWith(wxT("ptrace: No such process")))
                {
                    // log this and quit debugging
                    m_pDriver->Log(_("Attaching failed: ")+lines[i]);
                    m_pDriver->QueueCommand(new DebuggerCmd(m_pDriver, _T("quit")));
                }
//                m_pDriver->DebugLog(lines[i]);
            }
        }
};

/**
  * Command to the detach from the process.
  */
class GdbCmd_Detach : public DebuggerCmd
{
    public:
        /** @param file The file to debug. */
        GdbCmd_Detach(DebuggerDriver* driver)
            : DebuggerCmd(driver)
        {
            m_Cmd << _T("detach");
        }
        void ParseOutput(const wxString& output)
        {
            // Output:
            // Attaching to process <pid>
            wxArrayString lines = GetArrayFromString(output, _T('\n'));
            for (unsigned int i = 0; i < lines.GetCount(); ++i)
            {
                if (lines[i].StartsWith(_T("Detaching")))
                    m_pDriver->Log(lines[i]);
//                m_pDriver->DebugLog(lines[i]);
            }
        }
};

/**
  * Utility command to set a breakpoint condition.
  * Catches errors when setting an invalid condition and responds.
  *
  * (called by GdbCmd_AddBreakpoint)
  */
class GdbCmd_AddBreakpointCondition : public DebuggerCmd
{
        cb::shared_ptr<DebuggerBreakpoint> m_BP;
    public:
        /** @param bp The breakpoint to set its condition. */
        GdbCmd_AddBreakpointCondition(DebuggerDriver* driver, cb::shared_ptr<DebuggerBreakpoint> bp)
            : DebuggerCmd(driver),
            m_BP(bp)
        {
            m_Cmd << _T("condition ") << wxString::Format(_T("%ld"), (int) m_BP->index);
            if (m_BP->useCondition)
                m_Cmd << _T(" ") << m_BP->condition;
        }
        void ParseOutput(const wxString& output)
        {
            if (output.StartsWith(_T("No symbol ")))
            {
                wxString s = wxString::Format(_("While setting up custom conditions for breakpoint %ld (%s, line %d),\n"
                                                "the debugger responded with the following error:\n"
                                                "\nError: %s\n\n"
                                                "Do you want to make this an un-conditional breakpoint?"),
                                                m_BP->index,
                                                m_BP->filename.c_str(),
                                                m_BP->line + 1,
                                                output.c_str());
                if (cbMessageBox(s, _("Warning"), wxICON_WARNING | wxYES_NO) == wxID_YES)
                {
                    // re-run this command but without a condition
                    m_BP->useCondition = false;
                    m_pDriver->QueueCommand(new GdbCmd_AddBreakpointCondition(m_pDriver, m_BP), DebuggerDriver::High);
                }
                else if ( m_BP->alreadySet )
                {
                    m_pDriver->RemoveBreakpoint(m_BP);
                    ((cbEditor*)Manager::Get()->GetEditorManager()->GetActiveEditor())->SetDebugLine(-1);
                    m_pDriver->Continue();
                }
            }

        }
};

/**
  * Command to add a breakpoint.
  */
class GdbCmd_AddBreakpoint : public DebuggerCmd
{
        cb::shared_ptr<DebuggerBreakpoint> m_BP;
    public:
        /** @param bp The breakpoint to set. */
        GdbCmd_AddBreakpoint(DebuggerDriver* driver, cb::shared_ptr<DebuggerBreakpoint> bp)
            : DebuggerCmd(driver),
            m_BP(bp)
        {
            // gdb doesn't allow setting the bp number.
            // instead, we must read it back in ParseOutput()...
            m_BP->index = -1;

            if (m_BP->enabled)
            {
                if (m_BP->type == DebuggerBreakpoint::bptCode)//m_BP->func.IsEmpty())
                {
                    wxString out = m_BP->filename;
                    // we add one to line,  because scintilla uses 0-based line numbers, while gdb uses 1-based
                    if (!m_BP->temporary)
                        m_Cmd << _T("break ");
                    else
                        m_Cmd << _T("tbreak ");
                    m_Cmd << _T('"') << out << _T(":") << wxString::Format(_T("%d"), m_BP->line) << _T('"');
                }
                else if (m_BP->type == DebuggerBreakpoint::bptData)
                {
                    if (m_BP->breakOnRead && m_BP->breakOnWrite)
                        m_Cmd << _T("awatch ");
                    else if (m_BP->breakOnRead)
                        m_Cmd << _T("rwatch ");
                    else
                        m_Cmd << _T("watch ");
                    m_Cmd << m_BP->breakAddress;
                }
                //GDB workaround
                //Use function name if this is C++ constructor/destructor
                else
                {
//                    if (m_BP->temporary)
//                        cbThrow(_T("Temporary breakpoint on constructor/destructor is not allowed"));
                    m_Cmd << _T("rbreak ") << m_BP->func;
                }
                //end GDB workaround

                m_BP->alreadySet = true;
                // condition and ignore count will be set in ParseOutput, where we 'll have the bp number
            }
        }
        void ParseOutput(const wxString& output)
        {
            // possible outputs (we 're only interested in 1st and 2nd samples):
            //
            // Hardware watchpoint 1: expr
            // Breakpoint 1 at 0x4013d6: file main.cpp, line 8.
            // No line 100 in file "main.cpp".
            // No source file named main2.cpp.
            if (reBreakpoint.Matches(output))
            {
//                m_pDriver->DebugLog(wxString::Format(_("Breakpoint added: file %s, line %d"), m_BP->filename.c_str(), m_BP->line + 1));
                if (!m_BP->func.IsEmpty())
                    m_pDriver->Log(_("GDB workaround for constructor/destructor breakpoints activated."));

                reBreakpoint.GetMatch(output, 1).ToLong(&m_BP->index);
                reBreakpoint.GetMatch(output, 2).ToULong(&m_BP->address, 16);

                // conditional breakpoint
                if (m_BP->useCondition && !m_BP->condition.IsEmpty())
                {
                    m_pDriver->QueueCommand(new GdbCmd_AddBreakpointCondition(m_pDriver, m_BP), DebuggerDriver::High);
                }

                // ignore count
                if (m_BP->useIgnoreCount && m_BP->ignoreCount > 0)
                {
                    wxString cmd;
                    cmd << _T("ignore ") << wxString::Format(_T("%d"), (int) m_BP->index) << _T(" ") << wxString::Format(_T("%d"), (int) m_BP->ignoreCount);
                    m_pDriver->QueueCommand(new DebuggerCmd(m_pDriver, cmd), DebuggerDriver::High);
                }
            }
            else if (rePendingBreakpoint.Matches(output))
            {
                if (!m_BP->func.IsEmpty())
                    m_pDriver->Log(_("GDB workaround for constructor/destructor breakpoints activated."));

                rePendingBreakpoint.GetMatch(output, 1).ToLong(&m_BP->index);

                // conditional breakpoint
                // condition can not be evaluated for pending breakpoints, so we only set a flag and do this later
                if (m_BP->useCondition && !m_BP->condition.IsEmpty())
                {
                    m_BP->wantsCondition = true;
                }

                // ignore count
                if (m_BP->useIgnoreCount && m_BP->ignoreCount > 0)
                {
                    wxString cmd;
                    cmd << _T("ignore ") << wxString::Format(_T("%d"), (int) m_BP->index) << _T(" ") << wxString::Format(_T("%d"), (int) m_BP->ignoreCount);
                    m_pDriver->QueueCommand(new DebuggerCmd(m_pDriver, cmd), DebuggerDriver::High);
                }
            }
            else if (reDataBreakpoint.Matches(output))
            {
                reDataBreakpoint.GetMatch(output, 1).ToLong(&m_BP->index);
            }
            else if (reHWBreakpoint.Matches(output))
            {
                reHWBreakpoint.GetMatch(output, 1).ToLong(&m_BP->index);
                reHWBreakpoint.GetMatch(output, 2).ToULong(&m_BP->address, 16);
            }
            else if (reTemporaryBreakpoint.Matches(output))
                reTemporaryBreakpoint.GetMatch(output, 1).ToLong(&m_BP->index);
            else
                m_pDriver->Log(output); // one of the error responses

            Manager::Get()->GetDebuggerManager()->GetBreakpointDialog()->Reload();
        }
};

/**
  * Command to add a data breakpoint.
  */
class GdbCmd_AddDataBreakpoint : public DebuggerCmd
{
        cb::shared_ptr<DebuggerBreakpoint> m_BP;
    public:
        /** @param bp The breakpoint to set. */
        GdbCmd_AddDataBreakpoint(DebuggerDriver* driver, cb::shared_ptr<DebuggerBreakpoint> bp)
            : DebuggerCmd(driver),
            m_BP(bp)
        {
            if (m_BP->enabled)
                m_Cmd << _T("output &") << m_BP->breakAddress;
        }
        void ParseOutput(const wxString& output)
        {
            // Hardware watchpoint 1: expr
            if (output.StartsWith(_T("No symbol ")) || output.StartsWith(_T("Attempt to ")))
                m_pDriver->Log(output);
            else
            {
                if (reGenericHexAddress.Matches(output))
                {
                    wxString contents = reGenericHexAddress.GetMatch(output, 1);
                    m_BP->breakAddress = _T("*") + contents;
                    DebuggerManager *dbgManager = Manager::Get()->GetDebuggerManager();
                    dbgManager->GetBreakpointDialog()->Reload();

                    m_pDriver->QueueCommand(new GdbCmd_AddBreakpoint(m_pDriver, m_BP), DebuggerDriver::High);
                }
            }
        }
};

/**
  * Command to remove a breakpoint.
  */
class GdbCmd_RemoveBreakpoint : public DebuggerCmd
{
    public:
        /** @param bp The breakpoint to remove. If NULL, all breakpoints are removed. */
        GdbCmd_RemoveBreakpoint(DebuggerDriver* driver, cb::shared_ptr<DebuggerBreakpoint> bp)
            : DebuggerCmd(driver),
            m_BP(bp)
        {
            if (!bp)
            {
                m_Cmd << _T("delete breakpoints");
                return;
            }
            if (bp->index >= 0)
            {
                m_Cmd << _T("delete breakpoints ") << wxString::Format(_T("%d"), (int) bp->index);
            }
        }
        void ParseOutput(const wxString& output)
        {
            if (!m_BP)
                return;

            // This can crash because m_BP could already be deleted
            // and if it isn't deleted already, it will be soon
            // so there's no point in invalidating the bp number anyway

            // invalidate bp number
//            m_BP->index = -1;

            if (!output.IsEmpty())
                m_pDriver->Log(output);
//            m_pDriver->DebugLog(wxString::Format(_("Breakpoint removed: file %s, line %d"), m_BP->filename.c_str(), m_BP->line + 1));
        }

        cb::shared_ptr<DebuggerBreakpoint> m_BP;
};

/**
  * Command to setup an exception breakpoint (for a throw or a catch).
  */
class GdbCmd_SetCatch : public DebuggerCmd
{
        wxString m_type;
        int *m_resultIndex;
        wxRegEx m_regExp;
    public:
        GdbCmd_SetCatch(DebuggerDriver *driver, const wxString &type, int *resultIndex) :
            DebuggerCmd(driver),
            m_type(type),
            m_resultIndex(resultIndex),
            m_regExp(wxT("^Catchpoint[ \\t]([0-9]+)[ \\t]\\(") + type + wxT("\\)$"), wxRE_ADVANCED)
        {
            m_Cmd = wxT("catch ") + type;
        }

        void ParseOutput(const wxString& output)
        {
            if (m_regExp.Matches(output))
            {
                long index;
                m_regExp.GetMatch(output, 1).ToLong(&index);
                *m_resultIndex = index;
            }
        }
};


/**
  * Command that notifies the debugger plugin that the debuggee has been continued
  */
class GdbCmd_Continue : public DebuggerContinueBaseCmd
{
    public:
        GdbCmd_Continue(DebuggerDriver* driver) :
            DebuggerContinueBaseCmd(driver, wxT("cont"))
        {
        }

        virtual void Action()
        {
            m_pDriver->NotifyDebuggeeContinued();
        }
};

class GdbCmd_Start : public DebuggerContinueBaseCmd
{
    public:
        GdbCmd_Start(DebuggerDriver* driver, const wxString &cmd) :
            DebuggerContinueBaseCmd(driver, cmd)
        {
        }

        virtual void ParseOutput(const wxString &output)
        {
            const wxArrayString &lines = GetArrayFromString(output, _T('\n'));
            for (size_t ii = 0; ii < lines.GetCount(); ++ii)
            {
                if (   lines[ii].StartsWith(wxT("No symbol table loaded"))
                    || lines[ii].StartsWith(wxT("No executable file specified"))
                    || lines[ii].StartsWith(wxT("No executable specified"))
                    || lines[ii].StartsWith(wxT("Don't know how to run")))
                {
                    // log this and quit debugging
                    m_pDriver->Log(_("Starting the debuggee failed: ")+lines[ii]);
                    m_pDriver->MarkProgramStopped(true);
                    m_pDriver->QueueCommand(new DebuggerCmd(m_pDriver, _T("quit")));
                }
            }
        }
};

/**
  * Command to get info about current program and state.
  */
class GdbCmd_InfoProgram : public DebuggerCmd
{
    public:
        GdbCmd_InfoProgram(DebuggerDriver* driver)
            : DebuggerCmd(driver)
        {
            m_Cmd << _T("info program");
        }
        void ParseOutput(const wxString& output)
        {
            wxString pid_str;
            if (reInfoProgramThread.Matches(output))
                pid_str = reInfoProgramThread.GetMatch(output, 1);
            else if (reInfoProgramProcess.Matches(output))
                pid_str = reInfoProgramProcess.GetMatch(output, 1);

            if (!pid_str.IsEmpty())
            {
                long pid;
                if (pid_str.ToLong(&pid, 10) && pid != 0)
                    m_pDriver->SetChildPID(pid);
            }
        }
};

/**
  * Command to get info about running threads.
  */
class GdbCmd_Threads : public DebuggerCmd
{
    public:
        /** @param tree The tree to display the args. */
        GdbCmd_Threads(DebuggerDriver* driver)
            : DebuggerCmd(driver)
        {
            m_Cmd << _T("info threads");
        }
        void ParseOutput(const wxString& output)
        {
            m_pDriver->GetThreads().clear();
            wxArrayString lines = GetArrayFromString(output, _T('\n'));
            for (unsigned int i = 0; i < lines.GetCount(); ++i)
            {
//                m_pDriver->Log(lines[i]);
                if (reInfoThreads.Matches(lines[i]))
                {
//                    m_pDriver->Log(_T("MATCH!"));
                    wxString active = reInfoThreads.GetMatch(lines[i], 1);
                    active.Trim(true);
                    active.Trim(false);
                    wxString num = reInfoThreads.GetMatch(lines[i], 2);
                    wxString info = reInfoThreads.GetMatch(lines[i], 3);

                    #if defined(_WIN64)
                    long long int number;
                    num.ToLongLong(&number, 10);
                    #else
                    long number;
                    num.ToLong(&number, 10);
                    #endif

                    DebuggerDriver::ThreadsContainer &threads = m_pDriver->GetThreads();
                    threads.push_back(cb::shared_ptr<cbThread>(new cbThread(!active.empty(), number, info)));
                }
            }
            Manager::Get()->GetDebuggerManager()->GetThreadsDialog()->Reload();
        }
};

/**
  * Command to get info about a watched variable.
  */
class GdbCmd_Watch : public DebuggerCmd
{
        cb::shared_ptr<GDBWatch> m_watch;
        wxString m_ParseFunc;
    public:
        GdbCmd_Watch(DebuggerDriver* driver, cb::shared_ptr<GDBWatch> watch) :
            DebuggerCmd(driver),
            m_watch(watch)
        {
            wxString type;
            wxString symbol;

            m_watch->GetSymbol(symbol);
            m_watch->GetType(type);
            type.Trim(true);
            type.Trim(false);
            m_Cmd = static_cast<GDB_driver*>(m_pDriver)->GetScriptedTypeCommand(type, m_ParseFunc);
            if (m_Cmd.IsEmpty())
            {
                m_Cmd << _T("output ");
                switch (m_watch->GetFormat())
                {
                    case Decimal:       m_Cmd << _T("/d "); break;
                    case Unsigned:      m_Cmd << _T("/u "); break;
                    case Hex:           m_Cmd << _T("/x "); break;
                    case Binary:        m_Cmd << _T("/t "); break;
                    case Char:          m_Cmd << _T("/c "); break;
                    case Float:         m_Cmd << _T("/f "); break;
                    case Last:
                    case Any:
                    case Undefined:
                    default:            break;
                }

                // auto-set array types
                if (!m_watch->IsArray() &&  m_watch->GetFormat() == Undefined && type.Contains(_T('[')))
                    m_watch->SetArray(true);

                if (m_watch->IsArray() && m_watch->GetArrayCount() > 0)
                {
                    m_Cmd << wxT("(") << symbol << wxT(")");
                    m_Cmd << wxString::Format(_T("[%d]@%d"), m_watch->GetArrayStart(), m_watch->GetArrayCount());
                }
                else
                    m_Cmd << symbol;
            }
            else
            {
                try
                {
                    SqPlus::SquirrelFunction<wxString&> f(cbU2C(m_Cmd));
                    m_Cmd = f(type, symbol, m_watch->GetArrayStart(), m_watch->GetArrayCount());
                }
                catch (SquirrelError e)
                {
                    m_Cmd = cbC2U(e.desc);
                }
            }
        }
        void ParseOutput(const wxString& output)
        {
            wxString w;
            if (!m_ParseFunc.IsEmpty())
            {
                try
                {
                    SqPlus::SquirrelFunction<wxString&> f(cbU2C(m_ParseFunc));
                    w << f(output, m_watch->GetArrayStart());
                }
                catch (SquirrelError e)
                {
                    w << cbC2U(e.desc);
                }
            }
            else
                w = output;

            w.Trim(true);
            w.Trim(false);

            if(!ParseGDBWatchValue(m_watch, w))
            {
                wxString symbol;
                m_watch->GetSymbol(symbol);
                wxString const &msg = wxT("Parsing GDB output failed for '") + symbol + wxT("'!");
                m_watch->SetValue(msg);
                Manager::Get()->GetLogManager()->LogError(msg);
            }
        }
};

/**
  * Command to get a watched variable's type.
  */
class GdbCmd_FindWatchType : public DebuggerCmd
{
        cb::shared_ptr<GDBWatch> m_watch;
        bool m_firstTry;
    public:
        GdbCmd_FindWatchType(DebuggerDriver* driver, cb::shared_ptr<GDBWatch> watch, bool firstTry = true) :
            DebuggerCmd(driver),
            m_watch(watch),
            m_firstTry(firstTry)
        {
            if (m_firstTry)
                m_Cmd << wxT("whatis ");
            else
                m_Cmd << wxT("whatis &");
            wxString symbol;
            m_watch->GetSymbol(symbol);
            m_Cmd << symbol;
        }
        void ParseOutput(const wxString& output)
        {
            // happens, when wxString is passed as const reference parameter
            if (m_firstTry && output == wxT("Attempt to take contents of a non-pointer value."))
            {
                m_pDriver->QueueCommand(new GdbCmd_FindWatchType(m_pDriver, m_watch, false), DebuggerDriver::High);
                return;
            }
            if (output.StartsWith(wxT("No symbol \"")) && output.EndsWith(wxT("\" in current context.")))
            {
                m_watch->RemoveChildren();
                m_watch->SetType(wxEmptyString);
                m_watch->SetValue(_("Not available in current context!"));
                return;
            }

            // examples:
            // type = wxString
            // type = const wxChar
            // type = Action *
            // type = bool

            wxString tmp = output.AfterFirst(_T('='));
            if (!m_firstTry && !tmp.empty())
                tmp = tmp.substr(0, tmp.length() - 1);

            wxString old_type;
            m_watch->GetType(old_type);
            if(old_type != tmp)
            {
                m_watch->RemoveChildren();
                m_watch->SetType(tmp);
                m_watch->SetValue(wxEmptyString);
            }
            m_pDriver->QueueCommand(new GdbCmd_Watch(m_pDriver, m_watch), DebuggerDriver::High);
        }
};

/**
  * Command to display a tooltip about a variables value.
  */
class GdbCmd_TooltipEvaluation : public DebuggerCmd
{
        wxRect m_WinRect;
        wxString m_What;
        wxString m_Type;
        wxString m_Address;
        wxString m_ParseFunc;
        bool m_autoDereferenced;
    public:
        /** @param what The variable to evaluate.
            @param win A pointer to the tip window pointer.
            @param tiprect The tip window's rect.
        */
        GdbCmd_TooltipEvaluation(DebuggerDriver* driver, const wxString& what, const wxRect& tiprect,
                                 const wxString& w_type = wxEmptyString, const wxString& address = wxEmptyString)
            : DebuggerCmd(driver),
            m_WinRect(tiprect),
            m_What(what),
            m_Type(w_type),
            m_Address(address),
            m_autoDereferenced(false)
        {
            m_Type.Trim(true);
            m_Type.Trim(false);
            m_Cmd = static_cast<GDB_driver*>(m_pDriver)->GetScriptedTypeCommand(w_type, m_ParseFunc);
            if (m_Cmd.IsEmpty())
            {
                /*
                // if it's a pointer, automatically dereference it
                if (w_type.Length() > 2 && // at least 2 chars
                    w_type.Last() == _T('*') && // last is *
                    w_type.GetChar(w_type.Length() - 2) != _T('*') && // second last is not * (i.e. doesn't end with **)
                    !w_type.Contains(_T("char "))) // not char* (special case)
                {
                    m_What = wxT("*") + m_What;
                }*/
                if (IsPointerType(w_type))
                {
                    m_What = wxT("*") + m_What;
                    m_autoDereferenced = true;
                }

                m_Cmd << wxT("output ");
                m_Cmd << m_What;
            }
            else
            {
                try
                {
                    SqPlus::SquirrelFunction<wxString&> f(cbU2C(m_Cmd));
                    m_Cmd = f(w_type, what, 0, 0);
                }
                catch (SquirrelError e)
                {
                    m_Cmd = cbC2U(e.desc);
                    m_pDriver->DebugLog(_T("Script exception: ") + m_Cmd);
                }
            }
        }
        void ParseOutput(const wxString& output)
        {
            wxString contents;
            if (output.StartsWith(_T("No symbol ")) || output.StartsWith(_T("Attempt to ")))
                contents = output;
            else
            {
                if (!m_ParseFunc.IsEmpty())
                {
                    try
                    {
                        SqPlus::SquirrelFunction<wxString&> f(cbU2C(m_ParseFunc));
                        contents << f(output, 0);
                    }
                    catch (SquirrelError e)
                    {
                        contents << cbC2U(e.desc);
                        m_pDriver->DebugLog(_T("Script exception: ") + contents);
                    }
                }
                else
                {
                    contents << output;
                    // the following breaks the text when it *is* a hex number
//                    if (reGenericHexAddress.Matches(output))
//                    {
//                        contents.Replace(reGenericHexAddress.GetMatch(output, 1), _T(""));
//                        contents.Trim(false);
//                    }
                }
            }
            contents.Trim(true);
            contents.Trim(false);

            cb::shared_ptr<GDBWatch> watch(new GDBWatch(m_What));
            watch->SetType(m_Type);

            ParseGDBWatchValue(watch, contents);
            if (!m_Address.empty() && m_autoDereferenced)
            {
                // Add the address of the expression only if the value is empty, this
                // way we won't override the value of the dereferenced expression.
                wxString value;
                watch->GetValue(value);
                if (value.empty())
                    watch->SetValue(m_Address);
                else if (!value.Contains(m_Address))
                    watch->SetValue(m_Address + wxT(": ") + value);
            }
            watch->SetForTooltip(true);
            if (watch->GetChildCount() > 0)
                watch->Expand(true);

            if (Manager::Get()->GetDebuggerManager()->ShowValueTooltip(watch, m_WinRect))
                m_pDriver->GetDebugger()->AddWatchNoUpdate(watch);
        }
};

/**
  * Command to get a symbol's type and use it for tooltip evaluation.
  */
class GdbCmd_FindTooltipAddress : public DebuggerCmd
{
        wxRect m_WinRect;
        wxString m_What;
        wxString m_Type;
    public:
        /** @param tree The tree to display the watch. */
        GdbCmd_FindTooltipAddress(DebuggerDriver* driver, const wxString& what, const wxRect& tiprect, const wxString& w_type = wxEmptyString)
            : DebuggerCmd(driver),
            m_WinRect(tiprect),
            m_What(what),
            m_Type(w_type)
        {
            if (m_Type.IsEmpty())
            {
                m_pDriver->QueueCommand(new GdbCmd_TooltipEvaluation(m_pDriver, m_What, m_WinRect, m_Type), DebuggerDriver::High);
                return;
            }
            m_Cmd << _T("output ");
            if (m_Type.Last() != _T('*'))
                m_Cmd << _T('&');
            m_Cmd << m_What;
        }
        void ParseOutput(const wxString& output)
        {
            // examples:
            // type = wxString
            // type = const wxChar
            // type = Action *
            // type = bool

            wxString tmp;
            if (reGenericHexAddress.Matches(output))
                tmp = reGenericHexAddress.GetMatch(output, 1);

            // add the actual evaluation command with high priority
            m_pDriver->QueueCommand(new GdbCmd_TooltipEvaluation(m_pDriver, m_What, m_WinRect, m_Type, tmp), DebuggerDriver::High);
        }
};

/**
  * Command to get a symbol's type and use it for tooltip evaluation.
  */
class GdbCmd_FindTooltipType : public DebuggerCmd
{
        wxRect m_WinRect;
        wxString m_What;
        static bool singleUsage; // special flag to avoid launching multiple tooltips because of event chain latency
    public:
        /** @param tree The tree to display the watch. */
        GdbCmd_FindTooltipType(DebuggerDriver* driver, const wxString& what, const wxRect& tiprect)
            : DebuggerCmd(driver),
            m_WinRect(tiprect),
            m_What(what)
        {
            if (!singleUsage)
            {
                singleUsage = true;
                m_Cmd << _T("whatis ");
                m_Cmd << m_What;
            }
        }
        ~GdbCmd_FindTooltipType()
        {
            singleUsage = false;
        }
        void ParseOutput(const wxString& output)
        {
            // examples:
            // type = wxString
            // type = const wxChar
            // type = Action *
            // type = bool

            wxString tmp = output.AfterFirst(_T('='));
            tmp.Trim(false);

            // add the actual evaluation command with high priority
            m_pDriver->QueueCommand(new GdbCmd_FindTooltipAddress(m_pDriver, m_What, m_WinRect, tmp), DebuggerDriver::High);
        }
};
bool GdbCmd_FindTooltipType::singleUsage = false;

class GdbCmd_LocalsFuncArgs : public DebuggerCmd
{
        cb::shared_ptr<GDBWatch> m_watch;
        bool m_doLocals;
    public:
        GdbCmd_LocalsFuncArgs(DebuggerDriver* driver, cb::shared_ptr<GDBWatch> watch, bool doLocals) :
            DebuggerCmd(driver),
            m_watch(watch),
            m_doLocals(doLocals)
        {
            if (m_doLocals)
                m_Cmd = wxT("info locals");
            else
                m_Cmd = wxT("info args");
        }
        void ParseOutput(const wxString& output)
        {
            if ((m_doLocals && output == wxT("No locals.")) || (!m_doLocals && output == wxT("No arguments.")))
            {
                m_watch->RemoveChildren();
                return;
            }

            std::vector<GDBLocalVariable> watchStrings;
            TokenizeGDBLocals(watchStrings, output);

            m_watch->MarkChildsAsRemoved();
            for (std::vector<GDBLocalVariable>::const_iterator it = watchStrings.begin(); it != watchStrings.end(); ++it)
            {
                if (it->error)
                    continue;
                cb::shared_ptr<GDBWatch> watch = AddChild(m_watch, it->name);
                ParseGDBWatchValue(watch, it->value);
            }
            m_watch->RemoveMarkedChildren();
        }
};

/**
  * Command to change the current frame.
  * Exists so GDB_driver::ParseOutput() can detect changeframe vs
  * breakpoint output and avoid undesired part of breakpoint processing.
  */
class GdbCmd_ChangeFrame : public DebuggerCmd
{
    int   m_addrchgmode;
    public:
        int AddrChgMode() { return m_addrchgmode; }
        GdbCmd_ChangeFrame(DebuggerDriver* driver, int frameno, int p_addrchgmode=1)
            : DebuggerCmd(driver)
            ,m_addrchgmode(p_addrchgmode) //1 means do not change disassembly address
        {
            m_Cmd << _T("frame ") << frameno;
        }
        void ParseOutput(const wxString& output)
        {
            m_pDriver->Log(output);
        }
};
/**
  * Command to run a backtrace.
  */
class GdbCmd_Backtrace : public DebuggerCmd
{
    public:
        /** @param dlg The backtrace dialog. */
        GdbCmd_Backtrace(DebuggerDriver* driver)
            : DebuggerCmd(driver)
        {
            m_Cmd << _T("bt 30");
        }
        void ParseOutput(const wxString& output)
        {
            int validFrameNumber = -1;
            cbStackFrame validSF;

            m_pDriver->GetStackFrames().clear();
            wxArrayString lines = GetArrayFromString(output, _T('\n'));
            for (unsigned int i = 0; i < lines.GetCount(); ++i)
            {
                cbStackFrame sf;
                bool hasLineInfo;
                bool matched = MatchLine(sf, hasLineInfo, lines[i]);
                if (matched)
                {
                    if (hasLineInfo && validFrameNumber == -1)
                    {
                        validSF = sf;
                        validFrameNumber = sf.GetNumber();
                    }
                    m_pDriver->GetStackFrames().push_back(cb::shared_ptr<cbStackFrame>(new cbStackFrame(sf)));
                }
            }
            if (validFrameNumber > 0) // if it's 0, then the driver already synced the editor
            {
                bool autoSwitch = cbDebuggerCommonConfig::GetFlag(cbDebuggerCommonConfig::AutoSwitchFrame);
                if (!autoSwitch)
                {
                    long line;

                    // replace the valid stack frame with the first frame or the user selected frame
                    if (!m_pDriver->GetStackFrames().empty())
                    {
                        if (m_pDriver->GetUserSelectedFrame() != -1)
                        {
                            validFrameNumber = m_pDriver->GetUserSelectedFrame();
                            DebuggerDriver::StackFrameContainer const &frames = m_pDriver->GetStackFrames();

                            if (validFrameNumber >= 0 && validFrameNumber <= static_cast<int>(frames.size()))
                                validSF = *frames[validFrameNumber];
                            else if (!frames.empty())
                                validSF = *frames.front();
                        }
                    }
                    if (validSF.GetLine().ToLong(&line))
                    {
                        m_pDriver->Log(wxString::Format(_T("Displaying first frame with valid source info (#%d)"), validFrameNumber));
                        m_pDriver->ShowFile(validSF.GetFilename(), line);
                    }
                }
                else
                {
                    if (m_pDriver->GetUserSelectedFrame() != -1)
                        validFrameNumber = m_pDriver->GetUserSelectedFrame();
                    // can't call m_pDriver->SwitchToFrame() here
                    // because it causes a cascade update, never stopping...
                    //m_pDriver->Log(wxString::Format(_T("Switching to frame #%d which has valid source info"), validFrameNumber));

                    //The following output:
                    //>>>>>>cb_gdb:
                    //> frame 1
                    //#1  0x6f826722 in wxInitAllImageHandlers () at ../../src/common/imagall.cpp:29
                    //^Z^ZC:\dev\wxwidgets\wxWidgets-2.8.10\build\msw/../../src/common/imagall.cpp:29:961:beg:0x6f826722
                    //>>>>>>cb_gdb:
                    //matches output from both break and frame responses. We need to ignore it
                    //for a frame command to avoid incorrect disassembly displays when stepping instructions.
                    m_pDriver->QueueCommand(new GdbCmd_ChangeFrame(m_pDriver, validFrameNumber));
                    m_pDriver->SetCurrentFrame(validFrameNumber, false);
                }
            }
            Manager::Get()->GetDebuggerManager()->GetBacktraceDialog()->Reload();
        }

        static bool MatchLine(cbStackFrame &sf, bool &hasLineInfo, const wxString &line)
        {
            hasLineInfo = false;
            // reBT1 matches frame number, address, function and args (common to all formats)
            // reBT2 matches filename and line (optional)
            // reBT3 matches filename only (for DLLs) (optional)

            // #0  main (argc=1, argv=0x3e2440) at my main.cpp:15
            if (reBTX.Matches(line))
            {
                long int number;
                reBTX.GetMatch(line, 1).ToLong(&number);
                sf.SetNumber(number);
                sf.SetAddress(cbDebuggerStringToAddress(reBTX.GetMatch(line, 2)));
                sf.SetSymbol(reBTX.GetMatch(line, 3) + reBTX.GetMatch(line, 4));
            }
            else if (reBT1.Matches(line))
            {
                long int number;
                reBT1.GetMatch(line, 1).ToLong(&number);
                sf.SetNumber(number);
                sf.SetAddress(cbDebuggerStringToAddress(reBT1.GetMatch(line, 2)));
                sf.SetSymbol(reBT1.GetMatch(line, 3) + reBT1.GetMatch(line, 4));
            }
            else if (reBT0.Matches(line))
            {
                long int number;
                reBT0.GetMatch(line, 1).ToLong(&number);
                sf.SetNumber(number);
                sf.SetAddress(0);
                sf.SetSymbol(reBT0.GetMatch(line, 2));
                sf.SetFile(reBT0.GetMatch(line, 3), wxEmptyString);
            }
            else if (reBT4.Matches(line))
            {
                long int number;
                reBT4.GetMatch(line, 1).ToLong(&number);
                sf.SetNumber(number);
                sf.SetAddress(cbDebuggerStringToAddress(reBT4.GetMatch(line, 2)));
                sf.SetSymbol(reBT4.GetMatch(line, 3));
            }
            else
                return false;

            sf.MakeValid(true);
            if (reBT2.Matches(line))
            {
                sf.SetFile(reBT2.GetMatch(line, 1), reBT2.GetMatch(line, 2));
                hasLineInfo = true;
            }
            else if (reBT3.Matches(line))
                sf.SetFile(reBT3.GetMatch(line, 1), wxEmptyString);
            return true;
        }
};

/**
  * Command to obtain register info.
  */
class GdbCmd_InfoRegisters : public DebuggerCmd
{
        wxString m_disassemblyFlavor;

    public:
        /** @param dlg The disassembly dialog. */

        // only tested on mingw/pc/win env
        GdbCmd_InfoRegisters(DebuggerDriver* driver, wxString disassemblyFlavor = wxEmptyString) :
            DebuggerCmd(driver),
            m_disassemblyFlavor(disassemblyFlavor)
        {
            m_Cmd << _T("info registers");
        };

        void ParseOutput(const wxString& output)
        {
            // output is a series of:
            //
            // eax            0x40e66666       1088841318
            // ecx            0x40cbf0 4246512
            // edx            0x77c61ae8       2009471720
            // ebx            0x4000   16384
            // esp            0x22ff50 0x22ff50
            // ebp            0x22ff78 0x22ff78
            // esi            0x22ef80 2289536
            // edi            0x5dd3f4 6149108
            // eip            0x4013c9 0x4013c9
            // eflags         0x247    583
            // cs             0x1b     27
            // ss             0x23     35
            // ds             0x23     35
            // es             0x23     35
            // fs             0x3b     59
            // gs             0x0      0

            // or32 register string parser
            if(m_disassemblyFlavor == _T("set disassembly-flavor or32"))
            {
                ParseOutputFromOR32gdbPort(output);
            }
            else
            // use generic parser - this may work for other platforms or you may have to write your own
            {
                cbCPURegistersDlg *dialog = Manager::Get()->GetDebuggerManager()->GetCPURegistersDialog();

                wxArrayString lines = GetArrayFromString(output, _T('\n'));
                for (unsigned int i = 0; i < lines.GetCount(); ++i)
                {
                    if (reRegisters.Matches(lines[i]))
                    {
                        const wxString &addr = reRegisters.GetMatch(lines[i], 1);
                        const wxString &hex = reRegisters.GetMatch(lines[i], 2);
                        const wxString &interpreted = reRegisters.GetMatch(lines[i], 3);
                        dialog->SetRegisterValue(addr, hex, interpreted);
                    }
                }
            }

//            m_pDlg->Show(true);
//            m_pDriver->DebugLog(output);
        }

        void ParseOutputFromOR32gdbPort(const wxString& output)
        {
// (gdb) info reg
//        R0        R1        R2        R3        R4        R5        R6        R7
//  00000000  f0016f2c  f0016ff8  00000005  00000008  00004c84  ffffbfff  00000001
//        R8        R9       R10       R11       R12       R13       R14       R15
//  00000001  00004ce0  0001e888  00000000  00000000  00000000  f0001754  00000014
//       R16       R17       R18       R19       R20       R21       R22       R23
//  000000e1  00000000  00000003  00000000  8000000c  00000000  f0000870  00000000
//       R24       R25       R26       R27       R28       R29       R30       R31
//  000000c0  00000000  00030021  00000000  00000000  00000000  00000000  f0016f2c

            // produce an array of alternate register/value string lines, each entry
            // is started on detecting a '\n'
            wxArrayString lines = GetArrayFromString(output, _T("\n"));

            // check for empty or short string
            if((output == _T("")) || (lines.GetCount()<2))
            {
                return;
            }
            cbCPURegistersDlg *dialog = Manager::Get()->GetDebuggerManager()->GetCPURegistersDialog();

            for (unsigned int i = 0; i < lines.GetCount(); i+=2)
            {
                wxArrayString regMnemonics;
                wxArrayString regValues;
                wxString RegisterMnemonicString;
                wxString RegisterValueString;

                // filter register values
                RegisterValueString =lines.Item(i+1);

                wxStringTokenizer RegisterValueStringTokenizer((RegisterValueString), wxT(" "), wxTOKEN_STRTOK);
                while ( RegisterValueStringTokenizer.HasMoreTokens() )
                {
                    wxString RegisterValueStringToken = RegisterValueStringTokenizer.GetNextToken();
                    // add register value to array
                    regValues.Add(RegisterValueStringToken);
                }
                // register mnemonics on even (and zero) lines
                RegisterMnemonicString =lines.Item(i);

                wxStringTokenizer RegisterMnemonicStringTokenizer((RegisterMnemonicString), wxT(" "), wxTOKEN_STRTOK);
                while ( RegisterMnemonicStringTokenizer.HasMoreTokens() )
                {
                    wxString RegisterMnemonicStringToken = RegisterMnemonicStringTokenizer.GetNextToken();
                    // add register mnemonic to arrau
                    regMnemonics.Add(RegisterMnemonicStringToken);
                }

                // loop around the values and mnemonics arrays and add them to the dialog boxes
                for (unsigned int j = 0; j < regMnemonics.GetCount(); j++)
                {
                    wxString reg = regMnemonics.Item(j);
                    wxString addr = regValues.Item(j);

                    if (!reg.IsEmpty() && !addr.IsEmpty())
                        dialog->SetRegisterValue(reg, addr, wxEmptyString);
                }
            }
        }
};

/**
  * Command to run a disassembly.
  */
class GdbCmd_Disassembly : public DebuggerCmd
{
    bool m_mixedMode;

    public:
        GdbCmd_Disassembly(DebuggerDriver* driver, bool MixedMode, wxString hexAddrStr)
            : DebuggerCmd(driver)
            , m_mixedMode(MixedMode)
        {
            m_Cmd << _T("disassemble");
            if(m_mixedMode)
                //gdb's ordering of instructions with /m can be pretty confusing...
                //with /m, sometimes, some instructions are missing ( for gdb 7.1/7.2 x86
                //on output from tdm gxx 4.4.1) from returned responses.
                m_Cmd << _T(" /m");

            if(hexAddrStr.IsEmpty())
                //****NOTE: If this branch is taken, disassembly may not reflect the program's
                //actual current location.  Other areas of code will change the current (stack) frame
                //which results in $pc reflecting the eip(x86-based) of that frame.  After changing to
                //a non-top frame, a request (gdb 7.2 x86) to print either '$pc' or '$eip' will
                //return the same value.
                //So, there seems to be no way to obtain the actual current address in this (non-MI)
                //interface.  Hence, we can't get the correct disassembly (when the $pc does not
                //reflect actual current address.)  GDB itself does continue to step from the correct address, so
                //there may be some other way to obtain it yet to be found.
                m_Cmd << _T(" $pc");
            else if(wxT("0x") == hexAddrStr.Left(2) || wxT("0X") == hexAddrStr.Left(2))
                m_Cmd << wxT(" ") << hexAddrStr;
            else
                m_Cmd << wxT(" 0x") << hexAddrStr;
        }
        void ParseOutput(const wxString& output)
        {
            // output for "disassemble" is a series of:
            //
            // Dump of assembler code for function main:
            // 0x00401390 <main+0>:    push   ebp
            // ...
            // End of assembler dump.
            //
            // OR, output for "disassemble /m" is:
            //Dump of assembler code for function main:
            //6       {
            //   0x00401318 <+0>:     push   %ebp
            //   0x00401319 <+1>:     mov    %esp,%ebp
            //   0x0040131b <+3>:     and    $0xfffffff0,%esp
            //   0x0040131e <+6>:     push   %ebx
            //   0x0040131f <+7>:     mov    $0x103c,%eax
            //   0x00401324 <+12>:    call   0x401bac <_alloca>
            //   0x00401329 <+17>:    call   0x4019a0 <__main>
            //
            //7       #if 1
            //8           char filename[2048], filenameabs[2048] ;
            //9           if(argc > 1)
            //=> 0x0040132e <+22>:    cmpl   $0x1,0x8(%ebp)
            //   0x00401332 <+26>:    jle    0x401351 <main+57>
            // ...
            //   0x004015aa <+658>:   ret
            //
            //End of assembler dump.
            const wxString disasmerror(_T("No function contains specified address."));
            cbDisassemblyDlg *dialog = Manager::Get()->GetDebuggerManager()->GetDisassemblyDialog();
            wxArrayString lines = GetArrayFromString(output, _T('\n'));
            for (unsigned int i = 0; i < lines.GetCount(); ++i)
            {
                if (lines[i].StartsWith(disasmerror))
                {
                    //So, GDB won't disassemble anywhere there is code????
                    dialog->AddSourceLine(0, disasmerror);
                    break ;
                }
                else if (reDisassembly.Matches(lines[i]))
                {
                    uint64_t addr = cbDebuggerStringToAddress(reDisassembly.GetMatch(lines[i], 1));
                    dialog->AddAssemblerLine(addr, reDisassembly.GetMatch(lines[i], 2));
                }
                else if (m_mixedMode && reDisassemblySource.Matches(lines[i]))
                {
                    long int lineno;
                    reDisassemblySource.GetMatch(lines[i], 1).ToLong(&lineno, 10);
                    dialog->AddSourceLine(lineno, reDisassemblySource.GetMatch(lines[i], 2));
                }
            }
            dialog->CenterCurrentLine();
        }
};

/**
  * Command to initialize a disassembly. Use this instead of GdbCmd_Disassembly, which is chain-called by this.
  */
class GdbCmd_DisassemblyInit : public DebuggerCmd
{
        wxString m_disassemblyFlavor;
        wxString m_hexAddrStr;

        static wxString LastAddr;
        static wxString LastSymbol;
    public:
         // only tested on mingw/pc/win env
        GdbCmd_DisassemblyInit(DebuggerDriver* driver, wxString disassemblyFlavor = wxEmptyString,
                               wxString hexAddrStr = wxT(""))
            : DebuggerCmd(driver),
            m_disassemblyFlavor(disassemblyFlavor),
            m_hexAddrStr(hexAddrStr)
        {
            m_Cmd << _T("if 1\n") ;
            if(m_hexAddrStr.empty())
            {
                const Cursor &cursor = driver->GetCursor() ;
                if(cursor.address.empty())
                    m_Cmd << _T("disassemble $pc,$pc+50\n") ;
                else
                {
                    m_Cmd << _T("disassemble ") << cursor.address << _T("\n") ;
                }
            }
            else
                m_Cmd << _T("disassemble ") << m_hexAddrStr << _T("\n") ;

            m_Cmd << _T("info frame\n") << _T("end");
        };

        void ParseOutput(const wxString& p_output)
        {
            cbDisassemblyDlg *dialog = Manager::Get()->GetDebuggerManager()->GetDisassemblyDialog();

            wxString frame_output, reg_output ;
            size_t apos ;
            apos = p_output.find(_T("Stack level ")); //looking for 'info frame' output
            if(apos == wxString::npos)
            {
                m_pDriver->Log(_T("Failure finding \"Stack level \""));
                apos = p_output.length();
            }
            reg_output = p_output.substr(0,apos);
            frame_output = p_output.substr(apos, p_output.length()-apos);
            wxString &output = frame_output ;
            if(reDisassemblyCurPC.Matches(reg_output))
            {
                if(m_hexAddrStr.empty())
                {
                    m_hexAddrStr = reDisassemblyCurPC.GetMatch(reg_output,1);
                }
            }
            else
            {
                m_pDriver->Log(_T("Failure matching reg_output"));
            }
            //process 'info frame'
            const wxArrayString &lines = GetArrayFromString(output, _T('\n'));
            if (lines.Count() <= 2)
                return;
            size_t firstLine = 0;
            for (; firstLine < lines.Count() && !reDisassemblyInit.Matches(lines[firstLine]); ++firstLine)
                ;
            if (firstLine + 1 < lines.Count())
            {
                bool sameSymbol = false;
                if (reDisassemblyInitSymbol.Matches(lines[firstLine]))
                {
                    const wxString &symbol = reDisassemblyInitSymbol.GetMatch(lines[firstLine], 1)
                                             + reDisassemblyInitSymbol.GetMatch(lines[firstLine], 2);
                    sameSymbol = (LastSymbol == symbol);

                    if (!sameSymbol)
                        LastSymbol = symbol;
                }

                cbStackFrame sf;
                const wxString &addr = reDisassemblyInit.GetMatch(output, 1);
                if (addr == LastAddr && sameSymbol)
                    return;
                LastAddr = addr;
                sf.SetAddress(cbDebuggerStringToAddress(addr));
                if (reDisassemblyInitFunc.Matches(output))
                    sf.SetSymbol(reDisassemblyInitFunc.GetMatch(output, 2));

                sf.MakeValid(true);
                dialog->Clear(sf);
                if(!m_hexAddrStr.empty())
                {
                    dialog->SetActiveAddress(cbDebuggerStringToAddress(m_hexAddrStr));
                    Cursor acursor = m_pDriver->GetCursor();
                    acursor.address = m_hexAddrStr;
                    m_pDriver->SetCursor(acursor);
                }
                bool mixedmode = Manager::Get()->GetDebuggerManager()->IsDisassemblyMixedMode();
                m_pDriver->QueueCommand(new GdbCmd_Disassembly(m_pDriver, mixedmode, m_hexAddrStr)); //chain call
            }
        }

        static void Clear()
        {
            LastAddr.Clear();
            LastSymbol.Clear();
        }
};
// static
wxString GdbCmd_DisassemblyInit::LastAddr;
wxString GdbCmd_DisassemblyInit::LastSymbol;

/**
  * Command to examine a memory region.
  */
class GdbCmd_ExamineMemory : public DebuggerCmd
{
    public:
        /** @param dlg The memory dialog. */
        GdbCmd_ExamineMemory(DebuggerDriver* driver)
            : DebuggerCmd(driver)
        {
            cbExamineMemoryDlg *dialog = Manager::Get()->GetDebuggerManager()->GetExamineMemoryDialog();
            const wxString &address = CleanStringValue(dialog->GetBaseAddress());
            m_Cmd.Printf(_T("x/%dxb %s"), dialog->GetBytes(), address.c_str());
        }
        void ParseOutput(const wxString& output)
        {
            // output is a series of:
            //
            // 0x22ffc0:       0xf0    0xff    0x22    0x00    0x4f    0x6d    0x81    0x7c
            // or
            // 0x85267a0 <RS485TxTask::taskProc()::rcptBuf>:   0x00   0x00   0x00   0x00   0x00   0x00   0x00   0x00

            cbExamineMemoryDlg *dialog = Manager::Get()->GetDebuggerManager()->GetExamineMemoryDialog();

            dialog->Begin();
            dialog->Clear();

            wxArrayString lines = GetArrayFromString(output, _T('\n'));
            wxString addr, memory;
            for (unsigned int i = 0; i < lines.GetCount(); ++i)
            {
                if (reExamineMemoryLine.Matches(lines[i]))
                {
                    addr = reExamineMemoryLine.GetMatch(lines[i], 1);
                    memory = reExamineMemoryLine.GetMatch(lines[i], 2);
                }
                else
                {
                    if (lines[i].First(_T(':')) == -1)
                    {
                        dialog->AddError(lines[i]);
                        continue;
                    }
                    addr = lines[i].BeforeFirst(_T(':'));
                    memory = lines[i].AfterFirst(_T(':'));
                }

                size_t pos = memory.find(_T('x'));
                while (pos != wxString::npos)
                {
                    wxString hexbyte;
                    hexbyte << memory[pos + 1];
                    hexbyte << memory[pos + 2];
                    dialog->AddHexByte(addr, hexbyte);
                    pos = memory.find(_T('x'), pos + 1); // skip current 'x'
                }
            }
            dialog->End();
        }
};

class GdbCmd_RemoteBaud : public DebuggerCmd
{
    public:
        GdbCmd_RemoteBaud(DebuggerDriver* driver, const wxString& baud)
            : DebuggerCmd(driver)
        {
            m_Cmd << _T("set remotebaud ") << baud;
            driver->Log(_("Setting serial connection speed to ") + baud);
        }
        void ParseOutput(cb_unused const wxString& output)
        {
        }
};

class GdbCmd_RemoteTarget : public DebuggerCmd
{
    public:
        GdbCmd_RemoteTarget(DebuggerDriver* driver, RemoteDebugging* rd)
            : DebuggerCmd(driver)
        {
            const wxString targetRemote = rd->extendedRemote ? _T("target extended-remote ") : _T("target remote ");
            switch (rd->connType)
            {
                case RemoteDebugging::TCP:
                {
                    if (!rd->ip.IsEmpty() && !rd->ipPort.IsEmpty())
                        m_Cmd << targetRemote << _T("tcp:") << rd->ip << _T(":") << rd->ipPort;
                }
                break;

                case RemoteDebugging::UDP:
                {
                    if (!rd->ip.IsEmpty() && !rd->ipPort.IsEmpty())
                        m_Cmd << targetRemote << _T("udp:") << rd->ip << _T(":") << rd->ipPort;
                }
                break;

                case RemoteDebugging::Serial:
                {
                    if (!rd->serialPort.IsEmpty())
                        m_Cmd << targetRemote << rd->serialPort;
                }
                break;

                default:
                    break;
            }

            Manager::Get()->GetMacrosManager()->ReplaceEnvVars(m_Cmd);
            if (!m_Cmd.IsEmpty())
                driver->Log(_("Connecting to remote target"));
            else
                m_pDriver->Log(_("Invalid settings for remote debugging!"));
        }
        void ParseOutput(const wxString& output)
        {
            // This command will either output an error or a breakpoint address info
            // Connection errors are of the form:
            //
            // tcp:10.10.1.205:2345: No route to host.
            // (remote system can't be contacted on the IP level)
            //
            // tcp:10.10.1.205:2345: Connection refused.
            // (no gdb proxy/server running on the specified remote system ip/port)
            //
            // tcp:1111:222: Invalid argument.
            //
            // sdsdsds: unknown host
            // tcp:sdsdsds:ddd: No such file or directory.
            //
            // Malformed response to offset query, *
            // Ignoring packet error, continuing...
            // (serial line errors)
            //
            // Now, we could use a regex to filter these but this might be overkill
            // since the above errors are the only (?) ones we could get.
            // So for now we 'll just check them verbatim...

            wxString errMsg;

            if (output.Contains(_T("No route to host")))
                errMsg << _("Can't connect to the remote system.\nVerify your connection settings and that\nthe remote system is reachable/powered-on.");
            else if (output.Contains(_T("Connection refused")))
                errMsg << _("Connection refused by the remote system.\nVerify your connection settings and that\nthe GDB server/proxy is running on the remote system.");
            else if (output.Contains(_T("Malformed response")) ||
                    output.Contains(_T("packet error")))
            {
                errMsg << _("Connection can't be established.\nVerify your connection settings and that\nthe GDB server/proxy is running on the remote system.");
            }
            else if (output.Contains(_T("Invalid argument")))
                errMsg << _("Invalid argument.\nVerify your connection settings (probably some typo).");
            else if (output.Contains(_T("unknown host")))
                errMsg << _("Unknown host.\nVerify your connection settings (probably some typo).");

            if (!errMsg.IsEmpty())
            {
                m_pDriver->Log(_("Failed"));

                // tell the user
                errMsg << _("\nThe exact error message was:\n\n");
                errMsg << output;
                InfoWindow::Display(_("Error"), errMsg, 10000, 1000); // show for 10 seconds with 1 second delay
                return;
            }

            m_pDriver->Log(_("Connected"));
        }
};

class GdbCmd_StepOrNextInstruction : public DebuggerContinueBaseCmd
{

    public:
        GdbCmd_StepOrNextInstruction(GDB_driver* driver, const wxChar *command)
            : DebuggerContinueBaseCmd(driver)
        {
            m_Cmd << command;
        }
        void ParseOutput(const wxString& output)
        {
            DebuggerManager *manager = Manager::Get()->GetDebuggerManager();
            if (!manager->UpdateDisassembly())
                return;
            wxString disasm_flavour = static_cast<GDB_driver*>(m_pDriver)->AsmFlavour() ;
            cbDisassemblyDlg *dialog = manager->GetDisassemblyDialog();
            m_pDriver->Log(output);

            wxString addrstr;

            if(reStepI.Matches(output)) //applies to reStepI and reNextI seem to be same
                addrstr = reStepI.GetMatch(output, 6);
            else if(reStepI2.Matches(output))
                addrstr = reStepI2.GetMatch(output, 1);
            else if(reStepI3.Matches(output))
                addrstr = reStepI3.GetMatch(output, 1);
            else if(reStepI4.Matches(output))
                addrstr = reStepI4.GetMatch(output, 1);
            else
            {
                // There is an error parsing the output, so clear file/line location info
                cbStackFrame sf;
                dialog->Clear(sf);
                //Since we don't recognize/anticipate that output, and thus
                //can't get an address, request a complete re-disassembly.
                m_pDriver->QueueCommand(new GdbCmd_DisassemblyInit(m_pDriver,disasm_flavour));
                return;
            }

            if (addrstr.empty())
                return;

            if (!dialog->SetActiveAddress(cbDebuggerStringToAddress(addrstr)))
                m_pDriver->QueueCommand(new GdbCmd_DisassemblyInit(m_pDriver,disasm_flavour ,addrstr));
        }
};
class GdbCmd_StepInstruction : public GdbCmd_StepOrNextInstruction
{
    public:
        GdbCmd_StepInstruction(GDB_driver* driver)
            : GdbCmd_StepOrNextInstruction(driver, _T("nexti"))
        {
        }
};
class GdbCmd_StepIntoInstruction : public GdbCmd_StepOrNextInstruction
{
    public:
        GdbCmd_StepIntoInstruction(GDB_driver* driver)
            : GdbCmd_StepOrNextInstruction(driver, _T("stepi"))
        {
        }
};

/** Command which tries to find the current cursor position.
  * It is executed if the gdb prompt is detected, but no cursor information is parsed prior it.
  */
class GdbCmd_FindCursor : public DebuggerCmd
{
    public:
        GdbCmd_FindCursor(GDB_driver *driver) :
            DebuggerCmd(driver, wxT("info frame"))
        {
        }

        void ParseOutput(const wxString& output)
        {
            const wxArrayString &lines = GetArrayFromString(output, _T('\n'));
            if (lines.Count() <= 2)
                return;
            size_t firstLine = 0;
            for (; firstLine < lines.Count() && !reDisassemblyInit.Matches(lines[firstLine]); ++firstLine)
                ;
            firstLine++;
            if (firstLine < lines.Count())
            {
                wxString symbol, file, line;
                if (reDisassemblyInitSymbol.Matches(lines[firstLine]))
                {
                    symbol = reDisassemblyInitSymbol.GetMatch(lines[firstLine], 1);
                    file = reDisassemblyInitSymbol.GetMatch(lines[firstLine], 2);
                    line = reDisassemblyInitSymbol.GetMatch(lines[firstLine], 3);
                }

                const wxString &addr = reDisassemblyInit.GetMatch(output, 1);
                long longAddress;
                addr.ToULong((unsigned long int*)&longAddress, 16);

                Cursor cursor = m_pDriver->GetCursor();
                cursor.address =  addr;
                cursor.changed = true;
                cursor.file = file;
                cursor.function = symbol;
                if (!line.ToLong(&cursor.line))
                    cursor.line = -1;
                m_pDriver->SetCursor(cursor);
                m_pDriver->NotifyCursorChanged();
            }
        }
};

#endif // DEBUGGER_COMMANDS_H
