/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef EDITARRAYORDERDLG_H
#define EDITARRAYORDERDLG_H

#include <wx/dialog.h>
#include <wx/arrstr.h>
#include "settings.h"

/*
 * No description
 */
class EditArrayOrderDlg : public wxDialog
{
	public:
		// class constructor
		EditArrayOrderDlg(wxWindow* parent, const wxArrayString& array = 0L);
		// class destructor
		~EditArrayOrderDlg();
		EditArrayOrderDlg& operator=(const EditArrayOrderDlg&){ return *this; } // just to satisfy script bindings (never used)
		void SetArray(const wxArrayString& array){ m_Array = array; }
		const wxArrayString& GetArray(){ return m_Array; }
		void EndModal(int retCode);
		void OnMoveUp(wxCommandEvent& event);
		void OnMoveDown(wxCommandEvent& event);
		void OnUpdateUI(wxUpdateUIEvent& event);
    private:
        void DoFillList();
        wxArrayString m_Array;
    	DECLARE_EVENT_TABLE()
};

#endif // EDITARRAYORDERDLG_H

