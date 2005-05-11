#ifndef BACKTRACEDLG_H
#define BACKTRACEDLG_H

#include <wx/dialog.h>
#include <wx/listctrl.h>

class DebuggerGDB;
struct StackFrame;

class BacktraceDlg : public wxDialog
{
	public:
		BacktraceDlg(wxWindow* parent, DebuggerGDB* debugger);
		virtual ~BacktraceDlg();

        void Clear();
		void AddFrame(const StackFrame& frame);
	protected:
        void OnDblClick(wxListEvent& event);
        void OnSave(wxCommandEvent& event);
        void OnRefresh(wxCommandEvent& event);

        DebuggerGDB* m_pDbg;
	private:
        DECLARE_EVENT_TABLE();
};

#endif // BACKTRACEDLG_H
