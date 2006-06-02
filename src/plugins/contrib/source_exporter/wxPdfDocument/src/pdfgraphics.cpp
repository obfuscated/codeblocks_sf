///////////////////////////////////////////////////////////////////////////////
// Name:        pdfgraphics.cpp
// Purpose:     Implementation of wxPdfDocument graphics primitives
// Author:      Ulrich Telle
// Modified by:
// Created:     2006-01-27
// Copyright:   (c) Ulrich Telle
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/// \file pdfgraphics.cpp Implementation of the wxPdfDocument graphics primitives

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/pdfdoc.h"

// ----------------------------------------------------------------------------
// wxPdfLineStyle: class representing line style for drawing graphics
// ----------------------------------------------------------------------------

wxPdfLineStyle::wxPdfLineStyle(double width,
                               wxPdfLineCap cap, wxPdfLineJoin join,
                               const wxPdfArrayDouble& dash, double phase,
                               const wxPdfColour& color)
{
  m_isSet = (width > 0) || (cap >= 0) || (join >= 0) || (dash.GetCount() > 0);
  m_width = width;
  m_cap   = cap;
  m_join  = join;
  m_dash  = dash;
  m_phase = phase;
  m_color = color;
}

wxPdfLineStyle::~wxPdfLineStyle()
{
}


wxPdfLineStyle::wxPdfLineStyle(const wxPdfLineStyle& lineStyle)
{
  m_isSet = lineStyle.m_isSet;
  m_width = lineStyle.m_width;
  m_cap   = lineStyle.m_cap;
  m_join  = lineStyle.m_join;
  m_dash  = lineStyle.m_dash;
  m_phase = lineStyle.m_phase;
  m_color = lineStyle.m_color;
}

wxPdfLineStyle&
wxPdfLineStyle::operator= (const wxPdfLineStyle& lineStyle)
{
  m_isSet = lineStyle.m_isSet;
  m_width = lineStyle.m_width;
  m_cap   = lineStyle.m_cap;
  m_join  = lineStyle.m_join;
  m_dash  = lineStyle.m_dash;
  m_phase = lineStyle.m_phase;
  m_color = lineStyle.m_color;
  return *this;
}

wxPdfGradient::wxPdfGradient(int type, const wxColour& color1, const wxColour& color2, double coords[])
{
  m_type = type;
  m_color1 = color1;
  m_color2 = color2;
  int nCoords = (type == 2) ? 4 : ((type == 3) ? 5 : 0);
  int j;
  for (j = 0; j < nCoords; j++)
  {
    m_coords[j] = coords[j];
  }
}

wxPdfGradient::~wxPdfGradient()
{
}

// ---

void
wxPdfDocument::Line(double x1, double y1, double x2, double y2)
{
  // Draw a line
  OutAscii(Double2String(x1*m_k,2) + wxString(_T(" ")) +
           Double2String((m_h-y1)*m_k,2) + wxString(_T(" m ")) +
           Double2String(x2*m_k,2) + wxString(_T(" ")) +
           Double2String((m_h-y2)*m_k,2) + wxString(_T(" l S")));
}

void
wxPdfDocument::Rect(double x, double y, double w, double h, int style)
{
  wxString op;
  // Draw a rectangle
  if ((style & wxPDF_STYLE_FILLDRAW) == wxPDF_STYLE_FILL)
  {
    op = _T("f");
  }
  else if ((style & wxPDF_STYLE_FILLDRAW) == wxPDF_STYLE_FILLDRAW)
  {
    op = _T("B");
  }
  else
  {
    op = _T("S");
  }
  OutAscii(Double2String(x*m_k,2) + wxString(_T(" ")) +
           Double2String((m_h-y)*m_k,2) + wxString(_T(" ")) +
           Double2String(w*m_k,2) + wxString(_T(" ")) +
           Double2String(-h*m_k,2) + wxString(_T(" re ")) + op);
}

void
wxPdfDocument::RoundedRect(double x, double y, double w, double h,
                           double r, int roundCorner, int style)
{
  if ((roundCorner & wxPDF_CORNER_ALL) == wxPDF_CORNER_NONE)
  {
    // Not rounded
    Rect(x, y, w, h, style);
  }
  else
  { 
    // Rounded
    wxString op;
    // Draw a rectangle
    if ((style & wxPDF_STYLE_FILLDRAW) == wxPDF_STYLE_FILL)
    {
      op = _T("f");
    }
    else
    {
      if ((style & wxPDF_STYLE_FILLDRAW) == wxPDF_STYLE_FILLDRAW)
      {
        op = _T("B");
      }
      else
      {
        op = _T("S");
      }
    }

    double myArc = 4. / 3. * (sqrt(2.) - 1.);

    OutPoint(x + r, y);
    double xc = x + w - r;
    double yc = y + r;
    OutLine(xc, y);

    if (roundCorner & wxPDF_CORNER_TOP_LEFT)
    {
      OutCurve(xc + (r * myArc), yc - r, xc + r, yc - (r * myArc), xc + r, yc);
    }
    else
    {
      OutLine(x + w, y);
    }

    xc = x + w - r ;
    yc = y + h - r;
    OutLine(x + w, yc);

    if (roundCorner & wxPDF_CORNER_TOP_RIGHT)
    {
      OutCurve(xc + r, yc + (r * myArc), xc + (r * myArc), yc + r, xc, yc + r);
    }
    else
    {
      OutLine(x + w, y + h);
    }

    xc = x + r;
    yc = y + h - r;
    OutLine(xc, y + h);

    if (roundCorner & wxPDF_CORNER_BOTTOM_LEFT)
    {
      OutCurve(xc - (r * myArc), yc + r, xc - r, yc + (r * myArc), xc - r, yc);
    }
    else
    {
      OutLine(x, y + h);
    }

    xc = x + r;
    yc = y + r;
    OutLine(x, yc);
    
    if (roundCorner & wxPDF_CORNER_BOTTOM_RIGHT)
    {
      OutCurve(xc - r, yc - (r * myArc), xc - (r * myArc), yc - r, xc, yc - r);
    }
    else
    {
      OutLine(x, y);
      OutLine(x + r, y);
    }
    OutAscii(op);
  }
}

