///////////////////////////////////////////////////////////////////////////////
// Name:        pdfdoc.cpp
// Purpose:     Implementation of wxPdfDocument and some helper classes
// Author:      Ulrich Telle
// Modified by:
// Created:     2005-08-04
// Copyright:   (c) Ulrich Telle
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/// \file pdfdoc.cpp Implementation of the wxPdfDoc class

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "pdfdoc.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/datetime.h"
#include "wx/paper.h"
#include "wx/wfstream.h"
#include "wx/zstream.h"
#include "wx/xml/xml.h"

#include "wx/pdfdoc.h"

#include "pdffontdata.inc"

// ----------------------------------------------------------------------------
// wxPdfLink: class representing internal or external links
// ----------------------------------------------------------------------------

wxPdfLink::wxPdfLink(int linkRef)
  : m_isRef(true), m_linkRef(linkRef), m_linkURL(wxEmptyString)
{
  m_isValid = linkRef > 0;
  m_page = 0;
  m_ypos = 0;
}

wxPdfLink::wxPdfLink(const wxString& linkURL)
  : m_isRef(false), m_linkRef(0), m_linkURL(linkURL)
{
  m_isValid = linkURL.Length() > 0;
}

wxPdfLink::wxPdfLink(const wxPdfLink& pdfLink)
{
  m_isValid = pdfLink.m_isValid;
  m_isRef   = pdfLink.m_isRef;
  m_linkRef = pdfLink.m_linkRef;
  m_linkURL = pdfLink.m_linkURL;
  m_page    = pdfLink.m_page;
  m_ypos    = pdfLink.m_ypos;
}

wxPdfLink::~wxPdfLink()
{
}

wxPdfPageLink::wxPdfPageLink(double x, double y, double w, double h, const wxPdfLink& pdfLink)
  : wxPdfLink(pdfLink)
{
  m_x = x;
  m_y = y;
  m_w = w;
  m_h = h;
}

wxPdfPageLink::~wxPdfPageLink()
{
}

// ----------------------------------------------------------------------------
// wxPdfBookmark: class representing bookmark objects for the document outline
// ----------------------------------------------------------------------------

wxPdfBookmark::wxPdfBookmark(const wxString& txt, int level, double y, int page)
{
  m_text = txt;
  m_level = level;
  m_y = y;
  m_page = page;

  m_parent = -1;
  m_prev   = -1;
  m_next   = -1;
  m_first  = -1;
  m_last   = -1;
}

wxPdfBookmark::~wxPdfBookmark()
{
}

// ----------------------------------------------------------------------------
// wxPdfLineStyle: class representing line style for drawing graphics
// ----------------------------------------------------------------------------

