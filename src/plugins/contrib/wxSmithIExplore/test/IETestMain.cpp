/***************************************************************
 * Name:      IETestMain.cpp
 * Purpose:   Code for Application Frame
 * Author:     ()
 * Created:   2010-03-05
 * Copyright:  ()
 * License:
 **************************************************************/

#include "IETestMain.h"
#include <wx/msgdlg.h>

//(*InternalHeaders(IETestFrame)
#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)

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

//(*IdInit(IETestFrame)
const long IETestFrame::ID_BITMAPBUTTON1 = wxNewId();
const long IETestFrame::ID_BITMAPBUTTON2 = wxNewId();
const long IETestFrame::ID_BITMAPBUTTON3 = wxNewId();
const long IETestFrame::ID_BITMAPBUTTON4 = wxNewId();
const long IETestFrame::ID_TEXTCTRL1 = wxNewId();
const long IETestFrame::ID_BITMAPBUTTON5 = wxNewId();
const long IETestFrame::ID_IE1 = wxNewId();
const long IETestFrame::idMenuQuit = wxNewId();
const long IETestFrame::idMenuAbout = wxNewId();
const long IETestFrame::ID_STATUSBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(IETestFrame,wxFrame)
    //(*EventTable(IETestFrame)
    //*)
END_EVENT_TABLE()

