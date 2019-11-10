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
    void Export(const wxString &filename, const wxString &title, const wxMemoryBuffer &styled_text, const EditorColourSet *color_set, int lineCount, int tabWidth);

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
    static void ODTCreateStylesFile(wxZipOutputStream &zout, const EditorColourSet *color_set, HighlightLanguage lang);
    static void ODTCreateContentFile(wxZipOutputStream &zout, const wxMemoryBuffer &styled_text, int lineCount, int tabWidth);
};

#endif // ODTEXPORTER_INCLUDED
