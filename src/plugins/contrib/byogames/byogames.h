/***************************************************************
 * Name:      byogames.h
 * Purpose:   Code::Blocks plugin
 * Author:    BYO<byo.spoon@gmail.com>
 * Copyright: (c) BYO
 * License:   GPL
 **************************************************************/

#ifndef BYOGAMES_H
#define BYOGAMES_H

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#include <wx/timer.h>
#include "cbplugin.h" // the base class we 're inheriting

class BYOGames : public cbToolPlugin
{
	public:

		BYOGames();
		~BYOGames();
		int Configure(){ return  0; }
		int GetConfigurationPriority() const{ return  25; }
		int GetConfigurationGroup() const { return cgContribPlugin; }
		cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);
		int Execute();
		void OnAttach(); // fires when the plugin is attached to the application
		void OnRelease(bool appShutDown); // fires when the plugin is released from the application

	private:

        wxTimer SecondTick;
        void OnTimer(wxTimerEvent& event);

        int SelectGame();

        DECLARE_EVENT_TABLE()
};

#endif
