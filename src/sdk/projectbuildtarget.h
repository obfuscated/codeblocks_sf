#ifndef PROJECTBUILDTARGET_H
#define PROJECTBUILDTARGET_H

#include "settings.h"
#include "globals.h"
#include "compiletargetbase.h"
#include <wx/dynarray.h>
#include <wx/filename.h>
#include <wx/list.h>

class cbProject;
class ProjectBuildTarget;

WX_DEFINE_ARRAY(DebuggerBreakpoint*, BreakpointsList);
WX_DEFINE_ARRAY(ProjectBuildTarget*, BuildTargets);

class ProjectFile
{
    public:
        ProjectFile();
        ~ProjectFile();
        
        void AddBuildTarget(const wxString& targetName);
        void RenameBuildTarget(const wxString& oldTargetName, const wxString& newTargetName);
        void RemoveBuildTarget(const wxString& targetName);
        bool ShowOptions(wxWindow* parent);
        void ClearBreakpoints();
        DebuggerBreakpoint* HasBreakpoint(int line);
        void SetBreakpoint(int line);
        void RemoveBreakpoint(int line);
        void ToggleBreakpoint(int line);

        // take as example the relative file sdk/cbProject.cpp
        wxString GetBaseName(); // returns sdk/cbProject
        const wxString& GetObjName(); // returns sdk/cbProject.o
        void SetObjName(const wxString& name);

        cbProject* project;
        wxFileName file;
        wxString relativeFilename;
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
        BreakpointsList breakpoints;
        wxArrayString buildTargets;
    private:
        wxString m_ObjName;
};
WX_DECLARE_LIST(ProjectFile, FilesList);

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

