/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef CONFIRMREPLACEDLG_H
#define CONFIRMREPLACEDLG_H

#include "scrollingdialog.h"
#include <wx/intl.h>
#include "settings.h"

class cbStyledTextCtrl;

enum ConfirmResponse
{
	crYes = 0,
	crNo,
	crAllInFile,
	crSkipFile,
	crAll,
	crCancel
};

class DLLIMPORT ConfirmReplaceDlg : public wxScrollingDialog
{
	public:
		ConfirmReplaceDlg(wxWindow* parent, bool replaceInFiles = false,
            const wxString& label = _("Replace this occurrence?"));
		~ConfirmReplaceDlg();
		void OnYes(wxCommandEvent& event);
		void OnNo(wxCommandEvent& event);
		void OnAllInFile(wxCommandEvent& event);
		void OnSkipFile(wxCommandEvent& event);
		void OnAll(wxCommandEvent& event);
		void OnCancel(wxCommandEvent& event);
		void CalcPosition(cbStyledTextCtrl* ed);

	private:
		DECLARE_EVENT_TABLE()
};

#endif // CONFIRMREPLACEDLG_H
