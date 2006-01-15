/***************************************************************
 * Name:      cbprofiler.h
 * Purpose:   Code::Blocks plugin
 * Author:    Dark Lord & Zlika
 * Created:   07/20/05 11:12:57
 * Copyright: (c) Dark Lord & Zlika
 * Thanks:    Yiannis Mandravellos and his Source code formatter (AStyle) sources
 * License:   GPL
 **************************************************************/

#ifndef CBPROFILER_H
#define CBPROFILER_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/fs_zip.h>
#include <wx/choicdlg.h>

#include <cbplugin.h> // the base class we 're inheriting
#include <settings.h> // needed to use the Code::Blocks SDK
#include <licenses.h> // defines some common licenses (like the GPL)
#include <manager.h>
#include <cbproject.h>
#include <projectmanager.h>
#include <messagemanager.h>

#include "cbprofilerexec.h"
#include "cbprofilerconfig.h"

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

CB_DECLARE_PLUGIN();

#endif // CBPROFILER_H

