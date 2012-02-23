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
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/buffer.h>
#include <wx/image.h>
#include <wx/splitter.h>
#include <wx/tglbtn.h>
#include <wx/notebook.h>
#include "wx/things/toggle.h"
#include "wx/things/spinctld.h"
#include "wx/things/menubtn.h"
#include "wx/things/bmpcombo.h"
#include "wx/things/filebrws.h"
#include "wx/things/matrix2d.h"
#include "wx/things/range.h"

/* XPM */
static const char *mondrian_xpm[] = {
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
    ID_FILEBROWSER = wxID_HIGHEST + 1,
    ID_TEST_MATRIX,
    ID_TEST_RANGE
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

    bool CheckControlId(int win_id) const;

    void OnTestMatrix(wxCommandEvent& event);
    void OnTestRange(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& WXUNUSED(event)) { Close(true); }
    void OnAbout(wxCommandEvent& event);

    void OnSpin(wxSpinEvent &event);
    void OnTextEnter(wxCommandEvent &event);
    void OnText(wxCommandEvent &event);
    void OnToggle(wxCommandEvent &event);
    void OnButton(wxCommandEvent &event);
    void OnMenu(wxCommandEvent &event);
    void OnComboBox(wxCommandEvent &event);
    void OnFileBrowser(wxFileBrowserEvent &event);

    wxNotebook       *m_notebook;
    wxTextCtrl       *m_textCtrl;
    wxScrolledWindow *m_buttonPage;

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
        MyFrame *frame = new MyFrame(_T("wxThings Demo App"),
                                     wxPoint(50, 50), wxSize(450, 400));
        frame->Show(true);

        return true;
    }
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU (ID_TEST_MATRIX, MyFrame::OnTestMatrix)
    EVT_MENU (ID_TEST_RANGE,  MyFrame::OnTestRange)

    EVT_MENU (wxID_EXIT,      MyFrame::OnQuit)
    EVT_MENU (wxID_ABOUT,     MyFrame::OnAbout)

    EVT_SPINCTRL     (wxID_ANY, MyFrame::OnSpin)
    EVT_TEXT_ENTER   (wxID_ANY, MyFrame::OnTextEnter)
    EVT_TEXT         (wxID_ANY, MyFrame::OnText)
    EVT_TOGGLEBUTTON (wxID_ANY, MyFrame::OnToggle)
    EVT_BUTTON       (wxID_ANY, MyFrame::OnButton)
    EVT_MENU         (wxID_ANY, MyFrame::OnMenu)
    EVT_COMBOBOX     (wxID_ANY, MyFrame::OnComboBox)

    EVT_FILEBROWSER_FILE_SELECTED  (wxID_ANY, MyFrame::OnFileBrowser)
    EVT_FILEBROWSER_FILE_ACTIVATED (wxID_ANY, MyFrame::OnFileBrowser)
    EVT_FILEBROWSER_DIR_SELECTED   (wxID_ANY, MyFrame::OnFileBrowser)
    EVT_FILEBROWSER_DIR_ACTIVATED  (wxID_ANY, MyFrame::OnFileBrowser)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style)
        :wxFrame(NULL, wxID_ANY, title, pos, size, style)
{
    m_notebook   = NULL;
    m_textCtrl   = NULL;
    m_buttonPage = NULL;

    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append(ID_TEST_MATRIX, _T("Test wx&Matrix2D"), _T("Test wxMatrix2D routines"));
    fileMenu->Append(ID_TEST_RANGE,  _T("Test wx&RangeInt/Double"), _T("Test wxRangeInt/Double routines"));
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, _T("E&xit\tAlt-X"), _T("Quit this program"));

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT, _T("&About...\tF1"), _T("Show about dialog"));

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    SetMenuBar(menuBar);

    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxWidgets!"));

    wxInitAllImageHandlers();

    wxSplitterWindow *splitter = new wxSplitterWindow(this, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D);
    splitter->SetMinimumPaneSize(20);

    m_notebook = new wxNotebook(splitter, wxID_ANY);
    m_textCtrl = new wxTextCtrl( splitter, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxVSCROLL|wxHSCROLL );

    m_buttonPage = CreateButtonPage(m_notebook);
    m_notebook->AddPage(m_buttonPage, wxT("Controls"));

    wxFileBrowser* fileBrowser = new wxFileBrowser(m_notebook, ID_FILEBROWSER);
    m_notebook->AddPage(fileBrowser, wxT("wxFileBrowser"));

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
    item0->Add( cbutton, 0, wxALIGN_CENTRE|wxALL, 5 );

    cbutton = new wxCustomButton( panel, ID++, wxT("Right"), mondrian, wxDefaultPosition, wxDefaultSize, wxCUSTBUT_TOGGLE|wxCUSTBUT_RIGHT );
    cbutton->SetMargins(wxSize(20,20), true);
    item0->Add( cbutton, 0, wxALIGN_CENTRE|wxALL, 5 );

    cbutton = new wxCustomButton( panel, ID++, wxT("Top Disabled"), mondrian, wxDefaultPosition, wxDefaultSize, wxCUSTBUT_BUT_DCLICK_TOG|wxCUSTBUT_TOP );
    cbutton->SetMargins(wxSize(20,20), true);
    cbutton->Enable(false);
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

    wxBmpComboBox *bmpcombo = new wxBmpComboBox(panel, ID++, labels, bitmaps);
    item0->Add( bmpcombo, 0, wxALIGN_CENTRE|wxALL, 5 );

    bitmaps.Clear();
    bitmaps.Add(wxBitmap(wxImage(mondrian.ConvertToImage()).Scale(16,16)), 9);
    bmpcombo = new wxBmpComboBox(panel, ID++, labels, bitmaps);
    item0->Add( bmpcombo, 0, wxALIGN_CENTRE|wxALL, 5 );
