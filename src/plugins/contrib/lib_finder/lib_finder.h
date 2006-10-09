/***************************************************************
 * Name:      lib_finder.h
 * Purpose:   Code::Blocks plugin
 * Author:    BYO<byo.spoon@gmail.com>
 * Copyright: (c) BYO
 * License:   GPL
 **************************************************************/

#ifndef LIB_AUTO_CONF_H
#define LIB_AUTO_CONF_H

#include "cbplugin.h"
#include "settings.h"

class LibraryResult;

class lib_finder : public cbToolPlugin
{
	public:
		lib_finder();
		~lib_finder();
		int Configure(){ return 0; }
		int Execute();
		void OnAttach();
		void OnRelease(bool appShutDown);
	private:

        void SetGlobalVar(const LibraryResult* Result);
};

#endif

