///////////////////////////////////////////////////////////////////////////////
// Name:        pdffont.cpp
// Purpose:     Implementation of wxPdfFont classes
// Author:      Ulrich Telle
// Modified by:
// Created:     2005-08-04
// Copyright:   (c) Ulrich Telle
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/// \file pdffont.cpp Implementation of the wxPdfFont class

//#if defined(__GNUG__) && !defined(__APPLE__)
//#pragma implementation "pdffont.h"
//#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/tokenzr.h"

#include "wx/pdfdoc.h"
#include "wx/pdffont.h"

static wxString
GetNodeContent(const wxXmlNode *node)
{
  const wxXmlNode *n = node;
  if (n == NULL) return wxEmptyString;
   n = n->GetChildren();

  while (n)
  {
    if (n->GetType() == wxXML_TEXT_NODE ||
        n->GetType() == wxXML_CDATA_SECTION_NODE)
      return n->GetContent();
    n = n->GetNext();
  }

  return wxEmptyString;
}

static bool
GetFontDescription(const wxXmlNode *node, wxPdfFontDescription& fontDescription)
{
  bool bAscent       = false,
       bDescent      = false,
       bCapheight    = false,
       bFlags        = false,
       bFontbbox     = false,
       bItalicangle  = false,
       bStemv        = false,
       bMissingwidth = false;
  wxString value;
  long number;
  wxXmlNode* child = node->GetChildren();
  while (child)
  {
    // parse the children
    if (child->GetName() == wxT("ascent"))
    {
      value = GetNodeContent(child);
      if (value.ToLong(&number))
      {
        bAscent = true;
        fontDescription.SetAscent(number);
      }
    }
    else if (child->GetName() == wxT("descent"))
    {
      value = GetNodeContent(child);
      if (value.ToLong(&number))
      {
        bDescent = true;
        fontDescription.SetDescent(number);
      }
    }
    else if (child->GetName() == wxT("cap-height"))
    {
      value = GetNodeContent(child);
      if (value.ToLong(&number))
      {
        bCapheight = true;
        fontDescription.SetCapHeight(number);
      }
    }
    else if (child->GetName() == wxT("flags"))
    {
      value = GetNodeContent(child);
      if (value.ToLong(&number))
      {
        bFlags = true;
        fontDescription.SetFlags(number);
      }
    }
    else if (child->GetName() == wxT("font-bbox"))
    {
      value = GetNodeContent(child);
      if (value.Length() > 0 && value[0] == _T('[') && value.Last() == _T(']'))
      {
        bFontbbox = true;
        fontDescription.SetFontBBox(value);
      }
    }
    else if (child->GetName() == wxT("italic-angle"))
    {
      value = GetNodeContent(child);
      if (value.ToLong(&number))
      {
        bItalicangle = true;
        fontDescription.SetItalicAngle(number);
      }
    }
    else if (child->GetName() == wxT("stem-v"))
    {
      value = GetNodeContent(child);
      if (value.ToLong(&number))
      {
        bStemv = true;
        fontDescription.SetStemV(number);
      }
    }
    else if (child->GetName() == wxT("missing-width"))
    {
      value = GetNodeContent(child);
      if (value.ToLong(&number))
      {
        bMissingwidth = true;
        fontDescription.SetMissingWidth(number);
      }
    }
    child = child->GetNext();
  }
  return (bAscent && bDescent && bCapheight && bFlags && bFontbbox &&
          bItalicangle && bStemv && bMissingwidth);
}

wxPdfFontDescription::wxPdfFontDescription()
{
}

wxPdfFontDescription::~wxPdfFontDescription()
{
}

wxMBConv* wxPdfFont::ms_winEncoding = NULL;

wxMBConv*
wxPdfFont::GetWinEncodingConv()
{
  if (ms_winEncoding == NULL)
  {
    static wxCSConv winEncoding(wxFONTENCODING_CP1252);
    ms_winEncoding = &winEncoding;
  }
  return ms_winEncoding;
}

// ----------------------------------------------------------------------------
// wxPdfFont: class 
// ----------------------------------------------------------------------------

