#ifndef MSVC7WORKSPACELOADER_H
#define MSVC7WORKSPACELOADER_H

#include "ibaseworkspaceloader.h"
#include "msvcworkspacebase.h"

class MSVC7WorkspaceLoader : public IBaseWorkspaceLoader, public MSVCWorkspaceBase
{
	public:
		MSVC7WorkspaceLoader();
		virtual ~MSVC7WorkspaceLoader();

        bool Open(const wxString& filename, wxString& Title);
        bool Save(const wxString& title, const wxString& filename);
};

#endif // MSVC7WORKSPACELOADER_H
