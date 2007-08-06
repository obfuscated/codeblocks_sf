#ifndef SDK_GLOBALS_H
#define SDK_GLOBALS_H

#include "settings.h"
#include <wx/string.h>
#include <wx/treectrl.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/intl.h>
#include <wx/msgdlg.h>
#include <wx/bitmap.h>

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
    ftXcode1Project,
    ftXcode2Project,
    ftSource,
    ftHeader,
    ftObject,
    ftXRCResource,
    ftResource,
    ftResourceBin,
    ftStaticLib,
    ftDynamicLib,
    ftExecutable,
    ftNative,
    ftXMLDocument,
    ftScript,
    ftOther
};

/** These are valid values for the state of each project file.
    If a value is casted to integer, it corresponds to the tree's imagelist index for the state.
    NOTE: Keep in sync with icons loaded in ProjectManager::BuildTree()!
*/
enum FileVisualState
{
    // The following are related to (editable, source-) file states
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
    fvsVcExternal,
    fvsVcGotLock,
    fvsVcLockStolen,
    fvsVcMismatch,
    fvsVcNonControlled,
    // The following are related to C::B workspace/project/folder/virtual
    fvsWorkspace,
    fvsWorkspaceReadOnly,
    fvsProject,
    fvsProjectReadOnly,
    fvsFolder,
    fvsVirtualFolder,

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

/** Supported platforms */
enum SupportedPlatforms
{
    spMac       = 0x01,
    spUnix      = 0x02,
    spWindows   = 0x04,

    spAll       = 0xff
};
// NOTE: if you add/remove platforms, remember to update the relevant Get/Set globals

typedef wxString HighlightLanguage;
#define HL_AUTO _T(" ")
#define HL_NONE _T("  ")

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
extern DLLIMPORT const wxWX2MBbuf cbU2C(const wxString& str);
/// Try converting a C-string from different encodings until a possible match is found.
/// This tries the following encoding converters (in the same order):
/// utf8, system, default and iso8859-1 to iso8859-15.
/// Returns the final encoding detected.
extern DLLIMPORT wxFontEncoding DetectEncodingAndConvert(const char* strIn, wxString& strOut, wxFontEncoding possibleEncoding = wxFONTENCODING_SYSTEM);

extern DLLIMPORT int GetPlatformsFromString(const wxString& platforms);
extern DLLIMPORT wxString GetStringFromPlatforms(int platforms, bool forceSeparate = false);

extern DLLIMPORT wxString GetStringFromArray(const wxArrayString& array, const wxString& separator = DEFAULT_ARRAY_SEP);
extern DLLIMPORT wxArrayString GetArrayFromString(const wxString& text, const wxString& separator = DEFAULT_ARRAY_SEP, bool trimSpaces = true);
extern DLLIMPORT void AppendArray(const wxArrayString& from, wxArrayString& to);

extern DLLIMPORT wxString UnixFilename(const wxString& filename);
extern DLLIMPORT void QuoteStringIfNeeded(wxString& str);

/// Escapes spaces and tabs (NOT quoting the string)
extern DLLIMPORT wxString EscapeSpaces(const wxString& str);

extern DLLIMPORT FileType FileTypeOf(const wxString& filename);

extern DLLIMPORT void SaveTreeState(wxTreeCtrl* tree, const wxTreeItemId& parent, wxArrayString& nodePaths);
extern DLLIMPORT void RestoreTreeState(wxTreeCtrl* tree, const wxTreeItemId& parent, wxArrayString& nodePaths);

extern DLLIMPORT bool CreateDirRecursively(const wxString& full_path, int perms = 0755);
extern DLLIMPORT bool CreateDir(const wxString& full_path, int perms = 0755);
extern DLLIMPORT wxString ChooseDirectory(wxWindow* parent,
                                          const wxString& message = _("Select directory"),
                                          const wxString& initialPath = _T(""),
                                          const wxString& basePath = _T(""),
                                          bool askToMakeRelative = false, // relative to basePath
                                          bool showCreateDirButton = false); // where supported

extern DLLIMPORT bool NormalizePath(wxFileName& f,const wxString& base);

extern DLLIMPORT wxString URLEncode(const wxString &str);

/// Check if CommonControls version is at least 6 (XP and up)
extern DLLIMPORT bool UsesCommonControls6();

/** This function loads a bitmap from disk.
  * Always use this to load bitmaps because it takes care of various
  * issues with pre-XP windows (actually common controls < 6.00).
  */
extern DLLIMPORT wxBitmap cbLoadBitmap(const wxString& filename, int bitmapType = wxBITMAP_TYPE_PNG);

// compatibility function
inline wxBitmap LoadPNGWindows2000Hack(const wxString& filename){ return cbLoadBitmap(filename); }

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

enum cbPlaceDialogMode
{
    pdlDont = 0,
    pdlBest,
    pdlCentre,
    pdlHead,
    pdlConstrain,
    pdlClip
};

extern DLLIMPORT void PlaceWindow(wxTopLevelWindow *w, cbPlaceDialogMode mode = pdlBest, bool enforce = false);

/** wxMessageBox wrapper.
  *
  * Use this instead of wxMessageBox(), as this uses PlaceWindow() to show it in the correct monitor.
  * @note The return value for wxOK would be wxID_OK and so on for other buttons.
  * So, read wxMessageDialog 's documentation, *not* wxMessageBox 's...
  */
inline int cbMessageBox(const wxString& message, const wxString& caption = wxEmptyString, int style = wxOK, wxWindow *parent = NULL, int x = -1, int y = -1)
{
    // Cannot create a wxMessageDialog with a NULL as parent
    if (!parent)
    {
      // wxMessage*Box* returns any of: wxYES, wxNO, wxCANCEL, wxOK.
      int answer = wxMessageBox(message, caption, style, parent, x, y);
      switch (answer)
      {
        // map answer to the one of wxMessage*Dialog* to ensure compatibility
        case (wxOK):
          return wxID_OK;
        case (wxCANCEL):
          return wxID_CANCEL;
        case (wxYES):
          return wxID_YES;
        case (wxNO):
          return wxID_NO;
        default:
          return -1; // NOTE: Cannot happen unless wxWidgets API changes
      }
    }

    wxMessageDialog dlg(parent, message, caption, style, wxPoint(x,y));
    PlaceWindow(&dlg);
    // wxMessage*Dialog* returns any of wxID_OK, wxID_CANCEL, wxID_YES, wxID_NO
    return dlg.ShowModal();
};

inline void NotifyMissingFile(const wxString &name)
{
    wxString msg;
    msg.Printf(_T("The file %s could not be found.\nPlease check your installation."), name.c_str());
    cbMessageBox(msg);
};

/// Result values of cbDirAccessCheck()
enum DirAccessCheck
{
    dacInvalidDir, ///< Invalid directory (does not exist).
    dacReadWrite, ///< Current user has read-write access to the directory.
    dacReadOnly ///< Current user has read-only access to the directory.
};

/** Check what access the current user has in a directory.
  *
  * @param dir The directory in question.
  *            It may or may not contain an ending path separator.
  * @return The type of access the current user has in the directory.
  */
extern DLLIMPORT DirAccessCheck cbDirAccessCheck(const wxString& dir);

namespace platform
{
    typedef enum
    {
        winver_NotWindows = 0,
        winver_UnknownWindows,
        winver_Windows9598ME,
        winver_WindowsNT2000,
        winver_WindowsXP,
        winver_Vista, // untested!
    }windows_version_t;

    extern DLLIMPORT windows_version_t WindowsVersion();
};

#endif // SDK_GLOBALS_H
