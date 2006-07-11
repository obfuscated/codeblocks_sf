#ifndef GENERICSELECTPATH_H
#define GENERICSELECTPATH_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(GenericSelectPath)
#include <wx/button.h>
#include <wx/intl.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

class GenericSelectPath: public wxPanel
{
	public:

		GenericSelectPath(wxWindow* parent,wxWindowID id = -1);
		virtual ~GenericSelectPath();

		// use this because it adjusts the sizer too
		void SetDescription(const wxString& descr)
		{
		    lblDescr->SetLabel(descr);

            GetSizer()->Fit(this);
            GetSizer()->SetSizeHints(this);
		}

		//(*Identifiers(GenericSelectPath)
		enum Identifiers
		{
		    ID_BUTTON1 = 0x1000,
		    ID_STATICTEXT1,
		    ID_STATICTEXT2,
		    ID_TEXTCTRL1
		};
		//*)

		//(*Handlers(GenericSelectPath)
		//*)

		//(*Declarations(GenericSelectPath)
		wxBoxSizer* BoxSizer1;
		wxStaticText* lblDescr;
		wxStaticText* lblLabel;
		wxBoxSizer* BoxSizer2;
		wxTextCtrl* txtFolder;
		wxButton* btnBrowse;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif
