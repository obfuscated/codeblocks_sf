/////////////////////////////////////////////////////////////////////////////
// Name:        things.cpp
// Purpose:     wxThings control test program
// Author:      John Labenski
// Modified by:
// Created:     04/01/98
// RCS-ID:
// Copyright:   (c) John Labenski
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/buffer.h"
#include "wx/image.h"
#include "wx/splitter.h"
#include "wx/tglbtn.h"
#include "wx/notebook.h"
#include "wx/things/toggle.h"
#include "wx/things/spinctld.h"
#include "wx/things/menubtn.h"
#include "wx/things/bmpcombo.h"

/* XPM */
static char *mondrian_xpm[] = {
/* columns rows colors chars-per-pixel */
"32 32 6 1",
"  c Black",
". c Blue",
"X c #00bf00",
"o c Red",
"O c Yellow",
"+ c Gray100",
/* pixels */
"                                ",
" oooooo +++++++++++++++++++++++ ",
" oooooo +++++++++++++++++++++++ ",
" oooooo +++++++++++++++++++++++ ",
" oooooo +++++++++++++++++++++++ ",
" oooooo +++++++++++++++++++++++ ",
" oooooo +++++++++++++++++++++++ ",
" oooooo +++++++++++++++++++++++ ",
"                                ",
" ++++++ ++++++++++++++++++ .... ",
" ++++++ ++++++++++++++++++ .... ",
" ++++++ ++++++++++++++++++ .... ",
" ++++++ ++++++++++++++++++ .... ",
" ++++++ ++++++++++++++++++ .... ",
" ++++++ ++++++++++++++++++      ",
" ++++++ ++++++++++++++++++ ++++ ",
" ++++++ ++++++++++++++++++ ++++ ",
" ++++++ ++++++++++++++++++ ++++ ",
" ++++++ ++++++++++++++++++ ++++ ",
" ++++++ ++++++++++++++++++ ++++ ",
" ++++++ ++++++++++++++++++ ++++ ",
" ++++++ ++++++++++++++++++ ++++ ",
" ++++++ ++++++++++++++++++ ++++ ",
" ++++++ ++++++++++++++++++ ++++ ",
" ++++++                    ++++ ",
" ++++++ OOOOOOOOOOOO XXXXX ++++ ",
" ++++++ OOOOOOOOOOOO XXXXX ++++ ",
" ++++++ OOOOOOOOOOOO XXXXX ++++ ",
" ++++++ OOOOOOOOOOOO XXXXX ++++ ",
" ++++++ OOOOOOOOOOOO XXXXX ++++ ",
" ++++++ OOOOOOOOOOOO XXXXX ++++ ",
"                                "};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

enum
{
    Minimal_Quit,
    Minimal_About = wxID_ABOUT,
};

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size,
            long style = wxDEFAULT_FRAME_STYLE);

    virtual ~MyFrame() { }

    wxScrolledWindow *CreateButtonPage(wxWindow *parent);

    void OnQuit(wxCommandEvent& WXUNUSED(event)) { Close(true); }
    void OnAbout(wxCommandEvent& event);

    void OnSpin(wxSpinEvent &event);
    void OnTextEnter(wxCommandEvent &event);
    void OnText(wxCommandEvent &event);
    void OnToggle(wxCommandEvent &event);
    void OnButton(wxCommandEvent &event);
    void OnMenu(wxCommandEvent &event);
    void OnComboBox(wxCommandEvent &event);

    wxTextCtrl *m_textCtrl;

    wxNotebook *m_notebook;

private:
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

