/***************************************************************
 * Name:      [PROJECT_NAME]Main.cpp
 * Purpose:   Code for Application Frame
 * Author:    [AUTHOR_NAME] ([AUTHOR_EMAIL])
 * Created:   [NOW]
 * Copyright: [AUTHOR_NAME] ([AUTHOR_WWW])
 * License:   
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "[PROJECT_NAME]Main.h"

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

[IF WXFRAME][IF NONE]BEGIN_EVENT_TABLE([PROJECT_NAME]Frame, wxFrame)
	EVT_CLOSE([PROJECT_NAME]Frame::OnClose)
    EVT_MENU(idMenuQuit, [PROJECT_NAME]Frame::OnQuit)
    EVT_MENU(idMenuAbout, [PROJECT_NAME]Frame::OnAbout)
END_EVENT_TABLE()

[PROJECT_NAME]Frame::[PROJECT_NAME]Frame(wxFrame *frame, const wxString& title)
    : wxFrame(frame, -1, title)
{
#if wxUSE_MENUS
    // create a menu bar
    wxMenuBar* mbar = new wxMenuBar();
    wxMenu* fileMenu = new wxMenu(_T(""));
    fileMenu->Append(idMenuQuit, _("&Quit\tAlt-F4"), _("Quit the application"));
    mbar->Append(fileMenu, _("&File"));

    wxMenu* helpMenu = new wxMenu(_T(""));
    helpMenu->Append(idMenuAbout, _("&About\tF1"), _("Show info about this application"));
    mbar->Append(helpMenu, _("&Help"));

    SetMenuBar(mbar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar with some information about the used wxWidgets version
    CreateStatusBar(2);
    SetStatusText(_("Hello Code::Blocks user!"),0);
    SetStatusText(wxbuildinfo(short_f), 1);
#endif // wxUSE_STATUSBAR

}[ENDIF NONE]
[IF WXFB][PROJECT_NAME]Frame::[PROJECT_NAME]Frame(wxFrame *frame)
	: GUIFrame(frame)
{
#if wxUSE_STATUSBAR
	statusBar->SetStatusText(_("Hello Code::Blocks user!"), 0);
	statusBar->SetStatusText(wxbuildinfo(short_f), 1);
#endif
}[ENDIF WXFB]

[PROJECT_NAME]Frame::~[PROJECT_NAME]Frame()
{
}

void [PROJECT_NAME]Frame::OnClose(wxCloseEvent &event)
{
    Destroy();
}

void [PROJECT_NAME]Frame::OnQuit(wxCommandEvent &event)
{
    Destroy();
}

void [PROJECT_NAME]Frame::OnAbout(wxCommandEvent &event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}[ENDIF WXFRAME]
[IF WXDIALOG][IF NONE]BEGIN_EVENT_TABLE([PROJECT_NAME]Dialog, wxDialog)
	EVT_CLOSE([PROJECT_NAME]Dialog::OnClose)
    EVT_BUTTON(idBtnQuit, [PROJECT_NAME]Dialog::OnQuit)
    EVT_BUTTON(idBtnAbout, [PROJECT_NAME]Dialog::OnAbout)
END_EVENT_TABLE()

[PROJECT_NAME]Dialog::[PROJECT_NAME]Dialog(wxDialog *dlg, const wxString &title)
    : wxDialog(dlg, -1, title)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxHORIZONTAL);
	m_staticText1 = new wxStaticText(this, wxID_ANY, wxT("Welcome To\nwxWidgets"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1->SetFont(wxFont(20, 74, 90, 90, false, wxT("Arial")));
	bSizer1->Add(m_staticText1, 0, wxALL|wxEXPAND, 5);
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxVERTICAL);
	BtnAbout = new wxButton(this, idBtnAbout, wxT("&About"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer2->Add(BtnAbout, 0, wxALL, 5);
	m_staticline1 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	bSizer2->Add(m_staticline1, 0, wxALL|wxEXPAND, 5);
	BtnQuit = new wxButton(this, idBtnQuit, wxT("&Quit"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer2->Add(BtnQuit, 0, wxALL, 5);
	bSizer1->Add(bSizer2, 1, wxEXPAND, 5);
	this->SetSizer(bSizer1);
	this->Layout();
	bSizer1->Fit(this);
}[ENDIF NONE]
[IF WXFB][PROJECT_NAME]Dialog::[PROJECT_NAME]Dialog(wxDialog *dlg)
    : GUIDialog(dlg)
{
}[ENDIF WXFB]

[PROJECT_NAME]Dialog::~[PROJECT_NAME]Dialog()
{
}

void [PROJECT_NAME]Dialog::OnClose(wxCloseEvent &event)
{
    Destroy();
}

void [PROJECT_NAME]Dialog::OnQuit(wxCommandEvent &event)
{
    Destroy();
}

void [PROJECT_NAME]Dialog::OnAbout(wxCommandEvent &event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}[ENDIF WXDIALOG]