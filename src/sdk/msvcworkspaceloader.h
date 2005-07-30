#ifndef MSVCWORKSPACELOADER_H
#define MSVCWORKSPACELOADER_H

#include "ibaseworkspaceloader.h"
#if wxCHECK_VERSION(2, 5, 0)
    #include <wx/arrstr.h>
#endif
#include <wx/hashmap.h>

class cbProject;

class MSVCWorkspaceLoader : public IBaseWorkspaceLoader
{
    public:
		MSVCWorkspaceLoader();
		virtual ~MSVCWorkspaceLoader();

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

        void initDependencies(cbProject* project);
        void addDependency(cbProject*, const wxString& depProject);
        void resolveDependencies();
        
        HashProjdeps _projdeps;

	private:
};

#endif // MSVCWORKSPACELOADER_H
