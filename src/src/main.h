#ifndef MAIN_H
#define MAIN_H

#include <wx/toolbar.h>
#include <wx/docview.h> // for wxFileHistory
#include <wx/notebook.h>
#include <../sdk/cbeditor.h>
#include "../sdk/manager.h"
#include "../sdk/cbplugin.h"
#include "../sdk/sdk_events.h"

#include "wx/layoutmanager.h"
#include "wx/dockwindow.h"
#include "wx/dockhost.h"
#include "wx/pane.h"
#include "wx/slidebar.h"

WX_DECLARE_HASH_MAP(int, wxString, wxIntegerHash, wxIntegerEqual, WindowIDsMap);
WX_DECLARE_HASH_MAP(int, wxString, wxIntegerHash, wxIntegerEqual, PluginIDsMap);
WX_DECLARE_HASH_MAP(cbPlugin*, wxToolBar*, wxPointerHash, wxPointerEqual, PluginToolbarsMap);

class MainFrame : public wxFrame
{
        wxLayoutManager* pLayoutManager;
        wxSlideBar* pSlideBar;
        wxPane * pPane;
        wxDockWindow * pDockWindow1;
        wxDockWindow * pDockWindow2;
    public:
        wxAcceleratorTable* m_pAccel;
        MainFrame(wxLocale& locale, wxWindow* parent = (wxWindow*)NULL);
        ~MainFrame();

        wxLocale& m_locale;

