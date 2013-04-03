/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */
#ifndef _RECENT_ITEMS_LIST_H_
#define _RECENT_ITEMS_LIST_H_

class wxFileHistory;

class RecentItemsList
{
    public:
        RecentItemsList(const wxString &configPath, int menuID, int firstMenuItemID);
        void AddToHistory(wxMenu* menu, const wxString& FileName);

        void AskToRemoveFileFromHistory(size_t id, bool cannot_open = true);
        wxString GetHistoryFile(size_t id) const;

        void ClearHistory(wxMenu *menu);

        void Initialize(wxMenu *menu);
        void TerminateHistory(wxMenu *menu);

        const wxFileHistory* GetFileHistory() const { return m_list; }
        bool Empty() const { return !m_list || m_list->GetCount() == 0; }
    private:
        wxFileHistory* m_list;
        wxString m_configPath;
        int m_menuID, m_firstMenuItemID;
};

#endif // _RECENT_ITEMS_LIST_H_
