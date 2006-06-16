#include <sdk.h> // PCH support
#include <annoyingdialog.h>
#include "byogame.h"

////////////////////////////////////////////////////////////////////////////////
// BackToWorkTimer - START

static const int idTimer = wxNewId();

BEGIN_EVENT_TABLE(BackToWorkTimer, wxEvtHandler)
    EVT_TIMER(idTimer, BackToWorkTimer::OnTimer)
END_EVENT_TABLE()

BackToWorkTimer::BackToWorkTimer(size_t minutes)
    : m_Timeout(minutes * 60 * 1000),
    m_Timer(this, idTimer)
{
    m_Timer.Start(m_Timeout, wxTIMER_ONE_SHOT);
}

void BackToWorkTimer::OnTimer(wxTimerEvent& event)
{
    AnnoyingDialog dlg(_("Work reminder (stop playing games!)"),
                        _("Don't you think you had enough already?\nGet back to work, NOW!"),
                        wxART_WARNING,
                        AnnoyingDialog::OK,
                        wxID_OK);
    dlg.ShowModal();
    m_Timer.Start(m_Timeout, wxTIMER_ONE_SHOT);
}

// BackToWorkTimer - END
////////////////////////////////////////////////////////////////////////////////

byoGame::GamesT& byoGame::GetGames()
{
    static GamesT m_Games;
    return m_Games;
}

byoGame::byoGame(const wxString& Name): m_Name(Name)
{
    GetGames().Add(this);
}

byoGame::~byoGame()
{
    GetGames().Remove(this);
}
