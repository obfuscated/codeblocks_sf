
#include "HooverLine.h"

#include "logger.h"

// END Interface Dependencies -----------------------------------------


HooverLine::HooverLine(wxPoint start, wxPoint stop)
    :HooverDrawlet(),
    m_start(start),
    m_stop(stop)
{}
HooverLine::~HooverLine(){}


bool HooverLine::Draw(wxDC &dc)
{
    int old_lf = dc.GetLogicalFunction();
    dc.SetLogicalFunction(wxXOR);

    wxPen old_pen = dc.GetPen();
    dc.SetPen(*wxRED_PEN);

    dc.DrawLine(m_start, m_stop);

    dc.SetLogicalFunction(old_lf);
    dc.SetPen(old_pen);

    return true;
}
void HooverLine::UnDraw(wxDC &dc)
{
    this->Draw(dc);
}



