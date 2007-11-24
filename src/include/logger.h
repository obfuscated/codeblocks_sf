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

    virtual void UpdateSettings() {};

    virtual void Append(const wxString& msg, Logger::level lv = info) = 0;
    virtual void Clear() = 0;
    virtual bool IsEmpty() const{ return false; }
    virtual void CopyContentsToClipboard(bool selectionOnly = false){}

	virtual wxWindow* CreateControl(wxWindow* parent) { return 0; };
};



class DLLIMPORT NullLogger : public Logger
{
public:
    virtual void Append(const wxString& msg, Logger::level lv){};
    virtual void Clear(){};
};

// for more built-in loggers, check loggers.h

#endif // LOGGER_H
