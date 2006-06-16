/***************************************************************
 * Name:      byogames.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    BYO<byo.spoon@gmail.com>
 * Copyright: (c) BYO
 * License:   GPL
 **************************************************************/

#include <sdk.h> // PCH support
#include "byogames.h"
#include "byogame.h"
#include "byogameselect.h"
#include <licenses.h> // defines some common licenses (like the GPL)
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Implement the plugin's hooks
CB_IMPLEMENT_PLUGIN(BYOGames, "BYO Games");

BYOGames::BYOGames()
{
	//ctor
	m_PluginInfo.name = _T("BYOGames");
	m_PluginInfo.title = _("BYO Games");
	m_PluginInfo.version = _T("1.0");
	m_PluginInfo.description = _("Collection of games for C::B IDE");
	m_PluginInfo.author = _T("BYO");
	m_PluginInfo.authorEmail = _T("byo.spoon@gmail.com");
	m_PluginInfo.authorWebsite = _T("");
	m_PluginInfo.thanksTo = _("");
	m_PluginInfo.license = LICENSE_GPL;
}

BYOGames::~BYOGames()
{
}

void BYOGames::OnAttach()
{
    srand( time(NULL) );
}

void BYOGames::OnRelease(bool appShutDown)
{
}

int BYOGames::Execute()
{
    int gameNum = SelectGame();
    if ( gameNum<0 || gameNum>=byoGame::GetGamesCount() ) return 0;
    byoGame::PlayGame(gameNum);
    return 0;
}

int BYOGames::SelectGame()
{
    byoGameSelect Select(NULL);
    return Select.ShowModal();
}
