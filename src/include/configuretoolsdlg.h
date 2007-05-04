#ifndef CONFIGURETOOLSDLG_H
#define CONFIGURETOOLSDLG_H

#include <wx/dialog.h>

// forward decls
class cbTool;
class wxCommandEvent;
class wxUpdateUIEvent;

class ConfigureToolsDlg : public wxDialog
{
	public:
		ConfigureToolsDlg(wxWindow* parent);
		~ConfigureToolsDlg();
	private:
		void DoFillList();
		bool DoEditTool(cbTool* tool);
		void OnAdd(wxCommandEvent& event);
		void OnEdit(wxCommandEvent& event);
		void OnRemove(wxCommandEvent& event);
		void OnAddSeparator(wxCommandEvent& event);
		void OnUp(wxCommandEvent& event);
		void OnDown(wxCommandEvent& event);
		void OnUpdateUI(wxUpdateUIEvent& event);

		DECLARE_EVENT_TABLE()
};

#endif // CONFIGURETOOLSDLG_H

