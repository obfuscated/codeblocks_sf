#include "GraphBricks.h"
#include "TextGraph.h"
#include "bricks.h"
#include "NassiView.h"

#include "RedLineDrawlet.h"
#include "RedHatchDrawlet.h"

#include "rc/iftool16.xpm"
#include "rc/whiletool16.xpm"
#include "rc/dowhiletool16.xpm"
#include "rc/fortool16.xpm"
#include "rc/blocktool16.xpm"
#include "rc/switchtool16.xpm"


GraphNassiBrick::GraphNassiBrick(NassiView *view, NassiBrick *brick, BricksMap *bmap):
    m_brick(brick),
    m_view(view),
    m_offset(),
    m_size(),
    m_minimumsize(),
    m_visible(true),
    m_active(false),
    m_used(true),
    m_map(bmap)
{}
GraphNassiBrick::~GraphNassiBrick(void){}

bool GraphNassiBrick::IsVisible()
{
    return m_visible;
}
void GraphNassiBrick::SetInvisible(bool vis)
{
    m_visible = vis;

    NassiBrick *next = m_brick->GetNext();
    if ( next )
    {
        GraphNassiBrick *gbrick = this->GetGraphBrick(next);
        if ( gbrick )
            gbrick->SetInvisible(vis);
    }

    for ( wxUint32 n = 0; n < m_brick->GetChildCount() ; n++)
    {
        NassiBrick *child = m_brick->GetChild(n);
        if ( child )
        {
            GraphNassiBrick *gbrick = this->GetGraphBrick(child);
            if ( gbrick )
                gbrick->SetInvisible(vis);
        }
    }

}
wxUint32 GraphNassiBrick::GetWidth()
{
    return m_size.x;
}
wxPoint GraphNassiBrick::GetOffset()
{
    return m_offset;
}
wxUint32 GraphNassiBrick::GetHeight()
{
    return m_size.y;
}
wxUint32 GraphNassiBrick::GetMinimumWidth()
{
    return m_minimumsize.x;
}
wxUint32 GraphNassiBrick::GetMinimumHeight()
{
    return m_minimumsize.y;
}
void GraphNassiBrick::DrawActive(wxDC *dc)
{
    if ( !IsActive() || !IsVisible() ) return;
    wxBrush *brush = new wxBrush(*wxBLUE, wxTRANSPARENT);
    wxPen *pen = new wxPen(*wxBLUE, 3);
    dc->SetBrush(*brush);
    dc->SetPen(*pen);
    dc->DrawRectangle(m_offset.x, m_offset.y, m_size.x, m_size.y);
    dc->SetBrush(*wxWHITE_BRUSH);
    dc->SetPen(wxNullPen);
    delete brush;
    delete pen;
}
bool GraphNassiBrick::HasPoint(const wxPoint &pos)
{
    if ( !IsVisible() ) return false;

    if ( pos.x >= m_offset.x &&           pos.y > m_offset.y &&
         pos.x < m_offset.x + m_size.x && pos.y < m_offset.y + m_size.y)
         return true;
    return false;

}
void GraphNassiBrick::SetActive(bool act, bool withChilds)
{
    m_active = act;

    if ( withChilds )
    {
        for ( wxUint32 n = 0 ; n < m_brick->GetChildCount() ; n++)
        {
            NassiBrick *child = m_brick->GetChild(n);
            if ( child )
            {
                GraphNassiBrick *chgbrick = this->GetGraphBrick( child );
                while ( chgbrick )
                {
                    chgbrick->SetActive(act, true);
                    child = child->GetNext();
                    chgbrick = this->GetGraphBrick( child );
                }
            }
        }
    }
}
bool GraphNassiBrick::IsOverChild(const wxPoint & /*pos*/, wxRect * /*childRect*/, wxUint32 * /*childNumber*/)
{
    return false;
}
GraphNassiBrick *GraphNassiBrick::GetGraphBrick(NassiBrick *brick)
{
    if ( m_map->find(brick) == m_map->end() )
        return (GraphNassiBrick *)0;
    return (*m_map)[brick];
}
GraphNassiBrick::Position GraphNassiBrick::GetPosition(const wxPoint &pos)
{
    GraphNassiBrick::Position res;

    if ( IsActive() || !HasPoint(pos) )
    {
        res.pos = Position::none;
        return res;
    }

    wxPoint offset = GetOffset();
    wxInt32 height = GetHeight();

    res.pos = Position::top;
    if ( 2*pos.y > 2* offset.y + height )
        res.pos = Position::bottom;

    return res;
}
HooverDrawlet *GraphNassiBrick::GetDrawlet(const wxPoint &pos, bool HasNoBricks)
{
    Position p = GetPosition(pos);

    if ( p.pos == Position::none || HasNoBricks )
        return 0;

    if ( p.pos == Position::top )
        return new RedLineDrawlet(m_offset, GetWidth());

    return new RedLineDrawlet( m_offset + wxPoint(0, GetHeight()-1), GetWidth() );

//    if ( IsActive() || !HasPoint(pos) || HasNoBricks  )
//        return 0;
//    wxPoint offset = GetOffset();
//    wxUint32 height = GetHeight();
//    if ( 2*pos.y > 2*offset.y + height )
//        offset.y += height - 1;
//    return new RedLineDrawlet(offset, GetWidth());
}
void GraphNassiBrick::Draw(wxDC *dc)
{
    dc->SetBrush(*wxWHITE_BRUSH);
}



GraphNassiMinimizableBrick::GraphNassiMinimizableBrick(NassiView *view, NassiBrick *brick, BricksMap *bmap):
    GraphNassiBrick(view, brick, bmap),
    m_minimized(false)
{}

void GraphNassiMinimizableBrick::DrawMinMaxBox(wxDC *dc)
{
    if ( IsMinimized() )
        DrawMinBox(dc);
    else
        DrawMaxBox(dc);
}
void GraphNassiMinimizableBrick::DrawMinBox(wxDC *dc)
{
#if defined(__WXMSW__)
    dc->DrawRectangle(m_offset.x + 2, m_offset.y + 2, 9, 9);
    dc->DrawLine(m_offset.x + 4, m_offset.y + 6, m_offset.x + 9, m_offset.y + 6);
    dc->DrawLine(m_offset.x + 6, m_offset.y + 4, m_offset.x + 6, m_offset.y + 9);
#else
    //right triangle
    dc->DrawLine(m_offset.x+2, m_offset.y+2, m_offset.x+2, m_offset.y+10);
    dc->DrawLine(m_offset.x+2, m_offset.y+2, m_offset.x+7, m_offset.y+6);
    dc->DrawLine(m_offset.x+2, m_offset.y+9, m_offset.x+7, m_offset.y+5);
#endif
}
void GraphNassiMinimizableBrick::DrawMaxBox(wxDC *dc)
{
#if defined(__WXMSW__)
    dc->DrawRectangle(m_offset.x + 2, m_offset.y + 2, 9, 9);
    dc->DrawLine(m_offset.x + 4, m_offset.y + 6, m_offset.x +9, m_offset.y + 6);
#else
    dc->DrawLine(m_offset.x+2, m_offset.y+2, m_offset.x+10, m_offset.y+2);
    dc->DrawLine(m_offset.x+2, m_offset.y+2, m_offset.x+6, m_offset.y+7);
    dc->DrawLine(m_offset.x+10, m_offset.y+2, m_offset.x+5, m_offset.y+7);
#endif
}
bool GraphNassiMinimizableBrick::IsOverMinMaxBox(const wxPoint &pos)
{
    if ( !m_visible ) return false;
    if ( pos.x >= m_offset.x + 2 && pos.y >= m_offset.y + 2 &&
         pos.x <= m_offset.x + 9 && pos.y <= m_offset.y + 9 )
        return true;
    return false;
}
HooverDrawlet *GraphNassiMinimizableBrick::GetDrawlet(const wxPoint & pos, bool HasNoBricks)
{
    Position p = GetPosition(pos);

    if ( p.pos == Position::none || HasNoBricks )
        return 0;

    if ( p.pos == Position::top )
        return new RedLineDrawlet(m_offset, GetWidth());
    if ( p.pos == Position::bottom )
        return new RedLineDrawlet( m_offset + wxPoint(0, GetHeight()-1), GetWidth() );

    wxRect rect;
    IsOverChild(pos, &rect);
    return new RedHatchDrawlet(rect);
}

GraphNassiBrick::Position GraphNassiMinimizableBrick::GetPosition(const wxPoint &pos)
{
    GraphNassiBrick::Position res;

    if ( IsActive() || !HasPoint(pos) )
    {
        res.pos = Position::none;
        return res;
    }

    wxRect rect;
    wxUint32 childnumber;
    if ( IsOverChild(pos, &rect, &childnumber) )
    {
        res.pos = Position::child;
        res.number = childnumber;
    }
    else
    {
        res.pos = Position::top;
        if ( 2*pos.y > 2*m_offset.y + static_cast<int>(GetHeight()) )
            res.pos = Position::bottom;
    }
    return res;
}



GraphNassiInstructionBrick::GraphNassiInstructionBrick(NassiView *view, NassiBrick *brick, BricksMap *bmap):
    GraphNassiBrick(view, brick, bmap),
    comment(view, brick, 0 ),
    source(view, brick, 1 )
{
    //m_view->CreateTextGraph(m_brick, 0);
    //m_view->CreateTextGraph(m_brick, 1);
}
GraphNassiInstructionBrick::~GraphNassiInstructionBrick(){}
void GraphNassiInstructionBrick::Draw(wxDC *dc)
{
    if ( !m_visible ) return;
    GraphNassiBrick::Draw(dc);
    dc->DrawRectangle(m_offset.x, m_offset.y, m_size.x, m_size.y);
    if ( m_view->IsDrawingComment() )
    {
        dc->SetFont(m_view->GetCommentFont());
        //MultilineTextGraph *comment = m_view->GetTextGraph(m_brick, 0);
        comment.Draw(dc);
    }
    if ( m_view->IsDrawingSource() )
    {
        dc->SetFont(m_view->GetSourceFont());
        //MultilineTextGraph *source = m_view->GetTextGraph(m_brick, 1);
        source.Draw(dc);
    }
}
void GraphNassiInstructionBrick::SetOffsetAndSize(wxDC *dc, wxPoint offset, wxPoint size)
{
    if ( !m_visible ) return;
    if ( m_brick->GetNext() )
        m_size.y = GetMinimumHeight();
    else
        m_size.y = size.y;
    m_size.x = size.x;
    m_offset = offset;


    wxCoord dx = dc->GetCharWidth(),
            dy = dc->GetCharHeight();

    wxCoord d=0;
    if ( m_view->IsDrawingComment() )
    {
        //MultilineTextGraph *comment = m_view->GetTextGraph(m_brick, 0);
        comment.SetOffset( wxPoint(m_offset.x + dx, m_offset.y + dy) );
        d += comment.GetTotalHeight();
        d += dy;
    }
    if ( m_view->IsDrawingSource() )
    {
        //MultilineTextGraph *source = m_view->GetTextGraph(m_brick, 1);
        source.SetOffset( wxPoint(m_offset.x + dx, m_offset.y + dy + d) );
    }

    offset.y += ( m_size.y - 1 );
    size.y -= ( m_size.y - 1);

    GraphNassiBrick *nextgbrick = this->GetGraphBrick( m_brick->GetNext() );
    if ( nextgbrick )
        nextgbrick->SetOffsetAndSize(dc, offset, size);
}
void GraphNassiInstructionBrick::CalcMinSize(wxDC *dc, wxPoint &size)
{
    dc->SetFont( m_view->GetCommentFont() );
    //MultilineTextGraph *comment = m_view->GetTextGraph(m_brick, 0);
    comment.CalcMinSize(dc);

    dc->SetFont( m_view->GetSourceFont() );
    //MultilineTextGraph *source = m_view->GetTextGraph(m_brick, 1);
    source.CalcMinSize(dc);

    wxCoord w=0, h=0;
    if ( m_view->IsDrawingComment() )
    {
        w = comment.GetWidth();
        h = comment.GetTotalHeight();
    }

    if ( m_view->IsDrawingSource() )
    {
        wxCoord a = source.GetWidth(),
                d = source.GetTotalHeight();
        h += d;
        if (m_view->IsDrawingComment() )
            h += dc->GetCharHeight();
        if ( w < a )
            w = a;
    }

    m_minimumsize.x = w + 2*dc->GetCharWidth();
    m_minimumsize.y = h + 2*dc->GetCharHeight();

    if ( size.x < m_minimumsize.x )
        size.x = m_minimumsize.x;
    size.y += m_minimumsize.y;

    NassiBrick *next = m_brick->GetNext();
    GraphNassiBrick *gnext = this->GetGraphBrick(next);
    if ( gnext )
    {
        gnext->CalcMinSize(dc, size);
        size.y--;
    }
}
TextGraph *GraphNassiInstructionBrick::IsOverText(const wxPoint &pos)
{
    if ( !m_visible ) return 0;
    if ( !m_visible ) return 0;
    if (m_view->IsDrawingComment() && comment.HasPoint(pos))
        return &comment;
    if (m_view->IsDrawingSource() && source.HasPoint(pos))
        return &source;

    return 0;
}


