#include <sdk.h>
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
#include "startherepage.h"
#include "appglobals.h"

wxString g_StartHereTitle = _("Start here");
int idStartHerePageLink = wxNewId();
int idStartHerePageVarSubst = wxNewId();
int idWin = wxNewId();

class MyHtmlWin : public wxHtmlWindow
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
                    ShellExecute(0,0,link.GetHref().c_str(),0,_T(""),SW_SHOWNORMAL);
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

    wxString resPath = ConfigManager::ReadDataPath();
	m_pWin = new MyHtmlWin(this, idWin, wxPoint(0,0), GetSize());

	// set default font sizes based on system default font size

    /* NOTE (mandrav#1#): wxWidgets documentation on wxHtmlWindow::SetFonts(),
    states that the sizes array accepts values from -2 to +4.
    My tests (under linux at least) have showed that it actually
    expects real point sizes. */

	wxFont systemFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	int sizes[7] = {};
	for (int i = 0; i < 7; ++i)
        sizes[i] = systemFont.GetPointSize();
	m_pWin->SetFonts(wxEmptyString, wxEmptyString, &sizes[0]);

    // must load the page this way because if we don't the image can't be found...
	m_pWin->LoadPage(resPath + _T("/start_here.zip#zip:start_here.html"));

    // alternate way to read the file so we can perform some search and replace
    // the C::B image referenced in the default start page can be found now
    // because we used LoadPage() above...
    wxString buf;
    wxFileSystem* fs = new wxFileSystem;
    wxFSFile* f = fs->OpenFile(resPath + _T("/start_here.zip#zip:start_here.html"));
    if (f)
    {
    	wxInputStream* is = f->GetStream();
    	char tmp[1024] = {};
    	while (!is->Eof() && is->CanRead())
    	{
    		memset(tmp, 0, sizeof(tmp));
    		is->Read(tmp, sizeof(tmp) - 1);
    		buf << _U((const char*)tmp);
    	}
        delete f;
    }
    else
        buf = _("<html><body><h1>Welcome to Code::Blocks!</h1><br>The default start page seems to be missing...</body></html>");
    delete fs;

    // perform var substitution
    buf.Replace(_T("CB_VAR_VERSION_VERB"), APP_ACTUAL_VERSION_VERB);
    buf.Replace(_T("CB_VAR_VERSION"), APP_ACTUAL_VERSION);
    m_pWin->SetPage(buf);

    // ask our parent to perform any more substitutions
    // (for unknown vars to this, as CB_VAR_HISTORY_FILE_*)
    // if the parent performs substitutions, it should call
    // SetPageContent() on this...
    if (m_pOwner)
    {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, idStartHerePageVarSubst);
        evt.SetString(buf);
        m_pOwner->ProcessEvent(evt); // direct call
    }

    bs->Add(m_pWin, 1, wxEXPAND);
    SetSizer(bs);
    SetAutoLayout(true);
}

StartHerePage::~StartHerePage()
{
	//dtor
	m_pWin->Destroy();
}

void StartHerePage::SetPageContent(const wxString& buffer)
{
    m_pWin->SetPage(buffer);
}

bool StartHerePage::LinkClicked(const wxHtmlLinkInfo& link)
{
    if (!m_pOwner)
        return true;

    if (link.GetHref().StartsWith(_T("CB_CMD_")))
    {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, idStartHerePageLink);
        evt.SetString(link.GetHref());
        wxPostEvent(m_pOwner, evt);
        return true;
    }
    return false;
}
