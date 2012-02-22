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
#include "configmanager.h"
#include <globals.h>
#include <manager.h>
#include <editormanager.h>
#include <cbeditor.h>
#include <scriptingmanager.h>
#include <sqplus.h>
#include <infowindow.h>
#include "debugger_defs.h"
#include "debuggergdb.h"
#include "gdb_driver.h"
#include "debuggertree.h"
#include "backtracedlg.h"
#include "examinememorydlg.h"
#include "threadsdlg.h"
#include "gdb_tipwindow.h"
#include "remotedebugging.h"

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
      #if wxCHECK_VERSION(2, 9, 0)
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
static wxRegEx reBT0(_T("#([0-9]+)[ \t]+([^(]+)[ \t]+(\\([^)]*\\))")); // case #0
static wxRegEx reBT1(_T("#([0-9]+)[ \t]+0x([A-Fa-f0-9]+)[ \t]+in[ \t]+(.+)[ \t]+(\\([^)]*\\))[ \t]")); // all other cases (gdb 6.3)
static wxRegEx reBTX(_T("#([0-9]+)[ \t]+0x([A-Fa-f0-9]+)[ \t]+in[ \t]+([^(]+)[ \t]*(\\([^)]*\\)[ \t]*\\([^)]*\\))")); // all other cases (gdb 5.2)
static wxRegEx reBT2(_T("\\)[ \t]+[atfrom]+[ \t]+(.*):([0-9]+)"));
static wxRegEx reBT3(_T("\\)[ \t]+[atfrom]+[ \t]+(.*)"));
// Breakpoint 1 at 0x4013d6: file main.cpp, line 8.
static wxRegEx reBreakpoint(_T("Breakpoint ([0-9]+) at (0x[0-9A-Fa-f]+)"));
// Breakpoint 1 ("/home/jens/codeblocks-build/codeblocks-1.0svn/src/plugins/debuggergdb/gdb_commands.h:125) pending.
static wxRegEx rePendingBreakpoint(_T("Breakpoint ([0-9]+)[ \\t]\\(\\\"(.+):([0-9]+)\\)[ \\t]pending\\."));
// Hardware assisted breakpoint 1 at 0x4013d6: file main.cpp, line 8.
static wxRegEx reHWBreakpoint(_T("Hardware assisted breakpoint ([0-9]+) at (0x[0-9A-Fa-f]+)"));
// Hardware watchpoint 1: expr
static wxRegEx reDataBreakpoint(_T("Hardware watchpoint ([0-9]+):.*"));
// eax            0x40e66666       1088841318
static wxRegEx reRegisters(_T("([A-z0-9]+)[ \t]+(0x[0-9A-Fa-f]+)[ \t]+(.*)"));
// wayne registers
//static wxRegEx reRegisters(_T("(R[0-9]+)[ \t]+(0x[0-9A-Fa-f]+)"));
// 0x00401390 <main+0>:    push   ebp
static wxRegEx reDisassembly(_T("(0x[0-9A-Za-z]+)[ \t]+<.*>:[ \t]+(.*)"));
//Stack level 0, frame at 0x22ff80:
// eip = 0x401497 in main (main.cpp:16); saved eip 0x4011e7
// source language c++.
// Arglist at 0x22ff78, args: argc=1, argv=0x3e3cb0
// Locals at 0x22ff78, Previous frame's sp is 0x22ff80
// Saved registers:
//  ebx at 0x22ff6c, ebp at 0x22ff78, esi at 0x22ff70, edi at 0x22ff74, eip at 0x22ff7c
static wxRegEx reDisassemblyInit(_T("^Stack level [0-9]+, frame at (0x[A-Fa-f0-9]+):"));
static wxRegEx reDisassemblyInitFunc(_T("eip = (0x[A-Fa-f0-9]+) in ([^;]*)"));
// or32 variant
#ifdef __WXMSW__
static wxRegEx reDisassemblyInitFuncOR32(_T("PC = (0x[A-Fa-f0-9]+) in ([^;]*)"));
#else
// not used on linux, but make sure it exists otherwise compilation fails on linux
// if(platform::windows && m_disassemblyFlavor == _T("set disassembly-flavor or32")) blabla
static wxRegEx reDisassemblyInitFuncOR32(_T("PC = (0x[A-Fa-f0-9]+) in ([^;]*)"));
#endif
//    Using the running image of child Thread 46912568064384 (LWP 7051).
static wxRegEx reInfoProgramThread(_T("\\(LWP[ \t]([0-9]+)\\)"));
//    Using the running image of child process 10011.
static wxRegEx reInfoProgramProcess(_T("child process ([0-9]+)"));
//  2 Thread 1082132832 (LWP 8017)  0x00002aaaac5a2aca in pthread_cond_wait@@GLIBC_2.3.2 () from /lib/libpthread.so.0
//* 1 Thread 46912568064384 (LWP 7926)  0x00002aaaac76e612 in poll () from /lib/libc.so.6
static wxRegEx reInfoThreads(_T("(\\**)[ \t]*([0-9]+)[ \t](.*)[ \t]in"));
static wxRegEx reGenericHexAddress(_T("(0x[A-Fa-f0-9]+)"));

