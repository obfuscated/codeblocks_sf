/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef _DEBUGGER_INTERFACE_CREATOR_H_
#define _DEBUGGER_INTERFACE_CREATOR_H_

#include <cbdebugger_interfaces.h>

class ValueTooltip;

class DebugInterfaceFactory : public cbDebugInterfaceFactory
{
    public:
        DebugInterfaceFactory();
        ~DebugInterfaceFactory();

    public:
        cbBacktraceDlg* CreateBacktrace() override;
        void DeleteBacktrace(cbBacktraceDlg *dialog) override;

        cbBreakpointsDlg* CreateBreapoints() override;
        void DeleteBreakpoints(cbBreakpointsDlg *dialog) override;

        cbCPURegistersDlg* CreateCPURegisters() override;
        void DeleteCPURegisters(cbCPURegistersDlg *dialog) override;

        cbDisassemblyDlg* CreateDisassembly() override;
        void DeleteDisassembly(cbDisassemblyDlg *dialog) override;

        cbExamineMemoryDlg* CreateMemory() override;
        void DeleteMemory(cbExamineMemoryDlg *dialog) override;

        cbThreadsDlg* CreateThreads() override;
        void DeleteThreads(cbThreadsDlg *dialog) override;

        cbWatchesDlg* CreateWatches() override;
        void DeleteWatches(cbWatchesDlg *dialog) override;

        bool ShowValueTooltip(const cb::shared_ptr<cbWatch> &watch, const wxRect &rect) override;
        void HideValueTooltip() override;
        bool IsValueTooltipShown() override;
        void UpdateValueTooltip() override;
    private:
        void OnEditorDeactivate(CodeBlocksEvent &event);
    private:
        ValueTooltip *m_tooltip;
};

#endif // _DEBUGGER_INTERFACE_CREATOR_H_

