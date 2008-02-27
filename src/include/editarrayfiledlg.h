/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef EDITARRAYFILEDLG_H
#define EDITARRAYFILEDLG_H

#include <wx/dialog.h>
#include <wx/intl.h>
#include "settings.h"

class DLLIMPORT EditArrayFileDlg : public wxDialog
{
	public:
		EditArrayFileDlg(wxWindow* parent, wxArrayString& array, bool useRelativePaths = false, const wxString& basePath = _T(""));
		virtual ~EditArrayFileDlg();
		EditArrayFileDlg& operator=(const EditArrayFileDlg&){ return *this; } // just to satisfy script bindings (never used)
		virtual void EndModal(int retCode);
	protected:
		void OnAdd(wxCommandEvent& event);
		void OnEdit(wxCommandEvent& event);
		void OnDelete(wxCommandEvent& event);
		void OnUpdateUI(wxUpdateUIEvent& event);
		wxArrayString& m_Array;
		bool m_UseRelativePaths;
		wxString m_BasePath;
	private:
		DECLARE_EVENT_TABLE()
};

#endif // EDITARRAYFILEDLG_H

