#ifndef MAIN_H
#define MAIN_H

#include <wx/toolbar.h>
#include <wx/laywin.h>
#include <wx/stc/stc.h>
#include "../sdk/manager.h"
#include "../sdk/cbplugin.h"
#include "../sdk/sdk_events.h"

WX_DECLARE_HASH_MAP(int, wxString, wxIntegerHash, wxIntegerEqual, WindowIDsMap);
WX_DECLARE_HASH_MAP(int, wxString, wxIntegerHash, wxIntegerEqual, PluginIDsMap);
WX_DECLARE_HASH_MAP(cbPlugin*, wxToolBar*, wxPointerHash, wxPointerEqual, PluginToolbarsMap);

class MainFrame : public wxMDIParentFrame
{
    public:
        MainFrame(wxWindow* parent = (wxWindow*)NULL);
        ~MainFrame();

        bool Open(const wxString& filename, bool addToHistory = true);
        void ShowTips(bool forceShow = false);
        
        // event handlers
        void OnApplicationClose(wxCloseEvent& event);

        void OnFileNewEmpty(wxCommandEvent& event);
        void OnFileOpen(wxCommandEvent& event);
        void OnFileReopen(wxCommandEvent& event);
        void OnFileOpenRecentClearHistory(wxCommandEvent& event);
        void OnFileSave(wxCommandEvent& event);
        void OnFileSaveAs(wxCommandEvent& event);
        void OnFileSaveAllFiles(wxCommandEvent& event);
        void OnFileSaveWorkspaceAs(wxCommandEvent& event);
        void OnFileClose(wxCommandEvent& event);
        void OnFileCloseAll(wxCommandEvent& event);
        void OnFileQuit(wxCommandEvent& event);

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
		
		void OnEditBookmarksToggle(wxCommandEvent& event);
		void OnEditBookmarksNext(wxCommandEvent& event);
		void OnEditBookmarksPrevious(wxCommandEvent& event);

        void OnSearchFind(wxCommandEvent& event);
        void OnSearchFindNext(wxCommandEvent& event);
        void OnSearchReplace(wxCommandEvent& event);
		void OnSearchGotoLine(wxCommandEvent& event);

        void OnProjectNew(wxCommandEvent& event);
        void OnProjectNewEmpty(wxCommandEvent& event);
		// project/open is handled in OnFileOpen
        void OnProjectSaveProject(wxCommandEvent& event);
        void OnProjectSaveProjectAs(wxCommandEvent& event);
        void OnProjectSaveAllProjects(wxCommandEvent& event);
        void OnProjectCloseProject(wxCommandEvent& event);
        void OnProjectCloseAllProjects(wxCommandEvent& event);
		void OnProjectImportDevCpp(wxCommandEvent& event);
		void OnProjectImportMSVC(wxCommandEvent& event);
		void OnProjectImportMSVS(wxCommandEvent& event);

        void OnToolsExecuteMenu(wxCommandEvent& event);

		void OnSettingsEnvironment(wxCommandEvent& event);
        void OnSettingsEditor(wxCommandEvent& event);
        void OnSettingsPlugins(wxCommandEvent& event);
        void OnPluginSettingsMenu(wxCommandEvent& event);

        void OnHelpAbout(wxCommandEvent& event);
        void OnHelpTips(wxCommandEvent& event);
        void OnHelpPluginMenu(wxCommandEvent& event);

        void OnSize(wxSizeEvent& event);
        void OnToggleBar(wxCommandEvent& event);
        void OnToggleStatusBar(wxCommandEvent& event);
        void OnFocusEditor(wxCommandEvent& event);
        void OnToggleFullScreen(wxCommandEvent& event);
        
        // plugin events
        void OnPluginLoaded(CodeBlocksEvent& event);
        void OnPluginUnloaded(CodeBlocksEvent& event);
		
		// general UpdateUI events
        void OnEditorUpdateUI(CodeBlocksEvent& event);
		void OnFileMenuUpdateUI(wxUpdateUIEvent& event);
		void OnEditMenuUpdateUI(wxUpdateUIEvent& event);
		void OnViewMenuUpdateUI(wxUpdateUIEvent& event);
		void OnSearchMenuUpdateUI(wxUpdateUIEvent& event);
		void OnProjectMenuUpdateUI(wxUpdateUIEvent& event);
		
		// sash events
		void OnDragSash(wxSashEvent& event);
		
		// project events
		void OnProjectActivated(CodeBlocksEvent& event);
    protected:
        void CreateIDE();
		void CreateMenubar();
        void CreateToolbars();
        void ScanForPlugins();
		void AddToolbarItem(int id, const wxString& title, const wxString& shortHelp, const wxString& longHelp, const wxString& image);

        void DoAddPlugin(cbPlugin* plugin);
        void AddPluginInToolsMenu(cbPlugin* plugin);
        void AddPluginInSettingsMenu(cbPlugin* plugin);
        void AddPluginInHelpPluginsMenu(cbPlugin* plugin);
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
		void DoUpdateLayout();
		
        void LoadWindowState();
        void SaveWindowState();

        void InitializeRecentFilesHistory();
        void TerminateRecentFilesHistory();
		
        wxFileHistory m_FilesHistory;

        /// "Close FullScreen" button. Only shown when in FullScreen view
        wxButton* m_pCloseFullScreenBtn;
        
        wxNotebook* m_pNotebook;
		wxSashLayoutWindow* m_pLeftSash;
		wxSashLayoutWindow* m_pBottomSash;
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
		
        DECLARE_EVENT_TABLE()
};

#endif // MAIN_H