wxPdfFont::wxPdfFont(int index, const wxString& name, short* cwArray, const wxString& bbox)
{
  m_index = index;
  m_name  = name;
  m_type  = _T("core");
  m_up = -100;
  m_ut = 50;
  m_desc.SetFontBBox(bbox);

  if (cwArray != NULL)
  {
    m_cw = new wxPdfCharWidthMap();
    int j;
    for (j = 0; j <256; j++)
    {
      (*m_cw)[j] = cwArray[j];
    }
  }
  else
  {
    m_cw = NULL;
  }

  m_enc   = wxEmptyString;
  m_diffs = wxEmptyString;
  m_file  = wxEmptyString;
  m_ctg   = wxEmptyString;
  m_size1 = -1;
  m_size2 = -1;
}

wxPdfFont::~wxPdfFont()
{
  delete m_cw;
}

wxString
wxPdfFont::GetWidthsAsString()
{
  wxString s = wxString(_T("["));
  int i;
  for (i = 32; i <= 255; i++)
  {
    s += wxString::Format(_T("%d "), (*m_cw)[i]);
  }
  s += wxString(_T("]"));
  return s;
}

#if wxUSE_UNICODE
wxMBConv*
wxPdfFont::GetEncodingConv()
{
  wxMBConv* conv;
  if (m_type == _T("core") && (m_name == _T("") || m_name == _T("")))
  {
    conv = &wxConvISO8859_1;
  }
  else
  {
    conv = GetWinEncodingConv();
  }
  return conv;
}
#endif

double
wxPdfFont::GetStringWidth(const wxString& s)
{
  double w = 0;
  // Get width of a string in the current font
#if wxUSE_UNICODE
  wxCharBuffer wcb(s.mb_str(*GetEncodingConv()));
  const char* str = (const char*) wcb;
#else
  const char* str = s.c_str();
#endif

  size_t i;
  for (i = 0; i < s.Length(); i++)
  {
    w += (*m_cw)[(unsigned char) str[i]];
  }
  return w / 1000;
}

int
wxPdfFont::GetBBoxTopPosition()
{
  long top = 1000;
  wxString bBox = m_desc.GetFontBBox();
  wxStringTokenizer tkz(bBox, _T(" []"), wxTOKEN_STRTOK);
  if (tkz.CountTokens() >= 4)
  {
    tkz.GetNextToken();
    tkz.GetNextToken();
    tkz.GetNextToken();
    wxString topToken = tkz.GetNextToken();
    topToken.ToLong(&top);
  }
  return top;
}

// ----------------------------------------------------------------------------
// wxPdfFontTrueType: class 
// ----------------------------------------------------------------------------

wxPdfFontTrueType::wxPdfFontTrueType(int index)
  : wxPdfFont(index)
{
  m_type  = _T("TrueType");
  m_conv = NULL;
}

wxPdfFontTrueType::~wxPdfFontTrueType()
{
  // delete m_cw;
  if (m_conv != NULL)
  {
    delete m_conv;
  }
}

