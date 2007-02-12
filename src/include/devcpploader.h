#ifndef DEVCPPLOADER_H
#define DEVCPPLOADER_H

#include "ibaseloader.h"

// forward decls
class cbProject;

class DLLIMPORT DevCppLoader : public IBaseLoader
{
	public:
		DevCppLoader(cbProject* project);
		virtual ~DevCppLoader();
		
		bool Open(const wxString& filename);
		bool Save(const wxString& filename);
	protected:
        cbProject* m_pProject;
	private:
        DevCppLoader(){} // no default ctor
};

#endif // DEVCPPLOADER_H

