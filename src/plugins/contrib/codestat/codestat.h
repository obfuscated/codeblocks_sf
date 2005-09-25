/***************************************************************
 * Name:      codestat.h
 * Purpose:   Code::Blocks plugin
 * Author:    Zlika
 * Created:   11/09/2005
 * Copyright: (c) Zlika
 * License:   GPL
 **************************************************************/

#ifndef CODESTAT_H
#define CODESTAT_H

#if defined(__GNUG__) && !defined(__APPLE__)
	#pragma interface "cbprofiler.h"
#endif
// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <cbplugin.h> // the base class we 're inheriting
#include <settings.h> // needed to use the Code::Blocks SDK

#include "codestatexec.h"
#include "language_def.h"


class CodeStat : public cbToolPlugin
{
	public:
		CodeStat();
		~CodeStat();
		int Configure();
		int Execute();
		void OnAttach(); // fires when the plugin is attached to the application
		void OnRelease(bool appShutDown); // fires when the plugin is released from the application
	protected:
	private:
	   void LoadSettings(LanguageDef languages[NB_FILETYPES]);
      CodeStatExecDlg* dlg;
};

#ifdef __cplusplus
extern "C" {
#endif
	PLUGIN_EXPORT cbPlugin* GetPlugin();
#ifdef __cplusplus
};
#endif

#endif // CODESTAT_H