bool
wxPdfFontTrueType::LoadFontMetrics(wxXmlNode* root)
{
  bool bName  = false,
       bDesc  = false,
       bUp    = false,
       bUt    = false,
       bFile  = true,
       bSize  = true,
       bWidth = false;
  wxString value;
  long number;
  wxXmlNode *child = root->GetChildren();
  while (child)
  {
    // parse the children
    if (child->GetName() == wxT("font-name"))
    {
      m_name = GetNodeContent(child);
      bName = m_name.Length() > 0;
    }
    else if (child->GetName() == wxT("encoding"))
    {
      m_enc = GetNodeContent(child);
    }
    else if (child->GetName() == wxT("description"))
    {
      bDesc = GetFontDescription(child, m_desc);
    }
    else if (child->GetName() == wxT("underline-position"))
    {
      value = GetNodeContent(child);
      if (value.ToLong(&number))
      {
        bUp = true;
        m_up = number;
      }
    }
    else if (child->GetName() == wxT("underline-thickness"))
    {
      value = GetNodeContent(child);
      if (value.ToLong(&number))
      {
        bUt = true;
        m_ut = number;
      }
    }
    else if (child->GetName() == wxT("diff"))
    {
      m_diffs = GetNodeContent(child);
    }
    else if (child->GetName() == wxT("file"))
    {
      value = child->GetPropVal(_T("name"), _T(""));
      if (value.Length() > 0)
      {
        m_file = value;
        value = child->GetPropVal(_T("originalsize"), _T(""));
        if (value.Length() > 0 && value.ToLong(&number))
        {
          bFile = true;
          m_size1 = number;
        }
        else
        {
          bFile = false;
          m_file = _T("");
        }
      }
    }
    else if (child->GetName() == wxT("widths"))
    {
      bWidth = true;
      m_cw = new wxPdfCharWidthMap();
      const wxXmlNode *charNode = child->GetChildren();
      while (charNode)
      {
        wxString strId, strWidth;
        long charId, charWidth;
        if (charNode->GetName() == wxT("char"))
        {
          strId = charNode->GetPropVal(_T("id"), _T(""));
          strWidth = charNode->GetPropVal(_T("width"), _T(""));
          if (strId.Length() > 0 && strId.ToLong(&charId) &&
              strWidth.Length() > 0 && strWidth.ToLong(&charWidth))
          {
            (*m_cw)[charId] = charWidth;
          }
        }
        charNode = charNode->GetNext();
      }
    }
    child = child->GetNext();
  }

#if wxUSE_UNICODE
  // Unicode build needs charset conversion
  if (m_conv == NULL)
  {
    if (m_enc.Length() > 0)
    {
      m_conv = new wxCSConv(m_enc);
    }
    else
    {
      m_conv = new wxCSConv(wxFONTENCODING_ISO8859_1);
    }
  }
#endif

  return (bName && bDesc && bUp && bUt && bFile && bSize && bWidth);
}

double
wxPdfFontTrueType::GetStringWidth(const wxString& s)
{
  // Get width of a string in the current font
  double w = 0;
#if wxUSE_UNICODE
  wxCharBuffer wcb(s.mb_str(*m_conv));
  const char* str = (const char*) wcb;
#else
  const char* str = s.c_str();
#endif

  wxPdfCharWidthMap::iterator charIter;
  size_t i;
  for (i = 0; i < s.Length(); i++)
  {
    wxPdfCharWidthMap::iterator charIter = (*m_cw).find((unsigned char) str[i]);
    if (charIter != (*m_cw).end())
    {
      w += charIter->second;
    }
    else
    {
      w += m_desc.GetMissingWidth();
    }
  }
  return w / 1000;
}

#if wxUSE_UNICODE

// ----------------------------------------------------------------------------
// wxPdfFontTrueTypeUnicode: class 
// ----------------------------------------------------------------------------

wxPdfFontTrueTypeUnicode::wxPdfFontTrueTypeUnicode(int index)
  : wxPdfFont(index)
{
  m_type  = _T("TrueTypeUnicode");
  m_conv = NULL;
}

wxPdfFontTrueTypeUnicode::~wxPdfFontTrueTypeUnicode()
{
  // delete m_cw;
  if (m_conv != NULL)
  {
    delete m_conv;
  }
}

