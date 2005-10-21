#ifndef RTFEXPORTER_INCLUDED
#define RTFEXPORTER_INCLUDED

#include <wx/wx.h>
#include <editorcolorset.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

class RTFExporter
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
    string Export(wxString title, const wxMemoryBuffer &styled_text, const EditorColorSet *color_set);

  private:
    vector<Style> m_styles;
    int defStyleIdx;

    static string RTFFontTable(int &pt);
    string RTFColorTable(const EditorColorSet *c_color_set, HighlightLanguage lang);
    static const char *RTFInfo;
    static const char *RTFTitle;
    string RTFBody(const wxMemoryBuffer &styled_text, int pt);
    static const char *RTFEnd;
};

#endif // RTFEXPORTER_INCLUDED
