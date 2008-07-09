/***************************************************************
 * Name:      Valgrind.h
 * Purpose:   Code::Blocks Valgrind plugin: main functions
 * Author:    killerbot
 * Created:   28/07/2007
 * Copyright: (c) killerbot
 * License:   GPL
 **************************************************************/

#ifndef VALGRIND_H_INCLUDED
#define VALGRIND_H_INCLUDED

#include "cbplugin.h" // the base class we 're inheriting

class TextCtrlLogger;
//class ListCtrlLogger;
class ValgrindListLog;
class TiXmlElement;

class Valgrind : public cbToolPlugin
{
public:
	Valgrind();
	~Valgrind();
//		int GetConfigurationGroup()  const { return cgEditor; }
//        cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);
	int Execute();
	void OnAttach(); // fires when the plugin is attached to the application
	void OnRelease(bool appShutDown); // fires when the plugin is released from the application
private:
	void WriteToLog(const wxString& Text);
	void AppendToLog(const wxString& Text);
	void ProcessStack(const TiXmlElement& Stack, const wxString& What);

	TextCtrlLogger*	m_ValgrindLog;	//!< log tab in the message pane
	ValgrindListLog* m_ListLog;		//!< log tab to click/double click to take you to offending line of code
	int				m_LogPageIndex;	//!< index of our log tab (can this change during run time ??)
	int				m_ListLogPageIndex;	//!< index of our list log tab
};

#endif // VALGRIND_H_INCLUDED
