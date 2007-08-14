#ifndef Log_H
#define Log_H

#include <wx/panel.h>
#include <wx/font.h>

#include <wx/string.h>
#include <wx/ffile.h>

#include "manager.h"
#include "configmanager.h"

#include <stdio.h>

// TODO (killerbot) : other includes are needed -->wxDateTime

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

	virtual wxWindow* CreateControl(wxWindow* parent) { return 0; };
};



class DLLIMPORT NullLogger : public Logger
{
public:
    virtual void Append(const wxString& msg, Logger::level lv){};
    virtual void Clear(){};
};

class DLLIMPORT StdoutLogger : public Logger
{
public:
    virtual void Append(const wxString& msg, Logger::level lv){ fputs(msg.mb_str(), lv < error ? stdout : stderr); };
    virtual void Clear(){};
};

class DLLIMPORT FileLogger : public Logger
{
	wxFFile f;
public:
	FileLogger(const wxString& filename) : f(filename, _T("wb")) {  };

    virtual void Append(const wxString& msg, Logger::level lv) { fputs(msg.mb_str(), f.fp()); fputs(::newline_string.mb_str(), f.fp()); };
    virtual void Clear(){};
};


class DLLIMPORT TextCtrlLogger : public Logger
{
protected:

    wxTextCtrl* control;
	bool fixed;
	wxTextAttr style[num_levels];

public:

    TextCtrlLogger(bool fixedPitchFont = false) : control(0), fixed(fixedPitchFont){};

    virtual void UpdateSettings()
	{
        control->SetBackgroundColour(*wxWHITE);

		int size = Manager::Get()->GetConfigManager(_T("message_manager"))->ReadInt(_T("/log_font_size"), platform::macosx ? 10 : 8);

		wxFont default_font(size, fixed ? wxFONTFAMILY_MODERN : wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
		wxFont bold_font(default_font);
		wxFont italic_font(default_font);

		bold_font.SetWeight(wxFONTWEIGHT_BOLD);

		wxFont bigger_font(bold_font);
		bigger_font.SetPointSize(size + 2);

		wxFont small_font(default_font);
		small_font.SetPointSize(size - 4);

		italic_font.SetStyle(wxFONTSTYLE_ITALIC);

		// might try alternatively
		//italic_font.SetStyle(wxFONTSTYLE_SLANT);

		for(unsigned int i = 0; i < num_levels; ++i)
		{
			style[i].SetFont(default_font);
			style[i].SetAlignment(wxTEXT_ALIGNMENT_DEFAULT);
			style[i].SetTextColour(*wxBLACK);
			style[i].SetBackgroundColour(*wxWHITE);

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
		style[spacer].SetFont(small_font);
	};

    virtual void Append(const wxString& msg, Logger::level lv = info)
	{
		if(!control)
			return;

		::temp_string.assign(msg);
		::temp_string.append(_T("\n"));

        if(lv == caption)
        {
            control->SetDefaultStyle(style[info]);
            control->AppendText(::newline_string);

            control->SetDefaultStyle(style[lv]);
            control->AppendText(::temp_string);

            control->SetDefaultStyle(style[spacer]);
            control->AppendText(::newline_string);
        }
        else
        {
            control->SetDefaultStyle(style[lv]);
            control->AppendText(::temp_string);
        }
	};


    virtual void Clear()
	{
		if(control)
			control->Clear();
	};

	virtual wxWindow* CreateControl(wxWindow* parent)
	{
		control = new wxTextCtrl(parent, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH | wxTE_NOHIDESEL);
		return control;
	};
};


class DLLIMPORT TimestampTextCtrlLogger : public TextCtrlLogger
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
