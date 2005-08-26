#ifndef CONFIRMREPLACEDLG_H
#define CONFIRMREPLACEDLG_H

#include <wx/dialog.h>
#include <wx/intl.h>
#include "settings.h"
#include "cbeditor.h"

enum ConfirmResponse
{
	crYes = 0,
	crNo,
	crAll,
	crCancel
};

class ConfirmReplaceDlg : public wxDialog
{
	public:
		ConfirmReplaceDlg(wxWindow* parent, const wxString& label = _("Replace this occurence?"));
		~ConfirmReplaceDlg();
		void OnYes(wxCommandEvent& event);
		void OnNo(wxCommandEvent& event);
		void OnAll(wxCommandEvent& event);
		void OnCancel(wxCommandEvent& event);
		void CalcPosition(cbStyledTextCtrl* ed);
		
	private:
		DECLARE_EVENT_TABLE()
};

#endif // CONFIRMREPLACEDLG_H
