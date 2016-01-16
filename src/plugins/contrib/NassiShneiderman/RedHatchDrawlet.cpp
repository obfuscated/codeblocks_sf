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
#if wxCHECK_VERSION(2, 9, 0)
    wxRasterOperationMode old_lf = dc.GetLogicalFunction();
#else
    int old_lf = dc.GetLogicalFunction();
#endif
    dc.SetLogicalFunction(wxXOR);

    wxPen old_pen = dc.GetPen();
    wxBrush old_brush = dc.GetBrush();

    wxColor red = wxColor( ~wxRED->Red(), ~wxRED->Green(), ~wxRED->Blue());
#if wxCHECK_VERSION(2,9,0)
    wxBrush brush = wxBrush(red, wxHATCHSTYLE_CROSSDIAG );
#else
    wxBrush brush = wxBrush(red, wxCROSSDIAG_HATCH );
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

