#include "wxsdrawingwindow.h"

#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/dcclient.h>
#include <wx/dcscreen.h>
#include <wx/dcmemory.h>
#include <wx/dcbuffer.h>

#define FETCH_EVENT_TICKS   0x10

/** \brief Drawing panel
 *
 * This panel is put over all other items in wxsDrawingWindow class. It's
 * responsible for fetching paing, mouse and keyboard events
 */
class wxsDrawingWindow::DrawingPanel: public wxPanel
{
    public:

        /** \brief Ctor */
        DrawingPanel(wxsDrawingWindow* Parent): wxPanel(Parent,-1)
        {
            // Connecting event handlers of drawing window
            Connect(-1,wxEVT_PAINT,(wxObjectEventFunction)&wxsDrawingWindow::PanelPaint,NULL,Parent);
            Connect(-1,wxEVT_LEFT_DOWN,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(-1,wxEVT_LEFT_UP,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(-1,wxEVT_LEFT_DCLICK,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(-1,wxEVT_MIDDLE_DOWN,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(-1,wxEVT_MIDDLE_UP,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(-1,wxEVT_MIDDLE_DCLICK,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(-1,wxEVT_RIGHT_DOWN,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(-1,wxEVT_RIGHT_UP,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(-1,wxEVT_RIGHT_DCLICK,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(-1,wxEVT_MOTION,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(-1,wxEVT_ENTER_WINDOW,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(-1,wxEVT_LEAVE_WINDOW,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(-1,wxEVT_MOUSEWHEEL,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(-1,wxEVT_KEY_DOWN,(wxObjectEventFunction)&wxsDrawingWindow::PanelKeyboard,NULL,Parent);
            Connect(-1,wxEVT_KEY_UP,(wxObjectEventFunction)&wxsDrawingWindow::PanelKeyboard,NULL,Parent);
            Connect(-1,wxEVT_CHAR,(wxObjectEventFunction)&wxsDrawingWindow::PanelKeyboard,NULL,Parent);
        }

};

DECLARE_EVENT_TYPE(wxEVT_FETCH_BACKGROUND_DELAY, -1)
DEFINE_EVENT_TYPE(wxEVT_FETCH_BACKGROUND_DELAY)

BEGIN_EVENT_TABLE(wxsDrawingWindow,wxScrolledWindow)
    EVT_COMMAND(-1,wxEVT_FETCH_BACKGROUND_DELAY,wxsDrawingWindow::OnFetchDelay)
    EVT_SIZE(wxsDrawingWindow::OnSize)
    EVT_TIMER(1,wxsDrawingWindow::OnFetchTimer)
    EVT_TIMER(2,wxsDrawingWindow::OnRepaintTimer)
END_EVENT_TABLE()

wxsDrawingWindow::wxsDrawingWindow(wxWindow* Parent,wxWindowID id):
    wxScrolledWindow(Parent,id),
    Panel(NULL),
    PaintAfterFetch(false),
    IsBlockFetch(false),
    FetchCounter(0),
    Bitmap(NULL),
    RepaintTimer(this,2),
    FetchTimer(this,1)
{
    ContentChanged();
}

wxsDrawingWindow::~wxsDrawingWindow()
{
    if ( Bitmap ) delete Bitmap;
}

void wxsDrawingWindow::ContentChanged()
{
    wxSize Size = GetClientSize();

    // Generating new bitmap
    if ( Bitmap ) delete Bitmap;
    Bitmap = new wxBitmap(Size.GetWidth(),Size.GetHeight());

    // Recreating drawing panel
    if ( Panel ) delete Panel;
    Panel = new DrawingPanel(this);
    Panel->Raise();

    // Resizing panel to cover whole window
    Panel->SetSize(0,0,Size.GetWidth(),Size.GetHeight());

    // Background will be fetched inside panel's internal routines when showing this window
}

void wxsDrawingWindow::PanelPaint(wxPaintEvent& event)
{
    if ( PaintAfterFetch || IsBlockFetch )
    {
        wxPaintDC DC(Panel);
        PaintAfterFetch = false;
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
    // Hiding panel to show content under it
    Panel->Hide();

    // We will add FETCH_EVENT_TICKS ticks in event queue - this will
    // allow processing all events left like repositioning and repainting
    // stuff
    FetchCounter = FETCH_EVENT_TICKS;
    wxCommandEvent evt(wxEVT_FETCH_BACKGROUND_DELAY, GetId() );
    AddPendingEvent(evt);
}

void wxsDrawingWindow::OnFetchDelay(wxCommandEvent& event)
{
    // Waiting till given number of ticks is done
    if ( FetchCounter-- > 0 )
    {
        AddPendingEvent(event);
        return;
    }

    // Fetching preview directly from screen
	wxScreenDC DC;
	wxMemoryDC DestDC;
    int X = 0, Y = 0;
    ClientToScreen(&X,&Y);
    DestDC.SelectObject(*Bitmap);
    DestDC.Blit(0,0,GetSize().GetWidth(),GetSize().GetHeight(),&DC,X,Y);

    // Now starting fetching timer
    FetchTimer.Start(100,true);
}

void wxsDrawingWindow::OnFetchTimer(wxTimerEvent& event)
{
    // Showing panel and repainting using fetched bitmap
    PaintAfterFetch = true;
    Panel->Show();
    RepaintTimer.Start(100,true);
}

void wxsDrawingWindow::OnRepaintTimer(wxTimerEvent& event)
{
    wxClientDC ClientDC(Panel);
    wxBitmap BmpCopy(*Bitmap);
    wxBufferedDC DC(&ClientDC,BmpCopy);
    PaintExtra(&DC);
}

void wxsDrawingWindow::OnSize(wxSizeEvent& event)
{
    // After resizing we need to invalidate content
    ContentChanged();
}