GraphNassiBreakBrick::GraphNassiBreakBrick(NassiView *view, NassiBrick *brick, BricksMap *bmap):
    GraphNassiBrick(view, brick, bmap),
    comment(view, brick, 0),
    m_b(0)
{}
GraphNassiBreakBrick::~GraphNassiBreakBrick(){}
void GraphNassiBreakBrick::Draw(wxDC *dc)
{
    if ( !m_visible ) return;
    GraphNassiBrick::Draw(dc);
    dc->DrawRectangle(m_offset.x, m_offset.y, m_size.x, m_size.y);
    dc->DrawLine(m_offset.x + m_size.x - m_b - 1, m_offset.y,
                 m_offset.x + m_size.x - 1,       m_offset.y + m_size.y/2);
    dc->DrawLine(m_offset.x + m_size.x - 1,       m_offset.y + m_size.y/2,
                 m_offset.x + m_size.x - m_b - 1, m_offset.y + m_size.y);

    if ( m_view->IsDrawingComment() )
    {
        dc->SetFont(m_view->GetCommentFont());
        //MultilineTextGraph *comment = m_view->GetTextGraph(m_brick, 0);
        comment.Draw( dc );
    }
}
void GraphNassiBreakBrick::SetOffsetAndSize(wxDC *dc, wxPoint offset, wxPoint size)
{
    if ( !m_visible ) return;
    if ( m_brick->GetNext() )
        m_size.y = GetMinimumHeight();
    else
        m_size.y = size.y;
    m_size.x = size.x;
    m_offset = offset;

    wxCoord dx = dc->GetCharWidth(),
            dy = dc->GetCharHeight(),
            h = 0;
    if ( m_view->IsDrawingComment() )
    {
        //MultilineTextGraph *comment = m_view->GetTextGraph(m_brick, 0);
        h = comment.GetTotalHeight();
    }

    m_b = h + dy;

    if ( m_view->IsDrawingComment() )
    {
        dc->SetFont(m_view->GetCommentFont());
        //MultilineTextGraph *comment = m_view->GetTextGraph(m_brick, 0);
        comment.SetOffset( wxPoint(m_offset.x + dx, m_offset.y + m_size.y/2 - h/2 ) );
    }

    offset.y += ( m_size.y - 1 );
    size.y -= ( m_size.y - 1);

    GraphNassiBrick *nextgbrick = this->GetGraphBrick( m_brick->GetNext() );
    if ( nextgbrick )
        nextgbrick->SetOffsetAndSize(dc, offset, size);
}
void GraphNassiBreakBrick::CalcMinSize(wxDC *dc, wxPoint &size)
{
    dc->SetFont( m_view->GetCommentFont() );
    //MultilineTextGraph *comment = m_view->GetTextGraph(m_brick, 0);
    comment.CalcMinSize(dc);

    wxCoord w=0, h=0;
    if ( m_view->IsDrawingComment() )
    {
        w = comment.GetWidth();
        h = comment.GetTotalHeight();
    }

    m_minimumsize.x = 2*w + 4*dc->GetCharWidth() + h/2;
    m_minimumsize.y = h + 2*dc->GetCharHeight();

    if ( size.x < m_minimumsize.x ) size.x = m_minimumsize.x;
    size.y += m_minimumsize.y ;

    NassiBrick *next = m_brick->GetNext();
    GraphNassiBrick *gnext = this->GetGraphBrick(next);
    if ( gnext )
    {
        gnext->CalcMinSize(dc, size);
        size.y--;
    }

}
TextGraph *GraphNassiBreakBrick::IsOverText(const wxPoint &pos)
{
    if ( !m_visible ) return 0;
    if ( m_view->IsDrawingComment() && comment.HasPoint(pos) )
        return &comment;

    return 0;
}






GraphNassiContinueBrick::GraphNassiContinueBrick(NassiView *view, NassiBrick *brick, BricksMap *bmap):
    GraphNassiBrick(view, brick, bmap),
    comment(view, brick, 0),
    m_h(0)
{
    //m_view->CreateTextGraph(m_brick, 0);
}
GraphNassiContinueBrick::~GraphNassiContinueBrick(){}
void GraphNassiContinueBrick::Draw(wxDC *dc)
{
    if ( !m_visible ) return;
    GraphNassiBrick::Draw(dc);
    dc->DrawRectangle(m_offset.x,  m_offset.y, m_size.x, m_size.y);
    dc->DrawLine(m_offset.x + m_h, m_offset.y,
                 m_offset.x,       m_offset.y + m_size.y/2);
    dc->DrawLine(m_offset.x,       m_offset.y + m_size.y/2,
                 m_offset.x + m_h, m_offset.y + m_size.y);

    if ( m_view->IsDrawingComment() )
    {
        dc->SetFont(m_view->GetCommentFont());
        //MultilineTextGraph *comment = m_view->GetTextGraph(m_brick, 0);
        comment.Draw( dc );
    }
}
void GraphNassiContinueBrick::SetOffsetAndSize(wxDC *dc, wxPoint offset, wxPoint size)
{
    if ( !m_visible ) return;
    if ( m_brick->GetNext() )
        m_size.y = GetMinimumHeight();
    else
        m_size.y = size.y;
    m_size.x = size.x;
    m_offset = offset;

    wxCoord dx = dc->GetCharWidth(),
            dy = dc->GetCharHeight(),
            h = 0;

    if ( m_view->IsDrawingComment() )
    {
        //MultilineTextGraph *comment = m_view->GetTextGraph(m_brick, 0);
        h = comment.GetTotalHeight();
    }

    m_h = h/2 + dy;
    if ( m_view->IsDrawingComment() )
    {
        //MultilineTextGraph *comment = m_view->GetTextGraph(m_brick, 0);
        comment.SetOffset( wxPoint(m_offset.x + dx + h/2, m_offset.y + m_size.y/2 - h/2 ) );
    }

    offset.y += ( m_size.y - 1 );
    size.y -= ( m_size.y - 1);

    GraphNassiBrick *nextgbrick = this->GetGraphBrick( m_brick->GetNext() );
    if ( nextgbrick )
        nextgbrick->SetOffsetAndSize(dc, offset, size);
}
void GraphNassiContinueBrick::CalcMinSize(wxDC *dc, wxPoint &size)
{
    dc->SetFont( m_view->GetCommentFont() );
    //MultilineTextGraph *comment = m_view->GetTextGraph(m_brick, 0);
    comment.CalcMinSize(dc);

    wxCoord w=0, h=0;
    if ( m_view->IsDrawingComment() )
    {
        w = comment.GetWidth();
        h = comment.GetTotalHeight();
    }


    m_minimumsize.x = 2*w + 4*dc->GetCharWidth() + h/2;
    m_minimumsize.y = h + 2*dc->GetCharHeight();

    if ( size.x < m_minimumsize.x )
        size.x = m_minimumsize.x;
    size.y += m_minimumsize.y;

    NassiBrick *next = m_brick->GetNext();
    GraphNassiBrick *gnext = this->GetGraphBrick(next);
    if ( gnext )
    {
        gnext->CalcMinSize(dc, size);
        size.y--;
    }
}
TextGraph *GraphNassiContinueBrick::IsOverText(const wxPoint &pos)
{
    if ( !m_visible ) return 0;
    if ( m_view->IsDrawingComment() && comment.HasPoint(pos) )
        return &comment;

    return 0;
}





GraphNassiReturnBrick::GraphNassiReturnBrick(NassiView *view, NassiBrick *brick, BricksMap *bmap):
    GraphNassiBrick(view, brick, bmap),
    comment(view, brick, 0),
    source(view, brick, 1),
    m_h(0)
{
//    m_view->CreateTextGraph(m_brick, 0);
//    m_view->CreateTextGraph(m_brick, 1);
}
GraphNassiReturnBrick::~GraphNassiReturnBrick(){}
void GraphNassiReturnBrick::Draw(wxDC *dc)
{
    if ( !m_visible ) return;
    GraphNassiBrick::Draw(dc);
    dc->DrawRectangle(m_offset.x, m_offset.y, m_size.x, m_size.y);
    dc->DrawLine(m_offset.x + m_h,                m_offset.y,
                 m_offset.x,                      m_offset.y + m_size.y/2);
    dc->DrawLine(m_offset.x,                      m_offset.y + m_size.y/2,
                 m_offset.x + m_h,                m_offset.y + m_size.y);
    dc->DrawLine(m_offset.x + m_size.x - m_h - 1, m_offset.y,
                 m_offset.x + m_size.x - 1,       m_offset.y + m_size.y/2);
    dc->DrawLine(m_offset.x + m_size.x - 1,       m_offset.y + m_size.y/2,
                 m_offset.x + m_size.x - m_h - 1, m_offset.y + m_size.y);

    if ( m_view->IsDrawingComment() )
    {
        dc->SetFont(m_view->GetCommentFont());
        //MultilineTextGraph *comment = m_view->GetTextGraph(m_brick, 0);
        comment.Draw( dc );
    }
    if ( m_view->IsDrawingSource() )
    {
        dc->SetFont(m_view->GetSourceFont());
        //MultilineTextGraph *source = m_view->GetTextGraph(m_brick, 1);
        source.Draw( dc );
    }
}
void GraphNassiReturnBrick::SetOffsetAndSize(wxDC *dc, wxPoint offset, wxPoint size)
{
    if ( !m_visible ) return;
    if ( m_brick->GetNext() )
        m_size.y = GetMinimumHeight();
    else
        m_size.y = size.y;
    m_size.x = size.x;
    m_offset = offset;

    wxCoord dx = dc->GetCharWidth(),
            dy = dc->GetCharHeight(),
            h = 0;

    //MultilineTextGraph *source = m_view->GetTextGraph(m_brick, 0);
    //MultilineTextGraph *comment = m_view->GetTextGraph(m_brick, 1);
    if ( m_view->IsDrawingComment() )
        h = comment.GetTotalHeight();
    if ( m_view->IsDrawingSource() )
    {
        if ( m_view->IsDrawingComment() )
            h += dy;
        h += source.GetTotalHeight();
    }

    m_h = h/2 + dy;


    if ( m_view->IsDrawingComment() )
    {
        comment.SetOffset( wxPoint(m_offset.x + dx + h/2, m_offset.y + m_size.y/2 - h/2 ) );
    }
    if ( m_view->IsDrawingSource() )
    {
        wxCoord d = 0;
        if ( m_view->IsDrawingComment() )
            d += dy + comment.GetTotalHeight();
        source.SetOffset( wxPoint(m_offset.x + dx + h/2, m_offset.y + m_size.y/2 - h/2 + d) );
    }

    offset.y += ( m_size.y - 1 );
    size.y -= ( m_size.y - 1);

    GraphNassiBrick *nextgbrick = this->GetGraphBrick( m_brick->GetNext() );
    if ( nextgbrick )
        nextgbrick->SetOffsetAndSize(dc, offset, size);
}
void GraphNassiReturnBrick::CalcMinSize(wxDC *dc, wxPoint &size)
{
    dc->SetFont( m_view->GetCommentFont() );
    comment.CalcMinSize(dc);

    dc->SetFont( m_view->GetSourceFont() );
    source.CalcMinSize(dc);

    wxCoord w=0, h=0;
    if ( m_view->IsDrawingComment() )
    {
        w = comment.GetWidth();
        h = comment.GetTotalHeight();
    }

    if ( m_view->IsDrawingSource() )
    {
        if (m_view->IsDrawingComment() )
            h += dc->GetCharHeight();
        h += source.GetTotalHeight();
        if ( w < static_cast<wxCoord>(source.GetWidth()) )
            w = source.GetWidth();
    }

    m_minimumsize.x = w + 6*dc->GetCharWidth() + h;
    m_minimumsize.y = h + 2*dc->GetCharHeight();

    if ( size.x < m_minimumsize.x )
        size.x = m_minimumsize.x;
    size.y += m_minimumsize.y;

    NassiBrick *next = m_brick->GetNext();
    GraphNassiBrick *gnext = this->GetGraphBrick(next);
    if ( gnext )
    {
        gnext->CalcMinSize(dc, size);
        size.y--;
    }
}
TextGraph *GraphNassiReturnBrick::IsOverText(const wxPoint &pos)
{
    if ( !m_visible ) return 0;
    if (m_view->IsDrawingComment() && comment.HasPoint(pos))
        return &comment;
    if (m_view->IsDrawingSource() && source.HasPoint(pos))
        return &source;

    return 0;
}




