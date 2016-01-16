/***************************************************************
 * Name:      codestat.h
 * Purpose:   Code::Blocks CodeStat plugin: main functions
 * Author:    Zlika
 * Created:   11/09/2005
 * Copyright: (c) Zlika
 * License:   GPL
 **************************************************************/

#ifndef CODESTAT_H
#define CODESTAT_H

#include "cbplugin.h" // the base class we 're inheriting

class cbConfigurationPanel;
class CodeStatExecDlg;
class wxWindow;

/** Main class for the Code Statistics plugin.
 *  @see CodeStatConfigDlg, CodeStatExecDlg, LanguageDef
 */
class CodeStat : public cbToolPlugin
{
	public:
		CodeStat();
		~CodeStat();
		int GetConfigurationGroup()  const { return cgEditor; }
        cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);
		int Execute();
		void OnAttach(); // fires when the plugin is attached to the application
		void OnRelease(bool appShutDown); // fires when the plugin is released from the application
	private:
      CodeStatExecDlg* dlg;
};

#endif // CODESTAT_H
