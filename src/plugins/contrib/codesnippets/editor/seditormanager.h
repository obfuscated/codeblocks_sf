/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef SEDITORMANAGER_H
#define SEDITORMANAGER_H

#include <wx/list.h>
#include <wx/treectrl.h>
#include <wx/hashmap.h>

#include "settings.h"
#include "manager.h"
#include "cbexception.h" // cbThrow

#include "printing_types.h"

#ifndef CB_PRECOMP
    #include "globals.h" // cbC2U
#endif

//DLLIMPORT extern int ID_NBEditorManager;
//DLLIMPORT extern int ID_EditorManager;
//DLLIMPORT extern int idEditorManagerCheckFiles;
//DLLIMPORT extern int ID_EditorManagerCloseButton;
 extern int ID_NBSEditorManager;
 extern int ID_SEditorManager;
 extern int idSEditorManagerCheckFiles;
 extern int ID_SEditorManagerCloseButton;

// forward decls
class SEditorBase;
class cbAuiNotebook;
class wxAuiNotebookEvent;
class wxMenuBar;
class SEditorColourSet;
class cbProject;
class ProjectFile;
class ScbEditor;
class cbStyledTextCtrl;
class ListCtrlLogger;
class LoaderBase;
struct EditorManagerInternalData;

WX_DECLARE_STRING_HASH_MAP(wxString, SAutoCompleteMap);

// forward decl
struct cbFindReplaceData;

/*
 * No description
 */
//class DLLIMPORT EditorManager : public Mgr<EditorManager>, public wxEvtHandler
class SEditorManager : public wxEvtHandler
{
        friend class ThreadSearchFrame;
        friend class CodeSnippetsConfig;

        static bool s_CanShutdown;
    public:
        //friend class SnippetsSearchFrame; // give Manager access to our private members
        static bool CanShutdown(){ return s_CanShutdown; }

        SEditorManager(const SEditorManager& rhs) { cbThrow(_T("Can't call SEditorManager's copy ctor!!!")); }
        virtual void operator=(const SEditorManager& rhs){ cbThrow(_T("Can't assign an SEditorManager* !!!")); }

        cbAuiNotebook* GetNotebook(){ return m_pNotebook; }
        void CreateMenu(wxMenuBar* menuBar);
        void ReleaseMenu(wxMenuBar* menuBar);
        void Configure();
        int GetEditorsCount();
        SAutoCompleteMap& GetAutoCompleteMap(){ return m_AutoCompleteMap; }

        SEditorBase* IsOpen(const wxString& filename);
        ScbEditor* Open(const wxString& filename, int pos = 0,ProjectFile* data = 0);
        ScbEditor* Open(LoaderBase* fileLdr, const wxString& filename, int pos = 0,ProjectFile* data = 0);
        SEditorBase* GetEditor(int index);
        SEditorBase* GetEditor(const wxString& filename){ return IsOpen(filename); } // synonym of IsOpen()
        SEditorBase* GetActiveEditor();
        ScbEditor* GetBuiltinEditor(SEditorBase* eb);
        int FindPageFromEditor(SEditorBase* eb);

        // "overloaded" functions for easier access
        // they all return a cbEditor pointer if the editor is builtin, or NULL
        ScbEditor* IsBuiltinOpen(const wxString& filename){ return GetBuiltinEditor(IsOpen(filename)); }
        ScbEditor* GetBuiltinEditor(int index){ return GetBuiltinEditor(GetEditor(index)); }
        ScbEditor* GetBuiltinEditor(const wxString& filename){ return IsBuiltinOpen(filename); } // synonym of IsBuiltinOpen()
        ScbEditor* GetBuiltinActiveEditor(){ return GetBuiltinEditor(GetActiveEditor()); }

        void ActivateNext();
        void ActivatePrevious();
        void SetActiveEditor(SEditorBase* ed);
        SEditorColourSet* GetColourSet(){ return m_Theme; }
        void SetColourSet(SEditorColourSet* theme);
        ScbEditor* New(const wxString& newFileName = wxEmptyString);

        // these are used *only* for custom editors
        void AddCustomEditor(SEditorBase* eb);
        void RemoveCustomEditor(SEditorBase* eb);

        bool UpdateSnippetFiles(cbProject* project);
        bool IsHeaderSource(const wxFileName& testedFileName, const wxFileName& activeFileName, FileType ftActive);
        wxFileName FindHeaderSource(const wxArrayString& candidateFilesArray, const wxFileName& activeFile, bool& isCandidate);
        bool SwapActiveHeaderSource();
        bool CloseActive(bool dontsave = false);
        bool Close(const wxString& filename,bool dontsave = false);
        bool Close(SEditorBase* editor,bool dontsave = false);
        bool Close(int index,bool dontsave = false);

        // If file is modified, queries to save (yes/no/cancel).
        // Returns false on "cancel".
        bool QueryClose(SEditorBase* editor);
        bool QueryCloseAll();
        bool CloseAll(bool dontsave=false);
        bool CloseAllExcept(SEditorBase* editor,bool dontsave=false);
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

        int GetManagerID();
        void FilePrint(wxWindow* parent); //ported from main.cpp
        int GetLongestLinePixelWidth( int top_line = -1, int bottom_line = -1); //(pecan 2007/4/04)

        void OnGenericContextMenuHandler(wxCommandEvent& event);
        void OnPageChanged(wxAuiNotebookEvent& event);
        void OnPageChanging(wxAuiNotebookEvent& event);
        void OnPageClose(wxAuiNotebookEvent& event);
        void OnTabRightUp(wxAuiNotebookEvent& event);
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

    protected:
        // m_EditorsList access
        void AddEditorBase(SEditorBase* eb);
        void RemoveEditorBase(SEditorBase* eb, bool deleteObject = true);
        ScbEditor* InternalGetBuiltinEditor(int page);
        SEditorBase* InternalGetEditorBase(int page);

        void CreateSearchLog();
        void LogSearch(const wxString& file, int line, const wxString& lineText);

        void LoadAutoComplete();
        void SaveAutoComplete();

        SAutoCompleteMap m_AutoCompleteMap;
    public:
        SEditorManager(wxWindow* parent);
        ~SEditorManager();
    private:
        SEditorManager();
        void InitSEditorManager(wxWindow* parent);

        void CalculateFindReplaceStartEnd(cbStyledTextCtrl* control, cbFindReplaceData* data, bool replace = false);
        void OnCheckForModifiedFiles(wxCommandEvent& event);
        int Find(cbStyledTextCtrl* control, cbFindReplaceData* data);
        int FindInFiles(cbFindReplaceData* data);
        int Replace(cbStyledTextCtrl* control, cbFindReplaceData* data);
        int ReplaceInFiles(cbFindReplaceData* data);

        cbAuiNotebook* m_pNotebook;
        cbFindReplaceData* m_LastFindReplaceData;
        SEditorColourSet* m_Theme;
        ListCtrlLogger* m_pSearchLog;
        int m_SearchLogIndex;
        int m_SashPosition;
        int m_zoom;
        bool m_isCheckingForExternallyModifiedFiles;
        friend struct EditorManagerInternalData;
        EditorManagerInternalData* m_pData;

        wxWindow* m_pParent;

        DECLARE_EVENT_TABLE()
};

#endif // SEDITORMANAGER_H


