#ifndef WORKSPACELOADER_H
#define WORKSPACELOADER_H

#include "ibaseworkspaceloader.h"

class cbProject;

class WorkspaceLoader : public IBaseWorkspaceLoader
{
	public:
		WorkspaceLoader();
		virtual ~WorkspaceLoader();

        bool Open(const wxString& filename);
        bool Save(const wxString& title, const wxString& filename);
	protected:
	private:
};

#endif // WORKSPACELOADER_H
