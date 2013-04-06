/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef CRC32_H
#define CRC32_H

#include "settings.h"
#include <wx/defs.h>

class wxString;

namespace wxCrc32
{
    DLLIMPORT wxUint32 FromFile(const wxString& filename);
    DLLIMPORT wxUint32 FromString(const wxString& text);
}

#endif // CRC32_H
