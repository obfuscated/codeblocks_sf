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

#define DECLARE_CB_EVENT_TYPE(type) \
	extern EVTIMPORT const wxEventType type;
#define DEFINE_CB_EVENT_TYPE(type) \
	const wxEventType type = wxNewEventType();

#define DEFAULT_WORKSPACE			"default.workspace"

#define WORKSPACES_FILES_FILTER		"Code::Blocks workspace files (*.workspace)|*.workspace"
#define CODEBLOCKS_FILES_FILTER		"Code::Blocks project files (*.cbp)|*.cbp"
#define DEVCPP_FILES_FILTER         "Bloodshed Dev-C++ project files (*.dev)|*.dev"
#define MSVC_FILES_FILTER           "MS Visual C++ project files (*.dsp)|*.dsp"
#define MSVS_FILES_FILTER           "MS Visual Studio project files (*.vcproj)|*.vcproj"
#define MSVC_WORKSPACE_FILES_FILTER "MS Visual C++ workspace files (*.dsw)|*.dsw"
#define MSVS_WORKSPACE_FILES_FILTER "MS Visual Studio solution files (*.sln)|*.sln"
#define C_FILES_FILTER              "C/C++ files (*.c;*.cpp;*.cc;*.cxx;*.h;*.hpp;*.hh;*.hxx)|*.c;*.cpp;*.cc;*.cxx;*.h;*.hpp;*.hh;*.hxx"
#define SOURCE_FILES_FILTER         "C/C++ source files (*.c;*.cpp;*.cc;*.cxx)|*.c;*.cpp;*.cc;*.cxx"
#define HEADER_FILES_FILTER         "C/C++ header files (*.h;*.hpp;*.hh;*.hxx)|*.h;*.hpp;*.hh;*.hxx"
#define RESOURCE_FILES_FILTER       "Resource files (*.xrc;*.rc)|*.xrc;*.rc"
#define ALL_KNOWN_FILES_FILTER      "All known files|*.workspace;*.cbp;*.c;*.cpp;*.cc;*.cxx;*.h;*.hpp;*.hh;*.hxx;*.xrc;*.rc"

#define ALL_FILES_FILTER            "All files (*.*)|*.*"

#define SOURCE_FILES_DIALOG_FILTER  WORKSPACES_FILES_FILTER"|" \
									CODEBLOCKS_FILES_FILTER"|" \
                                    C_FILES_FILTER"|" \
                                    SOURCE_FILES_FILTER"|" \
                                    HEADER_FILES_FILTER"|" \
                                    RESOURCE_FILES_FILTER"|" \
                                    ALL_KNOWN_FILES_FILTER"|" \
                                    ALL_FILES_FILTER
#define SOURCE_FILES_FILTER_INDEX   6

#define KNOWN_SOURCES_DIALOG_FILTER C_FILES_FILTER"|" \
                                    SOURCE_FILES_FILTER"|" \
                                    HEADER_FILES_FILTER"|" \
                                    RESOURCE_FILES_FILTER"|" \
                                    ALL_FILES_FILTER
#define KNOWN_SOURCES_FILTER_INDEX  4

#define WORKSPACE_EXT		"workspace"
#define CODEBLOCKS_EXT		"cbp"
#define DEVCPP_EXT			"dev"
#define MSVC_EXT			"dsp"
#define MSVS_EXT			"vcproj"
#define MSVC_WORKSPACE_EXT  "dsw"
#define MSVS_WORKSPACE_EXT	"sln"
#define CPP_EXT				"cpp"
#define C_EXT				"c"
#define CC_EXT				"cc"
#define CXX_EXT				"cxx"
#define HPP_EXT				"hpp"
#define H_EXT				"h"
#define HH_EXT				"hh"
#define HXX_EXT				"hxx"
#define LUA_EXT				"lua"
#define OBJECT_EXT			"o"
#define XRCRESOURCE_EXT		"xrc"
#define STATICLIB_EXT		"a"
#ifdef __WXMSW__
	#define DYNAMICLIB_EXT	"dll"
	#define EXECUTABLE_EXT	"exe"
	#define RESOURCE_EXT	"rc"
	#define RESOURCEBIN_EXT	"res"
#else
	#define DYNAMICLIB_EXT	"so"
	#define EXECUTABLE_EXT	""
	#define RESOURCE_EXT	""
	#define RESOURCEBIN_EXT	""
#endif

#endif // SETTINGS_H

