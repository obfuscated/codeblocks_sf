#ifndef EDITARRAYSTRINGDLG_H
#define EDITARRAYSTRINGDLG_H

#include <wx/dialog.h>
#include <wx/arrstr.h>
#include "settings.h"

class DLLIMPORT EditArrayStringDlg : public wxDialog
{
	public:
		EditArrayStringDlg(wxWindow* parent, wxArrayString& array);
		virtual ~EditArrayStringDlg();
		EditArrayStringDlg& operator=(const EditArrayStringDlg&){ return *this; } // just to satisfy script bindings (never used)
		virtual void EndModal(int retCode);
	protected:
		void OnAdd(wxCommandEvent& event);
		void OnEdit(wxCommandEvent& event);
		void OnDelete(wxCommandEvent& event);
		void OnUpdateUI(wxUpdateUIEvent& event);
		wxArrayString& m_Array;
	private:
		DECLARE_EVENT_TABLE()
};

#endif // EDITARRAYSTRINGDLG_H

