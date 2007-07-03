#ifndef EDITORMANAGER_H
#define EDITORMANAGER_H

#include <wx/list.h>
#include <wx/treectrl.h>
#include <wx/hashmap.h>

#include "settings.h"
#include "manager.h"
#include "cbexception.h" // cbThrow

#include "openfilestree.h"
#include "printing_types.h"

#ifndef CB_PRECOMP
    #include "globals.h" // cbC2U
#endif

DLLIMPORT extern int ID_NBEditorManager;
DLLIMPORT extern int ID_EditorManager;
DLLIMPORT extern int idEditorManagerCheckFiles;
DLLIMPORT extern int ID_EditorManagerCloseButton;

// forward decls
class EditorBase;
class wxFlatNotebook;
class wxFlatNotebookEvent;
class wxMenuBar;
class EditorColourSet;
class cbProject;
class ProjectFile;
class cbEditor;
class cbStyledTextCtrl;
class SimpleListLog;
struct EditorManagerInternalData;

WX_DECLARE_STRING_HASH_MAP(wxString, AutoCompleteMap);

// forward decl
struct cbFindReplaceData;

/*
 * No description
 */
class DLLIMPORT EditorManager : public Mgr<EditorManager>, public wxEvtHandler
{
        friend class Mgr<EditorManager>;
        static bool s_CanShutdown;
    public:
        friend class Manager; // give Manager access to our private members
        static bool CanShutdown(){ return s_CanShutdown; }

        EditorManager(const EditorManager& rhs) { cbThrow(_T("Can't call EditorManager's copy ctor!!!")); }
        virtual void operator=(const EditorManager& rhs){ cbThrow(_T("Can't assign an EditorManager* !!!")); }

        wxFlatNotebook* GetNotebook(){ return m_pNotebook; }
        void CreateMenu(wxMenuBar* menuBar);
        void ReleaseMenu(wxMenuBar* menuBar);
        void Configure();
        int GetEditorsCount();
        AutoCompleteMap& GetAutoCompleteMap(){ return m_AutoCompleteMap; }

        EditorBase* IsOpen(const wxString& filename);
        cbEditor* Open(const wxString& filename, int pos = 0,ProjectFile* data = 0);
        EditorBase* GetEditor(int index);
        EditorBase* GetEditor(const wxString& filename){ return IsOpen(filename); } // synonym of IsOpen()
        EditorBase* GetActiveEditor();
        cbEditor* GetBuiltinEditor(EditorBase* eb);
        int FindPageFromEditor(EditorBase* eb);

        // "overloaded" functions for easier access
        // they all return a cbEditor pointer if the editor is builtin, or NULL
        cbEditor* IsBuiltinOpen(const wxString& filename){ return GetBuiltinEditor(IsOpen(filename)); }
        cbEditor* GetBuiltinEditor(int index){ return GetBuiltinEditor(GetEditor(index)); }
        cbEditor* GetBuiltinEditor(const wxString& filename){ return IsBuiltinOpen(filename); } // synonym of IsBuiltinOpen()
        cbEditor* GetBuiltinActiveEditor(){ return GetBuiltinEditor(GetActiveEditor()); }

        void ActivateNext();
        void ActivatePrevious();
        void SetActiveEditor(EditorBase* ed);
        EditorColourSet* GetColourSet(){ return (this==NULL) ? 0 : m_Theme; }
        void SetColourSet(EditorColourSet* theme);
        cbEditor* New(const wxString& newFileName = wxEmptyString);

        // these are used *only* for custom editors
        void AddCustomEditor(EditorBase* eb);
        void RemoveCustomEditor(EditorBase* eb);

        bool UpdateProjectFiles(cbProject* project);
        bool SwapActiveHeaderSource();
        bool CloseActive(bool dontsave = false);
        bool Close(const wxString& filename,bool dontsave = false);
        bool Close(EditorBase* editor,bool dontsave = false);
        bool Close(int index,bool dontsave = false);

        // If file is modified, queries to save (yes/no/cancel).
        // Returns false on "cancel".
        bool QueryClose(EditorBase* editor);
        bool QueryCloseAll();
        bool CloseAll(bool dontsave=false);
        bool CloseAllExcept(EditorBase* editor,bool dontsave=false);
        bool Save(const wxString& filename);
        bool Save(int index);
        bool SaveActive();
        bool SaveAs(int index);
        bool SaveActiveAs();
        bool SaveAll();
        int ShowFindDialog(bool replace,  bool explicitly_find_in_files = false);
        int FindNext(bool goingDown, cbStyledTextCtrl* control = 0, cbFindReplaceData* data = 0);