void
wxPdfDocument::Curve(double x0, double y0, double x1, double y1, 
                     double x2, double y2, double x3, double y3,
                     int style)
{
  wxString op;
  // Draw a rectangle
  if ((style & wxPDF_STYLE_FILLDRAW) == wxPDF_STYLE_FILL)
  {
    op = _T("f");
  }
  else
  {
    if ((style & wxPDF_STYLE_FILLDRAW) == wxPDF_STYLE_FILLDRAW)
    {
      op = _T("B");
    }
    else
    {
      op = _T("S");
    }
  }

  OutPoint(x0, y0);
  OutCurve(x1, y1, x2, y2, x3, y3);
  OutAscii(op);
}

void
wxPdfDocument::Ellipse(double x0, double y0, double rx, double ry, 
                       double angle, double astart, double afinish,
                       int style, int nSeg)
{
  if (rx <= 0) return;

  wxString op;
  // Draw a rectangle
  if ((style & wxPDF_STYLE_MASK) == wxPDF_STYLE_FILL)
  {
    op = _T("f");
  }
  else
  {
    if ((style & wxPDF_STYLE_MASK) == wxPDF_STYLE_FILLDRAW)
    {
      op = _T("B");
    }
    else if ((style & wxPDF_STYLE_MASK) == wxPDF_STYLE_DRAWCLOSE)
    {
      op = _T("s"); // small 's' means closing the path as well
    }
    else
    {
      op = _T("S");
    }
  }

  if (ry <= 0)
  {
    ry = rx;
  }
  rx *= m_k;
  ry *= m_k;
  if (nSeg < 2)
  {
    nSeg = 2;
  }

  static double pi = 4. * atan(1.0);
  astart = pi * astart / 180.;
  afinish = pi * afinish / 180.;
  double totalAngle = afinish - astart;

  double dt = totalAngle / nSeg;
  double dtm = dt / 3;

  x0 *= m_k;
  y0 = (m_h - y0) * m_k;
  if (angle != 0)
  {
    double a = -(pi * angle / 180.);
    OutAscii(wxString(_T("q ")) + 
             Double2String(cos(a),2) + wxString(_T(" ")) +
             Double2String(-1 * sin(a),2) + wxString(_T(" ")) +
             Double2String(sin(a),2) + wxString(_T(" ")) +
             Double2String(cos(a),2) + wxString(_T(" ")) +
             Double2String(x0,2) + wxString(_T(" ")) +
             Double2String(y0,2) + wxString(_T(" cm")));
    x0 = 0;
    y0 = 0;
  }

  double t1, a0, b0, c0, d0, a1, b1, c1, d1;
  t1 = astart;
  a0 = x0 + (rx * cos(t1));
  b0 = y0 + (ry * sin(t1));
  c0 = -rx * sin(t1);
  d0 = ry * cos(t1);
  OutPoint(a0 / m_k, m_h - (b0 / m_k));
  int i;
  for (i = 1; i <= nSeg; i++)
  {
    // Draw this bit of the total curve
    t1 = (i * dt) + astart;
    a1 = x0 + (rx * cos(t1));
    b1 = y0 + (ry * sin(t1));
    c1 = -rx * sin(t1);
    d1 = ry * cos(t1);
    OutCurve((a0 + (c0 * dtm)) / m_k,
             m_h - ((b0 + (d0 * dtm)) / m_k),
             (a1 - (c1 * dtm)) / m_k,
             m_h - ((b1 - (d1 * dtm)) / m_k),
             a1 / m_k,
             m_h - (b1 / m_k));
    a0 = a1;
    b0 = b1;
    c0 = c1;
    d0 = d1;
  }
  OutAscii(op);
  if (angle !=0)
  {
    Out("Q");
  }
}

void
wxPdfDocument::Circle(double x0, double y0, double r, double astart, double afinish,
                      int style, int nSeg)
{
  Ellipse(x0, y0, r, 0, 0, astart, afinish, style, nSeg);
}

