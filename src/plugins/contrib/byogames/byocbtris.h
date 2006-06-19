#ifndef BYOCBTRIS_H
#define BYOCBTRIS_H

#include "byogame.h"

#include <wx/window.h>
#include <wx/font.h>

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
        void OnChar(wxKeyEvent& event);
        void OnSpeedTimer(wxTimerEvent& event);
        void OnLeftRightTimer(wxTimerEvent& event);
        void OnUpTimer(wxTimerEvent& event);
        void OnDownTimer(wxTimerEvent& event);
        void OnEraseBack(wxEraseEvent& event);

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
        void UpdateChunkPos();
        void DrawStats(wxDC* DC);
        void AddRemovedLines(int removed);

        wxTimer SpeedTimer;
        wxTimer LeftRightTimer;
        wxTimer UpTimer;
        wxTimer DownTimer;

        int m_Level;
        int m_Score;

        int m_LeftRightCnt;
        int m_UpCnt;
        int m_DownCnt;
        bool m_WasLeft;
        bool m_WasRight;
        bool m_WasUp;
        bool m_WasDown;
        int totalRemovedLines;

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
