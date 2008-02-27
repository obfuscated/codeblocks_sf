/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef CBPROJECT_H
#define CBPROJECT_H

#include <wx/datetime.h>
#include <wx/dynarray.h>
#include <wx/hashmap.h>
#include <wx/treectrl.h>

#include "settings.h"
#include "misctreeitemdata.h"
#include "compiletargetbase.h"
#include "cbplugin.h"
#include "projectbuildtarget.h"

#include <map>

// forward decl
class cbProject;
class ProjectBuildTarget;
class ProjectFile;
class FilesGroupsAndMasks;
class TiXmlNode;
class TiXmlElement;

// hashmap for fast searches in cbProject::GetFileByFilename()
WX_DECLARE_STRING_HASH_MAP(ProjectFile*, ProjectFiles);

typedef std::map<wxString, wxArrayString> VirtualBuildTargetsMap;

class DLLIMPORT FileTreeData : public MiscTreeItemData
{
    public:
        /// The kind of tree node
        enum FileTreeDataKind
        {
            ftdkUndefined = 0,
            ftdkProject,
            ftdkFolder,
            ftdkFile,
            ftdkVirtualGroup, // wilcard matching
            ftdkVirtualFolder
        };

        FileTreeData(cbProject* project, FileTreeDataKind kind = ftdkUndefined)
            : m_Index(-1),
            m_Project(project),
            m_file(0),
            m_kind(kind)
        {}

        FileTreeDataKind GetKind() const { return m_kind; }
        cbProject* GetProject() const { return m_Project; }
        int GetFileIndex() const { return m_Index; }
        ProjectFile* GetProjectFile() const { return m_file; }
        const wxString& GetFolder() const { return m_folder; }

        void SetKind(FileTreeDataKind kind){ m_kind = kind; }
        void SetProject(cbProject* project){ m_Project = project; }
        // only valid for file selections
        void SetFileIndex(int index){ m_Index = index; }
        void SetProjectFile(ProjectFile* file){ m_file = file; }
        // only valid for folder selections
        void SetFolder(const wxString& folder){ m_folder = folder; }
    private:
        int m_Index;
        cbProject* m_Project;
        ProjectFile* m_file;
        wxString m_folder;
        FileTreeDataKind m_kind;
};

/** Precompiled headers mode.
  * Defines where and how are the project's precompiled headers generated.
  * Currently implemented only for GCC (3.4 and above).
  */
enum PCHMode
{
    pchSourceDir = 0,   /// In a dir (named by the PCH) on the same level as the source header (default).
    pchObjectDir,       /// In the objects output dir, along with other object files.
    pchSourceFile,      /// In a file alongside the source header (with .gch appended).
};

/** @brief Represents a Code::Blocks project.
  *
  * A project is a collection of build targets and files.
  * Each project can contain any number of build targets and files.
  * @see ProjectBuildTarget, ProjectFile.
  */
class DLLIMPORT cbProject : public CompileTargetBase
{
     public:
        /// Constructor
        cbProject(const wxString& filename = wxEmptyString);
        /// Destructor
        ~cbProject();

        /** @return True if the project fully loaded, false if not. */
        bool IsLoaded() const { return m_Loaded; }

        /** (Re)build the project tree.
          * @param tree The wxTreeCtrl to use.
          * @param root The tree item to use as root. The project is built as a child of this item.
          * @param categorize If true, use virtual folders like "Sources", "Headers", etc.
          * @param useFolders If true, create folders as needed. If false, the list is flat.
          * @param fgam If not NULL, use these file groups and masks for virtual folders.
          */
        void BuildTree(wxTreeCtrl* tree, const wxTreeItemId& root, bool categorize, bool useFolders, FilesGroupsAndMasks* fgam = 0L);

        /** This resets the project to a clear state. Like it's just been new'ed. */
        void ClearAllProperties();

        /** Calculates the top-level path common to all project files.
          * This is called automatically (no need for you to call it) and is used
          * to find the top-level folder for building the tree.
          */
        void CalculateCommonTopLevelPath();

        /** @return the top-level path common to all project files. */
        wxString GetCommonTopLevelPath() const;

        /** @return True if the project is modified in any way. */
        bool GetModified() const;

        /** Mark the project as modified or not.
          * @param modified If true, the project is marked as modified. If false, as not-modified.
          */
        void SetModified(bool modified = true);

        /** Access a file of the project.
          * @param index The index of the file. Must be greater or equal than zero and less than GetFilesCount().
          * @return A pointer to the file or NULL if not found.
          */
        ProjectFile* GetFile(int index);

