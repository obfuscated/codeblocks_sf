/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */
#ifndef CODEBLOCKS_SDK_CYGWIN_H
#define CODEBLOCKS_SDK_CYGWIN_H

#include "settings.h"

extern DLLIMPORT bool cbIsDetectedCygwinCompiler(void);
extern DLLIMPORT wxString cbGetCygwinCompilerPathRoot(void);
extern DLLIMPORT void cbGetWindowsPathFromCygwinPath(wxString& path);
extern DLLIMPORT void cbGetCygwinPathFromWindowsPath(wxString& path);

#endif // CODEBLOCKS_SDK_CYGWIN_H
