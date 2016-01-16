/////////////////////////////////////////////////////////////////////////////
// Name:        wxchart.cpp
// Purpose:     wxChart sample
// Author:      Paolo Gava
// Modified by:
// Created:
// Copyright:   (C) 2006, Paolo Gava
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "wxchart.cpp"
    #pragma interface "wxchart.cpp"
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/image.h>

#include "wx/points.h"
#include "wx/barchartpoints.h"
#include "wx/bar3dchartpoints.h"
#include "wx/chartctrl.h"
#include "wx/chartcolors.h"
#include "wx/piechartpoints.h"
#include "wx/pie3dchartpoints.h"
#include "wx/chart.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class MyPanel : public wxPanel
{
public:
    MyPanel(wxFrame *frame);

	//void ResizeChart();
	void WriteChart(wxChartCtrl *c);

    wxChartCtrl *GetChartL1() {return m_ChartCtrlL1;}
    wxChartCtrl *GetChartL2() {return m_ChartCtrlL2;}
    wxChartCtrl *GetChartR1() {return m_ChartCtrlR1;}
    wxChartCtrl *GetChartR2() {return m_ChartCtrlR2;}
	void AddPanelLeft1();
    void FitChart();

private:
	wxChartCtrl *m_ChartCtrlL1;
    wxChartCtrl *m_ChartCtrlL2;
    wxChartCtrl *m_ChartCtrlR1;
    wxChartCtrl *m_ChartCtrlR2;

    // Helper routines for creating Panels
    //------------------------------------
    void CreatePanelLeft1(wxBoxSizer *sizer);
    void CreatePanelLeft2(wxBoxSizer *sizer);
    void CreatePanelRight1(wxBoxSizer *sizer);
    void CreatePanelRight2(wxBoxSizer *sizer);
};


// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
};


// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    //void OnResize(wxCommandEvent& event);
    void OnWriteL1(wxCommandEvent& event);
    void OnWriteR1(wxCommandEvent& event);
    void OnWriteL2(wxCommandEvent& event);
    void OnWriteR2(wxCommandEvent& event);