/// /////////////////////////////////////////////////////////////////////////

GraphNassiIfBrick::GraphNassiIfBrick(NassiView *view, NassiBrick *brick, BricksMap *bmap):
    GraphNassiMinimizableBrick(view, brick, bmap),
    commentHead(view, brick, 0 ),
    commentTrue(view, brick, 2 ),
    commentFalse(view, brick, 4 ),
    source(view, brick, 1 ),
    m_p(0),
    m_hh(0)
{}
GraphNassiIfBrick::~GraphNassiIfBrick(){}
void GraphNassiIfBrick::Draw(wxDC *dc)
{
    if ( !m_visible ) return;
    GraphNassiBrick::Draw(dc);
    if ( this->IsMinimized() )
    {
        dc->DrawRectangle(m_offset.x, m_offset.y, m_size.x, m_size.y);
        if ( m_view->IsDrawingComment() )
        {
            dc->SetFont(m_view->GetCommentFont());
            commentHead.Draw( dc );
        }
        dc->DrawBitmap(
            wxBitmap(iftool16_xpm),
            m_offset.x + m_size.x - 18,
            m_offset.y + 1,
            true);
    }
    else
    {
        /// draw the head |V|
        dc->DrawRectangle(m_offset.x, m_offset.y, m_size.x, m_hh);
        dc->DrawLine(m_offset.x,                m_offset.y, m_offset.x + m_p, m_offset.y + m_hh - 1);
        dc->DrawLine(m_offset.x + m_size.x - 1, m_offset.y, m_offset.x + m_p, m_offset.y + m_hh - 1);

        if ( m_view->IsDrawingComment() )
        {
            dc->SetFont( m_view->GetCommentFont() );
            commentHead.Draw( dc );
            commentTrue.Draw( dc );
            commentFalse.Draw( dc );
        }
        if ( m_view->IsDrawingSource() )
        {
            dc->SetFont( m_view->GetSourceFont() );
            source.Draw( dc );
        }

        /// draw the true brick
        NassiBrick *childt = m_brick->GetChild(0);
        GraphNassiBrick *gchildt = this->GetGraphBrick(childt);
        if ( !gchildt )
        {
            dc->SetBrush(*wxLIGHT_GREY_BRUSH);
            dc->DrawRectangle(m_offset.x, m_offset.y + m_hh - 1,
                                m_p + 1,  m_size.y   - m_hh + 1 );
            dc->SetBrush(*wxWHITE_BRUSH);
        }

        /// draw the false brick
        NassiBrick *childf = m_brick->GetChild(1);
        GraphNassiBrick *gchildf = this->GetGraphBrick(childf);
        if ( !gchildf )
        {
            dc->SetBrush(*wxLIGHT_GREY_BRUSH);
            dc->DrawRectangle(m_offset.x + m_p, m_offset.y + m_hh - 1,
                              m_size.x - m_p,   m_size.y - m_hh + 1 );
            dc->SetBrush(*wxWHITE_BRUSH);
        }
    }

    this->DrawMinMaxBox(dc); // the box with + or -
}
void GraphNassiIfBrick::SetOffsetAndSize(wxDC *dc, wxPoint offset, wxPoint size)
{
    if ( !m_visible ) return;
    if ( m_brick->GetNext() )
        m_size.y = GetMinimumHeight();
    else
        m_size.y = size.y;
    m_size.x = size.x;
    m_offset = offset;

    wxCoord dx = dc->GetCharWidth(),
            dy = dc->GetCharHeight();

    if ( this->IsMinimized() )
    {
        if ( m_view->IsDrawingComment() )
            commentHead.SetOffset( wxPoint(m_offset.x + dx, m_offset.y + dy + 10) );
    }
    else
    {
        wxCoord d = 0;
        wxCoord w = 0;
        if ( m_view->IsDrawingComment() )
            w = commentHead.GetWidth();
        if ( m_view->IsDrawingSource() )
            if ( w < static_cast<wxCoord>(source.GetWidth()) )
                w = source.GetWidth();

        if ( m_view->IsDrawingComment() )
        {
            commentHead.SetOffset( wxPoint( m_offset.x + m_p - w/2, m_offset.y + dy)  );
            d = commentHead.GetTotalHeight();
            d += dy;
        }
        if ( m_view->IsDrawingSource() )
            source.SetOffset( wxPoint( m_offset.x + m_p - w/2, m_offset.y + dy + d) );

        if ( m_view->IsDrawingComment() )
        {
            commentTrue.SetOffset(
                wxPoint( m_offset.x + dx,
                         m_offset.y + m_hh - dy - commentTrue.GetTotalHeight() )
            );
            commentFalse.SetOffset(
                wxPoint( m_offset.x + m_size.x - dx - commentFalse.GetWidth() ,
                         m_offset.y + m_hh - dy - commentFalse.GetTotalHeight())
            );
        }

        /// draw the true brick
        GraphNassiBrick *gchildt = this->GetGraphBrick(m_brick->GetChild(0));
        if ( gchildt )
            gchildt->SetOffsetAndSize(dc,
                wxPoint(m_offset.x, m_offset.y + m_hh - 1 ),
                wxPoint(m_p + 1,    m_size.y   - m_hh + 1));

        /// draw the false brick
        GraphNassiBrick *gchildf = this->GetGraphBrick(m_brick->GetChild(1));
        if ( gchildf )
            gchildf->SetOffsetAndSize(dc,
                wxPoint(m_offset.x + m_p, m_offset.y + m_hh - 1),
                wxPoint(m_size.x - m_p,   m_size.y - m_hh + 1));
    }

    offset.y += ( m_size.y - 1 );
    size.y -= ( m_size.y - 1);

    GraphNassiBrick *nextgbrick = this->GetGraphBrick( m_brick->GetNext() );
    if ( nextgbrick )
        nextgbrick->SetOffsetAndSize(dc, offset, size);
}
void GraphNassiIfBrick::CalcMinSize(wxDC *dc, wxPoint &size)
{
    dc->SetFont(m_view->GetCommentFont() );
    commentHead.CalcMinSize(dc);
    commentTrue.CalcMinSize(dc);
    commentFalse.CalcMinSize(dc);

    dc->SetFont(m_view->GetSourceFont() );
    source.CalcMinSize(dc);

    for (wxInt32 n = 0; n < 2 ; n++)
    {
        NassiBrick *child;
        ///set child invisible, if minimized
        if ( (child = m_brick->GetChild(n)) )
        {
            GraphNassiBrick *childgbrick = this->GetGraphBrick(child);
            if ( childgbrick )
                childgbrick->SetInvisible( !(this->IsMinimized()) );
        }
    }

    wxCoord w, h, p, hh;
    if ( this->IsMinimized() )
    {
        h = 2 * dc->GetCharHeight();
        w = 2 * dc->GetCharWidth();
        if ( m_view->IsDrawingComment() )
        {
            h += commentHead.GetTotalHeight();
            w += commentHead.GetWidth();
        }

        h += 10; //MinMaxBox
        w += 18; // the symbol
        p = 0;
        hh = 0;
    }
    else
    {
        wxCoord dx = dc->GetCharWidth(),
                dy = dc->GetCharHeight();
        wxCoord b0 = 0, b1 = 0 , b2 = 0, b3 = 8 * dx, b4 = 8 * dx;
        wxCoord h0 = 0, h1 = 0 , h2 = 0, h3 = 4 * dy, h4 = 4 * dy;

        if ( m_view->IsDrawingComment() )
        {
            b0 = commentHead.GetWidth();
            h0 = commentHead.GetTotalHeight();
        }
        if ( m_view->IsDrawingSource() )
        {
            wxCoord a=0, d=0;
            a = source.GetWidth();
            d = source.GetTotalHeight();
            h0 += d;
            if (m_view->IsDrawingComment() )
                h0 += dx;
            if ( b0 < a )
                b0 = a;
        }
        b0 += 2*dx;
        h0 += 2*dy;


        if ( m_view->IsDrawingComment() )
        {
            b1 = commentTrue.GetWidth();
            h1 = commentTrue.GetTotalHeight();
        }
        b1 += 2*dx;
        h1 += 2*dy;

        if ( m_view->IsDrawingComment() )
        {
            b2 = commentFalse.GetWidth();
            h2 = commentFalse.GetTotalHeight();
        }
        b2 += 2*dx;
        h2 += 2*dy;

        GraphNassiBrick *gtrue = this->GetGraphBrick(m_brick->GetChild(0));

        GraphNassiBrick *gfalse = this->GetGraphBrick(m_brick->GetChild(1));
        if ( gtrue ) // true-part
        {
            wxPoint size(0,0);
            gtrue->CalcMinSize(dc, size);
            b3 = size.x;
            h3 = size.y;
        }

        if ( gfalse ) //false-part
        {
            wxPoint size(0,0);
            gfalse->CalcMinSize(dc, size);
            b4 = size.x;
            h4 = size.y;
        }

        wxCoord q = 0 ; // THIS WAS NOT INITIALIZED ; KILLERBOT : I put it to 0 , is that a good value
        if ( h1 > h2 )
        {
            hh = h0 + h1;
            p = hh / h1 * b1;
            if ( p < b0/2+b1 )
                p = b0/2 + b1;
            if ( q < b3 )
                q = b3;

            q = b2 * hh/(hh-h2);
            if ( q < b2 + b0/2 )
                q = b2 + b0/2;
            if ( q < b4 )
                q = b4;
        }
        else
        { // h2 > h1;
            hh = h0 + h2;
            q = hh / h2 * b2;
            if ( q < b0/2+b2 )
                q = b0/2 + b2;
            if ( q < b4 )
                q = b4;

            p = b1 * hh/(hh-h1);
            if ( p < b1 +b0/2 )
                p = b1 + b0/2;
            if ( p < b3 )
                p = b3;
        }

        w = p+q-1;
        if ( h3 > h4 )
            h = h3 + hh;
        else
            h = h4 + hh;
        h--;
    }

    m_minimumsize.x = w;
    m_minimumsize.y = h;

    m_p = p;
    m_hh = hh;

    if ( size.x < m_minimumsize.x )
        size.x = m_minimumsize.x;
    size.y += m_minimumsize.y;

    NassiBrick *next = m_brick->GetNext();
    GraphNassiBrick *gnext = this->GetGraphBrick(next);
    if ( gnext )
    {
        gnext->CalcMinSize(dc, size);
        size.y--;
    }

}
bool GraphNassiIfBrick::HasPoint(const wxPoint &pos)
{
    if (!IsVisible() ) return false;

    if ( IsMinimized() ) return GraphNassiBrick::HasPoint(pos);
    if ( GraphNassiBrick::HasPoint(pos) )
    {
        wxInt8 region = 0;

        if ( pos.y < m_offset.y + m_hh ||  // over the head |V|
            ( pos.y > m_offset.y + m_hh + 10 && // minmaxbox
              pos.x > m_offset.x + m_p - 10 && pos.x < m_offset.x + m_p + 10 )) // some place between both childs
                    return true;//region = 0;
        if ( pos.x < m_offset.x + m_p )
            region = 1;
        else
            region = 2;

        if ( ! m_brick->GetChild(region - 1) ) // we own placeholder for a child
            return true;
    }
    return false;
}
TextGraph *GraphNassiIfBrick::IsOverText(const wxPoint &pos)
{
    if ( !m_visible ) return 0;
    if ( this->IsMinimized() )
    {
        if ( m_view->IsDrawingComment() && commentHead.HasPoint(pos))
            return &commentHead;
        else
            return 0;
    }
    if ( m_view->IsDrawingComment() )
    {
        if ( commentHead.HasPoint(pos) )
            return &commentHead;
        if ( commentTrue.HasPoint(pos) )
            return &commentTrue;
        if ( commentFalse.HasPoint(pos) )
            return &commentFalse;
    }
    if (  m_view->IsDrawingSource() && source.HasPoint(pos) )
        return &source;

    return 0;
}
bool GraphNassiIfBrick::IsOverChild(const wxPoint &pos, wxRect *childRect, wxUint32 *childNumber)
{
    if ( !m_visible || IsMinimized() ) return false;

    wxRect rect[2];
    rect[0].x = m_offset.x;
    rect[0].y = m_offset.y + m_hh;
    rect[0].width = m_p;
    rect[0].height = m_size.y - m_hh;

    rect[1].x = m_offset.x + m_p;
    rect[1].y = m_offset.y + m_hh;
    rect[1].width = m_size.x - m_p;
    rect[1].height = m_size.y - m_hh;

    if ( pos.x > m_offset.x + m_p - 10 && pos.x < m_offset.x + m_p + 10 )
        return false;

    if ( !m_brick->GetChild(0) && rect[0].Contains(pos) )
    {
        if ( childRect )
            *childRect = rect[0];
        if ( childNumber )
            *childNumber = 0;
        return true;
    }
    if ( !m_brick->GetChild(1) && rect[1].Contains(pos) )
    {
        if ( childRect )
            *childRect = rect[1];
        if ( childNumber )
            *childNumber = 1;
        return true;
    }
    return false;
}
GraphNassiBrick::Position GraphNassiIfBrick::GetPosition(const wxPoint &pos)
{
    GraphNassiBrick::Position res;

    if ( IsActive() || !HasPoint(pos) )
    {
        res.pos = Position::none;
        return res;
    }

    wxRect rect;
    wxUint32 childnumber;
    if ( IsOverChild(pos, &rect, &childnumber) )
    {
        res.pos = Position::child;
        res.number = childnumber;
    }
    else
    {
        res.pos = Position::top;
        if ( 2*pos.y > 2*m_offset.y + m_hh )
            res.pos = Position::bottom;
    }
    return res;
}




