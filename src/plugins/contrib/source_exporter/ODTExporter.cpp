/*
 * Some of this code is based in the ODT output of OpenOffice.org 2
 * http://books.evc-cit.info/odbook/book.html was also really helpful
 */

#include "ODTExporter.h"
#include <configmanager.h>
#include <wx/fontutil.h>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <iomanip>

using std::strlen;
using std::ostringstream;
using std::hex;
using std::setw;
using std::setfill;
using std::uppercase;
using std::size_t;

namespace
{
  // Helper function to calculate the width of a number (ugly way)
  inline int calcWidth(int num)
  {
    if (num < 0)
    {
      return 0;
    }

    int width = 1;

    while ((num /= 10) != 0)
    {
      ++width;
    }

    return width;
  }

  // Helper function to convert i to a string
  inline string to_string(int i)
  {
    ostringstream ostr;

    ostr << i;

    return ostr.str();
  }

  // Helper function that returns a number of spaces as <text:s text:c="#"/> or " " and updates the index
  // When called, str[*index] must be a space
  // If force is true, it'll return in the form <text:s text:c="#"/> even if it's one space
  inline string fix_spaces(const char *str, size_t *index, size_t max, bool force = false)
  {
    int counter = 0;

    while (*index < max && str[*index] == ' ')
    {
      ++counter;
      *index += 2;
    }

    *index -= 2;

    if (counter == 1 && !force)
    {
      return string(" ");
    }

    return string("<text:s text:c=\"") + to_string(counter) + string("\"/>");
  }
};

const char *ODTExporter::ODTManifestFile =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
  "<!DOCTYPE manifest:manifest PUBLIC \"-//OpenOffice.org//DTD Manifest 1.0//EN\" \"Manifest.dtd\">\n"
  "<manifest:manifest xmlns:manifest=\"urn:oasis:names:tc:opendocument:xmlns:manifest:1.0\">\n"
  "  <manifest:file-entry manifest:media-type=\"application/vnd.oasis.opendocument.text\" manifest:full-path=\"/\"/>\n"
  "  <manifest:file-entry manifest:media-type=\"application/vnd.sun.xml.ui.configuration\" manifest:full-path=\"Configurations2/\"/>\n"
  "  <manifest:file-entry manifest:media-type=\"\" manifest:full-path=\"Pictures/\"/>\n"
  "  <manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"content.xml\"/>\n"
  "  <manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"styles.xml\"/>\n"
  "  <manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"meta.xml\"/>\n"
  "  <manifest:file-entry manifest:media-type=\"\" manifest:full-path=\"Thumbnails/\"/>\n"
  "  <manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"settings.xml\"/>\n"
  "</manifest:manifest>";

const char *ODTExporter::ODTMetaFile =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
  "<office:document-meta\n"
  "  xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\"\n"
  "  xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
  "  xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n"
  "  xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\"\n"
  "  office:version=\"1.0\">\n"
  "  <office:meta>\n"
  "    <meta:generator>Code::Blocks Source Exporter plugin</meta:generator>\n"
  "    <dc:title>Source file exported from Code::Blocks Source Exporter plugin</dc:title>\n"
  "  </office:meta>\n"
  "</office:document-meta>";

const char *ODTExporter::ODTMIMETypeFile =
  "application/vnd.oasis.opendocument.text";

const char *ODTExporter::ODTSettingsFile =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
  "<office:document-settings\n"
  "  xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\"\n"
  "  xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
  "  xmlns:config=\"urn:oasis:names:tc:opendocument:xmlns:config:1.0\"\n"
  "  office:version=\"1.0\">\n"
  "<office:settings/>\n"
  "</office:document-settings>";

const char *ODTExporter::ODTStylesFileBEG =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
  "<office:document-styles\n"
  "  xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\"\n"
  "  xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\"\n"
  "  xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\"\n"
  "  xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\"\n"
  "  xmlns:draw=\"urn:oasis:names:tc:opendocument:xmlns:drawing:1.0\"\n"
  "  xmlns:fo=\"urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0\"\n"
  "  xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
  "  xmlns:number=\"urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0\"\n"
  "  xmlns:presentation=\"urn:oasis:names:tc:opendocument:xmlns:presentation:1.0\"\n"
  "  xmlns:svg=\"urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0\"\n"
  "  xmlns:dom=\"http://www.w3.org/2001/xml-events\"\n"
  "  office:version=\"1.0\">\n";

