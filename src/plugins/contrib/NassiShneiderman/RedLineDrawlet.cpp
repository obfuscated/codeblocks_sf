#include "RedLineDrawlet.h"

RedLineDrawlet::RedLineDrawlet(wxPoint pos, wxCoord length, bool vertical):
    m_pos(pos),
    m_length(length),
    m_vertical(vertical)
{
    //ctor
}

RedLineDrawlet::~RedLineDrawlet()
{
    //dtor
}
bool RedLineDrawlet::Draw(wxDC &dc)
{
#if wxCHECK_VERSION(3, 0, 0)
    wxRasterOperationMode old_lf = dc.GetLogicalFunction();
#else
    int old_lf = dc.GetLogicalFunction();
#endif
    dc.SetLogicalFunction(wxXOR);

    wxPen old_pen = dc.GetPen();
    wxColor red = wxColor( ~(wxRED->Red()), ~wxRED->Green(), ~wxRED->Blue());
    wxPen pen = wxPen(red);
    dc.SetPen(pen);
    dc.DrawLine(m_pos.x, m_pos.y-1, m_pos.x+m_length, m_pos.y-1);
    dc.DrawLine(m_pos.x, m_pos.y+1, m_pos.x+m_length, m_pos.y+1);

    dc.SetPen(*wxRED_PEN);
    dc.DrawLine(m_pos.x, m_pos.y, m_pos.x+m_length, m_pos.y);

    dc.SetLogicalFunction(old_lf);
    dc.SetPen(old_pen);

    return true;
}
void RedLineDrawlet::UnDraw(wxDC &dc)
{
    this->Draw(dc);
}



