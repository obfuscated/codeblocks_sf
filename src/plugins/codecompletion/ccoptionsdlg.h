#ifndef CCOPTIONSDLG_H
#define CCOPTIONSDLG_H

#include <wx/intl.h>
#include "configurationpanel.h"
#include <settings.h>
#include "nativeparser.h"
#include "parser/parser.h"

class CodeCompletion;

class CCOptionsDlg : public cbConfigurationPanel
{
	public:
		CCOptionsDlg(wxWindow* parent, NativeParser* np, CodeCompletion* cc);
		virtual ~CCOptionsDlg();

        virtual wxString GetTitle() const { return _("Code-completion and symbols browser"); }
        virtual wxString GetBitmapBaseName() const { return _T("generic-plugin"); }
        virtual void OnApply();
        virtual void OnCancel(){}
	protected:
        void OnAddRepl(wxCommandEvent& event);
        void OnEditRepl(wxCommandEvent& event);
        void OnDelRepl(wxCommandEvent& event);
		void OnOK(wxCommandEvent& event);
		void OnChooseColour(wxCommandEvent& event);
		void OnSliderScroll(wxScrollEvent& event);
		void OnUpdateUI(wxUpdateUIEvent& event);
	private:
		void UpdateSliderLabel();
		bool ValidateReplacementToken(wxString& from, wxString& to);
		Parser m_Parser;
		NativeParser* m_pNativeParsers;
		CodeCompletion* m_pCodeCompletion;
		DECLARE_EVENT_TABLE()
};

#endif // CCOPTIONSDLG_H