        /** Access a file of the project.
          * @param filename The filename of the file.
          * @param isRelative True if @c filename is a relative filename, false if not.
          * @param isUnixFilename True if @c filename is already normalized with UnixFilename(), false if not.
          * @return A pointer to the file or NULL if not found.
          */
        ProjectFile* GetFileByFilename(const wxString& filename, bool isRelative = true, bool isUnixFilename = false);

        /** @return The number of files in the project. */
        int GetFilesCount(){ return m_Files.GetCount(); }

        /** Set the Makefile filename used when exporting a Makefile for the project,
          * or when using a custom Makefile to build the project.
          * @param makefile The filename for the Makefile.
          */
        void SetMakefile(const wxString& makefile){ m_Makefile = makefile; SetModified(true); }

        /** @return The filename for the Makefile. */
        const wxString& GetMakefile();

        /** Mark if the project should use a custom Makefile for compilation.
          * @param custom If true, use a custom Makefile for compilation. If false, use direct C::B build mode.
          */
        void SetMakefileCustom(bool custom);

        /** @return True if the project is using a custom Makefile for compilation, false if not. */
        bool IsMakefileCustom(){ return m_CustomMakefile; }

        /** Is there a build target (virtual or real) by @c name?
          * @param name The build target's name.
          * @param virtuals_too Include virtual build targets in query.
          * @return True if exists a build target (virtual or real) by that name, false if not.
          */
        bool BuildTargetValid(const wxString& name, bool virtuals_too = true) const;

        /** @return The first valid (virtual or real) build target. */
        wxString GetFirstValidBuildTargetName(bool virtuals_too = true) const;

        /** @return The build target index which will be pre-selected when the "Select target"
          * dialog appears when running the project. Valid only for multi-target projects. */
        const wxString& GetDefaultExecuteTarget() const;

        /** Set the build target index which will be pre-selected when the "Select target"
          * dialog appears when running the project.
          * @param name The build target's name.
          */
        void SetDefaultExecuteTarget(const wxString& name);

        /** @return The number of build targets this project contains. */
        int GetBuildTargetsCount(){ return m_Targets.GetCount(); }

        /** Access a build target.
          * @param index The build target index. Must be greater or equal to zero and less than GetBuildTargetsCount().
          * @return The build target or NULL if not found.
          */
        ProjectBuildTarget* GetBuildTarget(int index);

        /** Access a build target.
          * @param targetName The build target name.
          * @return The build target or NULL if not found.
          */
        ProjectBuildTarget* GetBuildTarget(const wxString& targetName);

        /** Add a new build target.
          * @param targetName The build target name.
          * @return The build target that was just created.
          */
        ProjectBuildTarget* AddBuildTarget(const wxString& targetName);

        /** Rename a build target.
          * @param index The build target's index to rename.
          * @param targetName The new name for the build target.
          * @return True if @c index was valid, false if not.
          */
        bool RenameBuildTarget(int index, const wxString& targetName);

        /** Rename a build target.
          * @param oldTargetName The build target's old name.
          * @param newTargetName The new name for the build target.
          * @return True if @c oldTargetName was valid, false if not.
          */
        bool RenameBuildTarget(const wxString& oldTargetName, const wxString& newTargetName);

        /** Duplicate a build target.
          * @param index The index of the build target to duplicate.
          * @param newName The name for the new build target. If empty, it will be named like "Copy of <base_target_name>".
          * @return The new build target if @c index was valid, NULL if not (or something went wrong).
          */
        ProjectBuildTarget* DuplicateBuildTarget(int index, const wxString& newName = wxEmptyString);

        /** Duplicate a build target.
          * @param targetName The name of the build target to duplicate.
          * @param newName The name for the new build target. If empty, it will be named like "Copy of <base_target_name>".
          * @return The new build target if @c index was valid, NULL if not (or something went wrong).
          */
        ProjectBuildTarget* DuplicateBuildTarget(const wxString& targetName, const wxString& newName = wxEmptyString);

        /** Export a target as a new project.
          * In other words, save a copy of the project containing only the specified target.
          * The user will be prompted with a dialog to select the new project name.
          * @param index The index of the build target to export.
          * @return True on success, false on failure (or dialog cancellation).
          * @note The dialog to select the new project name is not a file dialog because
          * the user is not allowed to save anywhere. The new project must remain "operational"
          * (assuming the target to export, was "operational") so the new project must be saved
          * in the same directory as the original to preserve relative paths.
          */
        bool ExportTargetAsProject(int index);

