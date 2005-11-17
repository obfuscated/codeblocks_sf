#ifndef DEVPROJECT_H
#define DEVPROJECT_H

#include <wx/dynarray.h>
#include <wx/hashmap.h>
#include <wx/treectrl.h>

#include "settings.h"
#include "openfilestree.h"
#include "compiletargetbase.h"
#include "projectbuildtarget.h"
#include "cbplugin.h"

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
        FileTreeData(cbProject* project, int index = -1){ m_Project = project; m_Index = index; }
        int GetFileIndex(){ return m_Index; }
        void SetFileIndex(int index){ m_Index = index; }
        cbProject* GetProject(){ return m_Project; }
        void SetProject(cbProject* project){ m_Project = project; }
    private:
        int m_Index;
        cbProject* m_Project;
};

enum PCHMode
{
    pchSourceDir = 0, // in a dir on the same level as the source header (default)
    pchObjectDir, // in the objects output dir, along with the other object files
    pchSourceFile, // in a file alongside the source header (with .gch appended)
};

/*
 * No description
 */
class DLLIMPORT cbProject : public CompileTargetBase
{
 	public:
		// class constructor
		cbProject(const wxString& filename = wxEmptyString);
		// class destructor
		~cbProject();

        bool IsLoaded(){ return m_Loaded; }
        void BuildTree(wxTreeCtrl* tree, const wxTreeItemId& root, bool categorize, bool useFolders, FilesGroupsAndMasks* fgam = 0L);
        void ClearAllProperties();

        void CalculateCommonTopLevelPath();
        wxString GetCommonTopLevelPath();

        // properties
		bool GetModified();
		void SetModified(bool modified);

        ProjectFile* GetFile(int index);
        ProjectFile* GetFileByFilename(const wxString& filename, bool isRelative = true, bool isUnixFilename = false);
        int GetFilesCount(){ return m_Files.GetCount(); }
        void SetMakefile(const wxString& makefile){ m_Makefile = makefile; SetModified(true); }
        const wxString& GetMakefile();
        void SetMakefileCustom(bool custom);
        bool IsMakefileCustom(){ return m_CustomMakefile; }
        int GetDefaultExecuteTargetIndex();
        void SetDefaultExecuteTargetIndex(int index);
        int GetBuildTargetsCount(){ return m_Targets.GetCount(); }
        ProjectBuildTarget* GetBuildTarget(int index);
        ProjectBuildTarget* GetBuildTarget(const wxString& targetName);
        ProjectBuildTarget* AddBuildTarget(const wxString& targetName);
        bool RenameBuildTarget(int index, const wxString& targetName);
        bool RenameBuildTarget(const wxString& oldTargetName, const wxString& newTargetName);
        bool RemoveBuildTarget(int index);
        bool RemoveBuildTarget(const wxString& targetName);
        void ReOrderTargets(const wxArrayString& nameOrder);
		bool SetActiveBuildTarget(int target);
		int GetActiveBuildTarget();

        // how are we gonna handle precompiled headers?
        PCHMode GetModeForPCH(){ return m_PCHMode; }
        void SetModeForPCH(PCHMode mode){ m_PCHMode = mode; SetModified(true); }

		const wxArrayString& ExpandedNodes(){ return m_ExpandedNodes; }
		void AddExpandedNode(const wxString& path){ m_ExpandedNodes.Add(path); }

        void SetCompilerIndex(int compilerIdx); // overriden

        wxTreeItemId GetProjectNode(){ return m_ProjectNode; }
        bool QueryCloseAllFiles();
        bool CloseAllFiles(bool dontsave=false);
        bool SaveAllFiles();
        bool Save();
        bool SaveAs();
		bool SaveLayout();
		bool LoadLayout();
        ProjectFile* AddFile(const wxString& targetName, const wxString& filename, bool compile = true, bool link = true, unsigned short int weight = 50);
        ProjectFile* AddFile(int targetIndex, const wxString& filename, bool compile = true, bool link = true, unsigned short int weight = 50);
        bool RemoveFile(int index);
        bool ShowOptions();
        void SaveTreeState(wxTreeCtrl* tree);
        void RestoreTreeState(wxTreeCtrl* tree);
		int SelectTarget(int initial = -1, bool evenIfOne = false);
        void RenameInTree(const wxString &newname);
        ProjectBuildTarget *GetCurrentlyCompilingTarget(){return m_CurrentlyCompilingTarget;};

        // This function is for internal use by compilers only.
        // Using this function outside in any other place results in undefined behaviour!
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

#endif // DEVPROJECT_H

