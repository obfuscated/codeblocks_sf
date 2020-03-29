/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef HELPER_H
#define HELPER_H

#include <wx/string.h>

namespace nsHeaderFixUp
{
  bool IsInsideMultilineComment(wxString& Line);
  bool IsInsideString          (wxString& Line);
  bool IsNextChar(const wxChar&   NextCharInLine,
                  const wxChar&   ThisChar,
                  const wxString& RemainingLine);
}

#endif // HELPER_H
