#ifndef BYOGAMESELECT_H
#define BYOGAMESELECT_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(byoGameSelect)
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/intl.h>
#include <wx/listbox.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>
//*)

class byoGameSelect: public wxDialog
{
	public:

		byoGameSelect(wxWindow* parent,wxWindowID id = -1);
		virtual ~byoGameSelect();

		//(*Identifiers(byoGameSelect)
		enum Identifiers
		{
		    ID_LISTBOX1 = 0x1000,
		    ID_PANEL1,
		    ID_STATICLINE1,
		    ID_STATICTEXT1
		};
		//*)

	protected:

		//(*Handlers(byoGameSelect)
		void OnCancel(wxCommandEvent& event);
		void OnPlay(wxCommandEvent& event);
		//*)

		//(*Declarations(byoGameSelect)
		wxBoxSizer* BoxSizer1;
		wxPanel* Panel1;
		wxBoxSizer* BoxSizer3;
		wxStaticText* StaticText1;
		wxBoxSizer* BoxSizer4;
		wxStaticBoxSizer* StaticBoxSizer1;
		wxListBox* m_GamesList;
		wxBoxSizer* BoxSizer2;
		wxButton* Button1;
		wxStaticLine* StaticLine1;
		wxButton* Button2;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif
