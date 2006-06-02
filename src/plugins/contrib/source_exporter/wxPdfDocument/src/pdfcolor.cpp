///////////////////////////////////////////////////////////////////////////////
// Name:        pdfcolor.cpp
// Purpose:     Implementation of wxPdfDocument color handling
// Author:      Ulrich Telle
// Modified by:
// Created:     2006-01-27
// Copyright:   (c) Ulrich Telle
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/// \file pdfcolor.cpp Implementation of the wxPdfDocument color handling

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/pdfdoc.h"

#include "pdfcolordata.inc"

/// Class representing the internal representation of colors. (For internal use only)
class wxPdfColourInternal : public wxPdfColour
{
public:
  /// Constructor
  wxPdfColourInternal(const wxString& color) : wxPdfColour(color, true) {}
};

wxColourDatabase* wxPdfColour::ms_colorDatabase = NULL;

wxColourDatabase*
wxPdfColour::GetColorDatabase()
{
  if (ms_colorDatabase == NULL)
  {
    if (wxTheColourDatabase != NULL)
    {
      ms_colorDatabase = wxTheColourDatabase;
    }
    else
    {
      static wxColourDatabase pdfColorDatabase;
      ms_colorDatabase = &pdfColorDatabase;
    }
    size_t n;
    for ( n = 0; n < WXSIZEOF(wxColourTable); n++ )
    {
      const wxColourDesc& cc = wxColourTable[n];
      ms_colorDatabase->AddColour(cc.name, wxColour(cc.r, cc.g, cc.b));
    }
  }
  return ms_colorDatabase;
}

wxPdfColour::wxPdfColour(const unsigned char grayscale)
{
  SetColor(grayscale);
}

wxPdfColour::wxPdfColour(const wxColour& color)
{
  SetColor(color);
}

wxPdfColour::wxPdfColour(const unsigned char red, const unsigned char green, const unsigned char blue)
{
  SetColor(red, green, blue);
}

wxPdfColour::wxPdfColour(double cyan, double magenta, double yellow, double black)
{
  SetColor(cyan, magenta, yellow, black);
}

wxPdfColour::wxPdfColour(const wxPdfColour& color)
{
  m_color = color.m_color;
}

wxPdfColour::wxPdfColour(const wxString& name)
{
  SetColor(name);
}

wxPdfColour::wxPdfColour(const wxString& color, bool WXUNUSED(internal))
{
  m_color = color;
}

void
wxPdfColour::SetColor(const unsigned char grayscale)
{
  m_color = wxPdfDocument::Double2String(((double) grayscale)/255.,3) + _T(" G");
}

void
wxPdfColour::SetColor(const wxColour& color)
{
  m_color = wxPdfDocument::RGB2String(color) + _T(" RG");
}

void
wxPdfColour::SetColor(const unsigned char red, const unsigned char green, const unsigned char blue)
{
  SetColor(wxColour(red,green,blue));
}

void
wxPdfColour::SetColor(double cyan, double magenta, double yellow, double black)
{
  m_color = wxPdfDocument::Double2String(wxPdfDocument::ForceRange(cyan,    0., 100.)/100.,3) + _T(" ") +
            wxPdfDocument::Double2String(wxPdfDocument::ForceRange(magenta, 0., 100.)/100.,3) + _T(" ") +
            wxPdfDocument::Double2String(wxPdfDocument::ForceRange(yellow,  0., 100.)/100.,3) + _T(" ") +
            wxPdfDocument::Double2String(wxPdfDocument::ForceRange(black,   0., 100.)/100.,3) + _T(" K");
}

void
wxPdfColour::SetColor(const wxString& name)
{
  if (name.Length() == 7 && name[0] == wxT('#'))
  {
    unsigned long r = 0, g = 0, b = 0;
    if (name.Mid(1,2).ToULong(&r,16) &&
        name.Mid(3,2).ToULong(&g,16) &&
        name.Mid(5,2).ToULong(&b,16))
    {
      SetColor((unsigned char) r, (unsigned char) g, (unsigned char) b);
    }
    else
    {
     SetColor(0);
    }
  }
  else
  {
    wxColourDatabase* colorDatabase = GetColorDatabase();
    wxColour color = colorDatabase->Find(name);
    if (color.Ok())
    {
      SetColor(color);
    }
    else
    {
      SetColor(0);
    }
  }
}

const wxString
wxPdfColour::GetColor(bool drawing) const
{
  wxString color = (drawing) ? m_color.Upper() : m_color.Lower();
  color.Replace(_T("/cs"), _T("/CS"));
  return color;
}

wxPdfSpotColour::wxPdfSpotColour(int index, double cyan, double magenta, double yellow, double black)
  : m_objIndex(0), m_index(index), m_cyan(cyan), m_magenta(magenta), m_yellow(yellow), m_black(black)
{
}

