#ifndef PROJECTBUILDTARGET_H
#define PROJECTBUILDTARGET_H

#include "settings.h"
#include "globals.h"
#include "compiletargetbase.h"
#include <wx/dynarray.h>
#include <wx/filename.h>

class cbProject;

WX_DEFINE_ARRAY(DebuggerBreakpoint*, BreakpointsList);

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
		ProjectBuildTarget();
		// class destructor
		~ProjectBuildTarget();

        //properties
        virtual const wxString& GetExternalDeps();
        virtual void SetExternalDeps(const wxString& deps);
        virtual bool GetIncludeInTargetAll();
        virtual void SetIncludeInTargetAll(bool buildIt);
        virtual bool GetCreateDefFile();
        virtual void SetCreateDefFile(bool createIt);
        virtual bool GetCreateStaticLib();
        virtual void SetCreateStaticLib(bool createIt);
    private:
        wxString m_ExternalDeps;
        bool m_BuildWithAll;
        bool m_CreateStaticLib;
        bool m_CreateDefFile;
};

#endif // PROJECTBUILDTARGET_H

