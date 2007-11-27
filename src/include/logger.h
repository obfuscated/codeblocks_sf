#ifndef LOGGER_H
#define LOGGER_H

#include <wx/string.h>

class wxWindow;

namespace
{
    static wxString temp_string(_T('\0'), 250);
    static wxString newline_string(_T("\n"));
}

class DLLIMPORT Logger
{
public:
	enum level { caption, info, warning, success, error, critical, spacer };
	enum { num_levels = spacer +1 };

    Logger() {};
    virtual ~Logger() {};


	/* Logger writers:
	*  This is the One Function you must implement. Everything else is optional or bull.
	*  It must be possible to call this function in presence and in absence of GUI without crashing the application.
	*  It is not necessary to provide any actual output at all times, but it must be 100% safe to call this function at all times.
	*  You may not throw from this function, it must return in finite time, and it must not call logging functions (to prevent infinite recursion).
	*  Other than that, you can do anything you want with the log messages that you receive.
	*/
    virtual void Append(const wxString& msg, Logger::level lv = info) = 0;


    virtual void Clear(){};
    virtual void CopyContentsToClipboard(bool selectionOnly = false) {};

    virtual void UpdateSettings() {};
	virtual wxWindow* CreateControl(wxWindow* parent) { return 0; };
};



class DLLIMPORT NullLogger : public Logger
{
public:
    virtual void Append(const wxString& msg, Logger::level lv){};
};

// for more built-in loggers, check loggers.h

#endif // LOGGER_H
