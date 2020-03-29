#ifndef BYOGAMESELECT_H
#define BYOGAMESELECT_H

#include "scrollingdialog.h"

class wxBoxSizer;
class wxStaticText;
class wxPanel;
class wxStaticBoxSizer;
class wxListBox;
class wxButton;
class wxStaticLine;
class wxCommandEvent;

class byoGameSelect: public wxScrollingDialog
{
	public:

		byoGameSelect(wxWindow* parent,wxWindowID id = -1);
		virtual ~byoGameSelect();

		//(*Identifiers(byoGameSelect)
		static const long ID_STATICTEXT1;
		static const long ID_PANEL1;
		static const long ID_LISTBOX1;
		static const long ID_STATICLINE1;
		//*)

	protected:

		//(*Handlers(byoGameSelect)
		void OnCancel(wxCommandEvent& event);
		void OnPlay(wxCommandEvent& event);
		//*)

		//(*Declarations(byoGameSelect)
		wxPanel* Panel1;
		wxBoxSizer* BoxSizer3;
		wxButton* Button1;
		wxButton* Button2;
		wxBoxSizer* BoxSizer2;
		wxStaticText* StaticText1;
		wxStaticLine* StaticLine1;
		wxListBox* m_GamesList;
		wxBoxSizer* BoxSizer4;
		wxBoxSizer* BoxSizer1;
		wxStaticBoxSizer* StaticBoxSizer1;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif // BYOGAMESELECT_H
