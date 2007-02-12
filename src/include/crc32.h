#ifndef CRC32_H
#define CRC32_H

#include "settings.h"
#include <wx/defs.h>

class wxString;

namespace wxCrc32
{
    DLLIMPORT wxUint32 FromFile(const wxString& filename);
    DLLIMPORT wxUint32 FromString(const wxString& text);
};

#endif // CRC32_H
