#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <wx/event.h>
#include <wx/dynarray.h>
#include <wx/treectrl.h>

#include "settings.h"

// forward decls
class wxMenuBar;
class wxNotebook;
class wxPanel;
class cbProject;
class wxImageList;
class ProjectFile;
class FilesGroupsAndMasks;

WX_DEFINE_ARRAY(cbProject*, ProjectsArray);

/*
 * No description
 */
class DLLIMPORT ProjectManager : public wxEvtHandler
{
	public:
        friend class Manager; // give Manager access to our private members
		void CreateMenu(wxMenuBar* menuBar);
		void ReleaseMenu(wxMenuBar* menuBar);
		bool GetModified(){ return m_Modified; }
		void SetModified(bool mod = true){ m_Modified = mod; }
		cbProject* GetActiveProject(){ return m_pActiveProject; }
        ProjectsArray* GetProjects(){ return m_pProjects; }
        wxTreeCtrl* GetTree(){ return m_pTree; }
		wxPanel* GetPanel(){ return m_pPanel; }
		cbProject* IsOpen(const wxString& filename);
		void SetProject(cbProject* project, bool refresh = true);
        cbProject* LoadProject(const wxString& filename);
        bool SaveProject(cbProject* project);
        bool SaveActiveProject();
        bool SaveActiveProjectAs();
        bool SaveAllProjects();
        bool CloseProject(cbProject* project);
        bool CloseActiveProject();
        bool CloseAllProjects();
        cbProject* NewProject();
		int AddFileToProject(const wxString& filename, cbProject* project = 0L, int target = -1);
		int AskForBuildTargetIndex(cbProject* project = 0L);
		bool LoadWorkspace(const wxString& filename = "");
		bool SaveWorkspace(const wxString& filename = "");
        const wxString& GetWorkspace();
        void RebuildTree();
    private:
        static ProjectManager* Get(wxNotebook* parent);
		static void Free();
		ProjectManager(wxNotebook* parent);
		~ProjectManager();
        void ShowMenu(wxTreeItemId id, const wxPoint& pt);
        void OnProjectFileActivated(wxTreeEvent& event);
        void OnExecParameters(wxCommandEvent& event);
        void OnTreeItemRightClick(wxTreeEvent& event);
        void OnRightClick(wxCommandEvent& event);
        void OnSetActiveProject(wxCommandEvent& event);
        void OnAddFileToProject(wxCommandEvent& event);
        void OnRemoveFileFromProject(wxCommandEvent& event);
        void OnCloseProject(wxCommandEvent& event);
        void OnCloseFile(wxCommandEvent& event);
        void OnOpenFile(wxCommandEvent& event);
        void OnProperties(wxCommandEvent& event);
		void OnGotoFile(wxCommandEvent& event);
        void OnViewCategorize(wxCommandEvent& event);
        void OnViewUseFolders(wxCommandEvent& event);
        void OnViewFileMasks(wxCommandEvent& event);
        void DoOpenSelectedFile();
		void DoOpenFile(ProjectFile* pf, const wxString& filename);

		bool m_Modified; // workspace modified?
        wxNotebook* m_pParent;
        wxTreeCtrl* m_pTree;
		wxPanel* m_pPanel;
        wxTreeItemId m_TreeRoot;
        cbProject* m_pActiveProject;
        wxImageList* m_pImages;
        ProjectsArray* m_pProjects;
        wxString m_Workspace;
        bool m_TreeCategorize;
        bool m_TreeUseFolders;
		FilesGroupsAndMasks* m_pFileGroups;

        DECLARE_EVENT_TABLE()
};

#endif // PROJECTMANAGER_H