        bool Open(const wxString& filename, bool addToHistory = true);
        bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);
        void ShowTips(bool forceShow = false);

        // show a file-open dialog and return the selection
        wxString ShowOpenFileDialog(const wxString& caption, const wxString& filter);
        // open the filename (based on what it is)
        bool OpenGeneric(const wxString& filename, bool addToHistory = true);

        // event handlers
        void OnApplicationClose(wxCloseEvent& event);
        void OnStartHereLink(wxCommandEvent& event);
        void OnStartHereVarSubst(wxCommandEvent& event);

        void OnFileNewEmpty(wxCommandEvent& event);
        void OnFileOpen(wxCommandEvent& event);
        void OnFileReopen(wxCommandEvent& event);
        void OnFileOpenRecentClearHistory(wxCommandEvent& event);
        void OnFileSave(wxCommandEvent& event);
        void OnFileSaveAs(wxCommandEvent& event);
        void OnFileSaveAllFiles(wxCommandEvent& event);
        void OnFileSaveWorkspace(wxCommandEvent& event);
        void OnFileSaveWorkspaceAs(wxCommandEvent& event);
        void OnFileClose(wxCommandEvent& event);
        void OnFileCloseAll(wxCommandEvent& event);
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
        void OnEditFoldAll(wxCommandEvent& event);
        void OnEditUnfoldAll(wxCommandEvent& event);
        void OnEditToggleAllFolds(wxCommandEvent& event);
        void OnEditFoldBlock(wxCommandEvent& event);
        void OnEditUnfoldBlock(wxCommandEvent& event);
        void OnEditToggleFoldBlock(wxCommandEvent& event);
        void OnEditEOLMode(wxCommandEvent& event);
        void OnEditSelectAll(wxCommandEvent& event);
        void OnEditCommentSelected(wxCommandEvent& event);
        void OnEditUncommentSelected(wxCommandEvent& event);
        void OnEditToggleCommentSelected(wxCommandEvent & event);
        void OnEditAutoComplete(wxCommandEvent& event);

		void OnEditBookmarksToggle(wxCommandEvent& event);
		void OnEditBookmarksNext(wxCommandEvent& event);
		void OnEditBookmarksPrevious(wxCommandEvent& event);

        void OnSearchFind(wxCommandEvent& event);
        void OnSearchFindNext(wxCommandEvent& event);
        void OnSearchReplace(wxCommandEvent& event);
		void OnSearchGotoLine(wxCommandEvent& event);

        void OnProjectNew(wxCommandEvent& event);
        void OnProjectNewEmpty(wxCommandEvent& event);
        void OnProjectOpen(wxCommandEvent& event);
        void OnProjectSaveProject(wxCommandEvent& event);
        void OnProjectSaveProjectAs(wxCommandEvent& event);
        void OnProjectSaveAllProjects(wxCommandEvent& event);
        void OnProjectSaveTemplate(wxCommandEvent& event);
        void OnProjectCloseProject(wxCommandEvent& event);
        void OnProjectCloseAllProjects(wxCommandEvent& event);
		void OnProjectImportDevCpp(wxCommandEvent& event);
		void OnProjectImportMSVC(wxCommandEvent& event);
		void OnProjectImportMSVCWksp(wxCommandEvent& event);
		void OnProjectImportMSVS(wxCommandEvent& event);
		void OnProjectImportMSVSWksp(wxCommandEvent& event);

        void OnPluginsExecuteMenu(wxCommandEvent& event);

		void OnSettingsEnvironment(wxCommandEvent& event);
        void OnSettingsEditor(wxCommandEvent& event);
        void OnSettingsPlugins(wxCommandEvent& event);
        void OnPluginSettingsMenu(wxCommandEvent& event);
        void OnSettingsImpExpConfig(wxCommandEvent& event);

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

		// general UpdateUI events
        void OnEditorUpdateUI(CodeBlocksEvent& event);
        void OnEditorUpdateUI_NB(wxNotebookEvent& event);       //tiwag 050917

		void OnFileMenuUpdateUI(wxUpdateUIEvent& event);
		void OnEditMenuUpdateUI(wxUpdateUIEvent& event);
		void OnViewMenuUpdateUI(wxUpdateUIEvent& event);
		void OnSearchMenuUpdateUI(wxUpdateUIEvent& event);
		void OnProjectMenuUpdateUI(wxUpdateUIEvent& event);

		void OnLayoutChanged(wxEvent& event);

		// project events
		void OnProjectActivated(CodeBlocksEvent& event);
		void OnProjectOpened(CodeBlocksEvent& event);
		void OnProjectClosed(CodeBlocksEvent& event);

		// dock/undock window requests
		void OnRequestDockWindow(CodeBlocksEvent& event);
		void OnRequestUndockWindow(CodeBlocksEvent& event);
		void OnRequestShowDockWindow(CodeBlocksEvent& event);

		// editor changed events
		void OnEditorOpened(CodeBlocksEvent& event);
		void OnEditorClosed(CodeBlocksEvent& event);
		void OnEditorSaved(CodeBlocksEvent& event);
		void OnPageChanged(wxNotebookEvent& event);
        void OnShiftTab(wxCommandEvent& event);
    protected:
        void CreateIDE();
		void CreateMenubar();
        void CreateToolbars();
        void ScanForPlugins();
		void AddToolbarItem(int id, const wxString& title, const wxString& shortHelp, const wxString& longHelp, const wxString& image);
        wxMenu* RecreateMenu(wxMenuBar* mbar, const wxString& name);

        void DoAddPlugin(cbPlugin* plugin);
        void DoAddPluginToolbar(cbPlugin* plugin);
        void AddPluginInPluginsMenu(cbPlugin* plugin);
        void AddPluginInSettingsMenu(cbPlugin* plugin);
        void AddPluginInHelpPluginsMenu(cbPlugin* plugin);
        void AddPluginInMenus(wxMenu* menu, cbPlugin* plugin, wxObjectEventFunction callback, int pos = -1);
        void RemovePluginFromMenus(const wxString& pluginName);

		void AddEditorInWindowMenu(const wxString& filename, const wxString& title);
		void RemoveEditorFromWindowMenu(const wxString& filename);
		int IsEditorInWindowMenu(const wxString& filename);

        bool DoCloseCurrentWorkspace();
        bool DoOpenProject(const wxString& filename, bool addToHistory = true);
        bool DoOpenFile(const wxString& filename, bool addToHistory = true);

        void DoCreateStatusBar();
        void DoUpdateStatusBar();
		void DoUpdateAppTitle();

        void ShowHideStartPage(bool forceHasProject = false);

        void LoadWindowState();
        void SaveWindowState();

        void InitializeRecentFilesHistory();
        void TerminateRecentFilesHistory();

        wxFileHistory m_FilesHistory;

        /// "Close FullScreen" button. Only shown when in FullScreen view
        wxButton* m_pCloseFullScreenBtn;

        wxNotebook* m_pNotebook;
		EditorManager* m_pEdMan;
		ProjectManager* m_pPrjMan;
		MessageManager* m_pMsgMan;

        wxToolBar* m_pToolbar;
        PluginToolbarsMap m_PluginsTools;

		WindowIDsMap m_WindowIDsMap;
        PluginIDsMap m_PluginIDsMap;
        wxMenu* m_ToolsMenu;
		wxMenu* m_PluginsMenu;
        wxMenu* m_SettingsMenu;
        wxMenu* m_HelpPluginsMenu;

        bool m_ReconfiguringPlugins;
        bool m_SmallToolBar;

        DECLARE_EVENT_TABLE()
};

#endif // MAIN_H

