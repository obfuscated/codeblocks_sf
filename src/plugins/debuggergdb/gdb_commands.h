#ifndef GDB_DEBUGGER_COMMANDS_H
#define GDB_DEBUGGER_COMMANDS_H

#include <wx/string.h>
#include <wx/regex.h>
#include <wx/tipwin.h>
#include <globals.h>
#include <manager.h>
#include "debugger_defs.h"
#include "debuggergdb.h"
#include "debuggertree.h"
#include "backtracedlg.h"

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
            m_Cmd << _T("attach ") << pid;
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
            if (bp->enabled)
            {
                if (bp->func.IsEmpty())
                {
                    wxString out = m_BP->filename;
                    DebuggerGDB::ConvertToGDBFile(out);
                    QuoteStringIfNeeded(out);
                    // we add one to line,  because scintilla uses 0-based line numbers, while gdb uses 1-based
                    if (!m_BP->temporary)
                        m_Cmd << _T("break ");
                    else
                        m_Cmd << _T("tbreak ");
                    m_Cmd << out << _T(":") << bp->line + 1;
                }
                //GDB workaround
                //Use function name if this is C++ constructor/destructor
                else
                {
                    if (!m_BP->temporary)
                        m_Cmd << _T("break ");
                    else
                        m_Cmd << _T("tbreak ");
                    m_Cmd << bp->func;
                }
                //end GDB workaround

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
            wxRegEx re(_T("Breakpoint ([0-9]+) at (0x[0-9A-Fa-f]+)"));
            if (re.Matches(output))
            {
//                m_pDriver->DebugLog(wxString::Format(_("Breakpoint added: file %s, line %d"), m_BP->filename.c_str(), m_BP->line + 1));
                if (!m_BP->func.IsEmpty())
                    m_pDriver->DebugLog(_("(work-around for constructors activated)"));

                re.GetMatch(output, 1).ToLong(&m_BP->bpNum);
                re.GetMatch(output, 2).ToULong(&m_BP->address, 16);

                // conditional breakpoint
                if (m_BP->useCondition && !m_BP->condition.IsEmpty())
                {
                    wxString cmd;
                    cmd << _T("condition ") << m_BP->bpNum << _T(" ") << m_BP->condition;
                    m_pDriver->QueueCommand(new DebuggerCmd(m_pDriver, cmd), DebuggerDriver::High);
                }

                // ignore count
                if (m_BP->useIgnoreCount && m_BP->ignoreCount > 0)
                {
                    wxString cmd;
                    cmd << _T("ignore ") << m_BP->bpNum << _T(" ") << m_BP->ignoreCount;
                    m_pDriver->QueueCommand(new DebuggerCmd(m_pDriver, cmd), DebuggerDriver::High);
                }

                if (m_BP->temporary)
                    delete m_BP;
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
        /** @param bp The breakpoint to remove. If NULL, all breakpoints are removed.
            @param deleteBPwhenDone If bp is not NULL and this is true the breakpoint will be deleted after removal. */
        GdbCmd_RemoveBreakpoint(DebuggerDriver* driver, DebuggerBreakpoint* bp, bool deleteBPwhenDone = false)
            : DebuggerCmd(driver),
            m_BP(bp),
            m_DeleteBPwhenDone(deleteBPwhenDone)
        {
            if (!bp)
            {
                m_Cmd << _T("delete");
                return;
            }

            if (bp->enabled && bp->bpNum > 0)
            {
                m_Cmd << _T("delete ") << bp->bpNum;
            }
        }
        void ParseOutput(const wxString& output)
        {
            if (!m_BP)
                return;

            // invalidate bp number
            m_BP->bpNum = -1;

            if (!output.IsEmpty())
                m_pDriver->Log(output);
//            m_pDriver->DebugLog(wxString::Format(_("Breakpoint removed: file %s, line %d"), m_BP->filename.c_str(), m_BP->line + 1));
            if (m_DeleteBPwhenDone)
                delete m_BP;
        }

        DebuggerBreakpoint* m_BP;
        bool m_DeleteBPwhenDone;
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
            m_pDTree->BuildTree(locals);
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
            m_pDTree->BuildTree(args);
        }
};

/**
  * Command to get info about a watched variable.
  */
