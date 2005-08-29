#ifndef MSVC7WORKSPACELOADER_H
#define MSVC7WORKSPACELOADER_H

#include "ibaseworkspaceloader.h"
#include "msvcworkspacebase.h"

class MSVC7WorkspaceLoader : public IBaseWorkspaceLoader, public MSVCWorkspaceBase
{
	public:
		MSVC7WorkspaceLoader();
		virtual ~MSVC7WorkspaceLoader();

        bool Open(const wxString& filename);
        bool Save(const wxString& title, const wxString& filename);

	protected:
        // workspace version, i.e. "7.00" or "8.00"
        wxString _version;
};

#endif // MSVC7WORKSPACELOADER_H
