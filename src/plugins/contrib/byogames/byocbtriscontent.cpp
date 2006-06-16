#include <sdk.h>
#include "byocbtriscontent.h"
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


BEGIN_EVENT_TABLE(byoCBTrisContent,wxWindow)
    EVT_PAINT(byoCBTrisContent::OnPaint)
    EVT_CHAR(byoCBTrisContent::OnChar)
    EVT_TIMER(1,byoCBTrisContent::OnSpeedTimer)
    EVT_TIMER(2,byoCBTrisContent::OnLeftRightTimer)
    EVT_TIMER(3,byoCBTrisContent::OnUpTimer)
    EVT_TIMER(4,byoCBTrisContent::OnDownTimer)
    EVT_SIZE(byoCBTrisContent::OnSize)
    EVT_ERASE_BACKGROUND(byoCBTrisContent::OnEraseBack)
    EVT_SET_FOCUS(byoCBTrisContent::OnSetFocus)
    EVT_KILL_FOCUS(byoCBTrisContent::OnKillFocus)
END_EVENT_TABLE()

byoCBTrisContent::byoCBTrisContent(wxWindow* parent,wxWindowID id):
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
    m_Paused(false),
    totalRemovedLines(0)
{
	Create(parent,id,wxDefaultPosition,wxDefaultSize,wxWANTS_CHARS);
	m_Font = wxSystemSettings::GetFont(wxSYS_OEM_FIXED_FONT);
	LeftRightTimer.Start(100);
	UpTimer.Start(1000);
	DownTimer.Start(20);
	SetSpeed();
	RecalculateScale();

    memset(m_Content,0,sizeof(m_Content));
    memset(m_CurrentChunk,0,sizeof(m_CurrentChunk));
    memset(m_NextChunk,0,sizeof(m_NextChunk));

    RandomizeChunk(m_NextChunk);
    GenerateNewChunk();
}

byoCBTrisContent::~byoCBTrisContent()
{
}

void byoCBTrisContent::OnPaint(wxPaintEvent& event)
{
    wxPaintDC PaintDC(this);
    wxBufferedDC DC(&PaintDC,GetClientSize());

    DrawBrickField(&DC);
    DrawCurrentChunk(&DC);
    DrawNextChunk(&DC);
    DrawStats(&DC);
}

