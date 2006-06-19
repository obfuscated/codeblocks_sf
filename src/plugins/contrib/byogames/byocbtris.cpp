#include <sdk.h>
#include "byocbtris.h"
#include <wx/dcbuffer.h>

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
}


BEGIN_EVENT_TABLE(byoCBTris,byoGameBase)
    EVT_PAINT(byoCBTris::OnPaint)
    EVT_CHAR(byoCBTris::OnChar)
    EVT_TIMER(1,byoCBTris::OnSpeedTimer)
    EVT_TIMER(2,byoCBTris::OnLeftRightTimer)
    EVT_TIMER(3,byoCBTris::OnUpTimer)
    EVT_TIMER(4,byoCBTris::OnDownTimer)
    EVT_ERASE_BACKGROUND(byoCBTris::OnEraseBack)
END_EVENT_TABLE()

byoCBTris::byoCBTris(wxWindow* parent,const wxString& Name):
    byoGameBase(parent,Name),
    SpeedTimer(this,1),
    LeftRightTimer(this,2),
    UpTimer(this,3),
    DownTimer(this,4),
    m_Level(1),
    m_Score(0),
    m_LeftRightCnt(0),
    m_UpCnt(0),
    m_DownCnt(0),
    m_WasLeft(false),
    m_WasRight(false),
    m_WasUp(false),
    m_WasDown(false),
    totalRemovedLines(0)
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

void byoCBTris::OnPaint(wxPaintEvent& event)
{
    wxBitmap buffer(GetClientSize().GetWidth(),GetClientSize().GetHeight());
    wxBufferedPaintDC DC(this,buffer);
    DrawBrickField(&DC);
    DrawCurrentChunk(&DC);
    DrawNextChunk(&DC);
    DrawStats(&DC);
}

void byoCBTris::OnChar(wxKeyEvent& event)
{
    if ( event.GetKeyCode() == 'p' || event.GetKeyCode() == 'P' )
    {
        SetPause(!IsPaused());
        Refresh();
    }

    if ( IsPaused() ) return;

    if ( ::wxGetKeyState(WXK_LEFT) && !m_WasLeft )
    {
        m_WasLeft = true;
        m_LeftRightCnt++;
        UpdateChunkPos();
        Refresh();
        LeftRightTimer.Start();
    }

    if ( ::wxGetKeyState(WXK_RIGHT) && !m_WasRight )
    {
        m_WasRight = true;
        m_LeftRightCnt++;
        UpdateChunkPos();
        Refresh();
        LeftRightTimer.Start();
    }

    if ( ::wxGetKeyState(WXK_UP) && !m_WasUp )
    {
        m_WasUp = true;
        m_UpCnt++;
        UpdateChunkPos();
        Refresh();
        UpTimer.Start();
    }

    if ( ::wxGetKeyState(WXK_DOWN) && !m_WasDown )
    {
        m_WasDown = true;
        m_UpCnt++;
        UpdateChunkPos();
        Refresh();
        DownTimer.Start();
    }

}

void byoCBTris::OnLeftRightTimer(wxTimerEvent& event)
{
    if ( IsPaused() ) return;
    m_LeftRightCnt++;
    UpdateChunkPos();
    Refresh();
}

void byoCBTris::OnUpTimer(wxTimerEvent& event)
{
    if ( IsPaused() ) return;
    m_UpCnt++;
    UpdateChunkPos();
    Refresh();
}

void byoCBTris::OnDownTimer(wxTimerEvent& event)
{
    if ( IsPaused() ) return;
    m_DownCnt++;
    UpdateChunkPos();
    Refresh();
}

void byoCBTris::OnSpeedTimer(wxTimerEvent& event)
{
    if ( IsPaused() ) return;
    if ( !ChunkDown() )
    {
        RemoveFullLines();
        if ( !GenerateNewChunk() )
        {
            GameOver();
        }
    }
    Refresh();
}

void byoCBTris::OnEraseBack(wxEraseEvent& event)
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
        m_Score += 10;
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

    m_Score += 100 * count*count;
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

void byoCBTris::UpdateChunkPos()
{
    bool IsLeft  = ::wxGetKeyState(WXK_LEFT);
    bool IsRight = ::wxGetKeyState(WXK_RIGHT);
    bool IsUp    = ::wxGetKeyState(WXK_UP);
    bool IsDown  = ::wxGetKeyState(WXK_DOWN);

    if ( IsLeft && !IsRight && m_LeftRightCnt)
    {
        if ( !CheckChunkColision(m_CurrentChunk,m_ChunkPosX-1,m_ChunkPosY) ) m_ChunkPosX--;
    }
    if ( !IsLeft && IsRight && m_LeftRightCnt)
    {
        if ( !CheckChunkColision(m_CurrentChunk,m_ChunkPosX+1,m_ChunkPosY) ) m_ChunkPosX++;
    }

    if ( IsUp && m_UpCnt )
    {
        ChunkConfig newChunk;
        RotateChunkLeft(m_CurrentChunk,newChunk);
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

    if ( IsDown && m_DownCnt )
    {
        if ( CheckChunkColision(m_CurrentChunk,m_ChunkPosX,m_ChunkPosY+1) )
        {
            // Raising speed timer little bit faster
            wxTimerEvent event;
            OnSpeedTimer(event);
            SpeedTimer.Start();
        }
        else
        {
            m_ChunkPosY++;
        }
    }

    m_LeftRightCnt = 0;
    m_UpCnt = 0;
    m_DownCnt = 0;
    m_WasLeft = IsLeft;
    m_WasRight = IsRight;
    m_WasUp = IsUp;
    m_WasDown = IsDown;
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
    totalRemovedLines += removed;

    int shouldBeLevel = (totalRemovedLines/20) + 1;
    if ( shouldBeLevel != m_Level )
    {
        m_Level = shouldBeLevel;
        SetSpeed();
    }
}

BYO_REGISTER_GAME(byoCBTris,"C::B-Tris")