private:
    MyPanel *m_panel;

    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    File_About = 100,
    File_Quit,
	//Options_Resize,
	Options_WriteL1,
    Options_WriteR1,
    Options_WriteL2,
    Options_WriteR2,

	Panel = 1000
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(File_Quit,  MyFrame::OnQuit)
    EVT_MENU(File_About, MyFrame::OnAbout)
    //EVT_MENU(Options_Resize, MyFrame::OnResize)
    EVT_MENU(Options_WriteL1, MyFrame::OnWriteL1)
    EVT_MENU(Options_WriteR1, MyFrame::OnWriteR1)
    EVT_MENU(Options_WriteL2, MyFrame::OnWriteL2)
    EVT_MENU(Options_WriteR2, MyFrame::OnWriteR2)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{

#if wxUSE_LIBPNG
  wxImage::AddHandler( new wxPNGHandler );
#endif
#if wxUSE_LIBJPEG
  wxImage::AddHandler( new wxJPEGHandler );
#endif
#if wxUSE_GIF
  wxImage::AddHandler( new wxGIFHandler );
#endif

	// Create the main application window
    MyFrame *frame = new MyFrame(_T("wxChart sample"),
                                 wxPoint(50, 50), wxSize(500, 400));

    frame->Show(TRUE);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.
    return TRUE;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    menuFile->Append(File_About, _T("&About...\tCtrl-A"), _T("Show about dialog"));
    menuFile->AppendSeparator();
    menuFile->Append(File_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    wxMenu *menuOptions = new wxMenu;
    //menuOptions->Append( Options_Resize, _T("&Resize"),
	//					 _T("Fit chart to actual window size"));
    menuOptions->Append( Options_WriteL1, _T("&Write Chart (L1) to File"),
						 _T("write chart to file"));
    menuOptions->Append( Options_WriteR1, _T("&Write Chart (R1) to File"),
                         _T("write chart to file"));
    menuOptions->Append( Options_WriteL2, _T("&Write Chart (L2) to File"),
                         _T("write chart to file"));
    menuOptions->Append( Options_WriteR2, _T("&Write Chart (R2) to File"),
                         _T("write chart to file"));


    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(menuOptions, _T("&Options"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    m_panel = new MyPanel(this);

}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	//m_panel->AddPanelLeft1();
    //m_panel->FitChart();

	wxString msg = wxString::Format(
        wxT("Chart for wxWidgets. \n 2006 Paolo Gava. Ver: %d.%d.%d"),
        wxCHART_MAJOR, wxCHART_MINOR, wxCHART_RELEASE );

	wxMessageBox( msg, _T("Chart"), wxOK | wxICON_INFORMATION, this );
}

//void MyFrame::OnResize(wxCommandEvent& WXUNUSED(event))
//{
//	m_panel->ResizeChart();
//}

void MyFrame::OnWriteL1(wxCommandEvent& WXUNUSED(event))
{
	m_panel->WriteChart( m_panel->GetChartL1() );
}

void MyFrame::OnWriteR1(wxCommandEvent& WXUNUSED(event))
{
    m_panel->WriteChart( m_panel->GetChartR1() );
}

void MyFrame::OnWriteL2(wxCommandEvent& WXUNUSED(event))
{
    m_panel->WriteChart( m_panel->GetChartL2() );
}

void MyFrame::OnWriteR2(wxCommandEvent& WXUNUSED(event))
{
    m_panel->WriteChart( m_panel->GetChartR2() );
}

// ----------------------------------------------------------------------------
// MyPanel
// ----------------------------------------------------------------------------
MyPanel::MyPanel(wxFrame *frame)
       : wxPanel(frame, -1)

{
    wxBoxSizer *VerSizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *Hor1Sizer = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *Hor2Sizer = new wxBoxSizer( wxHORIZONTAL );

    VerSizer->Add( Hor1Sizer, 1, wxEXPAND );
    VerSizer->Add( Hor2Sizer, 1, wxEXPAND );

    wxBoxSizer *Ver1Sizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *Ver2Sizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *Ver3Sizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *Ver4Sizer = new wxBoxSizer( wxVERTICAL );

    Hor1Sizer->Add( Ver1Sizer, 1, wxEXPAND );
    Hor1Sizer->Add( Ver2Sizer, 1, wxEXPAND );
    Hor2Sizer->Add( Ver3Sizer, 1, wxEXPAND );
    Hor2Sizer->Add( Ver4Sizer, 1, wxEXPAND );

    // Create Panel Left Row 1
    CreatePanelLeft1( Ver1Sizer );

    // Create Panel Right Row 1
    CreatePanelRight1(Ver2Sizer);

    // Create Panel Left Row2
    CreatePanelLeft2( Ver3Sizer );

    // Create Panel right Row2
    CreatePanelRight2(Ver4Sizer);

    SetAutoLayout( TRUE );
    SetSizer( VerSizer );
    VerSizer->Fit( this );
    SetAutoLayout(TRUE);

}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       CreatePanelLeft1()
//  DESC:       Create left Panel first row
//  PARAMETERS:
//      wxBoxSizer *sizer
//  RETURN:     None
//----------------------------------------------------------------------E-+++
void MyPanel::CreatePanelLeft1(wxBoxSizer *sizer)
{
    // Create Panel Left Row 1
    wxChartPoints *bcpLeft1, *bcpLeft2, *bcpLeft3, *bcpLeft4;

    bcpLeft1 = wxBar3DChartPoints::CreateWxBar3DChartPoints(
        wxT("Ita") );

    bcpLeft2 = wxBar3DChartPoints::CreateWxBar3DChartPoints(
        wxT("Fra") );

    bcpLeft3 = wxBar3DChartPoints::CreateWxBar3DChartPoints(
        wxT("Ger") );

    bcpLeft4 = wxBar3DChartPoints::CreateWxBar3DChartPoints(
        wxT("UK") );

    bcpLeft1->Add( wxT("val3"), 2, 3 );
    bcpLeft1->Add( wxT("val1"), 5, 1 );
    bcpLeft1->Add( wxT("val4"), 8, 4);
    bcpLeft1->Add( wxT("val2"), 15, 2 );
    bcpLeft1->Add( wxT("val5"), 20, 5 );
    bcpLeft1->SetDisplayTag( XVALUE );

    bcpLeft2->Add( wxT("val: 4"), 2, 4 );
    bcpLeft2->Add( wxT("val: 2"), 5, 2 );
    bcpLeft2->Add( wxT("val: 2"), 8, 2);
    bcpLeft2->Add( wxT("val: 6"), 15, 6 );
    bcpLeft2->Add( wxT("val: 2"), 20, 2 );
    bcpLeft2->SetDisplayTag( NAME );

    bcpLeft3->Add( wxT("val3"), 2, 1 );
    bcpLeft3->Add( wxT("val1"), 5, 3 );
    bcpLeft3->Add( wxT("val4"), 8, 4);
    bcpLeft3->Add( wxT("val2"), 15, 5 );
    bcpLeft3->Add( wxT("val5"), 20, 5 );
    bcpLeft3->SetDisplayTag( XVALUE );

    bcpLeft4->Add( wxT("val3"), 2, 3 );
    bcpLeft4->Add( wxT("val6"), 5, 6 );
    bcpLeft4->Add( wxT("val2"), 8, 2);
    bcpLeft4->Add( wxT("val2"), 15, 2 );
    bcpLeft4->Add( wxT("val4"), 20, 4 );
    bcpLeft4->SetDisplayTag( YVALUE );

    wxStaticText *lblTitle1 = new wxStaticText(this,
                                               wxID_ANY, wxT("Bar Chart (L1)"));
    m_ChartCtrlL1 = new wxChartCtrl( this, -1, DEFAULT_STYLE,
                                   wxDefaultPosition, wxSize(200,200),
                                   wxSUNKEN_BORDER  );

    m_ChartCtrlL1->Add( bcpLeft1 );
    m_ChartCtrlL1->Add( bcpLeft2 );
    m_ChartCtrlL1->Add( bcpLeft3 );
    m_ChartCtrlL1->Add( bcpLeft4 );

    sizer->Add( lblTitle1, 0, wxEXPAND );
    sizer->Add( m_ChartCtrlL1, 1, wxEXPAND );

}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       CreatePanelLeft1()
//  DESC:       Create left Panel first row
//  PARAMETERS:
//      wxBoxSizer *sizer
//  RETURN:     None
//----------------------------------------------------------------------E-+++
void MyPanel::AddPanelLeft1()
{
    // Create Panel Left Row 1
    wxChartPoints *bcpLeft1;

    bcpLeft1 = wxBar3DChartPoints::CreateWxBar3DChartPoints(
        wxT("Esp") );

    bcpLeft1->Add( wxT("val3"), 2, 3 );
    bcpLeft1->Add( wxT("val1"), 5, 1 );
    bcpLeft1->Add( wxT("val4"), 8, 4);
    bcpLeft1->Add( wxT("val2"), 15, 2 );
    bcpLeft1->Add( wxT("val5"), 20, 5 );
    bcpLeft1->SetDisplayTag( XVALUE );

    m_ChartCtrlL1->Add( bcpLeft1 );

	m_ChartCtrlL1->Refresh();
	//m_ChartCtrlL1->Resize();

}


//+++-S-cf-------------------------------------------------------------------
//  NAME:       CreatePanelLeft1()
//  DESC:       Create left Panel first row
//  PARAMETERS:
//      wxBoxSizer *sizer
//  RETURN:     None
//----------------------------------------------------------------------E-+++
void MyPanel::FitChart()
{
    m_ChartCtrlL2->Fit();
}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       CreatePanelLeft2()
//  DESC:       Create left Panel second row
//  PARAMETERS:
//      wxBoxSizer *sizer
//  RETURN:     None
//----------------------------------------------------------------------E-+++
void MyPanel::CreatePanelLeft2(wxBoxSizer *sizer)
{
    // Create Panel Left Row 2
    wxChartPoints *bcpLeft1;

    bcpLeft1 = wxBar3DChartPoints::CreateWxBar3DChartPoints(
		wxT("Ita"), wxCHART_NOCOLOR, true );

    bcpLeft1->Add( wxT("val3"), 2, 3.5 );
    bcpLeft1->Add( wxT("val1"), 5, 1.2 );
    bcpLeft1->Add( wxT("val4"), 8, 4.7);
    bcpLeft1->Add( wxT("val2"), 15, 2.3 );
    bcpLeft1->Add( wxT("val5"), 20, 5.0 );
    bcpLeft1->SetDisplayTag( YVALUE_FLOAT );

    wxStaticText *lblTitle3 = new wxStaticText(this,
                                               wxID_ANY, wxT("Bar Chart (L2)"));
    m_ChartCtrlL2 = new wxChartCtrl( this, -1,
                                   (wxChartStyle)(USE_AXIS_X | USE_ZOOM_BUT),
                                   wxDefaultPosition, wxSize(200,200),
                                   wxSUNKEN_BORDER  );

    m_ChartCtrlL2->Add( bcpLeft1 );

    sizer->Add( lblTitle3, 0, wxEXPAND );
    sizer->Add( m_ChartCtrlL2, 1, wxEXPAND );
}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       CreatePanelRight1()
//  DESC:       Create right Panel first row
//  PARAMETERS:
//      wxBoxSizer *sizer
//  RETURN:     None
//----------------------------------------------------------------------E-+++
void MyPanel::CreatePanelRight1(wxBoxSizer *sizer)
{
    // Create Panel Right Row 1
    wxChartPoints *pcpRight1;

    pcpRight1 = wxPie3DChartPoints::CreateWxPie3DChartPoints(
        wxT("Italy"), wxCHART_NOCOLOR, true );

    // Pie chart ignore x value
	pcpRight1->Add( wxT("10 %"), 0, 10, wxCHART_GOLD );
	pcpRight1->Add( wxT("20 %"), 0, 20, wxCHART_CYAN );
	pcpRight1->Add( wxT("5 %"), 0, 5, wxCHART_GOLDENROD );
	pcpRight1->Add( wxT("30 %"), 0, 30, wxCHART_GRAY );
	pcpRight1->Add( wxT("10 %"), 0, 10, wxCHART_AQUAMARINE );
	pcpRight1->Add( wxT("15 %"), 0, 15, wxCHART_LYELOW );
	pcpRight1->Add( wxT("10 %"), 0, 10, wxCHART_SEAGREEN );
    pcpRight1->SetDisplayTag( NAME );

    wxStaticText *lblTitle2 = new wxStaticText(this,
                                               wxID_ANY, wxT("Pie Chart (R1)"));
    m_ChartCtrlR1 = new wxChartCtrl( this, -1, USE_NONE,
                                    wxDefaultPosition, wxSize(200,200),
                                    wxSUNKEN_BORDER  );

    m_ChartCtrlR1->Add( pcpRight1 );

    sizer->Add( lblTitle2, 0, wxEXPAND );
    sizer->Add( m_ChartCtrlR1, 1, wxEXPAND );

}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       CreatePanelRight2()
//  DESC:       Create right Panel second row
//  PARAMETERS:
//      wxBoxSizer *sizer
//  RETURN:     None
//----------------------------------------------------------------------E-+++
void MyPanel::CreatePanelRight2(wxBoxSizer *sizer)
{
    wxChartPoints *bcpLeft1, *bcpLeft2, *bcpLeft3, *bcpLeft4, *pcpRight1;

    bcpLeft1 = wxBarChartPoints::CreateWxBarChartPoints(
        wxT("Ita") );

    bcpLeft2 = wxBarChartPoints::CreateWxBarChartPoints(
        wxT("Fra") );

    bcpLeft3 = wxBarChartPoints::CreateWxBarChartPoints(
        wxT("Ger") );

    bcpLeft4 = wxBarChartPoints::CreateWxBarChartPoints(
        wxT("UK") );

    pcpRight1 = wxPieChartPoints::CreateWxPieChartPoints(
        wxT("-Pie-") );

    bcpLeft1->Add( wxT("val3"), 2, 3 );
    bcpLeft1->Add( wxT("val1"), 5, 1 );
    bcpLeft1->Add( wxT("val4"), 8, 4);
    bcpLeft1->Add( wxT("val2"), 15, 2 );
    bcpLeft1->Add( wxT("val5"), 20, 5 );
    bcpLeft1->SetDisplayTag( XVALUE );

    bcpLeft2->Add( wxT("val: 4"), 2, 4 );
    bcpLeft2->Add( wxT("val: 2"), 5, 2 );
    bcpLeft2->Add( wxT("val: 2"), 8, 2);
    bcpLeft2->Add( wxT("val: 6"), 15, 6 );
    bcpLeft2->Add( wxT("val: 2"), 20, 2 );
    bcpLeft2->SetDisplayTag( NAME );

    bcpLeft3->Add( wxT("val3"), 2, 1 );
    bcpLeft3->Add( wxT("val1"), 5, 3 );
    bcpLeft3->Add( wxT("val4"), 8, 4);
    bcpLeft3->Add( wxT("val2"), 15, 5 );
    bcpLeft3->Add( wxT("val5"), 20, 5 );
    bcpLeft3->SetDisplayTag( XVALUE );

    bcpLeft4->Add( wxT("val3"), 2, 3 );
    bcpLeft4->Add( wxT("val6"), 5, 6 );
    bcpLeft4->Add( wxT("val2"), 8, 2);
    bcpLeft4->Add( wxT("val2"), 15, 2 );
    bcpLeft4->Add( wxT("val4"), 20, 4 );
    bcpLeft4->SetDisplayTag( YVALUE );

    pcpRight1->Add( wxT("val1"), 5, 10 );
    pcpRight1->Add( wxT("val2"), 15, 20 );
    pcpRight1->Add( wxT("val3"), 2, 5 );
    pcpRight1->Add( wxT("val4"), 8, 30);
    pcpRight1->Add( wxT("val5"), 20, 10 );
    pcpRight1->Add( wxT("val6"), 20, 15 );
    pcpRight1->Add( wxT("val7"), 20, 10 );
    pcpRight1->SetDisplayTag( NAME );

    wxStaticText *lblTitle4 = new wxStaticText(this,
                                               wxID_ANY, wxT("Bar/Pie Charts (R2)"));
    m_ChartCtrlR2 = new wxChartCtrl( this, -1, DEFAULT_STYLE,
                                    wxDefaultPosition, wxSize(200,200),
                                    wxSUNKEN_BORDER  );

    m_ChartCtrlR2->Add( bcpLeft1 );
    m_ChartCtrlR2->Add( bcpLeft2 );
    m_ChartCtrlR2->Add( bcpLeft3 );
    m_ChartCtrlR2->Add( bcpLeft4 );
    m_ChartCtrlR2->Add( pcpRight1 );

    sizer->Add( lblTitle4, 0, wxEXPAND );
    sizer->Add( m_ChartCtrlR2, 1, wxEXPAND );
}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       ResizeChart()
//  DESC:       resize chart
//  PARAMETERS: None
//  RETURN:     None
//----------------------------------------------------------------------E-+++
//void MyPanel::ResizeChart()
//{
//	m_pChartCtrl->Resize();
//}

//+++-S-cf-------------------------------------------------------------------
//  NAME:       WriteChart()
//  DESC:       Write chart to file
//  PARAMETERS: wxChartCtrl *c
//  RETURN:     None
//----------------------------------------------------------------------E-+++
void MyPanel::WriteChart(
    wxChartCtrl *c
)
{
    wxFileDialog dialog(this, _T("Save File As"), wxEmptyString, wxEmptyString,
                        wxT("*.bmp;*.gif;*.png;*.jpeg"),
                        wxSAVE | wxOVERWRITE_PROMPT);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString file = dialog.GetPath();

        //-------------------------------------------------------------------
        // choose here the image type you want.
        // It can be GIF, JPEG, PNG, BMP
        //-------------------------------------------------------------------
        if ( file.Find( wxT("jpeg") ) != -1 || file.Find( wxT("jpg") ) != -1 )
        {
            c->WriteToFile( dialog.GetPath(), wxCHART_JPEG );
        }
        else if ( file.Find( wxT("png") ) != -1 )
        {
            c->WriteToFile( dialog.GetPath(), wxCHART_PNG );
        }
        else if ( file.Find( wxT("bmp") ) != -1 )
        {
            c->WriteToFile( dialog.GetPath(), wxCHART_BMP );
        }
        else
        {
            c->WriteToFile( dialog.GetPath(), wxCHART_GIF );
        }

    }

}
