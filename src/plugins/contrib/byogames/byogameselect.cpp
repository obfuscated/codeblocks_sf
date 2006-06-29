#include "sdk.h"
#ifndef CB_PRECOMP
#include <wx/button.h>
#include <wx/intl.h>
#include <wx/listbox.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/string.h>
#endif
#include <wx/settings.h>
#include <wx/statline.h>
#include "byogameselect.h"
#include "byogame.h"

BEGIN_EVENT_TABLE(byoGameSelect,wxDialog)
	//(*EventTable(byoGameSelect)
	EVT_BUTTON(wxID_OK,byoGameSelect::OnPlay)
	EVT_BUTTON(wxID_CANCEL,byoGameSelect::OnCancel)
	//*)
END_EVENT_TABLE()

byoGameSelect::byoGameSelect(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(byoGameSelect)
	Create(parent,id,_("Select game to play"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE);
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	Panel1 = new wxPanel(this,ID_PANEL1,wxDefaultPosition,wxDefaultSize,0);
	Panel1->SetBackgroundColour(wxColour(0,0,128));
	BoxSizer3 = new wxBoxSizer(wxVERTICAL);
	StaticText1 = new wxStaticText(Panel1,ID_STATICTEXT1,_("BYO Games collection"),wxDefaultPosition,wxDefaultSize,wxALIGN_CENTRE);
	StaticText1->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT));
	StaticText1->SetBackgroundColour(wxColour(0,0,127));
	StaticText1->SetFont(wxFont(16,wxFONTFAMILY_SWISS,wxFONTSTYLE_ITALIC,wxFONTWEIGHT_NORMAL,true,_("Arial")));
	BoxSizer3->Add(StaticText1,0,wxALL|wxALIGN_CENTER,4);
	Panel1->SetSizer(BoxSizer3);
	BoxSizer3->Fit(Panel1);
	BoxSizer3->SetSizeHints(Panel1);
	BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("Select game"));
	m_GamesList = new wxListBox(this,ID_LISTBOX1,wxDefaultPosition,wxSize(320,177),0,0,0);
	m_GamesList->SetSelection(-1);
	BoxSizer2 = new wxBoxSizer(wxVERTICAL);
	Button1 = new wxButton(this,wxID_OK,_("Play"),wxDefaultPosition,wxDefaultSize,0);
	if (false) Button1->SetDefault();
	StaticLine1 = new wxStaticLine(this,ID_STATICLINE1,wxDefaultPosition,wxSize(10,-1),0);
	Button2 = new wxButton(this,wxID_CANCEL,_("Cancel"),wxDefaultPosition,wxDefaultSize,0);
	if (false) Button2->SetDefault();
	BoxSizer2->Add(Button1,0,wxBOTTOM|wxALIGN_CENTER|wxEXPAND,4);
	BoxSizer2->Add(StaticLine1,0,wxTOP|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,4);
	BoxSizer2->Add(Button2,0,wxTOP|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,4);
	BoxSizer2->Add(71,18,0);
	StaticBoxSizer1->Add(m_GamesList,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,4);
	StaticBoxSizer1->Add(BoxSizer2,0,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,4);
	BoxSizer4->Add(StaticBoxSizer1,1,wxALL|wxALIGN_CENTER|wxEXPAND,4);
	BoxSizer1->Add(Panel1,0,wxALIGN_CENTER|wxEXPAND,4);
	BoxSizer1->Add(BoxSizer4,1,wxBOTTOM|wxALIGN_CENTER|wxEXPAND,4);
	this->SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	Center();
	//*)

	for ( int i=0; i<byoGameLauncher::GetGamesCount(); ++i )
	{
	    m_GamesList->Append(byoGameLauncher::GetGameName(i));
	}

	m_GamesList->SetSelection(0);
}

byoGameSelect::~byoGameSelect()
{
}


void byoGameSelect::OnCancel(wxCommandEvent& event)
{
    EndModal(-1);
}

void byoGameSelect::OnPlay(wxCommandEvent& event)
{
    if ( m_GamesList->GetSelection() == wxNOT_FOUND ) return;
    EndModal(m_GamesList->GetSelection());
}
