///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 17 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "MainPanel.h"

///////////////////////////////////////////////////////////////////////////

MainPanel::MainPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
            : wxPanel( parent, id, pos, size, style )
{
	m_pMainSizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* SplitterSizer;
	SplitterSizer = new wxBoxSizer( wxVERTICAL );

	m_pSplitterWindow = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxSize(1,1), wxSP_3D|wxSP_BORDER  );
	//-m_pSplitterWindow->Connect( wxEVT_IDLE, wxIdleEventHandler( MainPanel::SplitterWindowOnIdle ), NULL, this );

	m_pSearchPanel = new wxPanel( m_pSplitterWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL,_T("MySearchPanel") );
	m_pSearchSizer = new wxBoxSizer( wxVERTICAL );
    m_pSearchPanel->SetSizer( m_pSearchSizer );
	m_pSearchPanel->Layout();
	//m_pSearchSizer->Fit( m_pSearchPanel );

	m_pNotebkPanel = new wxPanel( m_pSplitterWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL,_T("MyNotebkPanel") );
	m_pNotebkSizer = new wxBoxSizer( wxVERTICAL );
    m_pNotebkPanel->SetSizer( m_pNotebkSizer );
	m_pNotebkPanel->Layout();
	//m_pNotebkSizer->Fit( m_pNotebkPanel );

	m_pSplitterWindow->SplitHorizontally( m_pSearchPanel, m_pNotebkPanel, 100 );
	SplitterSizer->Add( m_pSplitterWindow, 1, wxEXPAND, 5 );

	m_pMainSizer->Add( SplitterSizer, 1, wxEXPAND, 5 );

	this->SetSizer( m_pMainSizer );
	this->Layout();
}

MainPanel::~MainPanel()
{
}
//Note:
//If you have a sizer for your frame that you want to
// add a panel to, make sure that you add it after
// you add everything to the panel and call
// panel->SetSizerAndFit( panelsizer ). If you first
// add the panel, then add controls to it, the
// window will not size correctly.
//
//Or you can call panel->GetSizer()->Fit(panel)
//after you add the panels controls to re-fit the
//sizer.
//[edit]
//
//
//http://www.wxwidgets.org/wiki/index.php/WxSplitterWindow
//WxSplitterWindow - WxWiki

