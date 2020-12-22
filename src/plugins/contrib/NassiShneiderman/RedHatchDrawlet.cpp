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
    wxRasterOperationMode old_lf = dc.GetLogicalFunction();
    dc.SetLogicalFunction(wxXOR);

    wxPen old_pen = dc.GetPen();
    wxBrush old_brush = dc.GetBrush();

    wxColor red = wxColor( ~wxRED->Red(), ~wxRED->Green(), ~wxRED->Blue());
#if wxCHECK_VERSION(3, 1, 0)
    wxBrush brush = wxBrush(red, wxBRUSHSTYLE_CROSSDIAG_HATCH );
#else
    wxBrush brush = wxBrush(red, wxHATCHSTYLE_CROSSDIAG );
#endif

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

