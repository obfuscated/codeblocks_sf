#include "sdk.h"
#ifndef CB_PRECOMP
#include <wx/event.h>
#include <wx/image.h>
#endif
#include <wx/bitmap.h>
#include <wx/dc.h>
#include <wx/dcbuffer.h>
#include <wx/settings.h>
#include "byocbtris.h"
#include "byogame.h"

#define LOCK_START() static bool _lock = false; if ( _lock ) return; _lock = true;
#define LOCK_END() _lock = false;

namespace
{
    const int Chunks[][16] =
    {
        { 1,1,1,1,
          0,0,0,0,
          0,0,0,0,
          0,0,0,0 },

        { 1,1,0,0,

          1,1,0,0,
          0,0,0,0,
          0,0,0,0 },

        { 1,1,1,0,
          1,0,0,0,
          0,0,0,0,
          0,0,0,0 },

        { 1,1,1,0,
          0,0,1,0,
          0,0,0,0,
          0,0,0,0 },

        { 0,1,1,0,
          1,1,0,0,
          0,0,0,0,
          0,0,0,0 },

        { 1,1,0,0,
          0,1,1,0,
          0,0,0,0,
          0,0,0,0 },

        { 1,1,1,0,
          0,1,0,0,
          0,0,0,0,
          0,0,0,0 },
    };

    const int ChunkTypes = sizeof(Chunks)/sizeof(Chunks[0]);

    const int SpeedTimerId = wxNewId();
    const int LeftRightTimerId = wxNewId();
    const int UpTimerId = wxNewId();
    const int DownTimerId = wxNewId();
}


BEGIN_EVENT_TABLE(byoCBTris,byoGameBase)
    EVT_PAINT(byoCBTris::OnPaint)
    EVT_KEY_DOWN(byoCBTris::OnKeyDown)
    EVT_KEY_UP(byoCBTris::OnKeyUp)
    EVT_TIMER(SpeedTimerId,    byoCBTris::OnSpeedTimer)
    EVT_TIMER(LeftRightTimerId,byoCBTris::OnLeftRightTimer)
    EVT_TIMER(UpTimerId,       byoCBTris::OnUpTimer)
    EVT_TIMER(DownTimerId,     byoCBTris::OnDownTimer)
    EVT_KILL_FOCUS(byoCBTris::OnKillFocus)
    EVT_ERASE_BACKGROUND(byoCBTris::OnEraseBack)
END_EVENT_TABLE()

byoCBTris::byoCBTris(wxWindow* parent,const wxString& Name):
    byoGameBase(parent,Name),
    SpeedTimer(this,SpeedTimerId),
    LeftRightTimer(this,LeftRightTimerId),
    UpTimer(this,UpTimerId),
    DownTimer(this,DownTimerId),
    m_Level(1),
    m_Score(0),
    m_IsLeft(false),
    m_IsRight(false),
    m_IsUp(false),
    m_IsDown(false),
    m_TotalRemovedLines(0),
    m_Guidelines(false)
{
	m_Font = wxSystemSettings::GetFont(wxSYS_OEM_FIXED_FONT);
	LeftRightTimer.Start(100);
	UpTimer.Start(1000);
	DownTimer.Start(20);
	SetSpeed();

    memset(m_Content,0,sizeof(m_Content));
    memset(m_CurrentChunk,0,sizeof(m_CurrentChunk));
    memset(m_NextChunk,0,sizeof(m_NextChunk));

    RandomizeChunk(m_NextChunk);
    GenerateNewChunk();

    RecalculateSizeHints(bricksHoriz+2*bricksMargin,bricksVert+1);
}

void byoCBTris::OnPaint(wxPaintEvent& /*event*/)
{
    wxSize size = GetClientSize();
    wxBitmap buffer(wxImage(size.GetWidth(),size.GetHeight()));
    wxBufferedPaintDC DC(this,buffer);
    DrawBrickField(&DC);
    DrawCurrentChunk(&DC);
    DrawNextChunk(&DC);
    DrawStats(&DC);
}

