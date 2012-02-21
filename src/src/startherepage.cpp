/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#include <wx/wxhtml.h>
#include <wx/intl.h>
#include <wx/utils.h>
#include <wx/sizer.h>
#include <wx/settings.h>
#include <manager.h>
#include <logmanager.h>
#include <projectmanager.h>
#include <templatemanager.h>
#include <pluginmanager.h>
#include <editormanager.h>
#include <configmanager.h>
#include "startherepage.h"
#include "main.h"
#include "appglobals.h"
#include <wx/clipbrd.h>
#include <wx/dataobj.h>

const wxString g_StartHereTitle = _("Start here");
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
                    wxLaunchDefaultBrowser(link.GetHref());
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

    // avoid gtk-critical because of sizes less than -1 (can happen with wxAuiNotebook/cbAuiNotebook)
    wxSize size = GetSize();
    size.x = std::max(size.x, -1);
    size.y = std::max(size.y, -1);

    m_pWin = new MyHtmlWin(this, idWin, wxPoint(0,0), size);

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
            buf << cbC2U((const char*)tmp);
        }
        delete f;
    }
    else
        buf = _("<html><body><h1>Welcome to Code::Blocks!</h1><br>The default start page seems to be missing...</body></html>");
    delete fs;

    #if defined(_LP64) || defined(_WIN64)
    const int bit_type = 64;
    #else
    const int bit_type = 32;
    #endif

    #ifdef __GNUC__
    revInfo.Printf(_T("%s (%s)   gcc %d.%d.%d %s/%s - %d bit"),
                    appglobals::AppActualVersionVerb.c_str(), ConfigManager::GetSvnDate().c_str(),
                    __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__, appglobals::AppPlatform.c_str(), appglobals::AppWXAnsiUnicode.c_str(), bit_type);
    #else
    revInfo.Printf(_T("%s (%s)   %s/%s"),
                    appglobals::AppActualVersionVerb.c_str(), ConfigManager::GetSvnDate().c_str(),
                    appglobals::AppPlatform.c_str(), appglobals::AppWXAnsiUnicode.c_str());
    #endif
    // perform var substitution
    buf.Replace(_T("CB_VAR_REVISION_INFO"), revInfo);
    buf.Replace(_T("CB_VAR_VERSION_VERB"), appglobals::AppActualVersionVerb);
    buf.Replace(_T("CB_VAR_VERSION"), appglobals::AppActualVersion);
    buf.Replace(_T("CB_SAFE_MODE"), PluginManager::GetSafeMode() ? _("SAFE MODE") : _T(""));
    m_pWin->SetPage(buf);

    m_OriginalPageContent = buf; // keep a copy of original for Reload()
    Reload();

    bs->Add(m_pWin, 1, wxEXPAND);
    SetSizer(bs);
    SetAutoLayout(true);
}

StartHerePage::~StartHerePage()
{
    //dtor
    //m_pWin->Destroy();
}

void StartHerePage::Reload()
{
    // ask our parent to perform any more substitutions
    // (for unknown vars to this, as CB_VAR_HISTORY_FILE_*)
    // if the parent performs substitutions, it should call
    // SetPageContent() on this...
    if (m_pOwner)
    {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, idStartHerePageVarSubst);
        evt.SetString(m_OriginalPageContent);
        m_pOwner->ProcessEvent(evt); // direct call
    }
}

void StartHerePage::SetPageContent(const wxString& buffer)
{
    m_pWin->SetPage(buffer);
}

bool StartHerePage::LinkClicked(const wxHtmlLinkInfo& link)
{
    //If it's already loading something, stop here
    if (Manager::Get()->GetProjectManager()->IsLoading())
        return true;
    if (!m_pOwner)
        return true;

    wxString href = link.GetHref();
    if (href.StartsWith(_T("CB_CMD_")))
    {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, idStartHerePageLink);
        evt.SetString(link.GetHref());
        wxPostEvent(m_pOwner, evt);
        return true;
    }

    if (   href.IsSameAs(_T("http://www.codeblocks.org/"))
        || href.StartsWith(_T("http://developer.berlios.de/bugs/")) )
    {
        wxTextDataObject *data = new wxTextDataObject(revInfo);
        if (wxTheClipboard->Open())
        {
            wxTheClipboard->SetData(data);
            wxTheClipboard->Close();
        }
    }

    if(href.IsSameAs(_T("rev")))
    {
        wxTextDataObject *data = new wxTextDataObject(revInfo);
        if (wxTheClipboard->Open())
        {
            wxTheClipboard->SetData(data);
            wxTheClipboard->Close();
        }
        return true;
    }

    return false;
}
