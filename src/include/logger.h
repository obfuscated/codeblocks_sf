#ifndef Log_H
#define Log_H

#include "settings.h"
#include <wx/panel.h>
#include <wx/font.h>

#include <wx/string.h>

#include <stdio.h>

class wxWindow;

namespace
{
    static wxString temp_string(_T('\0'), 250);
}

class DLLIMPORT Logger
{
public:
	enum level { caption, info, warning, success, error, critical };
	enum { num_levels = critical +1 };

    Logger() {};
    virtual ~Logger() {};

    virtual void UpdateSettings() {};

    virtual void Append(const wxString& msg, Logger::level lv = info) = 0;
    virtual void Clear() = 0;

	virtual wxWindow* CreateControl() { return 0; };
};



class DLLIMPORT NullLog : public Logger
{
public:
    virtual void Append(const wxString& msg, Logger::level lv){};
    virtual void Clear(){};
};

class DLLIMPORT StdoutLog : public Logger
{
public:
    virtual void Append(const wxString& msg, Logger::level lv){ fputs(msg.mb_str(), lv < error ? stdout : stderr); };
    virtual void Clear(){};
};


class DLLIMPORT TextCtrlLog : public Logger
{
protected:

    wxTextCtrl* control;
	bool fixed;
	wxTextAttr style[num_levels];

public:

    TextCtrlLog(bool fixedPitchFont = false) : control(0), fixed(fixedPitchFont){};

    virtual void UpdateSettings()
	{
		int size = Manager::Get()->GetConfigManager(_T("message_manager"))->ReadInt(_T("/log_font_size"), platform::macosx ? 10 : 8);

		wxFont default_font(size, fixed ? wxFONTFAMILY_MODERN : wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
		wxFont bold_font(default_font);
		wxFont italic_font(default_font);
		wxFont bigger_font(bold_font);

		bold_font.SetWeight(wxFONTWEIGHT_BOLD);
		bigger_font.SetPointSize(size + 2);
		italic_font.SetStyle(wxFONTSTYLE_ITALIC);

		// might try alternatively
		//italic_font.SetStyle(wxFONTSTYLE_SLANT);

		for(unsigned int i = 0; i < num_levels; ++i)
		{
			style[i].SetFont(default_font);
			style[i].SetAlignment(wxTEXT_ALIGNMENT_DEFAULT);
			style[i].SetTextColour(*wxBLACK);
			style[i].SetBackgroundColour(wxNullColour);

			// is it necessary to do that?
			//style[i].SetFlags(...);
		}

		style[caption].SetAlignment(wxTEXT_ALIGNMENT_CENTRE);
		bigger_font.SetUnderlined(true);
		style[caption].SetFont(bigger_font);

		style[success].SetTextColour(*wxBLUE);

		style[warning].SetFont(italic_font);

		style[error].SetFont(bold_font);
		style[error].SetTextColour(*wxRED);

		style[critical].SetFont(bold_font);
		style[critical].SetTextColour(*wxWHITE);
		style[critical].SetBackgroundColour(*wxRED);
	};

    virtual void Append(const wxString& msg, Logger::level lv = info)
	{
		if(!control)
			return;

		::temp_string.assign(msg);
		::temp_string.append(_T("\n"));

		control->SetDefaultStyle(style[lv]);
		control->AppendText(::temp_string);
	};


    virtual void Clear()
	{
		if(control)
			control->Clear();
	};

	virtual wxWindow* CreateControl()
	{
		control = new wxTextCtrl(0, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH | wxTE_NOHIDESEL);

		UpdateSettings();

		return control;
	};
};


class DLLIMPORT TimestampTextCtrlLog : public TextCtrlLog
{
public:
    virtual void Append(const wxString& msg, Logger::level lv = info)
	{
		if(!control)
			return;

        wxDateTime timestamp(wxDateTime::UNow());
		::temp_string.Printf(_T("[%2.2d:%2.2d:%2.2d.%3.3d] %s\n"), timestamp.GetHour(), timestamp.GetMinute(), timestamp.GetSecond(), timestamp.GetMillisecond(), msg.c_str());

		control->SetDefaultStyle(style[lv]);
		control->AppendText(::temp_string);
	};
};


#endif
