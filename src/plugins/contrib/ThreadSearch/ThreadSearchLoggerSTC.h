/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef CB_THREADSEARCH_LOGGER_STC_H
#define CB_THREADSEARCH_LOGGER_STC_H

#include "ThreadSearchLoggerBase.h"

#include <vector>

class wxScintillaEvent;

class STCList;

class ThreadSearchLoggerSTC : public ThreadSearchLoggerBase
{
public:
    ThreadSearchLoggerSTC(ThreadSearchView& threadSearchView, ThreadSearch& threadSearchPlugin,
                          InsertIndexManager::eFileSorting fileSorting, wxWindow* parent, long id);
    ~ThreadSearchLoggerSTC();

    static void RegisterColours();
    void SetupStyles();

    eLoggerTypes GetLoggerType() override;
    void OnThreadSearchEvent(const ThreadSearchEvent& event) override;
    void Clear() override;
    void OnSearchBegin(const ThreadSearchFindData& findData) override;
    void OnSearchEnd() override;
    wxWindow* GetWindow() override;
    void SetFocus() override;
    void UpdateSettings() override;
    void EditorLinesAddedOrRemoved(cbEditor *editor, int startLine, int linesAdded) override;

private:
    void ConnectEvents();
    void DisconnectEvents();

    void OnContextMenu(wxContextMenuEvent &event);
    void OnDoubleClick(wxScintillaEvent &event);
    void OnKeyDown(wxKeyEvent &event);
    void OnMarginClick(wxScintillaEvent &event);
    void OnStyleNeeded(wxScintillaEvent &event);
    void OnMenuCopy(wxCommandEvent &event);
    void OnMenuCopySelection(wxCommandEvent &event);
    void OnMenuCollapseFile(wxCommandEvent &event);
    void OnMenuCollapseSearch(wxCommandEvent &event);
    void OnMenuCollapseAll(wxCommandEvent &event);
    void OnMenuDelete(wxCommandEvent &event);
    void OnMenuDeleteAll(wxCommandEvent &event);
    void OnSTCUpdateUI(wxScintillaEvent &event);
    void OnSTCFocus(wxFocusEvent &event);

    void AppendStyleItem(int startPos, int endPos, int style);
    void AutoScroll();

    friend class STCList;
private:
    STCList *m_stc;
    int m_fileCount;
    int m_totalCount;
    int m_startLine;
    int m_lastVisibleLine;
    int m_lastLineMarkerHandle = -1;
    wxLongLong m_timeBegin;

    struct StyleItem
    {
        int startPos;
        int length;
        int style;

        static StyleItem make(int startPos)
        {
            StyleItem item;
            item.startPos = startPos;
            item.style = item.length = 0;
            return item;
        }

        bool operator<(const StyleItem &b) const
        {
            return startPos < b.startPos;
        }
    };

    typedef std::vector<StyleItem> StyleItemVector;
    StyleItemVector m_styles;
};

#endif // CB_THREADSEARCH_LOGGER_STC_H