void byoCBTris::OnKeyDown(wxKeyEvent& event)
{
    if ( event.GetKeyCode() == 'p' || event.GetKeyCode() == 'P' )
    {
        SetPause(!IsPaused());
        Refresh();
    }

    if ( IsPaused() ) return;

    if ( event.GetKeyCode() == WXK_LEFT && !m_IsLeft )
    {
        m_IsLeft = true;
        StartTimerNow(LeftRightTimer);
    }

    if ( event.GetKeyCode() == WXK_RIGHT && !m_IsRight )
    {
        m_IsRight = true;
        StartTimerNow(LeftRightTimer);
    }

    if ( event.GetKeyCode() == WXK_UP && !m_IsUp )
    {
        m_IsUp = true;
        StartTimerNow(UpTimer);
    }

    if ( event.GetKeyCode() == WXK_DOWN && !m_IsDown )
    {
        m_IsDown = true;
        StartTimerNow(DownTimer);
    }

    if ( event.GetKeyCode() == 'g' || event.GetKeyCode() == 'G' )
    {
        m_Guidelines = !m_Guidelines;
    }
}

void byoCBTris::OnKeyUp(wxKeyEvent& event)
{
    if ( event.GetKeyCode() == WXK_LEFT  ) m_IsLeft = false;
    if ( event.GetKeyCode() == WXK_RIGHT ) m_IsRight = false;
    if ( event.GetKeyCode() == WXK_UP    ) m_IsUp = false;
    if ( event.GetKeyCode() == WXK_DOWN  ) m_IsDown = false;
}

void byoCBTris::OnLeftRightTimer(wxTimerEvent& /*event*/)
{
    if ( IsPaused() ) return;
    LOCK_START();
    UpdateChunkPosLeftRight();
    Refresh();
    LOCK_END();
}

void byoCBTris::OnUpTimer(wxTimerEvent& /*event*/)
{
    if ( IsPaused() ) return;
    LOCK_START();
    UpdateChunkPosUp();
    Refresh();
    LOCK_END();
}

void byoCBTris::OnDownTimer(wxTimerEvent& /*event*/)
{
    if ( IsPaused() ) return;
    LOCK_START();
    UpdateChunkPosDown();
    Refresh();
    LOCK_END();
}

void byoCBTris::OnSpeedTimer(wxTimerEvent& /*event*/)
{
    if ( IsPaused() ) return;
    LOCK_START();
    if ( !ChunkDown() )
    {
        RemoveFullLines();
        if ( !GenerateNewChunk() )
        {
            GameOver();
        }
    }
    Refresh();
    LOCK_END();
}

void byoCBTris::OnEraseBack(wxEraseEvent& /*event*/)
{
}

void byoCBTris::DrawBrickField(wxDC* DC)
{
    static const wxColour Border(0x40,0x40,0x40);

    for ( int x=0; x<bricksHoriz; x++ )
        for ( int y=0; y<bricksVert; y++ )
            if ( m_Content[x][y] )
            {
                DrawBrick(DC,bricksMargin+x,y,GetColour(m_Content[x][y]));
            }
    for ( int y=0; y<bricksVert; y++ )
    {
        DrawBrick(DC,bricksMargin-1,y,Border);
        DrawBrick(DC,bricksMargin+bricksHoriz,y,Border);
    }
    for ( int x=bricksMargin-1; x<=bricksMargin+bricksHoriz; x++ )
    {
        DrawBrick(DC,x,bricksVert,Border);
    }

    static const wxColour Guideline(0x70,0x70,0x70);
    if(m_Guidelines) DrawGuidelines(DC,bricksMargin,bricksHoriz,bricksVert,Guideline);
}

void byoCBTris::DrawCurrentChunk(wxDC* DC)
{
    int* Ptr = m_CurrentChunk;
    for ( int y=0; y<4; y++ )
        for ( int x=0; x<4; x++ )
        {
            if ( *Ptr )
                DrawBrick(DC,
                    x+m_ChunkPosX+bricksMargin,
                    y+m_ChunkPosY,
                    GetColour(*Ptr));
            Ptr++;
        }
}

void byoCBTris::DrawNextChunk(wxDC* DC)
{
    int* Ptr = m_NextChunk;
    for ( int y=0; y<4; y++ )
        for ( int x=0; x<4; x++ )
        {
            if ( *Ptr )
                DrawBrick(DC,
                    bricksMargin-5+x,
                    bricksVert-5+y,
                    GetColour(*Ptr));
            Ptr++;
        }
}

