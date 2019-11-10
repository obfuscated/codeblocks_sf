#ifndef LISTVIEWCOMBOPOPUP_H
#define LISTVIEWCOMBOPOPUP_H

#ifndef WX_PRECOMP
	//(*HeadersPCH(ListViewComboPopup)
	//*)
#endif
//(*Headers(ListViewComboPopup)
//*)

class ListViewComboPopup: public wxComboPopup
{
	public:

		ListViewComboPopup(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~ListViewComboPopup();

		//(*Declarations(ListViewComboPopup)
		//*)

	protected:

		//(*Identifiers(ListViewComboPopup)
		//*)

	private:

		//(*Handlers(ListViewComboPopup)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
