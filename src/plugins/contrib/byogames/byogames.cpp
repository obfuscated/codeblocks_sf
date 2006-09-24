/***************************************************************
 * Name:      byogames.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    BYO<byo.spoon@gmail.com>
 * Copyright: (c) BYO
 * License:   GPL
 **************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "sdk.h"
#ifndef CB_PRECOMP
#include <wx/intl.h>
#include <wx/string.h>
#endif
#include "byogames.h"
#include "byogame.h"
#include "byogamebase.h"
#include "byogameselect.h"
#include "byoconf.h"

BEGIN_EVENT_TABLE(BYOGames,cbToolPlugin)
    EVT_TIMER(1,BYOGames::OnTimer)
END_EVENT_TABLE()

// Register the plugin
namespace
{
    PluginRegistrant<BYOGames> reg(_T("BYOGames"));
};

BYOGames::BYOGames(): SecondTick(this,1)
{
	SecondTick.Start(1000,true);
}

BYOGames::~BYOGames()
{
}

void BYOGames::OnAttach()
{
    srand( time(NULL) );
    byoGameBase::ReloadFromConfig();
}

void BYOGames::OnRelease(bool appShutDown)
{
}

int BYOGames::Execute()
{
    int gameNum = SelectGame();
    if ( gameNum<0 || gameNum>=byoGameLauncher::GetGamesCount() ) return 0;
    byoGameLauncher::PlayGame(gameNum);
    return 0;
}

int BYOGames::SelectGame()
{
    byoGameSelect Select(NULL);
    return Select.ShowModal();
}

void BYOGames::OnTimer(wxTimerEvent& event)
{
    byoGameBase::BackToWorkTimer();
    SecondTick.Start(-1,true);
}

cbConfigurationPanel* BYOGames::GetConfigurationPanel(wxWindow* parent)
{
    return new byoConf(parent);
}
