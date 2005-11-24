#ifndef IBASELOADER_H
#define IBASELOADER_H

#include <wx/string.h>
#include "settings.h"
#include "tinyxml/tinyxml.h"

class DLLIMPORT IBaseLoader
{
	public:
		IBaseLoader(){}
		virtual ~IBaseLoader(){}

        virtual bool Open(const wxString& filename) = 0;
        virtual bool Save(const wxString& filename) = 0;    protected:
};

#endif // IBASELOADER_H
