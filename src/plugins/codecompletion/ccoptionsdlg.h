#ifndef CCOPTIONSDLG_H
#define CCOPTIONSDLG_H

#include <wx/dialog.h>
#include <settings.h>
#include "parser/parser.h"

class CCOptionsDlg : public wxDialog
{
	public:
		CCOptionsDlg(wxWindow* parent);
		virtual ~CCOptionsDlg();
	protected:
		void OnOK(wxCommandEvent& event);
		void OnChooseColor(wxCommandEvent& event);
		void OnInheritanceToggle(wxCommandEvent& event);
		void OnSliderScroll(wxScrollEvent& event);
	private:
		void UpdateSliderLabel();
		Parser m_Parser;
		DECLARE_EVENT_TABLE()
};

#endif // CCOPTIONSDLG_H

