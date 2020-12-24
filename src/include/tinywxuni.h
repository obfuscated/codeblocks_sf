#ifndef TINYWXUNI_H
#define TINYWXUNI_H

class wxString;
class TiXmlDocument;

namespace TinyXML
{
    DLLIMPORT bool           LoadDocument(const wxString& filename, TiXmlDocument *doc);
    DLLIMPORT TiXmlDocument* LoadDocument(const wxString& filename);

    DLLIMPORT bool SaveDocument(const wxString& filename, TiXmlDocument* doc);
}

#endif
