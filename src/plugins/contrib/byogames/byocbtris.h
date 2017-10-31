#ifndef BYOCBTRIS_H
#define BYOCBTRIS_H

#include <wx/font.h>
#include <wx/timer.h>
#include "byogamebase.h"

class wxPaintEvent;
class wxTimerEvent;
class wxEraseEvent;
class wxKeyEvent;

class byoCBTris: public byoGameBase
{
    public:

        byoCBTris(wxWindow* parent,const wxString& GameName);

    private:

        static const int bricksHoriz  = 15;
        static const int bricksMargin = 5;
        static const int bricksVert   = 30;

        typedef int ChunkConfig[16];

        void OnPaint(wxPaintEvent& event);
        void OnKeyDown(wxKeyEvent& event);
        void OnKeyUp(wxKeyEvent& event);
        void OnSpeedTimer(wxTimerEvent& event);
        void OnLeftRightTimer(wxTimerEvent& event);
        void OnUpTimer(wxTimerEvent& event);
        void OnDownTimer(wxTimerEvent& event);
        void OnEraseBack(wxEraseEvent& event);
        void OnKillFocus(wxFocusEvent& event);

        void DrawBrickField(wxDC* DC);
        void DrawCurrentChunk(wxDC* DC);
        void DrawNextChunk(wxDC* DC);

        void RandomizeChunk(ChunkConfig& chunk,int color=-1);
        void RotateChunkLeft(const ChunkConfig& chunk,ChunkConfig& newChunk);
        void RotateChunkRight(const ChunkConfig& chunk,ChunkConfig& newChunk);
        void AlignChunk(ChunkConfig& chunk);
        bool CheckChunkColision(const ChunkConfig& chunk,int posX,int posY);
        void SetSpeed();
        bool ChunkDown();
        void RemoveFullLines();
        bool GenerateNewChunk();
        void GameOver();
        void UpdateChunkPosLeftRight();
        void UpdateChunkPosUp();
        void UpdateChunkPosDown();
        void DrawStats(wxDC* DC);
        void AddRemovedLines(int removed);
        void StartTimerNow(wxTimer& timer);
        int GetScoreScale() const;

        wxTimer SpeedTimer;
        wxTimer LeftRightTimer;
        wxTimer UpTimer;
        wxTimer DownTimer;

        int m_Level;
        int m_Score;

        bool m_IsLeft;
        bool m_IsRight;
        bool m_IsUp;
        bool m_IsDown;
        int m_TotalRemovedLines;

        bool m_Guidelines;

        // Font used inside game
        wxFont m_Font;

        int m_Content[bricksHoriz][bricksVert];

        ChunkConfig m_CurrentChunk;
        int m_ChunkPosX;
        int m_ChunkPosY;
        ChunkConfig m_NextChunk;

        DECLARE_EVENT_TABLE()
};

#endif
