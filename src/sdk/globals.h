#ifndef SDK_GLOBALS_H
#define SDK_GLOBALS_H

#include "settings.h"
#include <wx/string.h>
#include <wx/treectrl.h>
#include <wx/intl.h>

enum PluginType
{
    ptNone = 1,
    ptTool,
	ptMime,
    ptCompiler,
    ptDebugger,
	ptCodeCompletion,
	ptProjectWizard,
    ptOther
};

enum ModuleType
{
	mtProjectManager = 1,
	mtEditorManager,
	mtMessageManager,
	mtClassBrowser,
	mtDocumentsManager,
	mtUnknown
};

struct DebuggerBreakpoint
{
	int line;
	int ignoreCount;
	bool enabled;
	wxString func;
};

enum FileType
{
	ftCodeBlocksProject = 0,
    ftCodeBlocksWorkspace,
	ftDevCppProject,
	ftMSVCProject,
	ftMSVSProject,
	ftMSVCWorkspace,
	ftMSVSWorkspace,
	ftSource,
	ftHeader,
	ftObject,
	ftXRCResource,
	ftResource,
	ftResourceBin,
	ftStaticLib,
	ftDynamicLib,
	ftExecutable,
	ftOther
};

#define DEFAULT_ARRAY_SEP _T(";")

#ifdef __cplusplus
extern "C" {
#endif
// global helper funcs
extern DLLIMPORT wxString GetStringFromArray(const wxArrayString& array, const wxString& separator = DEFAULT_ARRAY_SEP);
extern DLLIMPORT wxArrayString GetArrayFromString(const wxString& text, const wxString& separator = DEFAULT_ARRAY_SEP, bool trimSpaces = true);
extern DLLIMPORT bool CreateDirRecursively(const wxString& full_path, int perms = 0755);
extern DLLIMPORT wxString UnixFilename(const wxString& filename);
extern DLLIMPORT FileType FileTypeOf(const wxString& filename);
extern DLLIMPORT void SaveTreeState(wxTreeCtrl* tree, const wxTreeItemId& parent, wxArrayString& nodePaths);
extern DLLIMPORT void RestoreTreeState(wxTreeCtrl* tree, const wxTreeItemId& parent, wxArrayString& nodePaths);
extern DLLIMPORT wxString ChooseDirectory(wxWindow* parent,
                                          const wxString& message = _("Select directory"),
                                          const wxString& initialPath = _T(""),
                                          const wxString& basePath = _T(""),
                                          bool askToMakeRelative = false, // relative to basePath
                                          bool showCreateDirButton = false); // where supported
#ifdef __cplusplus
};
#endif

#endif // SDK_GLOBALS_H
