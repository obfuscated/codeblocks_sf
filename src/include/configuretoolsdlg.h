/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

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

