/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef PROJECTFILEOPTIONSDLG_H
#define PROJECTFILEOPTIONSDLG_H

#include <wx/dialog.h>
#include "settings.h"

class ProjectFile;

class ProjectFileOptionsDlg : public wxDialog
{
	public:
		ProjectFileOptionsDlg(wxWindow* parent, ProjectFile* pf);
		ProjectFileOptionsDlg(wxWindow* parent, const wxString& fileName);
		~ProjectFileOptionsDlg();
		void OnCompilerCombo(wxCommandEvent& event);
		void OnUpdateUI(wxUpdateUIEvent& event);
		void EndModal(int retCode);
	private:
		void FillGeneralProperties(const wxString& fileName);
        void FillCompilers();
        void UpdateBuildCommand();
        void SaveBuildCommandSelection();

		ProjectFile* m_ProjectFile;
		int m_LastBuildStageCompilerSel;

		DECLARE_EVENT_TABLE()
};

#endif // PROJECTFILEOPTIONSDLG_H