wxPdfLineStyle::wxPdfLineStyle(double width,
                               wxPdfLineCap cap, wxPdfLineJoin join,
                               const wxPdfArrayDouble& dash, double phase,
                               const wxColour& color)
{
  m_isSet = (width > 0) || (cap >= 0) || (join >= 0) || 
            (dash.GetCount() > 0) || color.Ok();
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

// ----------------------------------------------------------------------------
// wxPdfDocument: class representing a PDF document
// ----------------------------------------------------------------------------

wxPdfDocument::wxPdfDocument(int orientation, const wxString& unit, wxPaperSize format)
{
  // Allocate arrays
  m_currentFont = NULL;

  m_page       = 0;
  m_n          = 2;
  m_offsets.SetCount(m_n);

  m_pages = new PageHashMap();
  m_orientationChanges = new BoolHashMap();

  m_state      = 0;

  InitializeCoreFonts();
  m_fonts      = new FontHashMap();
  m_images     = new ImageHashMap();
  m_pageLinks  = new PageLinksMap();
  m_links      = new LinkHashMap();
  m_diffs      = new DiffHashMap();
  m_gradients  = new GradientMap();

  m_outlineRoot     = -1;
  m_maxOutlineLevel = 0;

  m_inFooter   = false;
  m_lasth      = 0;
  m_fontFamily = _T("");
  m_fontStyle  = _T("");
  m_fontSizePt = 12;
  m_underline  = false;
  m_drawColor  = _T("0 G");
  m_fillColor  = _T("0 g");
  m_textColor  = _T("0 g");
  m_colorFlag  = false;
  m_ws         = 0;

  // Scale factor
  if (unit == _T("pt"))
  {
    m_k = 1.;
  }
  else if (unit == _T("in"))
  {
    m_k = 72.;
  }
  else if (unit == _T("cm"))
  {
    m_k = 72. / 2.54;
  }
  else // if (unit == "mm") or unknown
  {
    m_k = 72. / 25.4;
  }

  // Page format
  wxPrintPaperDatabase* printPaperDatabase = new wxPrintPaperDatabase;
  printPaperDatabase->CreateDatabase();
  wxPrintPaperType* paperType = printPaperDatabase->FindPaperType(format);
  if (paperType == NULL)
  {
    paperType = printPaperDatabase->FindPaperType(wxPAPER_A4);
  }
  wxSize paperSize = paperType->GetSize();
  m_fwPt = paperSize.GetWidth() / 254. * 72.;
  m_fhPt = paperSize.GetHeight() / 254. * 72.;
  delete printPaperDatabase;

  m_fw = m_fwPt / m_k;
  m_fh = m_fhPt / m_k;

  // Page orientation
  if (orientation == wxLANDSCAPE)
  {
    m_defOrientation = wxLANDSCAPE;
    m_wPt = m_fhPt;
    m_hPt = m_fwPt;
  }
  else // orientation == wxPORTRAIT or unknown
  {
    m_defOrientation = wxPORTRAIT;
    m_wPt = m_fwPt;
    m_hPt = m_fhPt;
  }
  
  m_curOrientation = m_defOrientation;
  m_w = m_wPt / m_k;
  m_h = m_hPt / m_k;
  m_angle = 0;
  m_inTransform = 0;

  // Page margins (1 cm)
  double margin = 28.35 / m_k;
  SetMargins(margin, margin);
  
  // Interior cell margin (1 mm)
  m_cMargin = margin / 10;
  
  // Line width (0.2 mm)
  m_lineWidth = .567 / m_k;
  
  // Automatic page break
  SetAutoPageBreak(true, 2*margin);
  
  // Full width display mode
  SetDisplayMode(wxPDF_ZOOM_FULLWIDTH);
  m_zoomFactor = 100.;
  
  // Enable compression
  SetCompression(true);

  // Set default PDF version number
  m_PDFVersion = _T("1.3");

  m_encrypted = false;
  m_encryptor = NULL;
}

wxPdfDocument::~wxPdfDocument()
{
  delete m_coreFonts;

  FontHashMap::iterator font = m_fonts->begin();
  for (font = m_fonts->begin(); font != m_fonts->end(); font++)
  {
    if (font->second != NULL)
    {
      delete font->second;
    }
  }
  delete m_fonts;

  ImageHashMap::iterator image = m_images->begin();
  for (image = m_images->begin(); image != m_images->end(); image++)
  {
    if (image->second != NULL)
    {
      delete image->second;
    }
  }
  delete m_images;

  PageHashMap::iterator page = m_pages->begin();
  for (page = m_pages->begin(); page != m_pages->end(); page++)
  {
    if (page->second != NULL)
    {
      delete page->second;
    }
  }
  delete m_pages;

  PageLinksMap::iterator pageLinks = m_pageLinks->begin();
  for (pageLinks = m_pageLinks->begin(); pageLinks != m_pageLinks->end(); pageLinks++)
  {
    if (pageLinks->second != NULL)
    {
      delete pageLinks->second;
    }
  }
  delete m_pageLinks;

  LinkHashMap::iterator link = m_links->begin();
  for (link = m_links->begin(); link != m_links->end(); link++)
  {
    if (link->second != NULL)
    {
      delete link->second;
    }
  }
  delete m_links;

  size_t j;
  for (j = 0; j < m_outlines.GetCount(); j++)
  {
    wxPdfBookmark* bookmark = (wxPdfBookmark*) m_outlines[j];
    delete bookmark;
  }

  DiffHashMap::iterator diff = m_diffs->begin();
  for (diff = m_diffs->begin(); diff != m_diffs->end(); diff++)
  {
    if (diff->second != NULL)
    {
      delete diff->second;
    }
  }
  delete m_diffs;

  GradientMap::iterator gradient = m_gradients->begin();
  for (gradient = m_gradients->begin(); gradient != m_gradients->end(); gradient++)
  {
    if (gradient->second != NULL)
    {
      delete gradient->second;
    }
  }
  delete m_gradients;

  delete m_orientationChanges;

  if (m_encryptor != NULL)
  {
    delete m_encryptor;
  }
}

// --- Public methods

void
wxPdfDocument::SetProtection(int permissions,
                             const wxString& userPassword,
                             const wxString& ownerPassword)
{
  if (m_encryptor == NULL)
  {
    m_encryptor = new wxPdfEncrypt();
    m_encrypted = true;
    int allowedFlags = wxPDF_PERMISSION_PRINT | wxPDF_PERMISSION_MODIFY |
                       wxPDF_PERMISSION_COPY  | wxPDF_PERMISSION_ANNOT;
    int protection = 192;
    protection += (permissions & allowedFlags);
    wxString ownerPswd = ownerPassword;
    if (ownerPswd.Length() == 0)
    {
      ownerPswd = wxPdfDocument::GetUniqueId(_T("wxPdfDoc"));
    }
    m_encryptor->GenerateEncryptionKey(userPassword, ownerPswd, protection);
  }
}

void
wxPdfDocument::SetImageScale(double scale)
{
  m_imgscale = scale;
}

double
wxPdfDocument::GetImageScale()
{
  return m_imgscale;
}

double
wxPdfDocument::GetPageWidth()
{
  return m_w;
}

double
wxPdfDocument::GetPageHeight()
{
  return m_fh;
}

double
wxPdfDocument::GetBreakMargin()
{
  return m_bMargin;
}

double
wxPdfDocument::GetScaleFactor()
{
  return m_k;
}

void
wxPdfDocument::AliasNbPages(const wxString& alias)
{
  // Define an alias for total number of pages
  m_aliasNbPages = alias;
}

void
wxPdfDocument::Open()
{
  // Begin document
  m_state = 1;
}

void
wxPdfDocument::AddPage(int orientation)
{
  // Start a new page
  if (m_state == 0)
  {
    Open();
  }
  wxString family = m_fontFamily;
  wxString style = m_fontStyle;
  if (m_underline)
  {
    style += wxString(_T("U"));
  }
  double size = m_fontSizePt;
  double lw = m_lineWidth;
  wxString dc = m_drawColor;
  wxString fc = m_fillColor;
  wxString tc = m_textColor;
  bool cf = m_colorFlag;

  if (m_page > 0)
  {
    // Page footer
    m_inFooter = true;
    Footer();
    m_inFooter = false;
    // Close page
    EndPage();
  }

  // Start new page
  BeginPage(orientation);
  
  // Set line cap style to square
  Out("2 J");
  
  // Set line width
  m_lineWidth = lw;
  OutAscii(Double2String(lw*m_k,2)+wxString(_T(" w")));

  // Set font
  if (family.Length() > 0)
  {
    SetFont(family, style, size);
  }
  
  // Set colors
  m_drawColor = dc;
  if (dc != _T("0 G"))
  {
    OutAscii(dc);
  }
  m_fillColor = fc;
  if (fc != _T("0 g"))
  {
    OutAscii(fc);
  }
  m_textColor = tc;
  m_colorFlag = cf;

  // Page header
  Header();

  // Restore line width
  if (m_lineWidth != lw)
  {
    m_lineWidth = lw;
    OutAscii(Double2String(lw*m_k,2)+wxString(_T(" w")));
  }

  // Restore font
  if(family.Length() > 0)
  {
    SetFont(family, style, size);
  }
  
  // Restore colors
  if (m_drawColor != dc)
  {
    m_drawColor = dc;
    OutAscii(dc);
  }
  if (m_fillColor != fc)
  {
    m_fillColor = fc;
    OutAscii(fc);
  }
  m_textColor = tc;
  m_colorFlag = cf;
}

void
wxPdfDocument::SetDrawColor(const wxColour& color)
{
  m_drawColor = RGB2String(color) + _T(" RG");
  if (m_page > 0)
  {
    OutAscii(m_drawColor);
  }
}

void
wxPdfDocument::SetDrawColor(const unsigned char grayscale)
{
  m_drawColor = Double2String(((double) grayscale)/255.,3) + _T(" G");
  if (m_page > 0)
  {
    OutAscii(m_drawColor);
  }
}

void
wxPdfDocument::SetFillColor(const wxColour& color)
{
  m_fillColorOrig = color;
  m_fillColor = RGB2String(color) + _T(" rg");
  m_colorFlag = (m_fillColor != m_textColor);
  if (m_page > 0)
  {
    OutAscii(m_fillColor);
  }
}

void
wxPdfDocument::SetFillColor(const unsigned char grayscale)
{
  m_fillColorOrig = wxColour(grayscale, grayscale, grayscale);
  m_fillColor = Double2String(((double) grayscale)/255.,3) + _T(" g");
  m_colorFlag = (m_fillColor != m_textColor);
  if (m_page > 0)
  {
    OutAscii(m_fillColor);
  }
}

const wxColour
wxPdfDocument::GetFillColor()
{
  return m_fillColorOrig;
}

void
wxPdfDocument::SetTextColor(const wxColour& color)
{
  m_textColor = RGB2String(color) + _T(" rg");
  m_colorFlag = (m_fillColor != m_textColor);
}

void
wxPdfDocument::SetTextColor(const unsigned char grayscale)
{
  m_textColor = Double2String(((double) grayscale)/255.,3) + _T(" g");
  m_colorFlag = (m_fillColor != m_textColor);
}

void
wxPdfDocument::SetLineWidth(double width)
{
  // Set line width
  m_lineWidth = width;
  if (m_page > 0)
  {
    OutAscii(Double2String(width*m_k,2)+ wxString(_T(" w")));
  }
}

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
                              int circleStyle, const wxPdfLineStyle& circleLineStyle, const wxColour& circleFillColor)
{
  if (ns < 3)
  {
    ns = 3;
  }
  if (circle)
  {
    wxPdfLineStyle saveStyle = GetLineStyle();
    SetLineStyle(circleLineStyle);
    wxColour saveColor = GetFillColor();
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
                           int circleStyle, const wxPdfLineStyle& circleLineStyle, const wxColour& circleFillColor)
{
  if (nv < 2)
  {
    nv = 2;
  }
  if (circle)
  {
    wxPdfLineStyle saveStyle = GetLineStyle();
    SetLineStyle(circleLineStyle);
    wxColour saveColor = GetFillColor();
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

#if 0
void
wxPdfDocument::Rotate(double angle, double x, double y)
{
  if (x < 0)
  {
    x = m_x;
  }
  if (y < 0)
  {
    y = m_y;
  }
  if (m_angle != 0)
  {
    Out("Q");
  }
  m_angle = angle;
  if (angle != 0)
  {
    angle *= (atan(1.) / 45.);
    double c = cos(angle);
    double s = sin(angle);
    double cx = x * m_k;
    double cy = (m_h - y) * m_k;
    OutAscii(wxString(_T("q ")) +
             Double2String(  c,5) + wxString(_T(" ")) +
             Double2String(  s,5) + wxString(_T(" ")) +
             Double2String( -s,5) + wxString(_T(" ")) +
             Double2String(  c,5) + wxString(_T(" ")) +
             Double2String( cx,2) + wxString(_T(" ")) +
             Double2String( cy,2) + wxString(_T(" cm 1 0 0 1 ")) +
             Double2String(-cx,2) + wxString(_T(" ")) +
             Double2String(-cy,2) + wxString(_T(" cm")));
  }
}
#endif

bool
wxPdfDocument::AddFont(const wxString& family, const wxString& style, const wxString& file)
{
  if (family.Length() == 0) return false;

  // Add a TrueType or Type1 font
  wxString lcFamily = family.Lower();
  wxString lcStyle = style.Lower();
  wxString ucStyle = style.Upper();

  wxString fileName = file;
  if (fileName.Length() == 0)
  {
    fileName = lcFamily + lcStyle + wxString(_T(".xml"));
    fileName.Replace(_T(" "),_T(""));
  }

  if (ucStyle == _T("IB"))
  {
    ucStyle = _T("BI");
  }

  // check if the font has been already added
  wxString fontkey = lcFamily + ucStyle;
  FontHashMap::iterator font = (*m_fonts).find(fontkey);
  if (font != (*m_fonts).end())
  {
    // Font already loaded
    return true;
  }

  // Open font metrics XML file
  wxFileName fontFileName(fileName);
  fontFileName.MakeAbsolute(GetFontPath());
  wxFileSystem fs;
  wxFSFile* xmlFontMetrics = fs.OpenFile(fontFileName.GetFullPath());
  if (!xmlFontMetrics)
  {
    // Font metrics XML file not found
    wxLogDebug(_T("wxPdfDocument::AddFont: Font metrics file '%s' not found."), fileName.c_str());
    return false;
  }

  // Load the XML file
  wxXmlDocument fontMetrics;
  bool loaded = fontMetrics.Load(*xmlFontMetrics->GetStream());
  delete xmlFontMetrics;
  if (!loaded)
  {
    // Font metrics file loading failed
    wxLogDebug(_T("wxPdfDocument::AddFont: Loading of font metrics file '%s' failed."), fileName.c_str());
    return false;
  }
  if (!fontMetrics.IsOk() || fontMetrics.GetRoot()->GetName() != wxT("wxpdfdoc-font-metrics"))
  {
    // Not a font metrics file
    wxLogDebug(_T("wxPdfDocument::AddFont: Font metrics file '%s' invalid."), fileName.c_str());
    return false;
  }

  wxString fontType;
  wxXmlNode* root = fontMetrics.GetRoot();
  if (!root->GetPropVal(_T("type"), &fontType))
  {
    // Font type not specified
    wxLogDebug(_T("wxPdfDocument::AddFont: Font type not specified for font '%s'."), family.c_str());
    return false;
  }

  int i = (*m_fonts).size() + 1;
  wxPdfFont* addedFont = NULL;
  if (fontType == _T("TrueType"))
  {
    addedFont = new wxPdfFontTrueType(i);
  }
  else if (fontType == _T("Type1"))
  {
    addedFont = new wxPdfFontType1(i);
  }
#if wxUSE_UNICODE
  else if (fontType == _T("TrueTypeUnicode"))
  {
    addedFont = new wxPdfFontTrueTypeUnicode(i);
  }
  else if (fontType == _T("Type0"))
  {
    addedFont = new wxPdfFontType0(i);
  }
#endif
  else
  {
    // Unknown font type
    wxLogDebug(_T("wxPdfDocument::AddFont: Unknown font type '%s'."), fontType.c_str());
    return false;
  }
  if (!addedFont->LoadFontMetrics(root))
  {
    delete addedFont;
    return false;
  }
  (*m_fonts)[fontkey] = addedFont;

  if (addedFont->HasDiffs())
  {
    // Search existing encodings
    int d = 0;
    int nb = (*m_diffs).size();
    for (i = 1; i <= nb; i++)
    {
      if (*(*m_diffs)[i] == addedFont->GetDiffs())
      {
        d = i;
        break;
      }
    }
    if (d == 0)
    {
      d = nb + 1;
      (*m_diffs)[d] = new wxString(addedFont->GetDiffs());
    }
    addedFont->SetDiffIndex(d);
  }

  return true;
}

#if wxUSE_UNICODE

bool
wxPdfDocument::AddFontCJK(const wxString& family)
{
  wxString lcFamily = family.Lower();
  wxString fontFile = lcFamily + wxString(_T(".xml"));
  wxString fontkey = lcFamily;
  wxString fontName;
  bool valid;

  FontHashMap::iterator font = (*m_fonts).find(fontkey);
  if (font != (*m_fonts).end())
  {
    return true;
  }

  valid = AddFont(family, _T(""), fontFile);
  if (valid)
  {
    // Add all available styles (bold, italic and bold-italic)
    // For all styles the same font metric file is used, therefore
    // the font name has to be changed afterwards to reflect the
    // style.
    AddFont(family, _T("B"), fontFile);
    fontkey = lcFamily + wxString(_T("B"));
    font = (*m_fonts).find(fontkey);
    fontName = font->second->GetName();
    fontName += wxString(_T(",Bold"));
    font->second->SetName(fontName);

    AddFont(family, _T("I"), fontFile);
    fontkey = lcFamily + wxString(_T("I"));
    font = (*m_fonts).find(fontkey);
    fontName = font->second->GetName();
    fontName += wxString(_T(",Italic"));
    font->second->SetName(fontName);
    
    AddFont(family, _T("BI"), fontFile);
    fontkey = lcFamily + wxString(_T("BI"));
    font = (*m_fonts).find(fontkey);
    fontName = font->second->GetName();
    fontName += wxString(_T(",BoldItalic"));
    font->second->SetName(fontName);
  }
  return valid;
}

#endif // wxUSE_UNICODE

bool
wxPdfDocument::SetFont(const wxString& family, const wxString& style, double size)
{
  // Select a font; size given in points

  wxString ucStyle = style.Upper();
  wxString lcFamily = family.Lower();
  if (lcFamily.Length() == 0)
  {
    lcFamily = m_fontFamily;
  }
  if (lcFamily == _T("arial"))
  {
    lcFamily = _T("helvetica");
  }
  else if (lcFamily == _T("symbol") || lcFamily == _T("zapfdingbats"))
  {
    ucStyle = wxEmptyString;
  }
  if (ucStyle.Find(_T('U')) >= 0)
  {
    m_underline = true;
    ucStyle.Replace(_T("U"),_T(""));
  }
  else
  {
    m_underline = false;
  }
  if (ucStyle == _T("IB"))
  {
    ucStyle = _T("BI");
  }
  if (size == 0)
  {
    size = m_fontSizePt;
  }

  // Test if font is already selected
  if (m_fontFamily == lcFamily && m_fontStyle == ucStyle && m_fontSizePt == size)
  {
    return true;
  }

  // Test if used for the first time
  wxPdfFont* currentFont = NULL;
  wxString fontkey = lcFamily + ucStyle;
  FontHashMap::iterator font = (*m_fonts).find(fontkey);
  if (font == (*m_fonts).end())
  {
    // Check if one of the standard fonts
    CoreFontMap::iterator coreFont = (*m_coreFonts).find(fontkey);
    if (coreFont != (*m_coreFonts).end())
    {
      int i = (*m_fonts).size() + 1;
      int j = coreFont->second;
      currentFont = new wxPdfFont(i, wxCoreFontTable[j].name, wxCoreFontTable[j].cwArray);
      (*m_fonts)[fontkey] = currentFont;
    }
    else
    {
      // Undefined font
      wxLogDebug(_T("wxPdfDocument::SetFont: Undefined font: '%s %s'."), family.c_str(), style.c_str());
      return false;
    }
  }
  else
  {
    currentFont = font->second;
  }

  // Select it
  m_fontFamily  = lcFamily;
  m_fontStyle   = ucStyle;
  m_fontSizePt  = size;
  m_fontSize    = size / m_k;
  m_currentFont = currentFont;
  if (m_page > 0)
  {
    OutAscii(wxString::Format(_T("BT /F%d "),m_currentFont->GetIndex()) +
             Double2String(m_fontSizePt,2) + wxString(_T(" Tf ET")));
  }
  return true;
}

void
wxPdfDocument::SetFontSize(double size)
{
  // Set font size in points
  if (m_fontSizePt == size)
  {
    return;
  }
  m_fontSizePt = size;
  m_fontSize = size / m_k;
  if ( m_page > 0)
  {
    OutAscii(wxString::Format(_T("BT /F%d "),m_currentFont->GetIndex()) +
             Double2String(m_fontSizePt,2) + wxString(_T(" Tf ET")));
  }
}

double
wxPdfDocument::GetStringWidth(const wxString& s)
{
  double w = 0;
  if (m_currentFont != 0)
  {
    w = m_currentFont->GetStringWidth(s) * m_fontSize;
  }
  return w;
}

int
wxPdfDocument::AddLink()
{
  // Create a new internal link
  int n = (*m_links).size()+1;
  (*m_links)[n] = new wxPdfLink(n);
  return n;
}

bool
wxPdfDocument::SetLink(int link, double ypos, int page)
{
  bool isValid = false;
  // Set destination of internal link
  if (ypos == -1)
  {
    ypos = m_y;
  }
  if (page == -1)
  {
    page = m_page;
  }
  LinkHashMap::iterator pLink = (*m_links).find(link);
  if (pLink != (*m_links).end())
  {
    isValid = true;
    wxPdfLink* currentLink = pLink->second;
    currentLink->SetLink(page,ypos);
  }
  return isValid;
}

void
wxPdfDocument::Link(double x, double y, double w, double h, const wxPdfLink& link)
{
  // Put a link on the page
  wxArrayPtrVoid* pageLinkArray = NULL;
  wxPdfPageLink* pageLink = new wxPdfPageLink(x*m_k, m_hPt-y*m_k, w*m_k, h*m_k, link);
  PageLinksMap::iterator pageLinks = (*m_pageLinks).find(m_page);
  if (pageLinks != (*m_pageLinks).end())
  {
    pageLinkArray = pageLinks->second;
  }
  else
  {
    pageLinkArray = new wxArrayPtrVoid;
    (*m_pageLinks)[m_page] = pageLinkArray;
  }
  pageLinkArray->Add(pageLink);
}

void
wxPdfDocument::Bookmark(const wxString& txt, int level, double y)
{
  if (y < 0)
  {
    y = GetY();
  }
  wxPdfBookmark* bookmark = new wxPdfBookmark(txt, level, y, PageNo());
  m_outlines.Add(bookmark);
  if (level > m_maxOutlineLevel)
  {
    m_maxOutlineLevel = level;
  }
}

void
wxPdfDocument::Text(double x, double y, const wxString& txt)
{
  // Output a string
  if (m_colorFlag)
  {
    Out("q ", false);
    OutAscii(m_textColor, false);
    Out(" ", false);
  }
  OutAscii(wxString(_T("BT ")) +
           Double2String(x*m_k,2) + wxString(_T(" ")) +
           Double2String((m_h-y)*m_k,2) + wxString(_T(" Td (")), false);
  TextEscape(txt,false);
  Out(") Tj ET", false);

  if (m_underline && txt.Length() > 0)
  {
    Out(" ", false);
    OutAscii(DoUnderline(x, y, txt), false);
  }

  if (m_colorFlag)
  {
    Out(" Q", false);
  }
  Out("\n", false);
}

void
wxPdfDocument::RotatedText(double x, double y, const wxString& txt, double angle)
{
  // Text rotated around its origin
  StartTransform();
  Rotate(angle, x, y);
  Text(x, y, txt);
//  TODO Rotate(0);
  StopTransform();
}

bool
wxPdfDocument::AcceptPageBreak()
{
  // Accept automatic page break or not
  return m_autoPageBreak;
}

void
wxPdfDocument::Cell(double w, double h, const wxString& txt, int border, int ln, int align, int fill, const wxPdfLink& link)
{
  // Output a cell
  double x, y;
  double k = m_k;
  if (m_y + h > m_pageBreakTrigger && !m_inFooter && AcceptPageBreak())
  {
    // Automatic page break
    x = m_x;
    double ws = m_ws;
    if (ws > 0)
    {
      m_ws = 0;
      Out("0 Tw");
    }
    AddPage(m_curOrientation);
    m_x = x;
    if (ws > 0)
    {
      m_ws = ws;
      OutAscii(Double2String(ws*k,3)+wxString(_T(" Tw")));
    }
  }
  if ( w == 0)
  {
    w = m_w - m_rMargin - m_x;
  }
  wxString s = wxEmptyString;
  if (fill == 1 || border == wxPDF_BORDER_FRAME)
  {
    s = Double2String(m_x*k,2) + wxString(_T(" ")) +
        Double2String((m_h-m_y)*k,2) + wxString(_T(" ")) +
        Double2String(w*k,2) + wxString(_T(" ")) +
        Double2String(-h*k,2);
    if (fill == 1)
    {
      if (border == wxPDF_BORDER_FRAME)
      {
        s += wxString(_T(" re B "));
      }
      else
      {
        s += wxString(_T(" re f "));
      }
    }
    else
    {
      s += wxString(_T(" re S "));
    }
  }
  if (border != wxPDF_BORDER_NONE && border != wxPDF_BORDER_FRAME)
  {
    x = m_x;
    y = m_y;
    if (border & wxPDF_BORDER_LEFT)
    {
      s += Double2String(x*k,2) + wxString(_T(" ")) +
           Double2String((m_h-y)*k,2) + wxString(_T(" m ")) +
           Double2String(x*k,2) + wxString(_T(" ")) +
           Double2String((m_h-(y+h))*k,2) + wxString(_T(" l S "));
    }
    if (border & wxPDF_BORDER_TOP)
    {
      s += Double2String(x*k,2) + wxString(_T(" ")) +
           Double2String((m_h-y)*k,2) + wxString(_T(" m ")) +
           Double2String((x+w)*k,2) + wxString(_T(" ")) +
           Double2String((m_h-y)*k,2) + wxString(_T(" l S "));
    }
    if (border & wxPDF_BORDER_RIGHT)
    {
      s += Double2String((x+w)*k,2) + wxString(_T(" ")) +
           Double2String((m_h-y)*k,2) + wxString(_T(" m ")) +
           Double2String((x+w)*k,2) + wxString(_T(" ")) +
           Double2String((m_h-(y+h))*k,2) + wxString(_T(" l S "));
    }
    if (border & wxPDF_BORDER_BOTTOM)
    {
      s += Double2String(x*k,2) + wxString(_T(" ")) +
           Double2String((m_h-(y+h))*k,2) + wxString(_T(" m ")) +
           Double2String((x+w)*k,2) + wxString(_T(" ")) +
           Double2String((m_h-(y+h))*k,2) + wxString(_T(" l S "));
    }
  }
  if (s.Length() > 0)
  {
    bool newline = txt.Length() == 0;
    OutAscii(s, newline);
    s = _T("");
  }
  
  if (txt.Length() > 0)
  {
    double width = GetStringWidth(txt);
    double dx;
    if (align == wxPDF_ALIGN_RIGHT)
    {
      dx = w - m_cMargin - width;
    }
    else if (align == wxPDF_ALIGN_CENTER)
    {
      dx = (w - width) / 2;
    }
    else
    {
      dx = m_cMargin;
    }
    if (m_colorFlag)
    {
      s += wxString(_T("q ")) + m_textColor + wxString(_T(" "));
    }
    s += wxString(_T("BT ")) +
         Double2String((m_x+dx)*k,2) + wxString(_T(" ")) +
         Double2String((m_h-(m_y+.5*h+.3*m_fontSize))*k,2) + wxString(_T(" Td ("));
    OutAscii(s,false);
    TextEscape(txt,false);
    s = _T(") Tj ET");

    if (m_underline)
    {
      s += wxString(_T(" ")) + DoUnderline(m_x+dx,m_y+.5*h+.3*m_fontSize,txt);
    }
    if (m_colorFlag)
    {
      s += wxString(_T(" Q"));
    }
    if (link.IsValid())
    {
      Link(m_x+dx,m_y+.5*h-.5*m_fontSize,width,m_fontSize,link);
    }
    OutAscii(s);
  }
  m_lasth = h;
  if (ln > 0)
  {
    // Go to next line
    m_y += h;
    if ( ln == 1)
    {
      m_x = m_lMargin;
    }
  }
  else
  {
    m_x += w;
  }
}

void
wxPdfDocument::MultiCell(double w, double h, const wxString& txt, int border, int align, int fill)
{
  // Output text with automatic or explicit line breaks
  if (w == 0)
  {
    w = m_w - m_rMargin - m_x;
  }

  double wmax = (w - 2 * m_cMargin);
  wxString s = txt;
  s.Replace(_T("\r"),_T("")); // remove carriage returns
  int nb = s.Length();
  if (nb > 0 && s[nb-1] == _T('\n'))
  {
    nb--;
  }

  int b = wxPDF_BORDER_NONE;
  int b2 = wxPDF_BORDER_NONE;
  if (border != wxPDF_BORDER_NONE)
  {
    if (border == wxPDF_BORDER_FRAME)
    {
      b = wxPDF_BORDER_LEFT | wxPDF_BORDER_RIGHT | wxPDF_BORDER_TOP;
      b2 = wxPDF_BORDER_LEFT | wxPDF_BORDER_RIGHT;
    }
    else
    {
      b2 = wxPDF_BORDER_NONE;
      if (border & wxPDF_BORDER_LEFT)
      {
        b2 = b2 | wxPDF_BORDER_LEFT;
      }
      if (border & wxPDF_BORDER_RIGHT)
      {
        b2 = b2 | wxPDF_BORDER_RIGHT;
      }
      b = (border & wxPDF_BORDER_TOP) ? b2 | wxPDF_BORDER_TOP : b2;
    }
  }
  int sep = -1;
  int i = 0;
  int j = 0;
  double len = 0;
  double ls = 0;
  int ns = 0;
  int nl = 1;
  wxChar c;
  while (i < nb)
  {
    // Get next character
    c = s[i];
    if (c == _T('\n'))
    {
      // Explicit line break
      if (m_ws > 0)
      {
        m_ws = 0;
        Out("0 Tw");
      }
      Cell(w,h,s.SubString(j,i-1),b,2,align,fill);
      i++;
      sep = -1;
      j = i;
      len = 0;
      ns = 0;
      nl++;
      if (border != wxPDF_BORDER_NONE && nl == 2)
      {
        b = b2;
      }
      continue;
    }
    if (c == _T(' '))
    {
      sep = i;
      ls = len;
      ns++;
    }
    len = GetStringWidth(s.SubString(j, i));

    if (len > wmax)
    {
      // Automatic line break
      if (sep == -1)
      {
        if (i == j)
        {
          i++;
        }
        if (m_ws > 0)
        {
          m_ws=0;
          Out("0 Tw");
        }
        Cell(w,h,s.SubString(j,i-1),b,2,align,fill);
      }
      else
      {
        if (align == wxPDF_ALIGN_JUSTIFY)
        {
          m_ws = (ns > 1) ? (wmax - ls)/(ns-1) : 0;
          OutAscii(Double2String(m_ws*m_k,3)+wxString(_T(" Tw")));
        }
        Cell(w,h,s.SubString(j,sep-1),b,2,align,fill);
        i = sep + 1;
      }
      sep = -1;
      j = i;
      len = 0;
      ns = 0;
      nl++;
      if (border != wxPDF_BORDER_NONE && nl == 2)
      {
        b = b2;
      }
    }
    else
    {
      i++;
    }
  }
  // Last chunk
  if (m_ws > 0)
  {
    m_ws = 0;
    Out("0 Tw");
  }
  if ((border != wxPDF_BORDER_NONE) && (border & wxPDF_BORDER_BOTTOM))
  {
    b = b | wxPDF_BORDER_BOTTOM;
  }
  Cell(w,h,s.SubString(j,i-1),b,2,align,fill);
  m_x = m_lMargin;
}

void
wxPdfDocument::Write(double h, const wxString& txt, const wxPdfLink& link)
{
  WriteCell(h, txt, wxPDF_BORDER_NONE, 0, link);
}

void
wxPdfDocument::WriteCell(double h, const wxString& txt, int border, int fill, const wxPdfLink& link)
{
  // Output text in flowing mode
  wxString s = txt;
  s.Replace(_T("\r"),_T("")); // remove carriage returns
  int nb = s.Length();

  // handle single space character
  if ((nb == 1) && s[0] == _T(' '))
  {
    m_x += GetStringWidth(s);
    return;
  }

  double saveCellMargin = GetCellMargin();
  SetCellMargin(0);

  double w = m_w - m_rMargin - m_x;
  double wmax = (w - 2 * m_cMargin);

  int sep = -1;
  int i = 0;
  int j = 0;
  double len=0;
  int nl = 1;
  wxChar c;
  while (i < nb)
  {
    // Get next character
    c = s[i];
    if (c == _T('\n'))
    {
      // Explicit line break
      Cell(w, h, s.SubString(j,i-1), border, 2, wxPDF_ALIGN_LEFT, fill, link);
      i++;
      sep = -1;
      j = i;
      len = 0;
      if (nl == 1)
      {
        m_x = m_lMargin;
        w = m_w - m_rMargin - m_x;
        wmax = (w - 2 * m_cMargin);
      }
      nl++;
      continue;
    }
    if (c == _T(' '))
    {
      sep = i;
    }
    len = GetStringWidth(s.SubString(j, i));
    if (len > wmax)
    {
      // Automatic line break
      if (sep == -1)
      {
        if (m_x > m_lMargin)
        {
          // Move to next line
          m_x = m_lMargin;
          m_y += h;
          w = m_w - m_rMargin -m_x;
          wmax = (w - 2 * m_cMargin);
          i++;
          nl++;
          continue;
        }
        if (i == j)
        {
          i++;
        }
        Cell(w, h,s.SubString(j, i-1), border, 2, wxPDF_ALIGN_LEFT, fill, link);
      }
      else
      {
        Cell(w, h, s.SubString(j, sep-1), border, 2, wxPDF_ALIGN_LEFT, fill, link);
        i = sep + 1;
      }
      sep = -1;
      j = i;
      len = 0;
      if (nl == 1)
      {
        m_x = m_lMargin;
        w = m_w - m_rMargin - m_x;
        wmax = (w - 2 * m_cMargin);
      }
      nl++;
    }
    else
    {
      i++;
    }
  }
  // Last chunk
  if (i != j)
  {
    Cell(len, h, s.SubString(j,i-1), border, 0, wxPDF_ALIGN_LEFT, fill, link);
  }

  // Following statement was in PHP code, but seems to be in error.
  // m_x += GetStringWidth(s.SubString(j, i-1));
  SetCellMargin(saveCellMargin);
}

bool
wxPdfDocument::Image(const wxString& file, double x, double y, double w, double h,
                     const wxString& type, const wxPdfLink& link)
{
  bool isValid = false;
  wxPdfImage* currentImage = NULL;
  // Put an image on the page
  ImageHashMap::iterator image = (*m_images).find(file);
  if (image == (*m_images).end())
  {
    // First use of image, get info
    int i = (*m_images).size() + 1;
    currentImage = new wxPdfImage(this, i, file, type);
    if (!currentImage->Parse())
    {
      delete currentImage;
      return false;
    }
    (*m_images)[file] = currentImage;
  }
  else
  {
    currentImage = image->second;
  }
  
  // Automatic width and height calculation if needed
  if (w == 0 && h == 0)
  {
    // Put image at 72 dpi, apply scale factor
    if (currentImage->IsFormObject())
    {
      w = currentImage->GetWidth() / (20 * m_imgscale * m_k);
      h = currentImage->GetHeight() / (20 * m_imgscale * m_k);
    }
    else
    {
      w = currentImage->GetWidth() / (m_imgscale * m_k);
      h = currentImage->GetHeight() / (m_imgscale * m_k);
    }
  }
  if (w == 0)
  {
    w = (h * currentImage->GetWidth()) / currentImage->GetHeight();
  }
  if (h == 0)
  {
    h = (w * currentImage->GetHeight()) / currentImage->GetWidth();
  }

  double sw, sh, sx, sy;
  if (currentImage->IsFormObject())
  {
    sw = w * m_k / currentImage->GetWidth();
    sh = -h * m_k / currentImage->GetHeight();
    sx = x * m_k - sw * currentImage->GetX();
    sy = ((m_h - y) * m_k) - sh * currentImage->GetY();
  }
  else
  {
    sw = w * m_k;
    sh = h * m_k;
    sx = x * m_k;
    sy = (m_h-(y+h))*m_k;
  }
  OutAscii(wxString(_T("q ")) +
           Double2String(sw,2) + wxString(_T(" 0 0 ")) +
           Double2String(sh,2) + wxString(_T(" ")) +
           Double2String(sx,2) + wxString(_T(" ")) +
           Double2String(sy,2) + 
           wxString::Format(_T(" cm /I%d Do Q"),currentImage->GetIndex()));

  if (link.IsValid())
  {
    Link(x,y,w,h,link);
  }

  // set right-bottom corner coordinates
  m_img_rb_x = x + w;
  m_img_rb_y = y + h;

  return isValid;
}

void
wxPdfDocument::RotatedImage(const wxString& file, double x, double y, double w, double h,
                            double angle, const wxString& type, const wxPdfLink& link)
{
  // Image rotated around its upper-left corner
  StartTransform();
  Rotate(angle, x, y);
  Image(file, x, y, w, h, type, link);
  // TODO Rotate(0);
  StopTransform();
}

void
wxPdfDocument::Ln(double h)
{
  // Line feed; default value is last cell height
  m_x = m_lMargin;
  if (h < 0)
  {
    m_y += m_lasth;
  }
  else
  {
    m_y += h;
  }
}

void
wxPdfDocument::SaveAsFile(const wxString& name)
{
  wxString fileName = name;
  // Finish document if necessary
  if (m_state < 3)
  {
    Close();
  }
  // Normalize parameters
  if(fileName.Length() == 0)
  {
    fileName = _T("doc.pdf");
  }
  // Save to local file
  wxFileOutputStream outfile(fileName);
  wxMemoryInputStream tmp(m_buffer);
  outfile.Write(tmp);
  outfile.Close();
}

void
wxPdfDocument::SetTitle(const wxString& title)
{
  // Title of document
  m_title = title;
}

void
wxPdfDocument::SetSubject(const wxString& subject)
{
  // Subject of document
  m_subject = subject;
}

void
wxPdfDocument::SetAuthor(const wxString& author)
{
  // Author of document
  m_author = author;
}

void
wxPdfDocument::SetKeywords(const wxString& keywords)
{
  // Keywords of document
  m_keywords = keywords;
}

void
wxPdfDocument::SetCreator(const wxString& creator)
{
  // Creator of document
  m_creator = creator;
}

void
wxPdfDocument::SetMargins(double left, double top, double right)
{
  // Set left, top and right margins
  m_lMargin = left;
  m_tMargin = top;
  if (right == -1)
  {
    right = left;
  }
  m_rMargin = right;
}

void
wxPdfDocument::SetLeftMargin(double margin)
{
  // Set left margin
  m_lMargin = margin;
  if (m_page > 0 && m_x < margin)
  {
    m_x = margin;
  }
}

double
wxPdfDocument::GetLeftMargin()
{
  return m_lMargin;
}

void
wxPdfDocument::SetTopMargin(double margin)
{
  // Set top margin
  m_tMargin = margin;
}

double
wxPdfDocument::GetTopMargin()
{
  return m_tMargin;
}

void
wxPdfDocument::SetRightMargin(double margin)
{
  // Set right margin
  m_rMargin = margin;
}

double
wxPdfDocument::GetRightMargin()
{
  return m_rMargin;
}

void
wxPdfDocument::SetCellMargin(double margin)
{
  // Set cell margin
  m_cMargin = margin;
}

double
wxPdfDocument::GetCellMargin()
{
  return m_cMargin;
}

void
wxPdfDocument::SetAutoPageBreak(bool autoPageBreak, double margin)
{
  // Set auto page break mode and triggering margin
  m_autoPageBreak = autoPageBreak;
  m_bMargin = margin;
  m_pageBreakTrigger = m_h - margin;
}

void
wxPdfDocument::SetDisplayMode(wxPdfZoom zoom, wxPdfLayout layout, double zoomFactor)
{
  // Set display mode in viewer
  switch (zoom)
  {
    case wxPDF_ZOOM_FULLPAGE:
    case wxPDF_ZOOM_FULLWIDTH:
    case wxPDF_ZOOM_REAL:
    case wxPDF_ZOOM_DEFAULT:
      m_zoomMode = zoom;
      break;
    case wxPDF_ZOOM_FACTOR:
      m_zoomMode = zoom;
      m_zoomFactor = (zoomFactor > 0) ? zoomFactor : 100.;
      break;
    default:
      m_zoomMode = wxPDF_ZOOM_FULLWIDTH;
      break;
  }

  switch (layout)
  {
    case wxPDF_LAYOUT_SINGLE:
    case wxPDF_LAYOUT_TWO:
    case wxPDF_LAYOUT_DEFAULT:
    case wxPDF_LAYOUT_CONTINUOUS:
      m_layoutMode = layout;
      break;
    default:
      m_layoutMode = wxPDF_LAYOUT_CONTINUOUS;
      break;
  }
}

void
wxPdfDocument::Close()
{
  // Terminate document
  if (m_state == 3)
  {
    return;
  }
  if (m_page == 0)
  {
    AddPage();
  }
  
  // Page footer
  m_inFooter = true;
  Footer();
  m_inFooter = false;

  // Close page
  EndPage();

  // Close document
  EndDoc();
}

void
wxPdfDocument::Header()
{
  // To be implemented in your own inherited class
}

void
wxPdfDocument::Footer()
{
  // To be implemented in your own inherited class
}

int
wxPdfDocument::PageNo()
{
  // Get current page number
  return m_page;
}

double
wxPdfDocument::GetX()
{
  // Get x position
  return m_x;
}

void
wxPdfDocument::SetX(double x)
{
  // Set x position
  if ( x >= 0.0)
  {
    m_x = x;
  }
  else
  {
    m_x = m_w + x;
  }
}

double
wxPdfDocument::GetY()
{
  // Get y position
  return m_y;
}

void
wxPdfDocument::SetY(double y)
{
  // Set y position and reset x
  m_x = m_lMargin;
  if ( y >= 0)
  {
    m_y = y;
  }
  else
  {
    m_y = m_h + y;
  }
}

void
wxPdfDocument::SetXY(double x, double y)
{
  // Set x and y positions
  SetY(y);
  SetX(x);
}

void
wxPdfDocument::SetCompression(bool compress)
{
  m_compress = compress;
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

// --- Protected methods

wxString
wxPdfDocument::GetFontPath()
{
  wxString fontPath;
  if (!wxGetEnv(_T("WXPDF_FONTPATH"), &fontPath))
  {
    fontPath = wxGetCwd();
    if (!wxEndsWithPathSeparator(fontPath))
    {
      fontPath += wxFILE_SEP_PATH;
    }
    fontPath += _T("fonts");
  }
  return fontPath;
}

void
wxPdfDocument::EndDoc()
{
  PutHeader();
  PutPages();
  PutResources();
  
  // Info
  NewObj();
  Out("<<");
  PutInfo();
  Out(">>");
  Out("endobj");
  
  // Catalog
  NewObj();
  Out("<<");
  PutCatalog();
  Out(">>");
  Out("endobj");
  
  // Cross-Reference
  int o = m_buffer.TellO();
  Out("xref");
  OutAscii(wxString(_T("0 ")) + wxString::Format(_T("%d"),(m_n+1)));
  Out("0000000000 65535 f ");
  int i;
  for (i = 0; i < m_n; i++)
  {
    OutAscii(wxString::Format(_T("%010d 00000 n "),m_offsets[i]));
  }
  
  // Trailer
  Out("trailer");
  Out("<<");
  PutTrailer();
  Out(">>");
  Out("startxref");
  OutAscii(wxString::Format(_T("%d"),o));
  Out("%%EOF");
  m_state = 3;
}

void
wxPdfDocument::BeginPage(int orientation)
{
  m_page++;
  (*m_pages)[m_page] = new wxMemoryOutputStream();
  m_state = 2;
  m_x = m_lMargin;
  m_y = m_tMargin;
  m_fontFamily = _T("");

  // Page orientation
  if (orientation < 0)
  {
    orientation = m_defOrientation;
  }
  else
  {
    if (orientation != m_defOrientation)
    {
      (*m_orientationChanges)[m_page] = true;
    }
  }
  if (orientation != m_curOrientation)
  {
    // Change orientation
    if (orientation == wxPORTRAIT)
    {
      m_wPt = m_fwPt;
      m_hPt = m_fhPt;
      m_w = m_fw;
      m_h = m_fh;
    }
    else
    {
      m_wPt = m_fhPt;
      m_hPt = m_fwPt;
      m_w = m_fh;
      m_h = m_fw;
    }
    m_pageBreakTrigger = m_h - m_bMargin;
    m_curOrientation = orientation;
  }
}

void
wxPdfDocument::EndPage()
{
  // End of page contents
  while (m_inTransform > 0)
  {
    StopTransform();
  }
#if 0
  if (m_angle != 0)
  {
    m_angle = 0;
    Out("Q");
  }
#endif
  m_state = 1;
}

void
wxPdfDocument::PutHeader()
{
  OutAscii(wxString(_T("%PDF-")) + m_PDFVersion);
}

void
wxPdfDocument::PutTrailer()
{
  OutAscii(wxString(_T("/Size ")) + wxString::Format(_T("%d"),(m_n+1)));
  OutAscii(wxString(_T("/Root ")) + wxString::Format(_T("%d"),m_n) + wxString(_T(" 0 R")));
  OutAscii(wxString(_T("/Info ")) + wxString::Format(_T("%d"),(m_n-1)) + wxString(_T(" 0 R")));

  if (m_encrypted)
  {
    OutAscii(wxString::Format(_T("/Encrypt %d 0 R"), m_encObjId));
    Out("/ID [()()]");
  }
}

void
wxPdfDocument::NewObj()
{
  // Begin a new object
  m_n++;
  m_offsets.Add(m_buffer.TellO());
  OutAscii(wxString::Format(_T("%d"),m_n) + wxString(_T(" 0 obj")));
}

void
wxPdfDocument::PutInfo()
{
  Out("/Producer ",false); 
  OutTextstring(wxString(wxPDF_PRODUCER));
  if (m_title.Length() > 0)
  {
    Out("/Title ",false);
    OutTextstring(m_title);
  }
  if (m_subject.Length() > 0)
  {
    Out("/Subject ",false);
    OutTextstring(m_subject);
  }
  if (m_author.Length() > 0)
  {
    Out("/Author ",false);
    OutTextstring(m_author);
  }
  if (m_keywords.Length() > 0)
  {
    Out("/Keywords ",false);
    OutTextstring(m_keywords);
  }
  if (m_creator.Length() > 0)
  {
    Out("/Creator ",false);
    OutTextstring(m_creator);
  }
  wxDateTime now = wxDateTime::Now();
  Out("/CreationDate ",false);
  OutTextstring(wxString(_T("D:")+now.Format(_T("%Y%m%d%H%M%S"))));
}

void
wxPdfDocument::PutCatalog()
{
  Out("/Type /Catalog");
  Out("/Pages 1 0 R");
  if (m_zoomMode == wxPDF_ZOOM_FULLPAGE)
  {
    Out("/OpenAction [3 0 R /Fit]");
  }
  else if (m_zoomMode == wxPDF_ZOOM_FULLWIDTH)
  {
    Out("/OpenAction [3 0 R /FitH null]");
  }
  else if (m_zoomMode == wxPDF_ZOOM_REAL)
  {
    Out("/OpenAction [3 0 R /XYZ null null 1]");
  }
  else if (m_zoomMode == wxPDF_ZOOM_FACTOR)
  {
    OutAscii(wxString(_T("/OpenAction [3 0 R /XYZ null null ")) +
             Double2String(m_zoomFactor/100.,3) + wxString(_T("]")));
  }

  if (m_layoutMode == wxPDF_LAYOUT_SINGLE)
  {
    Out("/PageLayout /SinglePage");
  }
  else if (m_layoutMode == wxPDF_LAYOUT_CONTINUOUS)
  {
    Out("/PageLayout /OneColumn");
  }
  else if (m_layoutMode == wxPDF_LAYOUT_TWO)
  {
    Out("/PageLayout /TwoColumnLeft");
  }

  if(m_outlines.GetCount() > 0)
  {
    OutAscii(wxString::Format(_T("/Outlines %d 0 R"), m_outlineRoot));
    Out("/PageMode /UseOutlines");
  }
}

// --- Fast string search (KMP method) for page number alias replacement

static int*
makeFail(const char* target, int tlen)
{
  int t = 0;
  int s, m;
  m = tlen;
  int* f = new int[m+1];
  f[1] = 0;
  for (s = 1; s < m; s++)
  {
    while ((t > 0) && (target[s] != target[t]))
    {
      t = f[t];
    }
    if (target[t] == target[s])
    {
      t++;
      f[s+1] = t;
    }
    else
    {
      f[s+1] = 0;
    }
  }
  return f;
}

static int
findString(const char* src, int slen, const char* target, int tlen, int* f)
{
  int s = 0;
  int i;
  int m = tlen;
  for (i = 0; i < slen; i++)
  {
    while ( (s > 0) && (src[i] != target[s]))
    {
      s = f[s];
    }
    if (src[i] == target[s]) s++;
    if (s == m) return (i-m+1);
  }
  return slen;
}

void
wxPdfDocument::ReplaceNbPagesAlias()
{
  int lenAsc = m_aliasNbPages.Length();
#if wxUSE_UNICODE
  wxCharBuffer wcb(m_aliasNbPages.ToAscii());
  const char* nbAsc = (const char*) wcb;
#else
  const char* nbAsc = m_aliasNbPages.c_str();
#endif
  int* fAsc = makeFail(nbAsc,lenAsc);

#if wxUSE_UNICODE
  wxMBConvUTF16BE conv;
  int lenUni = conv.WC2MB(NULL, m_aliasNbPages, 0);
  char* nbUni = new char[lenUni+3];
  lenUni = conv.WC2MB(nbUni, m_aliasNbPages, lenUni+3);
  int* fUni = makeFail(nbUni,lenUni);
#endif

  wxString pg = wxString::Format(_T("%d"),m_page);
  int lenPgAsc = pg.Length();
#if wxUSE_UNICODE
  wxCharBuffer wpg(pg.ToAscii());
  const char* pgAsc = (const char*) wpg;
  int lenPgUni = conv.WC2MB(NULL, pg, 0);
  char* pgUni = new char[lenPgUni+3];
  lenPgUni = conv.WC2MB(pgUni, pg, lenPgUni+3);
#else
  const char* pgAsc = pg.c_str();
#endif

  int n;
  for (n = 1; n <= m_page; n++)
  {
    wxMemoryOutputStream* p = new wxMemoryOutputStream();
    wxMemoryInputStream inPage(*((*m_pages)[n]));
    int len = inPage.GetSize();
    char* buffer = new char[len];
    char* pBuf = buffer;
    inPage.Read(buffer,len);
    int pAsc = findString(buffer,len,nbAsc,lenAsc,fAsc);
#if wxUSE_UNICODE
    int pUni = findString(buffer,len,nbUni,lenUni,fUni);
    while (pAsc < len || pUni < len)
    {
      if (pAsc < len && pAsc < pUni)
      {
        if (pAsc > 0)
        {
          p->Write(pBuf,pAsc);
        }
        p->Write(pgAsc,lenPgAsc);
        pBuf += pAsc + lenAsc;
        len  -= (pAsc + lenAsc);
        pUni -= (pAsc + lenAsc);
        pAsc = findString(pBuf,len,nbAsc,lenAsc,fAsc);
      }
      else if (pUni < len && pUni < pAsc)
      {
        if (pUni > 0)
        {
          p->Write(pBuf,pUni);
        }
        p->Write(pgUni,lenPgUni);
        pBuf += pUni + lenUni;
        len  -= (pUni + lenUni);
        pAsc -= (pUni + lenUni);
        pUni = findString(pBuf,len,nbUni,lenUni,fUni);
      }
    }
#else
    while (pAsc < len)
    {
      if (pAsc > 0)
      {
        p->Write(pBuf,pAsc);
      }
      p->Write(pgAsc,lenPgAsc);
      pBuf += pAsc + lenAsc;
      len  -= (pAsc + lenAsc);
      pAsc = findString(pBuf,len,nbAsc,lenAsc,fAsc);
    }
#endif
    if (len > 0)
    {
      p->Write(pBuf,len);
    }
    delete [] buffer;
    delete (*m_pages)[n];
    (*m_pages)[n] = p;
  }

#if wxUSE_UNICODE
  delete [] pgUni;
  delete [] fUni;
  delete [] nbUni;
#endif
  delete [] fAsc;
}

void
wxPdfDocument::PutPages()
{
  double wPt, hPt;
  int nb = m_page;

  if (m_aliasNbPages.Length() > 0)
  {
    // Replace number of pages
    ReplaceNbPagesAlias();
  }

  if (m_defOrientation == wxPORTRAIT)
  {
    wPt = m_fwPt;
    hPt = m_fhPt;
  }
  else
  {
    wPt = m_fhPt;
    hPt = m_fwPt;
  }
  wxString filter = (m_compress) ? _T("/Filter /FlateDecode ") : _T("");
  int n;
  for (n = 1; n <= nb; n++)
  {
    // Page
    NewObj();
    Out("<</Type /Page");
    Out("/Parent 1 0 R");

    BoolHashMap::iterator oChange = (*m_orientationChanges).find(n);
    if (oChange != (*m_orientationChanges).end())
    {
      OutAscii(wxString(_T("/MediaBox [0 0 ")) +
               Double2String(hPt,2) + wxString(_T(" ")) +
               Double2String(wPt,2) + wxString(_T("]")));
    }

    Out("/Resources 2 0 R");

    PageLinksMap::iterator pageLinks = (*m_pageLinks).find(n);
    if (pageLinks != (*m_pageLinks).end())
    {
      // Links
      wxArrayPtrVoid* pageLinkArray = pageLinks->second;
      Out("/Annots [",false);
      int pageLinkCount = pageLinkArray->GetCount();
      int j;
      for (j = 0; j < pageLinkCount; j++)
      {
        wxPdfPageLink* pl = (wxPdfPageLink*) (*pageLinkArray)[j];
        wxString rect = Double2String(pl->GetX(),2) + wxString(_T(" ")) +
                        Double2String(pl->GetY(),2) + wxString(_T(" ")) +
                        Double2String(pl->GetX()+pl->GetWidth(),2) + wxString(_T(" ")) +
                        Double2String(pl->GetY()-pl->GetHeight(),2);
        Out("<</Type /Annot /Subtype /Link /Rect [",false);
        OutAscii(rect,false);
        Out("] /Border [0 0 0] ",false);
        if (!pl->IsLinkRef())
        {
          Out("/A <</S /URI /URI ",false);
          OutTextstring(pl->GetLinkURL(),false);
          Out(">>>>",false);
        }
        else
        {
          wxPdfLink* link = (*m_links)[pl->GetLinkRef()];
          BoolHashMap::iterator oChange = (*m_orientationChanges).find(link->GetPage());
          double h = (oChange != (*m_orientationChanges).end()) ? wPt : hPt;
          OutAscii(wxString::Format(_T("/Dest [%d 0 R /XYZ 0 "),1+2*link->GetPage()) +
                   Double2String(h-link->GetPosition()*m_k,2) + 
                   wxString(_T(" null]>>")),false);
        }
        delete pl;
        (*pageLinkArray)[j] = NULL;
      }
      Out("]");
    }

    OutAscii(wxString::Format(_T("/Contents %d 0 R>>"), m_n+1));
    Out("endobj");
    
    // Page content
    wxMemoryOutputStream* p;
    if (m_compress)
    {
      p = new wxMemoryOutputStream();
      wxZlibOutputStream q(*p);
      wxMemoryInputStream tmp(*((*m_pages)[n]));
      q.Write(tmp);
    }
    else
    {
      p = (*m_pages)[n];
    }

    NewObj();
    OutAscii(wxString(_T("<<")) + filter + wxString(_T("/Length ")) + 
             wxString::Format(_T("%d"), p->TellO()) + wxString(_T(">>")));
    PutStream(*p);
    Out("endobj");
    if (m_compress)
    {
      delete p;
    }
  }
  // Pages root
  m_offsets[0] = m_buffer.TellO();
  Out("1 0 obj");
  Out("<</Type /Pages");
  wxString kids = _T("/Kids [");
  int i;
  for (i = 0; i < nb; i++)
  {
    kids += wxString::Format(_T("%d"),(3+2*i)) + wxString(_T(" 0 R "));
  }
  OutAscii(kids + wxString(_T("]")));
  OutAscii(wxString(_T("/Count ")) + wxString::Format(_T("%d"),nb));
  OutAscii(wxString(_T("/MediaBox [0 0 ")) +
           Double2String(wPt,2) + wxString(_T(" ")) +
           Double2String(hPt,2) + wxString(_T("]")));
  Out(">>");
  Out("endobj");
}

void
wxPdfDocument::PutShaders()
{
  GradientMap::iterator gradient;
  for (gradient = m_gradients->begin(); gradient != m_gradients->end(); gradient++)
  {
    int type = gradient->second->GetType();
		//foreach($this->gradients as $id=>$grad){
		NewObj();
		Out("<<");
		Out("/FunctionType 2");
		Out("/Domain [0.0 1.0]");
		Out("/C0 [", false);
    OutAscii(RGB2String(gradient->second->GetColor1()), false);
    Out("]");
		Out("/C1 [", false);
    OutAscii(RGB2String(gradient->second->GetColor2()), false);
    Out("]");
		Out("/N 1");
		Out(">>");
		Out("endobj");
		int f1 = m_n;

		NewObj();
		Out("<<");
    OutAscii(wxString::Format(_T("/ShadingType %d"), type));
		Out("/ColorSpace /DeviceRGB");
    const double* coords = gradient->second->GetCoords();
		if (type == 2)
    {
			OutAscii(wxString(_T("/Coords [")) +
               Double2String(coords[0],3) + wxString(_T(" ")) +
               Double2String(coords[1],3) + wxString(_T(" ")) +
               Double2String(coords[2],3) + wxString(_T(" ")) +
               Double2String(coords[3],3) + wxString(_T("]")));
      OutAscii(wxString::Format(_T("/Function %d 0 R"), f1));
			Out("/Extend [true true] ");
		}
		else if (type == 3)
    {
			// x0, y0, r0, x1, y1, r1
			// at this time radius of inner circle is 0
			OutAscii(wxString(_T("/Coords [")) +
               Double2String(coords[0],3) + wxString(_T(" ")) +
               Double2String(coords[1],3) + wxString(_T(" 0 ")) +
               Double2String(coords[2],3) + wxString(_T(" ")) +
               Double2String(coords[3],3) + wxString(_T(" ")) +
               Double2String(coords[4],3) + wxString(_T("]")));
			OutAscii(wxString::Format(_T("/Function %d 0 R"), f1));
			Out("/Extend [true true] ");
		}
		Out(">>");
		Out("endobj");
		gradient->second->SetObjIndex(m_n);
	}
}

void
wxPdfDocument::PutFonts()
{
  int nf = m_n;

  int nb = (*m_diffs).size();
  int i;
  for (i = 1; i <= nb; i++)
  {
    // Encodings
    NewObj();
    Out("<</Type /Encoding /BaseEncoding /WinAnsiEncoding /Differences [", false);
    OutAscii(*(*m_diffs)[i], false);
    Out("]>>");
    Out("endobj");
  }

  FontHashMap::iterator fontIter = m_fonts->begin();
  for (fontIter = m_fonts->begin(); fontIter != m_fonts->end(); fontIter++)
  {
    wxPdfFont* font = fontIter->second;
    if (font->HasFile())
    {
      // Font file embedding
      NewObj();
      font->SetFileIndex(m_n);

      wxString strFontFileName = font->GetFontFile();
      wxFileName fontFileName(strFontFileName);
      fontFileName.MakeAbsolute(GetFontPath());
       wxFileSystem fs;
      wxFSFile* fontFile = fs.OpenFile(fontFileName.GetFullPath());
      if (fontFile)
      {
        wxInputStream* fontStream = fontFile->GetStream();
        int fontLen = fontStream->GetSize();
        wxMemoryOutputStream* p = new wxMemoryOutputStream();
        
        bool compressed = strFontFileName.Right(2) == _T(".z");
        if (!compressed && font->HasSize2())
        {
          unsigned char first = (unsigned char) fontStream->Peek();
          if (first == 128)
          {
            unsigned char* buffer = new unsigned char[fontLen];
            fontStream->Read(buffer, fontLen);
            if (buffer[6+font->GetSize1()] == 128)
            {
              // Strip first and second binary header
              fontLen -= 12;
              p->Write(&buffer[6], font->GetSize1());
              p->Write(&buffer[12 + font->GetSize1()], fontLen - font->GetSize1());
            }
            else
            {
              // Strip first binary header
              fontLen -= 6;
              p->Write(&buffer[6], fontLen);
            }
            delete [] buffer;
          }
          else
          {
            p->Write(*fontStream);
          }
        }
        else
        {
          p->Write(*fontStream);
        }
        OutAscii(wxString::Format(_T("<</Length %d"), fontLen));
        if (compressed)
        {
          Out("/Filter /FlateDecode");
        }
        OutAscii(wxString::Format(_T("/Length1 %d"), font->GetSize1()));
        if (font->HasSize2())
        {
          OutAscii(wxString::Format(_T("/Length2 %d /Length3 0"), font->GetSize2()));
        }

        Out(">>");
        PutStream(*p);
        Out("endobj");

        delete p;
        delete fontFile;
      }
    }
  }
  
  fontIter = m_fonts->begin();
  for (fontIter = m_fonts->begin(); fontIter != m_fonts->end(); fontIter++)
  {
    // Font objects
    wxPdfFont* font = fontIter->second;
    font->SetObjIndex(m_n+1);
    wxString type = font->GetType();
    wxString name = font->GetName();
    if (type == _T("core"))
    {
      // Standard font
      NewObj();
      Out("<</Type /Font");
      OutAscii(wxString(_T("/BaseFont /"))+name);
      Out("/Subtype /Type1");
      if (name != _T("Symbol") && name != _T("ZapfDingbats"))
      {
        Out("/Encoding /WinAnsiEncoding");
      }
      Out(">>");
      Out("endobj");
    }
    else if (type == _T("Type1") || type == _T("TrueType"))
    {
      // Additional Type1 or TrueType font
      NewObj();
      Out("<</Type /Font");
      OutAscii(wxString(_T("/BaseFont /")) + name);
      OutAscii(wxString(_T("/Subtype /")) + type);
      Out("/FirstChar 32 /LastChar 255");
      OutAscii(wxString::Format(_T("/Widths %d  0 R"), m_n+1));
      OutAscii(wxString::Format(_T("/FontDescriptor %d 0 R"), m_n+2));
      if (font->GetEncoding() != _T(""))
      {
        if (font->HasDiffs())
        {
          OutAscii(wxString::Format(_T("/Encoding %d 0 R"), (nf+font->GetDiffIndex())));
        }
        else
        {
          Out("/Encoding /WinAnsiEncoding");
        }
      }
      Out(">>");
      Out("endobj");

      // Widths
      NewObj();
      wxString s = font->GetWidthsAsString();
      OutAscii(s);
      Out("endobj");

      // Descriptor
      const wxPdfFontDescription& fd = font->GetDesc();
      NewObj();
      Out("<</Type /FontDescriptor");
      OutAscii(wxString(_T("/FontName /")) + name);
      OutAscii(wxString::Format(_T("/Ascent %d"), fd.GetAscent()));
      OutAscii(wxString::Format(_T("/Descent %d"), fd.GetDescent()));
      OutAscii(wxString::Format(_T("/CapHeight %d"), fd.GetCapHeight()));
      OutAscii(wxString::Format(_T("/Flags %d"), fd.GetFlags()));
      OutAscii(wxString(_T("/FontBBox")) + fd.GetFontBBox());
      OutAscii(wxString::Format(_T("/ItalicAngle %d"), fd.GetItalicAngle()));
      OutAscii(wxString::Format(_T("/StemV %d"), fd.GetStemV()));
      OutAscii(wxString::Format(_T("/MissingWidth %d"), fd.GetMissingWidth()));
      if (font->HasFile())
      {
        if (type == _T("Type1"))
        {
          OutAscii(wxString::Format(_T("/FontFile %d 0 R"), font->GetFileIndex()));
        }
        else
        {
          OutAscii(wxString::Format(_T("/FontFile2 %d 0 R"), font->GetFileIndex()));
        }
      }
      Out(">>");
      Out("endobj");
    }
    else if (type == _T("TrueTypeUnicode"))
    {
      // Type0 Font
      // A composite font composed of other fonts, organized hierarchically
      NewObj();
      Out("<</Type /Font");
      Out("/Subtype /Type0");
      OutAscii(wxString(_T("/BaseFont /")) + name);
      // The horizontal identity mapping for 2-byte CIDs; may be used with
      // CIDFonts using any Registry, Ordering, and Supplement values.
      Out("/Encoding /Identity-H");
      OutAscii(wxString::Format(_T("/DescendantFonts [%d 0 R]"), (m_n + 1)));
      Out(">>");
      Out("endobj");
      
      // CIDFontType2
      // A CIDFont whose glyph descriptions are based on TrueType font technology
      NewObj();
      Out("<</Type /Font");
      Out("/Subtype /CIDFontType2");
      OutAscii(wxString(_T("/BaseFont /")) + name);
      OutAscii(wxString::Format(_T("/CIDSystemInfo %d 0 R"), (m_n + 1))); 
      OutAscii(wxString::Format(_T("/FontDescriptor %d 0 R"), (m_n + 2)));

      const wxPdfFontDescription& fd = font->GetDesc();
      if (fd.GetMissingWidth() > 0)
      {
        // The default width for glyphs in the CIDFont MissingWidth
        OutAscii(wxString::Format(_T("/DW %d"), fd.GetMissingWidth()));
      }
      
      OutAscii(wxString(_T("/W ")) + font->GetWidthsAsString()); // A description of the widths for the glyphs in the CIDFont
      OutAscii(wxString::Format(_T("/CIDToGIDMap %d 0 R"), (m_n + 3)));
      Out(">>");
      Out("endobj");
      
      // CIDSystemInfo dictionary
      // TODO: Probably needs string encryption if encryption is used
      // A dictionary containing entries that define the character collectionof the CIDFont.
      NewObj();
      // A string identifying an issuer of character collections
      Out("<</Registry (Adobe)");
      // A string that uniquely names a character collection issued by a specific registry
      Out("/Ordering (UCS)");
      // The supplement number of the character collection.
      Out("/Supplement 0");
      Out(">>");
      Out("endobj");
      
      // Font descriptor
      // A font descriptor describing the CIDFonts default metrics other than its glyph widths
      NewObj();
      Out("<</Type /FontDescriptor");
      OutAscii(wxString(_T("/FontName /")) + name);
      wxString s = wxEmptyString;
      OutAscii(wxString::Format(_T("/Ascent %d"), fd.GetAscent()));
      OutAscii(wxString::Format(_T("/Descent %d"), fd.GetDescent()));
      OutAscii(wxString::Format(_T("/CapHeight %d"), fd.GetCapHeight()));
      OutAscii(wxString::Format(_T("/Flags %d"), fd.GetFlags()));
      OutAscii(wxString(_T("/FontBBox")) + fd.GetFontBBox());
      OutAscii(wxString::Format(_T("/ItalicAngle %d"), fd.GetItalicAngle()));
      OutAscii(wxString::Format(_T("/StemV %d"), fd.GetStemV()));
      OutAscii(wxString::Format(_T("/MissingWidth %d"), fd.GetMissingWidth()));

      if (font->HasFile())
      {
        // A stream containing a TrueType font program
        OutAscii(wxString::Format(_T("/FontFile2 %d 0 R"), font->GetFileIndex()));
      }
      Out(">>");
      Out("endobj");

      // Embed CIDToGIDMap
      // A specification of the mapping from CIDs to glyph indices
      NewObj();
      wxString strCtgFileName = font->GetCtgFile();
      wxFileName ctgFileName(strCtgFileName);
      ctgFileName.MakeAbsolute(GetFontPath());
       wxFileSystem fs;
      wxFSFile* ctgFile = fs.OpenFile(ctgFileName.GetFullPath());
      if (ctgFile)
      {
        wxMemoryOutputStream* p = new wxMemoryOutputStream();
        wxInputStream* ctgStream = ctgFile->GetStream();
        int ctgLen = ctgStream->GetSize();
        OutAscii(wxString::Format(_T("<</Length %d"), ctgLen));
        // check file extension
        bool compressed = strCtgFileName.Right(2) == _T(".z");
        if (compressed)
        {
          // Decompresses data encoded using the public-domain zlib/deflate compression
          // method, reproducing the original text or binary data
          Out("/Filter /FlateDecode");
        }
        Out(">>");
        p->Write(*ctgStream);
        PutStream(*p);
        delete p;
        delete ctgFile;
      }
      else
      {
        // TODO : file not found, should be checked already when adding font!
        wxLogDebug(_T("wxPdfDocument::PutFonts: Font file '%s' not found."), strCtgFileName.c_str());
      }
      Out("endobj");
    }
    else if (type == _T("Type0"))
    {
      // Type0
      NewObj();
      Out("<</Type /Font");
      Out("/Subtype /Type0");
      OutAscii(wxString(_T("/BaseFont /")) + name + wxString(_T("-")) + font->GetCMap());
      OutAscii(wxString(_T("/Encoding /")) + font->GetCMap());
      OutAscii(wxString::Format(_T("/DescendantFonts [%d 0 R]"), (m_n+1)));
      Out(">>");
      Out("endobj");

      // CIDFont
      NewObj();
      Out("<</Type /Font");
      Out("/Subtype /CIDFontType0");
      OutAscii(wxString(_T("/BaseFont /")) + name);
      OutAscii(wxString(_T("/CIDSystemInfo <</Registry (Adobe) /Ordering (")) +
               font->GetOrdering() + wxString(_T(") /Supplement ")) + 
               font->GetSupplement() + wxString(_T(">>")));
      OutAscii(wxString::Format(_T("/FontDescriptor %d 0 R"), (m_n+1)));

      // Widths
      // A description of the widths for the glyphs in the CIDFont
      OutAscii(wxString(_T("/W ")) + font->GetWidthsAsString());
      Out(">>");
      Out("endobj");

      // Font descriptor
      const wxPdfFontDescription& fd = font->GetDesc();
      NewObj();
      Out("<</Type /FontDescriptor");
      OutAscii(wxString(_T("/FontName /")) + name);
      OutAscii(wxString::Format(_T("/Ascent %d"), fd.GetAscent()));
      OutAscii(wxString::Format(_T("/Descent %d"), fd.GetDescent()));
      OutAscii(wxString::Format(_T("/CapHeight %d"), fd.GetCapHeight()));
      OutAscii(wxString::Format(_T("/Flags %d"), fd.GetFlags()));
      OutAscii(wxString(_T("/FontBBox")) + fd.GetFontBBox());
      OutAscii(wxString::Format(_T("/ItalicAngle %d"), fd.GetItalicAngle()));
      OutAscii(wxString::Format(_T("/StemV %d"), fd.GetStemV()));
      Out(">>");
      Out("endobj");
    }
  }
}

void
wxPdfDocument::PutImages()
{
  wxString filter = (m_compress) ? _T("/Filter /FlateDecode ") : _T("");
  ImageHashMap::iterator image = m_images->begin();
  for (image = m_images->begin(); image != m_images->end(); image++)
  {
    // Image objects
    wxPdfImage* currentImage = image->second;
    NewObj();
    currentImage->SetObjIndex(m_n);
    Out("<</Type /XObject");
    if (currentImage->IsFormObject())
    {
      Out("/Subtype /Form");
      OutAscii(wxString::Format(_T("/BBox [%d %d %d %d]"),
                 currentImage->GetX(), currentImage->GetY(),
                 currentImage->GetWidth()+currentImage->GetX(),
                 currentImage->GetHeight() + currentImage->GetY()));
      if (m_compress)
      {
        Out("/Filter /FlateDecode");
      }
      int dataLen = currentImage->GetDataSize();
      wxMemoryOutputStream* p = new wxMemoryOutputStream();
      if (m_compress)
      {
        wxZlibOutputStream q(*p);
        q.Write(currentImage->GetData(),currentImage->GetDataSize());
      }
      else
      {
        p->Write(currentImage->GetData(),currentImage->GetDataSize());
      }
      dataLen = p->TellO();
      OutAscii(wxString::Format(_T("/Length %d>>"),dataLen));
      PutStream(*p);

      // TODO: unset($this->formobjects[$file]['data']);
      Out("endobj");
      delete p;
    }
    else
    {
      Out("/Subtype /Image");
      OutAscii(wxString::Format(_T("/Width %d"),currentImage->GetWidth()));
      OutAscii(wxString::Format(_T("/Height %d"),currentImage->GetHeight()));

      if (currentImage->GetColorSpace() == _T("Indexed"))
      {
        int palLen = currentImage->GetPaletteSize() / 3 - 1;
        OutAscii(wxString::Format(_T("/ColorSpace [/Indexed /DeviceRGB %d %d 0 R]"),
                 palLen,(m_n+1)));
      }
      else
      {
        OutAscii(wxString(_T("/ColorSpace /")) + currentImage->GetColorSpace());
        if (currentImage->GetColorSpace() == _T("DeviceCMYK"))
        {
          Out("/Decode [1 0 1 0 1 0 1 0]");
        }
      }
      OutAscii(wxString::Format(_T("/BitsPerComponent %d"),currentImage->GetBitsPerComponent()));
      wxString f = currentImage->GetF();
      if (f.Length() > 0)
      {
        OutAscii(wxString(_T("/Filter /")) + f);
      }
      wxString parms = currentImage->GetParms();
      if (parms.Length() > 0)
      {
        OutAscii(parms);
      }
      int trnsSize = currentImage->GetTransparencySize();
      unsigned char* trnsData = (unsigned char*) currentImage->GetTransparency();
      if (trnsSize > 0)
      {
        wxString trns = _T("");;
        int i;
        for (i = 0; i < trnsSize; i++)
        {
          int trnsValue = trnsData[i];
          trns += wxString::Format(_T("%d %d "), trnsValue, trnsValue);
        }
        OutAscii(wxString(_T("/Mask [")) + trns + wxString(_T("]")));
      }

      OutAscii(wxString::Format(_T("/Length %d>>"),currentImage->GetDataSize()));

      wxMemoryOutputStream* p = new wxMemoryOutputStream();
      p->Write(currentImage->GetData(),currentImage->GetDataSize());
      PutStream(*p);
      delete p;
      // TODO: unset($this->images[$file]['data']);
      Out("endobj");

      // Palette
      if (currentImage->GetColorSpace() == _T("Indexed"))
      {
        NewObj();
        int palLen = currentImage->GetPaletteSize();
        p = new wxMemoryOutputStream();
        if (m_compress)
        {
          wxZlibOutputStream q(*p);
          q.Write(currentImage->GetPalette(),currentImage->GetPaletteSize());
        }
        else
        {
          p->Write(currentImage->GetPalette(),currentImage->GetPaletteSize());
        }
        palLen = p->TellO();
        OutAscii(wxString(_T("<<")) + filter + wxString::Format(_T("/Length %d>>"),palLen));
        PutStream(*p);
        Out("endobj");
        delete p;
      }
    }
  }
}

void
wxPdfDocument::PutXObjectDict()
{
  ImageHashMap::iterator image = m_images->begin();
  for (image = m_images->begin(); image != m_images->end(); image++)
  {
    wxPdfImage* currentImage = image->second;
    OutAscii(wxString::Format(_T("/I%d %d 0 R"), currentImage->GetIndex(), currentImage->GetObjIndex()));
  }
}

void
wxPdfDocument::PutResourceDict()
{
  Out("/ProcSet [/PDF /Text /ImageB /ImageC /ImageI]");
  Out("/Font <<");
  FontHashMap::iterator font = m_fonts->begin();

  for (font = m_fonts->begin(); font != m_fonts->end(); font++)
  {
    OutAscii(wxString::Format(_T("/F%d %d 0 R"), font->second->GetIndex(), font->second->GetObjIndex()));
  }
  Out(">>");
  Out("/XObject <<");
  PutXObjectDict();
  Out(">>");
	Out("/Shading <<");
  GradientMap::iterator gradient;
  for (gradient = m_gradients->begin(); gradient != m_gradients->end(); gradient++)
  {
	  //foreach(m_gradients as $id=>$grad)
    OutAscii(wxString::Format(_T("/Sh%d %d 0 R"), gradient->first, gradient->second->GetObjIndex()));
  }
	Out(">>");
}

void
wxPdfDocument::PutBookmarks()
{
  int nb = m_outlines.GetCount();
  if (nb == 0)
  {
    return;
  }

  int i;
  int parent;
  wxArrayInt lru;
  lru.SetCount(m_maxOutlineLevel+1);
  int level = 0;
  for (i = 0; i < nb; i++)
  {
    wxPdfBookmark* bookmark = (wxPdfBookmark*) m_outlines[i];
    int currentLevel = bookmark->GetLevel();
    if (currentLevel > 0)
    {
      parent = lru[currentLevel-1];
      // Set parent and last pointers
      bookmark->SetParent(parent);
      wxPdfBookmark* parentBookmark = (wxPdfBookmark*) m_outlines[parent];
      parentBookmark->SetLast(i);
      if (currentLevel > level)
      {
        // Level increasing: set first pointer
        parentBookmark->SetFirst(i);
      }
    }
    else
    {
      bookmark->SetParent(nb);
    }
    if (currentLevel <= level && i > 0)
    {
      // Set prev and next pointers
      int prev = lru[currentLevel];
      wxPdfBookmark* prevBookmark = (wxPdfBookmark*) m_outlines[prev];
      prevBookmark->SetNext(i);
      bookmark->SetPrev(prev);
    }
    lru[currentLevel] = i;
    level = currentLevel;
  }

  // Outline items
  int n = m_n + 1;
  for (i = 0; i < nb; i++)
  {
    wxPdfBookmark* bookmark = (wxPdfBookmark*) m_outlines[i];
    NewObj();
    Out("<</Title ", false);
    OutTextstring(bookmark->GetText());
    OutAscii(wxString::Format(_T("/Parent %d 0 R"), (n+bookmark->GetParent())));
    if (bookmark->GetPrev() >= 0)
    {
      OutAscii(wxString::Format(_T("/Prev %d 0 R"), (n+bookmark->GetPrev())));
    }
    if (bookmark->GetNext() >= 0)
    {
      OutAscii(wxString::Format(_T("/Next %d 0 R"), (n+bookmark->GetNext())));
    }
    if (bookmark->GetFirst() >= 0)
    {
      OutAscii(wxString::Format(_T("/First %d 0 R"), (n+bookmark->GetFirst())));
    }
    if(bookmark->GetLast() >= 0)
    {
      OutAscii(wxString::Format(_T("/Last %d 0 R"), (n+bookmark->GetLast())));
    }
    OutAscii(wxString::Format(_T("/Dest [%d 0 R /XYZ 0 "), (1+2*bookmark->GetPage())) +
             Double2String((m_h-bookmark->GetY())*m_k,2) + wxString(_T(" null]")));
    Out("/Count 0>>");
    Out("endobj");
  }
  // Outline root
  NewObj();
  m_outlineRoot = m_n;
  OutAscii(wxString::Format(_T("<</Type /Outlines /First %d 0 R"), n));
  OutAscii(wxString::Format(_T("/Last %d 0 R>>"), (n+lru[0])));
  Out("endobj");
}

void
wxPdfDocument::PutEncryption()
{
  Out("/Filter /Standard");
  Out("/V 1");
  Out("/R 2");
  Out("/O (",false);
  OutEscape((char*) m_encryptor->GetOvalue(),32);
  Out(")");
  Out("/U (",false);
  OutEscape((char*) m_encryptor->GetUvalue(),32);
  Out(")");
  OutAscii(wxString::Format(_T("/P %d"),m_encryptor->GetPvalue()));
}


void
wxPdfDocument::PutResources()
{
	PutShaders();
  PutFonts();
  PutImages();
  
  // Resource dictionary
  m_offsets[2-1] = m_buffer.TellO();
  Out("2 0 obj");
  Out("<<");
  PutResourceDict();
  Out(">>");
  Out("endobj");

  PutBookmarks();

  if (m_encrypted)
  {
    NewObj();
    m_encObjId = m_n;
    Out("<<");
    PutEncryption();
    Out(">>");
    Out("endobj");
  }

}

wxString
wxPdfDocument::DoUnderline(double x, double y, const wxString& txt)
{
  // Underline text
  int up = m_currentFont->GetUnderlinePosition();
  int ut = m_currentFont->GetUnderlineThickness();
  double w = GetStringWidth(txt) + m_ws * txt.Freq(_T(' '));
  wxString underline =
    Double2String(x * m_k,2) + wxString(_T(" ")) +
    Double2String((m_h - (y - up/1000*m_fontSize)) * m_k,2) + wxString(_T(" ")) +
    Double2String(w * m_k,2) + wxString(_T(" ")) +
    Double2String(-ut/1000*m_fontSizePt,2) + wxString(_T(" re f"));
  return underline;
}

void
wxPdfDocument::TextEscape(const wxString& s, bool newline)
{
#if wxUSE_UNICODE
  wxMBConv* conv = m_currentFont->GetEncodingConv();
  int len = conv->WC2MB(NULL, s, 0);
  char* mbstr = new char[len+3];
  len = conv->WC2MB(mbstr, s, len+3);
#else
  int len = s.Length();;
  char* mbstr = new char[len+1];
  strcpy(mbstr,s.c_str());
#endif

  OutEscape(mbstr,len);
  if (newline)
  {
    Out("\n",false);
  }
  delete [] mbstr;
}

void
wxPdfDocument::PutStream(wxMemoryOutputStream& s)
{
  Out("stream");
  if (m_encrypted)
  {
    wxMemoryInputStream instream(s);
    int len = instream.GetSize();
    char* buffer = new char[len];
    instream.Read(buffer,len);
    m_encryptor->Encrypt(m_n, (unsigned char*) buffer, len);
    Out(buffer, len);
    delete [] buffer;
  }
  else
  {
    wxMemoryInputStream tmp(s);
    if(m_state==2)
    {
      (*m_pages)[m_page]->Write(tmp);
       (*m_pages)[m_page]->Write("\n",1);
    }
    else
    {
      m_buffer.Write(tmp);
      m_buffer.Write("\n",1);
    }
  }
  Out("endstream");
}

void
wxPdfDocument::OutEscape(const char* s, int len)
{
  int j;
  for (j = 0; j < len; j++)
  {
    switch (s[j])
    {
      case '\b':
        Out("\\b",false);
        break;
      case '\f':
        Out("\\f",false);
        break;
      case '\n':
        Out("\\n",false);
        break;
      case '\r':
        Out("\\r",false);
        break;
      case '\t':
        Out("\\t",false);
        break;
      case '\\':
      case '(':
      case ')':
        Out("\\",false);
      default:
        Out(&s[j],1,false);
        break;
    }
  }
}

void
wxPdfDocument::OutTextstring(const wxString& s, bool newline)
{
  // Format a text string
#if wxUSE_UNICODE
  wxMBConvUTF16BE conv;
  int len = conv.WC2MB(NULL, s, 0);
  char* mbstr = new char[len+5];
  mbstr[0] = '\xfe';
  mbstr[1] = '\xff';
  len = 2 + conv.WC2MB(mbstr+2, s, len+3);
#else
  int len = s.Length();;
  char* mbstr = new char[len+1];
  strcpy(mbstr,s.c_str());
#endif

  if (m_encrypted)
  {
    m_encryptor->Encrypt(m_n, (unsigned char*) mbstr, len);
  }
  Out("(",false);
  OutEscape(mbstr,len);
  Out(")",newline);
  delete [] mbstr;
}

void
wxPdfDocument::OutAscii(const wxString& s, bool newline)
{
  // Add a line of ASCII text to the document
  Out((const char*) s.ToAscii(),newline);
}

void
wxPdfDocument::Out(const char* s, bool newline)
{
  int len = strlen(s);
  Out(s,len,newline);
}

void
wxPdfDocument::Out(const char* s, int len, bool newline)
{
  if(m_state==2)
  {
    (*m_pages)[m_page]->Write(s,len);
    if (newline)
    {
      (*m_pages)[m_page]->Write("\n",1);
    }
  }
  else
  {
    m_buffer.Write(s,len);
    if (newline)
    {
      m_buffer.Write("\n",1);
    }
  }
}

void
wxPdfDocument::OutPoint(double x, double y)
{
  OutAscii(Double2String(x * m_k,2) + wxString(_T(" ")) +
           Double2String((m_h - y) * m_k,2) + wxString(_T(" m")));
  m_x = x;
  m_y = y;
}

void
wxPdfDocument::OutPointRelative(double dx, double dy)
{
  m_x += dx;
  m_y += dy;
  OutAscii(Double2String(m_x * m_k,2) + wxString(_T(" ")) +
           Double2String((m_h - m_y) * m_k,2) + wxString(_T(" m")));
}

void
wxPdfDocument::OutLine(double x, double y)
{
  // Draws a line from last draw point
  OutAscii(Double2String(x * m_k,2) + wxString(_T(" ")) +
           Double2String((m_h - y) * m_k,2) + wxString(_T(" l")));
  m_x = x;
  m_y = y;
}

void
wxPdfDocument::OutLineRelative(double dx, double dy)
{
  m_x += dx;
  m_y += dy;
  // Draws a line from last draw point
  OutAscii(Double2String(m_x * m_k,2) + wxString(_T(" ")) +
           Double2String((m_h - m_y) * m_k,2) + wxString(_T(" l")));
}

void
wxPdfDocument::OutCurve(double x1, double y1, double x2, double y2, double x3, double y3)
{
  // Draws a Bézier curve from last draw point
  OutAscii(Double2String(x1 * m_k,2) + wxString(_T(" ")) +
           Double2String((m_h - y1) * m_k,2) + wxString(_T(" ")) +
           Double2String(x2 * m_k,2) + wxString(_T(" ")) +
           Double2String((m_h - y2) * m_k,2) + wxString(_T(" ")) +
           Double2String(x3 * m_k,2) + wxString(_T(" ")) +
           Double2String((m_h - y3) * m_k,2) + wxString(_T(" c")));
  m_x = x3;
  m_y = y3;
}

void
wxPdfDocument::InitializeCoreFonts()
{
  m_coreFonts = new CoreFontMap();
  int j;
  for (j = 0; wxCoreFontTable[j].id != wxEmptyString; j++)
  {
    (*m_coreFonts)[wxCoreFontTable[j].id] = j;
  }
}

wxString
wxPdfDocument::RGB2String(const wxColour& color)
{
  double r = color.Red();
  double g = color.Green();
  double b = color.Blue();
  wxString rgb = Double2String(r/255.,3) + _T(" ") + 
                 Double2String(g/255.,3) + _T(" ") + 
                 Double2String(b/255.,3);
  return rgb;
}

wxString
wxPdfDocument::Double2String(double value, int precision)
{
  wxString number;
  if (precision < 0)
  {
    precision = 0;
  }
  else if (precision > 16)
  {
    precision = 16;
  }

  // Use absolute value locally
  double localValue = fabs(value);
  double localFraction = (localValue - floor(localValue)) +(5. * pow(10.0, -precision-1));
  if (localFraction >= 1)
  {
    localValue += 1.0;
    localFraction -= 1.0;
  }
  localFraction *= pow(10.0, precision);

  if (value < 0)
  {
    number += wxString(_T("-"));
  }

  number += wxString::Format(_T("%.0lf"), floor(localValue));

  // generate fraction, padding with zero if necessary.
  if (precision > 0)
  {
    number += wxString(_T("."));
    wxString fraction = wxString::Format(_T("%.0lf"), floor(localFraction));
    if (fraction.Length() < precision)
    {
      number += wxString(_T('0'), precision-fraction.Length());
    }
    number += fraction;
  }

  return number;
}

bool wxPdfDocument::ms_seeded = false;
int  wxPdfDocument::ms_s1     = 0;
int  wxPdfDocument::ms_s2     = 0;

#define MODMULT(a, b, c, m, s) q = s / a; s = b * (s - a * q) - c * q; if (s < 0) s += m

wxString
wxPdfDocument::GetUniqueId(const wxString& prefix)
{
  wxString uid = (prefix.Length() <= 114) ? prefix : prefix.Left(114);

  wxDateTime ts;
  ts.SetToCurrent();

  int q;
  int z;
  if (!ms_seeded)
  {
    ms_seeded = true;
    ms_s1 = ts.GetSecond() ^ (~ts.GetMillisecond());
    if (ms_s1 == 0) ms_s1 = 1;
    ms_s2 = wxGetProcessId();
  }
  MODMULT(53668, 40014, 12211, 2147483563L, ms_s1);
  MODMULT(52774, 40692,  3791, 2147483399L, ms_s2);

  z = ms_s1 - ms_s2;
  if (z < 1)
  {
    z += 2147483562;
  }

  uid += wxString::Format(_T("%08x%05x"), ts.GetSecond(), ts.GetMillisecond());
  uid += Double2String(z * 4.656613e-9,8);

  return uid;
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
  if (linestyle.GetColour().Ok())
  {
    SetDrawColor(linestyle.GetColour());
  }
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
		// TODO	$this->Error('Please use values unequal to zero for Scaling');
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
	//calculate elements of transformation matrix
  double tm[6];
	tm[0] = 1;
	tm[1] = 0;
	tm[2] = 0;
	tm[3] = 1;
	tm[4] = tx;
	tm[5] = -ty;
	//translate the coordinate system
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
	//calculate elements of transformation matrix
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
wxPdfDocument::Transform(double tm[6])
{
//	_out(sprintf('%.3f %.3f %.3f %.3f %.3f %.3f cm', $tm[0],$tm[1],$tm[2],$tm[3],$tm[4],$tm[5]));
  OutAscii(Double2String( tm[0],3) + wxString(_T(" ")) +
           Double2String( tm[1],3) + wxString(_T(" ")) +
           Double2String( tm[2],3) + wxString(_T(" ")) +
           Double2String( tm[3],3) + wxString(_T(" ")) +
           Double2String( tm[4],3) + wxString(_T(" ")) +
           Double2String( tm[5],3) + wxString(_T(" cm")));
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

void
wxPdfDocument::Gradient(int type, const wxColour& col1, const wxColour& col2, double coords[])
{
	int n = (*m_gradients).size()+1;
  (*m_gradients)[n] = new wxPdfGradient(type, col1, col2, coords);
	//paint the gradient
  OutAscii(wxString::Format(_T("/Sh%d sh"), n));
	//restore previous Graphic State
	Out("Q");
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
