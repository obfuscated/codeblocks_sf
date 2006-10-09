#ifndef BREAKPOINTSDLG_H
#define BREAKPOINTSDLG_H

#ifndef CB_PRECOMP
    #include <wx/listctrl.h>
#endif

#include <simplelistlog.h>
#include "debugger_defs.h"

class DebuggerState;

class BreakpointsDlg : public SimpleListLog
{
    public:
        BreakpointsDlg(DebuggerState& state);
        virtual ~BreakpointsDlg();

        void Refresh();
    protected:
        void FillBreakpoints();
        void RemoveBreakpoint(int sel);

        void OnRemove(wxCommandEvent& event);
        void OnRemoveAll(wxCommandEvent& event);
        void OnProperties(wxCommandEvent& event);
        void OnOpen(wxCommandEvent& event);
        void OnRightClick(wxListEvent& event);
        void OnDoubleClick(wxListEvent& event);

        DebuggerState& m_State;
        BreakpointsList& m_BreakpointsList;
    private:
        DECLARE_EVENT_TABLE()
};

#endif // BREAKPOINTSDLG_H