string ODTExporter::ODTStylesFileMID(wxZipOutputStream &zout)
{
  static const char *t1 =
    "<office:font-face-decls>\n"
    "  <style:font-face style:name=\""; // FontName
  static const char *t2 =
    "\" svg:font-family=\""; // FontName
  static const char *t3 =
    "\"/>\n"
    "</office:font-face-decls>\n"
    "<office:styles>\n"
    "<style:style style:family=\"paragraph\"\n"
    "  style:name=\"Default\"\n"
    "  style:display-name=\"Default\"\n"
    "  style:parent-style-name=\"Standard\"\n"
    "  style:class=\"text\">\n"
    "  <style:text-properties style:font-name=\""; // FontName
  static const char *t4 =
    "\" fo:font-size=\""; // Pt
  static const char *t5 =
    "pt\"/>\n"
    "</style:style>\n";

  string theFont("Courier New");
  string thePt("8");

  wxString fontstring = Manager::Get()->GetConfigManager(_T("editor"))->Read(_T("/font"), wxEmptyString);

  if (!fontstring.IsEmpty())
  {
    wxFont tmpFont;
    wxNativeFontInfo nfi;
    nfi.FromString(fontstring);
    tmpFont.SetNativeFontInfo(nfi);

    thePt = to_string(tmpFont.GetPointSize());
    wxString faceName = tmpFont.GetFaceName();

    if (!faceName.IsEmpty())
    {
      theFont = string(faceName.mb_str());
    }
  }

  zout.Write(t1, strlen(t1));
  zout.Write(theFont.c_str(), theFont.size());
  zout.Write(t2, strlen(t2));
  zout.Write(theFont.c_str(), theFont.size());
  zout.Write(t3, strlen(t3));
  zout.Write(theFont.c_str(), theFont.size());
  zout.Write(t4, strlen(t4));
  zout.Write(thePt.c_str(), thePt.size());
  zout.Write(t5, strlen(t5));

  return theFont;
}

const char *ODTExporter::ODTStylesFileEND =
  "</office:styles>\n"
  "</office:document-styles>";

const char *ODTExporter::ODTContentFileBEG =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
  "<office:document-content\n"
  "  xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\"\n"
  "  xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\"\n"
  "  xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\"\n"
  "  xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\"\n"
  "  xmlns:draw=\"urn:oasis:names:tc:opendocument:xmlns:drawing:1.0\"\n"
  "  xmlns:fo=\"urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0\"\n"
  "  xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
  "  xmlns:number=\"urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0\"\n"
  "  xmlns:presentation=\"urn:oasis:names:tc:opendocument:xmlns:presentation:1.0\"\n"
  "  xmlns:svg=\"urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0\"\n"
  "  xmlns:form=\"urn:oasis:names:tc:opendocument:xmlns:form:1.0\"\n"
  "  xmlns:script=\"urn:oasis:names:tc:opendocument:xmlns:script:1.0\"\n"
  "  xmlns:dom=\"http://www.w3.org/2001/xml-events\"\n"
  "  office:version=\"1.0\">\n"
  "<office:scripts/>\n"
  "<office:font-face-decls/>\n"
  "<office:body>\n"
  "<office:text>\n";

const char *ODTExporter::ODTContentFileEND =
  "</office:text>\n"
  "</office:body>\n"
  "</office:document-content>";

void ODTExporter::ODTCreateDirectoryStructure(wxZipOutputStream &zout)
{
  zout.PutNextEntry(_T("META-INF/"));
  zout.PutNextEntry(_T("Thumbnails/"));
  zout.PutNextEntry(_T("Pictures/"));
  zout.PutNextEntry(_T("Configurations2/"));
}

void ODTExporter::ODTCreateCommonFiles(wxZipOutputStream &zout)
{
  zout.PutNextEntry(_T("META-INF/manifest.xml"));
  zout.Write(ODTManifestFile, strlen(ODTManifestFile));
  zout.PutNextEntry(_T("meta.xml"));
  zout.Write(ODTMetaFile, strlen(ODTMetaFile));
  zout.PutNextEntry(_T("mimetype"));
  zout.Write(ODTMIMETypeFile, strlen(ODTMIMETypeFile));
  zout.PutNextEntry(_T("settings.xml"));
  zout.Write(ODTSettingsFile, strlen(ODTSettingsFile));
}

void ODTExporter::ODTCreateStylesFile(wxZipOutputStream &zout, const EditorColourSet *color_set, HighlightLanguage lang)
{
  zout.PutNextEntry(_T("styles.xml"));
  zout.Write(ODTStylesFileBEG, strlen(ODTStylesFileBEG));
  string fontName = ODTStylesFileMID(zout);

  if (lang != HL_NONE)
  {
    const int count = const_cast<EditorColourSet *>(color_set)->GetOptionCount(lang);

    for (int i = 0; i < count; ++i)
    {
      OptionColour *optc = const_cast<EditorColourSet *>(color_set)->GetOptionByIndex(lang, i);

      if (!optc->isStyle)
      {
        continue;
      }

      ostringstream ostr;

      ostr << "<style:style style:name=\"style" << optc->value << "\" style:family=\"text\">\n"
           << "  <style:text-properties\n"
           << "    style:font-name=\"" << fontName << "\"\n"
           << "    fo:color=\"#" << hex << setfill('0')
           << setw(2) << static_cast<unsigned int>(optc->fore.Red())
           << setw(2) << static_cast<unsigned int>(optc->fore.Green())
           << setw(2) << static_cast<unsigned int>(optc->fore.Blue())
           << "\"";

      if (optc->back.Ok())
      {
        ostr << "\n    fo:background-color=\"#"
             << setw(2) << static_cast<unsigned int>(optc->back.Red())
             << setw(2) << static_cast<unsigned int>(optc->back.Green())
             << setw(2) << static_cast<unsigned int>(optc->back.Blue())
             << "\"";
      }

      if (optc->bold)
      {
        ostr << "\n    fo:font-weight=\"bold\"";
      }

      if (optc->italics)
      {
        ostr << "\n    fo:font-style=\"italic\"";
      }

      if (optc->underlined)
      {
        ostr << "\n    style:text-underline-style=\"solid\""
             << "\n    style:text-underline-width=\"normal\""
             << "\n    style:text-underline-color=\"font-color\""
             << "\n    style:text-underline-mode=\"skip-white-space\"";
      }

      ostr << " />\n"
           << "</style:style>\n";

      zout.Write(ostr.str().c_str(), ostr.str().size());
    }
  }

  zout.Write(ODTStylesFileEND, strlen(ODTStylesFileEND));
}