GraphNassiWhileBrick::GraphNassiWhileBrick(NassiView *view, NassiBrick *brick, BricksMap *bmap):
    GraphNassiMinimizableBrick(view, brick, bmap),
    comment(view, brick, 0 ),
    source(view, brick, 1 ),
    m_hh(0),
    m_bb(0)
{}
GraphNassiWhileBrick::~GraphNassiWhileBrick(){}
void GraphNassiWhileBrick::Draw(wxDC *dc)
{
    if ( !m_visible ) return;
    GraphNassiBrick::Draw(dc);
    if ( this->IsMinimized() )
    {
        dc->DrawRectangle(m_offset.x, m_offset.y, m_size.x, m_size.y);
        if ( m_view->IsDrawingComment() )
        {
            dc->SetFont(m_view->GetCommentFont());
            comment.Draw( dc );
        }
        dc->DrawBitmap(
            wxBitmap(whiletool16_xpm),
            m_offset.x + m_size.x - 18,
            m_offset.y + 1,
            true);
    }
    else
    {
        wxPoint points[6];
        points[0] = wxPoint(m_offset.x               , m_offset.y);
        points[1] = wxPoint(m_offset.x               , m_offset.y + m_size.y - 1);
        points[2] = wxPoint(m_offset.x + m_bb        , m_offset.y + m_size.y - 1);
        points[3] = wxPoint(m_offset.x + m_bb        , m_offset.y + m_hh);
        points[4] = wxPoint(m_offset.x + m_size.x - 1, m_offset.y + m_hh);
        points[5] = wxPoint(m_offset.x + m_size.x - 1, m_offset.y);

        dc->DrawPolygon(6, points);

        if ( m_view->IsDrawingComment() )
        {
            dc->SetFont( m_view->GetCommentFont() );
            comment.Draw(dc);
        }
        if ( m_view->IsDrawingSource() )
        {
            dc->SetFont( m_view->GetSourceFont() );
            source.Draw(dc);
        }

        NassiBrick *child = m_brick->GetChild();
        GraphNassiBrick *gchild = this->GetGraphBrick(child);
        if ( !gchild )
        {
            dc->SetBrush(*wxLIGHT_GREY_BRUSH);
            dc->DrawRectangle(m_offset.x + m_bb, m_offset.y + m_hh,
                                m_size.x - m_bb, m_size.y - m_hh);
            dc->SetBrush(*wxWHITE_BRUSH);
        }
    }

    this->DrawMinMaxBox(dc); // the box with + or -
}
void GraphNassiWhileBrick::SetOffsetAndSize(wxDC *dc, wxPoint offset, wxPoint size)
{
    if ( !m_visible ) return;
    if ( m_brick->GetNext() )
        m_size.y = GetMinimumHeight();
    else
        m_size.y = size.y;
    m_size.x = size.x;
    m_offset = offset;

    wxCoord dx = dc->GetCharWidth(),
            dy = dc->GetCharHeight();

    if ( this->IsMinimized() )
    {
        if ( m_view->IsDrawingComment() )
            comment.SetOffset( wxPoint(m_offset.x + dx, m_offset.y + dy + 10) );
    }
    else
    {
        if ( m_view->IsDrawingComment() )
            comment.SetOffset(wxPoint( m_offset.x + dx,  m_offset.y + dy + 10));
        if ( m_view->IsDrawingSource() )
        {

            source.SetOffset(
                    wxPoint(m_offset.x + dx,
                        m_offset.y + m_hh - dy - source.GetTotalHeight() ) );
        }

        GraphNassiBrick *gchild = this->GetGraphBrick(m_brick->GetChild());
        if ( gchild )
            gchild->SetOffsetAndSize(dc,
                    wxPoint(m_offset.x + m_bb, m_offset.y + m_hh),
                    wxPoint(m_size.x - m_bb,   m_size.y - m_hh));
    }

    offset.y += ( m_size.y - 1 );
    size.y -= ( m_size.y - 1);

    GraphNassiBrick *nextgbrick = this->GetGraphBrick( m_brick->GetNext() );
    if ( nextgbrick )
        nextgbrick->SetOffsetAndSize(dc, offset, size);
}
void GraphNassiWhileBrick::CalcMinSize(wxDC *dc, wxPoint &size)
{
    dc->SetFont(m_view->GetCommentFont() );
    comment.CalcMinSize(dc);
    dc->SetFont(m_view->GetSourceFont() );
    source.CalcMinSize(dc);


    ///set child invisible, if minimized
    NassiBrick *child = m_brick->GetChild();
    GraphNassiBrick *childgbrick = this->GetGraphBrick(child);
    if ( childgbrick )
        childgbrick->SetInvisible( !(this->IsMinimized()) );


    wxCoord dx = dc->GetCharWidth(),
            dy = dc->GetCharHeight();

    wxCoord w, h, hh, bb;
    if ( this->IsMinimized() )
    {
        h = 2*dy;
        w = 2*dx;
        if ( m_view->IsDrawingComment() )
        {
            h += comment.GetTotalHeight();
            w += comment.GetWidth();
        }

        h += 10;  //MinMaxBox
        w += 18; // the symbol
        hh = 0;
        bb = 0;
    }
    else
    {
        w = 0;
        h = 2 * dy;
        if ( m_view->IsDrawingComment() )
        {
            h += comment.GetTotalHeight();
            w += comment.GetWidth();
        }
        if ( m_view->IsDrawingSource() )
        {
            h += source.GetTotalHeight();
            if ( m_view->IsDrawingComment() )
                h += dy;
            if ( w < static_cast<wxCoord>(source.GetWidth()) )
                w = source.GetWidth();
        }
        w += 2*dx;
        h += 10; //MinMaxBox
        h--;
        hh = h;

        if ( !childgbrick ) // no child
        {
            h += 4 * dy ;
            if ( w < (8 + 3) * dx )
                w = (8 + 3) * dx;
        }
        else
        {
            wxPoint size(0,0);
            childgbrick->CalcMinSize(dc, size);
            h += size.y;
            if ( w < 3*dx + size.x)
                w = 3*dx + size.x;
        }
        bb = 3*dx;
    }

    m_bb = bb;
    m_hh = hh;

    m_minimumsize.x = w;
    m_minimumsize.y = h;

    if ( size.x < m_minimumsize.x )
        size.x = m_minimumsize.x;
    size.y += m_minimumsize.y;

    NassiBrick *next = m_brick->GetNext();
    GraphNassiBrick *gnext = this->GetGraphBrick(next);
    if ( gnext )
    {
        gnext->CalcMinSize(dc, size);
        size.y--;// thickness of the line between bricks is only 1
    }
}
bool GraphNassiWhileBrick::HasPoint(const wxPoint &pos)
{
    if (!IsVisible() ) return false;

    if ( IsMinimized() ) return GraphNassiBrick::HasPoint(pos);
    if ( GraphNassiBrick::HasPoint(pos) )
    {
        if ( !m_brick->GetChild() ||
             pos.x < m_offset.x + m_bb ||
             pos.y < m_offset.y + m_hh)
            return true;
    }
    return false;
}
TextGraph *GraphNassiWhileBrick::IsOverText(const wxPoint &pos)
{
    if ( !m_visible ) return 0;
    if ( this->IsMinimized() )
    {
        if ( m_view->IsDrawingComment() && comment.HasPoint(pos) )
            return &comment;
        else
            return 0;
    }
    if ( m_view->IsDrawingComment() && comment.HasPoint(pos) )
        return &comment;
    if ( m_view->IsDrawingSource() && source.HasPoint(pos) )
        return &source;

    return 0;
}
bool GraphNassiWhileBrick::IsOverChild(const wxPoint &pos, wxRect *childRect, wxUint32 *childNumber)
{
    if ( !m_visible || IsMinimized() ) return false;

    wxRect rect;
    rect.x      = m_offset.x + m_bb;
    rect.y      = m_offset.y + m_hh;
    rect.width  = m_size.x - m_bb;
    rect.height = m_size.y - m_hh;

    if ( !m_brick->GetChild(0) && rect.Contains(pos))
    {
        if ( childRect )
            *childRect = rect;
        if ( childNumber )
            *childNumber = 0;
        return true;
    }
    return false;
}


