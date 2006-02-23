#ifndef SELECTTARGETDLG_H
#define SELECTTARGETDLG_H

#include <wx/dialog.h>
#include "projectbuildtarget.h"
#include "cbproject.h"

class SelectTargetDlg : public wxDialog
{
	public:
		SelectTargetDlg(wxWindow* parent, cbProject* project, int selected = 0);
		~SelectTargetDlg();

		void EndModal(int retCode);
		int GetSelection(){ return m_Selected; }
		ProjectBuildTarget* GetSelectionTarget(){ return m_pProject->GetBuildTarget(m_Selected); }
	protected:
	private:
		void OnListboxSelection(wxCommandEvent& event);
        void OnCheckboxSelection(wxCommandEvent& event);
		void UpdateSelected();
		cbProject* m_pProject;
		int m_Selected;

		DECLARE_EVENT_TABLE()
};

#endif // SELECTTARGETDLG_H

