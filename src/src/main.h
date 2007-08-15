#ifndef MAIN_H
#define MAIN_H

#include <map>

#include <wx/toolbar.h>
#include <wx/docview.h> // for wxFileHistory
#include <wx/notebook.h>
#include <wx/dynarray.h>
#include <cbeditor.h>
#include "manager.h"
#include "cbexception.h"
#include "cbplugin.h"
#include "sdk_events.h"
#include "scripting/bindings/sc_base_types.h"

// wxAUI
#include "aui_compat.h"

WX_DECLARE_HASH_MAP(int, wxString, wxIntegerHash, wxIntegerEqual, PluginIDsMap);
WX_DECLARE_HASH_MAP(cbPlugin*, wxToolBar*, wxPointerHash, wxPointerEqual, PluginToolbarsMap);
WX_DECLARE_STRING_HASH_MAP(wxString, LayoutViewsMap);

extern int idStartHerePageLink;
extern int idStartHerePageVarSubst;

class wxFlatNotebook;

class MainFrame : public wxFrame
{
    private:
        wxAuiManager m_LayoutManager;
        LayoutViewsMap m_LayoutViews;
    public:
        wxAcceleratorTable* m_pAccel;
        MainFrame(wxWindow* parent = (wxWindow*)NULL);
        ~MainFrame();

        // needed for binding with SqPlus
        MainFrame(const MainFrame& rhs){ cbThrow(_T("Can't use MainFrame's copy constructor")); }
        MainFrame& operator=(const MainFrame& rhs){ cbThrow(_T("Can't use MainFrame's operator=")); }

