#ifndef PROJECTLAYOUTLOADER_H
#define PROJECTLAYOUTLOADER_H

#include <wx/string.h>

class cbProject;

class DLLIMPORT ProjectLayoutLoader
{
	public:
		ProjectLayoutLoader(cbProject* project);
		virtual ~ProjectLayoutLoader();

        bool Open(const wxString& filename);
        bool Save(const wxString& filename);
        
        ProjectFile* GetTopProjectFile(){ return m_TopProjectFile; }
	protected:
	private:
        cbProject* m_pProject;
        ProjectFile* m_TopProjectFile;
};

#endif // PROJECTLAYOUTLOADER_H

