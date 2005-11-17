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

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <licenses.h> // defines some common licenses (like the GPL)
#include <manager.h>
#include <configmanager.h>
#include <cbproject.h>
#include <cbplugin.h> // the base class we 're inheriting
#include <settings.h> // needed to use the Code::Blocks SDK
#include <projectmanager.h>
#include <messagemanager.h>
#include <wx/xrc/xmlres.h>
#include <wx/fs_zip.h>
#include "codestatexec.h"
#include "codestatconfig.h"
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

CB_DECLARE_PLUGIN();

#endif // CODESTAT_H