        bool Open(const wxString& filename, bool addToHistory = true);
        bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);
        void ShowTips(bool forceShow = false);

        // show a file-open dialog and return the selection
        wxString ShowOpenFileDialog(const wxString& caption, const wxString& filter);
        // open the filename (based on what it is)
        bool OpenGeneric(const wxString& filename, bool addToHistory = true);

        // event handlers

        void OnEraseBackground(wxEraseEvent& event);
        void OnSize(wxSizeEvent& event);
        void OnApplicationClose(wxCloseEvent& event);
        void OnStartHereLink(wxCommandEvent& event);
        void OnStartHereVarSubst(wxCommandEvent& event);

        // File->New submenu entries handler
        void OnFileNewWhat(wxCommandEvent& event);

        void OnFileNew(wxCommandEvent& event);
        void OnFileOpen(wxCommandEvent& event);
        void OnFileReopenProject(wxCommandEvent& event);
        void OnFileOpenRecentProjectClearHistory(wxCommandEvent& event);
        void OnFileReopen(wxCommandEvent& event);
        void OnFileOpenRecentClearHistory(wxCommandEvent& event);
		void OnFileImportProjectDevCpp(wxCommandEvent& event);
		void OnFileImportProjectMSVC(wxCommandEvent& event);
		void OnFileImportProjectMSVCWksp(wxCommandEvent& event);
		void OnFileImportProjectMSVS(wxCommandEvent& event);
		void OnFileImportProjectMSVSWksp(wxCommandEvent& event);
        void OnFileSave(wxCommandEvent& event);
        void OnFileSaveAs(wxCommandEvent& event);
        void OnFileSaveAllFiles(wxCommandEvent& event);
        void OnFileSaveProject(wxCommandEvent& event);
        void OnFileSaveProjectAs(wxCommandEvent& event);
        void OnFileSaveProjectTemplate(wxCommandEvent& event);
        void OnFileSaveProjectAllProjects(wxCommandEvent& event);
        void OnFileOpenDefWorkspace(wxCommandEvent& event);
        void OnFileSaveWorkspace(wxCommandEvent& event);
        void OnFileSaveWorkspaceAs(wxCommandEvent& event);
        void OnFileSaveAll(wxCommandEvent& event);
        void OnFileCloseWorkspace(wxCommandEvent& event);
        void OnFileClose(wxCommandEvent& event);
        void OnFileCloseAll(wxCommandEvent& event);
        void OnFileCloseProject(wxCommandEvent& event);
        void OnFileCloseAllProjects(wxCommandEvent& event);
        void OnFilePrintSetup(wxCommandEvent& event);
        void OnFilePrint(wxCommandEvent& event);
        void OnFileQuit(wxCommandEvent& event);
        void OnFileNext(wxCommandEvent& event);
        void OnFilePrev(wxCommandEvent& event);

        void OnEditUndo(wxCommandEvent& event);
        void OnEditRedo(wxCommandEvent& event);
        void OnEditCopy(wxCommandEvent& event);
        void OnEditCut(wxCommandEvent& event);
        void OnEditPaste(wxCommandEvent& event);
        void OnEditSwapHeaderSource(wxCommandEvent& event);
        void OnEditGotoMatchingBrace(wxCommandEvent& event);
        void OnEditHighlightMode(wxCommandEvent& event);
        void OnEditFoldAll(wxCommandEvent& event);
        void OnEditUnfoldAll(wxCommandEvent& event);
        void OnEditToggleAllFolds(wxCommandEvent& event);
        void OnEditFoldBlock(wxCommandEvent& event);
        void OnEditUnfoldBlock(wxCommandEvent& event);
        void OnEditToggleFoldBlock(wxCommandEvent& event);
        void OnEditEOLMode(wxCommandEvent& event);
        void OnEditEncoding(wxCommandEvent& event);
        void OnEditParaUp(wxCommandEvent& event);
        void OnEditParaUpExtend(wxCommandEvent& event);
        void OnEditParaDown(wxCommandEvent& event);
        void OnEditParaDownExtend(wxCommandEvent& event);
        void OnEditWordPartLeft(wxCommandEvent& event);
        void OnEditWordPartLeftExtend(wxCommandEvent& event);
        void OnEditWordPartRight(wxCommandEvent& event);
        void OnEditWordPartRightExtend(wxCommandEvent& event);
        void OnEditZoomIn(wxCommandEvent& event);
        void OnEditZoomOut(wxCommandEvent& event);
        void OnEditZoomReset(wxCommandEvent& event);
        void OnEditLineCut(wxCommandEvent& event);
        void OnEditLineDelete(wxCommandEvent& event);
        void OnEditLineDuplicate(wxCommandEvent& event);
        void OnEditLineTranspose(wxCommandEvent& event);
        void OnEditLineCopy(wxCommandEvent& event);
        void OnEditLinePaste(wxCommandEvent& event);
        void OnEditUpperCase(wxCommandEvent& event);
        void OnEditLowerCase(wxCommandEvent& event);
        void OnEditSelectAll(wxCommandEvent& event);
        void OnEditCommentSelected(wxCommandEvent& event);
        void OnEditUncommentSelected(wxCommandEvent& event);
        void OnEditToggleCommentSelected(wxCommandEvent & event);
        void OnEditAutoComplete(wxCommandEvent& event);

		void OnEditBookmarksToggle(wxCommandEvent& event);
		void OnEditBookmarksNext(wxCommandEvent& event);
		void OnEditBookmarksPrevious(wxCommandEvent& event);

        void OnViewLayout(wxCommandEvent& event);
        void OnViewLayoutSave(wxCommandEvent& event);
        void OnViewLayoutDelete(wxCommandEvent& event);
        void OnViewScriptConsole(wxCommandEvent& event);

        void OnSearchFind(wxCommandEvent& event);
        void OnSearchFindNext(wxCommandEvent& event);
        void OnSearchReplace(wxCommandEvent& event);
		void OnSearchGotoLine(wxCommandEvent& event);

        void OnPluginsExecuteMenu(wxCommandEvent& event);

		void OnSettingsEnvironment(wxCommandEvent& event);
        void OnSettingsKeyBindings(wxCommandEvent& event);
		void OnGlobalUserVars(wxCommandEvent& event);
        void OnSettingsEditor(wxCommandEvent& event);
        void OnSettingsCompilerDebugger(wxCommandEvent& event);
        void OnSettingsPlugins(wxCommandEvent& event);
        void OnPluginSettingsMenu(wxCommandEvent& event);
        void OnSettingsScripting(wxCommandEvent& event);

        void OnHelpAbout(wxCommandEvent& event);
        void OnHelpTips(wxCommandEvent& event);
        void OnHelpPluginMenu(wxCommandEvent& event);

        void OnToggleBar(wxCommandEvent& event);
        void OnToggleOpenFilesTree(wxCommandEvent& event);
        void OnToggleStatusBar(wxCommandEvent& event);
        void OnFocusEditor(wxCommandEvent& event);
        void OnToggleFullScreen(wxCommandEvent& event);

        // plugin events
        void OnPluginLoaded(CodeBlocksEvent& event);
        void OnPluginUnloaded(CodeBlocksEvent& event);
        void OnPluginInstalled(CodeBlocksEvent& event);
        void OnPluginUninstalled(CodeBlocksEvent& event);

		// general UpdateUI events
        void OnEditorUpdateUI(CodeBlocksEvent& event);

		void OnFileMenuUpdateUI(wxUpdateUIEvent& event);
		void OnEditMenuUpdateUI(wxUpdateUIEvent& event);
		void OnViewMenuUpdateUI(wxUpdateUIEvent& event);
		void OnSearchMenuUpdateUI(wxUpdateUIEvent& event);
		void OnProjectMenuUpdateUI(wxUpdateUIEvent& event);

		// project events
		void OnProjectActivated(CodeBlocksEvent& event);
		void OnProjectOpened(CodeBlocksEvent& event);
		void OnProjectClosed(CodeBlocksEvent& event);

		// dock/undock window requests
		void OnRequestDockWindow(CodeBlocksDockEvent& event);
		void OnRequestUndockWindow(CodeBlocksDockEvent& event);
		void OnRequestShowDockWindow(CodeBlocksDockEvent& event);
		void OnRequestHideDockWindow(CodeBlocksDockEvent& event);
		void OnDockWindowVisibility(CodeBlocksDockEvent& event);

        // layout requests
        void OnLayoutSwitch(CodeBlocksLayoutEvent& event);

		// editor changed events
		void OnEditorOpened(CodeBlocksEvent& event);
		void OnEditorActivated(CodeBlocksEvent& event);
		void OnEditorClosed(CodeBlocksEvent& event);
		void OnEditorSaved(CodeBlocksEvent& event);
		void OnEditorModified(CodeBlocksEvent& event);
		void OnPageChanged(wxNotebookEvent& event);
        void OnShiftTab(wxCommandEvent& event);
    protected:
        void CreateIDE();
		void CreateMenubar();
        void CreateToolbars();
        void ScanForPlugins();
		void AddToolbarItem(int id, const wxString& title, const wxString& shortHelp, const wxString& longHelp, const wxString& image);
        void RecreateMenuBar();
        void RegisterEvents();
        void SetupGUILogging();

		void RegisterScriptFunctions();
		void RunStartupScripts();

        enum { Installed, Uninstalled, Unloaded };
        void PluginsUpdated(cbPlugin* plugin, int status);

        void DoAddPlugin(cbPlugin* plugin);
        void DoAddPluginToolbar(cbPlugin* plugin);
        void AddPluginInPluginsMenu(cbPlugin* plugin);
        void AddPluginInSettingsMenu(cbPlugin* plugin);
        void AddPluginInHelpPluginsMenu(cbPlugin* plugin);
        wxMenuItem* AddPluginInMenus(wxMenu* menu, cbPlugin* plugin, wxObjectEventFunction callback, int pos = -1, bool checkable = false);
        void RemovePluginFromMenus(const wxString& pluginName);

        void LoadViewLayout(const wxString& name);
        void SaveViewLayout(const wxString& name, const wxString& layout, bool select = false);
        void DoSelectLayout(const wxString& name);
        void DoFixToolbarsLayout();
        bool DoCheckCurrentLayoutForChanges(bool canCancel = true);

        void AskToRemoveFileFromHistory(wxFileHistory* hist, int id);

		void AddEditorInWindowMenu(const wxString& filename, const wxString& title);
		void RemoveEditorFromWindowMenu(const wxString& filename);
		int IsEditorInWindowMenu(const wxString& filename);

        bool DoCloseCurrentWorkspace();
        bool DoOpenProject(const wxString& filename, bool addToHistory = true);
        bool DoOpenFile(const wxString& filename, bool addToHistory = true);
        void DoOnFileOpen(bool bProject = false);

        void DoCreateStatusBar();
        void DoUpdateStatusBar();
		void DoUpdateAppTitle();
		void DoUpdateLayout();
        void DoUpdateLayoutColours();
		void DoUpdateEditorStyle();
		void DoUpdateEditorStyle(wxFlatNotebook* target, const wxString& prefix, long defaultStyle);

        void ShowHideStartPage(bool forceHasProject = false);
        void ShowHideScriptConsole();

        void LoadWindowState();
        void SaveWindowState();

        void InitializeRecentFilesHistory();
        void AddToRecentFilesHistory(const wxString& filename);
        void AddToRecentProjectsHistory(const wxString& filename);
        void TerminateRecentFilesHistory();

        wxFileHistory* m_pFilesHistory;
        wxFileHistory* m_pProjectsHistory;

        /// "Close FullScreen" button. Only shown when in FullScreen view
        wxButton* m_pCloseFullScreenBtn;

		EditorManager* m_pEdMan;
		ProjectManager* m_pPrjMan;
		MessageManager* m_pMsgMan;

        wxToolBar* m_pToolbar;
        PluginToolbarsMap m_PluginsTools;

        PluginIDsMap m_PluginIDsMap;
        wxMenu* m_ToolsMenu;
		wxMenu* m_PluginsMenu;
        wxMenu* m_HelpPluginsMenu;

        bool m_SmallToolBar;
        bool m_StartupDone;
        bool m_InitiatedShutdown;

        wxString m_LastLayoutName;
        wxString m_LastLayoutData;

        int m_ScriptConsoleID;
        bool m_ScriptConsoleVisible;

        typedef std::map<int, const wxString> MenuIDToScript; // script menuitem ID -> script function name
		MenuIDToScript m_MenuIDToScript;

        DECLARE_EVENT_TABLE()
};

#endif // MAIN_H