class MyApp : public wxApp
{
public:
    virtual bool OnInit()
    {
        MyFrame *frame = new MyFrame(_T("Minimal wxWidgets App"),
                                 wxPoint(50, 50), wxSize(450, 400));
        frame->Show(true);

        return true;
    }
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)

    EVT_SPINCTRL    (wxID_ANY, MyFrame::OnSpin)
    EVT_TEXT_ENTER  (wxID_ANY, MyFrame::OnTextEnter)
    EVT_TEXT        (wxID_ANY, MyFrame::OnText)
    EVT_TOGGLEBUTTON(wxID_ANY, MyFrame::OnToggle)
    EVT_BUTTON      (wxID_ANY, MyFrame::OnButton)
    EVT_MENU        (wxID_ANY, MyFrame::OnMenu)
    EVT_COMBOBOX    (wxID_ANY, MyFrame::OnComboBox)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style)
       : wxFrame(NULL, -1, title, pos, size, style)
{
#if wxUSE_MENUS
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(Minimal_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, _T("&About...\tF1"), _T("Show about dialog"));

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxWidgets!"));
#endif // wxUSE_STATUSBAR

    wxInitAllImageHandlers();

    wxSplitterWindow *splitter = new wxSplitterWindow(this, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D);
    splitter->SetMinimumPaneSize(20);

    m_textCtrl = new wxTextCtrl( splitter, -1, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxVSCROLL|wxHSCROLL );

    m_notebook = new wxNotebook(splitter, -1);

    m_notebook->AddPage(CreateButtonPage(m_notebook), wxT("Controls"));

    splitter->SplitHorizontally(m_notebook, m_textCtrl, 300);
}

