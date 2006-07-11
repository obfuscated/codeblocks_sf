#ifndef INTROPANEL_H
#define INTROPANEL_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(IntroPanel)
#include <wx/intl.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
//*)

class IntroPanel: public wxPanel
{
	public:

		IntroPanel(wxWindow* parent,wxWindowID id = -1);
		virtual ~IntroPanel();

		//(*Identifiers(IntroPanel)
		enum Identifiers
		{
		    ID_STATICTEXT1 = 0x1000
		};
		//*)

        void SetIntroText(const wxString& intro_msg)
        {
            lblIntro->SetLabel(intro_msg);

            GetSizer()->Fit(this);
            GetSizer()->SetSizeHints(this);
        }
	protected:

		//(*Handlers(IntroPanel)
		//*)

		//(*Declarations(IntroPanel)
		wxBoxSizer* BoxSizer1;
		wxStaticText* lblIntro;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif
