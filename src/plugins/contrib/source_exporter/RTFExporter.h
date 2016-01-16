#ifndef RTFEXPORTER_INCLUDED
#define RTFEXPORTER_INCLUDED

#include "BaseExporter.h"
#include <vector>
#include <string>

using std::string;
using std::vector;

class RTFExporter : public BaseExporter
{
  public:
    struct Style
    {
      int value;
      int backIdx;
      int foreIdx;
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

    static string RTFFontTable(int &pt);
    string RTFColorTable(const EditorColourSet *c_color_set, HighlightLanguage lang);
    static const char *RTFInfo;
    static const char *RTFTitle;
    string RTFBody(const wxMemoryBuffer &styled_text, int pt, int lineCount, int tabWidth);
    static const char *RTFEnd;
};

#endif // RTFEXPORTER_INCLUDED
