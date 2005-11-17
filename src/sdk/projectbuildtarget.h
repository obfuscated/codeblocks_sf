#ifndef PROJECTBUILDTARGET_H
#define PROJECTBUILDTARGET_H

#include "settings.h"
#include "globals.h"
#include "compiletargetbase.h"
#include <wx/dynarray.h>
#include <wx/filename.h>
#include <wx/list.h>
#include <wx/treectrl.h>

#include "blockallocated.h"

class cbProject;
class ProjectBuildTarget;
class pfDetails;

WX_DEFINE_ARRAY(ProjectBuildTarget*, BuildTargets);
WX_DECLARE_HASH_MAP(ProjectBuildTarget*, pfDetails*, wxPointerHash, wxPointerEqual, PFDMap);

class ProjectFile  : public BlockAllocated<ProjectFile, 500>
{
    public:
        ProjectFile(cbProject* prj);
        ~ProjectFile();

        void AddBuildTarget(const wxString& targetName);
        void RenameBuildTarget(const wxString& oldTargetName, const wxString& newTargetName);
        void RemoveBuildTarget(const wxString& targetName);
        bool ShowOptions(wxWindow* parent);

        // take as example the relative file sdk/cbProject.cpp
        wxString GetBaseName(); // returns sdk/cbProject
        const wxString& GetObjName(); // returns sdk/cbProject.o
        void SetObjName(const wxString& name);

        cbProject* project;

        /// @note: if you set 'file' or 'relativeFilename' anywhere in code,
        ///       you *MUST* call UpdateFileDetails() afterwards or it won't compile anymore
        wxFileName file;
        wxString relativeFilename;
        /// This is called automatically when adding/removing build targets
        void UpdateFileDetails(ProjectBuildTarget* target = 0);
        /// Retrieve the details for this project file for the specified build target
        const pfDetails& GetFileDetails(ProjectBuildTarget* target);

        /// Set the visual state (modified, read-only, etc)
		void SetFileState(FileVisualState state);
        /// Get the visual state (modified, read-only, etc)
		FileVisualState GetFileState();

        wxString relativeToCommonTopLevelPath; // used for the tree, .objs and .deps (has no "..")
        bool compile;
        bool link;
        unsigned short int weight; // files acn be sorted by their weight and a
                    // compiler can compile them in this sorted order
                    // weight ranges from 0 to 100, default is 50
        bool editorOpen; // layout info
        int editorPos; // layout info
        int editorTopLine; // layout info
        wxString buildCommand;
        bool useCustomBuildCommand;
        bool autoDeps;
        wxString customDeps;
        wxString compilerVar;
        wxArrayString buildTargets;
    private:
        friend class cbProject;
        void DoUpdateFileDetails(ProjectBuildTarget* target);
        FileVisualState m_VisualState;
        wxTreeItemId m_TreeItemId; // set by the project when building the tree
        wxString m_ObjName;
        PFDMap m_PFDMap;
};
WX_DECLARE_LIST(ProjectFile, FilesList);

// ProjectFile details
class pfDetails
{
    public:
        pfDetails(ProjectBuildTarget* target, ProjectFile* pf);
        void Update(ProjectBuildTarget* target, ProjectFile* pf);
        // all the members below, are set in the constructor
        wxString source_file;
        wxString object_file;
        wxString dep_file;
        wxString object_dir;
        wxString dep_dir;
        // those below, have no UnixFilename() applied, nor QuoteStringIfNeeded()
        wxString source_file_native;
        wxString object_file_native;
        wxString dep_file_native;
        wxString object_dir_native;
        wxString dep_dir_native;
        wxString source_file_absolute_native;
        wxString object_file_absolute_native;
        wxString dep_file_absolute_native;
};

/*
 * No description
 */
class DLLIMPORT ProjectBuildTarget : public CompileTargetBase
{
	public:
		// class constructor
		ProjectBuildTarget(cbProject* parentProject);
		// class destructor
		~ProjectBuildTarget();

        virtual cbProject* GetParentProject();
        virtual wxString GetFullTitle(); // returns "projectname - targetname"

        //properties
        virtual const wxString& GetExternalDeps();
        virtual void SetExternalDeps(const wxString& deps);
        virtual const wxString& GetAdditionalOutputFiles();
        virtual void SetAdditionalOutputFiles(const wxString& files);
        virtual bool GetIncludeInTargetAll();
        virtual void SetIncludeInTargetAll(bool buildIt);
        virtual bool GetCreateDefFile();
        virtual void SetCreateDefFile(bool createIt);
        virtual bool GetCreateStaticLib();
        virtual void SetCreateStaticLib(bool createIt);
        virtual bool GetUseConsoleRunner();
        virtual void SetUseConsoleRunner(bool useIt);

        virtual void SetTargetType(const TargetType& pt); // overriden

        // target dependencies: targets to be compiled (if necessary) before this one
        // add a target to the list of dependencies of this target. Be careful
        // not to add a target more than once
        virtual void AddTargetDep(ProjectBuildTarget* target);
        // get the list of dependency targets of this target
        virtual BuildTargets& GetTargetDeps();

    private:
        cbProject* m_Project;
        wxString m_ExternalDeps;
        wxString m_AdditionalOutputFiles;
        BuildTargets m_TargetDeps;
        bool m_BuildWithAll;
        bool m_CreateStaticLib;
        bool m_CreateDefFile;
        bool m_UseConsoleRunner;
};

#endif // PROJECTBUILDTARGET_H

