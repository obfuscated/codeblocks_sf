#ifndef SDK_GLOBALS_H
#define SDK_GLOBALS_H

#include "settings.h"
#include <wx/string.h>
#include <wx/treectrl.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/intl.h>
#include <wx/msgdlg.h>

class TiXmlDocument;

/// Known plugin types
enum PluginType
{
    ptNone = 1,
    ptTool,
	ptMime,
    ptCompiler,
    ptDebugger,
	ptCodeCompletion,
	ptWizard,
    ptOther
};

/// The type of module offering a context menu.
enum ModuleType
{
	mtProjectManager = 1,
	mtEditorManager,
	mtMessageManager,
	mtOpenFilesList,                //pecan 2006/03/22
	mtUnknown
};

/// Known file types
enum FileType
{
	ftCodeBlocksProject = 0,
    ftCodeBlocksWorkspace,
	ftDevCppProject,
	ftMSVC6Project,
	ftMSVC7Project,
	ftMSVC6Workspace,
	ftMSVC7Workspace,
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

/** These are valid values for the state of each project file.
    If a value is casted to integer, it corresponds to the tree's imagelist index for the state.
*/
enum FileVisualState
{
    fvsNormal = 0,
    fvsMissing,
    fvsModified,
    fvsReadOnly,
    // The following are related to version control systems (vc)
    fvsVcAdded,
    fvsVcConflict,
    fvsVcMissing,
    fvsVcModified,
    fvsVcOutOfDate,
    fvsVcUpToDate,
    fvsVcRequiresLock,

    /// do *not* use this, it exists just to know the number of items...
    fvsLast
};

/** Template output types. */
enum TemplateOutputType
{
    totProject = 0, ///< template outputs a new project
    totTarget, ///< template adds a new target in a project
    totFiles, ///< template outputs a new file (or files)
    totCustom, ///< template produces custom output (entirely up to the wizard used)
    totUser ///< template is a user-saved project template
};

extern DLLIMPORT const wxString DEFAULT_WORKSPACE;
extern DLLIMPORT const wxString DEFAULT_ARRAY_SEP;
extern DLLIMPORT const wxString DEFAULT_CONSOLE_TERM;
extern DLLIMPORT const wxString DEFAULT_CONSOLE_SHELL;

// global helper funcs
/// Reads a wxString from a non-unicode file. File must be open. File is closed automatically.
extern DLLIMPORT bool cbRead(wxFile& file, wxString& st, wxFontEncoding encoding = wxFONTENCODING_SYSTEM);
/// Reads a wxString from a non-unicode file. File must be open. File is closed automatically.
extern DLLIMPORT wxString cbReadFileContents(wxFile& file, wxFontEncoding encoding = wxFONTENCODING_SYSTEM);
/// Writes a wxString to a non-unicode file. File must be open. File is closed automatically.
extern DLLIMPORT bool cbWrite(wxFile& file, const wxString& buff, wxFontEncoding encoding = wxFONTENCODING_SYSTEM);
/// Writes a wxString to a file. Takes care of unicode and uses a temporary file
/// to save first and then it copies it over the original.
extern DLLIMPORT bool cbSaveToFile(const wxString& filename, const wxString& contents, wxFontEncoding encoding = wxFONTENCODING_SYSTEM, bool bom = false);
/// Saves a TinyXML document correctly, even if the path contains unicode characters.
extern DLLIMPORT bool cbSaveTinyXMLDocument(TiXmlDocument* doc, const wxString& filename);
/// Return @c str as a proper unicode-compatible string
extern DLLIMPORT wxString cbC2U(const char* str);
/// Return multibyte (C string) representation of the string
extern DLLIMPORT wxWX2MBbuf cbU2C(const wxString& str);

extern DLLIMPORT wxString GetStringFromArray(const wxArrayString& array, const wxString& separator = DEFAULT_ARRAY_SEP);
extern DLLIMPORT wxArrayString GetArrayFromString(const wxString& text, const wxString& separator = DEFAULT_ARRAY_SEP, bool trimSpaces = true);
extern DLLIMPORT void AppendArray(const wxArrayString& from, wxArrayString& to);

extern DLLIMPORT wxString UnixFilename(const wxString& filename);
extern DLLIMPORT void QuoteStringIfNeeded(wxString& str);

extern DLLIMPORT FileType FileTypeOf(const wxString& filename);

extern DLLIMPORT void SaveTreeState(wxTreeCtrl* tree, const wxTreeItemId& parent, wxArrayString& nodePaths);
extern DLLIMPORT void RestoreTreeState(wxTreeCtrl* tree, const wxTreeItemId& parent, wxArrayString& nodePaths);

extern DLLIMPORT bool CreateDirRecursively(const wxString& full_path, int perms = 0755);
extern DLLIMPORT wxString ChooseDirectory(wxWindow* parent,
                                          const wxString& message = _("Select directory"),
                                          const wxString& initialPath = _T(""),
                                          const wxString& basePath = _T(""),
                                          bool askToMakeRelative = false, // relative to basePath
                                          bool showCreateDirButton = false); // where supported

extern DLLIMPORT bool NormalizePath(wxFileName& f,const wxString& base);

extern DLLIMPORT wxString URLEncode(const wxString &str);

extern DLLIMPORT wxBitmap LoadPNGWindows2000Hack(const wxString& filename);

/** Finds out if a window is really shown.
  *
  * win->IsShown() is not that good because we don't know if the
  * window's container (parent) is actually shown or not...
  *
  * This is usually used to find out if docked windows are shown.
  *
  * @param win The window in question.
  * @return True if @c win is shown, false if not.
  */
extern DLLIMPORT bool IsWindowReallyShown(wxWindow* win);

/** Icons styles for settings dialogs.
  */
enum SettingsIconsStyle
{
    sisLargeIcons    = 0, ///< Large icons (default)
    sisNoIcons       = 1, ///< No icons, just text
};

class wxListCtrl;

/** Set the icons style for the supplied list control.
  * @param lc The wxListCtrl.
  * @param style The style to use.
  */
extern DLLIMPORT void SetSettingsIconsStyle(wxListCtrl* lc, SettingsIconsStyle style);
/** Get the icons style for the supplied list control.
  * @return The icons style.
  */
extern DLLIMPORT SettingsIconsStyle GetSettingsIconsStyle(wxListCtrl* lc);

class wxWindow;

typedef enum
{
    pdlDont = 0,
    pdlBest,
    pdlCentre,
    pdlHead,
    pdlConstrain,
    pdlClip
}cbPlaceDialogMode;

extern void PlaceWindow(wxWindow *w, cbPlaceDialogMode mode = pdlBest, bool enforce = false);

/** wxMessageBox wrapper.
  *
  * Use this instead of wxMessageBox(), as this uses PlaceWindow() to show it in the correct monitor.
  * @note The return value for wxOK would be wxID_OK and so on for other buttons.
  * So, read wxMessageDialog 's documentation, *not* wxMessageBox 's...
  */
inline int cbMessageBox(const wxString& message, const wxString& caption = wxEmptyString, int style = wxOK, wxWindow *parent = NULL, int x = -1, int y = -1)
{
    wxMessageDialog dlg(parent, message, caption, style, wxPoint(x,y));
    PlaceWindow(&dlg);
    return dlg.ShowModal();
};

inline void NotifyMissingFile(const wxString &name)
{
    wxString msg;
    msg.Printf(_T("The file %s could not be found.\nPlease check your installation."), name.c_str());
    cbMessageBox(msg);
};

#endif // SDK_GLOBALS_H