void
wxPdfDocument::Sector(double xc, double yc, double r, double astart, double afinish,
                      int style, bool clockwise, double origin)
{
  static double pi = 4. * atan(1.);
  static double pi2 = 0.5 * pi;
  double d;
  if (clockwise)
  {
    d = afinish;
    afinish = origin - astart;
    astart = origin - d;
  }
  else
  {
    afinish += origin;
    astart += origin;
  }
  astart = fmod(astart, 360.) + 360;
  afinish = fmod(afinish, 360.) + 360;
  if (astart > afinish)
  {
    afinish += 360;
  }
  afinish = afinish / 180. * pi;
  astart = astart / 180. * pi;
  d = afinish - astart;
  if (d == 0)
  {
    d = 2 * pi;
  }
  
  wxString op;
  if ((style & wxPDF_STYLE_FILLDRAW) == wxPDF_STYLE_FILL)
  {
    op = _T("f");
  }
  else
  {
    if ((style & wxPDF_STYLE_FILLDRAW) == wxPDF_STYLE_FILLDRAW)
    {
      op = _T("b");
    }
    else
    {
      op = _T("s");
    }
  }

  double myArc;
  if (sin(d/2) != 0.0)
  {
    myArc = 4./3. * (1.-cos(d/2))/sin(d/2) * r;
  }
  else
  {
    myArc = 0.0;
  }
  // first put the center
  OutPoint(xc,yc);
  // put the first point
  OutLine(xc+r*cos(astart),yc-r*sin(astart));
  // draw the arc
  if (d < pi2)
  {
    OutCurve(xc+r*cos(astart)+myArc*cos(pi2+astart),
             yc-r*sin(astart)-myArc*sin(pi2+astart),
             xc+r*cos(afinish)+myArc*cos(afinish-pi2),
             yc-r*sin(afinish)-myArc*sin(afinish-pi2),
             xc+r*cos(afinish),
             yc-r*sin(afinish));
  }
  else
  {
    afinish = astart + d/4;
    myArc = 4./3. * (1.-cos(d/8))/sin(d/8) * r;
    OutCurve(xc+r*cos(astart)+myArc*cos(pi2+astart),
             yc-r*sin(astart)-myArc*sin(pi2+astart),
             xc+r*cos(afinish)+myArc*cos(afinish-pi2),
             yc-r*sin(afinish)-myArc*sin(afinish-pi2),
             xc+r*cos(afinish),
             yc-r*sin(afinish));
    astart = afinish;
    afinish = astart + d/4;
    OutCurve(xc+r*cos(astart)+myArc*cos(pi2+astart),
             yc-r*sin(astart)-myArc*sin(pi2+astart),
             xc+r*cos(afinish)+myArc*cos(afinish-pi2),
             yc-r*sin(afinish)-myArc*sin(afinish-pi2),
             xc+r*cos(afinish),
             yc-r*sin(afinish));
    astart = afinish;
    afinish = astart + d/4;
    OutCurve(xc+r*cos(astart)+myArc*cos(pi2+astart),
             yc-r*sin(astart)-myArc*sin(pi2+astart),
             xc+r*cos(afinish)+myArc*cos(afinish-pi2),
             yc-r*sin(afinish)-myArc*sin(afinish-pi2),
             xc+r*cos(afinish),
             yc-r*sin(afinish));
    astart = afinish;
    afinish = astart + d/4;
    OutCurve(xc+r*cos(astart)+myArc*cos(pi2+astart),
             yc-r*sin(astart)-myArc*sin(pi2+astart),
             xc+r*cos(afinish)+myArc*cos(afinish-pi2),
             yc-r*sin(afinish)-myArc*sin(afinish-pi2),
             xc+r*cos(afinish),
             yc-r*sin(afinish));
  }
  // terminate drawing
  OutAscii(op);
}

void
wxPdfDocument::Polygon(const wxPdfArrayDouble& x, const wxPdfArrayDouble& y, int style)
{
  int np = (x.GetCount() < y.GetCount()) ? x.GetCount() : y.GetCount();

  wxString op;
  if ((style & wxPDF_STYLE_FILLDRAW) == wxPDF_STYLE_FILL)
  {
    op = _T("f");
  }
  else
  {
    if ((style & wxPDF_STYLE_FILLDRAW) == wxPDF_STYLE_FILLDRAW)
    {
      op = _T("B");
    }
    else
    {
      op = _T("S");
    }
  }

  OutPoint(x[0], y[0]);
  int i;
  for (i = 1; i < np; i++)
  {
    OutLine(x[i], y[i]);
  }
  OutLine(x[0], y[0]);
  OutAscii(op);
}

void
wxPdfDocument::RegularPolygon(double x0, double y0, double r, int ns, double angle, bool circle, int style, 
                              int circleStyle, const wxPdfLineStyle& circleLineStyle, const wxPdfColour& circleFillColor)
{
  if (ns < 3)
  {
    ns = 3;
  }
  if (circle)
  {
    wxPdfLineStyle saveStyle = GetLineStyle();
    SetLineStyle(circleLineStyle);
    wxPdfColour saveColor = GetFillColor();
    SetFillColor(circleFillColor);
    Circle(x0, y0, r, 0, 360, circleStyle);
    SetLineStyle(saveStyle);
    SetFillColor(saveColor);
  }
  static double pi = 4. * atan(1.);
  double a;
  wxPdfArrayDouble x, y;
  int i;
  for (i = 0; i < ns; i++)
  {
    a = (angle + (i * 360 / ns)) / 180. * pi;
    x.Add(x0 + (r * sin(a)));
    y.Add(y0 + (r * cos(a)));
  }
  Polygon(x, y, style);
}


