/*
 * This plugin was created thanks to wxPdfDocument library
 */

#include "PDFExporter.h"
#include "wx/pdfdoc.h"
#include <configmanager.h>
#include <wx/fontutil.h>
#include <cstdlib>
#include <algorithm>

using std::size_t;
using std::find;

namespace
{
  // Helper function to write text
  inline void PDFWriteText(wxPdfDocument &pdf, const wxString &text)
  {
    if (!text.IsEmpty())
    {
      // TODO (Ceniza#5#): Get a background colour drawn without "erasing" text around
      // Must be fixed in wxPdfDocument first
      pdf.Cell(pdf.GetStringWidth(text), 4, text);//, wxPDF_BORDER_NONE, 0, wxPDF_ALIGN_LEFT, 1);
    }
  }
};

bool PDFExporter::Style::operator == (int aValue)
{
  return value == aValue;
}

void PDFExporter::PDFSetFont(wxPdfDocument &pdf)
{
  wxString fontstring = ConfigManager::Get()->Read(_T("/editor/font"), wxEmptyString);
  wxString faceName(_T("Courier"));
  pdf.SetFont(faceName); // Set Courier as default
  int pt = 8; // Default point size

  if (!fontstring.IsEmpty())
  {
    wxFont tmpFont;
    wxNativeFontInfo nfi;
    nfi.FromString(fontstring);
    tmpFont.SetNativeFontInfo(nfi);

    pt = tmpFont.GetPointSize();
    faceName = tmpFont.GetFaceName();
  }

  pdf.SetFont(faceName); // Try to set the new font, if it fails it'll use the default one
  pdf.SetFontSize(pt);
}

void PDFExporter::PDFGetStyles(const EditorColorSet *c_color_set, HighlightLanguage lang)
{
  m_styles.clear(); // Be sure the styles are cleared
  defStyleIdx = -1; // No default style

  if (lang != HL_NONE)
  {
    const int count = const_cast<EditorColorSet *>(c_color_set)->GetOptionCount(lang);

    for (int i = 0; i < count; ++i)
    {
      OptionColor *optc = const_cast<EditorColorSet *>(c_color_set)->GetOptionByIndex(lang, i);

      if (!optc->isStyle)
      {
        continue;
      }

      Style tmpStyle =
      {
        optc->value,
        optc->back,
        optc->fore,
        optc->bold,
        optc->italics,
        optc->underlined
      };

      m_styles.push_back(tmpStyle);

      // Default Style
      if (optc->value == 0)
      {
        defStyleIdx = m_styles.size() - 1;
      }
    }
  }
}

void PDFExporter::PDFBody(wxPdfDocument &pdf, const wxMemoryBuffer &styled_text)
{
  const char *buffer = reinterpret_cast<char *>(styled_text.GetData());
  const size_t buffer_size = styled_text.GetDataLen();

  pdf.AddPage();

  if (buffer_size == 0)
  {
    return;
  }

  // Get the current style from the first character
  char current_style = buffer[1];

  // If the first style isn't the default style...
  if (current_style != 0)
  {
    vector<Style>::iterator i = find(m_styles.begin(), m_styles.end(), current_style);

    if (i != m_styles.end())
    {
      wxString style;

      if (i->bold)
      {
        style += "B";
      }

      if (i->italics)
      {
        style += "I";
      }

      if (i->underlined)
      {
        style += "U";
      }

      pdf.SetFont(wxEmptyString, style);
      pdf.SetTextColor(i->fore);

      if (i->back.Ok())
      {
        pdf.SetFillColor(i->back);
      }
      else
      {
        pdf.SetFillColor(*wxWHITE);
      }
    }
  }

  wxString text;

  for (size_t i = 0; i < buffer_size; i += 2)
  {
    if (buffer[i + 1] != current_style)
    {
      if (!isspace(buffer[i]))
      {
        PDFWriteText(pdf, text);
        text.Empty();

        current_style = buffer[i + 1];

        vector<Style>::iterator newStyle = find(m_styles.begin(), m_styles.end(), current_style);

        if (newStyle != m_styles.end())
        {
          wxString style;

          if (newStyle->bold)
          {
            style += "B";
          }

          if (newStyle->italics)
          {
            style += "I";
          }

          if (newStyle->underlined)
          {
            style += "U";
          }

          pdf.SetFont(wxEmptyString, style);
          pdf.SetTextColor(newStyle->fore);

          if (newStyle->back.Ok())
          {
            pdf.SetFillColor(newStyle->back);
          }
          else
          {
            pdf.SetFillColor(*wxWHITE);
          }
        }
        else if (defStyleIdx != -1)
        {
          pdf.SetFont(wxEmptyString);
          pdf.SetTextColor(*wxBLACK);
          pdf.SetFillColor(*wxWHITE);
        }
      }
    }

    switch (buffer[i])
    {
      case '\r':
        break;

      case '\n':
        PDFWriteText(pdf, text);
        text.Empty();
        pdf.Ln();
        break;

      default:
        text += _T(buffer[i]);
        break;
    };
  }

  PDFWriteText(pdf, text);
}

void PDFExporter::Export(const wxString &filename, const wxString &title, const wxMemoryBuffer &styled_text, const EditorColorSet *color_set)
{
  wxPdfDocument pdf;
  HighlightLanguage lang = const_cast<EditorColorSet *>(color_set)->GetLanguageForFilename(title);

  PDFSetFont(pdf);
  PDFGetStyles(color_set, lang);
  PDFBody(pdf, styled_text);

  pdf.SaveAsFile(filename);
}