        /** Export a target as a new project.
          * In other words, save a copy of the project containing only the specified target.
          * The user will be prompted with a dialog to select the new project name.
          * @param targetName The name of the build target to export.
          * @return True on success, false on failure (or dialog cancellation).
          * @see ExportTargetAsProject(int).
          */
        bool ExportTargetAsProject(const wxString& targetName);

        /** Remove a build target.
          * @param index The index of the build target to remove.
          * @return True if @c index was valid, false if not.
          */
        bool RemoveBuildTarget(int index);

        /** Remove a build target.
          * @param targetName The build target name.
          * @return True if the @c targetName was valid, false if not.
          */
        bool RemoveBuildTarget(const wxString& targetName);

        /** Reorder the list of build targets.
          * This is useful, so that when the special build target "All" is being built
          * targets are built in the desired order.
          * @param nameOrder An array of strings containing build target names in the order you desire.
          * The number of array elements must be equal to GetBuildTargetsCount().
          */
        void ReOrderTargets(const wxArrayString& nameOrder);

        /** Set the active build target.
          * @param name The build target name to set as active. If @c name does
          *             not exist, then the first virtual target is set
          *             or the first real target, depending which is valid.
          * @return True if @c name was valid, false if not.
          */
        bool SetActiveBuildTarget(const wxString& name);

        /** @return The active build target name. Note that this might be a virtual target. */
        const wxString& GetActiveBuildTarget() const;

        /** @return The mode precompiled headers are handled. */
        PCHMode GetModeForPCH() const { return m_PCHMode; }

        /** Set the mode to handle precompiled headers.
          * @param mode The desired PCH mode.
          */
        void SetModeForPCH(PCHMode mode){ m_PCHMode = mode; SetModified(true); }

        void SetCompilerID(const wxString& id); // overriden

        /** @return The root item of this project in the project manager's tree. */
        wxTreeItemId GetProjectNode(){ return m_ProjectNode; }

        /** Act like closing all project files, but don't do it.
          * Used to check if any of the project files need saving.
          * @return True if even one project file needs to be saved, false if not.
          */
        bool QueryCloseAllFiles();

        /** Close all project files.
          * @param dontsave If true, no project file will be saved even if modified.
          * If false, any modified file will be saved (default).
          * @return True if succesfull, false otherwise.
          */
        bool CloseAllFiles(bool dontsave=false);

        /** Save all project files.
          * @return True if succesfull, false otherwise.
          */
        bool SaveAllFiles();

        /** Save the project.
          * @return True if succesfull, false otherwise.
          */
        bool Save();

        /** Save the project under a different name.
          * A dialog pops up for the user to choose a new filename for the project.
          * @return True if succesfull, false otherwise.
          */
        bool SaveAs();

        /** Save the project's layout.
          * Layout is the list of open project files, which one is active,
          * where the cursor is located on each one of those, etc.
          * @return True if succesfull, false otherwise.
          */
        bool SaveLayout();

        /** Load the project's layout.
          * @see SaveLayout() for info.
          * @return True if succesfull, false otherwise.
          */
        bool LoadLayout();
        
        /** Notify that file(s) will be added shortly.
          * This function should be called before calling AddFile().
          * When done calling AddFile() as many times as needed, call
          * EndAddFiles().
          *
          * This sequence of function calls ensures proper events dispatching.
          * This function broadcasts the cbEVT_PROJECT_BEGIN_ADD_FILES event.
          */
		void BeginAddFiles();

        /** Notify that file(s) addition finished.
          * This function should be called when done calling AddFile() as many times as needed.
          *
          * This sequence of function calls ensures proper events dispatching.
          * This function broadcasts the cbEVT_PROJECT_END_ADD_FILES event.
          * @see BeginAddFiles().
          */
		void EndAddFiles();

        /** Notify that file(s) will be removed shortly.
          * This function should be called before calling RemoveFile().
          * When done calling RemoveFile() as many times as needed, call
          * EndRemoveFiles().
          *
          * This sequence of function calls ensures proper events dispatching.
          * This function broadcasts the cbEVT_PROJECT_BEGIN_REMOVE_FILES event.
          */
		void BeginRemoveFiles();

        /** Notify that file(s) removal finished.
          * This function should be called when done calling RemoveFile() as many times as needed.
          *
          * This sequence of function calls ensures proper events dispatching.
          * This function broadcasts the cbEVT_PROJECT_END_REMOVE_FILES event.
          * @see BeginRemoveFiles().
          */
		void EndRemoveFiles();