GraphNassiDoWhileBrick::GraphNassiDoWhileBrick(NassiView *view, NassiBrick *brick, BricksMap *bmap):
    GraphNassiMinimizableBrick(view, brick, bmap),
    comment(view, brick, 0),
    source(view, brick, 1),
    m_bb(0),
    m_hh(0)
{}
GraphNassiDoWhileBrick::~GraphNassiDoWhileBrick(){}
void GraphNassiDoWhileBrick::Draw(wxDC *dc)
{
    if ( !m_visible ) return;
    GraphNassiBrick::Draw(dc);
    if ( this->IsMinimized() )
    {
        dc->DrawRectangle(m_offset.x, m_offset.y, m_size.x, m_size.y);
        if ( m_view->IsDrawingComment() )
        {
            dc->SetFont(m_view->GetCommentFont());
            comment.Draw( dc );
        }
        dc->DrawBitmap(
            wxBitmap(dowhiletool16_xpm),
            m_offset.x + m_size.x - 18,
            m_offset.y + 1,
            true);
    }
    else
    {
        wxPoint points[6];
        points[0] = wxPoint(m_offset.x               , m_offset.y);
        points[1] = wxPoint(m_offset.x               , m_offset.y + m_size.y - 1);
        points[2] = wxPoint(m_offset.x + m_size.x - 1, m_offset.y + m_size.y - 1);
        points[3] = wxPoint(m_offset.x + m_size.x - 1, m_offset.y + m_size.y - m_hh - 1);
        points[4] = wxPoint(m_offset.x + m_bb        , m_offset.y + m_size.y - m_hh - 1);
        points[5] = wxPoint(m_offset.x + m_bb        , m_offset.y);

        dc->DrawPolygon(6, points);

        if ( m_view->IsDrawingComment() )
        {
            dc->SetFont(m_view->GetCommentFont());
            comment.Draw( dc );
        }
        if ( m_view->IsDrawingSource() )
        {
            dc->SetFont( m_view->GetSourceFont() );
            source.Draw( dc );
        }

        GraphNassiBrick *gchild = this->GetGraphBrick(m_brick->GetChild());
        if ( gchild )
        {
//            gchild->Draw(dc);
        }
        else
        {
            dc->SetBrush(*wxLIGHT_GREY_BRUSH);
            dc->DrawRectangle(m_offset.x + m_bb, m_offset.y ,
                                m_size.x - m_bb, m_size.y - m_hh);
            dc->SetBrush(*wxWHITE_BRUSH);
        }
    }

    this->DrawMinMaxBox(dc); // the box with + or -
}
void GraphNassiDoWhileBrick::SetOffsetAndSize(wxDC *dc, wxPoint offset, wxPoint size)
{
    if ( !m_visible ) return;
    if ( m_brick->GetNext() )
        m_size.y = GetMinimumHeight();
    else
        m_size.y = size.y;
    m_size.x = size.x;
    m_offset = offset;

    wxCoord dx = dc->GetCharWidth(),
            dy = dc->GetCharHeight();

    if ( this->IsMinimized() )
    {

        if ( m_view->IsDrawingComment() )
            comment.SetOffset( wxPoint(m_offset.x + dx, m_offset.y + dy + 10) );
    }
    else
    {
        if ( m_view->IsDrawingComment() )
            comment.SetOffset( wxPoint(m_offset.x + dx,
                                       m_offset.y + m_size.y - m_hh + dy ) );
        if ( m_view->IsDrawingSource() )
            source.SetOffset(wxPoint(m_offset.x + dx,
                        m_offset.y + m_size.y - dy - source.GetTotalHeight()));


        GraphNassiBrick *gchild = this->GetGraphBrick(m_brick->GetChild());
        if ( gchild )
            gchild->SetOffsetAndSize(dc,
                    wxPoint(m_offset.x + m_bb, m_offset.y),
                    wxPoint(m_size.x - m_bb,   m_size.y - m_hh));
    }

    offset.y += ( m_size.y - 1 );
    size.y -= ( m_size.y - 2);

    GraphNassiBrick *nextgbrick = this->GetGraphBrick( m_brick->GetNext() );
    if ( nextgbrick )
        nextgbrick->SetOffsetAndSize(dc, offset, size);

}
void GraphNassiDoWhileBrick::CalcMinSize(wxDC *dc, wxPoint &size)
{
    dc->SetFont(m_view->GetCommentFont() );
    comment.CalcMinSize(dc);
    dc->SetFont(m_view->GetSourceFont() );
    source.CalcMinSize(dc);


    ///set child invisible, if minimized
    NassiBrick *child = m_brick->GetChild();
    GraphNassiBrick *childgbrick = this->GetGraphBrick(child);
    if ( childgbrick )
        childgbrick->SetInvisible( !(this->IsMinimized()) );


    wxCoord dx = dc->GetCharWidth(),
            dy = dc->GetCharHeight();

    wxCoord w, h, bb;//, hh;
    if ( this->IsMinimized() )
    {
        h = 2*dy;
        w = 2*dx;
        if ( m_view->IsDrawingComment() )
        {
            h += comment.GetTotalHeight();
            w += comment.GetWidth();
        }

        h += 10;  //MinMaxBox
        w += 18; // the symbol
        bb = 0;
        //hh = 0;
        m_hh = 0;
    }
    else
    {
        w = 0;
        h = 2 * dy;
        if ( m_view->IsDrawingComment() )
        {
            h += comment.GetTotalHeight();
            w += comment.GetWidth();
        }
        if ( m_view->IsDrawingSource() )
        {
            if ( m_view->IsDrawingComment() )
                h += dy;
            h += source.GetTotalHeight();
            if ( w < static_cast<wxCoord>(source.GetWidth()) )
                w = source.GetWidth();
        }
        w += 2*dx;
        //hh = h;
        m_hh = h;

        if ( !childgbrick ) // no child
        {
            h += (4*dy) ;
            if ( w < (8 + 3)*dx )

                w = (8 + 3)*dx;
            //ch = 4*dy;
        }
        else
        {
            wxPoint chsize(0,0);
            childgbrick->CalcMinSize(dc, chsize);
            h += chsize.y;
            if ( w < 3*dx + chsize.x)
                w = 3*dx + chsize.x;
        }
        bb = 3*dx;

    }

    m_bb = bb;


    m_minimumsize.x = w;
    m_minimumsize.y = h+20;

    if ( size.x < m_minimumsize.x )
        size.x = m_minimumsize.x;
    size.y += m_minimumsize.y;

    NassiBrick *next = m_brick->GetNext();
    GraphNassiBrick *gnext = this->GetGraphBrick(next);
    if ( gnext )
    {
        gnext->CalcMinSize(dc, size);
        size.y--;// thickness of the line between bricks is only 1
    }
}
bool GraphNassiDoWhileBrick::HasPoint(const wxPoint &pos)
{
    if (!IsVisible() ) return false;

    if ( IsMinimized() ) return GraphNassiBrick::HasPoint(pos);
    if ( GraphNassiBrick::HasPoint(pos) )
    {
        if ( !m_brick->GetChild() ||
             pos.x < m_offset.x + m_bb ||
             pos.y > m_offset.y + m_size.y - m_hh)
            return true;
    }
    return false;
}
TextGraph *GraphNassiDoWhileBrick::IsOverText(const wxPoint &pos)
{
    if ( !m_visible ) return 0;
    if ( this->IsMinimized() )
    {
        if (m_view->IsDrawingComment() && comment.HasPoint(pos) )
            return &comment;
        else
            return 0;
    }
    if ( m_view->IsDrawingComment() && comment.HasPoint(pos) )
        return &comment;
    if ( m_view->IsDrawingSource() && source.HasPoint(pos))
        return &source;

    return 0;
}
bool GraphNassiDoWhileBrick::IsOverChild(const wxPoint &pos, wxRect *childRect, wxUint32 *childNumber)
{
    if ( !m_visible || IsMinimized() ) return false;

    wxRect rect;
    rect.x = m_offset.x + m_bb;
    rect.y = m_offset.y;
    rect.width = m_size.x - m_bb;
    rect.height = m_size.y - m_hh;

    if ( !m_brick->GetChild(0) && rect.Contains(pos) )
    {
        if ( childRect )
            *childRect = rect;
        if ( childNumber )
            *childNumber = 0;
        return true;
    }
    return false;
}



GraphNassiForBrick::GraphNassiForBrick(NassiView *view, NassiBrick *brick, BricksMap *bmap):
    GraphNassiMinimizableBrick(view, brick, bmap),
    comment(view, brick, 0),
    source(view, brick, 1),
    m_hh(0),
    m_bb(0),
    m_b(0)
{}
GraphNassiForBrick::~GraphNassiForBrick(){}
void GraphNassiForBrick::Draw(wxDC *dc)
{
    if ( !m_visible ) return;
    GraphNassiBrick::Draw(dc);
    if ( this->IsMinimized() )
    {
        dc->DrawRectangle(m_offset.x, m_offset.y, m_size.x, m_size.y);
        if ( m_view->IsDrawingComment() )
        {
            dc->SetFont(m_view->GetCommentFont());
            comment.Draw( dc );
        }
        dc->DrawBitmap(
            wxBitmap(fortool16_xpm),
            m_offset.x + m_size.x - 18,
            m_offset.y + 1,
            true);
    }
    else
    {
        wxPoint points[8];
        points[0] = wxPoint(m_offset.x               , m_offset.y);
        points[1] = wxPoint(m_offset.x               , m_offset.y + m_size.y - 1);
        points[2] = wxPoint(m_offset.x + m_size.x - 1, m_offset.y + m_size.y - 1);
        points[3] = wxPoint(m_offset.x + m_size.x - 1, m_offset.y + m_size.y - 1 - m_b);
        points[4] = wxPoint(m_offset.x + m_bb        , m_offset.y + m_size.y - 1 - m_b);
        points[5] = wxPoint(m_offset.x + m_bb        , m_offset.y + m_hh);
        points[6] = wxPoint(m_offset.x + m_size.x - 1, m_offset.y + m_hh);
        points[7] = wxPoint(m_offset.x + m_size.x - 1, m_offset.y);
        dc->DrawPolygon(8, points);

        if ( m_view->IsDrawingComment() )
        {
            dc->SetFont( m_view->GetCommentFont() );
            comment.Draw(dc);
        }
        if ( m_view->IsDrawingSource() )
        {
            dc->SetFont( m_view->GetSourceFont() );
            source.Draw(dc);
        }

        GraphNassiBrick *gchild = this->GetGraphBrick(m_brick->GetChild());
        if ( !gchild )
        {
            dc->SetBrush(*wxLIGHT_GREY_BRUSH);
            dc->DrawRectangle(m_offset.x + m_bb, m_offset.y + m_hh,
                                m_size.x - m_bb, m_size.y   - m_hh - m_b);
            dc->SetBrush(*wxWHITE_BRUSH);
        }
    }

    this->DrawMinMaxBox(dc); // the box with + or -
}
void GraphNassiForBrick::SetOffsetAndSize(wxDC *dc, wxPoint offset, wxPoint size)
{
    if ( !m_visible ) return;
    if ( m_brick->GetNext() )
        m_size.y = GetMinimumHeight();
    else
        m_size.y = size.y;
    m_size.x = size.x;
    m_offset = offset;

    wxCoord dx = dc->GetCharWidth(),
            dy = dc->GetCharHeight();

    if ( this->IsMinimized() )
    {
        if ( m_view->IsDrawingComment() )
            comment.SetOffset( wxPoint(m_offset.x + dx, m_offset.y + dy + 10) );
    }
    else
    {

        wxCoord d = 10;
        if ( m_view->IsDrawingComment() )
        {
            d += dy;
            comment.SetOffset( wxPoint(m_offset.x + dx, m_offset.y + d) );
            d += comment.GetTotalHeight();
        }
        if ( m_view->IsDrawingSource() )
        {
            d += dy;
            source.SetOffset( wxPoint( m_offset.x + dx, m_offset.y + d ));
        }

        GraphNassiBrick *gchild = this->GetGraphBrick(m_brick->GetChild());
        if ( gchild )
            gchild->SetOffsetAndSize(dc,
                    wxPoint(m_offset.x + m_bb, m_offset.y + m_hh),
                    wxPoint(m_size.x - m_bb,   m_size.y - m_hh - m_b));
    }

    offset.y += ( m_size.y - 1 );
    size.y -= ( m_size.y - 1);

    GraphNassiBrick *nextgbrick = this->GetGraphBrick( m_brick->GetNext() );
    if ( nextgbrick )
        nextgbrick->SetOffsetAndSize(dc, offset, size);
}
void GraphNassiForBrick::CalcMinSize(wxDC *dc, wxPoint &size)
{
    dc->SetFont(m_view->GetCommentFont() );
    comment.CalcMinSize(dc);
    dc->SetFont(m_view->GetSourceFont() );
    source.CalcMinSize(dc);


    ///set child invisible, if minimized
    NassiBrick *child = m_brick->GetChild();
    GraphNassiBrick *childgbrick = this->GetGraphBrick(child);
    if ( childgbrick )
        childgbrick->SetInvisible( !(this->IsMinimized()) );


    wxCoord dx = dc->GetCharWidth(),
            dy = dc->GetCharHeight();

    wxCoord w=0, h=0;
    if ( this->IsMinimized() )
    {
        h = 2*dy;
        w = 2*dx;
        if ( m_view->IsDrawingComment() )
        {
            h += comment.GetTotalHeight();
            w += comment.GetWidth();
        }

        h += 10;  //MinMaxBox
        w += 18; // the symbol
        m_hh = 0;
        m_bb = 0;
        m_b = 0;
    }
    else
    {
        h = 2*dy;
        if ( m_view->IsDrawingComment() )
        {
            h += comment.GetTotalHeight();
            w += comment.GetWidth();
        }
        if ( m_view->IsDrawingSource() )
        {
            if ( m_view->IsDrawingComment() )
                h += dy;
            h += source.GetTotalHeight();
            if ( w < static_cast<wxCoord>(source.GetWidth()) )
                w = source.GetWidth();
        }
        w += 2*dx;
        h += 10; //MinMaxBox
        h--;
        m_hh = h;
        h += 3*dx; //dy?

        if ( !childgbrick ) // no child
        {
            h += 4 * dy ;
            if ( w < (8 + 3) * dx )
                w = (8 + 3) * dx;
        }
        else
        {
            wxPoint size(0,0);
            childgbrick->CalcMinSize(dc, size);
            h += size.y;
            if ( w < 3*dx + size.x)
                w = 3*dx + size.x;
        }
        m_bb = 3*dx;
        m_b = 3*dx;
    }

    m_minimumsize.x = w;
    m_minimumsize.y = h;

    if ( size.x < m_minimumsize.x )
        size.x = m_minimumsize.x;
    size.y += m_minimumsize.y;

    NassiBrick *next = m_brick->GetNext();
    GraphNassiBrick *gnext = this->GetGraphBrick(next);
    if ( gnext )
    {
        gnext->CalcMinSize(dc, size);
        size.y--;// thickness of the line between bricks is only 1
    }
}
bool GraphNassiForBrick::HasPoint(const wxPoint &pos)
{
    if (!IsVisible() ) return false;

    if ( IsMinimized() ) return GraphNassiBrick::HasPoint(pos);
    if ( GraphNassiBrick::HasPoint(pos) )
    {
        if ( !m_brick->GetChild() ||
             pos.x < m_offset.x + m_bb ||
             pos.y < m_offset.y + m_hh ||
             pos.y > m_offset.y + m_size.y - m_b)
            return true;
    }
    return false;
}
TextGraph *GraphNassiForBrick::IsOverText(const wxPoint &pos)
{
    if ( !m_visible ) return 0;
    if ( this->IsMinimized() )
    {
        if (m_view->IsDrawingComment() && comment.HasPoint(pos) )
            return &comment;
        else
            return 0;
    }
    if ( m_view->IsDrawingComment() && comment.HasPoint(pos) )
        return &comment;
    if ( m_view->IsDrawingSource() && source.HasPoint(pos))
        return &source;

    return 0;
}
bool GraphNassiForBrick::IsOverChild(const wxPoint &pos, wxRect *childRect, wxUint32 *childNumber)
{
    if ( !m_visible || IsMinimized() ) return false;

    wxRect rect;
    rect.x = m_offset.x + m_bb;
    rect.y = m_offset.y + m_hh;
    rect.width = m_size.x - m_bb;
    rect.height = m_size.y - m_hh - m_b;

    if ( !m_brick->GetChild(0) && rect.Contains(pos) )
    {
        if ( childRect )
            *childRect = rect;
        if ( childNumber )
            *childNumber = 0;
        return true;
    }
    return false;
}