bool
wxPdfFontTrueTypeUnicode::LoadFontMetrics(wxXmlNode* root)
{
  bool bName  = false,
       bDesc  = false,
       bUp    = false,
       bUt    = false,
       bFile  = true,
       bSize  = true,
       bWidth = false;
  wxString value;
  long number;
  wxXmlNode *child = root->GetChildren();
  while (child)
  {
    // parse the children
    if (child->GetName() == wxT("font-name"))
    {
      m_name = GetNodeContent(child);
      bName = m_name.Length() > 0;
    }
    else if (child->GetName() == wxT("encoding"))
    {
      m_enc = GetNodeContent(child);
    }
    else if (child->GetName() == wxT("description"))
    {
      bDesc = GetFontDescription(child, m_desc);
    }
    else if (child->GetName() == wxT("underline-position"))
    {
      value = GetNodeContent(child);
      if (value.ToLong(&number))
      {
        bUp = true;
        m_up = number;
      }
    }
    else if (child->GetName() == wxT("underline-thickness"))
    {
      value = GetNodeContent(child);
      if (value.ToLong(&number))
      {
        bUt = true;
        m_ut = number;
      }
    }
    else if (child->GetName() == wxT("diff"))
    {
      m_diffs = GetNodeContent(child);
    }
    else if (child->GetName() == wxT("file"))
    {
      value = child->GetPropVal(_T("ctg"), _T(""));
      if (value.Length() > 0)
      {
        bFile = true;
        m_ctg = value;
        value = child->GetPropVal(_T("name"), _T(""));
        if (value.Length() > 0)
        {
          m_file = value;
          value = child->GetPropVal(_T("originalsize"), _T(""));
          if (value.Length() > 0 && value.ToLong(&number))
          {
            bFile = true;
            m_size1 = number;
          }
          else
          {
            bFile = false;
            m_file = _T("");
          }
        }
      }
      else
      {
        bFile = false;
        m_file = _T("");
        m_ctg = _T("");
      }
    }
    else if (child->GetName() == wxT("widths"))
    {
      bWidth = true;
      m_cw = new wxPdfCharWidthMap();
      const wxXmlNode *charNode = child->GetChildren();
      while (charNode)
      {
        wxString strId, strWidth;
        long charId, charWidth;
        if (charNode->GetName() == wxT("char"))
        {
          strId = charNode->GetPropVal(_T("id"), _T(""));
          strWidth = charNode->GetPropVal(_T("width"), _T(""));
          if (strId.Length() > 0 && strId.ToLong(&charId) &&
              strWidth.Length() > 0 && strWidth.ToLong(&charWidth))
          {
            (*m_cw)[charId] = charWidth;
          }
        }
        charNode = charNode->GetNext();
      }
    }
    child = child->GetNext();
  }
  if (m_conv == NULL)
  {
    m_conv = new wxMBConvUTF16BE();
  }
  return (bName && bDesc && bUp && bUt && bFile && bSize && bWidth);
}

wxString
wxPdfFontTrueTypeUnicode::GetWidthsAsString()
{
  wxString s = wxString(_T("["));
  wxPdfCharWidthMap::iterator charIter;
  for (charIter = (*m_cw).begin(); charIter != (*m_cw).end(); charIter++)
  {
    // define a specific width for each individual CID
    s += wxString::Format(_T("%d [%d] "), charIter->first, charIter->second);
  }
  s += wxString(_T("]"));
  return s;
}

double
wxPdfFontTrueTypeUnicode::GetStringWidth(const wxString& s)
{
  // Get width of a string in the current font
  double w = 0;

  wxPdfCharWidthMap::iterator charIter;
  size_t i;
  for (i = 0; i < s.Length(); i++)
  {
    charIter = (*m_cw).find(s[i]);
    if (charIter != (*m_cw).end())
    {
      w += charIter->second;
    }
    else
    {
      w += m_desc.GetMissingWidth();
    }
  }
  return w / 1000;
}

#endif // wxUSE_UNICODE

// ----------------------------------------------------------------------------
// wxPdfFontType1: class 
// ----------------------------------------------------------------------------

wxPdfFontType1::wxPdfFontType1(int index)
  : wxPdfFont(index)
{
  m_type  = _T("Type1");
  m_conv = NULL;
}

wxPdfFontType1::~wxPdfFontType1()
{
  if (m_conv != NULL)
  {
    delete m_conv;
  }
}

