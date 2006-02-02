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

	// Fix 'for loop scoping' for visual C++ 6
	/// @todo Fix this version number...My SP5 VC++6 is version 1200, but what about
	/// other SPs? (1-4,6)
	#if _MSC_VER <= 1200
		#define for if(0); else for
	#endif
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

#include <wx/string.h>
#include <wx/dynarray.h>
#if wxCHECK_VERSION(2, 5, 0)
    #include <wx/arrstr.h>
#endif

#define DECLARE_CB_EVENT_TYPE(type) \
	extern EVTIMPORT const wxEventType type;
#define DEFINE_CB_EVENT_TYPE(type) \
	const wxEventType type = wxNewEventType();

extern const DLLIMPORT wxString DEFAULT_WORKSPACE;

extern const DLLIMPORT wxString WORKSPACES_FILES_FILTER;
extern const DLLIMPORT wxString CODEBLOCKS_FILES_FILTER;
extern const DLLIMPORT wxString DEVCPP_FILES_FILTER;
extern const DLLIMPORT wxString MSVC6_FILES_FILTER;
extern const DLLIMPORT wxString MSVC7_FILES_FILTER;
extern const DLLIMPORT wxString MSVC6_WORKSPACE_FILES_FILTER;
extern const DLLIMPORT wxString MSVC7_WORKSPACE_FILES_FILTER;
extern const DLLIMPORT wxString D_FILES_FILTER;
extern const DLLIMPORT wxString C_FILES_FILTER;
extern const DLLIMPORT wxString SOURCE_FILES_FILTER;
extern const DLLIMPORT wxString HEADER_FILES_FILTER;
extern const DLLIMPORT wxString RESOURCE_FILES_FILTER;
extern const DLLIMPORT wxString ALL_KNOWN_FILES_FILTER;

extern const DLLIMPORT wxString ALL_FILES_FILTER;

extern const DLLIMPORT wxString SOURCE_FILES_DIALOG_FILTER;
extern const DLLIMPORT unsigned SOURCE_FILES_FILTER_INDEX;

extern const DLLIMPORT wxString KNOWN_SOURCES_DIALOG_FILTER;
extern const DLLIMPORT unsigned KNOWN_SOURCES_FILTER_INDEX;

extern const DLLIMPORT wxString WORKSPACE_EXT;
extern const DLLIMPORT wxString CODEBLOCKS_EXT;
extern const DLLIMPORT wxString DEVCPP_EXT;
extern const DLLIMPORT wxString MSVC6_EXT;
extern const DLLIMPORT wxString MSVC7_EXT;
extern const DLLIMPORT wxString MSVC6_WORKSPACE_EXT;
extern const DLLIMPORT wxString MSVC7_WORKSPACE_EXT;
extern const DLLIMPORT wxString D_EXT;
extern const DLLIMPORT wxString CPP_EXT;
extern const DLLIMPORT wxString C_EXT;
extern const DLLIMPORT wxString CC_EXT;
extern const DLLIMPORT wxString CXX_EXT;
extern const DLLIMPORT wxString HPP_EXT;
extern const DLLIMPORT wxString H_EXT;
extern const DLLIMPORT wxString HH_EXT;
extern const DLLIMPORT wxString HXX_EXT;
extern const DLLIMPORT wxString OBJECT_EXT;
extern const DLLIMPORT wxString XRCRESOURCE_EXT;
extern const DLLIMPORT wxString STATICLIB_EXT;
extern const DLLIMPORT wxString DYNAMICLIB_EXT;
extern const DLLIMPORT wxString EXECUTABLE_EXT;
extern const DLLIMPORT wxString RESOURCE_EXT;
extern const DLLIMPORT wxString RESOURCEBIN_EXT;

#endif // SETTINGS_H
