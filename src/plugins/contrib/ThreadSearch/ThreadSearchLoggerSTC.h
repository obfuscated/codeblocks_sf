#ifndef CB_THREADSEARCH_LOGGER_STC_H
#define CB_THREADSEARCH_LOGGER_STC_H

#include "ThreadSearchLoggerBase.h"

class wxScintilla;

class ThreadSearchLoggerSTC : public ThreadSearchLoggerBase
{
public:
    ThreadSearchLoggerSTC(ThreadSearchView& threadSearchView, ThreadSearch& threadSearchPlugin,
                          InsertIndexManager::eFileSorting fileSorting, wxWindow* parent, long id);

    static void RegisterColours();

    eLoggerTypes GetLoggerType() override;
    void OnThreadSearchEvent(const ThreadSearchEvent& event) override;
    void Clear() override;
    void OnSearchBegin(const ThreadSearchFindData& findData) override;
    void OnSearchEnd() override;
    wxWindow* GetWindow() override;
    void SetFocus() override;

private:
    void ConnectEvents(wxEvtHandler* pEvtHandler) override;
    void DisconnectEvents(wxEvtHandler* pEvtHandler) override;

    void AppendStyledText(int style, const wxString &text);
private:
    wxScintilla *m_stc;
    int m_fileCount;
    int m_totalCount;
    int m_startLine;
};

#endif // CB_THREADSEARCH_LOGGER_STC_H
