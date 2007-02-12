#ifndef CB_BASE64_H
#define CB_BASE64_H

#include <wx/defs.h>
#include <wx/string.h>
#include "settings.h"

namespace wxBase64
{
    DLLIMPORT wxString Encode(const wxUint8* pData, size_t len);
    DLLIMPORT wxString Encode(const wxString& data);
    DLLIMPORT wxString Decode(const wxString& data);
};

#endif // CB_BASE64_H
