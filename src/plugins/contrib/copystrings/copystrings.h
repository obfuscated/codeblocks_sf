/***************************************************************
 * Name:      copystrings.cpp
 * Purpose:   Code::Blocks plugin - copies all literal strings to the clipboard
 * Author:    Ricardo Garcia
 * Copyright: (c) 2005 Ricardo Garcia
 * License:   wxWindows License
 **************************************************************/

#ifndef COPYSTRINGS_H
#define COPYSTRINGS_H

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

class copystrings : public cbToolPlugin
{
	public:
		copystrings();
		~copystrings();
		int Configure(){ return 0; }
		int Execute();
		void OnAttach(); // fires when the plugin is attached to the application
		void OnRelease(bool appShutDown); // fires when the plugin is released from the application
		void GetStrings(const wxString& buffer,wxString& result);
	protected:
	private:
};

// Declare the plugin's hooks
CB_DECLARE_PLUGIN();

#endif // COPYSTRINGS_H