        void Print(PrintScope ps, PrintColourMode pcm, bool line_numbers);

        /** Hides the editor notebook for layout purposes */
        void HideNotebook();
        /** Shows the previously hidden editor notebook */
        void ShowNotebook();
        /** Check if one of the open files has been modified outside the IDE. If so, ask to reload it. */
        void CheckForExternallyModifiedFiles();

        /// Open Files Tree functions
        /// Is the opened files tree supported? (depends on platform)
        bool OpenFilesTreeSupported();
        /// Show/hide the opened files tree
        void ShowOpenFilesTree(bool show);
        /// Refresh the open files tree
        void RefreshOpenFilesTree();
        /// Return true if opened files tree is visible, false if not
        bool IsOpenFilesTreeVisible() const;

        /** Builds Opened Files tree in the Projects tab
          */
        wxTreeCtrl* GetTree();
        wxTreeItemId FindTreeFile(const wxString& filename);
        wxString GetTreeItemFilename(wxTreeItemId item);
        void BuildOpenedFilesTree(wxWindow* parent);
        void RebuildOpenedFilesTree(wxTreeCtrl *tree = 0L);
        void RefreshOpenedFilesTree(bool force = false);

        void OnGenericContextMenuHandler(wxCommandEvent& event);
        void OnPageChanged(wxFlatNotebookEvent& event);
        void OnPageChanging(wxFlatNotebookEvent& event);
        void OnPageClosing(wxFlatNotebookEvent& event);
        void OnPageContextMenu(wxFlatNotebookEvent& event);
        void OnClose(wxCommandEvent& event);
        void OnCloseAll(wxCommandEvent& event);
        void OnCloseAllOthers(wxCommandEvent& event);
        void OnSave(wxCommandEvent& event);
        void OnSaveAll(wxCommandEvent& event);
        void OnSwapHeaderSource(wxCommandEvent& event);
        void OnTabPosition(wxCommandEvent& event);
        void OnProperties(wxCommandEvent& event);
        void OnAppDoneStartup(wxCommandEvent& event);
        void OnAppStartShutdown(wxCommandEvent& event);
        void OnUpdateUI(wxUpdateUIEvent& event);
        void OnTreeItemSelected(wxTreeEvent &event);
        void OnTreeItemActivated(wxTreeEvent &event);
        void OnTreeItemRightClick(wxTreeEvent &event);
        void SetZoom(int zoom);
        int GetZoom()const;
        bool RenameTreeFile(const wxString& oldname, const wxString& newname);

    protected:
        // m_EditorsList access
        void AddEditorBase(EditorBase* eb);
        void RemoveEditorBase(EditorBase* eb, bool deleteObject = true);
        cbEditor* InternalGetBuiltinEditor(int page);
        EditorBase* InternalGetEditorBase(int page);

        void CreateSearchLog();
        void LogSearch(const wxString& file, int line, const wxString& lineText);

        void LoadAutoComplete();
        void SaveAutoComplete();

        void DeleteItemfromTree(wxTreeItemId item);
        void DeleteFilefromTree(const wxString& filename);
        void AddFiletoTree(EditorBase* ed);
        void InitPane();

        AutoCompleteMap m_AutoCompleteMap;
    private:
        EditorManager();
        ~EditorManager();
        void CalculateFindReplaceStartEnd(cbStyledTextCtrl* control, cbFindReplaceData* data, bool replace = false);
        void OnCheckForModifiedFiles(wxCommandEvent& event);
        int Find(cbStyledTextCtrl* control, cbFindReplaceData* data);
        int FindInFiles(cbFindReplaceData* data);
        int Replace(cbStyledTextCtrl* control, cbFindReplaceData* data);
        int ReplaceInFiles(cbFindReplaceData* data);
        int GetOpenFilesListIcon(EditorBase* ed);

        wxFlatNotebook* m_pNotebook;
        cbFindReplaceData* m_LastFindReplaceData;
        EditorColourSet* m_Theme;
        wxTreeCtrl* m_pTree;
        wxString m_LastActiveFile;
        bool m_LastModifiedflag;
        SimpleListLog* m_pSearchLog;
        int m_SearchLogIndex;
        int m_SashPosition;
        int m_zoom;
        bool m_isCheckingForExternallyModifiedFiles;
        friend struct EditorManagerInternalData;
        EditorManagerInternalData* m_pData;

        DECLARE_EVENT_TABLE()
};

#endif // EDITORMANAGER_H