void
wxPdfDocument::StarPolygon(double x0, double y0, double r, int nv, int ng, double angle, bool circle, int style, 
                           int circleStyle, const wxPdfLineStyle& circleLineStyle, const wxPdfColour& circleFillColor)
{
  if (nv < 2)
  {
    nv = 2;
  }
  if (circle)
  {
    wxPdfLineStyle saveStyle = GetLineStyle();
    SetLineStyle(circleLineStyle);
    wxPdfColour saveColor = GetFillColor();
    SetFillColor(circleFillColor);
    Circle(x0, y0, r, 0, 360, circleStyle);
    SetLineStyle(saveStyle);
    SetFillColor(saveColor);
  }
  wxArrayInt visited;
  visited.SetCount(nv);
  int i;
  for (i = 0; i < nv; i++)
  {
    visited[i] = 0;
  }
  static double pi = 4. * atan(1.);
  double a;
  wxPdfArrayDouble x, y;
  i = 0;
  do
  {
    visited[i] = 1;
    a = (angle + (i * 360 / nv)) / 180. * pi;
    x.Add(x0 + (r * sin(a)));
    y.Add(y0 + (r * cos(a)));
    i = (i + ng) % nv;
  }
  while (visited[i] == 0);
  Polygon(x, y, style);
}

void
wxPdfDocument::ClippingText(double x, double y, const wxString& txt, bool outline)
{
  wxString op = outline ? _T("5") : _T("7");
  OutAscii(wxString(_T("q BT ")) +
           Double2String(x*m_k,2) + wxString(_T(" ")) +
           Double2String((m_h-y)*m_k,2) + wxString(_T(" Td ")) +
           op + wxString(_T(" Tr (")),false);
  TextEscape(txt,false);
  Out(") Tj 0 Tr ET");
}

void
wxPdfDocument::ClippingRect(double x, double y, double w, double h, bool outline)
{
  wxString op = outline ? _T("S") : _T("n");
  OutAscii(wxString(_T("q ")) +
           Double2String(x*m_k,2) + wxString(_T(" ")) +
           Double2String((m_h-y)*m_k,2) + wxString(_T(" ")) +
           Double2String(w*m_k,2) + wxString(_T(" ")) +
           Double2String(-h*m_k,2) + wxString(_T(" re W ")) + op);
}

void
wxPdfDocument::ClippingEllipse(double x, double y, double rx, double ry, bool outline)
{
  wxString op = outline ? _T("S") : _T("n");
  if (ry <= 0)
  {
    ry = rx;
  }
  double lx = 4./3. * (sqrt(2.)-1.) * rx;
  double ly = 4./3. * (sqrt(2.)-1.) * ry;

  OutAscii(wxString(_T("q ")) +
           Double2String((x+rx)*m_k,2) + wxString(_T(" ")) +
           Double2String((m_h-y)*m_k,2) + wxString(_T(" m ")) +
           Double2String((x+rx)*m_k,2) + wxString(_T(" ")) +
           Double2String((m_h-(y-ly))*m_k,2) + wxString(_T(" ")) +
           Double2String((x+lx)*m_k,2) + wxString(_T(" ")) +
           Double2String((m_h-(y-ry))*m_k,2) + wxString(_T(" ")) +
           Double2String(x*m_k,2) + wxString(_T(" ")) +
           Double2String((m_h-(y-ry))*m_k,2) + wxString(_T(" c")));

  OutAscii(Double2String((x-lx)*m_k,2) + wxString(_T(" ")) +
           Double2String((m_h-(y-ry))*m_k,2) + wxString(_T(" ")) +
           Double2String((x-rx)*m_k,2) + wxString(_T(" ")) +
           Double2String((m_h-(y-ly))*m_k,2) + wxString(_T(" ")) +
           Double2String((x-rx)*m_k,2) + wxString(_T(" ")) +
           Double2String((m_h-y)*m_k,2) + wxString(_T(" c")));

  OutAscii(Double2String((x-rx)*m_k,2) + wxString(_T(" ")) +
           Double2String((m_h-(y+ly))*m_k,2) + wxString(_T(" ")) +
           Double2String((x-lx)*m_k,2) + wxString(_T(" ")) +
           Double2String((m_h-(y+ry))*m_k,2) + wxString(_T(" ")) +
           Double2String(x*m_k,2) + wxString(_T(" ")) +
           Double2String((m_h-(y+ry))*m_k,2) + wxString(_T(" c")));

  OutAscii(Double2String((x+lx)*m_k,2) + wxString(_T(" ")) +
           Double2String((m_h-(y+ry))*m_k,2) + wxString(_T(" ")) +
           Double2String((x+rx)*m_k,2) + wxString(_T(" ")) +
           Double2String((m_h-(y+ly))*m_k,2) + wxString(_T(" ")) +
           Double2String((x+rx)*m_k,2) + wxString(_T(" ")) +
           Double2String((m_h-y)*m_k,2) + wxString(_T(" c W ")) + op);
}

