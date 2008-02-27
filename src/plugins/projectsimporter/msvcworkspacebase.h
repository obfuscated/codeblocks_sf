/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef MSVCWORKSPACEBASE_H
#define MSVCWORKSPACEBASE_H

#include <wx/version.h>
#include <wx/arrstr.h>
#include <wx/hashmap.h>

class cbProject;

// common features for MSVC importers, version 6, 7 etc.
// note that there's no workspace config in MSVC6...
class MSVCWorkspaceBase {
public:
    MSVCWorkspaceBase();
    virtual ~MSVCWorkspaceBase();

protected:
    // register a new project in the project array
    virtual void registerProject(const wxString& projectID, cbProject* project);
    // add a dependency
    virtual void addDependency(const wxString& projectID, const wxString& dependencyID);
    // add a workspace config
    virtual void addWorkspaceConfiguration(const wxString& config);
    // add a matching between a workspace configuration and a project configuration
    virtual void addConfigurationMatching(const wxString& projectID, const wxString& workspConfig, const wxString& projConfig);
    // update cbProjects and cbWorkspace once parsing done
    virtual void updateProjects();

private:
    // hash for configurations matchings
    WX_DECLARE_STRING_HASH_MAP(wxString, ConfigurationMatchings);
    // project record while parsing
    struct ProjectRecord {
        // current project data structure
        cbProject* _project;
        // list of the IDs of projects that the current project depend on
        wxSortedArrayString _dependencyList;
        // which project-config each solution-config correspond to
        ConfigurationMatchings _configurations;
        // default constructor
        ProjectRecord() : _project(0) {}
        // constructor with a project data structure
        ProjectRecord(cbProject* project) : _project(project) {}
    };

    // an associative array of project records indexed by project ID which are:
    // UUIDs in MSVC7, like "{35AFBABB-DF05-43DE-91A7-BB828A874015}" (with brackets, no quotes)
    // Project names in MSVC6
    WX_DECLARE_STRING_HASH_MAP(ProjectRecord, HashProjects);
    // list of projects records
    HashProjects _projects;
    // build configurations for the workspace (i.e. solution)
    wxSortedArrayString _workspaceConfigurations;
};

#endif // MSVCWORKSPACEBASE_H

