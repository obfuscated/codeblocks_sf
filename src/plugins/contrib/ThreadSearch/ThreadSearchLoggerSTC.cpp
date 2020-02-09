/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#include <sdk.h> // Code::Blocks SDK
#ifndef CB_PRECOMP
    #include <wx/wxscintilla.h>
#endif

#include "cbcolourmanager.h"
#include "editor_utils.h"

#include "ThreadSearchEvent.h"
#include "ThreadSearchFindData.h"
#include "ThreadSearchLoggerSTC.h"

namespace
{

enum STCStyles : int
{
    File = 2,
    LineNo,
    Text,
    TextMatching
};

const int C_FOLDING_MARGIN = 0;
} // anonymous namespace

ThreadSearchLoggerSTC::ThreadSearchLoggerSTC(ThreadSearchView& threadSearchView,
                                             ThreadSearch& threadSearchPlugin,
                                             InsertIndexManager::eFileSorting fileSorting,
                                             wxWindow* parent, long id) :
    ThreadSearchLoggerBase(parent, threadSearchView, threadSearchPlugin, fileSorting)
{
    ColourManager *colours = Manager::Get()->GetColourManager();

    m_stc = new wxScintilla(this, id, wxDefaultPosition, wxDefaultSize);
    m_stc->SetCaretLineVisible(true);
    m_stc->SetCaretLineBackground(colours->GetColour(wxT("thread_search_selected_line_back")));
    m_stc->SetCaretWidth(0);
    m_stc->SetReadOnly(true);

    // Setup folding
    {
        m_stc->SetMarginType(C_FOLDING_MARGIN, wxSCI_MARGIN_SYMBOL);
        m_stc->SetMarginWidth(C_FOLDING_MARGIN, 16);
        m_stc->SetMarginMask(C_FOLDING_MARGIN, wxSCI_MASK_FOLDERS);
        m_stc->SetMarginSensitive(C_FOLDING_MARGIN, 1);

        ConfigManager* config = Manager::Get()->GetConfigManager(_T("editor"));
        const int id = config->ReadInt(_T("/folding/indicator"), 2);
        cb::UnderlineFoldedLines(m_stc, config->ReadBool(_T("/folding/underline_folded_line"), true));
        cb::SetFoldingMarkers(m_stc, id);
    }

    // Setup styles
    {
        m_stc->StyleSetForeground(wxSCI_STYLE_DEFAULT,
                                  colours->GetColour(wxT("thread_search_text_fore")));
        m_stc->StyleSetBackground(wxSCI_STYLE_DEFAULT,
                                  colours->GetColour(wxT("thread_search_text_back")));

        // Spread the default colours to all styles.
        m_stc->StyleClearAll();

        m_stc->StyleSetForeground(STCStyles::File,
                                  colours->GetColour(wxT("thread_search_file_fore")));
        m_stc->StyleSetBackground(STCStyles::File,
                                  colours->GetColour(wxT("thread_search_file_back")));

        m_stc->StyleSetForeground(STCStyles::LineNo,
                                  colours->GetColour(wxT("thread_search_lineno_fore")));
        m_stc->StyleSetBackground(STCStyles::LineNo,
                                  colours->GetColour(wxT("thread_search_lineno_back")));

        m_stc->StyleSetForeground(STCStyles::Text,
                                  colours->GetColour(wxT("thread_search_text_fore")));
        m_stc->StyleSetBackground(STCStyles::Text,
                                  colours->GetColour(wxT("thread_search_text_back")));

        m_stc->StyleSetForeground(STCStyles::TextMatching,
                                  colours->GetColour(wxT("thread_search_match_fore")));
        m_stc->StyleSetBackground(STCStyles::TextMatching,
                                  colours->GetColour(wxT("thread_search_match_back")));
    }

    SetupSizer(m_stc);

    ConnectEvents();
}

ThreadSearchLoggerSTC::~ThreadSearchLoggerSTC()
{
    DisconnectEvents();
}

void ThreadSearchLoggerSTC::RegisterColours()
{
    ColourManager *colours = Manager::Get()->GetColourManager();

    const wxColour defaultBg = wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX);
    const wxColour defaultFg = wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOXTEXT);

    colours->RegisterColour(wxT("Thread Search"), wxT("File foreground"),
                            wxT("thread_search_file_fore"), wxColour(255, 0, 255));
    colours->RegisterColour(wxT("Thread Search"), wxT("File background"),
                            wxT("thread_search_file_back"), defaultBg);

    colours->RegisterColour(wxT("Thread Search"), wxT("Line foreground"),
                            wxT("thread_search_lineno_fore"), wxColour(0, 0, 255));
    colours->RegisterColour(wxT("Thread Search"), wxT("Line background"),
                            wxT("thread_search_lineno_back"), defaultBg);

    colours->RegisterColour(wxT("Thread Search"), wxT("Text foreground"),
                            wxT("thread_search_text_fore"), defaultFg);
    colours->RegisterColour(wxT("Thread Search"), wxT("Text background"),
                            wxT("thread_search_text_back"), defaultBg);

    colours->RegisterColour(wxT("Thread Search"), wxT("Matching text foreground"),
                            wxT("thread_search_match_fore"), wxColor(255, 0, 0));
    colours->RegisterColour(wxT("Thread Search"), wxT("Matching text background"),
                            wxT("thread_search_match_back"), defaultBg);

    colours->RegisterColour(wxT("Thread Search"), wxT("Selected line background"),
                            wxT("thread_search_selected_line_back"),
                            wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
}

ThreadSearchLoggerBase::eLoggerTypes ThreadSearchLoggerSTC::GetLoggerType()
{
    return TypeSTC;
}