void ODTExporter::ODTCreateContentFile(wxZipOutputStream &zout, const wxMemoryBuffer &styled_text, int lineCount, int tabWidth)
{
  const char *buffer = reinterpret_cast<char *>(styled_text.GetData());
  const size_t buffer_size = styled_text.GetDataLen();
  int lineno = 1;
  int width = calcWidth(lineCount);

  zout.PutNextEntry(_T("content.xml"));
  zout.Write(ODTContentFileBEG, strlen(ODTContentFileBEG));

  if (buffer_size)
  {
    char current_style = buffer[1];
    string content("<text:h text:style-name=\"Default\">");

    if (lineCount != -1)
    {
      int difWidth = width - calcWidth(lineno);

      if (difWidth > 0)
      {
        content += string("<text:s text:c=\"") + to_string(difWidth) + string("\"/>");
      }

      content += to_string(lineno);
      ++lineno;
      content += "<text:s text:c=\"2\"/>";
    }

    size_t first = 0;

    if (buffer_size > 0 && buffer[0] == ' ')
    {
      content += fix_spaces(buffer, &first, buffer_size, true);
    }

    if (current_style != 0)
    {
      content += string("<text:span text:style-name=\"style") + to_string(current_style) + string("\">");
    }

    int charLinePos = 0;

    for (size_t i = first; i < buffer_size; i += 2, ++charLinePos)
    {
      if (buffer[i + 1] != current_style)
      {
        if (!isspace(buffer[i]))
        {
          if (current_style != 0)
          {
            content += string("</text:span>");
          }

          current_style = buffer[i + 1];

          if (current_style != 0)
          {
            content += string("<text:span text:style-name=\"style") + to_string(current_style) + string("\">");
          }
        }
      }

      switch (buffer[i])
      {
        case '<':
          content += "&lt;";
          break;

        case '>':
          content += "&gt;";
          break;

        case '&':
          content += "&amp;";
          break;

        case '\'':
          content += "&apos;";
          break;

        case '"':
          content += "&quot;";
          break;

        case ' ':
          content += fix_spaces(buffer, &i, buffer_size);
          break;

        case '\t':
          {
            const int extraSpaces = tabWidth - charLinePos % tabWidth;
            size_t dummy = 0;
            const std::string extraSpacesStr(extraSpaces * 2, ' '); // imitates buffer (char + style)
            content += fix_spaces(extraSpacesStr.c_str(), &dummy, extraSpacesStr.size());
            charLinePos += extraSpaces - 1; // account for auto-increment
          }
          break;

        case '\r':
          --charLinePos; // account for auto-increment
          break;

        case '\n':
          if (current_style != 0)
          {
            content += string("</text:span>");
            current_style = 0;
          }

          content += "</text:h>\n";
          content += "<text:h text:style-name=\"Default\">";

          if (lineCount != -1)
          {
            int difWidth = width - calcWidth(lineno);

            if (difWidth > 0)
            {
              content += string("<text:s text:c=\"") + to_string(difWidth) + string("\"/>");
            }

            content += to_string(lineno);
            ++lineno;
            content += "<text:s text:c=\"2\"/>";
          }

          if (i + 2 < buffer_size && buffer[i + 2] == ' ')
          {
            i += 2;
            content += fix_spaces(buffer, &i, buffer_size, true);
          }

          charLinePos = -1; // account for auto-increment
          break;

        default:
          content += buffer[i];
          break;
      }
    }

    if (current_style != 0)
    {
      content += string("</text:span>");
    }

    content += "</text:h>\n";

    zout.Write(content.c_str(), content.size());
  }

  zout.Write(ODTContentFileEND, strlen(ODTContentFileEND));
}

void ODTExporter::Export(const wxString &filename, const wxString &title, const wxMemoryBuffer &styled_text, const EditorColourSet *color_set, int lineCount, int tabWidth)
{
  HighlightLanguage lang = const_cast<EditorColourSet *>(color_set)->GetLanguageForFilename(title);

  wxFileOutputStream file(filename);
  wxZipOutputStream zout(file);

  ODTCreateDirectoryStructure(zout);
  ODTCreateCommonFiles(zout);
  ODTCreateStylesFile(zout, color_set, lang);
  ODTCreateContentFile(zout, styled_text, lineCount, tabWidth);
}
