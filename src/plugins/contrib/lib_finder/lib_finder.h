/***************************************************************
 * Name:      lib_finder.h
 * Purpose:   Code::Blocks plugin
 * Author:    BYO<byo.spoon@gmail.com>
 * Copyright: (c) BYO
 * License:   GPL
 **************************************************************/

#ifndef LIB_AUTO_CONF_H
#define LIB_AUTO_CONF_H

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <cbplugin.h>
#include <settings.h>
#include "libraryresult.h"

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

        void SetGlobalVar(LibraryResult* Result);
};

#endif

