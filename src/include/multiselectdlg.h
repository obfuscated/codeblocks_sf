/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef MULTISELECTDLG_H
#define MULTISELECTDLG_H

#include <wx/intl.h>
#include <wx/string.h>
#include <wx/dynarray.h>
#include <wx/dialog.h>

class MultiSelectDlg : public wxDialog
{
	public:
		MultiSelectDlg(wxWindow* parent,
                        const wxArrayString& items,
                        const wxString& wildcard,
                        const wxString& label = _("Select items:"),
                        const wxString& title = _("Multiple selection"));
		MultiSelectDlg(wxWindow* parent,
                        const wxArrayString& items,
                        bool selectall = false,
                        const wxString& label = _("Select items:"),
                        const wxString& title = _("Multiple selection"));
		virtual ~MultiSelectDlg();
		
		wxArrayString GetSelectedStrings() const;
		wxArrayInt GetSelectedIndices() const;
		
		void SelectWildCard(const wxString& wild, bool select = true, bool clearOld = false);
	protected:
        void Init(const wxArrayString& items, const wxString& wildcard);
        void UpdateStatus();
        void OnWildcard(wxCommandEvent& event);
        void OnToggle(wxCommandEvent& event);
        void OnSelectAll(wxCommandEvent& event);
        void OnDeselectAll(wxCommandEvent& event);
        void OnItemToggle(wxCommandEvent& event);
	private:
        DECLARE_EVENT_TABLE();
};

#endif // MULTISELECTDLG_H
