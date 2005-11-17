#ifndef CRC32_H
#define CRC32_H

#include "settings.h"

namespace wxCrc32
{
    DLLIMPORT unsigned long FromFile(const wxString& filename);
    DLLIMPORT unsigned long FromString(const wxString& text);
};

#endif // CRC32_H
