/***************************************************************
 * Name:      astyle.h
 * Purpose:   Code::Blocks plugin
 * Author:    Yiannis Mandravellos<mandrav@codeblocks.org>
 * Created:   05/25/04 10:06:39
 * Copyright: (c) Yiannis Mandravellos
 * License:   GPL
 **************************************************************/

#ifndef ASTYLEPLUGIN_H
#define ASTYLEPLUGIN_H

#if defined(__GNUG__) && !defined(__APPLE__)
	#pragma interface "astyle.h"
#endif
// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <cbPlugin.h> // the base class we 're inheriting
#include <settings.h> // needed to use the Code::Blocks SDK

class AStylePlugin : public cbToolPlugin
{
public:
	AStylePlugin();
	~AStylePlugin();
	int Configure();
	void BuildMenu(wxMenuBar* menuBar){ return; }
	void BuildModuleMenu(const ModuleType type, wxMenu* menu, const wxString& arg){ return; }
	void BuildToolBar(wxToolBar* toolBar){ return; }
	int Execute();
	void OnAttach(); // fires when the plugin is attached to the application
	void OnRelease(bool appShutDown); // fires when the plugin is released from the application
protected:
private:
};

extern "C"
{
	PLUGIN_EXPORT cbPlugin* GetPlugin();
};

#endif // ASTYLEPLUGIN_H

