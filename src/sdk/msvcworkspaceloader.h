#ifndef MSVCWORKSPACELOADER_H
#define MSVCWORKSPACELOADER_H

#include "ibaseworkspaceloader.h"

class MSVCWorkspaceLoader : public IBaseWorkspaceLoader
{
	public:
		MSVCWorkspaceLoader();
		virtual ~MSVCWorkspaceLoader();

        bool Open(const wxString& filename);
        bool Save(const wxString& title, const wxString& filename);
	protected:
	private:
};

#endif // MSVCWORKSPACELOADER_H
