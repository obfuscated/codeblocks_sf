#ifndef EDITTOOLDLG_H
#define EDITTOOLDLG_H

#include <wx/dialog.h>
#include "settings.h"
#include "toolsmanager.h"

class EditToolDlg : public wxDialog
{
	public:
		EditToolDlg(wxWindow* parent, Tool* tool);
		~EditToolDlg();
	protected:
	private:
		void OnUpdateUI(wxUpdateUIEvent& event);
		void OnBrowseCommand(wxCommandEvent& event);
		void OnBrowseDir(wxCommandEvent& event);
		void OnOKClick(wxCommandEvent& event);
		
		Tool* m_Tool;
		
		DECLARE_EVENT_TABLE()
};

#endif // EDITTOOLDLG_H

