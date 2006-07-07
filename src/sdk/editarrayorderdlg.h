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

