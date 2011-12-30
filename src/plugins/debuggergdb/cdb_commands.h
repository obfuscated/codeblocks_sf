/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef CDB_DEBUGGER_COMMANDS_H
#define CDB_DEBUGGER_COMMANDS_H

#include <wx/string.h>
#include <wx/regex.h>
#include <wx/tipwin.h>
#include <globals.h>
#include <manager.h>
#include "debugger_defs.h"
#include "debuggergdb.h"
#include "debuggertree.h"
#include "backtracedlg.h"

static wxRegEx reWatch(_T("(\\+0x[A-Fa-f0-9]+ )"));
static wxRegEx reBT1(_T("([0-9]+) ([A-Fa-f0-9]+) ([A-Fa-f0-9]+) ([^[]*)"));
static wxRegEx reBT2(_T("\\[([A-z]:)(.*) @ ([0-9]+)\\]"));
//    15 00401020 55               push    ebp
//    61 004010f9 ff15dcc24000  call dword ptr [Win32GUI!_imp__GetMessageA (0040c2dc)]
//    71 0040111f c21000           ret     0x10
static wxRegEx reDisassembly(_T("^[0-9]+[ \t]+([A-Fa-f0-9]+)[ \t]+[A-Fa-f0-9]+[ \t]+(.*)$"));
//  # ChildEBP RetAddr
// 00 0012fe98 00401426 Win32GUI!WinMain+0x89 [c:\devel\tmp\win32 test\main.cpp @ 55]
static wxRegEx reDisassemblyFile(_T("[0-9]+[ \t]+([A-Fa-f0-9]+)[ \t]+[A-Fa-f0-9]+[ \t]+(.*)\\[([A-z]:)(.*) @ ([0-9]+)\\]"));
static wxRegEx reDisassemblyFunc(_T("^\\(([A-Fa-f0-9]+)\\)[ \t]+"));

/**
  * Command to add a search directory for source files in debugger's paths.
  */
class CdbCmd_AddSourceDir : public DebuggerCmd
{
    public:
        /** If @c dir is empty, resets all search dirs to $cdir:$cwd, the default. */
        CdbCmd_AddSourceDir(DebuggerDriver* driver, const wxString& dir)
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
class CdbCmd_SetDebuggee : public DebuggerCmd
{
    public:
        /** @param file The file to debug. */
        CdbCmd_SetDebuggee(DebuggerDriver* driver, const wxString& file)
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
class CdbCmd_AddSymbolFile : public DebuggerCmd
{
    public:
        /** @param file The file which contains the symbols. */
        CdbCmd_AddSymbolFile(DebuggerDriver* driver, const wxString& file)
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
class CdbCmd_SetArguments : public DebuggerCmd
{
    public:
        /** @param file The file which contains the symbols. */
        CdbCmd_SetArguments(DebuggerDriver* driver, const wxString& args)
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
class CdbCmd_AttachToProcess : public DebuggerCmd
{
    public:
        /** @param file The file to debug. */
        CdbCmd_AttachToProcess(DebuggerDriver* driver, int pid)
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
class CdbCmd_Detach : public DebuggerCmd
{
    public:
        /** @param file The file to debug. */
        CdbCmd_Detach(DebuggerDriver* driver)
            : DebuggerCmd(driver)
        {
            m_Cmd << _T(".detach");
        }
        void ParseOutput(const wxString& output)
        {
            // output any return, usually "Detached"
            m_pDriver->Log(output);
        }
};

/**
  * Command to add a breakpoint.
  */
class CdbCmd_AddBreakpoint : public DebuggerCmd
{
    public:
        /** @param bp The breakpoint to set. */
        CdbCmd_AddBreakpoint(DebuggerDriver* driver, DebuggerBreakpoint* bp)
            : DebuggerCmd(driver),
            m_BP(bp)
        {
            if (bp->enabled)
            {
                wxString out = m_BP->filename;
//                DebuggerGDB::ConvertToGDBFile(out);
                QuoteStringIfNeeded(out);
                // we add one to line,  because scintilla uses 0-based line numbers, while cdb uses 1-based
                m_Cmd << _T("bu") << wxString::Format(_T("%d"), (int) m_BP->index) << _T(' ');
                if (m_BP->temporary)
                    m_Cmd << _T("/1 ");
                if (bp->func.IsEmpty())
                    m_Cmd << _T('`') << out << _T(":") << wxString::Format(_T("%d"), bp->line + 1) << _T('`');
                else
                    m_Cmd << bp->func;
                bp->alreadySet = true;
            }
        }
        void ParseOutput(const wxString& output)
        {
            // possible outputs (only output lines starting with ***):
            //
            // *** WARNING: Unable to verify checksum for Win32GUI.exe
            // *** ERROR: Symbol file could not be found.  Defaulted to export symbols for C:\WINDOWS\system32\USER32.dll -
            // *** ERROR: Symbol file could not be found.  Defaulted to export symbols for C:\WINDOWS\system32\GDI32.dll -
            wxArrayString lines = GetArrayFromString(output, _T('\n'));
            for (unsigned int i = 0; i < lines.GetCount(); ++i)
            {
                if (lines[i].StartsWith(_T("*** ")))
                    m_pDriver->Log(lines[i]);
            }
        }