*/

#define BMPCOMBO_LABEL_COUNT 9

    wxString labels[BMPCOMBO_LABEL_COUNT] = {
        wxT("Hello"),
        wxT("Hello 1"),
        wxT("H"),
        wxT("Hello 3"),
        wxT("Hello 4 "),
        wxT("Hello 5"),
        wxT("Hello 7 "),
        wxT("Hello     9"),
        wxT("Hello 10asdasda") };

    wxBmpComboBox *bmpcombo1 = new wxBmpComboBox(panel, ID++,
                                    wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                    BMPCOMBO_LABEL_COUNT, labels);

    wxBmpComboBox *bmpcombo2 = new wxBmpComboBox(panel, ID++,
                                    wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                    BMPCOMBO_LABEL_COUNT, labels);

    // freeze while adding items
    bmpcombo1->Freeze();
    bmpcombo2->Freeze();

    wxImage mondrianImage(mondrian.ConvertToImage());
    for (int n = 0; n < BMPCOMBO_LABEL_COUNT; n++)
    {
        bmpcombo1->SetItemBitmap(n, wxBitmap(mondrianImage.Scale(n*3+8,n*3+8)));
        bmpcombo2->SetItemBitmap(n, wxBitmap(mondrianImage.Scale(16,16)));
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

// We only want to print events from the controls
bool MyFrame::CheckControlId(int win_id) const
{
    return m_buttonPage && (m_buttonPage->FindWindow(win_id) != NULL);
}

void MyFrame::OnSpin(wxSpinEvent &event)
{
    if (CheckControlId(event.GetId()))
        m_textCtrl->AppendText(wxString::Format(wxT("%s EVT_SPINCTRL(id %d) val %d\n"), wxNow().c_str(), event.GetId(), event.GetInt()));
}
void MyFrame::OnTextEnter(wxCommandEvent& event)
{
    if (CheckControlId(event.GetId()))
        m_textCtrl->AppendText(wxString::Format(wxT("%s EVT_TEXT_ENTER(id %d) val '%s'\n"), wxNow().c_str(), event.GetId(), event.GetString().c_str()));
}
void MyFrame::OnText(wxCommandEvent& event)
{
    if (CheckControlId(event.GetId()))
        m_textCtrl->AppendText(wxString::Format(wxT("%s EVT_TEXT(id %d) val '%s'\n"), wxNow().c_str(), event.GetId(), event.GetString().c_str()));
}
void MyFrame::OnToggle(wxCommandEvent &event)
{
    if (CheckControlId(event.GetId()))
        m_textCtrl->AppendText(wxString::Format(wxT("%s EVT_TOGGLEBUTTON(id %d) val %d\n"), wxNow().c_str(), event.GetId(), event.GetInt()));
}
void MyFrame::OnButton(wxCommandEvent &event)
{
    if (CheckControlId(event.GetId()))
        m_textCtrl->AppendText(wxString::Format(wxT("%s EVT_BUTTON(id %d) val %d\n"), wxNow().c_str(), event.GetId(), event.GetInt()));
}
void MyFrame::OnMenu(wxCommandEvent &event)
{
    //if (CheckControlId(event.GetId()))
        m_textCtrl->AppendText(wxString::Format(wxT("%s EVT_MENU(id %d) val %d\n"), wxNow().c_str(), event.GetId(), event.GetInt()));
}
void MyFrame::OnComboBox(wxCommandEvent &event)
{
    if (CheckControlId(event.GetId()))
        m_textCtrl->AppendText(wxString::Format(wxT("%s EVT_COMBOBOX(id %d) val %d\n"), wxNow().c_str(), event.GetId(), event.GetInt()));
}

void MyFrame::OnFileBrowser(wxFileBrowserEvent &event)
{
    wxEventType evtType = event.GetEventType();
    wxString evtTypeStr, msg(wxNow());

    if (evtType == wxEVT_FILEBROWSER_FILE_SELECTED)
        evtTypeStr = wxT("wxEVT_FILEBROWSER_FILE_SELECTED");
    else if (evtType == wxEVT_FILEBROWSER_FILE_ACTIVATED)
        evtTypeStr = wxT("wxEVT_FILEBROWSER_FILE_ACTIVATED");
    else if (evtType == wxEVT_FILEBROWSER_DIR_SELECTED)
        evtTypeStr = wxT("wxEVT_FILEBROWSER_DIR_SELECTED");
    else if (evtType == wxEVT_FILEBROWSER_DIR_ACTIVATED)
        evtTypeStr = wxT("wxEVT_FILEBROWSER_DIR_ACTIVATED");
    else
        evtTypeStr = wxT("UNKNOWN EVENT TYPE!");

    m_textCtrl->AppendText(wxString::Format(wxT("%s %s(id %d) val %d str '%s'\n"), wxNow().c_str(), evtTypeStr.c_str(), event.GetId(), event.GetInt(), event.GetFilePath().c_str()));
}

// ============================================================================
#define wxSF wxString::Format

int bad_matrix_tests = 0;

void DisplayMatrix(wxTextCtrl* textCtrl, const wxthings::wxMatrix2D& matrix, const wxString& prepend, const wxString& test)
{
    wxString t(wxT(" : No test\n"));
    if (!test.IsEmpty())
    {
        int res = matrix.ToString() == test;
        if (res == 0) bad_matrix_tests++;
        t = wxString::Format(wxT(" : Test Ok %d\n"), res);
    }

    textCtrl->AppendText(prepend + t + matrix.ToString(wxT("\t")) + wxT("\n\n"));

    // print the comparison string for test
    //wxString s(matrix.ToString()); s.Replace(wxT("\n"), wxT("\\n"), true);
    //textCtrl->AppendText(wxT("wxT(\"") + s + wxT("\")\n\n"));
}

void MyFrame::OnTestMatrix(wxCommandEvent& WXUNUSED(event))
{
    bad_matrix_tests = 0;

    wxDialog dlg(this, wxID_ANY, wxT("Test wxMatrx2D"),
                 wxDefaultPosition, wxSize(600, 400), wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
    wxTextCtrl* textCtrl = new wxTextCtrl(&dlg, wxID_ANY, wxEmptyString,
                                          wxDefaultPosition, wxDefaultSize,
                                          wxTE_MULTILINE);
    textCtrl->SetSize(dlg.GetClientSize());

    double test_data[] = { 1, 2, 3, 4,  5, 6, 7, 8 };
    wxthings::wxMatrix2D matrix4x2(4, 2, test_data);
    DisplayMatrix(textCtrl, matrix4x2, wxT("Initial matrix4x2 = [1 2 3 4; 5 6 7 8]"), wxT("1 2 3 4\n5 6 7 8"));

    DisplayMatrix(textCtrl, matrix4x2.Transpose(), wxT("matrix4x2.Transpose()"), wxT("1 5\n2 6\n3 7\n4 8"));

    DisplayMatrix(textCtrl, matrix4x2.Rotate90(1), wxT("matrix4x2.Rotate90(1) clockwise"), wxT("5 1\n6 2\n7 3\n8 4"));
    DisplayMatrix(textCtrl, matrix4x2.Rotate90(2), wxT("matrix4x2.Rotate90(2)"), wxT("8 7 6 5\n4 3 2 1"));
    DisplayMatrix(textCtrl, matrix4x2.Rotate90(3), wxT("matrix4x2.Rotate90(3)"), wxT("4 8\n3 7\n2 6\n1 5"));

    DisplayMatrix(textCtrl, matrix4x2.Mirror(true), wxT("matrix4x2.Mirror(true) horiz"), wxT("4 3 2 1\n8 7 6 5"));
    DisplayMatrix(textCtrl, matrix4x2.Mirror(false), wxT("matrix4x2.Mirror(false) vert"), wxT("5 6 7 8\n1 2 3 4"));

    DisplayMatrix(textCtrl, matrix4x2.SubMatrix(wxRect(0, 0, 4, 2)), wxT("matrix4x2.SubRect(0, 0, 4, 2)"), wxT("1 2 3 4\n5 6 7 8"));
    DisplayMatrix(textCtrl, matrix4x2.SubMatrix(wxRect(1, 0, 2, 2)), wxT("matrix4x2.SubRect(1, 0, 2, 2)"), wxT("2 3\n6 7"));

    DisplayMatrix(textCtrl, matrix4x2.SubRows(0, 1), wxT("matrix4x2.SubRows(0, 1)"), wxT("1 2 3 4\n5 6 7 8"));
    DisplayMatrix(textCtrl, matrix4x2.SubRows(1, 1), wxT("matrix4x2.SubRows(1, 1)"), wxT("5 6 7 8"));

    DisplayMatrix(textCtrl, matrix4x2.SubCols(0, 3), wxT("matrix4x2.SubCols(0, 3)"), wxT("1 2 3 4\n5 6 7 8"));
    DisplayMatrix(textCtrl, matrix4x2.SubCols(1, 1), wxT("matrix4x2.SubCols(1, 1)"), wxT("2\n6"));
    DisplayMatrix(textCtrl, matrix4x2.SubCols(2, 3), wxT("matrix4x2.SubCols(2, 3)"), wxT("3 4\n7 8"));

    wxthings::wxMatrix2D matrix2x4(matrix4x2, true); matrix2x4.Reshape(2, 4);
    DisplayMatrix(textCtrl, matrix2x4, wxT("matrix4x2.Reshape(2, 4)"), wxT("1 2\n3 4\n5 6\n7 8"));

    wxthings::wxMatrix2D matrix4x2neg(matrix4x2.Mult(-1), true);
    DisplayMatrix(textCtrl, matrix4x2.AppendRows(matrix4x2neg), wxT("matrix4x2.AppendRows(matrix4x2neg) (insert at -1)"), wxT("1 2 3 4\n5 6 7 8\n-1 -2 -3 -4\n-5 -6 -7 -8"));
    DisplayMatrix(textCtrl, matrix4x2.InsertRows(0, matrix4x2neg), wxT("matrix4x2.InsertRows(0, matrix4x2neg)"), wxT("-1 -2 -3 -4\n-5 -6 -7 -8\n1 2 3 4\n5 6 7 8"));
    DisplayMatrix(textCtrl, matrix4x2.InsertRows(1, matrix4x2neg), wxT("matrix4x2.InsertRows(1, matrix4x2neg)"), wxT("1 2 3 4\n-1 -2 -3 -4\n-5 -6 -7 -8\n5 6 7 8"));
    DisplayMatrix(textCtrl, matrix4x2.InsertRows(2, matrix4x2neg), wxT("matrix4x2.InsertRows(2, matrix4x2neg)"), wxT("1 2 3 4\n5 6 7 8\n-1 -2 -3 -4\n-5 -6 -7 -8"));

    DisplayMatrix(textCtrl, matrix4x2.AppendCols(matrix4x2neg), wxT("matrix4x2.AppendCols(matrix4x2neg) (insert at -1)"), wxT("1 2 3 4 -1 -2 -3 -4\n5 6 7 8 -5 -6 -7 -8"));
    DisplayMatrix(textCtrl, matrix4x2.InsertCols(0, matrix4x2neg), wxT("matrix4x2.InsertCols(0, matrix4x2neg)"), wxT("-1 -2 -3 -4 1 2 3 4\n-5 -6 -7 -8 5 6 7 8"));
    DisplayMatrix(textCtrl, matrix4x2.InsertCols(1, matrix4x2neg), wxT("matrix4x2.InsertCols(1, matrix4x2neg)"), wxT("1 -1 -2 -3 -4 2 3 4\n5 -5 -6 -7 -8 6 7 8"));
    DisplayMatrix(textCtrl, matrix4x2.InsertCols(2, matrix4x2neg), wxT("matrix4x2.InsertCols(2, matrix4x2neg)"), wxT("1 2 -1 -2 -3 -4 3 4\n5 6 -5 -6 -7 -8 7 8"));

    DisplayMatrix(textCtrl, matrix4x2.Add(matrix4x2), wxT("matrix4x2.Add(matrix4x2)"), wxT("2 4 6 8\n10 12 14 16"));
    DisplayMatrix(textCtrl, matrix4x2.Mult(matrix4x2.Transpose()), wxT("matrix4x2.Mult(matrix4x2.Transpose())"), wxT("30 70\n70 174"));
    DisplayMatrix(textCtrl, matrix4x2.MultElement(matrix4x2), wxT("matrix4x2.MultElement(matrix4x2)"), wxT("1 4 9 16\n25 36 49 64"));
    DisplayMatrix(textCtrl, matrix4x2.Pow(2), wxT("matrix4x2.Pow(2)"), wxT("1 4 9 16\n25 36 49 64"));

    double square3[] = { 1, 2, 3,  4, 5, 6,  7, 8, 9 };
    wxthings::wxMatrix2D matrix3x3(3, 3, square3);
    DisplayMatrix(textCtrl, matrix3x3, wxT("Initial square matrix3x3"), wxT("1 2 3\n4 5 6\n7 8 9"));
    DisplayMatrix(textCtrl, matrix3x3.RotateSquare45(true), wxT("matrix3x3.RotateSquare45(true)"), wxT("4 1 2\n7 5 3\n8 9 6"));
    DisplayMatrix(textCtrl, matrix3x3.RotateSquare45(false), wxT("matrix3x3.RotateSquare45(false)"), wxT("2 3 6\n1 5 9\n4 7 8"));

    double square5[] = { 1, 2, 3, 4, 5,  6, 7, 8, 9, 10,  11, 12, 13, 14, 15,  16, 17, 18, 19, 20,  21, 22, 23, 24, 25 };
    wxthings::wxMatrix2D matrix5x5(5, 5, square5);
    DisplayMatrix(textCtrl, matrix5x5, wxT("Initial square matrix5x5"), wxT("1 2 3 4 5\n6 7 8 9 10\n11 12 13 14 15\n16 17 18 19 20\n21 22 23 24 25"));
    DisplayMatrix(textCtrl, matrix5x5.RotateSquare45(true), wxT("matrix5x5.RotateSquare45(true)"), wxT("11 6 1 2 3\n16 12 7 8 4\n21 17 13 9 5\n22 18 19 14 10\n23 24 25 20 15"));
    DisplayMatrix(textCtrl, matrix5x5.RotateSquare45(false), wxT("matrix5x5.RotateSquare45(false)"), wxT("3 4 5 10 15\n2 8 9 14 20\n1 7 13 19 25\n6 12 17 18 24\n11 16 21 22 23"));

    wxthings::wxMatrix2D identMatrix5x5; identMatrix5x5.CreateIdentity(5);
    DisplayMatrix(textCtrl, matrix5x5.Mult(identMatrix5x5), wxT("matrix5x5.Mult(identMatrix5x5 from CreateIdentity(5))"), wxT("1 2 3 4 5\n6 7 8 9 10\n11 12 13 14 15\n16 17 18 19 20\n21 22 23 24 25"));

    textCtrl->AppendText(wxString::Format(wxT("\n\nTests that Failed : %d\n"), bad_matrix_tests));

    dlg.ShowModal();
}

int bad_range_tests = 0;

void DisplayRangeInt(wxTextCtrl* textCtrl, const wxRangeInt& range, const wxString& prepend, bool test)
{
    if (!test) bad_range_tests++;

    textCtrl->AppendText(prepend +
                         wxString::Format(wxT(" : Ok %d : (%d %d)=%d\n\n"),
                         test, range.GetMin(), range.GetMax(), range.GetRange()));
}

void MyFrame::OnTestRange(wxCommandEvent& WXUNUSED(event))
{
    bad_range_tests = 0;

    wxDialog dlg(this, wxID_ANY, wxT("Test wxRangeInt/Double"),
                 wxDefaultPosition, wxSize(600, 400), wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
    wxTextCtrl* textCtrl = new wxTextCtrl(&dlg, wxID_ANY, wxEmptyString,
                                          wxDefaultPosition, wxDefaultSize,
                                          wxTE_MULTILINE);
    textCtrl->SetSize(dlg.GetClientSize());

    int ans = 0;

    wxRangeInt range2_8(2, 8);
    DisplayRangeInt(textCtrl, range2_8, wxT("Initial wxRangeInt(2, 8)"), range2_8.GetRange() == 7);

    ans = range2_8.Position(1); DisplayRangeInt(textCtrl, range2_8, wxSF(wxT("range2_8.Position(1) = %d"), ans), ans == -1);
    ans = range2_8.Position(2); DisplayRangeInt(textCtrl, range2_8, wxSF(wxT("range2_8.Position(2) = %d"), ans), ans == 0);
    ans = range2_8.Position(8); DisplayRangeInt(textCtrl, range2_8, wxSF(wxT("range2_8.Position(8) = %d"), ans), ans == 0);
    ans = range2_8.Position(9); DisplayRangeInt(textCtrl, range2_8, wxSF(wxT("range2_8.Position(9) = %d"), ans), ans == 1);

    ans = range2_8.Contains(wxRangeInt(0,1));   DisplayRangeInt(textCtrl, range2_8, wxSF(wxT("range2_8.Contains(wxRangeInt(0,1)) = %d"), ans), ans == 0);
    ans = range2_8.Contains(wxRangeInt(1,2));   DisplayRangeInt(textCtrl, range2_8, wxSF(wxT("range2_8.Contains(wxRangeInt(1,2)) = %d"), ans), ans == 0);
    ans = range2_8.Contains(wxRangeInt(3,-2));  DisplayRangeInt(textCtrl, range2_8, wxSF(wxT("range2_8.Contains(wxRangeInt(3,-2)) = %d"), ans), ans == 0);
    ans = range2_8.Contains(wxRangeInt(12,-2)); DisplayRangeInt(textCtrl, range2_8, wxSF(wxT("range2_8.Contains(wxRangeInt(12,-2)) = %d"), ans), ans == 0);
    ans = range2_8.Contains(wxRangeInt(3,4));   DisplayRangeInt(textCtrl, range2_8, wxSF(wxT("range2_8.Contains(wxRangeInt(3,4)) = %d"), ans), ans != 0);
    ans = range2_8.Contains(wxRangeInt(2,8));   DisplayRangeInt(textCtrl, range2_8, wxSF(wxT("range2_8.Contains(wxRangeInt(2,8)) = %d"), ans), ans != 0);
    ans = range2_8.Contains(wxRangeInt(8,9));   DisplayRangeInt(textCtrl, range2_8, wxSF(wxT("range2_8.Contains(wxRangeInt(8,9)) = %d"), ans), ans == 0);
    ans = range2_8.Contains(wxRangeInt(1,9));   DisplayRangeInt(textCtrl, range2_8, wxSF(wxT("range2_8.Contains(wxRangeInt(1,9)) = %d"), ans), ans == 0);

    wxRangeInt range2_8_i;
    range2_8_i = range2_8.Intersect(wxRangeInt(0,1));   DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Intersect(wxRangeInt(0,1))")), range2_8_i.IsEmpty());
    range2_8_i = range2_8.Intersect(wxRangeInt(1,2));   DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Intersect(wxRangeInt(1,2))")), range2_8_i == wxRangeInt(2,2));
    range2_8_i = range2_8.Intersect(wxRangeInt(3,-2));  DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Intersect(wxRangeInt(3,-2))")), range2_8_i.IsEmpty());
    range2_8_i = range2_8.Intersect(wxRangeInt(12,-2)); DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Intersect(wxRangeInt(12,-2))")), range2_8_i.IsEmpty());
    range2_8_i = range2_8.Intersect(wxRangeInt(3,4));   DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Intersect(wxRangeInt(3,4))")), range2_8_i == wxRangeInt(3,4));
    range2_8_i = range2_8.Intersect(wxRangeInt(2,8));   DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Intersect(wxRangeInt(2,8))")), range2_8_i == wxRangeInt(2,8));
    range2_8_i = range2_8.Intersect(wxRangeInt(8,9));   DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Intersect(wxRangeInt(8,9))")), range2_8_i == wxRangeInt(8,8));
    range2_8_i = range2_8.Intersect(wxRangeInt(1,9));   DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Intersect(wxRangeInt(1,9))")), range2_8_i == wxRangeInt(2,8));

    range2_8_i = range2_8.Union(wxRangeInt(0,1));   DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Union(wxRangeInt(0,1))")), range2_8_i == wxRangeInt(0,8));
    range2_8_i = range2_8.Union(wxRangeInt(1,2));   DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Union(wxRangeInt(1,2))")), range2_8_i == wxRangeInt(1,8));
    range2_8_i = range2_8.Union(wxRangeInt(3,-2));  DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Union(wxRangeInt(3,-2))")), range2_8_i.IsEmpty());
    range2_8_i = range2_8.Union(wxRangeInt(12,-2)); DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Union(wxRangeInt(12,-2))")), range2_8_i.IsEmpty());
    range2_8_i = range2_8.Union(wxRangeInt(3,4));   DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Union(wxRangeInt(3,4))")), range2_8_i == wxRangeInt(2,8));
    range2_8_i = range2_8.Union(wxRangeInt(2,8));   DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Union(wxRangeInt(2,8))")), range2_8_i == wxRangeInt(2,8));
    range2_8_i = range2_8.Union(wxRangeInt(8,9));   DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Union(wxRangeInt(8,9))")), range2_8_i == wxRangeInt(2,9));
    range2_8_i = range2_8.Union(wxRangeInt(1,9));   DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Union(wxRangeInt(1,9))")), range2_8_i == wxRangeInt(1,9));

    ans = range2_8.Touches(wxRangeInt(0,1));   DisplayRangeInt(textCtrl, range2_8, wxSF(wxT("range2_8.Touches(wxRangeInt(0,1)) = %d"), ans), ans != 0);
    ans = range2_8.Touches(wxRangeInt(1,2));   DisplayRangeInt(textCtrl, range2_8, wxSF(wxT("range2_8.Touches(wxRangeInt(1,2)) = %d"), ans), ans != 0);
    ans = range2_8.Touches(wxRangeInt(3,-2));  DisplayRangeInt(textCtrl, range2_8, wxSF(wxT("range2_8.Touches(wxRangeInt(3,-2)) = %d"), ans), ans == 0);
    ans = range2_8.Touches(wxRangeInt(12,-2)); DisplayRangeInt(textCtrl, range2_8, wxSF(wxT("range2_8.Touches(wxRangeInt(12,-2)) = %d"), ans), ans == 0);
    ans = range2_8.Touches(wxRangeInt(3,4));   DisplayRangeInt(textCtrl, range2_8, wxSF(wxT("range2_8.Touches(wxRangeInt(3,4)) = %d"), ans), ans != 0);
    ans = range2_8.Touches(wxRangeInt(2,8));   DisplayRangeInt(textCtrl, range2_8, wxSF(wxT("range2_8.Touches(wxRangeInt(2,8)) = %d"), ans), ans != 0);
    ans = range2_8.Touches(wxRangeInt(8,9));   DisplayRangeInt(textCtrl, range2_8, wxSF(wxT("range2_8.Touches(wxRangeInt(8,9)) = %d"), ans), ans != 0);
    ans = range2_8.Touches(wxRangeInt(1,9));   DisplayRangeInt(textCtrl, range2_8, wxSF(wxT("range2_8.Touches(wxRangeInt(1,9)) = %d"), ans), ans != 0);

    range2_8_i.Set(2,8); ans = range2_8_i.Combine(wxRangeInt(0,1), false);   DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Combine(wxRangeInt(0,1), false) = %d"), ans), range2_8_i == wxRangeInt(0,8));
    range2_8_i.Set(2,8); ans = range2_8_i.Combine(wxRangeInt(1,2), false);   DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Combine(wxRangeInt(1,2), false) = %d"), ans), range2_8_i == wxRangeInt(1,8));
    range2_8_i.Set(2,8); ans = range2_8_i.Combine(wxRangeInt(3,-2), false);  DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Combine(wxRangeInt(3,-2), false) = %d"), ans), range2_8_i == wxRangeInt(2,8));
    range2_8_i.Set(2,8); ans = range2_8_i.Combine(wxRangeInt(12,-2), false); DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Combine(wxRangeInt(12,-2), false) = %d"), ans), range2_8_i == wxRangeInt(2,8));
    range2_8_i.Set(2,8); ans = range2_8_i.Combine(wxRangeInt(3,4), false);   DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Combine(wxRangeInt(3,4), false) = %d"), ans), range2_8_i == wxRangeInt(2,8));
    range2_8_i.Set(2,8); ans = range2_8_i.Combine(wxRangeInt(2,8), false);   DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Combine(wxRangeInt(2,8), false) = %d"), ans), range2_8_i == wxRangeInt(2,8));
    range2_8_i.Set(2,8); ans = range2_8_i.Combine(wxRangeInt(8,9), false);   DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Combine(wxRangeInt(8,9), false) = %d"), ans), range2_8_i == wxRangeInt(2,9));
    range2_8_i.Set(2,8); ans = range2_8_i.Combine(wxRangeInt(1,9), false);   DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Combine(wxRangeInt(1,9), false) = %d"), ans), range2_8_i == wxRangeInt(1,9));

    range2_8_i.Set(2,8); ans = range2_8_i.Combine(wxRangeInt(0,0), true);   DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Combine(wxRangeInt(0,0), true) = %d"), ans), range2_8_i == wxRangeInt(2,8));
    range2_8_i.Set(2,8); ans = range2_8_i.Combine(wxRangeInt(0,1), true);   DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Combine(wxRangeInt(0,1), true) = %d"), ans), range2_8_i == wxRangeInt(0,8));
    range2_8_i.Set(2,8); ans = range2_8_i.Combine(wxRangeInt(1,2), true);   DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Combine(wxRangeInt(1,2), true) = %d"), ans), range2_8_i == wxRangeInt(1,8));
    range2_8_i.Set(2,8); ans = range2_8_i.Combine(wxRangeInt(3,-2), true);  DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Combine(wxRangeInt(3,-2), true) = %d"), ans), range2_8_i == wxRangeInt(2,8));
    range2_8_i.Set(2,8); ans = range2_8_i.Combine(wxRangeInt(12,-2), true); DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Combine(wxRangeInt(12,-2), true) = %d"), ans), range2_8_i == wxRangeInt(2,8));
    range2_8_i.Set(2,8); ans = range2_8_i.Combine(wxRangeInt(3,4), true);   DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Combine(wxRangeInt(3,4), true) = %d"), ans), range2_8_i == wxRangeInt(2,8));
    range2_8_i.Set(2,8); ans = range2_8_i.Combine(wxRangeInt(2,8), true);   DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Combine(wxRangeInt(2,8), true) = %d"), ans), range2_8_i == wxRangeInt(2,8));
    range2_8_i.Set(2,8); ans = range2_8_i.Combine(wxRangeInt(8,9), true);   DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Combine(wxRangeInt(8,9), true) = %d"), ans), range2_8_i == wxRangeInt(2,9));
    range2_8_i.Set(2,8); ans = range2_8_i.Combine(wxRangeInt(1,9), true);   DisplayRangeInt(textCtrl, range2_8_i, wxSF(wxT("range2_8.Combine(wxRangeInt(1,9), true) = %d"), ans), range2_8_i == wxRangeInt(1,9));

    textCtrl->AppendText(wxString::Format(wxT("\n\nTests that Failed : %d\n"), bad_range_tests));

    dlg.ShowModal();
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("This is the About dialog of the wxThingsDemo sample.\n")
                _T("Welcome to %s compiled with %s"), WXTHINGS_VERSION_STRING, wxVERSION_STRING);

    wxMessageBox(msg, _T("About wxThingsDemo"), wxOK | wxICON_INFORMATION, this);
}
