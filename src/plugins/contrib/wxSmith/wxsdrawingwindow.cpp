#include "wxsdrawingwindow.h"

#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/dcclient.h>
#include <wx/dcscreen.h>
#include <wx/dcmemory.h>
#include <wx/dcbuffer.h>

#include <manager.h>
#include <messagemanager.h>

namespace
{
    /** \brief Event type for background-fetching system */
    const int wxEVT_FETCH_SEQUENCE = wxNewEventType();

    /** \brief Identifier used by internal DrawingPanel class inside wxsDrawingWindow */
    const int DrawingPanelId = wxNewId();
}


/** \brief Drawing panel
 *
 * This panel is put over all other items in wxsDrawingWindow class. It's
 * responsible for fetching paing, mouse and keyboard events
 */
class wxsDrawingWindow::DrawingPanel: public wxPanel
{
    public:

        /** \brief Ctor */
        DrawingPanel(wxsDrawingWindow* Parent): wxPanel(Parent,DrawingPanelId)
        {
            // Connecting event handlers of drawing window
            Connect(DrawingPanelId,wxEVT_PAINT,(wxObjectEventFunction)&wxsDrawingWindow::PanelPaint,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_LEFT_DOWN,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_LEFT_UP,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_LEFT_DCLICK,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_MIDDLE_DOWN,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_MIDDLE_UP,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_MIDDLE_DCLICK,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_RIGHT_DOWN,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_RIGHT_UP,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_RIGHT_DCLICK,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_MOTION,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_ENTER_WINDOW,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_LEAVE_WINDOW,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_MOUSEWHEEL,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_KEY_DOWN,(wxObjectEventFunction)&wxsDrawingWindow::PanelKeyboard,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_KEY_UP,(wxObjectEventFunction)&wxsDrawingWindow::PanelKeyboard,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_CHAR,(wxObjectEventFunction)&wxsDrawingWindow::PanelKeyboard,NULL,Parent);
        }

};

BEGIN_EVENT_TABLE(wxsDrawingWindow,wxScrolledWindow)
END_EVENT_TABLE()

wxsDrawingWindow::wxsDrawingWindow(wxWindow* Parent,wxWindowID id):
    wxScrolledWindow(Parent,id),
    Panel(NULL),
    Bitmap(NULL),
    IsBlockFetch(false),
    DuringFetch(false)
{
    // Strange - it seems that by declaring this event in event table, it's not processed
    Connect(-1,-1,wxEVT_FETCH_SEQUENCE,(wxObjectEventFunction)&wxsDrawingWindow::OnFetchSequence);
    Panel = new DrawingPanel(this);
    ContentChanged();
    SetScrollbars(5,5,1,1,0,0,true);
}

wxsDrawingWindow::~wxsDrawingWindow()
{
    if ( Bitmap ) delete Bitmap;
}

void wxsDrawingWindow::ContentChanged()
{
    wxSize Size = GetVirtualSize();

    // Generating new bitmap
    if ( Bitmap ) delete Bitmap;
    Bitmap = new wxBitmap(Size.GetWidth(),Size.GetHeight());

    // Resizing panel to cover whole window
    int X, Y;
    CalcScrolledPosition(0,0,&X,&Y);
    Panel->SetSize(X,Y,Size.GetWidth(),Size.GetHeight());
    Panel->Refresh();
}

void wxsDrawingWindow::PanelPaint(wxPaintEvent& event)
{
    wxPaintDC PaintDC(Panel);
    if ( IsBlockFetch )
    {
        wxBitmap BmpCopy = Bitmap->GetSubBitmap(wxRect(0,0,Bitmap->GetWidth(),Bitmap->GetHeight()));
        wxBufferedDC DC(&PaintDC,BmpCopy);
        PaintExtra(&DC);
    }
    else
    {
        StartFetchingSequence();
    }
}

void wxsDrawingWindow::PanelMouse(wxMouseEvent& event)
{
    event.SetEventObject(this);
    event.SetId(GetId());
    ProcessEvent(event);
}

void wxsDrawingWindow::PanelKeyboard(wxKeyEvent& event)
{
    event.SetEventObject(this);
    event.SetId(GetId());
    ProcessEvent(event);
}

void wxsDrawingWindow::StartFetchingSequence()
{
    if ( DuringFetch )
    {
        return;
    }
    DuringFetch = true;

    // Fetching sequence will end after quitting
    // this event handler. This will be done
    // by adding some pending event
    wxCommandEvent event(wxEVT_FETCH_SEQUENCE,GetId());
    event.SetEventObject(this);
    GetEventHandler()->AddPendingEvent(event);
}

void wxsDrawingWindow::OnFetchSequence(wxCommandEvent& event)
{
    // Hiding panel to show content under it
    Panel->Hide();
    ShowChildren();
    Update();

    // Processing all pending events, it MUST be done
    // to repaint the content of window
    Manager::Yield();
    FetchScreen();
    HideChildren();
    Panel->Show();
    Manager::Yield();
    Panel->Update();
    Manager::Yield();

    FullRepaint();
    Manager::Yield();
    DuringFetch = false;
}

void wxsDrawingWindow::FetchScreen()
{
    // Fetching preview directly from screen
	wxScreenDC DC;
	wxMemoryDC DestDC;
    int X = 0, Y = 0;
    int DX = 0, DY = 0;
    ClientToScreen(&X,&Y);
    CalcUnscrolledPosition(0,0,&DX,&DY);
    DestDC.SelectObject(*Bitmap);
    DestDC.Blit(DX,DY,GetSize().GetWidth(),GetSize().GetHeight(),&DC,X,Y);
}

void wxsDrawingWindow::FullRepaint()
{
    wxClientDC ClientDC(Panel);
    wxBitmap BmpCopy = Bitmap->GetSubBitmap(wxRect(0,0,Bitmap->GetWidth(),Bitmap->GetHeight()));
    wxBufferedDC DC(&ClientDC,BmpCopy);
    PaintExtra(&DC);
}

void wxsDrawingWindow::ShowChildren()
{
    wxWindowList& Children = GetChildren();
    for ( size_t i=0; i<Children.GetCount(); i++ )
    {
        if ( Children[i] != Panel )
        {
            Children[i]->Show();
        }
    }
}

void wxsDrawingWindow::HideChildren()
{
    wxWindowList& Children = GetChildren();
    for ( size_t i=0; i<Children.GetCount(); i++ )
    {
        if ( Children[i] != Panel )
        {
            Children[i]->Hide();
        }
    }
}
