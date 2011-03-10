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
    void Export(const wxString &filename, const wxString &title, const wxMemoryBuffer &styled_text, const EditorColourSet *color_set, int lineCount, int tabWidth);

  private:
    vector<Style> m_styles;
    int defStyleIdx;

    static void PDFSetFont(wxPdfDocument &pdf);
    void PDFGetStyles(const EditorColourSet *c_color_set, HighlightLanguage lang);
    void PDFBody(wxPdfDocument &pdf, const wxMemoryBuffer &styled_text, int lineCount, int tabWidth);
};

#endif // PDFEXPORTER_INCLUDED
