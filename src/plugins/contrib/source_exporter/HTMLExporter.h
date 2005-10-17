#ifndef HTMLEXPORTER_INCLUDED
#define HTMLEXPORTER_INCLUDED

#include <wx/wx.h>
#include <editorcolorset.h>
#include <string>

using std::string;

class HTMLExporter
{
  public:
    string Export(wxString title, const wxMemoryBuffer &styled_text, const EditorColorSet *color_set);

  private:
    static const char *HTMLHeaderBEG;
    static const char *HTMLMeta;
    static const char *HTMLStyleBEG;
    static string HTMLStyle(const EditorColorSet *color_set, HighlightLanguage lang);
    static const char *HTMLStyleEND;
    static const char *HTMLHeaderEND;
    static const char *HTMLBodyBEG;
    static string HTMLBody(const wxMemoryBuffer &styled_text);
    static const char *HTMLBodyEND;
};

#endif // HTMLEXPORTER_INCLUDED
