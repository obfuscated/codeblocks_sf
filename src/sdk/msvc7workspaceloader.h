#ifndef MSVC7WORKSPACELOADER_H
#define MSVC7WORKSPACELOADER_H

#include "ibaseworkspaceloader.h"

class MSVC7WorkspaceLoader : public IBaseWorkspaceLoader
{
	public:
		MSVC7WorkspaceLoader();
		virtual ~MSVC7WorkspaceLoader();

        bool Open(const wxString& filename);
        bool Save(const wxString& title, const wxString& filename);
	protected:
	private:
};

#endif // MSVC7WORKSPACELOADER_H
