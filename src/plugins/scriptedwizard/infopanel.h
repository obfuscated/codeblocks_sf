#ifndef INFOPANEL_H
#define INFOPANEL_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(InfoPanel)
#include <wx/checkbox.h>
#include <wx/intl.h>
#include <wx/panel.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
//*)

class InfoPanel: public wxPanel
{
	public:

		InfoPanel(wxWindow* parent,wxWindowID id = -1);
		virtual ~InfoPanel();

		//(*Identifiers(InfoPanel)
		enum Identifiers
		{
		    ID_CHECKBOX1 = 0x1000,
		    ID_STATICTEXT1
		};
		//*)

        void SetIntroText(const wxString& intro_msg)
        {
            lblIntro->SetLabel(intro_msg);

            GetSizer()->Fit(this);
            GetSizer()->SetSizeHints(this);
        }
//	protected:

		//(*Handlers(InfoPanel)
		//*)

		//(*Declarations(InfoPanel)
		wxBoxSizer* BoxSizer1;
		wxStaticText* lblIntro;
		wxCheckBox* chkSkip;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif
