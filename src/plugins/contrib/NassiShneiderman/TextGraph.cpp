#include "TextGraph.h"

#include "bricks.h"
#include "NassiView.h"
#include "TextCtrlTask.h"


TextGraph::TextGraph(NassiView *view, NassiBrick *brick, wxUint32 nmbr):
    m_used(false),
    lineoffsets(),
    linesizes(),
    linewidths(),
    offset(),
    m_view(view),
    m_brick(brick),
    m_nmbr(nmbr),
    m_str( brick->GetTextByNumber(nmbr) ),
    m_editTask(0)
{
    lineoffsets.clear();
    linesizes.clear();
    linewidths.clear();
}
TextGraph::~TextGraph()
{
    //dtor
    if ( m_editTask )
        m_editTask->UnlinkTextGraph();
}

const wxString *TextGraph::GetStringPtr()
{
    return m_str;
}
wxUint32 TextGraph::GetNumberOfLines()
{
    //wxString s( m_str );
    wxString s(*m_str);
    wxInt32 pos;
    wxUint32 k = 0;
    do{
        pos = s.Find('\n');
        if ( pos != -1 )
            s = s.SubString( pos+1, s.Length() );
        ++k;
    }
    while ( pos != -1);
    return k;
}
void TextGraph::CalcMinSize( wxDC *dc )
{
    //wxString s( m_str );

    wxString s(*m_str);
    lineoffsets.clear();
    linesizes.clear();
    linewidths.clear();
    wxInt32 pos;
    wxUint32 k = 0;
    do{
        pos = s.Find('\n');
        wxString te = s;
        if ( pos != -1 )
        {
            te = s.SubString(0, pos-1);
            s = s.SubString(pos+1, s.Length() );
        }
        wxCoord w, h;
        wxArrayInt widths;
        dc->GetTextExtent(te, &w, &h);
        if ( w == 0 ) w = 5;
        h = dc->GetCharHeight();
        dc->GetPartialTextExtents(te, widths);
        widths.Insert(0,0);
        lineoffsets.push_back( wxPoint(0, k*h) );
        linesizes.push_back(wxPoint(w,h));
        linewidths.push_back(widths);

        ++k;
    }
    while ( pos != -1);
}
void TextGraph::SetOffset(wxPoint off)
{
    offset = off;
}
void TextGraph::Draw( wxDC *dc )
{
    //wxString s( m_str );
    wxString s(*m_str);
    wxInt32 pos;
    wxUint32 k = 0;
    do{
        pos = s.Find('\n');
        wxString te = s;
        if ( pos != -1 )
        {
            te = s.SubString(0, pos-1);
            s = s.SubString(pos+1, s.Length() );
        }
        dc->DrawText( te, offset.x + lineoffsets[k].x, offset.y + lineoffsets[k].y);
        ++k;
    }
    while ( pos != -1);
}
wxUint32 TextGraph::GetTotalHeight()
{
    wxUint32 sum = 0;
    for ( wxUint32 n = 0 ; n < linesizes.size() ; ++n)
    {
        sum += linesizes[n].y;
    }
    return sum;
}
wxUint32 TextGraph::GetWidth()
{
    wxUint32 max = 0;
    for (wxUint32 n = 0 ; n < linesizes.size() ; ++n)
    {
        if ( max < static_cast<wxUint32>(linesizes[n].x) )
        {
            max = static_cast<wxUint32>(linesizes[n].x);
        }
    }
    return max;
}
bool TextGraph::HasPoint(const wxPoint &pos)
{
    for (wxUint32 n = 0 ; n < linesizes.size() ; ++n )
    {
        if ( pos.x > offset.x + lineoffsets[n].x &&
             pos.y > offset.y + lineoffsets[n].y &&
             pos.x < offset.x + lineoffsets[n].x + linesizes[n].x &&
             pos.y < offset.y + lineoffsets[n].y + linesizes[n].y )
            return true;
    }
    return false;
}
void TextGraph::SetNumber(wxUint32 nmbr)
{
    m_nmbr = nmbr;
}

void TextGraph::SetEditTask(TextCtrlTask *editTast)
{
    m_editTask = editTast;
}
void TextGraph::ClearEditTask()
{
    m_editTask = 0;
}