bool
wxPdfFontType1::LoadFontMetrics(wxXmlNode* root)
{
  bool bName  = false,
       bDesc  = false,
       bUp    = false,
       bUt    = false,
       bFile  = true,
       bSize  = true,
       bWidth = false;
  wxString value;
  long number;
  wxXmlNode *child = root->GetChildren();
  while (child)
  {
    // parse the children
    if (child->GetName() == wxT("font-name"))
    {
      m_name = GetNodeContent(child);
      bName = m_name.Length() > 0;
    }
    else if (child->GetName() == wxT("encoding"))
    {
      m_enc = GetNodeContent(child);
    }
    else if (child->GetName() == wxT("description"))
    {
      bDesc = GetFontDescription(child, m_desc);
    }
    else if (child->GetName() == wxT("underline-position"))
    {
      value = GetNodeContent(child);
      if (value.ToLong(&number))
      {
        bUp = true;
        m_up = number;
      }
    }
    else if (child->GetName() == wxT("underline-thickness"))
    {
      value = GetNodeContent(child);
      if (value.ToLong(&number))
      {
        bUt = true;
        m_ut = number;
      }
    }
    else if (child->GetName() == wxT("diff"))
    {
      m_diffs = GetNodeContent(child);
    }
    else if (child->GetName() == wxT("file"))
    {
      value = child->GetPropVal(_T("name"), _T(""));
      if (value.Length() > 0)
      {
        m_file = value;
        value = child->GetPropVal(_T("size1"), _T(""));
        if (value.Length() > 0 && value.ToLong(&number))
        {
          bFile = true;
          m_size1 = number;
          value = child->GetPropVal(_T("size2"), _T(""));
          if (value.Length() > 0 && value.ToLong(&number))
          {
            m_size2 = number;
          }
        }
        else
        {
          bFile = false;
          m_file = _T("");
        }
      }
    }
    else if (child->GetName() == wxT("widths"))
    {
      bWidth = true;
      m_cw = new wxPdfCharWidthMap();
      const wxXmlNode *charNode = child->GetChildren();
      while (charNode)
      {
        wxString strId, strWidth;
        long charId, charWidth;
        if (charNode->GetName() == wxT("char"))
        {
          strId = charNode->GetPropVal(_T("id"), _T(""));
          strWidth = charNode->GetPropVal(_T("width"), _T(""));
          if (strId.Length() > 0 && strId.ToLong(&charId) &&
              strWidth.Length() > 0 && strWidth.ToLong(&charWidth))
          {
            (*m_cw)[charId] = charWidth;
          }
        }
        charNode = charNode->GetNext();
      }
    }
    child = child->GetNext();
  }

#if wxUSE_UNICODE
  // Unicode build needs charset conversion
  if (m_conv == NULL)
  {
    if (m_enc.Length() > 0)
    {
      m_conv = new wxCSConv(m_enc);
    }
    else
    {
      m_conv = new wxCSConv(wxFONTENCODING_ISO8859_1);
    }
  }
#endif

  return (bName && bDesc && bUp && bUt && bFile && bSize && bWidth);
}

double
wxPdfFontType1::GetStringWidth(const wxString& s)
{
  // Get width of a string in the current font
  double w = 0;
#if wxUSE_UNICODE
  wxCharBuffer wcb(s.mb_str(*m_conv));
  const char* str = (const char*) wcb;
#else
  const char* str = s.c_str();
#endif

  wxPdfCharWidthMap::iterator charIter;
  size_t i;
  for (i = 0; i < s.Length(); i++)
  {
    wxPdfCharWidthMap::iterator charIter = (*m_cw).find((unsigned char) str[i]);
    if (charIter != (*m_cw).end())
    {
      w += charIter->second;
    }
    else
    {
      w += m_desc.GetMissingWidth();
    }
  }
  return w / 1000;
}

#if wxUSE_UNICODE

// ----------------------------------------------------------------------------
// wxPdfFontType0: class 
// ----------------------------------------------------------------------------

wxPdfFontType0::wxPdfFontType0(int index)
  : wxPdfFont(index)
{
  m_type = _T("Type0");
  m_conv = NULL;
  m_hwRange = false;
}

wxPdfFontType0::~wxPdfFontType0()
{
  if (m_conv != NULL)
  {
    delete m_conv;
  }
}

