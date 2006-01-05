#ifndef SIMPLETEXTLOG_H
#define SIMPLETEXTLOG_H

#include "settings.h"
#include "messagelog.h" // inheriting class's header file
#include <wx/textctrl.h>

/*
 * No description
 */
class DLLIMPORT SimpleTextLog : public MessageLog
{
	public:
		// class constructor
		SimpleTextLog();
		// class destructor
		~SimpleTextLog();

		virtual void AddLog(const wxString& msg, bool addNewLine = true);
		wxTextCtrl* GetTextControl(){ return m_Text; }
    private:
        wxTextCtrl* m_Text;
};

#endif // SIMPLETEXTLOG_H

