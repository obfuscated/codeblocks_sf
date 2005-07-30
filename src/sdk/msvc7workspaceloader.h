#ifndef MSVC7WORKSPACELOADER_H
#define MSVC7WORKSPACELOADER_H

#include "ibaseworkspaceloader.h"
#if wxCHECK_VERSION(2, 5, 0)
    #include <wx/arrstr.h>
#endif
#include <wx/hashmap.h>

class cbProject;

class MSVC7WorkspaceLoader : public IBaseWorkspaceLoader
{
	public:
		MSVC7WorkspaceLoader();
		virtual ~MSVC7WorkspaceLoader();

        bool Open(const wxString& filename);
        bool Save(const wxString& title, const wxString& filename);
	protected:
        struct ProjDeps {
            cbProject* _project;
            wxArrayString _deps;
            ProjDeps() : _project(0) {}    
            ProjDeps(cbProject* project) : _project(project) {}
        };

        WX_DECLARE_STRING_HASH_MAP(ProjDeps, HashProjdeps);

        void initDependencies(cbProject* project, const wxString& idcode);
        void addDependency(const wxString& projIdcode, const wxString& depIdcode);
        void resolveDependencies();
        
        HashProjdeps _projdeps;

	private:
};

#endif // MSVC7WORKSPACELOADER_H
