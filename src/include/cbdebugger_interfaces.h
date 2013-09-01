/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef _CB_DEBUGGER_INTERFACES_H_
#define _CB_DEBUGGER_INTERFACES_H_

#include <wx/string.h>
#include "settings.h"
#include "debuggermanager.h"

class cbDebuggerPlugin;
class wxMenu;
class wxObject;
class wxWindow;

class DLLIMPORT cbBacktraceDlg
{
    public:
        virtual ~cbBacktraceDlg();

        virtual wxWindow* GetWindow() = 0;

        virtual void Reload() = 0;
        virtual void EnableWindow(bool enable) = 0;
};

class DLLIMPORT cbBreakpointsDlg
{
    public:
        virtual ~cbBreakpointsDlg();

        virtual wxWindow* GetWindow() = 0;

        virtual bool AddBreakpoint(cbDebuggerPlugin *plugin, const wxString& filename, int line) = 0;
        virtual bool RemoveBreakpoint(cbDebuggerPlugin *plugin, const wxString& filename, int line) = 0;
        virtual void RemoveAllBreakpoints() = 0;
        virtual void EditBreakpoint(const wxString& filename, int line) = 0;
        virtual void EnableBreakpoint(const wxString& filename, int line, bool enable) = 0;

        virtual void Reload() = 0;
};

class DLLIMPORT cbCPURegistersDlg
{
    public:
        virtual ~cbCPURegistersDlg();

        virtual wxWindow* GetWindow() = 0;

        virtual void Clear() = 0;
        virtual void SetRegisterValue(const wxString& reg_name, size_t value) = 0;
        virtual void EnableWindow(bool enable) = 0;
};

class DLLIMPORT cbDisassemblyDlg
{
    public:
        virtual ~cbDisassemblyDlg();

        virtual wxWindow* GetWindow() = 0;

        virtual void Clear(const cbStackFrame& frame) = 0;
        virtual void AddAssemblerLine(size_t addr, const wxString& line) = 0;
        virtual void AddSourceLine(size_t lineno, const wxString& line) = 0;
        virtual bool SetActiveAddress(size_t addr) = 0;
        virtual void CenterLine(size_t lineno) = 0;
        virtual void CenterCurrentLine() = 0;
        virtual bool HasActiveAddr() = 0;
        virtual void EnableWindow(bool enable) = 0;
};

class DLLIMPORT cbExamineMemoryDlg
{
    public:
        virtual ~cbExamineMemoryDlg();

        virtual wxWindow* GetWindow() = 0;

        // used for Freeze()/Thaw() calls
        virtual void Begin() = 0;
        virtual void End() = 0;

        virtual void Clear() = 0;
        virtual wxString GetBaseAddress() = 0;
        virtual int GetBytes() = 0;
        virtual void AddError(const wxString& err) = 0;
        virtual void AddHexByte(const wxString& addr, const wxString& hexbyte) = 0;
        virtual void EnableWindow(bool enable) = 0;
};

class DLLIMPORT cbThreadsDlg
{
    public:
        virtual ~cbThreadsDlg();

        virtual wxWindow* GetWindow() = 0;

        virtual void Reload() = 0;
        virtual void EnableWindow(bool enable) = 0;
};

class DLLIMPORT cbWatchesDlg
{
    public:
        virtual ~cbWatchesDlg();

        virtual wxWindow* GetWindow() = 0;

        virtual void UpdateWatches() = 0;
        virtual void AddWatch(cb::shared_ptr<cbWatch> watch) = 0;
        virtual void AddSpecialWatch(cb::shared_ptr<cbWatch> watch, bool readonly) = 0;
        virtual void RemoveWatch(cb::shared_ptr<cbWatch> watch) = 0;
        virtual void RenameWatch(wxObject *prop, const wxString &newSymbol) = 0;
        virtual void RefreshUI() = 0;
};

class DLLIMPORT cbDebuggerWindowMenuItem
{
    public:
        virtual ~cbDebuggerWindowMenuItem() {}

        virtual void OnClick(bool enable) = 0;
        virtual bool IsEnabled() = 0;
        virtual bool IsChecked() = 0;
};

class DLLIMPORT cbDebuggerMenuHandler
{
    public:
        virtual ~cbDebuggerMenuHandler();

        virtual void SetActiveDebugger(cbDebuggerPlugin *active) = 0;
        virtual void MarkActiveTargetAsValid(bool valid) = 0;
        virtual void RebuildMenus() = 0;
        virtual void BuildContextMenu(wxMenu &menu, const wxString& word_at_caret, bool is_running) = 0;

        virtual bool RegisterWindowMenu(const wxString &name, const wxString &help, cbDebuggerWindowMenuItem *item) = 0;
        virtual void UnregisterWindowMenu(const wxString &name) = 0;
};

class DLLIMPORT cbDebugInterfaceFactory
{
        // make class non copyable
        cbDebugInterfaceFactory(cbDebugInterfaceFactory &);
        cbDebugInterfaceFactory& operator=(cbDebugInterfaceFactory &);
    public:
        cbDebugInterfaceFactory();
        virtual ~cbDebugInterfaceFactory();

        virtual cbBacktraceDlg* CreateBacktrace() = 0;
        virtual void DeleteBacktrace(cbBacktraceDlg *dialog) = 0;

        virtual cbBreakpointsDlg* CreateBreapoints() = 0;
        virtual void DeleteBreakpoints(cbBreakpointsDlg *dialog) = 0;

        virtual cbCPURegistersDlg* CreateCPURegisters() = 0;
        virtual void DeleteCPURegisters(cbCPURegistersDlg *dialog) = 0;

        virtual cbDisassemblyDlg* CreateDisassembly() = 0;
        virtual void DeleteDisassembly(cbDisassemblyDlg *dialog) = 0;

        virtual cbExamineMemoryDlg* CreateMemory() = 0;
        virtual void DeleteMemory(cbExamineMemoryDlg *dialog) = 0;

        virtual cbThreadsDlg* CreateThreads() = 0;
        virtual void DeleteThreads(cbThreadsDlg *dialog) = 0;

        virtual cbWatchesDlg* CreateWatches() = 0;
        virtual void DeleteWatches(cbWatchesDlg *dialog) = 0;

        /** @brief Show new value tooltip
          * @return Return True only if new tooltip was shown, else return False.
          */
        virtual bool ShowValueTooltip(const cb::shared_ptr<cbWatch> &watch, const wxRect &rect) = 0;
        virtual void HideValueTooltip() = 0;
        virtual bool IsValueTooltipShown() = 0;
        virtual void UpdateValueTooltip() = 0;
};

#endif // _CB_DEBUGGER_INTERFACES_H_