GraphNassiBlockBrick::GraphNassiBlockBrick(NassiView *view, NassiBrick *brick, BricksMap *bmap):
    GraphNassiMinimizableBrick(view, brick, bmap),
    m_hh(0)
{}
GraphNassiBlockBrick::~GraphNassiBlockBrick(){}
wxString GraphNassiBlockBrick::GetSource()
{
    return _T("{}");
}
void GraphNassiBlockBrick::Draw(wxDC *dc)
{
    if ( !m_visible ) return;
    GraphNassiBrick::Draw(dc);
    if ( this->IsMinimized() )
    {
        dc->DrawRectangle(m_offset.x, m_offset.y, m_size.x, m_size.y);
        if ( m_view->IsDrawingSource() )
        {
            dc->SetFont(m_view->GetSourceFont());
            dc->DrawText(this->GetSource(),
                m_offset.x + dc->GetCharWidth() + 10,
                m_offset.y + dc->GetCharHeight());
        }
        dc->DrawBitmap(
            wxBitmap(blocktool16_xpm),
            m_offset.x + m_size.x - 18,
            m_offset.y + 1,
            true);
    }
    else
    {
        wxBrush currentBrush = dc->GetBrush();
        wxPen currentPen = dc->GetPen();

        // frame around block:
        dc->SetPen( *wxWHITE_PEN );
        dc->DrawRectangle(m_offset.x,                m_offset.y,              m_size.x, m_hh);
        dc->DrawRectangle(m_offset.x,                m_offset.y,              3,        m_size.y);
        dc->DrawRectangle(m_offset.x,                m_offset.y + m_size.y-6, m_size.x, 6);
        dc->DrawRectangle(m_offset.x + m_size.x - 3, m_offset.y,              3,        m_size.y);
        dc->SetPen(currentPen);
        dc->SetBrush( *wxTRANSPARENT_BRUSH);
        dc->DrawRectangle(m_offset.x, m_offset.y, m_size.x, m_size.y);
        dc->SetBrush(currentBrush);

        if ( m_view->IsDrawingSource() )
        {
            dc->SetFont( m_view->GetSourceFont() );
            dc->DrawText(this->GetSource(),
                m_offset.x + dc->GetCharWidth() + 10,
                m_offset.y + dc->GetCharHeight());
        }

        GraphNassiBrick *gchild = this->GetGraphBrick(m_brick->GetChild());
        if ( !gchild )
        {
            dc->SetBrush(*wxLIGHT_GREY_BRUSH);
            dc->DrawRectangle(m_offset.x + 3, m_offset.y + m_hh ,
                              m_size.x - 6,   m_size.y - m_hh - 6);
            dc->SetBrush(*wxWHITE_BRUSH);
        }
    }

    this->DrawMinMaxBox(dc); // the box with + or -
}
void GraphNassiBlockBrick::SetOffsetAndSize(wxDC *dc, wxPoint offset, wxPoint size)
{
    if ( !m_visible ) return;
    if ( m_brick->GetNext() )
        m_size.y = GetMinimumHeight();
    else
        m_size.y = size.y;
    m_size.x = size.x;
    m_offset = offset;

    if (  !this->IsMinimized() )
    {
        GraphNassiBrick *gchild = this->GetGraphBrick(m_brick->GetChild());
        if ( gchild )
            gchild->SetOffsetAndSize(dc,
                    wxPoint(m_offset.x + 3, m_offset.y + m_hh),
                    wxPoint(m_size.x - 6,   m_size.y - m_hh - 6));
    }

    offset.y += ( m_size.y - 1 );
    size.y -= ( m_size.y - 1);

    GraphNassiBrick *nextgbrick = this->GetGraphBrick( m_brick->GetNext() );
    if ( nextgbrick )
        nextgbrick->SetOffsetAndSize(dc, offset, size);
}
void GraphNassiBlockBrick::CalcMinSize(wxDC *dc, wxPoint &size)
{
    ///set child invisible, if minimized
    NassiBrick *child = m_brick->GetChild();
    GraphNassiBrick *childgbrick = this->GetGraphBrick(child);
    if ( childgbrick )
        childgbrick->SetInvisible( !(this->IsMinimized()) );


    wxCoord dx = dc->GetCharWidth(),
            dy = dc->GetCharHeight();

    wxCoord w = 0, h = 0;
    if ( this->IsMinimized() )
    {
        h = 2*dy;
        w = 2*dx;
        if ( m_view->IsDrawingSource() )
        {
            wxCoord a, d;
            dc->SetFont( m_view->GetSourceFont() );
            dc->GetTextExtent(this->GetSource(), &a, &d);
            h += d;
            w += a;
        }

        h += 10;  //MinMaxBox
        w += 28; // 18: the symbol  10: minMaxBox
    }
    else
    {
        w = 2*dx; // h = 0;
        if ( m_view->IsDrawingSource() )
        {
            wxCoord a, d;
            dc->SetFont( m_view->GetSourceFont() );
            dc->GetTextExtent(this->GetSource(), &a, &d);
            h += d;
            w += a;
        }
        w += 10;      // 10: MinMaxBox
        w += 6;       // don't remeber for what
        if ( h < 10 ) // text smaller than MinMaxBox?
            h = 10;

        h += 2*dy;
        h--;          // for the first horizontal line of the childs
        m_hh = h;

        if ( !childgbrick ) // no child
        {
            h += 4*dy ;
            if ( w < 6*dx )
                w = 6*dx;
        }
        else
        {
            wxPoint size(0,0);
            childgbrick->CalcMinSize(dc, size);
            h += size.y;
            if ( w < 6 + size.x)
                w = 6 + size.x;
        }
    }

    m_minimumsize.x = w;
    m_minimumsize.y = h;

    if ( size.x < m_minimumsize.x )
        size.x = m_minimumsize.x;
    size.y += m_minimumsize.y;

    NassiBrick *next = m_brick->GetNext();
    GraphNassiBrick *gnext = this->GetGraphBrick(next);
    if ( gnext )
    {
        gnext->CalcMinSize(dc, size);
        size.y--;// thickness of the line between bricks is only 1
    }
}
bool GraphNassiBlockBrick::HasPoint(const wxPoint &pos)
{
    if (!IsVisible() ) return false;

    if ( IsMinimized() ) return GraphNassiBrick::HasPoint(pos);
    if ( GraphNassiBrick::HasPoint(pos) )
    {
        if ( !m_brick->GetChild() || // no child .. all pixels are ours
             pos.x < m_offset.x + 3 || // 3 at the left
             pos.y < m_offset.y + m_hh || // the head
             pos.y >= m_offset.y + m_size.y - 6|| // at the bottom
             pos.x >= m_offset.y + m_size.x - 3
            )
            return true;
    }
    return false;
}
TextGraph *GraphNassiBlockBrick::IsOverText(const wxPoint & /*pos*/)
{
    return 0;
}
bool GraphNassiBlockBrick::IsOverChild(const wxPoint &pos, wxRect *childRect, wxUint32 *childNumber)
{
    if ( !m_visible || IsMinimized() ) return false;

    wxRect rect;
    rect.x = m_offset.x + 3;
    rect.y = m_offset.y + m_hh;
    rect.width = m_size.x - 6;  // 3 on the left and 3 on the right
    rect.height = m_size.y - m_hh - 6; // the head and 6 at the bottom

    if ( !m_brick->GetChild(0) && rect.Contains(pos) )
    {
        if ( childRect )
            *childRect = rect;
        if ( childNumber )
            *childNumber = 0;
        return true;
    }
    return false;
}

