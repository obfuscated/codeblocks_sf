#ifndef BYOGAME_H
#define BYOGAME_H

#include <wx/string.h>
#include <wx/dynarray.h>
#include <wx/intl.h>

/** \brief Generic "back-to-work" timer.
 *
 * By just creating an instance of this class, throught its lifetime
 * it will popup a "back-to-work" message every some predefined minutes (default is 5).
 */
class BackToWorkTimer : public wxEvtHandler
{
        DECLARE_EVENT_TABLE();
        size_t m_Timeout;
        wxTimer m_Timer;

        void OnTimer(wxTimerEvent& event);
    public:
        BackToWorkTimer(size_t minutes = 5);
};

/** \brief Generic interface for game.
 *
 * This class represents game type, not game instance.
 * It should be created as some static variable.
 * byoGame's constructor registers game in list of
 * available games.
 */
class byoGame
{
    public:

        /** \brief Ctor */
        byoGame(const wxString& Name);

        /** \brief Dctor */
        virtual ~byoGame();

        /** \brief Getting number of registered games */
        static inline int GetGamesCount() { return (int)GetGames().Count(); }

        /** \brief Getting name of game */
        static inline const wxString& GetGameName(int gameNum) { return GetGames()[gameNum]->m_Name; }

        /** \brief Playing game */
        static inline void PlayGame(int gameNum) { GetGames()[gameNum]->Play(); }

    protected:

        /** \brief Starting game */
        virtual void Play() = 0;

    private:

        WX_DEFINE_ARRAY(byoGame*,GamesT);

        wxString m_Name;        ///< \brief Name of this game

        /** \brief Returning games object */
        static GamesT& GetGames();
};

#endif
