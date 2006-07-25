#ifndef CBPROJECT_H
#define CBPROJECT_H

#include <wx/dynarray.h>
#include <wx/hashmap.h>
#include <wx/treectrl.h>

#include "settings.h"
#include "openfilestree.h"
#include "compiletargetbase.h"
#include "cbplugin.h"
#include "projectbuildtarget.h"

// forward decl
class cbProject;
class ProjectBuildTarget;
class ProjectFile;
class FilesGroupsAndMasks;

// hashmap for fast searches in cbProject::GetFileByFilename()
WX_DECLARE_STRING_HASH_MAP(ProjectFile*, ProjectFiles);

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
            ftdkVirtualGroup
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
        bool IsLoaded(){ return m_Loaded; }

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
        wxString GetCommonTopLevelPath();

        /** @return True if the project is modified in any way. */
        bool GetModified();

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

        /** @return The build target index which will be pre-selected when the "Select target"
          * dialog appears when running the project. Valid only for multi-target projects. */
        int GetDefaultExecuteTargetIndex();

        /** Set the build target index which will be pre-selected when the "Select target"
          * dialog appears when running the project.
          * @param index The build target index. Must be equal or greater than zero and less than GetBuildTargetsCount().
          */
        void SetDefaultExecuteTargetIndex(int index);

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

        /** Set the active build target. Mainly used by the compiler.
          * @param index The build target index to set as active.
          * @return True if @c index was valid, false if not.
          */
        bool SetActiveBuildTarget(int index);

        /** @return The active build target index. */
        int GetActiveBuildTarget();

        /** @return The mode precompiled headers are handled. */
        PCHMode GetModeForPCH(){ return m_PCHMode; }

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
    private:
        void Open();
        wxTreeItemId AddTreeNode(wxTreeCtrl* tree, const wxString& text, const wxTreeItemId& parent, bool useFolders, bool compiles, int image, FileTreeData* data = 0L);
        ProjectBuildTarget* AddDefaultBuildTarget();
        int IndexOfBuildTargetName(const wxString& targetName);
        wxString CreateUniqueFilename();
        void NotifyPlugins(wxEventType type);

        // properties
        BuildTargets m_Targets;
        int m_ActiveTarget;
        int m_LastSavedActiveTarget;
        int m_DefaultExecuteTarget;
        wxString m_Makefile;
        bool m_CustomMakefile;

        FilesList m_Files;
        wxArrayString m_ExpandedNodes;
        bool m_Loaded;
        wxTreeItemId m_ProjectNode;

        bool m_CurrentlyLoading;
        wxString m_CommonTopLevelPath;
        wxString m_BasePath;

        PCHMode m_PCHMode;

        // hashmap for fast searches in cbProject::GetFileByFilename()
        ProjectFiles m_ProjectFilesMap; // keeps UnixFilename(ProjectFile::relativeFilename)
        ProjectBuildTarget* m_CurrentlyCompilingTarget;
};

#endif // CBPROJECT_H