GraphNassiSwitchBrick::GraphNassiSwitchBrick(NassiView *view, NassiBrick *brick, BricksMap *bmap):
    GraphNassiMinimizableBrick(view, brick, bmap),
    comment(view, brick, 0),
    source(view, brick, 1),
    childcomment(),
    childsource(),
    m_textMap(),
    minimumHeightOfChildBricks(),
    offsetToChild(),
    offsetToChildSeparator(),
    heightOfChildBricks(),
    m_b(0),
    m_hw(0),
    m_ChildIndicatorIsActive(false),
    m_ActiveChildIndicator(0)
{}
GraphNassiSwitchBrick::~GraphNassiSwitchBrick(){}
void GraphNassiSwitchBrick::Draw(wxDC *dc)
{
    if ( !m_visible ) return;
    GraphNassiBrick::Draw(dc);
    if ( this->IsMinimized() )
    {
        dc->DrawRectangle(m_offset.x, m_offset.y, m_size.x, m_size.y);
        if ( m_view->IsDrawingComment() )
        {
            dc->SetFont(m_view->GetCommentFont());
            comment.Draw( dc );
        }
        dc->DrawBitmap(
            wxBitmap(switchtool16_xpm),
            m_offset.x + m_size.x - 18,
            m_offset.y + 1,
            true);
    }
    else
    {
        if ( m_brick->GetChildCount() == 0 )
            dc->DrawRectangle(m_offset.x,    m_offset.y, m_size.x, m_size.y);
        else
            dc->DrawRectangle(m_offset.x,    m_offset.y, m_hw, m_size.y);
        dc->DrawLine(m_offset.x + m_b,   m_offset.y,
                     m_offset.x + m_b/2, m_offset.y + m_size.y);
        if ( m_view->IsDrawingComment() )
        {
            dc->SetFont(m_view->GetCommentFont());
            comment.Draw(dc);
            for ( wxUint32 i = 0 ; i < childcomment.size() ; i++ )
                childcomments(i)->Draw(dc);
        }
        if ( m_view->IsDrawingSource() )
        {
            dc->SetFont( m_view->GetSourceFont() );
            source.Draw(dc);
            for ( wxUint32 i = 0 ; i < childsource.size() ; i++ )
                childsources(i)->Draw(dc);
        }

        for ( wxUint32 i = 0 ; i < m_brick->GetChildCount() ; i++ )
        {
            dc->DrawLine( m_offset.x + offsetToChildSeparator[i] , m_offset.y + offsetToChild[i],
                          m_offset.x + m_hw, m_offset.y + offsetToChild[i]);

            GraphNassiBrick *gchild = this->GetGraphBrick(m_brick->GetChild(i));
            if ( !gchild )
            {
                dc->SetBrush(*wxLIGHT_GREY_BRUSH);
                dc->DrawRectangle(m_offset.x + m_hw-1, m_offset.y + offsetToChild[i],
                                    m_size.x - m_hw+1, heightOfChildBricks[i]);
                dc->SetBrush(*wxWHITE_BRUSH);
            }
        }
    }

    this->DrawMinMaxBox(dc); // the box with + or -
}
void GraphNassiSwitchBrick::SetOffsetAndSize(wxDC *dc, wxPoint offset, wxPoint size)
{
    if ( !m_visible ) return;
    if ( m_brick->GetNext() )
        m_size.y = GetMinimumHeight();
    else
        m_size.y = size.y;
    m_size.x = size.x;
    m_offset = offset;

    wxCoord dx = dc->GetCharWidth(),
            dy = dc->GetCharHeight();

    if ( this->IsMinimized() )
    {
        if ( m_view->IsDrawingComment() )
            comment.SetOffset( wxPoint(m_offset.x + dx, m_offset.y + dy + 10) );
    }
    else
    {
        wxCoord d = 10;
        if ( m_view->IsDrawingComment() )
        {
            d += dy;
            comment.SetOffset( wxPoint(m_offset.x + dx, m_offset.y + d) );
            d += comment.GetTotalHeight();
        }
        if ( m_view->IsDrawingSource() )
        {
            d += dy;
            source.SetOffset( wxPoint( m_offset.x + dx, m_offset.y + d ));
        }

        wxCoord TargetHeight = m_size.y;
        offsetToChildSeparator.resize(m_brick->GetChildCount());
        offsetToChild.resize(m_brick->GetChildCount());
        heightOfChildBricks.resize(m_brick->GetChildCount());
        for ( wxUint32 idx =  0 ; idx+1 < m_brick->GetChildCount()  ; idx++ )
        {
            wxUint32 i = m_brick->GetChildCount() - 1 - idx;
            GraphNassiBrick *gchild = this->GetGraphBrick(m_brick->GetChild(i));
            wxCoord h = minimumHeightOfChildBricks[i];
            //wxCoord off = TargetHeight - h;
            heightOfChildBricks[i] = h;
            offsetToChild[i] = TargetHeight - h;
            if ( gchild )
                gchild->SetOffsetAndSize(dc,
                        wxPoint(m_offset.x + m_hw-1, m_offset.y + offsetToChild[i]),
                        wxPoint(m_size.x - m_hw+1,   heightOfChildBricks[i]));

            wxCoord offsetToDiagonalLine =
                m_b - (wxCoord)((float)(m_b*offsetToChild[i])/(2.0*(float)m_size.y));
            offsetToChildSeparator[i] = offsetToDiagonalLine;


            wxCoord ht = dy;
            wxCoord ddyy = 0;
            if ( m_view->IsDrawingComment() )
            {
                ht += childcomments(i)->GetTotalHeight();
                ddyy += dy;
            }
            if ( m_view->IsDrawingSource() )
            {
                if ( m_view->IsDrawingComment() )
                    ht += dy;
                ht += childsources(i)->GetTotalHeight();
                ddyy += dy;
            }
            wxCoord OffsetToText =
                m_b - (wxCoord)((float)(m_b*(offsetToChild[i] + h - ht))/(2.0*(float)m_size.y)) + dx;
            childcomments(i)->SetOffset(wxPoint(m_offset.x + OffsetToText, m_offset.y + offsetToChild[i] + h - ht + ddyy));
            if ( m_view->IsDrawingComment() )
                ht += dy + childcomments(i)->GetTotalHeight();
            childsources(i)->SetOffset(wxPoint(m_offset.x + OffsetToText, m_offset.y + offsetToChild[i] + h - ht + ddyy));

            TargetHeight -= (minimumHeightOfChildBricks[i] - 1);
        }
        /// the first ( read: streched ) child:
        if ( m_brick->GetChildCount() > 0 )
        {
            GraphNassiBrick *gchild = this->GetGraphBrick(m_brick->GetChild(0));
            wxCoord h = TargetHeight;
            offsetToChildSeparator[0] = m_b;
            heightOfChildBricks[0] = TargetHeight;
            offsetToChild[0] = 0;
            if ( gchild )
                gchild->SetOffsetAndSize(dc,
                        wxPoint(m_offset.x + m_hw-1, m_offset.y ),
                        wxPoint(m_size.x - m_hw+1,  heightOfChildBricks[0] ));

            wxCoord ht = 0;
            wxCoord ddyy = 0;
            if ( m_view->IsDrawingComment() )
            {
                ht += childcomments(0)->GetTotalHeight();
                ht += dy;
                ddyy += dy;
            }
            if ( m_view->IsDrawingSource() )
            {
                ht += childsources(0)->GetTotalHeight();
                ht += dy;
                ddyy +=dy;
            }
            wxCoord OffsetToText =
                m_b - (wxCoord)((float)(m_b*(h - ht))/(2.0*(float)m_size.y)) + dx;

            childcomments(0)->SetOffset(wxPoint(m_offset.x + OffsetToText, m_offset.y + h - ht + ddyy));
            if ( m_view->IsDrawingComment() )
                ht += dy + childcomments(0)->GetTotalHeight();
            childsources(0)->SetOffset(wxPoint(m_offset.x + OffsetToText, m_offset.y + h - ht + ddyy));
        }
    }

    offset.y += ( m_size.y - 1 );
    size.y -= ( m_size.y - 1);

    GraphNassiBrick *nextgbrick = this->GetGraphBrick( m_brick->GetNext() );
    if ( nextgbrick )
        nextgbrick->SetOffsetAndSize(dc, offset, size);
}
void GraphNassiSwitchBrick::CalcMinSize(wxDC *dc, wxPoint &size)
{
    ///set all childs invisible, if minimized
    for ( wxUint32 n = 0 ; n < m_brick->GetChildCount() ; n++ )
    {
        NassiBrick *child = m_brick->GetChild(n);
        GraphNassiBrick *childgbrick = this->GetGraphBrick(child);
        if ( childgbrick )
            childgbrick->SetInvisible( !(this->IsMinimized()) );
    }

    dc->SetFont(m_view->GetCommentFont() );
    comment.CalcMinSize(dc);
    dc->SetFont(m_view->GetSourceFont() );
    source.CalcMinSize(dc);


    wxCoord dx = dc->GetCharWidth(),
            dy = dc->GetCharHeight();

    wxCoord w = 0, h = 0;
    if ( this->IsMinimized() )
    {
        h = 2*dy;
        w = 2*dx;
        if ( m_view->IsDrawingComment() )
        {
            h += comment.GetTotalHeight();
            w += comment.GetWidth();
        }

        h += 10;  //MinMaxBox
        w += 18; // the symbol
        //p = 0;
        //q = 0;
        minimumHeightOfChildBricks.clear();
    }
    else
    {
        wxCoord h0 = 0, b0 = 0;

        if ( m_view->IsDrawingComment() )
        {
            h0 += comment.GetTotalHeight();
            b0 += comment.GetWidth();
        }
        if ( m_view->IsDrawingSource() )
        {
            if ( m_view->IsDrawingComment() )
                h0 += dy;
            h0 += source.GetTotalHeight();
            if ( b0 < static_cast<wxCoord>(source.GetWidth()) )
                b0 = source.GetWidth();

        }
        h0 += 10; // the MinMax Box
        h0 += 2*dy;
        b0 += 2*dx;

        /// clear the container with the text related numbers
        // set all TextGraphs as unused
        for ( TextGraphMap::iterator it = m_textMap.begin() ; it != m_textMap.end() ; it++)
            it->second->SetUsed( false );

        // clear all text pointers
        childcomment.clear();
        childsource.clear();

        // read text pointers from document
        for( wxUint32 i = 0 ; i < m_brick->GetChildCount() ; i++ )
        {
            // create new TextGraphs if needed mark the as used in both cases
            for ( wxUint32 l = 0; l < 2 ; l++ )
            {
                wxUint32 k = 2*(i+1) + l;
                const wxString *str = m_brick->GetTextByNumber( k );
                if ( l == 0 )
                    childcomment.push_back( str );
                else
                    childsource.push_back( str );
                TextGraphMap::iterator it = m_textMap.find(str);
                if ( it != m_textMap.end() )
                {
                    it->second->SetUsed();
                    it->second->SetNumber(k);
                }
                else
                {
                    m_textMap[str] = new TextGraph(m_view, m_brick, k);
                    m_textMap[str]->SetUsed();
                }
            }
        }
        // remove unsused text graphs
        std::vector<const wxString *> PtrsOfNonexistentStrings;
        for (TextGraphMap::iterator it = m_textMap.begin() ; it != m_textMap.end() ; it++ )
            if ( ! it->second->IsUsed() )
                PtrsOfNonexistentStrings.push_back(it->first);
        for ( wxUint32 i = 0 ; i < PtrsOfNonexistentStrings.size() ; i++ )
        {
            const wxString *str = PtrsOfNonexistentStrings[i];
            TextGraph *textgraph = m_textMap[str];
            if ( textgraph ) delete textgraph;
            m_textMap.erase(str);
        }

        ///get the height of each child brick
        std::vector<wxCoord> htext;
        std::vector<wxCoord> btext;
        std::vector<wxCoord> hblock;
        std::vector<wxCoord> bblock;
        for( wxUint32 i = 0 ; i < m_brick->GetChildCount() ; i++ )
        {
            wxCoord b=0, h=0;
            dc->SetFont(m_view->GetCommentFont() );
            TextGraph *ctg = m_textMap[m_brick->GetTextByNumber(2*(i+1))];
            if ( m_view->IsDrawingComment() )
            {
                ctg->CalcMinSize(dc);
                b = ctg->GetWidth();
                h = ctg->GetTotalHeight();
            }
            TextGraph *stg = m_textMap[m_brick->GetTextByNumber(2*(i+1)+1)];
            dc->SetFont(m_view->GetSourceFont() );
            if ( m_view->IsDrawingSource() )
            {
                stg->CalcMinSize(dc);
                if ( m_view->IsDrawingComment() )
                    h += dy;
                h += stg->GetTotalHeight();
                if ( b < static_cast<wxCoord>(stg->GetWidth()) )
                    b = stg->GetWidth();
            }

            h += 2 * dy;
            b += 2 * dx;
            htext.push_back(h);
            btext.push_back(b);

            GraphNassiBrick *gchild = this->GetGraphBrick(m_brick->GetChild(i));
            if ( gchild )
            {
                wxPoint size(0,0);
                gchild->CalcMinSize(dc, size);
                hblock.push_back(size.y);
                bblock.push_back(size.x);
            }
            else
            {
                hblock.push_back(4*dy);
                bblock.push_back(8*dx);
            }
        }

        ///calc the minimum height
        h=1;
        for ( wxUint32 i = 0 ; i < m_brick->GetChildCount() ; i++ )
        {
            if ( hblock[i] < htext[i] ) // is the "case"-Text higher than the child-brick?
                hblock[i] = htext[i];
            h += hblock[i] - 1;
        }
        if ( h < h0 ) // total child height is smaller than head-text
        {
            if ( m_brick->GetChildCount() > 0 )
            {
                h -= hblock[0];
                hblock[0] = ( h0 + 2*dy );
                h += ( h0 + 2*dy );
            }
            else
            {
                hblock.push_back(h0 + 2*dy);
                h = h0 + 2*dy;
            }
        }

        wxCoord b = (wxCoord)((double)(b0*h)/(double)(h-h0));
        wxCoord wi = 10;
        wxCoord hi = 0;
        for (wxUint32 i = 0 ; i < m_brick->GetChildCount() ; i++ )
        {
            hi += hblock[i] - 1;
            wxCoord nw = btext[i] - (wxCoord)((double)((hi - htext[i])*b)/(2.0*(double)h));
            if ( wi < nw ) wi = nw;
        }
        ///set the width
        for( wxUint32 i = 0 ; i < m_brick->GetChildCount() ; i++ )
        {
            if ( w < bblock[i] + b + wi - 1 )
                w = bblock[i] + b + wi - 1;
        }
        if ( m_brick->GetChildCount() == 0 && w < b + wi)
            w = b + wi;

        /// remember, a hint for the drawer
        minimumHeightOfChildBricks = hblock;
        m_b = b;
        m_hw = b + wi;
    }

    m_minimumsize.x = w;
    m_minimumsize.y = h;

    if ( size.x < m_minimumsize.x )
        size.x = m_minimumsize.x;
    size.y += m_minimumsize.y;

    GraphNassiBrick *gnext = this->GetGraphBrick(m_brick->GetNext());
    if ( gnext )
    {
        gnext->CalcMinSize(dc, size);
        size.y--;// thickness of the line between bricks is only 1
    }

}
bool GraphNassiSwitchBrick::HasPoint(const wxPoint &pos)
{
    if (!IsVisible() ) return false;

    if ( IsMinimized() || m_brick->GetChildCount() == 0 ) return GraphNassiBrick::HasPoint(pos);

    if ( GraphNassiBrick::HasPoint(pos) )
    {
        if ( pos.x < m_offset.x + m_hw + 1 ) return true;

        for ( wxUint32 i = 0 ; i < m_brick->GetChildCount() ; i++ )
        {
            if ( pos.y > m_offset.y + offsetToChild[i] &&
                 pos.y < m_offset.y + offsetToChild[i] + heightOfChildBricks[i] )
            {
                if ( m_brick->GetChild(i) )
                    return false;
                return true;
            }
        }
    }
    return false;
}
TextGraph *GraphNassiSwitchBrick::IsOverText(const wxPoint &pos)
{
    if ( !m_visible ) return 0;
    if ( this->IsMinimized() )
    {
        if (m_view->IsDrawingComment() && comment.HasPoint(pos) )
            return &comment;
        else
            return 0;
    }
    if ( m_view->IsDrawingComment() )
    {   if ( comment.HasPoint(pos) ) return &comment;
        for ( wxUint32 i = 0 ; i < childcomment.size() ; i++ )
            if ( childcomments(i)->HasPoint(pos) )
                return childcomments(i);
    }
    if ( m_view->IsDrawingSource() )
    {
        if ( source.HasPoint(pos) ) return &source;
        for ( wxUint32 i = 0 ; i < childsource.size() ; i++ )
            if ( childsources(i)->HasPoint(pos) ) return childsources(i);
    }

    return 0;
}
bool GraphNassiSwitchBrick::IsOverChild(const wxPoint &pos, wxRect *childRect, wxUint32 *childNumber)
{
    if ( !m_visible || IsMinimized() ) return false;

    for ( wxUint32 i = 0 ; i < m_brick->GetChildCount() ; i++ )
    {
        if ( !m_brick->GetChild(i) )
        {
            wxRect rect;
            rect.x = m_offset.x + m_hw;
            rect.y = m_offset.y + offsetToChild[i];
            rect.width = m_size.x - m_hw;
            rect.height = heightOfChildBricks[i];
            if ( rect.Contains(pos) )
            {
                if ( childRect )
                    *childRect = rect;
                if ( childNumber )
                    *childNumber = i;
                return true;
            }
        }
    }
    return false;
}