void ThreadSearchLoggerSTC::OnThreadSearchEvent(const ThreadSearchEvent& event)
{
    const wxArrayString& words = event.GetLineTextArray();
    const wxString &filename = event.GetString();
    const std::vector<int> &matchedPositions = event.GetMatchedPositions();

    ++m_fileCount;
    m_totalCount += words.size() / 2;

    m_stc->Freeze();
    m_stc->SetReadOnly(false);

    AppendStyledText(STCStyles::File, wxString::Format(_("%s (%d matches)\n"), filename.wx_str(),
                                                       words.size() / 2));

    // The only reason it is constructed here is to preserve the allocated space between loop
    // iterations.
    wxString justifier;

    std::vector<int>::const_iterator matchedIt = matchedPositions.begin();

    for (size_t ii = 0; ii + 1 < words.GetCount(); ii += 2)
    {
        justifier.clear();

        const wxString &lineNoStr = words[ii];
        if (lineNoStr.length() < 10)
        {
            justifier.Append(wxT(' '), 10 - lineNoStr.length());
        }

        AppendStyledText(STCStyles::LineNo, justifier + lineNoStr + wxT(':'));

        const int textStart = m_stc->GetLength() + 1;

        AppendStyledText(STCStyles::Text, wxT('\t') + words[ii + 1] + wxT('\n'));

        {
            const int matchedCount = *matchedIt;
            ++matchedIt;

            for (int ii = 0; ii < matchedCount; ++ii)
            {
                const int start = *matchedIt;
                ++matchedIt;
                const int length = *matchedIt;
                ++matchedIt;

                m_stc->StartStyling(textStart + start);
                m_stc->SetStyling(length, STCStyles::TextMatching);
            }
        }
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
    m_fileCount = 0;
    m_totalCount = 0;

    m_startLine = m_stc->LineFromPosition(m_stc->GetLength());

    // Create the string depicting selected options.
    wxString optionMessage;
    if (findData.GetMatchCase())
        optionMessage += _("match case");
    if (findData.GetMatchWord())
    {
        if (!optionMessage.empty())
            optionMessage += wxT(", ");
        optionMessage += _("match word");
    }
    if (findData.GetStartWord())
    {
        if (!optionMessage.empty())
            optionMessage += wxT(", ");
        optionMessage += _("start word");
    }
    if (findData.GetRegEx())
    {
        if (!optionMessage.empty())
            optionMessage += wxT(", ");
        optionMessage += _("regular expression");
    }

    // Add initial banner for the current search.
    wxString message;
    if (optionMessage.empty())
        message = wxString::Format(_("=> Searching for '%s'\n"), findData.GetFindText().wx_str());
    else
    {
        message = wxString::Format(_("=> Searching for '%s' (%s)\n"),
                                   findData.GetFindText().wx_str(),
                                   optionMessage.wx_str());
    }

    m_stc->SetReadOnly(false);
    m_stc->AppendText(message);
    m_stc->SetReadOnly(true);

    m_stc->SetFoldLevel(m_startLine, wxSCI_FOLDLEVELBASE | wxSCI_FOLDLEVELHEADERFLAG);
    m_stc->SetFirstVisibleLine(m_startLine);
}

void ThreadSearchLoggerSTC::OnSearchEnd()
{
    m_stc->SetReadOnly(false);

    const int line = std::max(0, m_stc->LineFromPosition(m_stc->GetLength()));

    wxString message = wxString::Format(_("=> Finished! Found: %d in %d files\n\n"), m_totalCount,
                                        m_fileCount);
    m_stc->AppendText(message);
    m_stc->SetReadOnly(true);

    m_stc->SetFoldLevel(line + 0, wxSCI_FOLDLEVELBASE + 1);
    m_stc->SetFoldLevel(line + 1, wxSCI_FOLDLEVELBASE + 1);

    m_stc->SetFirstVisibleLine(m_startLine);
}

wxWindow* ThreadSearchLoggerSTC::GetWindow()
{
    return m_stc;
}

void ThreadSearchLoggerSTC::SetFocus()
{
    m_stc->SetFocus();
}

void ThreadSearchLoggerSTC::AppendStyledText(int style, const wxString &text)
{
    const int position = m_stc->GetLength();
    m_stc->StartStyling(position);
    m_stc->AppendText(text);
    m_stc->SetStyling(text.length(), style);

    int foldLevel = -1;
    if (style == STCStyles::File)
        foldLevel = (wxSCI_FOLDLEVELBASE + 1) | wxSCI_FOLDLEVELHEADERFLAG;
    else if (style == STCStyles::LineNo)
        foldLevel = (wxSCI_FOLDLEVELBASE + 2);

    if (foldLevel != -1)
    {
        const int line = m_stc->LineFromPosition(position);
        m_stc->SetFoldLevel(line, foldLevel);
    }
}

void ThreadSearchLoggerSTC::ConnectEvents()
{
    const wxWindowID stcId = m_stc->GetId();

    Connect(stcId, wxEVT_SCI_MARGINCLICK,
            wxScintillaEventHandler(ThreadSearchLoggerSTC::OnMarginClick));
}

void ThreadSearchLoggerSTC::DisconnectEvents()
{
    const wxWindowID stcId = m_stc->GetId();

    Disconnect(stcId, wxEVT_SCI_MARGINCLICK,
               wxScintillaEventHandler(ThreadSearchLoggerSTC::OnMarginClick));
}

void ThreadSearchLoggerSTC::OnMarginClick(wxScintillaEvent &event)
{
    switch (event.GetMargin())
    {
        case C_FOLDING_MARGIN: // folding margin
        {
            const int lineYpix = event.GetPosition();
            const int line = m_stc->LineFromPosition(lineYpix);
            m_stc->ToggleFold(line);
            break;
        }
    }
}