DECLARE_INSTANCE_TYPE(wxString);

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
        void ParseOutput(const wxString& /*output*/)
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
                else if (lines[i].StartsWith(_T("Can't ")))
                {
                    // log this and quit debugging
                    m_pDriver->Log(lines[i]);
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
        DebuggerBreakpoint* m_BP;
    public:
        /** @param bp The breakpoint to set its condition. */
        GdbCmd_AddBreakpointCondition(DebuggerDriver* driver, DebuggerBreakpoint* bp)
            : DebuggerCmd(driver),
            m_BP(bp)
        {
            m_Cmd << _T("condition ") << wxString::Format(_T("%d"), (int) m_BP->index);
            if (m_BP->useCondition)
                m_Cmd << _T(" ") << m_BP->condition;
        }
        void ParseOutput(const wxString& output)
        {
            if (output.StartsWith(_T("No symbol ")))
            {
                wxString s = wxString::Format(_("While setting up custom conditions for breakpoint %d (%s, line %d),\n"
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
        DebuggerBreakpoint* m_BP;
    public:
        /** @param bp The breakpoint to set. */
        GdbCmd_AddBreakpoint(DebuggerDriver* driver, DebuggerBreakpoint* bp)
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
                    m_Cmd << _T('"') << out << _T(":") << wxString::Format(_T("%d"), m_BP->line + 1) << _T('"');
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
            else
                m_pDriver->Log(output); // one of the error responses
        }
};

/**
  * Command to add a data breakpoint.
  */
class GdbCmd_AddDataBreakpoint : public DebuggerCmd
{
        DebuggerBreakpoint* m_BP;
    public:
        /** @param bp The breakpoint to set. */
        GdbCmd_AddDataBreakpoint(DebuggerDriver* driver, DebuggerBreakpoint* bp)
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
        GdbCmd_RemoveBreakpoint(DebuggerDriver* driver, DebuggerBreakpoint* bp)
            : DebuggerCmd(driver),
            m_BP(bp)
        {
            if (!bp)
            {
                m_Cmd << _T("delete breakpoints");
                return;
            }

            if (bp->enabled && bp->index >= 0)
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

        DebuggerBreakpoint* m_BP;
};

/**
  * Command to get info about local frame variables.
  */
class GdbCmd_InfoLocals : public DebuggerCmd
{
        DebuggerTree* m_pDTree;
    public:
        /** @param tree The tree to display the locals. */
        GdbCmd_InfoLocals(DebuggerDriver* driver, DebuggerTree* dtree)
            : DebuggerCmd(driver),
            m_pDTree(dtree)
        {
            m_Cmd << _T("info locals");
        }
        void ParseOutput(const wxString& output)
        {
            wxArrayString lines = GetArrayFromString(output, _T('\n'));
            wxString locals;
            locals << _T("Local variables = {");
            for (unsigned int i = 0; i < lines.GetCount(); ++i)
                locals << lines[i] << _T(',');
            locals << _T("}") << _T('\n');
            m_pDTree->BuildTree(0, locals, wsfGDB);
        }
};

/**
  * Command to get info about current function arguments.
  */
class GdbCmd_InfoArguments : public DebuggerCmd
{
        DebuggerTree* m_pDTree;
    public:
        /** @param tree The tree to display the args. */
        GdbCmd_InfoArguments(DebuggerDriver* driver, DebuggerTree* dtree)
            : DebuggerCmd(driver),
            m_pDTree(dtree)
        {
            m_Cmd << _T("info args");
        }
        void ParseOutput(const wxString& output)
        {
            wxArrayString lines = GetArrayFromString(output, _T('\n'));
            wxString args;
            args << _T("Function Arguments = {");
            for (unsigned int i = 0; i < lines.GetCount(); ++i)
                args << lines[i] << _T(',');
            args << _T("}") << _T('\n');
            m_pDTree->BuildTree(0, args, wsfGDB);
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
        ThreadsDlg* m_pList;
    public:
        /** @param tree The tree to display the args. */
        GdbCmd_Threads(DebuggerDriver* driver, ThreadsDlg* list)
            : DebuggerCmd(driver),
            m_pList(list)
        {
            m_Cmd << _T("info threads");
        }
        void ParseOutput(const wxString& output)
        {
            m_pList->Clear();
            wxArrayString lines = GetArrayFromString(output, _T('\n'));
            for (unsigned int i = 0; i < lines.GetCount(); ++i)
            {
//                m_pDriver->Log(lines[i]);
                if (reInfoThreads.Matches(lines[i]))
                {
//                    m_pDriver->Log(_T("MATCH!"));
                    wxString active = reInfoThreads.GetMatch(lines[i], 1);
                    wxString num = reInfoThreads.GetMatch(lines[i], 2);
                    wxString info = reInfoThreads.GetMatch(lines[i], 3);
                    m_pList->AddThread(active, num, info);
                }
            }
        }
};

/**
  * Command to get info about a watched variable.
  */
class GdbCmd_Watch : public DebuggerCmd
{
        DebuggerTree* m_pDTree;
        Watch* m_pWatch;
        wxString m_ParseFunc;
    public:
        /** @param tree The tree to display the watch. */
        GdbCmd_Watch(DebuggerDriver* driver, DebuggerTree* dtree, Watch* watch, const wxString& w_type = wxEmptyString)
            : DebuggerCmd(driver),
            m_pDTree(dtree),
            m_pWatch(watch)
        {
            m_Cmd = static_cast<GDB_driver*>(m_pDriver)->GetScriptedTypeCommand(w_type, m_ParseFunc);
            if (m_Cmd.IsEmpty())
            {
                m_Cmd << _T("output ");
                switch (m_pWatch->format)
                {
                    case Decimal:       m_Cmd << _T("/d "); break;
                    case Unsigned:      m_Cmd << _T("/u "); break;
                    case Hex:           m_Cmd << _T("/x "); break;
                    case Binary:        m_Cmd << _T("/t "); break;
                    case Char:          m_Cmd << _T("/c "); break;
                    case Float:         m_Cmd << _T("/f "); break;
                    default:            break;
                }
                m_Cmd << m_pWatch->keyword;

                // auto-set array types
                if (!m_pWatch->is_array &&
                    m_pWatch->format == Undefined &&
                    w_type.Contains(_T('[')))
                {
                    m_pWatch->is_array = true;
                }

                if (m_pWatch->is_array && m_pWatch->array_count)
                    m_Cmd << wxString::Format(_T("[%d]@%d"), m_pWatch->array_start, m_pWatch->array_count);
            }
            else
            {
                try
                {
                    SqPlus::SquirrelFunction<wxString&> f(cbU2C(m_Cmd));
                    m_Cmd = f(w_type, m_pWatch->keyword, watch->array_start, watch->array_count);
                }
                catch (SquirrelError e)
                {
                    m_Cmd = cbC2U(e.desc);
                }
            }
            m_pWatch->hasActiveCommand = true;
        }
        void ParseOutput(const wxString& output)
        {
            if(m_pWatch->pendingDelete)
            {
                m_pDTree->DeleteWatch(m_pWatch);
                return;
            }
            wxString w;
            w << m_pWatch->keyword << _T(" = ");
            if (!m_ParseFunc.IsEmpty())
            {
                try
                {
                    SqPlus::SquirrelFunction<wxString&> f(cbU2C(m_ParseFunc));
                    w << f(output, m_pWatch->array_start);
                }
                catch (SquirrelError e)
                {
                    w << cbC2U(e.desc);
                }
            }
            else
            {
                wxArrayString lines = GetArrayFromString(output, _T('\n'));
                for (unsigned int i = 0; i < lines.GetCount(); ++i)
                {
                    w << lines[i] << _T(',');
                }
            }
            w << _T('\n');
            m_pDTree->BuildTree(m_pWatch, w, wsfGDB);
            m_pWatch->hasActiveCommand = false;
        }
};

/**
  * Command to get a watched variable's type.
  */
class GdbCmd_FindWatchType : public DebuggerCmd
{
        DebuggerTree* m_pDTree;
        Watch* m_pWatch;
    public:
        /** @param tree The tree to display the watch. */
        GdbCmd_FindWatchType(DebuggerDriver* driver, DebuggerTree* dtree, Watch* watch)
            : DebuggerCmd(driver),
            m_pDTree(dtree),
            m_pWatch(watch)
        {
            m_Cmd << _T("whatis ");
            m_Cmd << m_pWatch->keyword;
            m_pWatch->hasActiveCommand = true;
        }
        void ParseOutput(const wxString& output)
        {
            if(m_pWatch->pendingDelete)
            {
                m_pDTree->DeleteWatch(m_pWatch);
                return;
            }
            // examples:
            // type = wxString
            // type = const wxChar
            // type = Action *
            // type = bool
            if(output.StartsWith(_T("No symbol")) || output.StartsWith(_T("Attempt to ")))
            {
                wxString w;
                w << m_pWatch->keyword << _T(" = ") << output << _T('\n');
                m_pDTree->BuildTree(m_pWatch, w, wsfGDB);
            }
            else
            {
                wxString tmp = output.AfterFirst(_T('='));
                // actually add this watch with high priority
                m_pDriver->QueueCommand(new GdbCmd_Watch(m_pDriver, m_pDTree, m_pWatch, tmp), DebuggerDriver::High);
            }
            m_pWatch->hasActiveCommand = false;
        }
};

/**
  * Command to display a tooltip about a variables value.
  */
class GdbCmd_TooltipEvaluation : public DebuggerCmd
{
        static GDBTipWindow* s_pWin;
        wxRect m_WinRect;
        wxString m_What;
        wxString m_Type;
        wxString m_Address;
        wxString m_ParseFunc;
    public:
        /** @param what The variable to evaluate.
            @param win A pointer to the tip window pointer.
            @param tiprect The tip window's rect.
        */
        GdbCmd_TooltipEvaluation(DebuggerDriver* driver, const wxString& what, const wxRect& tiprect, const wxString& w_type = wxEmptyString, const wxString& address = wxEmptyString)
            : DebuggerCmd(driver),
            m_WinRect(tiprect),
            m_What(what),
            m_Type(w_type),
            m_Address(address)
        {
            m_Cmd = static_cast<GDB_driver*>(m_pDriver)->GetScriptedTypeCommand(w_type, m_ParseFunc);
            if (m_Cmd.IsEmpty())
            {
                // if it's a pointer, automatically dereference it
                wxString deref;
                if (w_type.Length() > 2 && // at least 2 chars
                    w_type.Last() == _T('*') && // last is *
                    w_type.GetChar(w_type.Length() - 2) != _T('*') && // second last is not * (i.e. doesn't end with **)
                    !w_type.Contains(_T("char "))) // not char* (special case)
                {
                    deref = _T("*");
                }

                m_Cmd << _T("output ");
                m_Cmd << deref << what;
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
            {
                m_What = _("Error");
                contents = output;
            }
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

            if (s_pWin)
                (s_pWin)->Close();
            s_pWin = new GDBTipWindow((wxWindow*)Manager::Get()->GetAppWindow(), m_What, m_Type, m_Address, contents, 640, &s_pWin, &m_WinRect);
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

/**
  * Command to run a backtrace.
  */
class GdbCmd_Backtrace : public DebuggerCmd
{
        BacktraceDlg* m_pDlg;
    public:
        /** @param dlg The backtrace dialog. */
        GdbCmd_Backtrace(DebuggerDriver* driver, BacktraceDlg* dlg)
            : DebuggerCmd(driver),
            m_pDlg(dlg)
        {
            m_Cmd << _T("bt 30");
        }
        void ParseOutput(const wxString& output)
        {
            int validFrameNumber = -1;
            StackFrame validSF;

            m_pDlg->Clear();
            wxArrayString lines = GetArrayFromString(output, _T('\n'));
            for (unsigned int i = 0; i < lines.GetCount(); ++i)
            {
                // reBT1 matches frame number, address, function and args (common to all formats)
                // reBT2 matches filename and line (optional)
                // reBT3 matches filename only (for DLLs) (optional)

                StackFrame sf;
                bool matched = false;
                // #0  main (argc=1, argv=0x3e2440) at my main.cpp:15
                if (reBTX.Matches(lines[i]))
                {
//                    m_pDriver->DebugLog(_T("MATCH!"));
                    reBTX.GetMatch(lines[i], 1).ToULong(&sf.number);
                    reBTX.GetMatch(lines[i], 2).ToULong(&sf.address, 16);
                    sf.function = reBTX.GetMatch(lines[i], 3) + reBTX.GetMatch(lines[i], 4);
                    matched = true;
                }
                else if (reBT1.Matches(lines[i]))
                {
//                    m_pDriver->DebugLog(_T("MATCH!"));
                    reBT1.GetMatch(lines[i], 1).ToULong(&sf.number);
                    reBT1.GetMatch(lines[i], 2).ToULong(&sf.address, 16);
                    sf.function = reBT1.GetMatch(lines[i], 3) + reBT1.GetMatch(lines[i], 4);
                    matched = true;
                }
                else if (reBT0.Matches(lines[i]))
                {
                    reBT0.GetMatch(lines[i], 1).ToULong(&sf.number);
                    sf.address = 0;
                    sf.function = reBT0.GetMatch(lines[i], 2) + reBT0.GetMatch(lines[i], 3);
                    matched = true;
                }

                if (matched)
                {
                    sf.valid = true;
                    if (reBT2.Matches(lines[i]))
                    {
                        sf.file = reBT2.GetMatch(lines[i], 1);
                        sf.line = reBT2.GetMatch(lines[i], 2);
                        if (validFrameNumber == -1)
                        {
                            validSF = sf;
                            validFrameNumber = sf.number;
                        }
                    }
                    else if (reBT3.Matches(lines[i]))
                        sf.file = reBT3.GetMatch(lines[i], 1);
                    m_pDlg->AddFrame(sf);
                }
            }
            if (validFrameNumber > 0) // if it's 0, then the driver already synced the editor
            {
                bool autoSwitch = Manager::Get()->GetConfigManager(_T("debugger"))->ReadBool(_T("auto_switch_frame"), true);
                if (!autoSwitch)
                {
                    long line;
                    if (validSF.line.ToLong(&line))
                    {
                        m_pDriver->Log(wxString::Format(_T("Displaying first frame with valid source info (#%d)"), validFrameNumber));
                        m_pDriver->ShowFile(validSF.file, line);
                    }
                }
                else
                {
                    // can't call m_pDriver->SwitchToFrame() here
                    // because it causes a cascade update, never stopping...
                    //m_pDriver->Log(wxString::Format(_T("Switching to frame #%d which has valid source info"), validFrameNumber));
                    m_pDriver->QueueCommand(new DebuggerCmd(m_pDriver, wxString::Format(_T("frame %d"), validFrameNumber)));
                }
            }
//            m_pDriver->DebugLog(output);
        }
};

/**
  * Command to run a disassembly. Use this instead of GdbCmd_DisassemblyInit, which is chained-called.
  */
class GdbCmd_InfoRegisters : public DebuggerCmd
{
        CPURegistersDlg* m_pDlg;
        wxString m_disassemblyFlavor;

    public:
        /** @param dlg The disassembly dialog. */

        // only tested on mingw/pc/win env
        GdbCmd_InfoRegisters(DebuggerDriver* driver, CPURegistersDlg* dlg, wxString disassemblyFlavor = wxEmptyString)
            : DebuggerCmd(driver),
            m_pDlg(dlg), m_disassemblyFlavor(disassemblyFlavor)
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

            if (!m_pDlg)
                return;
            // or32 register string parser
            if(m_disassemblyFlavor == _T("set disassembly-flavor or32"))
            {
                ParseOutputFromOR32gdbPort(output);
            }
            else
            // use generic parser - this may work for other platforms or you may have to write your own
            {
                wxArrayString lines = GetArrayFromString(output, _T('\n'));
                for (unsigned int i = 0; i < lines.GetCount(); ++i)
                {
                    if (reRegisters.Matches(lines[i]))
                    {
                        long int addr = wxStrHexTo<long int>(reRegisters.GetMatch(lines[i], 2));
                        m_pDlg->SetRegisterValue(reRegisters.GetMatch(lines[i], 1), addr);
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
                    {
                        unsigned long int addrL;
                        addr.ToULong(&addrL, 16);
                        m_pDlg->SetRegisterValue(reg, addrL);
                    }
                }
            }
        }
};

/**
  * Command to run a disassembly.
  */
class GdbCmd_Disassembly : public DebuggerCmd
{
        DisassemblyDlg* m_pDlg;
    public:
        /** @param dlg The disassembly dialog. */
        GdbCmd_Disassembly(DebuggerDriver* driver, DisassemblyDlg* dlg)
            : DebuggerCmd(driver),
            m_pDlg(dlg)
        {
            m_Cmd << _T("disassemble");
        }
        void ParseOutput(const wxString& output)
        {
            // output is a series of:
            //
            // Dump of assembler code for function main:
            // 0x00401390 <main+0>:    push   ebp
            // ...
            // End of assembler dump.

            if (!m_pDlg)
                return;

            wxArrayString lines = GetArrayFromString(output, _T('\n'));
            for (unsigned int i = 0; i < lines.GetCount(); ++i)
            {
                if (reDisassembly.Matches(lines[i]))
                {
                    unsigned long int addr;
                    reDisassembly.GetMatch(lines[i], 1).ToULong(&addr, 16);
                    m_pDlg->AddAssemblerLine(addr, reDisassembly.GetMatch(lines[i], 2));
                }
            }
//            m_pDlg->Show(true);
//            m_pDriver->DebugLog(output);
        }
};

/**
  * Command to initialize a disassembly. Use this instead of GdbCmd_Disassembly, which is chain-called by this.
  */
class GdbCmd_DisassemblyInit : public DebuggerCmd
{
        DisassemblyDlg* m_pDlg;
        wxString m_disassemblyFlavor;

    public:
        static wxString LastAddr;
        /** @param dlg The disassembly dialog. */

        // only tested on mingw/pc/win env
        GdbCmd_DisassemblyInit(DebuggerDriver* driver, DisassemblyDlg* dlg, wxString disassemblyFlavor = wxEmptyString)
            : DebuggerCmd(driver),
            m_pDlg(dlg), m_disassemblyFlavor(disassemblyFlavor)
        {
            m_Cmd << _T("info frame");
        };

        void ParseOutput(const wxString& output)
        {
            if (!m_pDlg)
                return;

            if (reDisassemblyInit.Matches(output))
            {
                StackFrame sf;
                wxString addr = reDisassemblyInit.GetMatch(output, 1);
                if (addr == LastAddr)
                    return;
                LastAddr = addr;
                addr.ToULong((unsigned long int*)&sf.address, 16);

                if (reDisassemblyInitFunc.Matches(output))
                {
                    sf.function = reDisassemblyInitFunc.GetMatch(output, 2);
                    long int active;

                    if(platform::windows && m_disassemblyFlavor == _T("set disassembly-flavor or32"))
                    {
                        reDisassemblyInitFuncOR32.GetMatch(output, 1).ToLong(&active, 16);
                    }
                    else
                    {
                        reDisassemblyInitFunc.GetMatch(output, 1).ToLong(&active, 16);
                    }
                    m_pDlg->SetActiveAddress(active);
                }

                sf.valid = true;
                m_pDlg->Clear(sf);
                m_pDriver->QueueCommand(new GdbCmd_Disassembly(m_pDriver, m_pDlg)); // chain call
            }
//            m_pDriver->DebugLog(output);
        }
};
// static
wxString GdbCmd_DisassemblyInit::LastAddr;

/**
  * Command to examine a memory region.
  */
class GdbCmd_ExamineMemory : public DebuggerCmd
{
        ExamineMemoryDlg* m_pDlg;
    public:
        /** @param dlg The memory dialog. */
        GdbCmd_ExamineMemory(DebuggerDriver* driver, ExamineMemoryDlg* dlg)
            : DebuggerCmd(driver),
            m_pDlg(dlg)
        {
            m_Cmd.Printf(_T("x/%dxb %s"), dlg->GetBytes(), dlg->GetBaseAddress().c_str());
        }
        void ParseOutput(const wxString& output)
        {
            // output is a series of:
            //
            // 0x22ffc0:       0xf0    0xff    0x22    0x00    0x4f    0x6d    0x81    0x7c

            if (!m_pDlg)
                return;
            m_pDlg->Begin();
            m_pDlg->Clear();

            wxArrayString lines = GetArrayFromString(output, _T('\n'));
            for (unsigned int i = 0; i < lines.GetCount(); ++i)
            {
                if (lines[i].First(_T(':')) == -1)
                {
                    m_pDlg->AddError(lines[i]);
                    continue;
                }
                wxString addr = lines[i].BeforeFirst(_T(':'));
                size_t pos = lines[i].find(_T('x'), lines[i].rfind(_T(':'))); // skip 'x' of address
                while (pos != wxString::npos)
                {
                    wxString hexbyte;
                    hexbyte << lines[i][pos + 1];
                    hexbyte << lines[i][pos + 2];
                    m_pDlg->AddHexByte(addr, hexbyte);
                    pos = lines[i].find(_T('x'), pos + 1); // skip current 'x'
                }
            }
            m_pDlg->End();
//            m_pDriver->DebugLog(output);
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
        void ParseOutput(const wxString& /*output*/)
        {
        }
};

class GdbCmd_RemoteTarget : public DebuggerCmd
{
    public:
        GdbCmd_RemoteTarget(DebuggerDriver* driver, RemoteDebugging* rd)
            : DebuggerCmd(driver)
        {
            switch (rd->connType)
            {
                case RemoteDebugging::TCP:
                {
                    if (!rd->ip.IsEmpty() && !rd->ipPort.IsEmpty())
                        m_Cmd << _T("target remote tcp:") << rd->ip << _T(":") << rd->ipPort;
                }
                break;

                case RemoteDebugging::UDP:
                {
                    if (!rd->ip.IsEmpty() && !rd->ipPort.IsEmpty())
                        m_Cmd << _T("target remote udp:") << rd->ip << _T(":") << rd->ipPort;
                }
                break;

                case RemoteDebugging::Serial:
                {
                    if (!rd->serialPort.IsEmpty())
                        m_Cmd << _T("target remote ") << rd->serialPort;
                }
                break;

                default:
                    break;
            }

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

#endif // DEBUGGER_COMMANDS_H