wxScrolledWindow *MyFrame::CreateButtonPage(wxWindow *parent)
{
    wxScrolledWindow *panel = new wxScrolledWindow(parent, -1);
    panel->SetScrollbars(1, 1, 2000, 2000);

    wxFlexGridSizer *item0 = new wxFlexGridSizer( 4, 0, 0 );

    wxBitmap mondrian(mondrian_xpm);

    int ID = 100;

    item0->Add(new wxStaticText(panel, -1, wxT("wxSpinCtrl constructor")), 0, wxALIGN_CENTRE|wxALL, 5 );
    item0->Add(new wxStaticText(panel, -1, wxT("Native constructor")), 0, wxALIGN_CENTRE|wxALL, 5 );
    item0->Add(new wxStaticText(panel, -1, wxT("Exponential")), 0, wxALIGN_CENTRE|wxALL, 5 );
    item0->Add(new wxStaticText(panel, -1, wxT("Default constructor")), 0, wxALIGN_CENTRE|wxALL, 5 );

    wxSpinCtrlDbl *item1 = new wxSpinCtrlDbl( panel, ID++, wxT("hello"), wxDefaultPosition, wxSize(100,-1), 0, 0, 100, 0 );
    item0->Add( item1, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT, 5 );

    wxSpinCtrlDbl *item2 = new wxSpinCtrlDbl( *panel, ID++, wxT("hello"), wxDefaultPosition, wxSize(100,-1), 0, 0, 100, .003, .001 );
    item0->Add( item2, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxSpinCtrlDbl *item3 = new wxSpinCtrlDbl( *panel, ID++, wxT("hello"), wxDefaultPosition, wxSize(100,-1), 0, 0, 100, .003, .001 );
    item3->SetDigits(4, wxSpinCtrlDbl::lf_fmt);
    item0->Add( item3, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxSpinCtrlDbl *item4 = new wxSpinCtrlDbl( panel, ID++, wxT("0"), wxDefaultPosition, wxSize(100,-1), 0, 0, 100, 0 );
    item0->Add( item4, 0, wxALIGN_CENTRE|wxALL, 5 );

    // next row

    item0->Add(new wxStaticText(panel, -1, wxT("wxButton")), 0, wxALIGN_CENTRE|wxALL, 5 );
    item0->Add(new wxStaticText(panel, -1, wxT("wxToggleButton")), 0, wxALIGN_CENTRE|wxALL, 5 );
    item0->Add(new wxStaticText(panel, -1, wxT("Button Dclick Toggle")), 0, wxALIGN_CENTRE|wxALL, 5 );
    item0->Add(new wxStaticText(panel, -1, wxT("Toggle Dclick Button")), 0, wxALIGN_CENTRE|wxALL, 5 );

    // next row

    wxCustomButton *cbutton;

    cbutton = new wxCustomButton( panel, ID++, wxT("Left"), mondrian, wxDefaultPosition, wxDefaultSize, wxCUSTBUT_BUTTON|wxCUSTBUT_LEFT );
    item0->Add( cbutton, 0, wxALIGN_CENTRE|wxALL, 5 );

    cbutton = new wxCustomButton( panel, ID++, wxT("Right"), mondrian, wxDefaultPosition, wxDefaultSize, wxCUSTBUT_TOGGLE|wxCUSTBUT_RIGHT );
    item0->Add( cbutton, 0, wxALIGN_CENTRE|wxALL, 5 );

    cbutton = new wxCustomButton( panel, ID++, wxT("Top"), mondrian, wxDefaultPosition, wxDefaultSize, wxCUSTBUT_BUT_DCLICK_TOG|wxCUSTBUT_TOP );
    item0->Add( cbutton, 0, wxALIGN_CENTRE|wxALL, 5 );

    cbutton = new wxCustomButton( panel, ID++, wxT("Bottom"), mondrian, wxDefaultPosition, wxDefaultSize, wxCUSTBUT_TOG_DCLICK_BUT|wxCUSTBUT_BOTTOM );
    item0->Add( cbutton, 0, wxALIGN_CENTRE|wxALL, 5 );

    // next row

    cbutton = new wxCustomButton( panel, ID++, wxT("Left"), mondrian, wxDefaultPosition, wxDefaultSize, wxCUSTBUT_BUTTON|wxCUSTBUT_LEFT );
    cbutton->SetMargins(wxSize(20,20), true);
    cbutton->Enable(false);
    item0->Add( cbutton, 0, wxALIGN_CENTRE|wxALL, 5 );

    cbutton = new wxCustomButton( panel, ID++, wxT("Right"), mondrian, wxDefaultPosition, wxDefaultSize, wxCUSTBUT_TOGGLE|wxCUSTBUT_RIGHT );
    cbutton->SetMargins(wxSize(20,20), true);
    item0->Add( cbutton, 0, wxALIGN_CENTRE|wxALL, 5 );

    cbutton = new wxCustomButton( panel, ID++, wxT("Top"), mondrian, wxDefaultPosition, wxDefaultSize, wxCUSTBUT_BUT_DCLICK_TOG|wxCUSTBUT_TOP );
    cbutton->SetMargins(wxSize(20,20), true);
    item0->Add( cbutton, 0, wxALIGN_CENTRE|wxALL, 5 );

    cbutton = new wxCustomButton( panel, ID++, wxT("Bottom"), mondrian, wxDefaultPosition, wxDefaultSize, wxCUSTBUT_TOG_DCLICK_BUT|wxCUSTBUT_BOTTOM );
    cbutton->SetMargins(wxSize(20,20), true);
    item0->Add( cbutton, 0, wxALIGN_CENTRE|wxALL, 5 );

    // next row

    cbutton = new wxCustomButton( panel, ID++, mondrian, wxDefaultPosition, wxDefaultSize, wxCUSTBUT_BUTTON|wxCUSTBUT_LEFT );
    item0->Add( cbutton, 0, wxALIGN_CENTRE|wxALL, 5 );

    cbutton = new wxCustomButton( panel, ID++, mondrian, wxDefaultPosition, wxDefaultSize, wxCUSTBUT_TOGGLE|wxCUSTBUT_RIGHT );
    cbutton->SetMargins(wxSize(20,20), true);
    item0->Add( cbutton, 0, wxALIGN_CENTRE|wxALL, 5 );

    cbutton = new wxCustomButton( panel, ID++, wxT("Top"), wxDefaultPosition, wxDefaultSize, wxCUSTBUT_BUT_DCLICK_TOG|wxCUSTBUT_TOP );
    item0->Add( cbutton, 0, wxALIGN_CENTRE|wxALL, 5 );

    cbutton = new wxCustomButton( panel, ID++, wxT("Bottom"), wxDefaultPosition, wxDefaultSize, wxCUSTBUT_TOG_DCLICK_BUT|wxCUSTBUT_BOTTOM );
    cbutton->SetMargins(wxSize(20,20), true);
    item0->Add( cbutton, 0, wxALIGN_CENTRE|wxALL, 5 );

    // next row

    item0->Add(new wxStaticText(panel, -1, wxT("wxButton")), 0, wxALIGN_CENTRE|wxALL, 5 );
    item0->Add(new wxStaticText(panel, -1, wxT("wxToggleButton")), 0, wxALIGN_CENTRE|wxALL, 5 );
    item0->Add(new wxStaticText(panel, -1, wxT("Button Dclick Toggle")), 0, wxALIGN_CENTRE|wxALL, 5 );
    item0->Add(new wxStaticText(panel, -1, wxT("Toggle Dclick Button")), 0, wxALIGN_CENTRE|wxALL, 5 );

    // next row

    wxMenuButton *menubtn;

    menubtn = new wxMenuButton(panel, ID++, mondrian);
    item0->Add( menubtn, 0, wxALIGN_CENTRE|wxALL, 5 );
    wxMenu *dmenu = new wxMenu();
    dmenu->AppendRadioItem(ID++, wxT("Tree view"));
    dmenu->AppendRadioItem(ID++, wxT("List view"));
    dmenu->AppendRadioItem(ID++, wxT("Details view"));
    dmenu->AppendRadioItem(ID++, wxT("Small icon view"));
    dmenu->AppendRadioItem(ID++, wxT("Large icon view"));
    dmenu->AppendRadioItem(ID++, wxT("Image preview view"));
    menubtn->AssignMenu(dmenu);
/*
    wxArrayBitmap bitmaps;
    bitmaps.Add(wxBitmap(wxImage(mondrian.ConvertToImage()).Scale(6,6)));
    bitmaps.Add(wxBitmap(wxImage(mondrian.ConvertToImage()).Scale(10,10)));
    bitmaps.Add(wxBitmap(wxImage(mondrian.ConvertToImage()).Scale(12,12)));
    bitmaps.Add(wxBitmap(wxImage(mondrian.ConvertToImage()).Scale(14,14)));
    bitmaps.Add(wxBitmap(wxImage(mondrian.ConvertToImage()).Scale(16,16)));
    bitmaps.Add(wxBitmap(wxImage(mondrian.ConvertToImage()).Scale(18,18)));
    bitmaps.Add(wxBitmap(wxImage(mondrian.ConvertToImage()).Scale(20,20)));
    bitmaps.Add(wxBitmap(wxImage(mondrian.ConvertToImage()).Scale(24,24)));
    bitmaps.Add(mondrian);

    wxArrayString labels;
    labels.Add(wxT("Hello"));
    labels.Add(wxT("Hello 1"));
    labels.Add(wxT("H"));
    labels.Add(wxT("Hello 3"));
    labels.Add(wxT("Hello 4 "));
    labels.Add(wxT("Hello 5"));
    labels.Add(wxT("Hello 7 "));
    labels.Add(wxT("Hello     9"));
    labels.Add(wxT("Hello 10asdasda"));

    wxBitmapComboBox *bmpcombo = new wxBitmapComboBox(panel, ID++, labels, bitmaps);
    item0->Add( bmpcombo, 0, wxALIGN_CENTRE|wxALL, 5 );

    bitmaps.Clear();
    bitmaps.Add(wxBitmap(wxImage(mondrian.ConvertToImage()).Scale(16,16)), 9);
    bmpcombo = new wxBitmapComboBox(panel, ID++, labels, bitmaps);
    item0->Add( bmpcombo, 0, wxALIGN_CENTRE|wxALL, 5 );
*/

    wxString labels[9] = { wxT("Hello"),
                           wxT("Hello 1"),
                           wxT("H"),
                           wxT("Hello 3"),
                           wxT("Hello 4 "),
                           wxT("Hello 5"),
                           wxT("Hello 7 "),
                           wxT("Hello     9"),
                           wxT("Hello 10asdasda") };

    wxBitmapComboBox *bmpcombo1 = new wxBitmapComboBox(panel, ID++, wxEmptyString,
                                 wxDefaultPosition, wxDefaultSize, 9, labels);

    wxBitmapComboBox *bmpcombo2 = new wxBitmapComboBox(panel, ID++, wxEmptyString,
                                 wxDefaultPosition, wxDefaultSize, 9, labels);

    // freeze while adding items
    bmpcombo1->Freeze();
    bmpcombo2->Freeze();

    wxImage mondrianImage(mondrian.ConvertToImage());
    for (int n=0; n<9; n++)
    {
        bmpcombo1->SetBitmap(n, wxBitmap(mondrianImage.Scale(n*3+8,n*3+8)));
        bmpcombo2->SetBitmap(n, wxBitmap(mondrianImage.Scale(16,16)));
    }
    // after adding items use Thaw to CalcLayout
    bmpcombo1->Thaw();
    bmpcombo2->Thaw();
    bmpcombo1->SetSize(bmpcombo1->GetBestSize());
    bmpcombo2->SetSize(bmpcombo2->GetBestSize());

    item0->Add( bmpcombo1, 0, wxALIGN_CENTRE|wxALL, 5 );
    item0->Add( bmpcombo2, 0, wxALIGN_CENTRE|wxALL, 5 );

    panel->SetAutoLayout(true);
    panel->SetSizer(item0);
    return panel;
}

void MyFrame::OnSpin(wxSpinEvent &event)
{
    m_textCtrl->AppendText(wxString::Format(wxT("%s Spin id %d, val %ld\n"), wxNow().c_str(), event.GetId(), event.GetInt()));
}
void MyFrame::OnTextEnter(wxCommandEvent& event)
{
    if (m_textCtrl->GetId() != event.GetId())
        m_textCtrl->AppendText(wxString::Format(wxT("%s TextEnter id %d, val '%s'\n"), wxNow().c_str(), event.GetId(), event.GetString().c_str()));
}
void MyFrame::OnText(wxCommandEvent& event)
{
    if (m_textCtrl->GetId() != event.GetId())
        m_textCtrl->AppendText(wxString::Format(wxT("%s TextUpdated id %d, val '%s'\n"), wxNow().c_str(), event.GetId(), event.GetString().c_str()));
}
void MyFrame::OnToggle(wxCommandEvent &event)
{
    m_textCtrl->AppendText(wxString::Format(wxT("%s Toggle id %d, val %ld\n"), wxNow().c_str(), event.GetId(), event.GetInt()));
}
void MyFrame::OnButton(wxCommandEvent &event)
{
    m_textCtrl->AppendText(wxString::Format(wxT("%s Button id %d, val %ld\n"), wxNow().c_str(), event.GetId(), event.GetInt()));
}
void MyFrame::OnMenu(wxCommandEvent &event)
{
    m_textCtrl->AppendText(wxString::Format(wxT("%s Menu id %d, val %ld\n"), wxNow().c_str(), event.GetId(), event.GetInt()));
}
void MyFrame::OnComboBox(wxCommandEvent &event)
{
    m_textCtrl->AppendText(wxString::Format(wxT("%s ComboBox id %d, val %ld\n"), wxNow().c_str(), event.GetId(), event.GetInt()));
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("This is the About dialog of the thingsdemo sample.\n")
                _T("Welcome to %s"), wxVERSION_STRING);

    wxMessageBox(msg, _T("About Minimal"), wxOK | wxICON_INFORMATION, this);
}

