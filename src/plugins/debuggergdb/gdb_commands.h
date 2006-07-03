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
#include <globals.h>
#include <manager.h>
#include <scriptingmanager.h>
#include <sqplus.h>
#include "debugger_defs.h"
#include "debuggergdb.h"
#include "gdb_driver.h"
#include "debuggertree.h"
#include "backtracedlg.h"
#include "examinememorydlg.h"
#include "threadsdlg.h"

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
      switch (str[pos])
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
static wxRegEx reBT0(_T("#([0-9]+)[ \t]+([^( \t]+)[ \t]+(\\([^)]*\\))")); // case #0
static wxRegEx reBT1(_T("#([0-9]+)[ \t]+0x([A-f0-9]+)[ \t]+in[ \t]+([^( \t]+)[ \t]+(\\([^)]*\\))")); // all other cases
static wxRegEx reBT2(_T("\\)[ \t]+[atfrom]+[ \t]+(.*):([0-9]+)"));
static wxRegEx reBT3(_T("\\)[ \t]+[atfrom]+[ \t]+(.*)"));
// Breakpoint 1 at 0x4013d6: file main.cpp, line 8.
static wxRegEx reBreakpoint(_T("Breakpoint ([0-9]+) at (0x[0-9A-Fa-f]+)"));
// eax            0x40e66666       1088841318
static wxRegEx reRegisters(_T("([A-z0-9]+)[ \t]+(0x[0-9A-Fa-f]+)[ \t]+(.*)"));
// 0x00401390 <main+0>:	push   ebp
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
//	Using the running image of child Thread 46912568064384 (LWP 7051).
static wxRegEx reInfoProgramThread(_T("\\(LWP[ \t]([0-9]+)\\)"));
//	Using the running image of child process 10011.
static wxRegEx reInfoProgramProcess(_T("child process ([0-9]+)"));
//  2 Thread 1082132832 (LWP 8017)  0x00002aaaac5a2aca in pthread_cond_wait@@GLIBC_2.3.2 () from /lib/libpthread.so.0
//* 1 Thread 46912568064384 (LWP 7926)  0x00002aaaac76e612 in poll () from /lib/libc.so.6
static wxRegEx reInfoThreads(_T("(\\**)[ \t]*([0-9]+)[ \t](.*)[ \t]in"));

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
        void ParseOutput(const wxString& output)
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
  * Command to add a breakpoint.
  */
class GdbCmd_AddBreakpoint : public DebuggerCmd
{
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
                if (m_BP->func.IsEmpty())
                {
                    wxString out = m_BP->filename;
                    DebuggerGDB::ConvertToGDBFile(out);
                    QuoteStringIfNeeded(out);
                    // we add one to line,  because scintilla uses 0-based line numbers, while gdb uses 1-based
                    if (!m_BP->temporary)
                        m_Cmd << _T("break ");
                    else
                        m_Cmd << _T("tbreak ");
                    m_Cmd << out << _T(":") << wxString::Format(_T("%d"), m_BP->line + 1);
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
            // possible outputs (we 're only interested in 1st sample):
            //
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
                    wxString cmd;
                    cmd << _T("condition ") << wxString::Format(_T("%d"), (int) m_BP->index) << _T(" ") << m_BP->condition;
                    m_pDriver->QueueCommand(new DebuggerCmd(m_pDriver, cmd), DebuggerDriver::High);
                }

