#ifndef BYOSNAKE_H
#define BYOSNAKE_H

#include <wx/font.h>
#include <wx/timer.h>
#include "byogamebase.h"

class wxPaintEvent;
class wxTimerEvent;
class wxEraseEvent;

/** \brief Game with traditional snake eating something */
class byoSnake : public byoGameBase
{
    public:

        /** \brief Ctor */
        byoSnake(wxWindow* parent,const wxString& GameName);

        /** \brief Dctor */
        virtual ~byoSnake();

    private:

        static const int m_FieldHoriz = 30;
        static const int m_FieldVert = 15;
        static const int m_FieldTotal = m_FieldHoriz * m_FieldVert;
        static const int m_BorderColour = 0;
        static const int m_SnakeColour = 1;
        static const int m_AppleColour = 2;
        static const int m_MaxKillCnt = 2;

        enum Direction
        {
            dLeft, dRight, dUp, dDown
        };

        int m_AppleX;
        int m_AppleY;

        int m_SnakeX[m_FieldTotal+2];
        int m_SnakeY[m_FieldTotal+2];
        int m_SnakeLen;
        bool m_Field[m_FieldHoriz][m_FieldVert];

        int m_Delay;
        int m_Lives;
        int m_Score;
        int m_InitialSlowdownCnt;
        int m_KillCnt;
        wxFont m_Font;

        wxTimer m_Timer;

        Direction m_Direction;

        void OnKeyDown(wxKeyEvent& event);
        void OnPaint(wxPaintEvent& event);
        void OnTimer(wxTimerEvent& event);
        void OnEraseBack(wxEraseEvent& event);

        void InitializeSnake();
        void RandomizeApple();
        void StartSnake();
        void RebuildField();
        void Move();
        void Died();
        void GameOver();
        void GetsBigger();
        void UpdateSpeed();

        void DrawBorder(wxDC* DC);
        void DrawSnake(wxDC* DC);
        void DrawApple(wxDC* DC);
        void DrawStats(wxDC* DC);

        DECLARE_EVENT_TABLE()
};

#endif
