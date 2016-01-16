/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef EDITPATHDLG_H
#define EDITPATHDLG_H

#include "scrollingdialog.h"
#include <wx/intl.h>

class DLLIMPORT EditPathDlg : public wxScrollingDialog
{
	public:
		EditPathDlg(wxWindow* parent,
                const wxString& path,       // initial path
                const wxString& basepath,   // for relative pathes
                const wxString& title = _("Edit Path"),      // title of the dialog
                const wxString& message = _T(""),    // message displayed in the dialogs
                const bool wantDir = true,  // whether to open a dir or a file dialog
                const bool allowMultiSel = false,  // whether to allow for multiple files selection
                const wxString& filter = _("All files(*)|*"));  // wildcard for files

		virtual ~EditPathDlg();
		EditPathDlg& operator=(const EditPathDlg&){ return *this; } // just to satisfy script bindings (never used)

		const wxString& GetPath(){ return m_Path; }
        void EndModal(int retCode);

	protected:
        void OnBrowse(wxCommandEvent& event);
        void OnUpdateUI(wxUpdateUIEvent& event);

        wxString m_Path;
        wxString m_Message, m_Basepath, m_Filter;
        bool     m_WantDir, m_AllowMultiSel, m_AskMakeRelative, m_ShowCreateDirButton;

	private:
        DECLARE_EVENT_TABLE();
};

#endif // EDITPATHDLG_H