void
wxPdfDocument::UnsetClipping()
{
  Out("Q");
}

void
wxPdfDocument::ClippedCell(double w, double h, const wxString& txt,
                           int border, int ln, int align, int fill, const wxPdfLink& link)
{
  if ((border != wxPDF_BORDER_NONE) || (fill != 0) || (m_y+h > m_pageBreakTrigger))
  {
    Cell(w, h, _T(""), border, 0, wxPDF_ALIGN_LEFT, fill);
    m_x -= w;
  }
  ClippingRect(m_x, m_y, w, h);
  Cell(w, h, txt, wxPDF_BORDER_NONE, ln, align, 0, link);
  UnsetClipping();
}

void
wxPdfDocument::SetLineStyle(const wxPdfLineStyle& linestyle)
{
  m_lineStyle = linestyle;
  if (linestyle.GetWidth() >= 0)
  {
    double width_prev = m_lineWidth;
    SetLineWidth(linestyle.GetWidth());
    m_lineWidth = width_prev;
  }
  switch (linestyle.GetLineCap())
  {
    case wxPDF_LINECAP_BUTT:
    case wxPDF_LINECAP_ROUND:
    case wxPDF_LINECAP_SQUARE:
      OutAscii(wxString::Format(_T("%d  J"), linestyle.GetLineCap()));
      break;
    default:
      break;
  }
  switch (linestyle.GetLineJoin())
  {
    case wxPDF_LINEJOIN_MITER:
    case wxPDF_LINEJOIN_ROUND:
    case wxPDF_LINEJOIN_BEVEL:
      OutAscii(wxString::Format(_T("%d  j"), linestyle.GetLineJoin()));
      break;
    default:
      break;
  }

  const wxPdfArrayDouble& dash = linestyle.GetDash();
  if (&dash != NULL)
  {
    wxString dashString = _T("");
    size_t j;
    for (j = 0; j < dash.GetCount(); j++)
    {
      if (j > 0)
      {
        dashString += wxString(_T(" "));
      }
      dashString += Double2String(dash[j],2);
    }
    double phase = linestyle.GetPhase();
    if (phase < 0 || dashString.Length() == 0)
    {
      phase = 0;
    }
    OutAscii(wxString(_T("[")) + dashString + wxString(_T("] ")) +
             Double2String(phase,2) + wxString(_T(" d")));
  }
  SetDrawColor(linestyle.GetColour());
}

const wxPdfLineStyle&
wxPdfDocument::GetLineStyle()
{
  return m_lineStyle;
}

void
wxPdfDocument::StartTransform()
{
  //save the current graphic state
  m_inTransform++;
  Out("q");
}

bool
wxPdfDocument::ScaleX(double sx, double x, double y)
{
  return Scale(sx, 100, x, y);
}

bool
wxPdfDocument::ScaleY(double sy, double x, double y)
{
  return Scale(100, sy, x, y);
}

bool
wxPdfDocument::ScaleXY(double s, double x, double y)
{
  return Scale(s, s, x, y);
}

bool
wxPdfDocument::Scale(double sx, double sy, double x, double y)
{
  if (x < 0)
  {
    x = m_x;
  }
  if (y < 0)
  {
    y = m_y;
  }
  if (sx == 0 || sy == 0)
  {
    // TODO  $this->Error('Please use values unequal to zero for Scaling');
    return false;
  }
  y = (m_h - y) * m_k;
  x *= m_k;
  //calculate elements of transformation matrix
  sx /= 100;
  sy /= 100;
  double tm[6];
  tm[0] = sx;
  tm[1] = 0;
  tm[2] = 0;
  tm[3] = sy;
  tm[4] = x * (1 - sx);
  tm[5] = y * (1 - sy);
  //scale the coordinate system
  if (m_inTransform == 0)
  {
    StartTransform();
  }
  Transform(tm);
  return true;
}

void
wxPdfDocument::MirrorH(double x)
{
  Scale(-100, 100, x);
}

void
wxPdfDocument::MirrorV(double y)
{
  Scale(100, -100, -1, y);
}

void
wxPdfDocument::TranslateX(double tx)
{
  Translate(tx, 0);
}

void
wxPdfDocument::TranslateY(double ty)
{
  Translate(0, ty);
}

void
wxPdfDocument::Translate(double tx, double ty)
{
  if (m_inTransform == 0)
  {
    StartTransform();
  }
  // calculate elements of transformation matrix
  double tm[6];
  tm[0] = 1;
  tm[1] = 0;
  tm[2] = 0;
  tm[3] = 1;
  tm[4] = tx;
  tm[5] = -ty;
  // translate the coordinate system
  Transform(tm);
}

