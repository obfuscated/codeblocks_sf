///////////////////////////////////////////////////////////////////////////////
// Name:        pdftemplate.h
// Purpose:     
// Author:      Ulrich Telle
// Modified by:
// Created:     2006-07-13
// Copyright:   (c) Ulrich Telle
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/// \file pdftemplate.h Interface of the wxPdfTemplate class

#ifndef _PDFTEMPLATE_H_
#define _PDFTEMPLATE_H_

// wxWidgets headers
#include "wx/pdfdocdef.h"

#include "wx/pdfdoc.h"

/// Class representing a template
class WXDLLIMPEXP_PDFDOC wxPdfTemplate
{
public:
  wxPdfTemplate(int templateId);
  virtual ~wxPdfTemplate();

  int    GetIndex()  { return m_templateId; }

  double GetX()      { return m_x; }
  double GetY()      { return m_y; }
  double GetWidth()  { return m_w; }
  double GetHeight() { return m_h; }

  /// Set object index
  void SetObjIndex(int n) { m_n = n; }

  /// Get object index
  int  GetObjIndex() { return m_n; }

private:
  int    m_templateId;
  int    m_n;

  double m_x;
  double m_y;
  double m_h;
  double m_w;

  wxPdfFontHashMap*    m_fonts;               ///< array of used fonts
  wxPdfImageHashMap*   m_images;              ///< array of used images
  wxPdfTemplatesMap*   m_templates;           ///< array of templates

  double m_xSave;
  double m_ySave;
  bool   m_autoPageBreakSave;
  double m_bMarginSave;
  double m_tMarginSave;
  double m_lMarginSave;
  double m_rMarginSave;
  double m_hSave;
  double m_wSave;

  wxMemoryOutputStream m_buffer;              ///< buffer holding in-memory PDF

  friend class wxPdfDocument;
};

#endif

