#ifndef CB_THREADSEARCH_LOGGER_STC_H
#define CB_THREADSEARCH_LOGGER_STC_H

#include "ThreadSearchLoggerBase.h"

class wxScintilla;

class ThreadSearchLoggerSTC : public ThreadSearchLoggerBase
{
public:
    ThreadSearchLoggerSTC(ThreadSearchView& threadSearchView, ThreadSearch& threadSearchPlugin,
                          InsertIndexManager::eFileSorting fileSorting, wxWindow* parent, long id);

    eLoggerTypes GetLoggerType() override;
    void OnThreadSearchEvent(const ThreadSearchEvent& event) override;
    void Clear() override;
    void OnSearchBegin(const ThreadSearchFindData& findData) override;
    void OnSearchEnd() override;
    wxWindow* GetWindow() override;
    void SetFocus() override;

protected:
    void ConnectEvents(wxEvtHandler* pEvtHandler) override;
    void DisconnectEvents(wxEvtHandler* pEvtHandler) override;

private:
    wxScintilla *m_stc;
};

#endif // CB_THREADSEARCH_LOGGER_STC_H