void
wxPdfDocument::Rotate(double angle, double x, double y)
{
  if (m_inTransform == 0)
  {
    StartTransform();
  }
  if (x < 0)
  {
    x = m_x;
  }
  if (y < 0)
  {
    y = m_y;
  }
  y = (m_h - y) * m_k;
  x *= m_k;
  // calculate elements of transformation matrix
  double tm[6];
  angle *= (atan(1.) / 45.);
  tm[0] = cos(angle);
  tm[1] = sin(angle);
  tm[2] = -tm[1];
  tm[3] = tm[0];
  tm[4] = x + tm[1] * y - tm[0] * x;
  tm[5] = y - tm[0] * y - tm[1] * x;
  //rotate the coordinate system around ($x,$y)
  Transform(tm);
}

bool
wxPdfDocument::SkewX(double xAngle, double x, double y)
{
  return Skew(xAngle, 0, x, y);
}

bool
wxPdfDocument::SkewY(double yAngle, double x, double y)
{
  return Skew(0, yAngle, x, y);
}

bool
wxPdfDocument::Skew(double xAngle, double yAngle, double x, double y)
{
  if (x < 0)
  {
    x = m_x;
  }
  if (y < 0)
  {
    y = m_y;
  }
  if (xAngle <= -90 || xAngle >= 90 || yAngle <= -90 || yAngle >= 90)
  {
    // TODO $this->Error('Please use values between -90° and 90° for skewing');
    return false;
  }
  x *= m_k;
  y = (m_h - y) * m_k;
  //calculate elements of transformation matrix
  double tm[6];
  xAngle *= (atan(1.) / 45.);
  yAngle *= (atan(1.) / 45.);
  tm[0] = 1;
  tm[1] = tan(yAngle);
  tm[2] = tan(xAngle);
  tm[3] = 1;
  tm[4] = -tm[2] * y;
  tm[5] = -tm[1] * x;
  //skew the coordinate system
  if (m_inTransform == 0)
  {
    StartTransform();
  }
  Transform(tm);
  return true;
}

void
wxPdfDocument::StopTransform()
{
  //restore previous graphic state
  if (m_inTransform > 0)
  {
    m_inTransform--;
    Out("Q");
  }
}

void
wxPdfDocument::LinearGradient(double x, double y, double w, double h,
                              const wxColour& col1, const wxColour& col2)
{
  double coords[4] = {0, 0, 1, 0};
  LinearGradient(x, y, w, h, col1, col2, coords);
}

void
wxPdfDocument::LinearGradient(double x, double y, double w, double h,
                              const wxColour& col1, const wxColour& col2,
                              double coords[4])
{
  ClippingRect(x, y, w, h, false);
  //set up transformation matrix for gradient
  double tm[6];
  tm[0] = w * m_k;
  tm[1] = 0;
  tm[2] = 0;
  tm[3] = h * m_k;
  tm[4] = x * m_k;
  tm[5] = (m_h - (y+h)) * m_k;
  Transform(tm);
  Gradient(2, col1, col2, coords);
}

void
wxPdfDocument::RadialGradient(double x, double y, double w, double h,
                              const wxColour& col1, const wxColour& col2)
{
  double coords[5] = {0.5, 0.5, 0.5, 0.5, 1};
  RadialGradient(x, y, w, h, col1, col2, coords);
}

void
wxPdfDocument::RadialGradient(double x, double y, double w, double h,
                              const wxColour& col1, const wxColour& col2, 
                              double coords[5])
{
  ClippingRect(x, y, w, h, false);
  //set up transformation matrix for gradient
  double tm[6];
  tm[0] = w * m_k;
  tm[1] = 0;
  tm[2] = 0;
  tm[3] = h * m_k;
  tm[4] = x * m_k;
  tm[5] = (m_h - (y+h)) * m_k;
  Transform(tm);
  Gradient(3, col1, col2, coords);
}

