/***************************************************************
 * Name:      cbprofiler.h
 * Purpose:   Code::Blocks plugin
 * Author:    Dark Lord
 * Created:   07/20/05 11:12:57
 * Copyright: (c) Dark Lord
 * Thanks:    Yiannis Mandravellos and his Source code formatter (AStyle) sources
 * License:   GPL
 **************************************************************/

#ifndef CBPROFILER_H
#define CBPROFILER_H

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

#include "cbprofilerexec.h"

/*
 * Save per pro config, or should I use exe path?
 * User can make a global variable, or per project? with the name of the file to profile or path (2 vars)
 * User can chose the target to profile
 */

class CBProfiler : public cbToolPlugin
{
	public:
		CBProfiler();
		~CBProfiler();
		int Configure();
		int Execute();
		void OnAttach(); // fires when the plugin is attached to the application
		void OnRelease(bool appShutDown); // fires when the plugin is released from the application
		//void ShowDialog();
	protected:
	private:
        CBProfilerExecDlg* dlg;
};

#ifdef __cplusplus
extern "C" {
#endif
	PLUGIN_EXPORT cbPlugin* GetPlugin();
#ifdef __cplusplus
};
#endif

#endif // CBPROFILER_H

