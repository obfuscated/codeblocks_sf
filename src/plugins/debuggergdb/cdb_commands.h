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
#include <cbdebugger_interfaces.h>
#include "debugger_defs.h"
#include "debuggergdb.h"
#include "debuggermanager.h"
#include "parsewatchvalue.h"

static wxRegEx reProcessInf(_T("id:[ \t]+([A-Fa-f0-9]+)[ \t]+create"));
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

// 01 0012ff68 00404168 cdb_test!main+0xae [c:\dev\projects\tests\cdb_test\main.cpp @ 21]
static wxRegEx reSwitchFrame(wxT("[ \\t]*([0-9]+)[ \\t]([0-9a-z]+)[ \\t](.+)[ \\t]\\[(.+)[ \\t]@[ \\t]([0-9]+)\\][ \\t]*"));

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
        void ParseOutput(cb_unused const wxString& output)
        {
            // No output
        }
};

/**
 * Command to find the PID of the active child
 */
class CdbCmd_GetPID : public DebuggerCmd
{
    public:
        /** @param file The file to debug. */
        CdbCmd_GetPID(DebuggerDriver* driver)
            : DebuggerCmd(driver)
        {
            m_Cmd << _T("|.");
        }
        void ParseOutput(const wxString& output)
        {
            // Output:
            // <decimal process num> id: <hex PID> create name: <process name>
            wxArrayString lines = GetArrayFromString(output, _T('\n'));
            for (unsigned int i = 0; i < lines.GetCount(); ++i)
            {
                if (reProcessInf.Matches(lines[i]))
                {
                    wxString hexID = reProcessInf.GetMatch(lines[i],1);

                    long pid;
                    if (hexID.ToLong(&pid,16))
                    {
                        m_pDriver->SetChildPID(pid);
                    }
                }
            }
        }
};

/**
  * Command to the attach to a process.
  */
