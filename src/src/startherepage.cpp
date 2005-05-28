#include "startherepage.h"
#include <wx/wxhtml.h>
#include <wx/intl.h>
#include <wx/utils.h>
#include <wx/sizer.h>
#include <manager.h>
#include <messagemanager.h>
#include <projectmanager.h>
#include <templatemanager.h>
#include <pluginmanager.h>
#include <editormanager.h>
#include <configmanager.h>

wxString g_StartHereTitle = _("Start here");
int idStartHerePageLink = wxNewId();
int idWin = wxNewId();

class DLLIMPORT MyHtmlWin : public wxHtmlWindow
{
    public:
        MyHtmlWin(StartHerePage* parent, int id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxHW_SCROLLBAR_AUTO)
            : wxHtmlWindow(parent, id, pos, size, style),
            m_pOwner(parent)
        {
        }
        
        void OnLinkClicked(const wxHtmlLinkInfo& link)
        {
            if (m_pOwner)
            {
                if (!m_pOwner->LinkClicked(link))
                {
                #ifdef __WXMSW__
                    ShellExecute(0,0,link.GetHref().c_str(),0,"",SW_SHOWNORMAL);
                #else
                    wxMessageBox(_("We 're sorry but currently this is not supported in your platform"), _("Information"), wxICON_INFORMATION);
                #endif
                }
            }
        }
    private:
        StartHerePage* m_pOwner;
};

BEGIN_EVENT_TABLE(StartHerePage, EditorBase)
END_EVENT_TABLE()

StartHerePage::StartHerePage(wxEvtHandler* owner, wxWindow* parent)
    : EditorBase(parent, g_StartHereTitle),
    m_pOwner(owner)
{
	//ctor
    wxBoxSizer* bs = new wxBoxSizer(wxVERTICAL);

    wxString resPath = ConfigManager::Get()->Read("/data_path");
	m_pWin = new MyHtmlWin(this, idWin, wxPoint(0,0), GetSize());
	m_pWin->LoadPage(resPath + "/start_here.zip#zip:start_here.html");
//	m_pWin->SetPage("<html><body><h1>Welcome to Code::Blocks!</h1></body></html>");

    bs->Add(m_pWin, 1, wxEXPAND);
    SetSizer(bs);
    SetAutoLayout(true);
}

StartHerePage::~StartHerePage()
{
	//dtor
	m_pWin->Destroy();
}

bool StartHerePage::LinkClicked(const wxHtmlLinkInfo& link)
{
    if (!m_pOwner)
        return true;
    
    if (link.GetHref().StartsWith("CB_CMD_"))
    {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, idStartHerePageLink);
        evt.SetString(link.GetHref());
        wxPostEvent(m_pOwner, evt);
        return true;
    }
    return false;
}
