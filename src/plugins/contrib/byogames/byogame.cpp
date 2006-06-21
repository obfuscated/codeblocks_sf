#include <sdk.h> // PCH support
#ifndef CB_PRECOMP
#include <wx/sizer.h>
#endif
#include <annoyingdialog.h>
#include <manager.h>
#include <editormanager.h>
#include "byogame.h"

byoGameLauncher::GamesT& byoGameLauncher::GetGames()
{
    static GamesT m_Games;
    return m_Games;
}

byoGameLauncher::byoGameLauncher(const wxString& Name): m_Name(Name)
{
    GetGames().Add(this);
}

byoGameLauncher::~byoGameLauncher()
{
    GetGames().Remove(this);
}

//////////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(byoEditorBase,EditorBase)
    EVT_SET_FOCUS(byoEditorBase::OnSetFocus)
END_EVENT_TABLE()

byoEditorBase::byoEditorBase(const wxString& GameName):
    EditorBase((wxWindow*)Manager::Get()->GetEditorManager()->GetNotebook(),GameName)
{
}

byoEditorBase::~byoEditorBase()
{
}

void byoEditorBase::AddGameContent(byoGameBase* base)
{
    wxSizer* Sizer = new wxBoxSizer(wxHORIZONTAL);
    m_Shortname = GetFilename();
    SetTitle(GetFilename());
    m_Content = base;
    Sizer->Add(m_Content,1,wxEXPAND);
    SetSizer(Sizer);
    Layout();
    m_Content->SetFocus();
}