IETestFrame::IETestFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(IETestFrame)
    wxMenuItem* MenuItem2;
    wxMenuItem* MenuItem1;
    wxFlexGridSizer* FlexGridSizer2;
    wxMenu* Menu1;
    wxMenuBar* MenuBar1;
    wxFlexGridSizer* FlexGridSizer1;
    wxMenu* Menu2;

    Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
    FlexGridSizer1 = new wxFlexGridSizer(3, 1, 0, 0);
    FlexGridSizer1->AddGrowableCol(0);
    FlexGridSizer1->AddGrowableRow(1);
    FlexGridSizer2 = new wxFlexGridSizer(1, 6, 0, 0);
    FlexGridSizer2->AddGrowableCol(4);
    FlexGridSizer2->AddGrowableRow(0);
    BitmapButton1 = new wxBitmapButton(this, ID_BITMAPBUTTON1, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_GO_BACK")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON1"));
    BitmapButton1->SetToolTip(_("Go Back"));
    FlexGridSizer2->Add(BitmapButton1, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BitmapButton2 = new wxBitmapButton(this, ID_BITMAPBUTTON2, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_GO_FORWARD")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON2"));
    BitmapButton2->SetDefault();
    BitmapButton2->SetToolTip(_("Go Forward"));
    FlexGridSizer2->Add(BitmapButton2, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BitmapButton3 = new wxBitmapButton(this, ID_BITMAPBUTTON3, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_GO_HOME")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON3"));
    BitmapButton3->SetDefault();
    BitmapButton3->SetToolTip(_("Go Home"));
    FlexGridSizer2->Add(BitmapButton3, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BitmapButton4 = new wxBitmapButton(this, ID_BITMAPBUTTON4, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_REDO")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON4"));
    BitmapButton4->SetDefault();
    BitmapButton4->SetToolTip(_("Reload"));
    FlexGridSizer2->Add(BitmapButton4, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrl1 = new wxTextCtrl(this, ID_TEXTCTRL1, _("Text"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    FlexGridSizer2->Add(TextCtrl1, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BitmapButton5 = new wxBitmapButton(this, ID_BITMAPBUTTON5, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_OPEN")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON5"));
    BitmapButton5->SetDefault();
    BitmapButton5->SetToolTip(_("Open URL"));
    FlexGridSizer2->Add(BitmapButton5, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(FlexGridSizer2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    IE1 = new wxIEHtmlWin(this, ID_IE1, wxDefaultPosition, wxSize(455,260), wxRAISED_BORDER|wxTAB_TRAVERSAL, _T("ID_IE1"));
    IE1->LoadUrl(_("http://www.google.com"));
    FlexGridSizer1->Add(IE1, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(FlexGridSizer1);
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("Help"));
    SetMenuBar(MenuBar1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, wxST_SIZEGRIP, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[6] = { -10, -10, -10, -10, -10, -10 };
    int __wxStatusBarStyles_1[6] = { wxSB_NORMAL, wxSB_NORMAL, wxSB_NORMAL, wxSB_NORMAL, wxSB_NORMAL, wxSB_NORMAL };
    StatusBar1->SetFieldsCount(6,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(6,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);
    FlexGridSizer1->Fit(this);
    FlexGridSizer1->SetSizeHints(this);

    Connect(ID_BITMAPBUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&IETestFrame::OnBitmapButton1Click);
    Connect(ID_BITMAPBUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&IETestFrame::OnBitmapButton2Click);
    Connect(ID_BITMAPBUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&IETestFrame::OnBitmapButton3Click);
    Connect(ID_BITMAPBUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&IETestFrame::OnBitmapButton4Click);
    Connect(ID_BITMAPBUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&IETestFrame::OnBitmapButton5Click);
    Connect(ID_IE1,wxEVT_COMMAND_MSHTML_BEFORENAVIGATE2,(wxObjectEventFunction)&IETestFrame::OnIE1BeforeNavigate2);
    Connect(ID_IE1,wxEVT_COMMAND_MSHTML_NEWWINDOW2,(wxObjectEventFunction)&IETestFrame::OnIE1NewWindow2);
    Connect(ID_IE1,wxEVT_COMMAND_MSHTML_DOCUMENTCOMPLETE,(wxObjectEventFunction)&IETestFrame::OnIE1DocumentComplete);
    Connect(ID_IE1,wxEVT_COMMAND_MSHTML_PROGRESSCHANGE,(wxObjectEventFunction)&IETestFrame::OnIE1ProgressChange);
    Connect(ID_IE1,wxEVT_COMMAND_MSHTML_STATUSTEXTCHANGE,(wxObjectEventFunction)&IETestFrame::OnIE1StatusTextChange);
    Connect(ID_IE1,wxEVT_COMMAND_MSHTML_TITLECHANGE,(wxObjectEventFunction)&IETestFrame::OnIE1TitleChange);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&IETestFrame::OnQuit);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&IETestFrame::OnAbout);
    //*)
}

IETestFrame::~IETestFrame()
{
    //(*Destroy(IETestFrame)
    //*)
}

void IETestFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void IETestFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

void IETestFrame::OnBitmapButton1Click(wxCommandEvent& event)
{
    IE1->GoBack();
}

void IETestFrame::OnBitmapButton2Click(wxCommandEvent& event)
{
    IE1->GoForward();
}

void IETestFrame::OnBitmapButton3Click(wxCommandEvent& event)
{
    IE1->GoHome();
}

void IETestFrame::OnBitmapButton4Click(wxCommandEvent& event)
{
    IE1->Refresh(wxIEHTML_REFRESH_NORMAL);
}

void IETestFrame::OnBitmapButton5Click(wxCommandEvent& event)
{
    wxString    s;
    s = TextCtrl1->GetValue();
    if (s.Find(_T("//")) == wxNOT_FOUND) s = _T("http://") + s;
    IE1->LoadUrl(s);
}

void IETestFrame::OnIE1BeforeNavigate2(wxMSHTMLEvent& event)
{
wxString    s;

    s = event.GetText1();
    StatusBar1->SetStatusText(s, 0);
}

void IETestFrame::OnIE1NewWindow2(wxMSHTMLEvent& event)
{
wxString    s;

    s = _T("New Window Requested");
    StatusBar1->SetStatusText(s, 1);
}

void IETestFrame::OnIE1DocumentComplete(wxMSHTMLEvent& event)
{
wxString    s;

    s = event.GetText1();
    StatusBar1->SetStatusText(s, 2);
}

void IETestFrame::OnIE1ProgressChange(wxMSHTMLEvent& event)
{
wxString    s;

    s.Printf(_T("%ld / %ld"), event.GetLong1(), event.GetLong2());
    StatusBar1->SetStatusText(s, 3);
}

void IETestFrame::OnIE1StatusTextChange(wxMSHTMLEvent& event)
{
wxString    s;

    s = event.GetText1();
    StatusBar1->SetStatusText(s, 4);
}

void IETestFrame::OnIE1TitleChange(wxMSHTMLEvent& event)
{
wxString    s;

    s = event.GetText1();
    StatusBar1->SetStatusText(s, 5);
}
