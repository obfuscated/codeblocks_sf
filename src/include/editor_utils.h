/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef CODEBLOCKS_EDITOR_HELPER_H
#define CODEBLOCKS_EDITOR_HELPER_H

#include "settings.h" // DLLIMPORT

class wxScintilla;

namespace cb
{
/// Function which is used to setup the markers for the folding margin in an editor control.
/// The id is the value stored in editor:folding/indicator config
DLLIMPORT void SetFoldingMarkers(wxScintilla *stc, int id);

DLLIMPORT void UnderlineFoldedLines(wxScintilla *stc, bool underline);
} // namespace cb

#endif // CODEBLOCKS_EDITOR_HELPER_H
