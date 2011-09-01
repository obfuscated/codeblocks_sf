#include "sdk.h"
#ifndef CB_PRECOMP
#include <wx/event.h>
#include <wx/image.h>
#endif
#include <wx/bitmap.h>
#include <wx/dc.h>
#include <wx/dcbuffer.h>
#include "byosnake.h"
#include "byogame.h"

BEGIN_EVENT_TABLE(byoSnake,byoGameBase)
    EVT_KEY_DOWN(byoSnake::OnKeyDown)
    EVT_PAINT(byoSnake::OnPaint)
    EVT_TIMER(-1,byoSnake::OnTimer)
    EVT_ERASE_BACKGROUND(byoSnake::OnEraseBack)
END_EVENT_TABLE()

byoSnake::byoSnake(wxWindow* parent,const wxString& GameName):
    byoGameBase(parent,GameName),
    m_AppleX(0),
    m_AppleY(0),
    m_SnakeLen(4),
    m_Delay(250),
    m_Lives(3),
    m_Score(0),
    m_InitialSlowdownCnt(0),
    m_KillCnt(0),
    m_Font(GetFont()),
    m_Timer(this,-1),
    m_Direction(dDown)
{
    RecalculateSizeHints(m_FieldHoriz+2,m_FieldVert+4);
    InitializeSnake();
    RandomizeApple();
    StartSnake();
}

byoSnake::~byoSnake()
{
}

void byoSnake::OnKeyDown(wxKeyEvent& event)
{
    if ( event.GetKeyCode() == 'p' || event.GetKeyCode() == 'P' )
    {
        SetPause(!IsPaused());
        Refresh();
    }

    if ( IsPaused() ) return;

    if ( event.GetKeyCode() == WXK_LEFT )
    {
        m_Direction = dLeft;
        Move();
    }

    if ( event.GetKeyCode() == WXK_RIGHT )
    {
        m_Direction = dRight;
        Move();
    }

    if ( event.GetKeyCode() == WXK_UP )
    {
        m_Direction = dUp;
        Move();
    }

    if ( event.GetKeyCode() == WXK_DOWN )
    {
        m_Direction = dDown;
        Move();
    }
}

void byoSnake::OnPaint(wxPaintEvent& /*event*/)
{
    wxSize size = GetClientSize();
    wxBitmap buffer(wxImage(size.GetWidth(),size.GetHeight()));
    wxBufferedPaintDC DC(this,buffer);
    DrawBorder(&DC);
    DrawSnake(&DC);
    DrawApple(&DC);
    DrawStats(&DC);
}

void byoSnake::OnEraseBack(wxEraseEvent& /*event*/)
{
}

void byoSnake::OnTimer(wxTimerEvent& /*event*/)
{
    Move();
}

void byoSnake::InitializeSnake()
{
    for ( int i=0; i<m_SnakeLen; i++ )
    {
        m_SnakeX[i] = m_FieldHoriz/2;
        m_SnakeY[i] = 0;
    }
    m_InitialSlowdownCnt = 2;
    m_Direction = dDown;
    RebuildField();
    UpdateSpeed();
}

void byoSnake::RandomizeApple()
{
    if ( m_SnakeLen == m_FieldTotal )
    {
        m_AppleX = -1;
        m_AppleY = -1;
        return;
    }

    int randRange = m_FieldTotal - m_SnakeLen;
    int randNum = ( (int) ( (float)rand() * (float)randRange / (float)RAND_MAX ) ) % randRange;

    m_AppleX = 0;
    m_AppleY = 0;

    while ( randNum-- > 0 )
    {
        do
        {
            m_AppleX++;
            if ( m_AppleX >= m_FieldHoriz )
            {
                m_AppleX = 0;
                m_AppleY++;
                if ( m_AppleY >= m_FieldVert )
                {
                    m_AppleX = -1;
                    m_AppleY = -1;
                    return;
                }
            }
        }
        while ( m_Field[m_AppleX][m_AppleY] );
    }

}

void byoSnake::StartSnake()
{
    m_Timer.Start(m_Delay,true);
    m_KillCnt = 0;
}

void byoSnake::RebuildField()
{
    memset(m_Field,0,sizeof(m_Field));
    for ( int i=0; i<m_SnakeLen; i++ )
    {
        m_Field[m_SnakeX[i]][m_SnakeY[i]] = true;
    }
}