TextGraph *GraphNassiSwitchBrick::childcomments(wxUint32 n)
{
    if ( n < childcomment.size() )
    {
        const wxString *str = childcomment[n];
        TextGraph *textgraph = m_textMap[str];
        if ( textgraph )
            return textgraph;
    }
    return 0;
}
TextGraph *GraphNassiSwitchBrick::childsources(wxUint32 n)
{
    if ( n < childsource.size() )
    {
        const wxString *str = childsource[n];
        TextGraph *textgraph = m_textMap[str];
        if ( textgraph )
            return textgraph;
    }
    return 0;
}
bool GraphNassiSwitchBrick::HasActiveChildIndicator()
{
    if ( !m_visible || IsMinimized() ) return false;

    return m_ChildIndicatorIsActive;
}
bool GraphNassiSwitchBrick::IsOverChildIndicator(const wxPoint & pos, wxUint32 *childNumber)
{
    if ( !m_visible || IsMinimized() ) return false;

    wxPoint pt ;
    pt.x = pos.x - m_offset.x;
    pt.y = pos.y - m_offset.y;
    if ( pt.x > m_hw && m_brick->GetChildCount() > 0 )
    {
        return false;
    }

    if ( pt.x < m_b - pt.y * m_b/2/m_size.y  )
    {
        return false;
    }

    if ( m_brick->GetChildCount() == 0)
    {
        if ( childNumber )*childNumber = 0;
        return true;
    }
    else
    {
        for ( wxUint32 i = 0 ; i < m_brick->GetChildCount() ; i++ )
        {
            wxCoord c = offsetToChild[i+1];
            if ( i + 1 == m_brick->GetChildCount() )
                c = m_size.y-1;
            if (  pt.y < c )
            {
                if ( childNumber ) *childNumber = i;
                return true;
            }
        }
    }
    return false;
}
void GraphNassiSwitchBrick::SetChildIndicatorActive(bool act, wxUint32 child)
{
    if ( child < m_brick->GetChildCount() )
    {
        m_ChildIndicatorIsActive = act;
        m_ActiveChildIndicator = child;
    }
    else
        m_ChildIndicatorIsActive = false;


}
void GraphNassiSwitchBrick::DrawActive(wxDC *dc)
{
    GraphNassiBrick::DrawActive(dc);

    if ( !m_ChildIndicatorIsActive || !IsVisible() ) return;

    wxBrush *brush = new wxBrush(*wxBLUE, wxTRANSPARENT);
    wxPen *pen = new wxPen(*wxBLUE, 3);
    dc->SetBrush(*brush);
    dc->SetPen(*pen);

    wxPoint points[5];
    points[0] = wxPoint( offsetToChildSeparator[m_ActiveChildIndicator],
                                offsetToChild[m_ActiveChildIndicator] );
    points[1] = wxPoint( m_hw,
                                offsetToChild[m_ActiveChildIndicator] );
    if ( m_ActiveChildIndicator + 1 == m_brick->GetChildCount() )
    {
        points[2] = wxPoint(m_hw, m_size.y-1);
        points[3] = wxPoint(m_b/2, m_size.y-1);
    }
    else
    {
        points[2] = wxPoint( m_hw,                                     offsetToChild[m_ActiveChildIndicator+1] );
        points[3] = wxPoint( offsetToChildSeparator[m_ActiveChildIndicator+1], offsetToChild[m_ActiveChildIndicator+1] );
    }
    points[4] = points[0];

    dc->DrawLines(5, points, m_offset.x, m_offset.y);
    dc->SetBrush(*wxWHITE_BRUSH);
    dc->SetPen(wxNullPen);
    delete brush;
    delete pen;
}
wxUint32 GraphNassiSwitchBrick::ActiveChildIndicator()
{
    return m_ActiveChildIndicator;
}
HooverDrawlet *GraphNassiSwitchBrick::GetDrawlet(const wxPoint & pos, bool /*HasNoBricks*/)
{

    Position p = GetPosition(pos);

    if ( p.pos == Position::none )
        return 0;
    if ( p.pos == Position::top )
        return new RedLineDrawlet(m_offset, GetWidth());
    if ( p.pos == Position::bottom )
        return new RedLineDrawlet( m_offset + wxPoint(0, GetHeight()-1), GetWidth() );
    if ( p.pos == Position::child )
    {
        wxRect rect;
        IsOverChild(pos, &rect);
        return new RedHatchDrawlet(rect);
    }

    if ( m_brick->GetChildCount() == 0 )
        return new RedLineDrawlet(wxPoint(m_offset.x+m_b, m_offset.y), m_size.x - m_b);
    else
    {
        wxPoint pt( m_offset.x + offsetToChildSeparator[p.number],
                    m_offset.y + offsetToChild[p.number]);
        if ( p.number == m_brick->GetChildCount() )
        {
            pt.x = m_offset.x + m_b/2;
            pt.y = m_offset.y + m_size.y - 1;
        }
        return new RedLineDrawlet(pt, m_offset.x + m_hw - pt.x );
    }
}
GraphNassiBrick::Position GraphNassiSwitchBrick::GetPosition(const wxPoint &pos)
{
    GraphNassiBrick::Position res;

    if ( IsActive() || !HasPoint(pos) )
    {
        res.pos = Position::none;
        return res;
    }


    wxUint32 OverChildIndicator;
    if ( IsOverChildIndicator(pos, &OverChildIndicator) )
    {
        if ( !HasActiveChildIndicator() || OverChildIndicator != m_ActiveChildIndicator )
        {
            res.pos = Position::childindicator;
            if ( m_brick->GetChildCount() == 0 )
                res.number = 0;
            else
            {
                bool top = false;
                wxCoord yt = m_offset.y + offsetToChild[OverChildIndicator];
                wxCoord yb = m_offset.y + offsetToChild[OverChildIndicator + 1];
                if ( OverChildIndicator + 1 == m_brick->GetChildCount() )
                    yb = m_offset.y + m_size.y-1;
                if ( 2*pos.y < yt + yb )
                    top = true;
                if ( HasActiveChildIndicator() )
                {
                    if ( OverChildIndicator + 1 == m_ActiveChildIndicator )
                        top = true;
                    else if ( OverChildIndicator == m_ActiveChildIndicator + 1 )
                        top = false;
                }
                if ( top )
                    res.number = OverChildIndicator;
                else
                    res.number = OverChildIndicator + 1;
            }
        }
        else
            res.pos = Position::none;
    }
    else
        return GraphNassiMinimizableBrick::GetPosition(pos);

    return res;
}