/* draw a marker at a raw point-based coordinate */
void
wxPdfDocument::Marker(double x, double y, wxPdfMarker markerType, double size)
{
  double saveLineWidth = m_lineWidth;
  double halfsize = size * 0.5;
  static double b = 4. / 3.;

  Out("q");
  switch (markerType) 
  {
    case wxPDF_MARKER_CIRCLE:
      SetLineWidth(size * 0.15);
      OutPoint(x - halfsize, y);
      OutCurve(x - halfsize, y + b * halfsize, x + halfsize, y + b * halfsize, x + halfsize, y);
      OutCurve(x + halfsize, y - b * halfsize, x - halfsize, y - b * halfsize, x - halfsize, y);
      Out("h");
      Out("B");
      break;
    case wxPDF_MARKER_TRIANGLE_UP:
      SetLineWidth(size * 0.15);
      OutPoint(x, y - size * 0.6667);
      OutLineRelative(-size / 1.7321, size);
      OutLineRelative(1.1546 * size, 0.0);
      Out("h");
      Out("B");
      break;
    case wxPDF_MARKER_TRIANGLE_DOWN:
      SetLineWidth(size * 0.15);
      OutPoint(x, y + size * 0.6667);
      OutLineRelative(-size / 1.7321, -size);
      OutLineRelative(1.1546 * size, 0.0);
      Out("h");
      Out("B");
      break;
    case wxPDF_MARKER_TRIANGLE_LEFT:
      SetLineWidth(size * 0.15);
      OutPoint(x - size * 0.6667, y);
      OutLineRelative(size, -size / 1.7321);
      OutLineRelative(0.0, 1.1546 * size);
      Out("h");
      Out("B");
      break;
    case wxPDF_MARKER_TRIANGLE_RIGHT:
      SetLineWidth(size * 0.15);
      OutPoint(x + size * 0.6667, y);
      OutLineRelative(-size, -size / 1.7321);
      OutLineRelative(0.0, 1.1546 * size);
      Out("h");
      Out("B");
      break;
    case wxPDF_MARKER_DIAMOND:
      SetLineWidth(size * 0.15);
      size *= 0.9;
      OutPoint( x, y+size/1.38);
      OutLineRelative( 0.546 * size, -size / 1.38);
      OutLineRelative(-0.546 * size, -size / 1.38);
      OutLineRelative(-0.546 * size,  size / 1.38);
      Out("h");
      Out("B");
      break;
    case wxPDF_MARKER_SQUARE:
      SetLineWidth(size * 0.15);
      Rect(x - halfsize, y - halfsize, size, size, wxPDF_STYLE_FILLDRAW);
      Out("B");
      break;
    case wxPDF_MARKER_STAR:
      size *= 1.2;
      halfsize = 0.5 * size;
      SetLineWidth(size * 0.09);
      OutPoint(x, y + size * 0.5);
      OutLine(x + 0.112255 * size, y + 0.15451 * size);
      OutLine(x + 0.47552  * size, y + 0.15451 * size);
      OutLine(x + 0.181635 * size, y - 0.05902 * size);
      OutLine(x + 0.29389  * size, y - 0.40451 * size);
      OutLine(x, y - 0.19098 * size);
      OutLine(x - 0.29389  * size, y - 0.40451 * size);
      OutLine(x - 0.181635 * size, y - 0.05902 * size);
      OutLine(x - 0.47552  * size, y + 0.15451 * size);
      OutLine(x - 0.112255 * size, y + 0.15451 * size);
      Out("h");
      Out("B");
      break;
    case wxPDF_MARKER_STAR4:
      size *= 1.2;
      halfsize = 0.5 * size;
      SetLineWidth(size * 0.09);
      OutPoint(x, y + size * 0.5);
      OutLine(x + 0.125 * size, y + 0.125 * size);
      OutLine(x + size * 0.5, y);
      OutLine(x + 0.125 * size, y - 0.125 * size);
      OutLine(x, y - size * 0.5);
      OutLine(x - 0.125 * size, y - 0.125 * size);
      OutLine(x - size * 0.5, y);
      OutLine(x - 0.125 * size, y + 0.125 * size);
      Out("h");
      Out("B");
      break;
    case wxPDF_MARKER_PLUS:
      size *= 1.2;
      halfsize = 0.5 * size;
      SetLineWidth(size * 0.1);
      OutPoint(x + 0.125 * size, y + size * 0.5);
      OutLine(x + 0.125 * size, y + 0.125 * size);
      OutLine(x + size * 0.5, y + 0.125 * size);
      OutLine(x + size * 0.5, y - 0.125 * size);
      OutLine(x + 0.125 * size, y - 0.125 * size);
      OutLine(x + 0.125 * size, y - size * 0.5);
      OutLine(x - 0.125 * size, y - size * 0.5);
      OutLine(x - 0.125 * size, y - 0.125 * size);
      OutLine(x - size * 0.5, y - 0.125 * size);
      OutLine(x - size * 0.5, y + 0.125 * size);
      OutLine(x - 0.125 * size, y + 0.125 * size);
      OutLine(x - 0.125 * size, y + size * 0.5);
      Out("h");
      Out("B");
      break;
    case wxPDF_MARKER_CROSS:
      size *= 1.2;
      halfsize = 0.5 * size;
      SetLineWidth(size * 0.1);
      OutPoint(x, y + 0.176777 * size);
      OutLine(x + 0.265165 * size, y + 0.441941 * size);
      OutLine(x + 0.441941 * size, y + 0.265165 * size);
      OutLine(x + 0.176777 * size, y);
      OutLine(x + 0.441941 * size, y - 0.265165 * size);
      OutLine(x + 0.265165 * size, y - 0.441941 * size);
      OutLine(x, y - 0.176777 * size);
      OutLine(x - 0.265165 * size, y - 0.441941 * size);
      OutLine(x - 0.441941 * size, y - 0.265165 * size);
      OutLine(x - 0.176777 * size, y);
      OutLine(x - 0.441941 * size, y + 0.265165 * size);
      OutLine(x - 0.265165 * size, y + 0.441941 * size);
      Out("h");
      Out("B");
      break;
    case wxPDF_MARKER_PENTAGON_UP:
      SetLineWidth(size * 0.15);
      OutPoint(x + 0.5257 * size, y - size * 0.1708);
      OutLineRelative(-0.5257 * size, -0.382  * size);
      OutLineRelative(-0.5257 * size, 0.382  * size);
      OutLineRelative(0.2008 * size, 0.6181 * size);
      OutLineRelative(0.6499 * size,  0.0);
      Out("h");
      Out("B");
      break;
    case wxPDF_MARKER_PENTAGON_DOWN:
      SetLineWidth(size * 0.15);
      OutPoint(x - 0.5257 * size, y + size * 0.1708);
      OutLineRelative( 0.5257 * size,  0.382  * size);
      OutLineRelative( 0.5257 * size, -0.382  * size);
      OutLineRelative(-0.2008 * size, -0.6181 * size);
      OutLineRelative(-0.6499 * size,  0.0);
      Out("h");
      Out("B");
      break;
    case wxPDF_MARKER_PENTAGON_LEFT:
      SetLineWidth(size * 0.15);
      OutPoint(x - size * 0.1708, y + 0.5257 * size);
      OutLineRelative(-0.382  * size, -0.5257 * size);
      OutLineRelative( 0.382  * size, -0.5257 * size);
      OutLineRelative( 0.6181 * size,  0.2008 * size);
      OutLineRelative( 0.0,            0.6499 * size);
      Out("h");
      Out("B");
      break;
    case wxPDF_MARKER_PENTAGON_RIGHT:
      SetLineWidth(size * 0.15);
      OutPoint(x + size * 0.1708, y - 0.5257 * size);
      OutLineRelative( 0.382  * size,  0.5257 * size);
      OutLineRelative(-0.382  * size,  0.5257 * size);
      OutLineRelative(-0.6181 * size, -0.2008 * size);
      OutLineRelative( 0.0,           -0.6499 * size);
      Out("h");
      Out("B");
      break;
    case wxPDF_MARKER_BOWTIE_HORIZONTAL:
      SetLineWidth(size * 0.13);
      OutPoint(x - 0.5 * size, y - 0.5 * size);
      OutLine(x + 0.5 * size, y + 0.5 * size);
      OutLine(x + 0.5 * size, y - 0.5 * size);
      OutLine(x - 0.5 * size, y + 0.5 * size);
      Out("h");
      Out("B");
      break;
    case wxPDF_MARKER_BOWTIE_VERTICAL:
      SetLineWidth(size * 0.13);
      OutPoint(x - 0.5 * size, y - 0.5 * size);
      OutLine(x + 0.5 * size, y + 0.5 * size);
      OutLine(x - 0.5 * size, y + 0.5 * size);
      OutLine(x + 0.5 * size, y - 0.5 * size);
      Out("h");
      Out("B");
      break;
    case wxPDF_MARKER_ASTERISK:
      size *= 1.05;
      SetLineWidth(size * 0.15);
      OutPoint( x, y + size * 0.5);
      OutLineRelative(0.0, -size);
      OutPoint( x + 0.433 * size, y + 0.25 * size);
      OutLine(x - 0.433 * size, y - 0.25 * size);
      OutPoint(x + 0.433 * size, y - 0.25 * size);
      OutLine(x - 0.433 * size, y + 0.25 * size);
      Out("S");
      break;
    case wxPDF_MARKER_SUN:
      SetLineWidth(size * 0.15);
      halfsize = size * 0.25;
      OutPoint(x - halfsize, y);
      OutCurve(x - halfsize, y + b * halfsize, x + halfsize, y + b * halfsize, x + halfsize, y);
      OutCurve(x + halfsize, y - b * halfsize, x - halfsize, y - b * halfsize, x - halfsize, y);
      Out("h");
      OutPoint(x + size * 0.5, y);
      OutLine(x + size * 0.25, y);
      OutPoint(x - size * 0.5, y);
      OutLine(x - size * 0.25, y);
      OutPoint(x, y - size * 0.5);
      OutLine(x, y - size * 0.25);
      OutPoint(x, y + size * 0.5);
      OutLine(x, y + size * 0.25);
      Out("B");
      break;

    default:
      break;
  }
  Out("Q");
  m_x = x;
  m_y = y;
  SetLineWidth(saveLineWidth);
}