void byoCBTris::RandomizeChunk(ChunkConfig& chunk,int color)
{
    if ( color < 1 || color > 6 ) color = (int)((rand() * 6.0 / RAND_MAX) + 1);
    if ( color < 1 ) color = 1;
    if ( color > 6 ) color = 6;
    int chunkNumber = (int)((rand() * (double)ChunkTypes) / RAND_MAX);
    if ( chunkNumber < 0 ) chunkNumber = 0;
    if ( chunkNumber >= ChunkTypes ) chunkNumber = ChunkTypes-1;
    color = chunkNumber + 1;

    for ( int i=0; i<16; i++ )
        chunk[i] = (Chunks[chunkNumber])[i] * color;

    for ( int i = (int)(rand() * 4.0 / RAND_MAX); i-->0; )
    {
        ChunkConfig newChunk;
        RotateChunkLeft(m_NextChunk,newChunk);
        memcpy(m_NextChunk,newChunk,sizeof(newChunk));
    }

}

void byoCBTris::RotateChunkLeft(const ChunkConfig& chunk,ChunkConfig& newChunk)
{
    for ( int y=0; y<4; y++ )
        for ( int x=0; x<4; x++ )
            newChunk[x+4*y] = chunk[(3-x)*4+y];

    AlignChunk(newChunk);
}

void byoCBTris::RotateChunkRight(const ChunkConfig& chunk,ChunkConfig& newChunk)
{
    for ( int y=0; y<4; y++ )
        for ( int x=0; x<4; x++ )
            newChunk[x+4*y] = chunk[x*4+3-y];

    AlignChunk(newChunk);
}

void byoCBTris::AlignChunk(ChunkConfig& chunk)
{
    // Searching for first not empty row
    int rowsShift = 0;
    while ( rowsShift<4 )
    {
        bool isEmpty = true;
        for ( int x=0; x<4; x++ )
            if ( chunk[rowsShift*4+x] != 0 )
            {
                isEmpty = false;
                break;
            }
        if ( !isEmpty ) break;
        rowsShift++;
    }

    // Searching for first not empty column
    int colsShift = 0;
    while ( colsShift<4 )
    {
        bool isEmpty = true;
        for ( int y=0; y<4; y++ )
            if ( chunk[colsShift+y*4] != 0 )
            {
                isEmpty = false;
                break;
            }
        if ( !isEmpty ) break;
        colsShift++;
    }

    if ( !colsShift && !rowsShift ) return;

    ChunkConfig newChunk;
    memset(newChunk,0,sizeof(newChunk));
    for ( int y=rowsShift,y2=0; y<4; y++,y2++ )
        for ( int x=colsShift,x2=0; x<4; x++,x2++ )
            newChunk[x2+y2*4] = chunk[x+y*4];
    memcpy(chunk,newChunk,sizeof(chunk));
}

bool byoCBTris::CheckChunkColision(const ChunkConfig& chunk,int posX,int posY)
{
    for ( int y=0; y<4; y++ )
        for ( int x=0; x<4; x++ )
            if ( chunk[x+4*y] )
            {
                int curX = posX + x;
                int curY = posY + y;
                if ( curX < 0 ) return true;
                if ( curX >= bricksHoriz ) return true;
                if ( curY < 0 ) return true;
                if ( curY >= bricksVert ) return true;
                if ( m_Content[curX][curY] ) return true;
            }
    return false;
}

void byoCBTris::SetSpeed()
{
    if ( m_Level < 10 )
    {
        SpeedTimer.Start((10-m_Level)*100);
    }
    else
    {
        SpeedTimer.Start(100);
    }
}

bool byoCBTris::ChunkDown()
{
    if ( CheckChunkColision(m_CurrentChunk,m_ChunkPosX,m_ChunkPosY+1 ) )
    {
        // Adding chunk to current map
        for ( int y=0; y<4; y++ )
            for ( int x=0; x<4; x++ )
                if ( m_CurrentChunk[x+4*y] )
                    m_Content[x+m_ChunkPosX][y+m_ChunkPosY] = m_CurrentChunk[x+4*y];
        m_Score += GetScoreScale();
        return false;
    }

    m_ChunkPosY++;
    return true;
}

void byoCBTris::RemoveFullLines()
{
    int count = 0;

    int checkY = bricksVert-1;
    int copyToY = bricksVert-1;

    while ( checkY >= 0 )
    {
        bool isFull = true;
        for ( int x=0; x<bricksHoriz; x++ )
            if ( !m_Content[x][checkY] ) isFull = false;

        if ( isFull )
        {
            count++;
        }
        else
        {
            if ( checkY != copyToY )
            {
                for ( int x=0; x<bricksHoriz; x++ )
                    m_Content[x][copyToY] = m_Content[x][checkY];
            }
            copyToY--;
        }
        checkY--;
    }

    while ( copyToY >= 0 )
    {
        for ( int x=0; x<bricksHoriz; x++ )
            m_Content[x][copyToY] = 0;
        copyToY--;
    }

    m_Score += 10 * count*count * GetScoreScale();
    AddRemovedLines(count);
}

