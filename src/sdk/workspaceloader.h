#ifndef WORKSPACELOADER_H
#define WORKSPACELOADER_H

#include <wx/string.h>

class cbProject;

class WorkspaceLoader
{
	public:
		WorkspaceLoader();
		virtual ~WorkspaceLoader();

        bool Open(const wxString& filename);
        bool Save(const wxString& filename);
	protected:
	private:
        cbProject* m_pActiveProj;
};

#endif // WORKSPACELOADER_H

