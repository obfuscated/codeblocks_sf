#ifndef EDITTOOLDLG_H
#define EDITTOOLDLG_H

#include <wx/dialog.h>

class cbTool;
class wxCommandEvent;
class wxUpdateUIEvent;

class EditToolDlg : public wxDialog
{
	public:
		EditToolDlg(wxWindow* parent, cbTool* tool);
		~EditToolDlg();
		void EndModal(int retCode);
	private:
		void OnUpdateUI(wxUpdateUIEvent& event);
		void OnBrowseCommand(wxCommandEvent& event);
		void OnBrowseDir(wxCommandEvent& event);

		cbTool* m_Tool;   //!< the tool we are editing (setting up)

		DECLARE_EVENT_TABLE()
};

#endif // EDITTOOLDLG_H