void byoSnake::Move()
{
    if ( IsPaused() )
    {
        Refresh();
        m_Timer.Start(-1,true);
        return;
    }

    // Calculating new position

    if ( m_InitialSlowdownCnt )
    {
        m_InitialSlowdownCnt--;
        m_Timer.Start(-1,true);
        return;
    }

    int newX = m_SnakeX[0];
    int newY = m_SnakeY[0];

    switch ( m_Direction )
    {
        case dLeft:  newX--; break;
        case dRight: newX++; break;
        case dUp:    newY--; break;
        case dDown:  newY++; break;
    }

    bool valid = true;
    if ( newX<0 || newX>=m_FieldHoriz ) valid = false;
    if ( newY<0 || newY>=m_FieldVert  ) valid = false;

    for ( int i=0; valid && i<m_SnakeLen-1; i++ )
    {
        if ( m_SnakeX[i] == newX && m_SnakeY[i] == newY ) valid = false;
    }

    if ( !valid )
    {
        if ( ++m_KillCnt >= m_MaxKillCnt )
        {
            Died();
        }
        else
        {
            m_Timer.Start(-1,true);
        }
        Refresh();
        return;
    }

    m_KillCnt = 0;

    if ( newX == m_AppleX && newY == m_AppleY )
    {
        GetsBigger();
    }

    // Shifting snake
    for ( int i=m_SnakeLen; i-->0;  )
    {
        m_SnakeX[i] = m_SnakeX[i-1];
        m_SnakeY[i] = m_SnakeY[i-1];
    }

    m_SnakeX[0] = newX;
    m_SnakeY[0] = newY;

    RebuildField();
    if ( newX == m_AppleX && newY == m_AppleY )
    {
        RandomizeApple();
    }
    else
    {
        m_Score -= m_Delay / 10;
        if ( m_Score <0 ) m_Score = 0;
    }

    Refresh();

    m_Timer.Start(-1,true);
}

void byoSnake::Died()
{
    if ( --m_Lives == 0 )
    {
        Refresh();
        GameOver();
        return;
    }

    InitializeSnake();
    RandomizeApple();
    StartSnake();
}

void byoSnake::GameOver()
{
    ::wxMessageBox(_("Game over"));
}

void byoSnake::DrawBorder(wxDC* DC)
{
    for ( int i=0; i<m_FieldHoriz+2; i++ )
    {
        DrawBrick(DC,i,2,GetColour(m_BorderColour));
        DrawBrick(DC,i,3+m_FieldVert,GetColour(m_BorderColour));
    }
    for ( int i=0; i<m_FieldVert; i++ )
    {
        DrawBrick(DC,0,i+3,GetColour(m_BorderColour));
        DrawBrick(DC,m_FieldHoriz+1,i+3,GetColour(m_BorderColour));
    }
}

void byoSnake::DrawSnake(wxDC* DC)
{
    for ( int i=0; i<m_SnakeLen; i++ )
    {
        DrawBrick(DC,m_SnakeX[i]+1,m_SnakeY[i]+3,GetColour(m_SnakeColour));
    }
}

void byoSnake::DrawApple(wxDC* DC)
{
    if ( m_AppleX >= 0 && m_AppleY >= 0 )
    {
        DrawBrick(DC,m_AppleX+1,m_AppleY+3,GetColour(m_AppleColour));
    }
}

void byoSnake::DrawStats(wxDC* DC)
{
    DC->SetTextForeground(*wxWHITE);
    DC->SetTextBackground(*wxBLACK);
    DC->SetFont(m_Font);
    wxString Line1 = wxString::Format(_("Lives: %d    Score: %d   Length: %d"),m_Lives,m_Score,m_SnakeLen);
    #if wxCHECK_VERSION(2, 9, 0)
    wxString Line2 = IsPaused() ? wxString(_("Paused")) : wxString(wxEmptyString);
    #else
    wxString Line2 = IsPaused() ? _("Paused") : wxEmptyString;
    #endif
    wxString Line3 = GetBackToWorkString();
    DC->DrawText(Line1,5,5);
    int xs, ys;
    DC->GetTextExtent(Line1,&xs,&ys);
    DC->DrawText(Line2,5,5+2*ys);
    DC->DrawText(Line3,5,5+4*ys);
}

void byoSnake::GetsBigger()
{
    m_SnakeLen++;
    m_Score += 1000;
    UpdateSpeed();
}

void byoSnake::UpdateSpeed()
{
    int level = (m_SnakeLen / 10)+1;
    if ( level > 11 ) level = 11;
    m_Delay = 250 - 20 * level;
    m_Timer.Start(m_Delay,true);
}

BYO_REGISTER_GAME(byoSnake,"C::B Snake")
