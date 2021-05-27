/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifdef __WXMSW__
// Only used on Windows

#ifndef SDK_GLOBALS_CYGWIN_H
#define SDK_GLOBALS_CYGWIN_H

#include "settings.h"

extern DLLIMPORT bool isDetectedCygwinCompiler(void);
extern DLLIMPORT wxString getCygwinCompilerPathRoot(void);
extern DLLIMPORT void GetWindowsPathFromCygwinPath(wxString& path);
extern DLLIMPORT void GetCygwinPathFromWindowsPath(wxString& path);

#endif // SDK_GLOBALS_CYGWIN_H

#endif // __WXMSW__
