#include "PasteTask.h"
#include "GraphBricks.h"
#include "RedLineDrawlet.h"
#include "RedHatchDrawlet.h"
#include "NassiFileContent.h"
#include "commands.h"
#include "NassiView.h"

#include "rc/dnd_move_cur.xpm"

PasteTask::PasteTask(NassiView *view, NassiFileContent *nfc, NassiBrick *brick, wxString strc, wxString strs):
    Task(),
    m_view(view),
    m_nfc(nfc),
    m_done(false),
    m_brick(brick),
    m_strc(strc),
    m_strs(strs)
{}

PasteTask::~PasteTask()
{
    if ( m_brick )
        delete m_brick;
}

wxCursor PasteTask::Start()
{
    wxBitmap *b_curs = new wxBitmap(dnd_move_cur_xpm);
    wxCursor newcursor(b_curs->ConvertToImage());
    delete b_curs;
    return newcursor;
}

bool PasteTask::Done()const
{
    return m_done;
}

// events from window:
void PasteTask::OnMouseLeftUp(wxMouseEvent& /*event*/, const wxPoint& /*position*/){}
void PasteTask::OnMouseLeftDown(wxMouseEvent& /*event*/, const wxPoint &position)
{
    if ( !m_nfc->GetFirstBrick() )
    {
        wxRect rect = m_view->GetEmptyRootRect();
        if ( rect.Contains(position) )
        {
            NassiBrick *brick = m_brick;
            m_brick = 0;
            m_nfc->GetCommandProcessor()->Submit(
                new NassiInsertFirstBrick(m_nfc, brick)
            );
            m_done = true;
        }
        return;
    }

    GraphNassiBrick *gbrick = m_view->GetBrickAtPosition(position);
    if ( gbrick )
    {
        GraphNassiBrick::Position p = gbrick->GetPosition(position);
        if (m_brick && (p.pos == GraphNassiBrick::Position::bottom ))
        {
            m_nfc->GetCommandProcessor()->Submit(
                new NassiInsertBrickAfter( m_nfc, gbrick->GetBrick(), m_brick ));
            m_brick = 0;
        }
        else if (m_brick && (p.pos == GraphNassiBrick::Position::top ))
        {
            m_nfc->GetCommandProcessor()->Submit(
                new NassiInsertBrickBefore(m_nfc, gbrick->GetBrick(), m_brick ));
            m_brick = 0;
        }
        else if ( m_brick && ( p.pos == GraphNassiBrick::Position::child ))
        {
            m_nfc->GetCommandProcessor()->Submit(
                new NassiInsertChildBrickCommand(m_nfc, gbrick->GetBrick(), m_brick, p.number));
            m_brick = 0;
        }
        else if ( p.pos == GraphNassiBrick::Position::childindicator )
        {
            m_nfc->GetCommandProcessor()->Submit(
                new NassiAddChildIndicatorCommand(m_nfc, gbrick->GetBrick(), m_brick, p.number, m_strc, m_strs));
            m_brick = 0;
        }
        m_done = true;
    }
}
void PasteTask::OnMouseRightDown(wxMouseEvent& /*event*/, const wxPoint& /*position*/)
{
    m_done = true;
}
void PasteTask::OnMouseRightUp(wxMouseEvent& /*event*/, const wxPoint& /*position*/){}
HooverDrawlet *PasteTask::OnMouseMove(wxMouseEvent& /*event*/, const wxPoint &position)
{
    if ( !m_nfc->GetFirstBrick() )
    {
        wxRect rect = m_view->GetEmptyRootRect();
        if ( rect.Contains(position) )
            return new RedHatchDrawlet(rect);
        else
            return 0;
    }

    GraphNassiBrick *gbrick = m_view->GetBrickAtPosition(position);
    if ( gbrick )
        return gbrick->GetDrawlet(position, false);
    return 0;
}
void PasteTask::OnKeyDown(wxKeyEvent &event)
{
    if ( event.GetKeyCode() == WXK_ESCAPE )
    {
        m_done = true;
        return;
    }
    //event.Skip();
}
void PasteTask::OnChar(wxKeyEvent & /*event*/){}

// events from frame( s )
bool PasteTask::CanEdit()const{ return false; }
bool PasteTask::HasSelection()const{ return false; }
bool PasteTask::CanPaste()const{ return false; }
void PasteTask::DeleteSelection(){}
void PasteTask::Copy(){}
void PasteTask::Paste(){}
void PasteTask::Cut(){}

//bool PasteTask::CanCopy()const{return false;}
//bool PasteTask::CanCut()const{return false;}
