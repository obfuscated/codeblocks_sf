#ifndef OPENFILESLISTPLUGIN_H
#define OPENFILESLISTPLUGIN_H

#include <cbplugin.h>

#include <wx/dynarray.h>

class wxTreeCtrl;
class wxTreeEvent;
class wxMenuBar;
class wxImageList;
class EditorBase;

WX_DEFINE_ARRAY(EditorBase*, EditorArray);

class OpenFilesListPlugin : public cbPlugin
{
    public:
        OpenFilesListPlugin();
        virtual ~OpenFilesListPlugin();

        virtual int GetConfigurationGroup() const { return cgEditor; }

        virtual void BuildMenu(wxMenuBar* menuBar);
        virtual void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0){}
        virtual bool BuildToolBar(wxToolBar* toolBar){ return false; }

        virtual void OnAttach();
        virtual void OnRelease();
    protected:
        int GetOpenFilesListIcon(EditorBase* ed);
        void RebuildOpenFilesTree();
        void RefreshOpenFilesTree(EditorBase* ed, bool remove = false);

        void OnTreeItemActivated(wxTreeEvent &event);
        void OnTreeItemRightClick(wxTreeEvent &event);
        void OnViewOpenFilesTree(wxCommandEvent& event);
        void OnUpdateUI(wxUpdateUIEvent& event);

        void OnEditorActivated(CodeBlocksEvent& event);
        void OnEditorClosed(CodeBlocksEvent& event);
        void OnEditorDeactivated(CodeBlocksEvent& event);
        void OnEditorModified(CodeBlocksEvent& event);
        void OnEditorOpened(CodeBlocksEvent& event);
        void OnEditorSaved(CodeBlocksEvent& event);

        void OnProjectOpened(CodeBlocksEvent& event);

        wxTreeCtrl* m_pTree;
        wxImageList* m_pImages;
        wxMenu* m_ViewMenu;
    private:
        EditorArray m_EditorArray;
        DECLARE_EVENT_TABLE();
};



#endif // OPENFILESLISTPLUGIN_H