        DebuggerBreakpoint* m_BP;
};

/**
  * Command to remove a breakpoint.
  */
class CdbCmd_RemoveBreakpoint : public DebuggerCmd
{
    public:
        /** @param bp The breakpoint to remove. If NULL, all breakpoints are removed. */
        CdbCmd_RemoveBreakpoint(DebuggerDriver* driver, DebuggerBreakpoint* bp)
            : DebuggerCmd(driver),
            m_BP(bp)
        {
            if (!bp)
                m_Cmd << _T("bc *");
            else
                m_Cmd << _T("bc ") << wxString::Format(_T("%d"), (int) bp->index);
        }
        void ParseOutput(const wxString& output)
        {
            // usually no output, so display whatever comes in
            if (!output.IsEmpty())
                m_pDriver->Log(output);
        }

        DebuggerBreakpoint* m_BP;
};

/**
  * Command to get info about local frame variables.
  */
class CdbCmd_InfoLocals : public DebuggerCmd
{
        DebuggerTree* m_pDTree;
    public:
        /** @param tree The tree to display the locals. */
        CdbCmd_InfoLocals(DebuggerDriver* driver, DebuggerTree* dtree)
            : DebuggerCmd(driver),
            m_pDTree(dtree)
        {
            m_Cmd << _T("dv");
        }
        void ParseOutput(const wxString& output)
        {
            if (output.StartsWith(_T("Unable to enumerate locals")))
                return;
            wxString locals;
            locals << _T("Local variables\n");
            wxArrayString lines = GetArrayFromString(output, _T('\n'));
            for (unsigned int i = 0; i < lines.GetCount(); ++i)
                locals << _T(' ') << lines[i].Strip(wxString::both) << _T('\n');
            m_pDTree->BuildTree(0, locals, wsfCDB);
        }
};

/**
  * Command to get info about a watched variable.
  */
class CdbCmd_Watch : public DebuggerCmd
{
        DebuggerTree* m_pDTree;
        Watch* m_pWatch;
    public:
        /** @param tree The tree to display the watch. */
        CdbCmd_Watch(DebuggerDriver* driver, DebuggerTree* dtree, Watch* watch)
            : DebuggerCmd(driver),
            m_pDTree(dtree),
            m_pWatch(watch)
        {
            if (m_pWatch->format != Undefined)
                m_pDriver->DebugLog(_T("Watch formats are not supported by this driver"));
            m_Cmd << _T("?? ") << m_pWatch->keyword;
            m_pWatch->hasActiveCommand = true;
        }
        void ParseOutput(const wxString& output)
        {
            if(m_pWatch->pendingDelete)
            {
                m_pDTree->DeleteWatch(m_pWatch);
                return;
            }
//            struct HWND__ * 0x7ffd8000
//
//            struct tagWNDCLASSEXA
//               +0x000 cbSize           : 0x7c8021b5
//               +0x004 style            : 0x7c802011
//               +0x008 lpfnWndProc      : 0x7c80b529     kernel32!GetModuleHandleA+0
//               +0x00c cbClsExtra       : 0
//               +0x010 cbWndExtra       : 2147319808
//               +0x014 hInstance        : 0x00400000
//               +0x018 hIcon            : 0x0012fe88
//               +0x01c hCursor          : 0x0040a104
//               +0x020 hbrBackground    : 0x689fa962
//               +0x024 lpszMenuName     : 0x004028ae  "???"
//               +0x028 lpszClassName    : 0x0040aa30  "CodeBlocksWindowsApp"
//               +0x02c hIconSm          : (null)
//
//            char * 0x0040aa30
//             "CodeBlocksWindowsApp"

            // just remove struct offsets
            wxString lines = output;
            if (reWatch.Matches(lines))
                reWatch.ReplaceAll(&lines, wxEmptyString);

            // replace : with =
            while (lines.Replace(_T(" : "), _T(" = ")))
                ;

            m_pDTree->BuildTree(m_pWatch, m_pWatch->keyword + _T(" = ") + lines, wsfCDB);
        }
};

/**
  * Command to display a tooltip about a variables value.
  */
class CdbCmd_TooltipEvaluation : public DebuggerCmd
{
        wxTipWindow* m_pWin;
        wxRect m_WinRect;
        wxString m_What;
    public:
        /** @param what The variable to evaluate.
            @param win A pointer to the tip window pointer.
            @param tiprect The tip window's rect.
        */
        CdbCmd_TooltipEvaluation(DebuggerDriver* driver, const wxString& what, const wxRect& tiprect)
            : DebuggerCmd(driver),
            m_pWin(0),
            m_WinRect(tiprect),
            m_What(what)
        {
            m_Cmd << _T("?? ") << what;
        }
        void ParseOutput(const wxString& output)
        {
//            struct HWND__ * 0x7ffd8000
//
//            struct tagWNDCLASSEXA
//               +0x000 cbSize           : 0x7c8021b5
//               +0x004 style            : 0x7c802011
//               +0x008 lpfnWndProc      : 0x7c80b529     kernel32!GetModuleHandleA+0
//               +0x00c cbClsExtra       : 0
//               +0x010 cbWndExtra       : 2147319808
//               +0x014 hInstance        : 0x00400000
//               +0x018 hIcon            : 0x0012fe88
//               +0x01c hCursor          : 0x0040a104
//               +0x020 hbrBackground    : 0x689fa962
//               +0x024 lpszMenuName     : 0x004028ae  "???"
//               +0x028 lpszClassName    : 0x0040aa30  "CodeBlocksWindowsApp"
//               +0x02c hIconSm          : (null)
//
//            char * 0x0040aa30
//             "CodeBlocksWindowsApp"
            wxString tip = m_What + _T("=") + output;

            if (m_pWin)
                (m_pWin)->Destroy();
            m_pWin = new wxTipWindow((wxWindow*)Manager::Get()->GetAppWindow(), tip, 640, &m_pWin, &m_WinRect);
//            m_pDriver->DebugLog(output);
        }
};

/**
  * Command to run a backtrace.
  */
class CdbCmd_Backtrace : public DebuggerCmd
{
        BacktraceDlg* m_pDlg;
    public:
        /** @param dlg The backtrace dialog. */
        CdbCmd_Backtrace(DebuggerDriver* driver, BacktraceDlg* dlg)
            : DebuggerCmd(driver),
            m_pDlg(dlg)
        {
            m_Cmd << _T("k n");
        }
        void ParseOutput(const wxString& output)
        {
            // output is:
            //  # ChildEBP RetAddr
            // 00 0012fe98 00401426 Win32GUI!WinMain+0x89 [c:\devel\tmp\win32 test\main.cpp @ 55]
            // 00 0012fe98 00401426 Win32GUI!WinMain+0x89
            //
            // so we have a two-steps process:
            // 1) Get match for the second version (without file/line info)
            // 2) See if we have file/line info and read it
            m_pDlg->Clear();
            wxArrayString lines = GetArrayFromString(output, _T('\n'));
            if (!lines.GetCount() || !lines[0].Contains(_T("ChildEBP")))
                return;
            // start from line 1
            for (unsigned int i = 1; i < lines.GetCount(); ++i)
            {
                if (reBT1.Matches(lines[i]))
                {
                    StackFrame sf;
                    sf.valid = true;
                    reBT1.GetMatch(lines[i], 1).ToULong(&sf.number);
                    reBT1.GetMatch(lines[i], 2).ToULong(&sf.address, 16); // match 2 or 3 ???
                    sf.function = reBT1.GetMatch(lines[i], 4);
                    // do we have file/line info?
                    if (reBT2.Matches(lines[i]))
                    {
                        sf.file = reBT2.GetMatch(lines[i], 1) + reBT2.GetMatch(lines[i], 2);
                        sf.line = reBT2.GetMatch(lines[i], 3);
                    }
                    m_pDlg->AddFrame(sf);
                }
            }
//            m_pDriver->DebugLog(output);
        }
};

/**
  * Command to run a disassembly. Use this instead of CdbCmd_DisassemblyInit, which is chained-called.
  */
class CdbCmd_InfoRegisters : public DebuggerCmd
{
        CPURegistersDlg* m_pDlg;
    public:
        /** @param dlg The disassembly dialog. */
        CdbCmd_InfoRegisters(DebuggerDriver* driver, CPURegistersDlg* dlg)
            : DebuggerCmd(driver),
            m_pDlg(dlg)
        {
            m_Cmd << _T("r");
        }
        void ParseOutput(const wxString& output)
        {
            // output is:
            //
            // eax=00400000 ebx=7ffd9000 ecx=00000065 edx=7c97e4c0 esi=00000000 edi=7c80b529
            // eip=0040102c esp=0012fe48 ebp=0012fe98 iopl=0         nv up ei pl nz na po nc
            // cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00000206

            if (!m_pDlg)
                return;

            wxString tmp = output;
            while (tmp.Replace(_T("\n"), _T(" ")))
                ;
            wxArrayString lines = GetArrayFromString(tmp, _T(' '));
            for (unsigned int i = 0; i < lines.GetCount(); ++i)
            {
                wxString reg = lines[i].BeforeFirst(_T('='));
                wxString addr = lines[i].AfterFirst(_T('='));
                if (!reg.IsEmpty() && !addr.IsEmpty())
                {
                    long int addrL;
                    addr.ToLong(&addrL, 16);
                    m_pDlg->SetRegisterValue(reg, addrL);
                }
            }
//            m_pDlg->Show(true);
//            m_pDriver->DebugLog(output);
        }
};

/**
  * Command to run a disassembly.
  */
class CdbCmd_Disassembly : public DebuggerCmd
{
        DisassemblyDlg* m_pDlg;
    public:
        /** @param dlg The disassembly dialog. */
        CdbCmd_Disassembly(DebuggerDriver* driver, DisassemblyDlg* dlg, const wxString& StopAddress)
            : DebuggerCmd(driver),
            m_pDlg(dlg)
        {
            m_Cmd << _T("uf ") << StopAddress;
        }
        void ParseOutput(const wxString& output)
        {
            // output is a series of:
            //
            // Win32GUI!WinMain [c:\devel\tmp\win32 test\main.cpp @ 15]:
            //    15 00401020 55               push    ebp
            // ...

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
  * Command to run a disassembly. Use this instead of CdbCmd_Disassembly, which is chain-called.
  */
class CdbCmd_DisassemblyInit : public DebuggerCmd
{
        static wxString LastAddr;
        DisassemblyDlg* m_pDlg;
    public:
        /** @param dlg The disassembly dialog. */
        CdbCmd_DisassemblyInit(DebuggerDriver* driver, DisassemblyDlg* dlg)
            : DebuggerCmd(driver),
            m_pDlg(dlg)
        {
            // print stack frame and nearest symbol (start of function)
            m_Cmd << _T("k n 1; ln");
        }
        void ParseOutput(const wxString& output)
        {
//            m_pDriver->QueueCommand(new CdbCmd_Disassembly(m_pDriver, m_pDlg, StopAddress)); // chain call

            long int offset = 0;
            wxArrayString lines = GetArrayFromString(output, _T('\n'));
            for (unsigned int i = 0; i < lines.GetCount(); ++i)
            {
                if (lines[i].Contains(_T("ChildEBP")))
                {
                    if (reDisassemblyFile.Matches(lines[i + 1]))
                    {
                        ++i; // we 're interested in the next line

                        StackFrame sf;
                        wxString addr = reDisassemblyFile.GetMatch(lines[i], 1);
                        sf.function = reDisassemblyFile.GetMatch(lines[i], 2);
                        wxString offsetStr = sf.function.AfterLast(_T('+'));
                        if (!offsetStr.IsEmpty())
                            offsetStr.ToLong(&offset, 16);
                        if (addr != LastAddr)
                        {
                            LastAddr = addr;
                            addr.ToLong((long int*)&sf.address, 16);

                            sf.valid = true;
                            m_pDlg->Clear(sf);
                            m_pDriver->QueueCommand(new CdbCmd_Disassembly(m_pDriver, m_pDlg, sf.function)); // chain call
//                            break;
                        }
                    }
                }
                else
                {
                    m_pDriver->Log(_T("Checking for current function start"));
                    if (reDisassemblyFunc.Matches(lines[i]))
                    {
                        long int start;
                        reDisassemblyFunc.GetMatch(lines[i], 1).ToLong(&start, 16);
                        m_pDlg->SetActiveAddress(start + offset);
                    }
                }
            }
        }
};
wxString CdbCmd_DisassemblyInit::LastAddr;

#endif // DEBUGGER_COMMANDS_H
