/***************************************************************
 * Name:      copystrings.cpp
 * Purpose:   Code::Blocks plugin - copies all literal strings to the clipboard
 * Author:    Ricardo Garcia
 * Copyright: (c) 2005 Ricardo Garcia
 * License:   wxWindows License
 **************************************************************/

#ifndef COPYSTRINGS_H
#define COPYSTRINGS_H

#include "cbplugin.h" // the base class we 're inheriting

class copystrings : public cbToolPlugin
{
	public:
		copystrings();
		~copystrings();
		int Execute();
		void OnAttach(); // fires when the plugin is attached to the application
		void OnRelease(bool appShutDown); // fires when the plugin is released from the application
};

#endif // COPYSTRINGS_H

