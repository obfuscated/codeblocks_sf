#ifndef EDITORMANAGER_H
#define EDITORMANAGER_H

#include <wx/list.h>
#include <wx/treectrl.h>

#include "settings.h"
#include "sanitycheck.h"

// New Feature: Opened Files tree in Projects tab
#define USE_OPENFILES_TREE

#include "cbeditor.h"
#include "cbproject.h"

extern int ID_EditorManager;

enum EditorInterfaceType
{
    eitTabbed = 0,
    eitMDI
};

WX_DECLARE_LIST(cbEditor, EditorsList);

// forward decls
class wxMenuBar;
class wxNotebook;
class EditorColorSet;
class cbProject;

#ifdef USE_OPENFILES_TREE
class MiscTreeItemData;

class DLLIMPORT EditorTreeData : public MiscTreeItemData
{
    public:
        EditorTreeData(wxEvtHandler *owner,const wxString &fullname)
        { SetOwner(owner);m_fullname = fullname; }
        wxString GetFullName(){ return m_fullname; }
        void SetFullName(const wxString &fullname){ m_fullname = fullname; }
    private:
        wxString m_fullname;
};
#endif

struct cbFindReplaceData
{
    int start;
    int end;
    wxString findText;
    wxString replaceText;
    bool findInFiles;
    bool matchWord;
    bool startWord;
    bool matchCase;
    bool regEx;
    bool directionDown;
    bool originEntireScope;
    bool scopeSelectedText;
};

/*
 * No description
 */
class DLLIMPORT EditorManager : public wxEvtHandler
{
        static bool s_CanShutdown;
    public:
        friend class Manager; // give Manager access to our private members
        static bool CanShutdown(){ return s_CanShutdown; }
        void CreateMenu(wxMenuBar* menuBar);
        void ReleaseMenu(wxMenuBar* menuBar);
        void Configure();        
        int GetEditorsCount(){ return m_EditorsList.GetCount(); }
        cbEditor* GetEditor(int index);
        cbEditor* GetEditor(const wxString& filename){ return IsOpen(filename); } // synonym of IsOpen()
        cbEditor* IsOpen(const wxString& filename);
        cbEditor* Open(const wxString& filename, int pos = 0);
        cbEditor* GetActiveEditor();
        void SetActiveEditor(cbEditor* ed);
        EditorColorSet* GetColorSet(){ return (this==NULL) ? 0 : m_Theme; }
        void SetColorSet(EditorColorSet* theme);
        const EditorInterfaceType& GetEditorInterfaceType(){ return m_IntfType; }
        void SetEditorInterfaceType(const EditorInterfaceType& _type);
        cbEditor* New();
        bool UpdateProjectFiles(cbProject* project);
        bool SwapActiveHeaderSource();
        bool CloseActive(bool dontsave = false);
        bool Close(const wxString& filename,bool dontsave = false);
        bool Close(cbEditor* editor,bool dontsave = false);
        bool Close(int index,bool dontsave = false);

        // If file is modified, queries to save (yes/no/cancel). 
        // Returns false on "cancel".
        bool QueryClose(cbEditor *editor);
        bool QueryCloseAll();
        bool CloseAll(bool dontsave=false);
        bool CloseAllExcept(cbEditor* editor,bool dontsave=false);
        bool Save(const wxString& filename);
        bool Save(int index);
        bool SaveActive();
        bool SaveAs(int index);
        bool SaveActiveAs();
        bool SaveAll();
        int ShowFindDialog(bool replace);
        int Find(cbEditor* editor, cbFindReplaceData* data);
        int Replace(cbEditor* editor, cbFindReplaceData* data);
        int FindNext(bool goingDown);

        /** Check if one of the open files has been modified outside the IDE. If so, ask to reload it. */
        void CheckForExternallyModifiedFiles();
        
// TODO (rick#1#): check if all these belong to the public section of the class...
        #ifdef USE_OPENFILES_TREE
        /// Is the opened files tree supported? (depends on platform)
        bool OpenFilesTreeSupported();
        /// Show/hide the opened files tree
        void ShowOpenFilesTree(bool show);
        /// Return true if opened files tree is visible, false if not
        bool IsOpenFilesTreeVisible();
        /** Builds Opened Files tree in the Projects tab
          */
        wxTreeCtrl *EditorManager::GetTree();
        wxTreeItemId FindTreeFile(const wxString& filename);
        wxString GetTreeItemFilename(wxTreeItemId item);
        void DeleteItemfromTree(wxTreeItemId item);
        void DeleteFilefromTree(const wxString& filename);
        void AddFiletoTree(cbEditor* ed);
        bool RenameTreeFile(const wxString& oldname, const wxString& newname);
        void InitPane();
        void BuildOpenedFilesTree(wxWindow* parent);
        void RebuildOpenedFilesTree(wxTreeCtrl *tree = 0L);
        void RefreshOpenedFilesTree(bool force = false);
        #endif
        
        void OnUpdateUI(wxUpdateUIEvent& event);
        void OnTreeItemSelected(wxTreeEvent &event);
        void OnTreeItemActivated(wxTreeEvent &event);
        void OnTreeItemRightClick(wxTreeEvent &event);
        
    private:
        static EditorManager* Get(wxWindow* parent);
        static void Free();
        EditorManager(wxWindow* parent);
        ~EditorManager();
        void UpdateEditorIndices();
        void CalculateFindReplaceStartEnd(cbEditor* editor, cbFindReplaceData* data);
        EditorsList m_EditorsList;
        cbFindReplaceData* m_LastFindReplaceData;
        EditorColorSet* m_Theme;
        EditorInterfaceType m_IntfType;
        #ifdef USE_OPENFILES_TREE
        wxImageList* m_pImages;
        wxTreeCtrl* m_pTree;
        wxTreeItemId m_TreeOpenedFiles;
        #endif
        wxString m_LastActiveFile;
        bool m_LastModifiedflag;
    DECLARE_EVENT_TABLE()
    DECLARE_SANITY_CHECK

};

#endif // EDITORMANAGER_H

