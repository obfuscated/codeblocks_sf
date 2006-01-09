#ifndef SETTINGS_H
#define SETTINGS_H

#if wxUSE_UNICODE
    #define _UU(x,y) wxString((x),(y))
    #define _CC(x,y) (x).mb_str((y))
#else
    #define _UU(x,y) (wxString(x))
    #define _CC(x,y) (x)
#endif

#define _U(x) _UU((x),wxConvUTF8)
#define _C(x) _CC((x),wxConvUTF8)

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

#define DEFAULT_WORKSPACE			_T("default.workspace")

#define WORKSPACES_FILES_FILTER		_("Code::Blocks workspace files (*.workspace)|*.workspace")
#define CODEBLOCKS_FILES_FILTER		_("Code::Blocks project files (*.cbp)|*.cbp")
#define DEVCPP_FILES_FILTER         _("Bloodshed Dev-C++ project files (*.dev)|*.dev")
#define MSVC_FILES_FILTER           _("MS Visual C++ project files (*.dsp)|*.dsp")
#define MSVS_FILES_FILTER           _("MS Visual Studio project files (*.vcproj)|*.vcproj")
#define MSVC_WORKSPACE_FILES_FILTER _("MS Visual C++ workspace files (*.dsw)|*.dsw")
#define MSVS_WORKSPACE_FILES_FILTER _("MS Visual Studio solution files (*.sln)|*.sln")
#define C_FILES_FILTER              _("C/C++ files (*.c;*.cpp;*.cc;*.cxx;*.h;*.hpp;*.hh;*.hxx)|*.c;*.cpp;*.cc;*.cxx;*.h;*.hpp;*.hh;*.hxx")
#define SOURCE_FILES_FILTER         _("C/C++ source files (*.c;*.cpp;*.cc;*.cxx)|*.c;*.cpp;*.cc;*.cxx")
#define HEADER_FILES_FILTER         _("C/C++ header files (*.h;*.hpp;*.hh;*.hxx)|*.h;*.hpp;*.hh;*.hxx")
#define RESOURCE_FILES_FILTER       _("Resource files (*.xrc;*.rc)|*.xrc;*.rc")
#define ALL_KNOWN_FILES_FILTER      _("All known files|*.workspace;*.cbp;*.c;*.cpp;*.cc;*.cxx;*.h;*.hpp;*.hh;*.hxx;*.xrc;*.rc")

#ifdef __WXMSW__
#define ALL_FILES_FILTER            _("All files (*.*)|*.*")
#else
#define ALL_FILES_FILTER            _("All files (*)|*")
#endif

#define SOURCE_FILES_DIALOG_FILTER  wxString(WORKSPACES_FILES_FILTER) + _T("|") + \
									         CODEBLOCKS_FILES_FILTER + _T("|") + \
                                             C_FILES_FILTER + _T("|") + \
                                             SOURCE_FILES_FILTER + _T("|") + \
                                             HEADER_FILES_FILTER + _T("|") + \
                                             RESOURCE_FILES_FILTER + _T("|") + \
                                             ALL_KNOWN_FILES_FILTER + _T("|") + \
                                             ALL_FILES_FILTER
#define SOURCE_FILES_FILTER_INDEX   6

#define KNOWN_SOURCES_DIALOG_FILTER wxString(C_FILES_FILTER) + _T("|") + \
                                             SOURCE_FILES_FILTER + _T("|") + \
                                             HEADER_FILES_FILTER + _T("|") + \
                                             RESOURCE_FILES_FILTER + _T("|") + \
                                             ALL_FILES_FILTER
#define KNOWN_SOURCES_FILTER_INDEX  4

#define WORKSPACE_EXT		_T("workspace")
#define CODEBLOCKS_EXT		_T("cbp")
#define DEVCPP_EXT			_T("dev")
#define MSVC_EXT			_T("dsp")
#define MSVS_EXT			_T("vcproj")
#define MSVC_WORKSPACE_EXT  _T("dsw")
#define MSVS_WORKSPACE_EXT	_T("sln")
#define CPP_EXT				_T("cpp")
#define C_EXT				_T("c")
#define CC_EXT				_T("cc")
#define CXX_EXT				_T("cxx")
#define HPP_EXT				_T("hpp")
#define H_EXT				_T("h")
#define HH_EXT				_T("hh")
#define HXX_EXT				_T("hxx")
#define OBJECT_EXT			_T("o")
#define XRCRESOURCE_EXT		_T("xrc")
#define STATICLIB_EXT		_T("a")
#ifdef __WXMSW__
	#define DYNAMICLIB_EXT	_T("dll")
	#define EXECUTABLE_EXT	_T("exe")
	#define RESOURCE_EXT	_T("rc")
	#define RESOURCEBIN_EXT	_T("res")
#else
	#define DYNAMICLIB_EXT	_T("so")
	#define EXECUTABLE_EXT	_T("")
	#define RESOURCE_EXT	_T("")
	#define RESOURCEBIN_EXT	_T("")
#endif

// Convenience Macros for the SDK
#define  EDMAN() Manager::Get()->GetEditorManager()
#define PRJMAN() Manager::Get()->GetProjectManager()
#define MSGMAN() Manager::Get()->GetMessageManager()
#define TLSMAN() Manager::Get()->GetToolsManager()
#define MACMAN() Manager::Get()->GetMacrosManager()
#define PRSMAN() Manager::Get()->GetPersonalityManager()

#define CFGMAN() OldConfigManager::Get()
#define CFG_READ(...)    OldConfigManager::Get()->Read(__VA_ARGS__)
#define CFG_WRITE(...) OldConfigManager::Get()->Write(__VA_ARGS__)

#endif // SETTINGS_H