void
wxPdfDocument::Arrow(double x1, double y1, double x2, double y2, double linewidth, double height, double width)
{
  double saveLineWidth = m_lineWidth;
  double dx = x2 - x1;
  double dy = y2 - y1;
  double dz = sqrt (dx*dx+dy*dy);
  double sina = dy / dz;
  double cosa = dx / dz;
  double x3 = x2 - cosa * height + sina * width;
  double y3 = y2 - sina * height - cosa * width;
  double x4 = x2 - cosa * height - sina * width;
  double y4 = y2 - sina * height + cosa * width;

  SetLineWidth(0.2);

  //Draw a arrow head
  OutAscii(Double2String( x2*m_k,2) + wxString(_T(" ")) +
           Double2String( (m_h-y2)*m_k,2) + wxString(_T(" m ")) +
           Double2String( x3*m_k,2) + wxString(_T(" ")) +
           Double2String( (m_h-y3)*m_k,2) + wxString(_T(" l ")) +
           Double2String( x4*m_k,2) + wxString(_T(" ")) +
           Double2String( (m_h-y4)*m_k,2) + wxString(_T(" l b")));

  SetLineWidth(linewidth);
  Line(x1+cosa*linewidth, y1+sina*linewidth, x2-cosa*height, y2-sina*height);
  SetLineWidth(saveLineWidth);
}


