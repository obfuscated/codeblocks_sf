/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#if defined(_MSC_VER)
	/*
	Needed for VC6. Windows headers are included somewhere after wxWidgets, and that
	causes the stupid CreateWindow/CreateDialog/FindWindow/...etc macros (that resolve
	to ANSI and UNICODE versions) to invalidate all calls to such methods in our
	code
	*/
	#include <wx/msw/winundef.h>

	/*
	Disable the warning:
	"<Unknown>' has C-linkage specified, but returns UDT 'wxString' which is
	incompatible with C"
	This is Ok, because we're always using the returned UDT from within C++
	*/
	#pragma warning(disable:4190)

#endif

/*
Exclude VC++, because it has silly constraints on importing/exporting classes
from DLLs. Instead, we build "sdk" as a static library
*/
#if defined(__WXMSW__) && !defined(_MSC_VER)
	#ifndef DLLIMPORT
		#if EXPORT_LIB
			#define DLLIMPORT __declspec (dllexport)
		#else
			#define DLLIMPORT __declspec (dllimport)
		#endif // EXPORT_LIB
	#endif // DLLIMPORT
	#ifndef EVTIMPORT
		#if EXPORT_EVENTS
			#define EVTIMPORT __declspec (dllexport)
		#else
			#define EVTIMPORT __declspec (dllimport)
		#endif // EXPORT_EVENTS
	#endif // EVTIMPORT
#else
	#define DLLIMPORT
	#define EVTIMPORT
#endif

#endif // SETTINGS_H