        /** Add a file to the project.
          * This variation, takes a target name as first parameter.
          * @param targetName The name of the build target to add this file to.
          * @param filename The file's filename. This *must* be a filename relative to the project's path.
          * @param compile If true this file is compiled when building the project.
          * @param link If true this file is linked when building the project.
          * @param weight A value between zero and 100 (defaults to 50). Smaller weight, makes the file compile earlier than those with larger weight.
          * @return The newly added file or NULL if something went wrong.
          */
        ProjectFile* AddFile(const wxString& targetName, const wxString& filename, bool compile = true, bool link = true, unsigned short int weight = 50);

        /** Add a file to the project.
          * This variation, takes a target index as first parameter.
          * @param targetIndex The index of the build target to add this file to.
          * @param filename The file's filename. This *must* be a filename relative to the project's path.
          * @param compile If true this file is compiled when building the project.
          * @param link If true this file is linked when building the project.
          * @param weight A value between zero and 100 (defaults to 50). Smaller weight, makes the file compile earlier than those with larger weight.
          * @return The newly added file or NULL if something went wrong.
          */
        ProjectFile* AddFile(int targetIndex, const wxString& filename, bool compile = true, bool link = true, unsigned short int weight = 50);

        /** Remove a file from the project.
          * @param index The index of the file.
          * @return True if @c index was valid, false if not.
          */
        bool RemoveFile(int index);

        /** Remove a file from the project.
          * @param pf The pointer to ProjectFile.
          * @return True if @c pf was a valid project file, false if not.
          */
        bool RemoveFile(ProjectFile* pf);

        /** Display the project options dialog.
          * @return True if the dialog was closed with "OK", false if closed with "Cancel".
          */
        bool ShowOptions();

        /** Convenience function for remembering the project's tree state when refreshing it.
          * @return An array of strings containing the tree-path names of expanded nodes.
          */
        const wxArrayString& ExpandedNodes(){ return m_ExpandedNodes; }

        /** Convenience function for remembering the project's tree state when refreshing it.
          * Adds an expanded node in this internal list.
          * @param path The tree-path to add.
          */
        void AddExpandedNode(const wxString& path){ m_ExpandedNodes.Add(path); }

        /** Convenience function for remembering the project's tree state when refreshing it.
          * @param tree The tree control to save its expanded state.
          */
        void SaveTreeState(wxTreeCtrl* tree);

        /** Convenience function for remembering the project's tree state when refreshing it.
          * @param tree The tree control to restore its expanded state to a previously saved.
          */
        void RestoreTreeState(wxTreeCtrl* tree);

        /** Displays a target selection dialog.
          * When invoked, a selection dialog is presented to the user so that he/she
          * can select one target from the list of this project's targets.
          * @param initial The index of the pre-selected target when the dialog is displayed.
          * Defaults to none (-1).
          * @param evenIfOne If true, the dialog is still shown even if the project contains only one target.
          * The default behaviour is to not show the dialog if the project has only one target.
          * @return The target's index that the user selected or -1 if the dialog was cancelled.
          */
        int SelectTarget(int initial = -1, bool evenIfOne = false);

        /** Rename the project's title in the tree.
          * @param newname The new title for the project.
          * @note This does *not* actually alter the project's title. It just changes it on the tree.
          */
        void RenameInTree(const wxString &newname);

        /** Get a pointer to the currently compiling target.
          * @return While the project is being built, this function returns the currently building
          * target. For all other times, NULL is returned.
          */
        ProjectBuildTarget* GetCurrentlyCompilingTarget(){ return m_CurrentlyCompilingTarget; }

        /** Set the currently compiling target.
          * @note This function is for internal use by compilers only.
          * Using this function in any other place results in undefined behaviour!
          * @param bt The build target that is currently building.
          */
        void SetCurrentlyCompilingTarget(ProjectBuildTarget* bt);

        /** Define a new virtual build target.
          *
          * A virtual build target is not really a build target itself but it is an alias
          * for a group of other build targets, real or virtual.
          * An example is the "All" virtual build target which means "all build targets".
          *
          * @param alias The virtual build target's name.
          * @param targets A list of build target names to include in this virtual build target.
          *                They can be real or other virtual build targets.
          * @return True for success, false for failure. The only reason for this function
          *         to return false is if a *real* build target exists with the same name as @c alias
          *         (or if you pass an empty @c targets array).
          * @note Every time you call this function with the same @c alias parameter, the virtual
          *       build target is re-defined. In other words, it's not an error if @c alias is already
          *       defined.
          */
        bool DefineVirtualBuildTarget(const wxString& alias, const wxArrayString& targets);

