#ifndef TINYWXUNI_H
#define TINYWXUNI_H

class wxString;
class TiXmlDocument;

namespace TinyXML
{
    extern DLLIMPORT bool           LoadDocument(const wxString& filename, TiXmlDocument *doc);
    extern DLLIMPORT TiXmlDocument* LoadDocument(const wxString& filename);

    extern DLLIMPORT bool SaveDocument(const wxString& filename, TiXmlDocument* doc);
}

#endif
