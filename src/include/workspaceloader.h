#ifndef WORKSPACELOADER_H
#define WORKSPACELOADER_H

#include "ibaseworkspaceloader.h"

class WorkspaceLoader : public IBaseWorkspaceLoader
{
	public:
		WorkspaceLoader();
		virtual ~WorkspaceLoader();

        bool Open(const wxString& filename, wxString& Title);
        bool Save(const wxString& title, const wxString& filename);
};

#endif // WORKSPACELOADER_H
