/***************************************************************
 * Name:      Valgrind.h
 * Purpose:   Code::Blocks Valgrind plugin: main functions
 * Author:    Lieven de Cock (aka killerbot)
 * Created:   28/07/2007
 * Copyright: (c) Lieven de Cock (aka killerbot)
 * License:   GPL
 **************************************************************/

#ifndef VALGRIND_H_INCLUDED
#define VALGRIND_H_INCLUDED

#include "cbplugin.h" // the base class we 're inheriting

class TextCtrlLogger;
class ValgrindListLog;
class TiXmlElement;

class Valgrind : public cbPlugin
{
public:
	Valgrind();
	~Valgrind();
	void BuildMenu(wxMenuBar* menuBar);
	void BuildModuleMenu(const ModuleType /*type*/, wxMenu* /*menu*/, const FileTreeData* /*data*/ = 0){};
	bool BuildToolBar(wxToolBar* /*toolBar*/){ return false; }
	void OnAttach(); // fires when the plugin is attached to the application
	void OnRelease(bool appShutDown); // fires when the plugin is released from the application
private:
	void WriteToLog(const wxString& Text);
	void AppendToLog(const wxString& Text);
	void ProcessStack(const TiXmlElement& Stack, bool AddHeader);
	long DoValgrindVersion();
	void OnMemCheck(wxCommandEvent& );
	void OnCachegrind(wxCommandEvent& );

	TextCtrlLogger*	m_ValgrindLog;	//!< log tab in the message pane
	ValgrindListLog* m_ListLog;		//!< log tab to click/double click to take you to offending line of code
	int				m_LogPageIndex;	//!< index of our log tab (can this change during run time ??)
	int				m_ListLogPageIndex;	//!< index of our list log tab
	DECLARE_EVENT_TABLE()
};

#endif // VALGRIND_H_INCLUDED
