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

RecentItemsList::RecentItemsList(const wxString &configPath, int menuID, int firstMenuItemID) :
    m_list(nullptr),
    m_configPath(configPath),
    m_menuID(menuID),
    m_firstMenuItemID(firstMenuItemID)
{
}

void RecentItemsList::AddToHistory(wxMenu* menu, const wxString& FileName)
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
    if (!menu)
        return;
    wxMenu* recentFiles = 0;
    menu->FindItem(m_menuID, &recentFiles);
    if (recentFiles)
    {
        while (recentFiles->GetMenuItemCount() > 1)
            recentFiles->Delete(recentFiles->GetMenuItems()[0]);
        if (m_list->GetCount() > 0)
        {
            recentFiles->InsertSeparator(0);
            for (size_t i = 0; i < m_list->GetCount(); ++i)
            {
                const wxString &name = wxString::Format(_T("&%lu "), static_cast<unsigned long>(i + 1))
                                       + m_list->GetHistoryFile(i);
                recentFiles->Insert(recentFiles->GetMenuItemCount() - 2, m_firstMenuItemID + i, name);
            }
        }
    }
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
        // update start here page
        EditorBase* sh = Manager::Get()->GetEditorManager()->GetEditor(g_StartHereTitle);
        if (sh)
            ((StartHerePage*)sh)->Reload();
    }
}

void RecentItemsList::ClearHistory(wxMenu *menu)
{
    while (m_list->GetCount())
        m_list->RemoveFileFromHistory(0);
    Manager::Get()->GetConfigManager(_T("app"))->DeleteSubPath(m_configPath);

    // update start here page
    Initialize(menu);
    EditorBase* sh = Manager::Get()->GetEditorManager()->GetEditor(g_StartHereTitle);
    if (sh)
        ((StartHerePage*)sh)->Reload();
}

void RecentItemsList::Initialize(wxMenu *menu)
{
    TerminateHistory(menu);

    m_list = new wxFileHistory(16, m_firstMenuItemID);

    if (!menu)
        return;
    wxMenu* recentFiles = 0;
    menu->FindItem(m_menuID, &recentFiles);
    if (recentFiles)
    {
        wxArrayString files = Manager::Get()->GetConfigManager(_T("app"))->ReadArrayString(m_configPath);
        for (int i = (int)files.GetCount() - 1; i >= 0; --i)
        {
            if (wxFileExists(files[i]))
                m_list->AddFileToHistory(files[i]);
        }
        if (m_list->GetCount() > 0)
        {
            recentFiles->InsertSeparator(0);
            for (size_t i = 0; i < m_list->GetCount(); ++i)
            {
                recentFiles->Insert(recentFiles->GetMenuItemCount() - 2, m_firstMenuItemID + i,
                    wxString::Format(_T("&%lu "), static_cast<unsigned long>(i + 1)) + m_list->GetHistoryFile(i));
            }
        }
    }
}


void RecentItemsList::TerminateHistory(wxMenu *menu)
{
    if (m_list)
    {
        wxArrayString files;
        for (unsigned int i = 0; i < m_list->GetCount(); ++i)
            files.Add(m_list->GetHistoryFile(i));
        Manager::Get()->GetConfigManager(_T("app"))->Write(m_configPath, files);

        if (menu)
        {
            wxMenu* recentFiles = 0;
            menu->FindItem(m_menuID, &recentFiles);
            if (recentFiles)
            {
                if (!Manager::IsAppShuttingDown())
                {
                    while (recentFiles->GetMenuItemCount() > 1)
                        recentFiles->Delete(recentFiles->GetMenuItems()[0]);
                }
                else
                    m_list->RemoveMenu(recentFiles);
            }
        }
        delete m_list;
        m_list = nullptr;
    }
}

