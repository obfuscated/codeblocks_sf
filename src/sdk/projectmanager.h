#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <wx/event.h>
#include <wx/dynarray.h>
#include <wx/treectrl.h>
#include "sanitycheck.h"
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

/** @brief The entry point singleton for working with projects.
  *
  * This is the project manager class. It handles all open projects and workspaces.
  * It is through this class that you make requests about projects, like open
  * a project, save it, set the active project (if more than one are opened), etc.
  * To access a project, you must retrieve it from this class.
  *
  * As all managers, this is a singleton class which you retrieve by asking
  * the global Manager singleton.
  * E.g. Manager::Get()->GetProjectManager()
  */
class DLLIMPORT ProjectManager : public wxEvtHandler
{
	public:
        friend class Manager; // give Manager access to our private members
        /// Application menu creation. Called by the application only.
		static void CreateMenu(wxMenuBar* menuBar);
        /// Application menu removal. Called by the application only.
		void ReleaseMenu(wxMenuBar* menuBar);
        /** @return True if modified, false if not.
          * @note: Refers to the workspace, not to a particular project.
          */
		bool GetModified(){ return (this==NULL) ? false : m_Modified; }
        /** Set the workspace modification status.
          * @param mod The modification flag.
          * @note: Refers to the workspace, not to a particular project.
          */
		void SetModified(bool mod = true){ if(this!=NULL) m_Modified = mod; }
        /** Retrieve the active project. Most of the times, this is the function
          * you 'll be calling in ProjectManager.
          * @return A pointer to the active project.
          */
		cbProject* GetActiveProject(){ return (this==NULL) ? 0 : m_pActiveProject; }
        /** Retrieve an array of all the opened projects. This is a standard
          * wxArray containing pointers to projects. Using this array you can
          * iterate through all the opened projects.
          * @return A pointer to the array containing the projects.
          */
        ProjectsArray* GetProjects(){ return (this==NULL) ? 0 : m_pProjects; }
        /** Check if a project is open based on the project's filename.
          * @param filename The project's filename. Must be an absolute path.
          * @return A pointer to the project if it is open, or NULL if it is not.
          */
		cbProject* IsOpen(const wxString& filename);
        /** Set the active project. Use this function if you want to change the
          * active project.
          * @param project A pointer to the new active project.
          * @param refresh If true, refresh the project manager's tree, else do not refresh it.
          */
		void SetProject(cbProject* project, bool refresh = true);
        /** Load a project from disk. This function, internally, uses IsOpen()
          * so that the same project can't be loaded twice.
          * @param filename The project file's filename.
          * @return If the function succeeds, a pointer to the newly opened project
          * is returned. Else the return value is NULL.
          */
        cbProject* LoadProject(const wxString& filename);
        /** Save a project to disk.
          * @param project A pointer to the project to save.
          * @return True if saving was succesful, false if not.
          */
        bool SaveProject(cbProject* project);
        /** Save a project to disk, asking for a filename.
          * @param project A pointer to the project to save.
          * @return True if saving was succesful, false if not.
          * @note A false return value doesn't necessarily mean failure. The user
          * might have cancelled the SaveAs dialog...
          */
        bool SaveProjectAs(cbProject* project);
        /** Save the active project to disk. Same as SaveProject(GetActiveProject()).
          * @return True if saving was succesful, false if not.
          */
        bool SaveActiveProject();
        /** Save the active project to disk, asking for a filename.
          * Same as SaveProjectAs(GetActiveProject()).
          * @return True if saving was succesful, false if not.
          * @note A false return value doesn't necessarily mean failure. The user
          * might have cancelled the SaveAs dialog...
          */
        bool SaveActiveProjectAs();
        /** Saves all projects to disk.
          * @return True if all projects were saved, false if even one save operation failed.
          */
        bool SaveAllProjects();
        /** Close a project.
          * @param project A pointer to the project to close.
          * @return True if project was closed, false if not.
          */
        bool CloseProject(cbProject* project);
        /** Close the active project. Same as CloseProject(GetActiveProject()).
          * @return True if project was closed, false if not.
          */
        bool CloseActiveProject();
        /** Close all projects.
          * @return True if all projects were closed, false if even one close operation failed.
          */
        bool CloseAllProjects();
        /** Move a project up in the project manager tree. This effectively
          * re-orders the projects build order.
          * @param project The project to move up.
          * @param warpAround If true and the project is at the top of the list order,
          * then it wraps and goes to the bottom of the list.
          */
        void MoveProjectUp(cbProject* project, bool warpAround = false);
        /** Move a project down in the project manager tree. This effectively
          * re-orders the projects build order.
          * @param project The project to move down.
          * @param warpAround If true and the project is at the bottom of the list order,
          * then it wraps and goes to the top of the list.
          */
        void MoveProjectDown(cbProject* project, bool warpAround = false);
        /** Create a new empty project.
          * @return A pointer to the new project if succesful, or NULL if not.
          * @note When the new project is created, it asks the user where to save it.
          * If the user cancels the Save dialog, then NULL is returned from this function.
          */
        cbProject* NewProject();
        /** Add a file to a project. This function comes in two versions. This version,
          * expects a single build target indx for the added file to belong to.
          * @param filename The file to add to the project.
          * @param project The project to add this file to. If NULL, the active project is used.
          * @param target The index of the project's build target to add this file.
          * @return The build target's index that this file was added to.
          * @note If the index supplied is -1 then, if the project has exactly
          * one build target, the file is added to this single build target.
          * If the project has more than one build targets, a dialog appears so
          * that the user can select which build target this file should belong to.
          */
		int AddFileToProject(const wxString& filename, cbProject* project = 0L, int target = -1);
        /** Add a file to a project. This function comes in two versions. This version,
          * expects an array of build target indices for the added file to belong to.
          * @param filename The file to add to the project.
          * @param project The project to add this file to. If NULL, the active project is used.
          * @param targets The array of the project's build targets indices to add this file.
          * @return The number of build targets this file was added to.
          * @note If the targets array is empty then, if the project has exactly
          * one build target, the file is added to this single build target.
          * If the project has more than one build targets, a dialog appears so
          * that the user can select which build targets (multiple) this file should belong to.\n
          * Also note than when this function returns, the targets array will contain
          * the user-selected build targets.
          */
		int AddFileToProject(const wxString& filename, cbProject* project, wxArrayInt& targets);
		/** Utility function. Displays a single selection list of a project's
		  * build targets to choose from.
		  * @param project The project to use. If NULL, the active project is used.
		  * @return The selected build target's index, or -1 if no build target was selected.
		  */
		int AskForBuildTargetIndex(cbProject* project = 0L);
		/** Utility function. Displays a multiple selection list of a project's
		  * build targets to choose from.
		  * @param project The project to use. If NULL, the active project is used.
		  * @return An integer array containing the selected build targets indices.
		  * This array will be empty if no build targets were selected.
		  */
		wxArrayInt AskForMultiBuildTargetIndex(cbProject* project = 0L);
		/** Load a workspace.
		  * @param filename The workspace to open.
		  * @return True if the workspace loads succefully, false if not.
		  */
		bool LoadWorkspace(const wxString& filename = "");
		/** Save the workspace.
		  * @param filename The workspace to save.
		  * @return True if the workspace loads succefully, false if not.
		  */
		bool SaveWorkspace(const wxString& filename = "");
		/** Get the current workspace filename.
		  * @return The current workspace filename.
		  */
        const wxString& GetWorkspace();
		/// Rebuild the project manager's tree.
        void RebuildTree();
		/** Stop the tree control from updating.
		  * @note This operation is accumulative. This means you have to call
		  * UnfreezeTree() as many times as you 've called FreezeTree() for the 
		  * tree control to un-freeze (except if you call UnfreezeTree(true)).
		  */
        void FreezeTree();
		/** Le the tree control be updated again.
		  * @param force If true the tree control is forced to un-freeze. Else it
		  * depends on freeze-unfreeze balance (see note).
		  * @note This operation is accumulative. This means you have to call
		  * UnfreezeTree() as many times as you 've called FreezeTree() for the 
		  * tree control to un-freeze (except if you call UnfreezeTree(true)).
		  */
        void UnfreezeTree(bool force = false);
        /** Retrieve a pointer to the project manager's tree (GUI).
          * @return A pointer to a wxTreeCtrl window.
          */
        wxTreeCtrl* GetTree(){ return m_pTree; }
        /** Retrieve a pointer to the project manager's panel (GUI). This panel
          * is the parent of the project manager's tree obtained through GetTree().
          * @return A pointer to a wxPanel window.
          */
		wxPanel* GetPanel(){ return m_pPanel; }
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
		int m_TreeFreezeCounter;
		
        DECLARE_EVENT_TABLE()
        DECLARE_SANITY_CHECK
};

#endif // PROJECTMANAGER_H