void byoCBTrisContent::OnChar(wxKeyEvent& event)
{
    if ( event.GetKeyCode() == 'p' || event.GetKeyCode() == 'P' )
    {
        m_Paused = !m_Paused;
        Refresh();
    }

    if ( m_Paused ) return;

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

void byoCBTrisContent::OnLeftRightTimer(wxTimerEvent& event)
{
    if ( m_Paused ) return;
    m_LeftRightCnt++;
    UpdateChunkPos();
    Refresh();
}

void byoCBTrisContent::OnUpTimer(wxTimerEvent& event)
{
    if ( m_Paused ) return;
    m_UpCnt++;
    UpdateChunkPos();
    Refresh();
}

void byoCBTrisContent::OnDownTimer(wxTimerEvent& event)
{
    if ( m_Paused ) return;
    m_DownCnt++;
    UpdateChunkPos();
    Refresh();
}

void byoCBTrisContent::OnSpeedTimer(wxTimerEvent& event)
{
    if ( m_Paused ) return;
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

void byoCBTrisContent::OnSize(wxSizeEvent& event)
{
    RecalculateScale();
    event.Skip();
}

void byoCBTrisContent::OnEraseBack(wxEraseEvent& event)
{
}

void byoCBTrisContent::DrawBrick(wxDC* DC,int posX,int posY,const wxColour& colour)
{
    wxColour darker(colour.Red()/2,colour.Green()/2,colour.Blue()/2);
    wxColour brighter(darker.Red()+0x80,darker.Green()+0x80,darker.Blue()+0x80);

    DC->SetPen(brighter);
    DC->SetBrush(colour);
    DC->DrawRectangle(posX,posY,m_BricksSize,m_BricksSize);
    for ( int i=0; i<m_BricksBorderSize; i++ )
    {
        DC->SetPen(brighter);
        DC->DrawLine(posX+i,posY+i,posX+m_BricksSize-i,posY+i);
        DC->DrawLine(posX+i,posY+i,posX+i,posY+m_BricksSize-i);
        DC->SetPen(darker);
        DC->DrawLine(posX+m_BricksSize-1-i,posY+m_BricksSize-1-i,posX+i-1,posY+m_BricksSize-1-i);
        DC->DrawLine(posX+m_BricksSize-1-i,posY+m_BricksSize-1-i,posX+m_BricksSize-1-i,posY+i);
    }
}

static const wxColour colours[] =
{
    wxColour(0xFF,0,0),
    wxColour(0,0xFF,0),
    wxColour(0,0,0xFF),
    wxColour(0xFF,0xFF,0),
    wxColour(0xFF,0,0xFF),
    wxColour(0,0xFF,0xFF)
};

void byoCBTrisContent::DrawBrickField(wxDC* DC)
{
    static const wxColour Border(0x40,0x40,0x40);

    for ( int x=0; x<bricksHoriz; x++ )
        for ( int y=0; y<bricksVert; y++ )
            if ( m_Content[x][y] )
            {
                DrawBrick(DC,m_BricksPosX+m_BricksSize*x,m_BricksPosY+m_BricksSize*y,colours[m_Content[x][y]%6]);
            }
    for ( int y=0; y<bricksVert; y++ )
    {
        DrawBrick(DC,m_BricksPosX-m_BricksSize,m_BricksPosY+m_BricksSize*y,Border);
        DrawBrick(DC,m_BricksPosX+m_BricksSize*bricksHoriz,m_BricksPosY+m_BricksSize*y,Border);
    }
    for ( int x=-1; x<=bricksHoriz; x++ )
    {
        DrawBrick(DC,m_BricksPosX+x*m_BricksSize,m_BricksPosY+m_BricksSize*bricksVert,Border);
    }
}

void byoCBTrisContent::DrawCurrentChunk(wxDC* DC)
{
    int* Ptr = m_CurrentChunk;
    for ( int y=0; y<4; y++ )
        for ( int x=0; x<4; x++ )
        {
            if ( *Ptr )
                DrawBrick(DC,
                    m_BricksPosX+m_BricksSize*(x+m_ChunkPosX),
                    m_BricksPosY+m_BricksSize*(y+m_ChunkPosY),
                    colours[*Ptr%6]);
            Ptr++;
        }
}

void byoCBTrisContent::DrawNextChunk(wxDC* DC)
{
    int* Ptr = m_NextChunk;
    for ( int y=0; y<4; y++ )
        for ( int x=0; x<4; x++ )
        {
            if ( *Ptr )
                DrawBrick(DC,
                    m_NewChunkPosX+m_BricksSize*x,
                    m_NewChunkPosY+m_BricksSize*y,
                    colours[*Ptr%6]);
            Ptr++;
        }
}

void byoCBTrisContent::RecalculateScale()
{
    wxSize Size = GetClientSize();

    int LessSize = (Size.GetWidth() < Size.GetHeight()) ? Size.GetWidth() : Size.GetHeight();

    m_BricksSize = LessSize / (bricksVert+1);
    m_BricksBorderSize = m_BricksSize / 8;
    if ( m_BricksBorderSize < 1 ) m_BricksBorderSize = 1;
    if ( m_BricksSize - 2*m_BricksBorderSize < 1 ) m_BricksSize = 2*m_BricksBorderSize + 1;

    m_BricksPosX = (Size.GetWidth()-bricksHoriz*(m_BricksSize)) / 2;
    m_BricksPosY = (Size.GetHeight()-(bricksVert+1)*(m_BricksSize)) / 2;

    m_NewChunkPosX = m_BricksPosX - 6*m_BricksSize;
    m_NewChunkPosY = m_BricksPosY + (bricksVert-4)*m_BricksSize;
}

void byoCBTrisContent::RandomizeChunk(ChunkConfig& chunk,int color)
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

void byoCBTrisContent::RotateChunkLeft(const ChunkConfig& chunk,ChunkConfig& newChunk)
{
    for ( int y=0; y<4; y++ )
        for ( int x=0; x<4; x++ )
            newChunk[x+4*y] = chunk[(3-x)*4+y];

    AlignChunk(newChunk);
}

void byoCBTrisContent::RotateChunkRight(const ChunkConfig& chunk,ChunkConfig& newChunk)
{
    for ( int y=0; y<4; y++ )
        for ( int x=0; x<4; x++ )
            newChunk[x+4*y] = chunk[x*4+3-y];

    AlignChunk(newChunk);
}

void byoCBTrisContent::AlignChunk(ChunkConfig& chunk)
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

bool byoCBTrisContent::CheckChunkColision(const ChunkConfig& chunk,int posX,int posY)
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

void byoCBTrisContent::SetSpeed()
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

bool byoCBTrisContent::ChunkDown()
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

void byoCBTrisContent::RemoveFullLines()
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

bool byoCBTrisContent::GenerateNewChunk()
{
    memcpy(m_CurrentChunk,m_NextChunk,sizeof(m_CurrentChunk));
    RandomizeChunk(m_NextChunk);

    m_ChunkPosY = 0;
    m_ChunkPosX = (bricksHoriz-4)/2;
    return !CheckChunkColision(m_CurrentChunk,m_ChunkPosX,m_ChunkPosY);
}

void byoCBTrisContent::GameOver()
{
    Refresh();
    SpeedTimer.Stop();
    LeftRightTimer.Stop();
    DownTimer.Stop();
    UpTimer.Stop();
    wxMessageBox(_("Game over"));
}

void byoCBTrisContent::UpdateChunkPos()
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

void byoCBTrisContent::DrawStats(wxDC* DC)
{
    DC->SetTextForeground(*wxWHITE);
    DC->SetTextBackground(*wxBLACK);
    DC->SetFont(m_Font);
    wxString Line1 = wxString::Format(_("Score: %d"),m_Score);
    wxString Line2 = wxString::Format(_("Level: %d"),m_Level);
    DC->DrawText(Line1,5,5);
    int xs, ys;
    DC->GetTextExtent(Line1,&xs,&ys);
    DC->DrawText(Line2,5,5+2*ys);
    if ( m_Paused )
    {
        DC->DrawText(_("Paused"),5,5+4*ys);
    }
}

void byoCBTrisContent::AddRemovedLines(int removed)
{
    totalRemovedLines += removed;

    int shouldBeLevel = (totalRemovedLines/20) + 1;
    if ( shouldBeLevel != m_Level )
    {
        m_Level = shouldBeLevel;
        SetSpeed();
    }
}

void byoCBTrisContent::OnSetFocus(wxFocusEvent& event)
{
//    m_Paused = false;
}

void byoCBTrisContent::OnKillFocus(wxFocusEvent& event)
{
    m_Paused = true;
}
