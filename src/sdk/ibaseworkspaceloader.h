#ifndef IBASEWORKSPACELOADER_H
#define IBASEWORKSPACELOADER_H

#include <wx/string.h>
#include <wx/intl.h>

class IBaseWorkspaceLoader
{
	public:
		IBaseWorkspaceLoader(){}
		virtual ~IBaseWorkspaceLoader(){}

        virtual bool Open(const wxString& filename) = 0;
        virtual bool Save(const wxString& title, const wxString& filename) = 0;
        virtual wxString GetTitle(){ return !m_Title.IsEmpty() ? m_Title : wxString(_("Default workspace")); }
	protected:
        wxString m_Title;
	private:
};

#endif // IBASEWORKSPACELOADER_H
