#ifndef IBASEWORKSPACELOADER_H
#define IBASEWORKSPACELOADER_H

class wxString;

class IBaseWorkspaceLoader
{
	public:
		IBaseWorkspaceLoader(){}
		virtual ~IBaseWorkspaceLoader(){}

        virtual bool Open(const wxString& filename, wxString& Title) = 0;
        virtual bool Save(const wxString& title, const wxString& filename) = 0;
};

#endif // IBASEWORKSPACELOADER_H
