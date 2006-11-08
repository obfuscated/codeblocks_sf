///////////////////////////////////////////////////////////////////////////////
// Name:        pdftemplate.cpp
// Purpose:     
// Author:      Ulrich Telle
// Modified by:
// Created:     2006-07-13
// RCS-ID:      $$
// Copyright:   (c) Ulrich Telle
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/// \file pdftemplate.cpp Implementation of the PDF templates

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

// includes
//#include "wx/sstream.h"

//#include "wx/pdfxml.h"

#include "wx/pdfdoc.h"
#include "wx/pdftemplate.h"

wxPdfTemplate::wxPdfTemplate(int templateId)
{
  m_templateId = templateId;
  m_fonts      = new wxPdfFontHashMap();
  m_images     = new wxPdfImageHashMap();
  m_templates  = new wxPdfTemplatesMap();
}

wxPdfTemplate::~wxPdfTemplate()
{
  delete m_fonts;
  delete m_images;
  delete m_templates;
}

int
wxPdfDocument::BeginTemplate(double x, double y, double w, double h)
{
  if (m_page <= 0)
  {
    wxLogError(_("wxPdfDocument::BeginTemplate: You have to add a page first!"));
    return 0;
  }

  // Save settings
  m_templateId++;
  m_currentTemplate = new wxPdfTemplate(m_templateId);

  m_currentTemplate->m_xSave = m_x;
  m_currentTemplate->m_ySave = m_y;
  m_currentTemplate->m_hSave = m_h;
  m_currentTemplate->m_wSave = m_w;
  m_currentTemplate->m_autoPageBreakSave = m_autoPageBreak;
  m_currentTemplate->m_bMarginSave = m_bMargin;
  m_currentTemplate->m_tMarginSave = m_tMargin;
  m_currentTemplate->m_lMarginSave = m_lMargin;
  m_currentTemplate->m_rMarginSave = m_rMargin;

  SetAutoPageBreak(false);
        
  if (x <= 0) x = 0;
  if (y <= 0) y = 0;
  if (w <= 0) w = m_w;
  if (h <= 0) h = m_h;

  // Define own high and width to calculate positions correctly
  m_h = h;
  m_w = w;

  //$this->tpls[$this->tpl]['buffer'] = "";
  m_currentTemplate->m_x = x;
  m_currentTemplate->m_y = y;
  m_currentTemplate->m_w = w;
  m_currentTemplate->m_h = h;

  m_inTemplate = true;
  SetXY(x + m_lMargin, y + m_tMargin);
  SetRightMargin(m_w - w + m_rMargin);

  (*m_templates)[m_templateId] = m_currentTemplate;

  return m_templateId;
}

int
wxPdfDocument::EndTemplate()
{
  int templateId = 0;
  if (m_inTemplate)
  {
    m_inTemplate = false;
    SetAutoPageBreak(m_currentTemplate->m_autoPageBreakSave, m_currentTemplate->m_bMarginSave);
    SetXY(m_currentTemplate->m_xSave, m_currentTemplate->m_ySave);
    m_tMargin = m_currentTemplate->m_tMarginSave;
    m_lMargin = m_currentTemplate->m_lMarginSave;
    m_rMargin = m_currentTemplate->m_rMarginSave;
    m_h = m_currentTemplate->m_hSave;
    m_w = m_currentTemplate->m_wSave;
    templateId = m_templateId;
  }
  return templateId;
}

void
wxPdfDocument::UseTemplate(int templateId, double x, double y, double w, double h)
{
  if (m_page <= 0)
  {
    wxLogError(_("wxPdfDocument::UseTemplate: You have to add a page first!"));
    return;
  }

  wxPdfTemplate* tpl;
  wxPdfTemplatesMap::iterator templateIter = (*m_templates).find(templateId);
  if (templateIter != (*m_templates).end())
  {
    tpl = templateIter->second;
  }
  else
  {
    wxLogWarning(_("wxPdfDocument::UseTemplate: Template %d does not exist!"), templateId);
    return;
  }
            
  if (m_inTemplate)
  {
    // TODO: template muss referenzierte templates registrieren
    // $this->res['tpl'][$this->tpl]['tpls'][$tplidx] =& $this->tpls[$tplidx];
    (*(m_currentTemplate->m_templates))[templateId] = tpl;
  }

  if (x < 0) x = tpl->GetX();
  if (y < 0) y = tpl->GetY();
  GetTemplateSize(templateId, w, h);

  OutAscii(wxString(_T("q ")) +
           Double2String(w / tpl->GetWidth(), 4) + wxString(_T(" 0 0 ")) +
           Double2String(h / tpl->GetHeight(), 4) + wxString(_T(" ")) +
           Double2String(x * m_k, 2) + wxString(_T(" ")) +
           Double2String((m_h - (y + h))*m_k, 2) +  wxString(_T(" cm"))); // Translate
  OutAscii(m_templatePrefix + wxString::Format(_T("%d Do Q"), tpl->GetIndex()));
}
    
void
wxPdfDocument::GetTemplateSize(int templateId, double& w, double& h)
{
  wxPdfTemplate* tpl;
  wxPdfTemplatesMap::iterator templateIter = (*m_templates).find(templateId);
  if (templateIter != (*m_templates).end())
  {
    tpl = templateIter->second;
    if (w <= 0 && h <= 0)
    {
      w = tpl->GetWidth();
      h = tpl->GetHeight();
    }
    if (w <= 0)
    {
      w = h * tpl->GetWidth() / tpl->GetHeight();
    }
    if (h <= 0)
    {
      h = w * tpl->GetHeight() / tpl->GetWidth();
    }
  }
  else
  {
    wxLogWarning(_("wxPdfDocument::UseTemplate: Template %d does not exist!"), templateId);
    w = 0;
    h = 0;
  }
}

