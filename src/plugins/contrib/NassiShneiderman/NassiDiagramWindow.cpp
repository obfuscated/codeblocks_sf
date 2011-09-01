#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/dcbuffer.h>

#include "NassiView.h"
#include "NassiDiagramWindow.h"
#include "NassiDropTarget.h"
//#include "DataObject.h"
#include "commands.h"
#include "HooverDrawlet.h"


//IMPLEMENT_DYNAMIC_CLASS(NassiDiagramWindow, wxScrolledWindow)
BEGIN_EVENT_TABLE(NassiDiagramWindow, wxScrolledWindow)
    EVT_MOUSEWHEEL(NassiDiagramWindow::OnMouseWheel)
    EVT_RIGHT_DOWN(NassiDiagramWindow::OnMouseRightDown)
    EVT_RIGHT_UP(NassiDiagramWindow::OnMouseRightUp)
    EVT_LEFT_DOWN(NassiDiagramWindow::OnMouseLeftDown)
    EVT_LEFT_UP(NassiDiagramWindow::OnMouseLeftUp)
    EVT_MOTION(NassiDiagramWindow::OnMouseMove)

    EVT_LEAVE_WINDOW(NassiDiagramWindow::OnEnter)
    EVT_ENTER_WINDOW(NassiDiagramWindow::OnLeave)

    EVT_PAINT(NassiDiagramWindow::OnPaint)
    EVT_ERASE_BACKGROUND(NassiDiagramWindow::OnErase)
    EVT_KEY_DOWN(NassiDiagramWindow::OnKeyDown)
    EVT_CHAR(NassiDiagramWindow::OnChar)
    //EVT_CHAR_HOOK(NassiDiagramWindow::OnChar)
    EVT_SET_FOCUS(NassiDiagramWindow::OnSetFocus)
    EVT_KILL_FOCUS(NassiDiagramWindow::OnKillFocus)

END_EVENT_TABLE()

// Define a constructor for my window
//MyDiagramWindow::MyDiagramWindow(wxView *v, wxMDIChildFrame *frame, const wxPoint& pos, const wxSize& size, long style):
// wxScrolledWindow(frame, wxID_ANY, pos, size, style)
NassiDiagramWindow::NassiDiagramWindow(wxWindow *parent, NassiView *view):
        wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER | wxWANTS_CHARS ),
        //dndpt(-1, -1),
        m_view(view),
        m_hd(0)
{
    SetDropTarget( new NassiDropTarget(this, view) );

    SetCursor(wxCursor(wxCURSOR_ARROW));
    SetScrollRate(5, 5);

    wxClientDC dc(this);
    dc.SetFont(m_view->GetSourceFont());
    SetCaret( new wxCaret(this, 10,10) );//1, dc.GetCharHeight()
    SetFocus();
}

NassiDiagramWindow::~NassiDiagramWindow()
{
    if ( m_hd )
        delete m_hd;
    // is not needed?
//    if ( caret )
//        delete caret;
}
void NassiDiagramWindow::OnMouseWheel(wxMouseEvent& event)
{
    wxClientDC dc(this);
    PrepareDC(dc);
    dc.SetFont(m_view->GetCommentFont());
    wxCoord dx, dy;
    dx = dc.GetCharWidth();
    dy = dc.GetCharHeight();

    if ( m_hd )
    {
        m_hd->Draw(dc);
        delete m_hd;
        m_hd = 0;
    }
    if ( ! event.IsPageScroll() )
    {
        wxInt32 nWheelRotation = event.GetWheelRotation();
        if ( !event.ControlDown() )
        {
            wxInt32 x, y;
            GetViewStart(&x, &y);
            if (nWheelRotation < 0)
                y += dy/4;
            else
                y -= dy/4;
            Scroll(x, y);
        }
        else
        {
            if (nWheelRotation < 0)
                m_view->ZoomIn();
            else
                m_view->ZoomOut();
        }
    }
}