bool byoCBTris::GenerateNewChunk()
{
    memcpy(m_CurrentChunk,m_NextChunk,sizeof(m_CurrentChunk));
    RandomizeChunk(m_NextChunk);

    m_ChunkPosY = 0;
    m_ChunkPosX = (bricksHoriz-4)/2;
    return !CheckChunkColision(m_CurrentChunk,m_ChunkPosX,m_ChunkPosY);
}

void byoCBTris::GameOver()
{
    Refresh();
    SpeedTimer.Stop();
    LeftRightTimer.Stop();
    DownTimer.Stop();
    UpTimer.Stop();
    SetPause(true);
    wxMessageBox(_("Game over"));
}

void byoCBTris::UpdateChunkPosLeftRight()
{
    if ( m_IsLeft && !m_IsRight )
    {
        if ( !CheckChunkColision(m_CurrentChunk,m_ChunkPosX-1,m_ChunkPosY) ) m_ChunkPosX--;
    }
    if ( !m_IsLeft && m_IsRight )
    {
        if ( !CheckChunkColision(m_CurrentChunk,m_ChunkPosX+1,m_ChunkPosY) ) m_ChunkPosX++;
    }
}

void byoCBTris::UpdateChunkPosUp()
{
    bool IsShift = ::wxGetKeyState(WXK_SHIFT);

    if ( m_IsUp )
    {
        ChunkConfig newChunk;
        if (IsShift)
            RotateChunkLeft(m_CurrentChunk,newChunk);
        else
            RotateChunkRight(m_CurrentChunk,newChunk);
        if ( !CheckChunkColision(newChunk,m_ChunkPosX,m_ChunkPosY) ) memcpy(m_CurrentChunk,newChunk,sizeof(newChunk));
        else if ( !CheckChunkColision(newChunk,m_ChunkPosX-1,m_ChunkPosY) )
        {
            memcpy(m_CurrentChunk,newChunk,sizeof(newChunk));
            m_ChunkPosX--;
        }
        else if ( !CheckChunkColision(newChunk,m_ChunkPosX+1,m_ChunkPosY) )
        {
            memcpy(m_CurrentChunk,newChunk,sizeof(newChunk));
            m_ChunkPosX++;
        }
    }
}

void byoCBTris::UpdateChunkPosDown()
{
    if ( m_IsDown )
    {
        if ( CheckChunkColision(m_CurrentChunk,m_ChunkPosX,m_ChunkPosY+1) )
        {
            // Raising speed timer little bit faster
            StartTimerNow(SpeedTimer);
        }
        else
        {
            m_ChunkPosY++;
            SpeedTimer.Start();
        }
    }
}

void byoCBTris::DrawStats(wxDC* DC)
{
    DC->SetTextForeground(*wxWHITE);
    DC->SetTextBackground(*wxBLACK);
    DC->SetFont(m_Font);
    wxString Line1 = wxString::Format(_("Score: %d"),m_Score);
    wxString Line2 = wxString::Format(_("Level: %d"),m_Level);
    wxString Line3 = GetBackToWorkString();
    DC->DrawText(Line1,5,5);
    int xs, ys;
    DC->GetTextExtent(Line1,&xs,&ys);
    DC->DrawText(Line2,5,5+2*ys);
    DC->DrawText(Line3,5,5+6*ys);
    if ( IsPaused() )
    {
        DC->DrawText(_("Paused"),5,5+4*ys);
    }
}

void byoCBTris::AddRemovedLines(int removed)
{
    m_TotalRemovedLines += removed;

    int shouldBeLevel = (m_TotalRemovedLines/20) + 1;
    if ( shouldBeLevel != m_Level )
    {
        m_Level = shouldBeLevel;
        SetSpeed();
    }
}

void byoCBTris::StartTimerNow(wxTimer& timer)
{
    timer.Notify();
    timer.Start();
}

int byoCBTris::GetScoreScale() const
{
    return m_Guidelines ? 5 : 10;
}

void byoCBTris::OnKillFocus(wxFocusEvent& event)
{
    m_IsLeft = false;
    m_IsRight = false;
    m_IsUp = false;
    m_IsDown = false;
    event.Skip();
}

BYO_REGISTER_GAME(byoCBTris,"C::B-Tris")
