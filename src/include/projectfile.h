#ifndef PROJECTFILE_H
#define PROJECTFILE_H

#include "settings.h"
#include "globals.h"
#include <wx/dynarray.h>
#include <wx/filename.h>
#include <wx/list.h>
#include <wx/treectrl.h>

#include "blockallocated.h"

class cbProject;
class ProjectBuildTarget;
class pfDetails;

WX_DECLARE_HASH_MAP(ProjectBuildTarget*, pfDetails*, wxPointerHash, wxPointerEqual, PFDMap);

struct pfCustomBuild
{
    pfCustomBuild() : useCustomBuildCommand(false) {}
    wxString buildCommand;
    bool useCustomBuildCommand;
};
WX_DECLARE_HASH_MAP(wxString, pfCustomBuild, wxStringHash, wxStringEqual, pfCustomBuildMap);

/** Represents a file in a Code::Blocks project. */
class ProjectFile  : public BlockAllocated<ProjectFile, 1000>
{
    public:
        /// Constructor
        ProjectFile(cbProject* prj);
        /// Destructor
        ~ProjectFile();

        /** Make this file belong to an additional build target.
          * @param targetName The build target to add this file to. */
        void AddBuildTarget(const wxString& targetName);

        /** Rename a build target this file belongs in.
          * @param oldTargetName The build target's old name.
          * @param newTargetName The build target's new name.
          * @note This does *not* change the build target's name, just the reference in the project file.
          * This is actually used by cbProject::RenameBuildTarget(). */
        void RenameBuildTarget(const wxString& oldTargetName, const wxString& newTargetName);

        /** Remove this file from the specified build target.
          * @param targetName The build target's name to remove this file from. */
        void RemoveBuildTarget(const wxString& targetName);

        /** Show the file properties dialog.
          * @param parent The parent window for the dialog (can be NULL).
          * @return True if the user closed the dialog with "OK", false if closed it with "Cancel".
          */
        bool ShowOptions(wxWindow* parent);

        // take as example the relative file sdk/cbProject.cpp
        /** @return The relative (to the project) filename without extension. */
        wxString GetBaseName() const; // returns sdk/cbProject

        /** @return The generated object filename. */
        const wxString& GetObjName(); // returns sdk/cbProject.o

        /** Set the generated object filename.
          * @param name The filename for the generated object. */
        void SetObjName(const wxString& name);

        /** @return The parent project. */
        cbProject* GetParentProject(){ return project; }

        /** This is called automatically when adding/removing build targets.
          * @param target A pointer to the build target whose file details should be updated. */
        void UpdateFileDetails(ProjectBuildTarget* target = 0);

        /** Access the file details for this project file for the specified target.
          * @param target A pointer to the build target whose file details should be updated.
          * @return The details for this project file for the specified build target. */
        const pfDetails& GetFileDetails(ProjectBuildTarget* target);

        /** Set the visual state (modified, read-only, etc).
          * @param state The new visual state. */
		void SetFileState(FileVisualState state);

        /** @return The visual state (modified, read-only, etc). */
		FileVisualState GetFileState() const;

        /** The full filename of this file. Usually you need to read from it and never write to it.
          * @note If you set this anywhere in code,
          * you *MUST* call UpdateFileDetails() afterwards or it won't compile anymore
          */
        wxFileName file;

        /** The relative (to the project) filename of this file. Usually you need to read from it and never write to it.
          * @note If you set this anywhere in code,
          * you *MUST* call UpdateFileDetails() afterwards or it won't compile anymore
          */
        wxString relativeFilename;

        /** The relative filename to the common top-level path.
          * This is used mainly for the tree, as this is guaranteed to not contain '..' */
        wxString relativeToCommonTopLevelPath;

        /** Compile flag. If it's true, the file is compiled (generates object file) else it is not. */
        bool compile;

        /** Link flag. If it's true, the (generated object) file is linked else it is not. */
        bool link;

        /** The weight. This is a number between 0 and 100 (defaults to 50).
          * Files with smaller weights are compiled earlier than those with larger weights. */
        unsigned short int weight;

        /** If true, the file is open inside an editor. */
        bool editorOpen; // layout info

        /** The last known caret position in an editor for this file. */
        int editorPos; // layout info

        /** The last known caret line in an editor for this file. */
        int editorTopLine; // layout info

		/** The position of the editor-tab for this file. */
		int editorTabPos; // layout info

        /** A map for custom builds. Key is compiler ID, value is pfCustomBuild struct. */
        pfCustomBuildMap customBuild;

        /** The compiler variable used for this file (e.g CPP, CC, etc). */
        wxString compilerVar;

        /** An array of strings, containing the names of all the build targets this file belongs to. */
        wxArrayString buildTargets;

        /** A string that represents the virtual folder this file will appear in.
          * This is a relative path which doesn't have to exist in the filesystem
          * hierarchy. */
        wxString virtual_path;
    protected:
        friend class cbProject;
        void DoUpdateFileDetails(ProjectBuildTarget* target);
        cbProject* project;
        FileVisualState m_VisualState;
        wxTreeItemId m_TreeItemId; // set by the project when building the tree
        wxString m_ObjName;
        PFDMap m_PFDMap;
};
WX_DECLARE_LIST(ProjectFile, FilesList);

/** This is a helper class that caches various filenames for one ProjectFile.
  * These include the source filename, the generated object filename,
  * relative and absolute versions of the above, etc.
  * Mainly used by the compiler...
  */
class pfDetails : public BlockAllocated<pfDetails, 1000>
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
        wxString object_file_flat;
        // those below, have no UnixFilename() applied, nor QuoteStringIfNeeded()
        wxString source_file_native;
        wxString object_file_native;
        wxString dep_file_native;
        wxString object_dir_native;
        wxString dep_dir_native;
        wxString source_file_absolute_native;
        wxString object_file_absolute_native;
        wxString object_file_flat_absolute_native;
        wxString dep_file_absolute_native;
        wxString object_file_flat_native;
};

#endif // PROJECTFILE_H
