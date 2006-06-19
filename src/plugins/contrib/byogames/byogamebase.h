#ifndef BYOGAMEBASE_H
#define BYOGAMEBASE_H

#include <wx/window.h>
#include <wx/dc.h>

class byoGameBase: public wxWindow
{
    public:

        /** \brief Ctor */
        byoGameBase(wxWindow* parent,const wxString& GameName);

        /** \brief Dctor */
        virtual ~byoGameBase();

        /** \brief Getting name of the game */
        inline const wxString& GetName() { return m_GameName; }

        /** \brief Causing this class to reload all configuration stuff */
        static void ReloadFromConfig();

        /** \brief Time tick - this is used in Back-To-Work mode */
        static void BackToWorkTimer();

    protected:

        /** \brief Function recalculating size hints used inside bricks drawing
         *
         * Settings provided here will be used whenever content of window is resized
         * to recalculate valid values
         */
        void RecalculateSizeHints(int minStepsHoriz,int minStepsVert);

        /** \brief Getting abrolute position of given cell */
        void GetCellAbsolutePos(int cellX,int cellY,int& posX,int& posY);

        /** \brief Function drawing brick in given absolute position */
        void DrawBrickAbsolute(wxDC* DC,int posX,int posY,int width,int height,const wxColour& base);

        /** \brief Function drawing brick using grid ans size */
        void DrawBrick(wxDC* DC,int cellX,int cellY,const wxColour& base);

        /** \brief Getting one of standard colours used in game */
        const wxColour& GetColour(int index);

        /** \brief Function changing paused state
         *  \return state really set
         *  \note when in "work-forcing-mode", pause state can not be set to false
         */
        bool SetPause(bool pause=true);

        /** \brief Getting paused state */
        inline bool IsPaused() { return m_Paused; }

        /** \brief Getting string informing about Back-To-Work mode */
        wxString GetBackToWorkString();

    private:

        WX_DEFINE_ARRAY(byoGameBase*,GamesListT);

        /** \brief Resize event, used to recalculate size hints */
        void OnSize(wxSizeEvent& event);

        /** \brief Focus killing event, does automatically pause the game */
        void OnKillFocus(wxFocusEvent& event);

        int m_CellSize;
        int m_FirstCellXPos;
        int m_FirstCellYPos;
        int m_MinCellsHoriz;
        int m_MinCellsVert;
        bool m_Paused;
        wxString m_GameName;

        static GamesListT AllGames;

        DECLARE_EVENT_TABLE()
};

#endif
