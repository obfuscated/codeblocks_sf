#ifndef SIMPLELISTLOG_H
#define SIMPLELISTLOG_H

#include "settings.h"
#include "messagelog.h" // inheriting class's header file
#include <wx/listctrl.h>

/*
 * No description
 */
class DLLIMPORT SimpleListLog : public MessageLog
{
	public:
		// class constructor
		SimpleListLog(wxNotebook* parent, const wxString& title);
		SimpleListLog(wxNotebook* parent, const wxString& title, int numCols, int widths[], const wxArrayString& titles);
		// class destructor
		~SimpleListLog();

		virtual void AddLog(const wxString& msg, bool addNewLine = true);
		wxListCtrl* GetListControl(){ return m_pList; }
		
		void SetColumns(int numCols, int widths[], const wxArrayString& titles);
		void AddLog(const wxArrayString& values);
		void Clear();
    protected:
        wxListCtrl* m_pList;
    private:
		void Init(const wxString& title);
};

#endif // SIMPLELISTLOG_H
