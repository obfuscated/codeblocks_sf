#ifndef BREAKPOINTS_H
#define BREAKPOINTS_H

#include <wx/dialog.h>
#include "debugger_defs.h"

class BreakpointsDlg : public wxDialog
{
    public:
        BreakpointsDlg(BreakpointsList& list, wxWindow* parent = 0);
        virtual ~BreakpointsDlg();
    protected:
        void FillBreakpoints();
        void FillRecord(int sel);
        void EndModal(int retCode);
        void RemoveBreakpoint(int sel);

        void OnRemove(wxCommandEvent& event);
        void OnRemoveAll(wxCommandEvent& event);
        void OnListboxClick(wxCommandEvent& event);
        void OnUpdateUI(wxUpdateUIEvent& event);

        int m_LastSel;
        BreakpointsList& m_List;
    private:
        DECLARE_EVENT_TABLE()
};

#endif // BREAKPOINTS_H
