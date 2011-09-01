#include "RedHatchDrawlet.h"

RedHatchDrawlet::RedHatchDrawlet(wxRect rect):
    m_rect(rect)
{
    //ctor
}

RedHatchDrawlet::~RedHatchDrawlet()
{
    //dtor
}

bool RedHatchDrawlet::Draw(wxDC &dc)
{
    int old_lf = dc.GetLogicalFunction();
    dc.SetLogicalFunction(wxXOR);

    wxPen old_pen = dc.GetPen();
    wxBrush old_brush = dc.GetBrush();

    wxColor red = wxColor( ~wxRED->Red(), ~wxRED->Green(), ~wxRED->Blue());
    wxBrush brush = wxBrush(red, wxCROSSDIAG_HATCH );

    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(brush);

    dc.DrawRectangle(m_rect);

    dc.SetLogicalFunction(old_lf);
    dc.SetPen(old_pen);
    dc.SetBrush(old_brush);

    return true;
}

void RedHatchDrawlet::UnDraw(wxDC &dc)
{
    this->Draw(dc);
}

