/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef EDITORMANAGER_H
#define EDITORMANAGER_H

#include "prep.h"
#include <wx/list.h>
#include <wx/treectrl.h>
#include <wx/hashmap.h>
#include <wx/filename.h>

#include "settings.h"
#include "manager.h"
#include "cbexception.h" // cbThrow

#include "printing_types.h"

#include "globals.h" // cbC2U, FileType

DLLIMPORT extern int ID_NBEditorManager;
DLLIMPORT extern int ID_EditorManager;
DLLIMPORT extern int idEditorManagerCheckFiles;
DLLIMPORT extern int ID_EditorManagerCloseButton;

// forward decls
class EditorBase;
class cbAuiNotebook;
class wxAuiNotebookEvent;
class EditorColourSet;
class cbProject;
class ProjectFile;
class cbEditor;
class cbStyledTextCtrl;
class ListCtrlLogger;
class LoaderBase;
struct EditorManagerInternalData;
class SearchResultsLog;

/*
 * Struct for store tabs stack info
 */
struct cbNotebookStack
{
    cbNotebookStack(wxWindow* a_pWindow = nullptr)
        : window (a_pWindow),
          next (nullptr)
   {}

    wxWindow*           window;
    cbNotebookStack*    next;
};

/*
 * No description
 */
class DLLIMPORT EditorManager : public Mgr<EditorManager>, public wxEvtHandler
{
        friend class Mgr<EditorManager>;
        static bool s_CanShutdown;
    public:
        EditorManager& operator=(cb_unused const EditorManager& rhs) // prevent assignment operator
        {
            cbThrow(_T("Can't assign an EditorManager* !!!"));
            return *this;
        }

        friend class Manager; // give Manager access to our private members
        static bool CanShutdown(){ return s_CanShutdown; }

        cbAuiNotebook* GetNotebook() { return m_pNotebook; }
        cbNotebookStack* GetNotebookStack();
        void DeleteNotebookStack();
        void RebuildNotebookStack();

        void RecreateOpenEditorStyles();
        int GetEditorsCount();

        EditorBase* IsOpen(const wxString& filename);
        cbEditor* Open(const wxString& filename, int pos = 0, ProjectFile* data = nullptr);
        cbEditor* Open(LoaderBase* fileLdr, const wxString& filename, int pos = 0, ProjectFile* data = nullptr);
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
        EditorColourSet* GetColourSet(){ return m_Theme; }
        void SetColourSet(EditorColourSet* theme);
        cbEditor* New(const wxString& newFileName = wxEmptyString);

        // these are used *only* for custom editors
        void AddCustomEditor(EditorBase* eb);
        void RemoveCustomEditor(EditorBase* eb);

        bool UpdateProjectFiles(cbProject* project);
        bool SwapActiveHeaderSource();
        bool OpenContainingFolder();
        bool CloseActive(bool dontsave = false);
        bool Close(const wxString& filename, bool dontsave = false);
        bool Close(EditorBase* editor, bool dontsave = false);
        bool Close(int index, bool dontsave = false);

        // If file is modified, queries to save (yes/no/cancel).
        // Returns false on "cancel".
        bool QueryClose(EditorBase* editor);
        bool QueryCloseAll();
        bool CloseAll(bool dontsave = false);
        bool CloseAllExcept(EditorBase* editor, bool dontsave = false);
        bool Save(const wxString& filename);
        bool Save(int index);
        bool SaveActive();
        bool SaveAs(int index);
        bool SaveActiveAs();
        bool SaveAll();

        void Print(PrintScope ps, PrintColourMode pcm, bool line_numbers);

        /** Hides the editor notebook for layout purposes */
        void HideNotebook();
        /** Shows the previously hidden editor notebook */
        void ShowNotebook();
        /** Check if one of the open files has been modified outside the IDE. If so, ask to reload it. */
        void CheckForExternallyModifiedFiles();

        void OnGenericContextMenuHandler(wxCommandEvent& event);
        void OnPageChanged(wxAuiNotebookEvent& event);
        void OnPageChanging(wxAuiNotebookEvent& event);
        void OnPageClose(wxAuiNotebookEvent& event);
        void OnPageContextMenu(wxAuiNotebookEvent& event);
        void OnClose(wxCommandEvent& event);
        void OnCloseAll(wxCommandEvent& event);
        void OnCloseAllOthers(wxCommandEvent& event);
        void OnSave(wxCommandEvent& event);
        void OnSaveAll(wxCommandEvent& event);
        void OnSwapHeaderSource(wxCommandEvent& event);
        void OnOpenContainingFolder(wxCommandEvent& event);
        void OnTabPosition(wxCommandEvent& event);
        void OnProperties(wxCommandEvent& event);
        void OnAddFileToProject(wxCommandEvent& event);
        void OnRemoveFileFromProject(wxCommandEvent& event);
        void OnShowFileInTree(wxCommandEvent& event);
        void OnAppDoneStartup(wxCommandEvent& event);
        void OnAppStartShutdown(wxCommandEvent& event);
        void OnUpdateUI(wxUpdateUIEvent& event);
        void OnTreeItemSelected(wxTreeEvent &event);
        void OnTreeItemActivated(wxTreeEvent &event);
        void OnTreeItemRightClick(wxTreeEvent &event);
        void CollectDefines(CodeBlocksEvent& event);
        void SetZoom(int zoom);
        int GetZoom()const;

        void MarkReadOnly(int page, bool readOnly = true);

        void OnAppActivated(CodeBlocksEvent& event);
        wxString GetSelectionClipboard();
        void SetSelectionClipboard(const wxString& data);

    protected:
        // m_EditorsList access
        void AddEditorBase(EditorBase* eb);
        void RemoveEditorBase(EditorBase* eb, bool deleteObject = true);
        cbEditor* InternalGetBuiltinEditor(int page);
        EditorBase* InternalGetEditorBase(int page);

    private:
        EditorManager(cb_unused const EditorManager& rhs); // prevent copy construction

        EditorManager();
        ~EditorManager();
        void OnCheckForModifiedFiles(wxCommandEvent& event);
        bool IsHeaderSource(const wxFileName& candidateFile, const wxFileName& activeFile, FileType ftActive, bool& isCandidate);
        wxFileName FindHeaderSource(const wxArrayString& candidateFilesArray, const wxFileName& activeFile, bool& isCandidate);

        cbAuiNotebook*             m_pNotebook;
        cbNotebookStack*           m_pNotebookStackHead;
        cbNotebookStack*           m_pNotebookStackTail;
        size_t                     m_nNotebookStackSize;
        EditorColourSet*           m_Theme;
        int                        m_Zoom;
        bool                       m_isCheckingForExternallyModifiedFiles;
        friend struct              EditorManagerInternalData;
        EditorManagerInternalData* m_pData;

        DECLARE_EVENT_TABLE()
};

#endif // EDITORMANAGER_H


