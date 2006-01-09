#ifndef CPUREGISTERSDLG_H
#define CPUREGISTERSDLG_H

#include <wx/panel.h>

class DebuggerGDB;
class wxListCtrl;

class CPURegistersDlg : public wxPanel
{
	public:
        static wxString Registers[];
        static int RegisterIndexFromName(const wxString& name);

		CPURegistersDlg(wxWindow* parent, DebuggerGDB* debugger);
		virtual ~CPURegistersDlg();

        void Clear();
		void AddRegisterValue(int idx, long int value);
		void SetRegisterValue(int idx, long int value);
	protected:
        void OnRefresh(wxCommandEvent& event);

        DebuggerGDB* m_pDbg;
        wxListCtrl* m_pList;
	private:
        DECLARE_EVENT_TABLE();
};

#endif // CPUREGISTERSDLG_H