class GdbCmd_Watch : public DebuggerCmd
{
        DebuggerTree* m_pDTree;
        Watch* m_pWatch;
    public:
        /** @param tree The tree to display the watch. */
        GdbCmd_Watch(DebuggerDriver* driver, DebuggerTree* dtree, Watch* watch)
            : DebuggerCmd(driver),
            m_pDTree(dtree),
            m_pWatch(watch)
        {
            m_Cmd << _T("output ") << Watch::FormatCommand(m_pWatch->format) << _T(" ") << m_pWatch->keyword;
        }
        void ParseOutput(const wxString& output)
        {
            wxArrayString lines = GetArrayFromString(output, _T('\n'));
            wxString w;
    		w << m_pWatch->keyword << _T(" = ");
    		for (unsigned int i = 0; i < lines.GetCount(); ++i)
                w << lines[i] << _T(',');
            w << _T('\n');
            m_pDTree->BuildTree(w);
        }
};

/**
  * Command to display a tooltip about a variables value.
  */
class GdbCmd_TooltipEvaluation : public DebuggerCmd
{
        DebuggerTree* m_pDTree;
        wxTipWindow** m_pWin;
        wxRect m_WinRect;
        wxString m_What;
    public:
        /** @param what The variable to evaluate.
            @param win A pointer to the tip window pointer.
            @param tiprect The tip window's rect.
        */
        GdbCmd_TooltipEvaluation(DebuggerDriver* driver, const wxString& what, wxTipWindow** win, const wxRect& tiprect)
            : DebuggerCmd(driver),
            m_pWin(win),
            m_WinRect(tiprect),
            m_What(what)
        {
            m_Cmd << _T("output ") << what;
        }
        void ParseOutput(const wxString& output)
        {
            wxString tip;
            if (output.StartsWith(_T("No symbol ")) || output.StartsWith(_T("Attempt to ")))
                tip = output;
            else
                tip = m_What + _T("=") + output;

            if (*m_pWin)
                (*m_pWin)->Destroy();
            *m_pWin = new wxTipWindow(Manager::Get()->GetAppWindow(), tip, 640, m_pWin, &m_WinRect);
//            m_pDriver->DebugLog(output);
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
            m_Cmd << _T("bt");
        }
        void ParseOutput(const wxString& output)
        {
            // output is a series of:
            // #0  main () at main.cpp:8
            wxArrayString lines = GetArrayFromString(output, _T('\n'));
    		for (unsigned int i = 0; i < lines.GetCount(); ++i)
    		{
    		    // #0  main (argc=1, argv=0x3e2440) at my main.cpp:15
//    		    wxRegEx re(_T("#([0-9]+)[ \t]+([A-Za-z0-9_:]+)[ \t]+\\([^)]*\\)[ \t]+at[ \t]+(.*):([0-9]+)"));
    		    // #0  0x004013cf in main () at main.cpp:11
    		    wxRegEx re(_T("#([0-9]+)[ \t]+(0x[A-Fa-f0-9]+)[ \t]+in[ \t]+([A-Za-z0-9_:]+)[ \t]+\\([^)]*\\)[ \t]+at[ \t]+(.*):([0-9]+)"));
    		    if (re.Matches(lines[i]))
    		    {
                    m_pDriver->DebugLog(_T("MATCH!"));
                    StackFrame sf;
                    sf.valid = true;
    		        re.GetMatch(lines[i], 1).ToLong(&sf.number);
    		        re.GetMatch(lines[i], 2).ToULong(&sf.address, 16);
    		        sf.function = re.GetMatch(lines[i], 3);
    		        sf.file = re.GetMatch(lines[i], 4);
    		        sf.line = re.GetMatch(lines[i], 5);
                    m_pDlg->AddFrame(sf);
    		    }
    		}
//            m_pDriver->DebugLog(output);
        }
};

/**
  * Command to initialize a disassembly.
  */
