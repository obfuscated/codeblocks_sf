#ifndef CONFIRMREPLACEDLG_H
#define CONFIRMREPLACEDLG_H

#include <wx/dialog.h>
#include "settings.h"

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
		ConfirmReplaceDlg(wxWindow* parent);
		~ConfirmReplaceDlg();
		void OnYes(wxCommandEvent& event);
		void OnNo(wxCommandEvent& event);
		void OnAll(wxCommandEvent& event);
		void OnCancel(wxCommandEvent& event);
	private:
		DECLARE_EVENT_TABLE()
};

#endif // CONFIRMREPLACEDLG_H
