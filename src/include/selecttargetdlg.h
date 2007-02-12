#ifndef SELECTTARGETDLG_H
#define SELECTTARGETDLG_H

#include <wx/dialog.h>

class cbProject;
class ProjectBuildTarget;

class SelectTargetDlg : public wxDialog
{
	public:
		SelectTargetDlg(wxWindow* parent, cbProject* project, int selected = 0);
		~SelectTargetDlg();

		void EndModal(int retCode);
		int GetSelection() const { return m_Selected; }
		ProjectBuildTarget* GetSelectionTarget();
	private:
		void OnListboxSelection(wxCommandEvent& event);
        void OnCheckboxSelection(wxCommandEvent& event);
        void OnHostApplicationButtonClick(wxCommandEvent& event);
		void UpdateSelected();
		cbProject* m_pProject;
		int m_Selected;

		DECLARE_EVENT_TABLE()
};

#endif // SELECTTARGETDLG_H

