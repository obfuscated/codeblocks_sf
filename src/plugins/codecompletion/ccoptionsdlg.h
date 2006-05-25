#ifndef CCOPTIONSDLG_H
#define CCOPTIONSDLG_H

#include <wx/intl.h>
#include "configurationpanel.h"
#include <settings.h>
#include "nativeparser.h"
#include "parser/parser.h"

#define USE_CUST_CTRL false

class CCOptionsDlg : public cbConfigurationPanel
{
	public:
		CCOptionsDlg(wxWindow* parent, NativeParser* np);
		virtual ~CCOptionsDlg();

        virtual wxString GetTitle() const { return _("Code-completion and symbols browser"); }
        virtual wxString GetBitmapBaseName() const { return _T("generic-plugin"); }
        virtual void OnApply();
        virtual void OnCancel(){}
	protected:
		void OnOK(wxCommandEvent& event);
		void OnChooseColour(wxCommandEvent& event);
		void OnInheritanceToggle(wxCommandEvent& event);
		void OnSliderScroll(wxScrollEvent& event);
		void OnUpdateUI(wxUpdateUIEvent& event);
	private:
		void UpdateSliderLabel();
		Parser m_Parser;
		NativeParser* m_pNativeParsers;
		DECLARE_EVENT_TABLE()
};

#endif // CCOPTIONSDLG_H