        /** Does a virtual build target exist?
          *
          * @param alias The virtual build target's name.
          * @return True if the virtual build target exists, false if not.
          */
        bool HasVirtualBuildTarget(const wxString& alias) const;

        /** Remove a virtual build target.
          *
          * @param alias The virtual build target's name.
          * @return True if the virtual build target was removed, false if not.
          */
        bool RemoveVirtualBuildTarget(const wxString& alias);

        /** Get a list of all defined virtual build targets.
          *
          * @return A list of all defined virtual build targets.
          */
        wxArrayString GetVirtualBuildTargets() const;

        /** Access a virtual build target's group of build targets.
          *
          * @param alias The virtual build target's name.
          * @return The list of all build targets under the alias @c alias.
          */
        const wxArrayString& GetVirtualBuildTargetGroup(const wxString& alias) const;

        /** Access a virtual build target's expanded group of build targets.
          *
          * The difference from GetVirtualBuildTargetGroup() lies in that this function
          * returns the full list of real build targets in this group (by recursively
          * expanding virtual build targets in the group).
          * @param alias The virtual build target's name.
          * @return The expanded list of all real build targets under the alias @c alias.
          */
        wxArrayString GetExpandedVirtualBuildTargetGroup(const wxString& alias) const;

        /** Checks if a build target (virtual or real) can be added to a virtual build target,
          * without causing a circular-reference.
          *
          * @param alias The "parent" virtual build target to add the build target in.
          * @param target The build target to add in the @c alias virtual build target.
          * @return True if a circular reference is not detected, false if it is.
          */
        bool CanAddToVirtualBuildTarget(const wxString& alias, const wxString& target);

        /** Request if a specific tree node can be dragged.
          *
          * @note Called by ProjectManager.
          * @return True if it is allowed to drag this node, false not.
          */
        bool CanDragNode(wxTreeCtrl* tree, wxTreeItemId node);

        /** Notify that a specific tree node has been dragged.
          *
          * @note Called by ProjectManager.
          * @return True if succeeded, false if not.
          */
        bool NodeDragged(wxTreeCtrl* tree, wxTreeItemId from, wxTreeItemId to);

        /** Notify that a virtual folder has been added.
          * @return True if it is allowed, false if not. */
        bool VirtualFolderAdded(wxTreeCtrl* tree, wxTreeItemId parent_node, const wxString& virtual_folder);

        /** Notify that a virtual folder has been deleted. */
        void VirtualFolderDeleted(wxTreeCtrl* tree, wxTreeItemId node);

        /** Notify that a virtual folder has been renamed.
          * @return True if the renaming is allowed, false if not. */
        bool VirtualFolderRenamed(wxTreeCtrl* tree, wxTreeItemId node, const wxString& new_name);

        /** Get a list of the virtual folders. Normally used by the project loader only.*/
        const wxArrayString& GetVirtualFolders() const;

        /** Set the virtual folders list. Normally used by the project loader only. */
        void SetVirtualFolders(const wxArrayString& folders);

        /** Returns the last modification time for the file. Used to detect modifications outside the Program. */
        wxDateTime GetLastModificationTime() const { return m_LastModified; }

        /** Sets the last modification time for the project to 'now'. Used to detect modifications outside the Program. */
        void Touch();

        /** Sets object names generation to extended/normal mode.
          *
          * In normal mode (the default), the file @c foo.cpp generates the @c foo.o object file.
          * In extended mode, the file @c foo.cpp generates the @c foo.cpp.o object file.
          *
          * This option is useful for large projects containing similarly named files
          * (in the same directory) differing only on their extensions. Using the
          * extended mode with said projects guarantees that each object name will
          * be unique.
          *
          * @param ext Set to true to switch to extended mode, false for normal mode.
          */
        void SetExtendedObjectNamesGeneration(bool ext);

        /** Gets object names generation mode (extended/normal).
          * @return True for extended mode, false for normal mode.
          * @see SetExtendedObjectNamesGeneration.
          */
        bool GetExtendedObjectNamesGeneration() const;

        /** Set notes on the project.
          *
          * @param notes Simple text notes about the project.
          */
        void SetNotes(const wxString& notes);

        /** Get notes on the project.
          *
          * @return Simple text notes about the project.
          */
        const wxString& GetNotes() const;

