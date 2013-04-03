/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */


#include <sdk.h>

#include <wx/docview.h>

#include "recentitemslist.h"

#include "annoyingdialog.h"
#include "startherepage.h"

RecentItemsList::RecentItemsList(const wxString &menuName, const wxString &configPath, int menuID, int firstMenuItemID) :
    m_list(nullptr),
    m_menuName(menuName),
    m_configPath(configPath),
    m_menuID(menuID),
    m_firstMenuItemID(firstMenuItemID)
{
}

void RecentItemsList::AddToHistory(const wxString& FileName)
{
    wxString filename = FileName;
#ifdef __WXMSW__
    // for windows, look for case-insensitive matches
    // if found, don't add it
    wxString low = filename.Lower();
    for (size_t i = 0; i < m_list->GetCount(); ++i)
    {
        if (low == m_list->GetHistoryFile(i).Lower())
        {    // it exists, set filename to the existing name, so it can become
            // the most recent one
            filename = m_list->GetHistoryFile(i);
            break;
        }
    }
#endif

    m_list->AddFileToHistory(filename);

    // because we append "clear history" menu to the end of the list,
    // each time we must add a history item we have to:
    // a) remove "Clear history" (Biplab#1: Don't remove or you'll loose icon)
    // b) clear the menu (Biplab#1: except the last item)
    // c) fill it with the history items (Biplab#1: by inserting them)
    // and d) append "Clear history"... (Biplab#1: Not needed, item has not been removed)
    wxMenu* recentFiles = GetMenu();
    if (recentFiles)
    {
        ClearMenu(recentFiles);
        BuildMenu(recentFiles);
    }
    RefreshStartHerePage();
}

wxString RecentItemsList::GetHistoryFile(size_t id) const
{
    return (m_list && id < m_list->GetCount()) ? m_list->GetHistoryFile(id) : wxString(wxEmptyString);
}


void RecentItemsList::AskToRemoveFileFromHistory(size_t id, bool cannot_open)
{
    if (!m_list || id >= m_list->GetCount())
        return;

    wxString question(_("Do you want to remove it from the recent files list?"));
    wxString query(wxEmptyString);
    if (cannot_open)
    {
        query << _("The file cannot be opened (probably it's not available anymore).")
              << _T("\n") << question;
    }
    else
        query << question;

    AnnoyingDialog dialog(_("Question"), query, wxART_QUESTION);
    PlaceWindow(&dialog);
    if (dialog.ShowModal() == wxID_YES)
    {
        m_list->RemoveFileFromHistory(id);
        wxMenu* recentFiles = GetMenu();
        if (recentFiles)
        {
            ClearMenu(recentFiles);
            BuildMenu(recentFiles);
        }
        RefreshStartHerePage();
    }
}

void RecentItemsList::ClearHistory()
{
    while (m_list->GetCount())
        m_list->RemoveFileFromHistory(0);
    Manager::Get()->GetConfigManager(_T("app"))->DeleteSubPath(m_configPath);

    Initialize();
    RefreshStartHerePage();
}

void RecentItemsList::Initialize()
{
    TerminateHistory();

    m_list = new wxFileHistory(16, m_firstMenuItemID);

    wxMenu* recentFiles = GetMenu();
    if (recentFiles)
    {
        wxArrayString files = Manager::Get()->GetConfigManager(_T("app"))->ReadArrayString(m_configPath);
        for (int i = (int)files.GetCount() - 1; i >= 0; --i)
        {
            if (wxFileExists(files[i]))
                m_list->AddFileToHistory(files[i]);
        }
        BuildMenu(recentFiles);
    }
}


void RecentItemsList::TerminateHistory()
{
    if (m_list)
    {
        wxArrayString files;
        for (unsigned int i = 0; i < m_list->GetCount(); ++i)
            files.Add(m_list->GetHistoryFile(i));
        Manager::Get()->GetConfigManager(_T("app"))->Write(m_configPath, files);

        wxMenu* recentFiles = GetMenu();
        if (recentFiles)
        {
            if (!Manager::IsAppShuttingDown())
                ClearMenu(recentFiles);
            else
                m_list->RemoveMenu(recentFiles);
        }
        delete m_list;
        m_list = nullptr;
    }
}

void RecentItemsList::BuildMenu(wxMenu *menu)
{
    if (m_list->GetCount() > 0)
    {
        menu->InsertSeparator(0);
        for (size_t i = 0; i < m_list->GetCount(); ++i)
        {
            const wxString &name = wxString::Format(_T("&%lu "), static_cast<unsigned long>(i + 1))
                                   + m_list->GetHistoryFile(i);
            menu->Insert(menu->GetMenuItemCount() - 2, m_firstMenuItemID + i, name);
        }
    }
}

void RecentItemsList::ClearMenu(wxMenu *menu)
{
    while (menu->GetMenuItemCount() > 1)
        menu->Delete(menu->GetMenuItems()[0]);
}

wxMenu* RecentItemsList::GetMenu()
{
    wxMenuBar *mbar = Manager::Get()->GetAppFrame()->GetMenuBar();
    if (!mbar)
        return nullptr;
    int pos = mbar->FindMenu(m_menuName);
    if (pos == wxNOT_FOUND)
        return nullptr;
    wxMenu *menu = mbar->GetMenu(pos);
    wxMenu *recentFiles;
    menu->FindItem(m_menuID, &recentFiles);
    return recentFiles;
}

void RecentItemsList::RefreshStartHerePage()
{
    // update start here page
    EditorBase* sh = Manager::Get()->GetEditorManager()->GetEditor(g_StartHereTitle);
    if (sh)
        ((StartHerePage*)sh)->Reload();
}
