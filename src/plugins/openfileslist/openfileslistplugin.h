/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef OPENFILESLISTPLUGIN_H
#define OPENFILESLISTPLUGIN_H

#include <cbplugin.h>
#include <projectfile.h>

#include <wx/dynarray.h>

class wxTreeCtrl;
class wxTreeEvent;
class wxMenuBar;
class wxImageList;
class EditorBase;

struct TargetFilesData
{
    TargetFilesData() : activeFile(nullptr) {} // ctor
    // Functor for the std::set predicate to sort the opened editor files according to their tab order
    struct compareLess
    {
        bool operator()(const ProjectFile* lhs, const ProjectFile* rhs) { return lhs->editorTabPos < rhs->editorTabPos; }
    };
    typedef std::set<ProjectFile*, compareLess> OpenFilesSet;
    ProjectFile* activeFile;
    OpenFilesSet openFiles;
};

WX_DEFINE_ARRAY(EditorBase*, EditorArray);
typedef std::map<wxString, TargetFilesData> ProjectFilesMap;
typedef std::map<cbProject*, ProjectFilesMap> WorkspaceFilesMap;

class OpenFilesListPlugin : public cbPlugin
{
    public:
        OpenFilesListPlugin();
        virtual ~OpenFilesListPlugin();

        virtual int GetConfigurationGroup() const { return cgEditor; }

        virtual void BuildMenu(wxMenuBar* menuBar);

        virtual void OnAttach();
        virtual void OnRelease(bool appShutDown);
    protected:
        int GetOpenFilesListIcon(EditorBase* ed);
        void RebuildOpenFilesTree();
        void RefreshOpenFilesTree(EditorBase* ed, bool remove = false);

        void OnTreeItemActivated(wxTreeEvent& event);
        void OnTreeItemRightClick(wxTreeEvent& event);
        void OnViewOpenFilesTree(wxCommandEvent& event);
        void OnViewPreserveOpenEditors(wxCommandEvent& event);
        void OnUpdateUI(wxUpdateUIEvent& event);

        void OnEditorActivated(CodeBlocksEvent& event);
        void OnEditorClosed(CodeBlocksEvent& event);
        void OnEditorDeactivated(CodeBlocksEvent& event);
        void OnEditorModified(CodeBlocksEvent& event);
        void OnEditorOpened(CodeBlocksEvent& event);
        void OnEditorSaved(CodeBlocksEvent& event);

        void OnProjectOpened(CodeBlocksEvent& event);
        void OnProjectActivated(CodeBlocksEvent& event);
        void OnProjectClosed(CodeBlocksEvent& event);
        void OnBuildTargetSelected(CodeBlocksEvent& event);

        wxTreeCtrl* m_pTree;
        wxImageList* m_pImages;
        wxMenu* m_ViewMenu;
        wxMenuItem* m_ViewPreserveChk;
    private:
        void SaveEditors(cbProject* project, const wxString& targetName);
        void CloseEditors(cbProject* project, const wxString& targetName);
        void LoadEditors(cbProject* project, const wxString& targetName);

        EditorArray m_EditorArray;
        bool m_PreserveOpenEditors;
        bool m_ProjectLoading;
        cbProject* m_pActiveProject;
        wxString m_ActiveTargetName;
        WorkspaceFilesMap m_WorkspaceFilesMap;
        DECLARE_EVENT_TABLE();
};



#endif // OPENFILESLISTPLUGIN_H
