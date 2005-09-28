#ifndef DISASSEMBLYDLG_H
#define DISASSEMBLYDLG_H

#include <wx/dialog.h>

class DebuggerGDB;
struct StackFrame;

class DisassemblyDlg : public wxDialog
{
	public:
        static wxString Registers[];

		DisassemblyDlg(wxWindow* parent, DebuggerGDB* debugger);
		virtual ~DisassemblyDlg();

        void Clear(const StackFrame& frame);
        void ClearRegisters();
		void AddAssemblerLine(unsigned long int addr, const wxString& line);
		void SetActiveAddress(unsigned long int addr);
		void AddRegisterValue(int idx, long int value);
		void SetRegisterValue(int idx, long int value);
	protected:
        void OnSave(wxCommandEvent& event);
        void OnRefresh(wxCommandEvent& event);

        DebuggerGDB* m_pDbg;
        unsigned long int m_LastActiveAddr;
	private:
        DECLARE_EVENT_TABLE();
};

#endif // DISASSEMBLYDLG_H
