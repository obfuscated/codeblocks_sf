#include "main.h"

int idMenuQuit = wxNewId();
int idMenuAbout = wxNewId();

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_MENU(idMenuQuit, MyFrame::OnQuit)
	EVT_MENU(idMenuAbout, MyFrame::OnAbout)
END_EVENT_TABLE()

MyFrame::MyFrame(wxFrame *frame, const wxString& title)
	: wxFrame(frame, -1, title)
{
	wxMenuBar* mbar = new wxMenuBar();
	wxMenu* fileMenu = new wxMenu(_(""));
	fileMenu->Append(idMenuQuit, _("&Quit\tAlt-F4"), _("Quit the application"));
	mbar->Append(fileMenu, _("&File"));

	wxMenu* helpMenu = new wxMenu(_(""));
	helpMenu->Append(idMenuAbout, _("&About\tF1"), _("Show info about this application"));
	mbar->Append(helpMenu, _("&Help"));
	
	SetMenuBar(mbar);
}

MyFrame::~MyFrame()
{
}

void MyFrame::OnQuit(wxCommandEvent& event)
{
	Close();
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageBox(_("wxWidgets Application Template"), _("Welcome to..."));
}