        /** Set show project notes on load automatically.
          *
          * @param show If true show project notes on load.
          */
        void SetShowNotesOnLoad(bool show);

        /** Get show project notes on load automatically.
          *
          * @return True if show project notes on load is set, false if not.
          */
        bool GetShowNotesOnLoad() const;

        /** Show project notes now.
          *
          * @param nonEmptyOnly If true, show notes only if non-empty.
          * @param editable If true, the notes will be editable.
          *
          * @note If @c editable is true, the @c nonEmptyOnly parameter is ignored...
          */
        void ShowNotes(bool nonEmptyOnly, bool editable = false);

        /** Changes project title
          *
          * This function overrides CompileTargetBase::SetTitle.
          * It sends additional notification event to plugins
          * and than calls base function.
          */
        virtual void SetTitle(const wxString& title);
        
        /** Access the \<Extensions\> XML node of this project
          *
          * This function is for advanced users only. Use at your own risk
          * (and respect other plugins authors work under this node).
          *
          * @return The \<Extensions\> XML node.
          * @note This function will never return NULL.
          */
        virtual TiXmlNode* GetExtensionsNode();
        
        /** Convenience function (mainly for scripts) to add nodes/attributes
          * under the \<Extensions\> node.
          *
          * It is mainly useful for scripts that can't otherwise access the XML node.
          * For C++ code, using GetExtensionsNode() is recommended instead (which is much faster).
          *
          * @param stringDesc A string representation of the nodes/attributes to add/edit
          * under \<Extensions\>.
          * @c stringDesc is a string of the form:
          * "node/subnode/.../+subnode:attr=val"
          *
          * The ":attr=val" part is optional.
          * The node/subnode structure will be created if not there.
          * To set more than one attribute, call this function more than once,
          * using the same node/subnode structure.
          * If a node begins with the "plus" sign (+), then this forces adding a new node
          * instead of re-using the existing one (if any).
          */
        virtual void AddToExtensions(const wxString& stringDesc);
        
        /** Internal use only.
          * Updates the internal hashmap of project files.
          */
        virtual void ProjectFileRenamed(ProjectFile* pf);

    private:
        void Open();
        void ExpandVirtualBuildTargetGroup(const wxString& alias, wxArrayString& result) const;
        wxTreeItemId AddTreeNode(wxTreeCtrl* tree, const wxString& text, const wxTreeItemId& parent, bool useFolders, FileTreeData::FileTreeDataKind folders_kind, bool compiles, int image, FileTreeData* data = 0L);
        wxTreeItemId FindNodeToInsertAfter(wxTreeCtrl* tree, const wxString& text, const wxTreeItemId& parent, bool in_folders); // alphabetical sorting
        ProjectBuildTarget* AddDefaultBuildTarget();
        int IndexOfBuildTargetName(const wxString& targetName) const;
        wxString CreateUniqueFilename();
        void NotifyPlugins(wxEventType type, const wxString& targetName = wxEmptyString, const wxString& oldTargetName = wxEmptyString);
        void CopyTreeNodeRecursively(wxTreeCtrl* tree, const wxTreeItemId& item, const wxTreeItemId& new_parent);

        // properties
        VirtualBuildTargetsMap m_VirtualTargets;
        BuildTargets m_Targets;
        wxString m_ActiveTarget;
        wxString  m_LastSavedActiveTarget;
        wxString m_DefaultExecuteTarget;
        wxString m_Makefile;
        bool m_CustomMakefile;

        FilesList m_Files;
        wxArrayString m_ExpandedNodes;
        bool m_Loaded;
        wxTreeItemId m_ProjectNode;

        wxArrayString m_VirtualFolders; // not saved, just used throughout cbProject's lifetime

        bool m_CurrentlyLoading;
        wxString m_CommonTopLevelPath;
        wxString m_BasePath;

        PCHMode m_PCHMode;

        // hashmap for fast searches in cbProject::GetFileByFilename()
        ProjectFiles m_ProjectFilesMap; // keeps UnixFilename(ProjectFile::relativeFilename)
        ProjectBuildTarget* m_CurrentlyCompilingTarget;

        wxDateTime m_LastModified;

        bool m_ExtendedObjectNamesGeneration;
        wxString m_Notes;
        bool m_AutoShowNotesOnLoad;
        
        // copy of <Extensions> element, in case certain plugins are disabled
        // so that the contents are not lost
        TiXmlElement* m_pExtensionsElement;
};

#endif // CBPROJECT_H


