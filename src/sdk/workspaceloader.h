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
        bool Save(const wxString& title, const wxString& filename);
        wxString GetTitle(){ return m_Title; }
	protected:
	private:
        cbProject* m_pActiveProj;
        wxString m_Title;
};

#endif // WORKSPACELOADER_H

