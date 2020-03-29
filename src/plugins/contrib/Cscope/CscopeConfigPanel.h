#ifndef CSCOPECONFIGPANEL_H
#define CSCOPECONFIGPANEL_H

//(*Headers(CscopeConfigPanel)
#include <wx/button.h>
#include <wx/hyperlink.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

#include <configurationpanel.h>

class CscopeConfigPanel: public cbConfigurationPanel
{
	public:

		CscopeConfigPanel(wxWindow* parent);
		virtual ~CscopeConfigPanel();

        /// @return the panel's title.
        virtual wxString GetTitle() const override { return _("Cscope"); }
        /// @return the panel's bitmap base name. You must supply two bitmaps: \<basename\>.png and \<basename\>-off.png...
        virtual wxString GetBitmapBaseName() const override { return wxT("Cscope"); }
        /// Called when the user chooses to apply the configuration.
        virtual void OnApply() override;
        /// Called when the user chooses to cancel the configuration.
        virtual void OnCancel() override { ; }


		//(*Declarations(CscopeConfigPanel)
		wxTextCtrl* txtCscopeApp;
		//*)

	protected:

		//(*Identifiers(CscopeConfigPanel)
		static const long ID_TXT_CPP_CHECK_APP;
		static const long ID_BTN_CPPCHECK_APP;
		static const long ID_HYC_CPP_CHECK_WWW;
		//*)

	private:

        wxString GetDefaultCscopeExecutableName();

		//(*Handlers(CscopeConfigPanel)
		void OnbtnCscopeAppClick(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
