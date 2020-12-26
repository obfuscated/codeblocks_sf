#ifndef TINYWXUNI_H
#define TINYWXUNI_H

#include "settings.h"

class wxString;
class TiXmlDocument;

namespace TinyXML
{
    DLLIMPORT bool           LoadDocument(const wxString& filename, TiXmlDocument *doc);
    DLLIMPORT TiXmlDocument* LoadDocument(const wxString& filename);

    DLLIMPORT bool SaveDocument(const wxString& filename, TiXmlDocument* doc);
}

#endif
