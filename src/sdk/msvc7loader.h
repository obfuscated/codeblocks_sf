#ifndef MSVC7LOADER_H
#define MSVC7LOADER_H

#include "ibaseloader.h"

// forward decls
class cbProject;

class MSVC7Loader : public IBaseLoader
{
	public:
		MSVC7Loader(cbProject* project);
		virtual ~MSVC7Loader();

		bool Open(const wxString& filename);
		bool Save(const wxString& filename);
	protected:
        cbProject* m_pProject;
        bool m_ConvertSwitches;
	private:
        bool DoSelectConfiguration(TiXmlElement* root);
        bool DoImport(TiXmlElement* conf);
        bool DoImportFiles(TiXmlElement* root);
};

#endif // MSVC7LOADER_H