class CdbCmd_AttachToProcess : public DebuggerCmd
{
    private:
        int m_pid;
    public:
        /** @param file The file to debug. */
        CdbCmd_AttachToProcess(DebuggerDriver* driver, int pid)
            : DebuggerCmd(driver),
            m_pid(pid)
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
                {
                    m_pDriver->Log(lines[i]);
                    m_pDriver->SetChildPID(m_pid);
                }
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
  * Command to continue execution and notify the debugger plugin.
  */
class CdbCmd_Continue : public DebuggerContinueBaseCmd
{
    public:
        /** @param bp The breakpoint to set. */
        CdbCmd_Continue(DebuggerDriver* driver)
            : DebuggerContinueBaseCmd(driver,_T("g"))
        {
        }
        virtual void Action()
        {
            m_pDriver->NotifyDebuggeeContinued();
        }
};

/**
  * Command to add a breakpoint.
  */
class CdbCmd_AddBreakpoint : public DebuggerCmd
{
        static int m_lastIndex;
    public:
        /** @param bp The breakpoint to set. */
        CdbCmd_AddBreakpoint(DebuggerDriver* driver, cb::shared_ptr<DebuggerBreakpoint> bp)
            : DebuggerCmd(driver),
            m_BP(bp)
        {
            if (bp->enabled)
            {
                if (bp->index==-1)
                    bp->index = m_lastIndex++;

                wxString out = m_BP->filename;
//                DebuggerGDB::ConvertToGDBFile(out);
                QuoteStringIfNeeded(out);
                // we add one to line,  because scintilla uses 0-based line numbers, while cdb uses 1-based
                m_Cmd << _T("bu") << wxString::Format(_T("%ld"), (int) bp->index) << _T(' ');
                if (m_BP->temporary)
                    m_Cmd << _T("/1 ");
                if (bp->func.IsEmpty())
                    m_Cmd << _T('`') << out << _T(":") << wxString::Format(_T("%d"), bp->line) << _T('`');
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

        cb::shared_ptr<DebuggerBreakpoint> m_BP;
};

int CdbCmd_AddBreakpoint::m_lastIndex = 1;

/**
  * Command to remove a breakpoint.
  */
class CdbCmd_RemoveBreakpoint : public DebuggerCmd
{
    public:
        /** @param bp The breakpoint to remove. If NULL, all breakpoints are removed. */
        CdbCmd_RemoveBreakpoint(DebuggerDriver* driver, cb::shared_ptr<DebuggerBreakpoint> bp)
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

        cb::shared_ptr<DebuggerBreakpoint> m_BP;
};

/**
  * Command to get info about a watched variable.
  */
class CdbCmd_Watch : public DebuggerCmd
{
        cb::shared_ptr<GDBWatch> m_watch;
    public:
        CdbCmd_Watch(DebuggerDriver* driver, cb::shared_ptr<GDBWatch> const &watch)
            : DebuggerCmd(driver),
            m_watch(watch)
        {
            wxString symbol;
            m_watch->GetSymbol(symbol);
            m_Cmd << wxT("?? ") << symbol;
        }

        void ParseOutput(const wxString& output)
        {
            if(!ParseCDBWatchValue(m_watch, output))
            {
                wxString symbol;
                m_watch->GetSymbol(symbol);
                wxString const &msg = wxT("Parsing CDB output failed for '") + symbol + wxT("'!");
                m_watch->SetValue(msg);
                Manager::Get()->GetLogManager()->LogError(msg);
            }
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
    public:
        /** @param dlg The backtrace dialog. */
        CdbCmd_Backtrace(DebuggerDriver* driver, bool switchToFirst)
            : DebuggerCmd(driver),
            m_SwitchToFirst(switchToFirst)
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
            m_pDriver->GetStackFrames().clear();

            wxArrayString lines = GetArrayFromString(output, _T('\n'));
            if (!lines.GetCount() || !lines[0].Contains(_T("ChildEBP")))
                return;

            bool firstValid = true;
            cbStackFrame frameToSwitch;

            // start from line 1
            for (unsigned int i = 1; i < lines.GetCount(); ++i)
            {
                if (reBT1.Matches(lines[i]))
                {
                    cbStackFrame sf;
                    sf.MakeValid(true);

                    long int number;
                    reBT1.GetMatch(lines[i], 1).ToLong(&number);

                    sf.SetNumber(number);
                    sf.SetAddress(cbDebuggerStringToAddress(reBT1.GetMatch(lines[i], 2)));
                    sf.SetSymbol(reBT1.GetMatch(lines[i], 4));
                    // do we have file/line info?
                    if (reBT2.Matches(lines[i]))
                    {
                        sf.SetFile(reBT2.GetMatch(lines[i], 1) + reBT2.GetMatch(lines[i], 2),
                                   reBT2.GetMatch(lines[i], 3));
                    }
                    m_pDriver->GetStackFrames().push_back(cb::shared_ptr<cbStackFrame>(new cbStackFrame(sf)));

                    if (m_SwitchToFirst && sf.IsValid() && firstValid)
                    {
                        firstValid = false;
                        frameToSwitch = sf;
                    }
                }
            }
            Manager::Get()->GetDebuggerManager()->GetBacktraceDialog()->Reload();
            if (!firstValid)
            {
                Cursor cursor;
                cursor.file = frameToSwitch.GetFilename();
                frameToSwitch.GetLine().ToLong(&cursor.line);
                cursor.address = frameToSwitch.GetAddressAsString();
                cursor.changed = true;
                m_pDriver->SetCursor(cursor);
                m_pDriver->NotifyCursorChanged();
            }
        }
    private:
        bool m_SwitchToFirst;
};

class CdbCmd_SwitchFrame : public DebuggerCmd
{
    public:
        CdbCmd_SwitchFrame(DebuggerDriver *driver, int frameNumber) :
            DebuggerCmd(driver)
        {
            if (frameNumber < 0)
                m_Cmd = wxT("k n 1");
            else
                m_Cmd = wxString::Format(wxT(".frame %d"), frameNumber);
        }

        virtual void ParseOutput(const wxString& output)
        {
            wxArrayString lines = GetArrayFromString(output, wxT('\n'));

            for (unsigned ii = 0; ii < lines.GetCount(); ++ii)
            {
                if (lines[ii].Contains(wxT("ChildEBP")))
                    continue;
                else if (reSwitchFrame.Matches(lines[ii]))
                {
                    Cursor cursor;
                    cursor.file = reSwitchFrame.GetMatch(lines[ii], 4);
                    wxString const &line_str = reSwitchFrame.GetMatch(lines[ii], 5);
                    if (!line_str.empty())
                        line_str.ToLong(&cursor.line);
                    else
                        cursor.line = -1;

                    cursor.address = reSwitchFrame.GetMatch(lines[ii], 1);
                    cursor.changed = true;
                    m_pDriver->SetCursor(cursor);
                    m_pDriver->NotifyCursorChanged();
                    Manager::Get()->GetDebuggerManager()->GetBacktraceDialog()->Reload();
                    break;
                }
                else
                    break;
            }
        }
};

/**
  * Command to run a disassembly. Use this instead of CdbCmd_DisassemblyInit, which is chained-called.
  */
class CdbCmd_InfoRegisters : public DebuggerCmd
{
    public:
        /** @param dlg The disassembly dialog. */
        CdbCmd_InfoRegisters(DebuggerDriver* driver)
            : DebuggerCmd(driver)
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

            cbCPURegistersDlg *dialog = Manager::Get()->GetDebuggerManager()->GetCPURegistersDialog();

            wxString tmp = output;
            while (tmp.Replace(_T("\n"), _T(" ")))
                ;
            wxArrayString lines = GetArrayFromString(tmp, _T(' '));
            for (unsigned int i = 0; i < lines.GetCount(); ++i)
            {
                wxString reg = lines[i].BeforeFirst(_T('='));
                wxString addr = lines[i].AfterFirst(_T('='));
                if (!reg.IsEmpty() && !addr.IsEmpty())
                    dialog->SetRegisterValue(reg, addr, wxEmptyString);
            }
        }
};

/**
  * Command to run a disassembly.
  */
class CdbCmd_Disassembly : public DebuggerCmd
{
    public:
        CdbCmd_Disassembly(DebuggerDriver* driver, const wxString& StopAddress)
            : DebuggerCmd(driver)
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

