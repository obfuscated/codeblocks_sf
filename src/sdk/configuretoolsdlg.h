#ifndef CONFIGURETOOLSDLG_H
#define CONFIGURETOOLSDLG_H

#include <wx/dialog.h>

#include "settings.h"

// forward decls
struct Tool;

class ConfigureToolsDlg : public wxDialog
{
	public:
		ConfigureToolsDlg(wxWindow* parent);
		~ConfigureToolsDlg();
	protected:
	private:
		void DoFillList();
		bool DoEditTool(Tool* tool);
		void OnAdd(wxCommandEvent& event);
		void OnEdit(wxCommandEvent& event);
		void OnRemove(wxCommandEvent& event);
		void OnUp(wxCommandEvent& event);
		void OnDown(wxCommandEvent& event);
		void OnUpdateUI(wxUpdateUIEvent& event);
		
		DECLARE_EVENT_TABLE()
};

#endif // CONFIGURETOOLSDLG_H

