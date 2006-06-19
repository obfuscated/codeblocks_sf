#ifndef BYOGAME_H
#define BYOGAME_H

#include "byogamebase.h"

#include <wx/string.h>
#include <wx/dynarray.h>
#include <wx/intl.h>
#include <editorbase.h>

/** \brief Generic interface for game launcher.
 *
 * This class represents game type, not game instance.
 * It should be created as some static variable.
 * byoGameLauncher's constructor registers game in list of
 * available games.
 */
class byoGameLauncher
{
    public:

        /** \brief Ctor */
        byoGameLauncher(const wxString& Name);

        /** \brief Dctor */
        virtual ~byoGameLauncher();

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

        WX_DEFINE_ARRAY(byoGameLauncher*,GamesT);

        wxString m_Name;        ///< \brief Name of this game

        /** \brief Returning games object */
        static GamesT& GetGames();
};

/** \brief Gaming extension to editorbase class
 *
 * This class handles mostly focus setting into real game class
 */
class byoEditorBase: public EditorBase
{
    public:

        /** \brief Ctor, parent window is not required becuase
          *        notebook will be used
          */
        byoEditorBase(const wxString& GameName);

        /** \brief Dctor */
        virtual ~byoEditorBase();

        /** \brief Adding game class */
        void AddGameContent(byoGameBase* base);

        virtual void Activate() { if (m_Content) m_Content->SetFocus(); EditorBase::Activate(); }

    private:

        void OnSetFocus(wxFocusEvent& event) { if (m_Content) m_Content->SetFocus(); }

        byoGameBase* m_Content;
        DECLARE_EVENT_TABLE()
};


#define BYO_REGISTER_GAME(ClassName,GameName) \
    namespace \
    { \
        class ClassName##_Launcher: public byoGameLauncher \
        { \
            public:\
                ClassName##_Launcher(): byoGameLauncher(_(GameName)) {}\
            protected:\
                virtual void Play()\
                {\
                    byoEditorBase* base = new byoEditorBase(_(GameName));\
                    base->AddGameContent(new ClassName(base,_(GameName)));\
                }\
        } ClassName##_Launcher_Instance;\
    }


#endif
