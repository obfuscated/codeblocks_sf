#ifndef MSVCLOADER_H
#define MSVCLOADER_H

#include "ibaseloader.h"

// forward decls
class cbProject;

class MSVCLoader : public IBaseLoader
{
	public:
		MSVCLoader(cbProject* project);
		virtual ~MSVCLoader();
		
		bool Open(const wxString& filename);
		bool Save(const wxString& filename);
	protected:
        bool ProcessContents();
        void ProcessCompilerOptions(const wxString& opts);
        void ProcessLinkerOptions(const wxString& opts);
        wxString RemoveQuotes(const wxString& src);
        cbProject* m_pProject;
        wxString m_FileContents;
	private:
};

#endif // MSVCLOADER_H

