/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef OPENFILESLISTPLUGIN_H
#define OPENFILESLISTPLUGIN_H

#include <cbplugin.h>
#include <projectfile.h>

#include <wx/dynarray.h>
#include <functional>

class wxTreeCtrl;
class wxTreeEvent;
class wxMenuBar;
class wxImageList;
class EditorBase;

struct TargetFilesData
{
    TargetFilesData() : activeFile(nullptr) {} // ctor

    typedef std::set<ProjectFile*, std::less<ProjectFile*>> OpenFilesSet;
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
        ~OpenFilesListPlugin() override;

        int GetConfigurationGroup() const override { return cgEditor; }

        void BuildMenu(wxMenuBar* menuBar) override;

        void OnAttach() override;
        void OnRelease(bool appShutDown) override;
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
        std::unique_ptr<wxImageList> m_pImages;
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
