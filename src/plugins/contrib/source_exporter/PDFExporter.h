#ifndef PDFEXPORTER_INCLUDED
#define PDFEXPORTER_INCLUDED

#include "BaseExporter.h"
#include <vector>

using std::vector;

class wxPdfDocument; // Forward declaration

class PDFExporter : public BaseExporter
{
  public:
    struct Style
    {
      int value;
      wxColour back;
      wxColour fore;
      bool bold;
      bool italics;
      bool underlined;

      bool operator == (int aValue);
    };

  public:
    void Export(const wxString &filename, const wxString &title, const wxMemoryBuffer &styled_text, const EditorColorSet *color_set);

  private:
    vector<Style> m_styles;
    int defStyleIdx;

    static void PDFSetFont(wxPdfDocument &pdf);
    void PDFGetStyles(const EditorColorSet *c_color_set, HighlightLanguage lang);
    void PDFBody(wxPdfDocument &pdf, const wxMemoryBuffer &styled_text);
};

#endif // PDFEXPORTER_INCLUDED
