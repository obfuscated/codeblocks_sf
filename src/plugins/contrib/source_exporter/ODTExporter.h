#ifndef ODTEXPORTER_INCLUDED
#define ODTEXPORTER_INCLUDED

#include "BaseExporter.h"
#include <wx/wfstream.h>
#include <wx/zipstrm.h>
#include <string>

using std::string;

class ODTExporter : public BaseExporter
{
  public:
    void Export(const wxString &filename, const wxString &title, const wxMemoryBuffer &styled_text, const EditorColorSet *color_set);

  private:
    static const char *ODTManifestFile;
    static const char *ODTMetaFile;
    static const char *ODTMIMETypeFile;
    static const char *ODTSettingsFile;
    static const char *ODTStylesFileBEG;
    static string ODTStylesFileMID(wxZipOutputStream &zout); // Sets and returns the font
    static const char *ODTStylesFileEND;
    static const char *ODTContentFileBEG;
    static const char *ODTContentFileEND;
    static void ODTCreateDirectoryStructure(wxZipOutputStream &zout);
    static void ODTCreateCommonFiles(wxZipOutputStream &zout);
    static void ODTCreateStylesFile(wxZipOutputStream &zout, const EditorColorSet *color_set, HighlightLanguage lang);
    static void ODTCreateContentFile(wxZipOutputStream &zout, const wxMemoryBuffer &styled_text);
    /*static const char *HTMLHeaderBEG;
    static const char *HTMLMeta;
    static const char *HTMLStyleBEG;
    static string HTMLStyle(const EditorColorSet *color_set, HighlightLanguage lang);
    static const char *HTMLStyleEND;
    static const char *HTMLHeaderEND;
    static const char *HTMLBodyBEG;
    static string HTMLBody(const wxMemoryBuffer &styled_text);
    static const char *HTMLBodyEND;*/
};

#endif // ODTEXPORTER_INCLUDED