void NassiDiagramWindow::OnErase(wxEraseEvent & /*event*/){}
void NassiDiagramWindow::OnPaint(wxPaintEvent & /*event*/)
{
    wxBufferedPaintDC dc(this);
    DoPrepareDC(dc);
    PaintBackground(dc);
    Draw(dc);
}
void NassiDiagramWindow::PaintBackground(wxDC &dc)
{
    wxColour backgroundColour = GetBackgroundColour();
    if ( !backgroundColour.Ok())
        backgroundColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);

    dc.SetBrush(wxBrush(backgroundColour));
    dc.SetPen(wxPen(backgroundColour, 1));
    wxRect windowRect(wxPoint(0,0), GetClientSize());
    CalcUnscrolledPosition(windowRect.x, windowRect.y, &windowRect.x, &windowRect.y);
    dc.DrawRectangle(windowRect);
    dc.SetBrush(wxNullBrush);
    dc.SetPen(wxNullPen);
}
void NassiDiagramWindow::Draw(wxDC& dc)
{
    m_view->DrawDiagram(&dc);

    if ( m_hd ) m_hd->Draw(dc);
}
void NassiDiagramWindow::OnEnter(wxMouseEvent & /*event*/)
{
    wxClientDC dc(this);
    DoPrepareDC(dc);

    if ( m_hd )
        m_hd->Draw(dc);
}
void NassiDiagramWindow::OnLeave(wxMouseEvent & /*event*/)
{
    wxClientDC dc(this);
    DoPrepareDC(dc);

    if ( m_hd )
        m_hd->UnDraw(dc);
}
void NassiDiagramWindow::OnChar(wxKeyEvent & event)
{
    m_view->OnChar(event);
}
void NassiDiagramWindow::OnKeyDown(wxKeyEvent &event)
{
    m_view->OnKeyDown(event);
    //event.Skip();
}

void NassiDiagramWindow::OnMouseLeftDown(wxMouseEvent &event)
{
    wxClientDC dc(this);
    DoPrepareDC(dc);
    RemoveDrawlet(dc);
    wxPoint pos = event.GetLogicalPosition(dc);
    m_view->OnMouseLeftDown(event, pos);

    this->SetFocus();
}
void NassiDiagramWindow::OnMouseLeftUp(wxMouseEvent &event)
{
    wxClientDC dc(this);
    DoPrepareDC(dc);
    RemoveDrawlet(dc);

    wxPoint pos = event.GetLogicalPosition(dc);
    m_view->OnMouseLeftUp(event, pos);

    this->SetFocus();
}
void NassiDiagramWindow::OnMouseRightDown(wxMouseEvent &event)
{
    wxClientDC dc(this);
    DoPrepareDC(dc);
    RemoveDrawlet(dc);

    wxPoint pos = event.GetLogicalPosition(dc);
    m_view->OnMouseRightDown(event, pos);

    this->SetFocus();
}
void NassiDiagramWindow::OnMouseRightUp(wxMouseEvent& event)
{
    wxClientDC dc(this);
    DoPrepareDC(dc);
    RemoveDrawlet(dc);

    wxPoint pos = event.GetLogicalPosition(dc);
    m_view->OnMouseRightUp(event, pos);

    this->SetFocus();
}

void NassiDiagramWindow::OnMouseMove(wxMouseEvent &event)
{
    wxClientDC dc(this);
    DoPrepareDC(dc);

    RemoveDrawlet(dc);

    wxPoint pos = event.GetLogicalPosition(dc);
    m_hd = m_view->OnMouseMove(event, pos);

    if ( m_hd && !m_hd->Draw(dc) )
    {
        delete m_hd;
        m_hd = 0;
    }
}

void NassiDiagramWindow::RemoveDrawlet(wxDC &dc)
{
    if ( m_hd )
    {
        m_hd->UnDraw(dc);
        delete m_hd;
        m_hd = 0;
    }
}

void NassiDiagramWindow::OnSetFocus(wxFocusEvent & /*event*/){}
void NassiDiagramWindow::OnKillFocus(wxFocusEvent & /*event*/)
{
    wxClientDC dc(this);
    DoPrepareDC(dc);

    RemoveDrawlet(dc);
}

wxDragResult NassiDiagramWindow::OnDrop(const wxPoint &pt, NassiBrick *brick, wxString strc, wxString strs, wxDragResult def )
{
    wxClientDC dc(this);
    DoPrepareDC(dc);
    wxCoord xx, yy;
    CalcUnscrolledPosition( pt.x, pt.y, &xx, &yy);

    wxPoint pos(xx, yy);

    RemoveDrawlet(dc);

    return m_view->OnDrop(pos, brick, strc, strs, def);
}
wxDragResult NassiDiagramWindow::OnDragOver(const wxPoint &pt, wxDragResult def, bool HasNoBricks)
{
    wxClientDC dc(this);
    DoPrepareDC(dc);
    wxCoord xx, yy;
    CalcUnscrolledPosition( pt.x, pt.y, &xx, &yy);
    wxPoint pos(xx, yy);
    RemoveDrawlet(dc);

    m_hd = m_view->OnDragOver(pos, def, HasNoBricks);

    if ( m_hd && !m_hd->Draw(dc) )
    {
        delete m_hd;
        m_hd = 0;
    }

    return def;
}
void NassiDiagramWindow::OnDragLeave(void)
{
    m_view->OnDragLeave();
}
void NassiDiagramWindow::OnDragEnter(void)
{
    wxClientDC dc(this);
    DoPrepareDC(dc);

    RemoveDrawlet(dc);

    m_view->OnDragEnter();
}

