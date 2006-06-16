#include <sdk.h> // PCH support
#include "byogame.h"

byoGame::GamesT& byoGame::GetGames()
{
    static GamesT m_Games;
    return m_Games;
}

byoGame::byoGame(const wxString& Name): m_Name(Name)
{
    GetGames().Add(this);
}

byoGame::~byoGame()
{
    GetGames().Remove(this);
}