                // ignore count
                if (m_BP->useIgnoreCount && m_BP->ignoreCount > 0)
                {
                    wxString cmd;
                    cmd << _T("ignore ") << wxString::Format(_T("%d"), (int) m_BP->index) << _T(" ") << wxString::Format(_T("%d"), (int) m_BP->ignoreCount);
                    m_pDriver->QueueCommand(new DebuggerCmd(m_pDriver, cmd), DebuggerDriver::High);
                }
            }
            else
                m_pDriver->Log(output); // one of the error responses
        }

        DebuggerBreakpoint* m_BP;
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

            // invalidate bp number
            m_BP->index = -1;

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
                unsigned long pid;
                if (pid_str.ToULong(&pid, 10) && pid != 0)
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
//    		    m_pDriver->Log(lines[i]);
    		    if (reInfoThreads.Matches(lines[i]))
    		    {
//    		        m_pDriver->Log(_T("MATCH!"));
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
                    default:            break;
                }
                m_Cmd << m_pWatch->keyword;
            }
            else
            {
                try
                {
                    m_Cmd = SqPlus::SquirrelFunction<wxString&>(cbU2C(m_Cmd))(w_type, m_pWatch->keyword, watch->array_start, watch->array_count);
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
    		w << m_pWatch->keyword << _T(" = ");
            if (!m_ParseFunc.IsEmpty())
            {
                try
                {
                    w << SqPlus::SquirrelFunction<wxString&>(cbU2C(m_ParseFunc))(output, m_pWatch->array_start);
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
        }
        void ParseOutput(const wxString& output)
        {
            // examples:
            // type = wxString
            // type = const wxChar
            // type = Action *
            // type = bool

            wxString tmp = output.AfterFirst(_T('='));
            // actually add this watch with high priority
            m_pDriver->QueueCommand(new GdbCmd_Watch(m_pDriver, m_pDTree, m_pWatch, tmp), DebuggerDriver::High);
        }
};

/**
  * Command to display a tooltip about a variables value.
  */
class GdbCmd_TooltipEvaluation : public DebuggerCmd
{
        wxTipWindow** m_pWin;
        wxRect m_WinRect;
        wxString m_What;
        wxString m_ParseFunc;
    public:
        /** @param what The variable to evaluate.
            @param win A pointer to the tip window pointer.
            @param tiprect The tip window's rect.
        */
        GdbCmd_TooltipEvaluation(DebuggerDriver* driver, const wxString& what, wxTipWindow** win, const wxRect& tiprect, const wxString& w_type = wxEmptyString)
            : DebuggerCmd(driver),
            m_pWin(win),
            m_WinRect(tiprect),
            m_What(what)
        {
            m_Cmd = static_cast<GDB_driver*>(m_pDriver)->GetScriptedTypeCommand(w_type, m_ParseFunc);
            if (m_Cmd.IsEmpty())
            {
                m_Cmd << _T("output ");
                m_Cmd << what;
            }
            else
            {
                try
                {
                    m_Cmd = SqPlus::SquirrelFunction<wxString&>(cbU2C(m_Cmd))(w_type, what, 0, 0);
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
            wxString tip;
            if (output.StartsWith(_T("No symbol ")) || output.StartsWith(_T("Attempt to ")))
                tip = output;
            else
            {
                tip = m_What + _T("=");
    		    if (!m_ParseFunc.IsEmpty())
    		    {
    		        try
    		        {
                        tip << SqPlus::SquirrelFunction<wxString&>(cbU2C(m_ParseFunc))(output, 0);
//                        tip << SqPlus::SquirrelFunction<wxString>(cbU2C(m_ParseFunc))(output, 0);
                    }
                    catch (SquirrelError e)
                    {
                        tip << cbC2U(e.desc);
                        m_pDriver->DebugLog(_T("Script exception: ") + tip);
                    }
    		    }
                else
                    tip << output;
            }

            if (*m_pWin)
                (*m_pWin)->Destroy();
            *m_pWin = new wxTipWindow(Manager::Get()->GetAppWindow(), tip, 640, m_pWin, &m_WinRect);
//            m_pDriver->DebugLog(output);
        }
};

/**
  * Command to get a symbol's type and use it for tooltip evaluation.
  */
class GdbCmd_FindTooltipType : public DebuggerCmd
{
        wxTipWindow** m_pWin;
        wxRect m_WinRect;
        wxString m_What;
    public:
        /** @param tree The tree to display the watch. */
        GdbCmd_FindTooltipType(DebuggerDriver* driver, const wxString& what, wxTipWindow** win, const wxRect& tiprect)
            : DebuggerCmd(driver),
            m_pWin(win),
            m_WinRect(tiprect),
            m_What(what)
        {
            m_Cmd << _T("whatis ");
            m_Cmd << m_What;
        }
        void ParseOutput(const wxString& output)
        {
            // examples:
            // type = wxString
            // type = const wxChar
            // type = Action *
            // type = bool

            wxString tmp = output.AfterFirst(_T('='));
            // actually add this watch with high priority
            m_pDriver->QueueCommand(new GdbCmd_TooltipEvaluation(m_pDriver, m_What, m_pWin, m_WinRect, tmp), DebuggerDriver::High);
        }
};

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
    		    if (reBT1.Matches(lines[i]))
    		    {
//                    m_pDriver->DebugLog(_T("MATCH!"));
    		        reBT1.GetMatch(lines[i], 1).ToLong(&sf.number);
    		        reBT1.GetMatch(lines[i], 2).ToULong(&sf.address, 16);
    		        sf.function = reBT1.GetMatch(lines[i], 3) + reBT1.GetMatch(lines[i], 4);
    		        matched = true;
    		    }
    		    else if (reBT0.Matches(lines[i]))
    		    {
    		        reBT0.GetMatch(lines[i], 1).ToLong(&sf.number);
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
                    }
                    else if (reBT3.Matches(lines[i]))
                        sf.file = reBT3.GetMatch(lines[i], 1);
                    m_pDlg->AddFrame(sf);
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
    public:
        /** @param dlg The disassembly dialog. */
        GdbCmd_InfoRegisters(DebuggerDriver* driver, CPURegistersDlg* dlg)
            : DebuggerCmd(driver),
            m_pDlg(dlg)
        {
            m_Cmd << _T("info registers");
        }
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

            wxArrayString lines = GetArrayFromString(output, _T('\n'));
    		for (unsigned int i = 0; i < lines.GetCount(); ++i)
    		{
                if (reRegisters.Matches(lines[i]))
                {
                    long int addr = wxStrHexTo<long int>(reRegisters.GetMatch(lines[i], 2));
                    m_pDlg->SetRegisterValue(reRegisters.GetMatch(lines[i], 1), addr);
                }
    		}
//            m_pDlg->Show(true);
//            m_pDriver->DebugLog(output);
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
            // 0x00401390 <main+0>:	push   ebp
            // ...
            // End of assembler dump.

            if (!m_pDlg)
                return;

            wxArrayString lines = GetArrayFromString(output, _T('\n'));
    		for (unsigned int i = 0; i < lines.GetCount(); ++i)
    		{
                if (reDisassembly.Matches(lines[i]))
                {
                    long int addr;
                    reDisassembly.GetMatch(lines[i], 1).ToLong(&addr, 16);
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
        static wxString LastAddr;
    public:
        /** @param dlg The disassembly dialog. */
        GdbCmd_DisassemblyInit(DebuggerDriver* driver, DisassemblyDlg* dlg)
            : DebuggerCmd(driver),
            m_pDlg(dlg)
        {
            m_Cmd << _T("info frame");
        }
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
                addr.ToLong((long int*)&sf.address, 16);

                if (reDisassemblyInitFunc.Matches(output))
                {
                    sf.function = reDisassemblyInitFunc.GetMatch(output, 2);
                    long int active;
                    reDisassemblyInitFunc.GetMatch(output, 1).ToLong(&active, 16);
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
    		    size_t pos = lines[i].find(_T('x'), 3); // skip 'x' of address
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

#endif // DEBUGGER_COMMANDS_H
