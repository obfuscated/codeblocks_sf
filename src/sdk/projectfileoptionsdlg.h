#ifndef PROJECTFILEOPTIONSDLG_H
#define PROJECTFILEOPTIONSDLG_H

#include <wx/dialog.h>
#include "settings.h"

class ProjectFile;

class ProjectFileOptionsDlg : public wxDialog
{
	public:
		ProjectFileOptionsDlg(wxWindow* parent, ProjectFile* pf);
		~ProjectFileOptionsDlg();
		void OnUpdateUI(wxUpdateUIEvent& event);
		void EndModal(int retCode);
	private:
		ProjectFile* m_ProjectFile;

		DECLARE_EVENT_TABLE()
};

#endif // PROJECTFILEOPTIONSDLG_H
