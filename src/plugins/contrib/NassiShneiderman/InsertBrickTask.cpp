#include "InsertBrickTask.h"
#include "GraphBricks.h"
#include "RedLineDrawlet.h"
#include "RedHatchDrawlet.h"
#include "NassiFileContent.h"
#include "commands.h"
#include "NassiView.h"


#include <wx/mstream.h>


#if defined(__WXGTK__)
    #include "rc/ifcur_inv.xpm"
    #include "rc/whilecur_inv.xpm"
    #include "rc/dowhilecur_inv.xpm"
    #include "rc/instrcur_inv.xpm"
    #include "rc/forcur_inv.xpm"
    #include "rc/switchcur_inv.xpm"
    #include "rc/breakcur_inv.xpm"
    #include "rc/continuecur_inv.xpm"
    #include "rc/returncur_inv.xpm"
    #include "rc/blockcur_inv.xpm"
#else
    #include "rc/ifcur.xpm"
    #include "rc/whilecur.xpm"
    #include "rc/dowhilecur.xpm"
    #include "rc/instrcur.xpm"
    #include "rc/forcur.xpm"
    #include "rc/switchcur.xpm"
    #include "rc/breakcur.xpm"
    #include "rc/continuecur.xpm"
    #include "rc/returncur.xpm"
    #include "rc/blockcur.xpm"
#endif


InsertBrickTask::InsertBrickTask(NassiView *view, NassiFileContent *nfc, NassiView::NassiTools tool):
    Task(),
    m_view(view),
    m_nfc(nfc),
    m_done(false),
    m_tool(tool)
{}

InsertBrickTask::~InsertBrickTask()
{}

wxCursor InsertBrickTask::Start()
{
    switch ( m_tool )
    {
        case NassiView::NASSI_TOOL_CONTINUE:    return wxCursor(continuecur_xpm);
        case NassiView::NASSI_TOOL_BREAK:       return wxCursor(breakcur_xpm);
        case NassiView::NASSI_TOOL_RETURN:      return wxCursor(returncur_xpm);
        case NassiView::NASSI_TOOL_WHILE:       return wxCursor(whilecur_xpm);
        case NassiView::NASSI_TOOL_DOWHILE:     return wxCursor(dowhilecur_xpm);
        case NassiView::NASSI_TOOL_FOR:         return wxCursor(forcur_xpm);
        case NassiView::NASSI_TOOL_BLOCK:       return wxCursor(blockcur_xpm);
        case NassiView::NASSI_TOOL_IF:          return wxCursor(wxImage(ifcur_xpm));
        case NassiView::NASSI_TOOL_SWITCH:      return wxCursor(switchcur_xpm);
        default:
        case NassiView::NASSI_TOOL_INSTRUCTION: return wxCursor(instrcur_xpm);
    }
}

bool InsertBrickTask::Done()const
{
    return m_done;
}

// events from window:
void InsertBrickTask::OnMouseLeftUp(wxMouseEvent & /*event*/, const wxPoint & /*position*/){}
void InsertBrickTask::OnMouseLeftDown(wxMouseEvent & /*event*/, const wxPoint &position)
{
    if ( !m_nfc->GetFirstBrick() )
    {
        wxRect rect = m_view->GetEmptyRootRect();
        if ( rect.Contains(position) )
            m_nfc->GetCommandProcessor()->Submit(
                new NassiInsertFirstBrick(m_nfc, m_view->GenerateNewBrick(m_tool))
            );
        return;
    }

    GraphNassiBrick *gbrick = m_view->GetBrickAtPosition(position);
    if ( gbrick )
    {

        GraphNassiBrick::Position p = gbrick->GetPosition(position);
        if ( p.pos == GraphNassiBrick::Position::bottom )
            m_nfc->GetCommandProcessor()->Submit(
                new NassiInsertBrickAfter( m_nfc, gbrick->GetBrick(), m_view->GenerateNewBrick(m_tool) ));
        else if ( p.pos == GraphNassiBrick::Position::top )
            m_nfc->GetCommandProcessor()->Submit(
                new NassiInsertBrickBefore(m_nfc, gbrick->GetBrick(), m_view->GenerateNewBrick(m_tool) ));
        else if ( p.pos == GraphNassiBrick::Position::child )
            m_nfc->GetCommandProcessor()->Submit(
                new NassiInsertChildBrickCommand(m_nfc, gbrick->GetBrick(), m_view->GenerateNewBrick(m_tool), p.number));
        else if ( p.pos == GraphNassiBrick::Position::childindicator )
            m_nfc->GetCommandProcessor()->Submit(
                new NassiAddChildIndicatorCommand(m_nfc, gbrick->GetBrick(), m_view->GenerateNewBrick(m_tool), p.number));
    }
}
void InsertBrickTask::OnMouseRightDown(wxMouseEvent & /*event*/, const wxPoint & /*position*/)
{
    m_done = true;
}
void InsertBrickTask::OnMouseRightUp(wxMouseEvent& /*event*/, const wxPoint & /*position*/){}
HooverDrawlet *InsertBrickTask::OnMouseMove(wxMouseEvent & /*event*/, const wxPoint &position)
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
void InsertBrickTask::OnKeyDown(wxKeyEvent &event)
{
    if ( event.GetKeyCode() == WXK_ESCAPE )
    {
        m_done = true;
        return;
    }
    //event.Skip();
}
void InsertBrickTask::OnChar(wxKeyEvent & /*event*/){}

// events from frame(s)
bool InsertBrickTask::CanEdit()const{ return false; }
//bool InsertBrickTask::CanCopy()const{ return false; }
//bool InsertBrickTask::CanCut()const{ return false; }
bool InsertBrickTask::CanPaste()const{ return false; }
bool InsertBrickTask::HasSelection()const{ return false; }
void InsertBrickTask::DeleteSelection(){}
void InsertBrickTask::Copy(){}
void InsertBrickTask::Cut(){}
void InsertBrickTask::Paste(){}