class GdbCmd_DisassemblyInit : public DebuggerCmd
{
        DisassemblyDlg* m_pDlg;
    public:
        /** @param dlg The disassembly dialog. */
        GdbCmd_DisassemblyInit(DebuggerDriver* driver, DisassemblyDlg* dlg)
            : DebuggerCmd(driver),
            m_pDlg(dlg)
        {
            m_Cmd << _T("frame");
        }
        void ParseOutput(const wxString& output)
        {
            // output is two lines describing the current frame:
            //
            // #0  main () at main.cpp:8
            // C:/Devel/tmp/console/main.cpp:8:63:beg:0x4013ba

            if (!m_pDlg)
                return;

            StackFrame sf;
            wxArrayString lines = GetArrayFromString(output, _T('\n'));
    		for (unsigned int i = 0; i < lines.GetCount(); ++i)
    		{
                if (!lines[i].StartsWith(g_EscapeChars)) // ->->
                {
                    // #0  main () at main.cpp:8
                    wxRegEx re(_T("#([0-9]+)[ \t]+([A-Za-z0-9_:]+) \\(\\) at"));
                    if (re.Matches(lines[i]))
                    {
                        re.GetMatch(lines[i], 1).ToLong(&sf.number);
                        sf.function = re.GetMatch(lines[i], 2);
                    }
                }
                else
                {
                    // C:/Devel/tmp/console/main.cpp:11:113:beg:0x4013cf
                    lines[i].Remove(0, 2); // remove ->->
                    wxRegEx reSource;
                    #ifdef __WXMSW__
                    reSource.Compile(_T("([A-Za-z]:)([ A-Za-z0-9_/\\.~-]*):([0-9]*):[0-9]*:[begmidl]+:(0x[0-9A-Za-z]*)"));
                    #else
                    reSource.Compile(_T("([ A-Za-z0-9_/\\.~-]*):([0-9]*):[0-9]*:[begmidl]+:(0x[0-9A-Za-z]*)"));
                    #endif
                    if ( reSource.Matches(lines[i]) )
                    {
                        sf.valid = true;
                        #ifdef __WXMSW__
                        sf.file = reSource.GetMatch(lines[i], 1) + reSource.GetMatch(lines[i], 2); // drive + path
                        sf.line = reSource.GetMatch(lines[i], 3);
                        reSource.GetMatch(lines[i], 4).ToULong(&sf.address, 16);
                        #else
                        sf.file = reSource.GetMatch(lines[i], 1);
                        sf.line = reSource.GetMatch(lines[i], 2);
                        reSource.GetMatch(lines[i], 3).ToULong(&sf.address, 16);
                        #endif
                        break; // we 're only interested for the top-level stack frame
                    }
                }
    		}
            m_pDlg->Clear(sf);
//            m_pDriver->DebugLog(output);
        }
};

/**
  * Command to run a disassembly. Use this instead of GdbCmd_DisassemblyInit, which is chained-called.
  */
class GdbCmd_InfoRegisters : public DebuggerCmd
{
        DisassemblyDlg* m_pDlg;
    public:
        /** @param dlg The disassembly dialog. */
        GdbCmd_InfoRegisters(DebuggerDriver* driver, DisassemblyDlg* dlg)
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
                // eax            0x40e66666       1088841318
                wxRegEx re(_T("([A-Za-z0-9]+)[ \t]+(0x[0-9A-Za-z]+)"));
                if (re.Matches(lines[i]))
                {
                    long int addr;
                    re.GetMatch(lines[i], 2).ToLong(&addr, 16);
                    m_pDlg->SetRegisterValue(DisassemblyDlg::RegisterIndexFromName(re.GetMatch(lines[i], 1)), addr);
                }
    		}
//            m_pDlg->Show(true);
//            m_pDriver->DebugLog(output);
        }
};

/**
  * Command to run a disassembly. Use this instead of GdbCmd_DisassemblyInit, which is chained-called.
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
            m_pDriver->QueueCommand(new GdbCmd_DisassemblyInit(driver, dlg)); // chain call
            m_pDriver->QueueCommand(new GdbCmd_InfoRegisters(driver, dlg)); // chain call
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
                // 0x00401390 <main+0>:	push   ebp
                wxRegEx re(_T("(0x[0-9A-Za-z]+)[ \t]+<.*>:[ \t]+(.*)"));
                if (re.Matches(lines[i]))
                {
                    long int addr;
                    re.GetMatch(lines[i], 1).ToLong(&addr, 16);
                    m_pDlg->AddAssemblerLine(addr, re.GetMatch(lines[i], 2));
                }
    		}
            m_pDlg->Show(true);
//            m_pDriver->DebugLog(output);
        }
};

#endif // DEBUGGER_COMMANDS_H
