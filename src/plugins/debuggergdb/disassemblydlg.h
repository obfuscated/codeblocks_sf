#ifndef DISASSEMBLYDLG_H
#define DISASSEMBLYDLG_H

#include <wx/dialog.h>

class DebuggerGDB;
struct StackFrame;

class DisassemblyDlg : public wxDialog
{
	public:
		DisassemblyDlg(wxWindow* parent, DebuggerGDB* debugger);
		virtual ~DisassemblyDlg();

        void Clear(const StackFrame& frame);
		void AddAssemblerLine(const wxString& line);
	protected:
        void OnSave(wxCommandEvent& event);
        void OnRefresh(wxCommandEvent& event);

        DebuggerGDB* m_pDbg;
	private:
        DECLARE_EVENT_TABLE();
};

#endif // DISASSEMBLYDLG_H