wxPdfSpotColour::wxPdfSpotColour(const wxPdfSpotColour& color)
{
  m_objIndex = color.m_objIndex;
  m_index    = color.m_index;
  m_cyan     = color.m_cyan;
  m_magenta  = color.m_magenta;
  m_yellow   = color.m_yellow;
  m_black    = color.m_black;
}

// ---

void
wxPdfDocument::AddSpotColor(const wxString& name, double cyan, double magenta, double yellow, double black)
{
  wxPdfSpotColourMap::iterator spotColor = (*m_spotColors).find(name);
  if (spotColor == (*m_spotColors).end())
  {
    int i = (*m_spotColors).size() + 1;
    (*m_spotColors)[name] = new wxPdfSpotColour(i, cyan, magenta, yellow, black);
  }
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
wxPdfDocument::SetDrawColor(const unsigned char red, const unsigned char green, const unsigned char blue)
{
  SetDrawColor(wxColour(red, green, blue));
}

void
wxPdfDocument::SetDrawColor(double cyan, double magenta, double yellow, double black)
{
  SetDrawColor(wxPdfColour(cyan, magenta, yellow, black));
}

void
wxPdfDocument::SetDrawColor(const wxPdfColour& color)
{
  m_drawColor = color.GetColor(true);
  if (m_page > 0)
  {
    OutAscii(m_drawColor);
  }
}

void
wxPdfDocument::SetDrawColor(const wxString& name, double tint)
{
  wxPdfSpotColourMap::iterator spotColor = (*m_spotColors).find(name);
  if (spotColor != (*m_spotColors).end())
  {
    m_drawColor = wxString::Format(_T("/CS%d CS "), spotColor->second->GetIndex()) +
                  Double2String(ForceRange(tint, 0., 100.)/100.,3) + _T(" SCN");
    if (m_page > 0)
    {
      OutAscii(m_drawColor);
    }
  }
  else
  {
    wxLogError(_("SetDrawColor: Undefined spot color: ") + name);
  }
}

const wxPdfColour
wxPdfDocument::GetDrawColor()
{
  return wxPdfColourInternal(m_drawColor);
}

void
wxPdfDocument::SetFillColor(const wxColour& color)
{
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
  m_fillColor = Double2String(((double) grayscale)/255.,3) + _T(" g");
  m_colorFlag = (m_fillColor != m_textColor);
  if (m_page > 0)
  {
    OutAscii(m_fillColor);
  }
}

void
wxPdfDocument::SetFillColor(const wxPdfColour& color)
{
  m_fillColor = color.GetColor(false);
  m_colorFlag = (m_fillColor != m_textColor);
  if (m_page > 0)
  {
    OutAscii(m_fillColor);
  }
}

void
wxPdfDocument::SetFillColor(const unsigned char red, const unsigned char green, const unsigned char blue)
{
  SetFillColor(wxColour(red, green, blue));
}

void
wxPdfDocument::SetFillColor(double cyan, double magenta, double yellow, double black)
{
  SetFillColor(wxPdfColour(cyan, magenta, yellow, black));
}

void
wxPdfDocument::SetFillColor(const wxString& name, double tint)
{
  wxPdfSpotColourMap::iterator spotColor = (*m_spotColors).find(name);
  if (spotColor != (*m_spotColors).end())
  {
    m_fillColor = wxString::Format(_T("/CS%d cs "), spotColor->second->GetIndex()) +
                  Double2String(ForceRange(tint, 0., 100.)/100.,3) + _T(" scn");
    m_colorFlag = (m_fillColor != m_textColor);
    if (m_page > 0)
    {
      OutAscii(m_fillColor);
    }
  }
  else
  {
    wxLogError(_("SetFillColor: Undefined spot color: ") + name);
  }
}

const wxPdfColour
wxPdfDocument::GetFillColor()
{
  return wxPdfColourInternal(m_fillColor);
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
wxPdfDocument::SetTextColor(const wxPdfColour& color)
{
  m_textColor = color.GetColor(false);
  m_colorFlag = (m_fillColor != m_textColor);
}

void
wxPdfDocument::SetTextColor(const unsigned char red, const unsigned char green, const unsigned char blue)
{
  SetTextColor(wxColour(red, green, blue));
}

void
wxPdfDocument::SetTextColor(double cyan, double magenta, double yellow, double black)
{
  SetTextColor(wxPdfColour(cyan, magenta, yellow, black));
}

void
wxPdfDocument::SetTextColor(const wxString& name, double tint)
{
  wxPdfSpotColourMap::iterator spotColor = (*m_spotColors).find(name);
  if (spotColor != (*m_spotColors).end())
  {
    m_textColor = wxString::Format(_T("/CS%d cs "), spotColor->second->GetIndex()) +
                  Double2String(ForceRange(tint, 0., 100.)/100.,3) + _T(" scn");
    m_colorFlag = (m_fillColor != m_textColor);
  }
  else
  {
    wxLogError(_("SetTextColor: Undefined spot color: ") + name);
  }
}
 
const wxPdfColour
wxPdfDocument::GetTextColor()
{
  return wxPdfColourInternal(m_textColor);
}