bool
wxPdfFontType0::LoadFontMetrics(wxXmlNode* root)
{
  bool bName     = false,
       bDesc     = false,
       bUp       = false,
       bUt       = false,
       bRegistry = false,
       bCmap     = false,
       bWidth    = false;
  wxString value;
  long number;
  wxXmlNode *child = root->GetChildren();
  while (child)
  {
    // parse the children
    if (child->GetName() == wxT("font-name"))
    {
      m_name = GetNodeContent(child);
      bName = m_name.Length() > 0;
    }
    else if (child->GetName() == wxT("encoding"))
    {
      m_enc = GetNodeContent(child);
    }
    else if (child->GetName() == wxT("description"))
    {
      bDesc = GetFontDescription(child, m_desc);
    }
    else if (child->GetName() == wxT("underline-position"))
    {
      value = GetNodeContent(child);
      if (value.ToLong(&number))
      {
        bUp = true;
        m_up = number;
      }
    }
    else if (child->GetName() == wxT("underline-thickness"))
    {
      value = GetNodeContent(child);
      if (value.ToLong(&number))
      {
        bUt = true;
        m_ut = number;
      }
    }
    else if (child->GetName() == wxT("cmap"))
    {
      m_cmap = wxEmptyString;
      value = GetNodeContent(child);
      if (value.Length() > 0)
      {
        bCmap = true;
        m_cmap = value;
      }
    }
    else if (child->GetName() == wxT("registry"))
    {
      m_ordering = wxEmptyString;
      m_supplement = wxEmptyString;
      value = child->GetPropVal(_T("ordering"), _T(""));
      if (value.Length() > 0)
      {
        m_ordering = value;
        value = child->GetPropVal(_T("supplement"), _T(""));
        if (value.Length() > 0)
        {
          bRegistry = true;
          m_supplement = value;
        }
        else
        {
          bRegistry = false;
        }
      }
    }
    else if (child->GetName() == wxT("widths"))
    {
      bWidth = true;
      m_cw = new wxPdfCharWidthMap();
      const wxXmlNode *charNode = child->GetChildren();
      while (charNode)
      {
        wxString strId, strWidth;
        long charId, charWidth;
        if (charNode->GetName() == wxT("char"))
        {
          strId = charNode->GetPropVal(_T("id"), _T(""));
          strWidth = charNode->GetPropVal(_T("width"), _T(""));
          if (strId.Length() > 0 && strId.ToLong(&charId) &&
              strWidth.Length() > 0 && strWidth.ToLong(&charWidth))
          {
            (*m_cw)[charId] = charWidth;
          }
        }
        charNode = charNode->GetNext();
      }
    }
    child = child->GetNext();
  }
  if (m_conv == NULL)
  {
    if (m_enc.Length() > 0)
    {
      m_conv = new wxCSConv(m_enc);
    }
    else
    {
      m_conv = new wxCSConv(wxFONTENCODING_ISO8859_1);
    }
  }
  if (m_ordering == _T("Japan1"))
  {
    m_hwRange = true;
    m_hwFirst = 0xff61;
    m_hwLast  = 0xff9f;
  }
  return (bName && bDesc && bUp && bUt && bRegistry && bCmap && bWidth);
}

wxString
wxPdfFontType0::GetWidthsAsString()
{
  wxString s = wxString(_T("[1 ["));
  int i;
  for (i = 32; i <= 126; i++)
  {
    s += wxString::Format(_T("%d "), (*m_cw)[i]);
  }
  s += wxString(_T("]"));
  if (HasHalfWidthRange())
  {
    s += wxString(_T(" 231 325 500 631 [500] 326 389 500"));
  }
  s += wxString(_T("]"));
  return s;
}

double
wxPdfFontType0::GetStringWidth(const wxString& s)
{
  // Get width of a string in the current font
  double w = 0;
  wxPdfCharWidthMap::iterator charIter;
  size_t i;
  for (i = 0; i < s.Length(); i++)
  {
    wxChar c = s[i];
    if (c >= 0 && c < 128)
    {
      wxPdfCharWidthMap::iterator charIter = (*m_cw).find(c);
      if (charIter != (*m_cw).end())
      {
        w += charIter->second;
      }
      else
      {
        // TODO: ???
        // w += m_desc.GetMissingWidth();
      }
    }
    else
    {
      if (HasHalfWidthRange() &&
          c >= HalfWidthRangeFirst() &&
          c <= HalfWidthRangeLast())
      {
        w += 500;
      }
      else
      {
        w += 1000;
      }
    }
  }
  return w / 1000;
}

#endif // wxUSE_UNICODE
