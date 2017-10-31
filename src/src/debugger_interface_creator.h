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
        virtual cbBacktraceDlg* CreateBacktrace();
        virtual void DeleteBacktrace(cbBacktraceDlg *dialog);

        virtual cbBreakpointsDlg* CreateBreapoints();
        virtual void DeleteBreakpoints(cbBreakpointsDlg *dialog);

        virtual cbCPURegistersDlg* CreateCPURegisters();
        virtual void DeleteCPURegisters(cbCPURegistersDlg *dialog);

        virtual cbDisassemblyDlg* CreateDisassembly();
        virtual void DeleteDisassembly(cbDisassemblyDlg *dialog);

        virtual cbExamineMemoryDlg* CreateMemory();
        virtual void DeleteMemory(cbExamineMemoryDlg *dialog);

        virtual cbThreadsDlg* CreateThreads();
        virtual void DeleteThreads(cbThreadsDlg *dialog);

        virtual cbWatchesDlg* CreateWatches();
        virtual void DeleteWatches(cbWatchesDlg *dialog);

        virtual bool ShowValueTooltip(const cb::shared_ptr<cbWatch> &watch, const wxRect &rect);
        virtual void HideValueTooltip();
        virtual bool IsValueTooltipShown();
        virtual void UpdateValueTooltip();
    private:
        void OnEditorDeactivate(CodeBlocksEvent &event);
    private:
        ValueTooltip *m_tooltip;
};

#endif // _DEBUGGER_INTERFACE_CREATOR_H_

