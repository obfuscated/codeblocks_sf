#include <sdk.h> // Code::Blocks SDK
#ifndef CB_PRECOMP
    #include <wx/wxscintilla.h>
#endif

#include "ThreadSearchEvent.h"
#include "ThreadSearchFindData.h"
#include "ThreadSearchLoggerSTC.h"

ThreadSearchLoggerSTC::ThreadSearchLoggerSTC(ThreadSearchView& threadSearchView,
                                             ThreadSearch& threadSearchPlugin,
                                             InsertIndexManager::eFileSorting fileSorting,
                                             wxWindow* parent, long id) :
    ThreadSearchLoggerBase(parent, threadSearchView, threadSearchPlugin, fileSorting)
{
    m_stc = new wxScintilla(this, id, wxDefaultPosition, wxDefaultSize);
    m_stc->SetCaretLineVisible(true);

    SetupSizer(m_stc);
}

ThreadSearchLoggerBase::eLoggerTypes ThreadSearchLoggerSTC::GetLoggerType()
{
    return TypeSTC;
}

void ThreadSearchLoggerSTC::OnThreadSearchEvent(const ThreadSearchEvent& event)
{
    const wxArrayString& words = event.GetLineTextArray();
    const wxString &filename = event.GetString();

    m_stc->Freeze();
    m_stc->SetReadOnly(false);
    m_stc->AppendText(filename + wxT('\n'));

    wxString justifier;

    for (size_t ii = 0; ii + 1 < words.GetCount(); ii += 2)
    {
        justifier.clear();
        if (words[ii].length() < 10)
        {
            justifier.Append(wxT(' '), 10 - words[ii].length());
        }

        m_stc->AppendText(justifier + words[ii] + wxT(":\t") + words[ii + 1].Trim().Trim(false) + wxT('\n'));
    }

    m_stc->SetReadOnly(true);
    m_stc->Thaw();
}

void ThreadSearchLoggerSTC::Clear()
{
    m_stc->Clear();
}

void ThreadSearchLoggerSTC::OnSearchBegin(const ThreadSearchFindData& findData)
{
    m_stc->SetReadOnly(false);
    m_stc->AppendText(wxT("=> Searching for ") + findData.GetFindText() + wxT('\n'));
    m_stc->SetReadOnly(true);
}

void ThreadSearchLoggerSTC::OnSearchEnd()
{
    m_stc->SetReadOnly(false);
    m_stc->AppendText(wxT("=> Searching finished!\n"));
    m_stc->SetReadOnly(true);
}

wxWindow* ThreadSearchLoggerSTC::GetWindow()
{
    return m_stc;
}

void ThreadSearchLoggerSTC::SetFocus()
{
    m_stc->SetFocus();
}

void ThreadSearchLoggerSTC::ConnectEvents(wxEvtHandler* pEvtHandler)
{
}

void ThreadSearchLoggerSTC::DisconnectEvents(wxEvtHandler* pEvtHandler)
{
}
