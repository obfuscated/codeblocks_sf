/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef SETTINGS_H
#define SETTINGS_H

/*
Exclude VC++, because it has silly constraints on importing/exporting classes
from DLLs. Instead, we build "sdk" as a static library
*/
#if defined(__WXMSW__)
	#ifndef DLLIMPORT
		#if defined(EXPORT_LIB)
			#define DLLIMPORT __declspec (dllexport)
		#else
			#define DLLIMPORT __declspec (dllimport)
		#endif // EXPORT_LIB
	#endif // DLLIMPORT
	#ifndef EVTIMPORT
		#if defined(EXPORT_EVENTS)
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
