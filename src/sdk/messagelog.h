#ifndef MESSAGELOG_H
#define MESSAGELOG_H

#include "settings.h"
#include <wx/panel.h>
#include <wx/notebook.h>

/*
 * No description
 */
class DLLIMPORT MessageLog : public wxPanel
{
	public:
		// class destructor
		virtual ~MessageLog();

        virtual void CreateLog(const wxString& title);
		virtual void AddLog(const wxString& msg, bool addNewLine = true) = 0;
		int GetPageIndex(){ return m_PageIndex; }
    protected:
		// class constructor
		MessageLog(wxNotebook* parent);

    private:
        wxNotebook* m_Parent;
        int m_PageIndex;
};

#endif // MESSAGELOG_H

