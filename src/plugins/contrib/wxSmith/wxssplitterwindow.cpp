#include "wxssplitterwindow.h"

#include <wx/sizer.h>

#define MIN_MARGIN  20

#include <manager.h>
#include <messagemanager.h>
#include <configmanager.h>

static const wxWindowID SplitterId = wxNewId();

wxsSplitterWindow::wxsSplitterWindow(wxWindow* Parent):
    wxPanel(Parent)
{
    wxBoxSizer* Sizer = new wxBoxSizer(wxVERTICAL);
    Sizer->Add( Splitter = new wxSplitterWindow(this,SplitterId), 1, wxGROW|wxALL );
    SetSizer(Sizer);
    SplitPosition = ConfigManager::Get()->Read("/wxsmith/res_panel_split",Splitter->GetSize().GetHeight()/2);
}

wxsSplitterWindow::~wxsSplitterWindow()
{
}

void wxsSplitterWindow::Split(wxWindow* Top,wxWindow* Bottom,int SashPosition)
{
    if ( Top ) Top->SetParent(Splitter);
    if ( Bottom ) Bottom->SetParent(Splitter);
    
    if ( SashPosition ) SplitPosition = SashPosition;
    Splitter->SplitHorizontally(Top,Bottom,SplitterFixup(SplitPosition));
}

void wxsSplitterWindow::OnSize(wxSizeEvent& event)
{
	Splitter->SetSashPosition(SplitterFixup(SplitPosition));
	event.Skip();
}

void wxsSplitterWindow::OnSplitterChanging(wxSplitterEvent& event)
{
	SplitPosition = event.GetSashPosition();
	event.SetSashPosition(SplitterFixup(SplitPosition));
}

void wxsSplitterWindow::OnSplitterChanged(wxSplitterEvent& event)
{
    // We use value which was previously set through OnSplitterChanging()
    // This will avoid hiding top panel when docking managment window
    event.SetSashPosition(SplitterFixup(SplitPosition));
    ConfigManager::Get()->Write("/wxsmith/res_panel_split",SplitPosition);
}

int wxsSplitterWindow::SplitterFixup(int Position)
{
    int Height = GetSize().GetHeight();
	int MinMargin = Height / 2;

	if ( MinMargin > MIN_MARGIN ) MinMargin = MIN_MARGIN;
	if ( Position < MinMargin ) Position = MinMargin;
	if ( Position > Height - MinMargin ) Position = Height - MinMargin;
	
	return Position;
}

BEGIN_EVENT_TABLE(wxsSplitterWindow,wxPanel)
    EVT_SIZE(wxsSplitterWindow::OnSize)
    EVT_SPLITTER_SASH_POS_CHANGING(SplitterId,wxsSplitterWindow::OnSplitterChanging)
    EVT_SPLITTER_SASH_POS_CHANGED(SplitterId,wxsSplitterWindow::OnSplitterChanged)
END_EVENT_TABLE()
