#ifndef TINYWXUNI_H
#define TINYWXUNI_H

class wxString;
class TiXmlDocument;

namespace TinyXML
{
    bool           LoadDocument(const wxString& filename, TiXmlDocument *doc);
    TiXmlDocument* LoadDocument(const wxString& filename);

    bool SaveDocument(const wxString& filename, TiXmlDocument* doc);
}

#endif