            cbDisassemblyDlg *dialog = Manager::Get()->GetDebuggerManager()->GetDisassemblyDialog();

            wxArrayString lines = GetArrayFromString(output, _T('\n'));
            for (unsigned int i = 0; i < lines.GetCount(); ++i)
            {
                if (reDisassembly.Matches(lines[i]))
                {
                    long int addr;
                    reDisassembly.GetMatch(lines[i], 1).ToLong(&addr, 16);
                    dialog->AddAssemblerLine(addr, reDisassembly.GetMatch(lines[i], 2));
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
    public:
        CdbCmd_DisassemblyInit(DebuggerDriver* driver)
            : DebuggerCmd(driver)
        {
            // print stack frame and nearest symbol (start of function)
            m_Cmd << _T("k n 1; ln");
        }
        void ParseOutput(const wxString& output)
        {
//            m_pDriver->QueueCommand(new CdbCmd_Disassembly(m_pDriver, m_pDlg, StopAddress)); // chain call

            cbDisassemblyDlg *dialog = Manager::Get()->GetDebuggerManager()->GetDisassemblyDialog();

            long int offset = 0;
            wxArrayString lines = GetArrayFromString(output, _T('\n'));
            for (unsigned int i = 0; i < lines.GetCount(); ++i)
            {
                if (lines[i].Contains(_T("ChildEBP")))
                {
                    if (reDisassemblyFile.Matches(lines[i + 1]))
                    {
                        ++i; // we 're interested in the next line
                        cbStackFrame sf;
                        wxString addr = reDisassemblyFile.GetMatch(lines[i], 1);
                        sf.SetSymbol(reDisassemblyFile.GetMatch(lines[i], 2));
                        wxString offsetStr = sf.GetSymbol().AfterLast(_T('+'));
                        if (!offsetStr.IsEmpty())
                            offsetStr.ToLong(&offset, 16);
                        if (addr != LastAddr)
                        {
                            LastAddr = addr;
                            sf.SetAddress(cbDebuggerStringToAddress(addr));
                            sf.MakeValid(true);
                            dialog->Clear(sf);
                            m_pDriver->QueueCommand(new CdbCmd_Disassembly(m_pDriver, sf.GetSymbol())); // chain call
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
                        dialog->SetActiveAddress(start + offset);
                    }
                }
            }
        }
};
wxString CdbCmd_DisassemblyInit::LastAddr;

#endif // DEBUGGER_COMMANDS_H
