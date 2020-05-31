/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#include <sdk.h> // Code::Blocks SDK
#ifndef CB_PRECOMP
    #include <algorithm>

    #include <wx/menu.h>
    #include <wx/settings.h>
    #include <wx/wxscintilla.h>

    #include <cbeditor.h>
    #include <configmanager.h>
#endif

#include "cbcolourmanager.h"
#include "editor_utils.h"

#include "ThreadSearch.h"
#include "ThreadSearchControlIds.h"
#include "ThreadSearchEvent.h"
#include "ThreadSearchFindData.h"
#include "ThreadSearchLoggerSTC.h"
#include "ThreadSearchView.h"

namespace
{

enum STCStyles : int
{
    File = 2,
    LineNo,
    Text,
    TextMatching
};

enum STCFoldLevels : int
{
    Search = wxSCI_FOLDLEVELBASE + 1,
    Messages,
    Files,
    ResultLines
};

const int C_FOLDING_MARGIN = 0;
const int MARKER_UNFOCUSED_LINE = 5;
} // anonymous namespace

/// We use this class only to handle enter presses.
class STCList : public wxScintilla
{
public:
    STCList(wxWindow *parent, wxWindowID id) : wxScintilla(parent, id)
    {
    }

private:
    void OnKeyDown(wxKeyEvent &event)
    {
        static_cast<ThreadSearchLoggerSTC*>(GetParent())->OnKeyDown(event);
    }

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(STCList, wxScintilla)
    EVT_KEY_DOWN(STCList::OnKeyDown)
END_EVENT_TABLE()

ThreadSearchLoggerSTC::ThreadSearchLoggerSTC(ThreadSearchView& threadSearchView,
                                             ThreadSearch& threadSearchPlugin,
                                             InsertIndexManager::eFileSorting fileSorting,
                                             wxWindow* parent, long id) :
    ThreadSearchLoggerBase(parent, threadSearchView, threadSearchPlugin, fileSorting)
{
    m_stc = new STCList(this, id);
    m_stc->SetCaretLineVisible(true);
    m_stc->SetCaretWidth(0);
    m_stc->SetReadOnly(true);
    m_stc->UsePopUp(false);
    // Setup horizontal scrolling - scintilla doesn't do horizontal scroll adjustments based on
    // document content. It could only enlarge the width. So we have to fake it by setting the
    // initial width to be low and reset it if the document is cleared.
    m_stc->SetScrollWidth(100);
    m_stc->SetScrollWidthTracking(true);

    // Setup folding
    {
        m_stc->SetMarginCount(1);
        m_stc->SetMarginType(C_FOLDING_MARGIN, wxSCI_MARGIN_SYMBOL);
        m_stc->SetMarginWidth(C_FOLDING_MARGIN, 16);
        m_stc->SetMarginMask(C_FOLDING_MARGIN, wxSCI_MASK_FOLDERS);
        m_stc->SetMarginSensitive(C_FOLDING_MARGIN, 1);

        ConfigManager* config = Manager::Get()->GetConfigManager(_T("editor"));
        const int id = config->ReadInt(_T("/folding/indicator"), 2);
        cb::UnderlineFoldedLines(m_stc, config->ReadBool(_T("/folding/underline_folded_line"), true));
        cb::SetFoldingMarkers(m_stc, id);
    }

    // Define a marker which will be used when the stc control looses focus to preserve the selected
    // line. When the focus is regained the marker would be removed.
    m_stc->MarkerDefine(MARKER_UNFOCUSED_LINE, wxSCI_MARK_BACKGROUND);

    // Disable modification notifications. We don't need those and they are pretty expensive when
    // handling EditorLinesAddedOrRemoved.
    m_stc->SetModEventMask(0);
    // Also disable undo, there is no user editing allowed, so no point in undo.
    m_stc->SetUndoCollection(false);

    SetupStyles();

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

    wxColour fileFg, lineFg, matchingFg, matchingBg;
    if (cbIsDarkTheme())
    {
        fileFg = wxColour(103, 140, 177);
        lineFg = wxColour(147, 199, 99);
        matchingFg = wxColour(241, 242, 243);
        matchingBg = wxColour(131, 38, 23);
    }
    else
    {
        fileFg = wxColour(0, 0, 160);
        lineFg = wxColour(240, 0, 240);
        matchingFg = wxColour(255, 255, 255);
        matchingBg = wxColour(0, 128, 128);
    }

    colours->RegisterColour(wxT("Thread Search"), wxT("File foreground"),
                            wxT("thread_search_file_fore"), fileFg);
    colours->RegisterColour(wxT("Thread Search"), wxT("File background"),
                            wxT("thread_search_file_back"), defaultBg);

    colours->RegisterColour(wxT("Thread Search"), wxT("Line foreground"),
                            wxT("thread_search_lineno_fore"), lineFg);
    colours->RegisterColour(wxT("Thread Search"), wxT("Line background"),
                            wxT("thread_search_lineno_back"), defaultBg);

    colours->RegisterColour(wxT("Thread Search"), wxT("Text foreground"),
                            wxT("thread_search_text_fore"), defaultFg);
    colours->RegisterColour(wxT("Thread Search"), wxT("Text background"),
                            wxT("thread_search_text_back"), defaultBg);

    colours->RegisterColour(wxT("Thread Search"), wxT("Matching text foreground"),
                            wxT("thread_search_match_fore"), matchingFg);
    colours->RegisterColour(wxT("Thread Search"), wxT("Matching text background"),
                            wxT("thread_search_match_back"), matchingBg);

    colours->RegisterColour(wxT("Thread Search"), wxT("Selected line background"),
                            wxT("thread_search_selected_line_back"),
                            wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
}

void ThreadSearchLoggerSTC::SetupStyles()
{
    ColourManager *colours = Manager::Get()->GetColourManager();

    m_stc->SetLexer(wxSCI_LEX_CONTAINER);

    m_stc->SetCaretLineBackground(colours->GetColour(wxT("thread_search_selected_line_back")));
    m_stc->MarkerSetBackground(MARKER_UNFOCUSED_LINE,
                               colours->GetColour("thread_search_selected_line_back"));

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
    m_stc->StyleSetBold(STCStyles::TextMatching, true);

    m_stc->SetFoldMarginColour(true, colours->GetColour(wxT("editor_margin_chrome")));
    m_stc->SetFoldMarginHiColour(true, colours->GetColour(wxT("editor_margin_chrome_highlight")));
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

    {
        int position = m_stc->GetLength();
        // Append, style and fold the filename
        m_stc->AppendText(filename);
        int endPosition = m_stc->GetLength();
        AppendStyleItem(position, endPosition, STCStyles::File);
        const int line = m_stc->LineFromPosition(position);
        m_stc->SetFoldLevel(line, STCFoldLevels::Files | wxSCI_FOLDLEVELHEADERFLAG);

        // Append and style the number of matches in the file
        position = endPosition;
        m_stc->AppendText(wxString::Format(_(" (%lld matches)\n"),
                                           static_cast<long long>(words.size() / 2)));
        endPosition = m_stc->GetLength();
        AppendStyleItem(position, endPosition, STCStyles::Text);
    }

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

        const int startPosition = m_stc->GetLength();
        const int line = m_stc->LineFromPosition(startPosition);

        m_stc->AppendText(justifier + lineNoStr + wxT(':'));

        const int textStartPos = m_stc->GetLength();
        AppendStyleItem(startPosition, textStartPos, STCStyles::LineNo);
        m_stc->SetFoldLevel(line, STCFoldLevels::ResultLines);

        m_stc->AppendText(wxT("    ") + words[ii + 1] + wxT('\n'));

        int lastStyledPosition = textStartPos;

        {
            const int matchedCount = *matchedIt;
            ++matchedIt;

            const int textStartColumn = m_stc->GetColumn(textStartPos + 4);

            for (int ii = 0; ii < matchedCount; ++ii)
            {
                const int start = *matchedIt;
                ++matchedIt;
                const int length = *matchedIt;
                ++matchedIt;

                // Translate character positions to byte positions.
                // wxString works with characters, not bytes.
                const int startPos = m_stc->FindColumn(line, textStartColumn + start);
                const int endPos = m_stc->FindColumn(line, textStartColumn + start + length);

                if (lastStyledPosition < startPos)
                    AppendStyleItem(lastStyledPosition, startPos, STCStyles::Text);

                AppendStyleItem(startPos, endPos, STCStyles::TextMatching);
                lastStyledPosition = endPos;
            }

            // Style the text after the last match.
            const int lineEndPos = m_stc->GetLength();
            if (lastStyledPosition < lineEndPos)
                AppendStyleItem(lastStyledPosition, lineEndPos, STCStyles::Text);
        }
    }

    m_stc->SetReadOnly(true);
    AutoScroll();
    m_stc->Thaw();
}

void ThreadSearchLoggerSTC::Clear()
{
    m_stc->SetReadOnly(false);
    m_stc->ClearAll();
    m_stc->SetScrollWidth(100);
    m_lastLineMarkerHandle = -1;
    m_fileCount = 0;
    m_totalCount = 0;
    m_styles.clear();
    m_stc->SetReadOnly(true);
}

void ThreadSearchLoggerSTC::OnSearchBegin(const ThreadSearchFindData& findData)
{
    m_timeBegin = wxGetUTCTimeMillis();

    // We have to reset the readonly flag, because Scintilla doesn't allow modifications while the
    // flag is set! Make sure to restore it back upon exit!
    m_stc->SetReadOnly(false);

    m_fileCount = 0;
    m_totalCount = 0;

    if (m_ThreadSearchPlugin.GetDeletePreviousResults())
    {
        m_stc->ClearAll();
        m_stc->SetScrollWidth(100);
        m_styles.clear();
    }

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

    m_stc->AppendText(message);

    m_stc->SetFoldLevel(m_startLine, STCFoldLevels::Search | wxSCI_FOLDLEVELHEADERFLAG);
    m_stc->SetFirstVisibleLine(m_startLine);
    m_lastVisibleLine = m_stc->GetFirstVisibleLine();

    m_stc->SetReadOnly(true);
}

void ThreadSearchLoggerSTC::OnSearchEnd()
{
    const double durationMilliSec = (wxGetUTCTimeMillis() - m_timeBegin).ToDouble();

    m_stc->SetReadOnly(false);

    const int line = std::max(0, m_stc->LineFromPosition(m_stc->GetLength()));

    wxString message = wxString::Format(_("=> Finished! Found %d matches in %d files (took %.3f sec)\n\n"),
                                        m_totalCount, m_fileCount, durationMilliSec * 0.001);
    m_stc->AppendText(message);
    m_stc->SetReadOnly(true);

    m_stc->SetFoldLevel(line + 0, STCFoldLevels::Messages);
    m_stc->SetFoldLevel(line + 1, STCFoldLevels::Messages);

    AutoScroll();
}

void ThreadSearchLoggerSTC::AutoScroll()
{
    // Detect if the user has scrolled the view.
    const int firstVisibleLine = m_stc->GetFirstVisibleLine();
    if (firstVisibleLine == m_lastVisibleLine && firstVisibleLine != m_startLine)
    {
        m_stc->SetFirstVisibleLine(m_startLine);
        m_lastVisibleLine = m_stc->GetFirstVisibleLine();
    }
}

wxWindow* ThreadSearchLoggerSTC::GetWindow()
{
    return m_stc;
}

void ThreadSearchLoggerSTC::SetFocus()
{
    m_stc->SetFocus();
}

void ThreadSearchLoggerSTC::UpdateSettings()
{
    SetupStyles();
}

void ThreadSearchLoggerSTC::AppendStyleItem(int startPos, int endPos, int style)
{
    StyleItem item;
    item.startPos = startPos;
    item.length = endPos - startPos;
    item.style = style;
    m_styles.push_back(item);
}

void ThreadSearchLoggerSTC::ConnectEvents()
{
    const wxWindowID stcId = m_stc->GetId();

    // Handle folding
    Connect(stcId, wxEVT_SCI_MARGINCLICK,
            wxScintillaEventHandler(ThreadSearchLoggerSTC::OnMarginClick));
    Connect(stcId, wxEVT_SCI_STYLENEEDED,
            wxScintillaEventHandler(ThreadSearchLoggerSTC::OnStyleNeeded));

    // Handle clicking/selection change events
    Connect(stcId, wxEVT_SCI_UPDATEUI,
            wxScintillaEventHandler(ThreadSearchLoggerSTC::OnSTCUpdateUI));
    Connect(stcId, wxEVT_SCI_DOUBLECLICK,
            wxScintillaEventHandler(ThreadSearchLoggerSTC::OnDoubleClick));

    // Handle Focus
    m_stc->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler(ThreadSearchLoggerSTC::OnSTCFocus),
                   nullptr, this);
    m_stc->Connect(wxEVT_SET_FOCUS, wxFocusEventHandler(ThreadSearchLoggerSTC::OnSTCFocus),
                   nullptr, this);

    // Context menu
    Connect(stcId, wxEVT_CONTEXT_MENU,
            wxContextMenuEventHandler(ThreadSearchLoggerSTC::OnContextMenu));

    Connect(controlIDs.Get(ControlIDs::idMenuCtxCopy), wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(ThreadSearchLoggerSTC::OnMenuCopy));
    Connect(controlIDs.Get(ControlIDs::idMenuCtxCopySelection), wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(ThreadSearchLoggerSTC::OnMenuCopySelection));
    Connect(controlIDs.Get(ControlIDs::idMenuCtxCollapseSearch), wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(ThreadSearchLoggerSTC::OnMenuCollapseSearch));
    Connect(controlIDs.Get(ControlIDs::idMenuCtxCollapseFile), wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(ThreadSearchLoggerSTC::OnMenuCollapseFile));
    Connect(controlIDs.Get(ControlIDs::idMenuCtxCollapseAll), wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(ThreadSearchLoggerSTC::OnMenuCollapseAll));
    Connect(controlIDs.Get(ControlIDs::idMenuCtxDeleteItem), wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(ThreadSearchLoggerSTC::OnMenuDelete));
    Connect(controlIDs.Get(ControlIDs::idMenuCtxDeleteAllItems), wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(ThreadSearchLoggerSTC::OnMenuDeleteAll));
}

void ThreadSearchLoggerSTC::DisconnectEvents()
{
    const wxWindowID stcId = m_stc->GetId();

    // Handle folding
    Disconnect(stcId, wxEVT_SCI_MARGINCLICK,
               wxScintillaEventHandler(ThreadSearchLoggerSTC::OnMarginClick));
    Disconnect(stcId, wxEVT_SCI_STYLENEEDED,
               wxScintillaEventHandler(ThreadSearchLoggerSTC::OnStyleNeeded));

    // Handle clicking/selection change events
    Disconnect(stcId, wxEVT_SCI_UPDATEUI,
            wxScintillaEventHandler(ThreadSearchLoggerSTC::OnSTCUpdateUI));
    Disconnect(stcId, wxEVT_SCI_DOUBLECLICK,
               wxScintillaEventHandler(ThreadSearchLoggerSTC::OnDoubleClick));

    // Handle Focus
    m_stc->Disconnect(wxEVT_KILL_FOCUS, wxFocusEventHandler(ThreadSearchLoggerSTC::OnSTCFocus),
                      nullptr, this);
    m_stc->Disconnect(wxEVT_SET_FOCUS, wxFocusEventHandler(ThreadSearchLoggerSTC::OnSTCFocus),
                      nullptr, this);

    // Context menu
    Disconnect(stcId, wxEVT_CONTEXT_MENU,
               wxContextMenuEventHandler(ThreadSearchLoggerSTC::OnContextMenu));

    Disconnect(controlIDs.Get(ControlIDs::idMenuCtxCopy), wxEVT_COMMAND_MENU_SELECTED,
               wxCommandEventHandler(ThreadSearchLoggerSTC::OnMenuCopy));
    Disconnect(controlIDs.Get(ControlIDs::idMenuCtxCopySelection), wxEVT_COMMAND_MENU_SELECTED,
               wxCommandEventHandler(ThreadSearchLoggerSTC::OnMenuCopySelection));
    Disconnect(controlIDs.Get(ControlIDs::idMenuCtxCollapseSearch), wxEVT_COMMAND_MENU_SELECTED,
               wxCommandEventHandler(ThreadSearchLoggerSTC::OnMenuCollapseSearch));
    Disconnect(controlIDs.Get(ControlIDs::idMenuCtxCollapseFile), wxEVT_COMMAND_MENU_SELECTED,
               wxCommandEventHandler(ThreadSearchLoggerSTC::OnMenuCollapseFile));
    Disconnect(controlIDs.Get(ControlIDs::idMenuCtxCollapseAll), wxEVT_COMMAND_MENU_SELECTED,
               wxCommandEventHandler(ThreadSearchLoggerSTC::OnMenuCollapseAll));
    Disconnect(controlIDs.Get(ControlIDs::idMenuCtxDeleteItem), wxEVT_COMMAND_MENU_SELECTED,
               wxCommandEventHandler(ThreadSearchLoggerSTC::OnMenuDelete));
    Disconnect(controlIDs.Get(ControlIDs::idMenuCtxDeleteAllItems), wxEVT_COMMAND_MENU_SELECTED,
               wxCommandEventHandler(ThreadSearchLoggerSTC::OnMenuDeleteAll));
}

static bool FindFileLineFromLine(int *outLine, wxScintilla *stc, int inLine)
{
    // We use the fold level to determine what is the kind of line. If the line is for a result
    // line, we know that its fold parent is the line with the file name.
    const int foldLevel = stc->GetFoldLevel(inLine) & wxSCI_FOLDLEVELNUMBERMASK;
    if (foldLevel == STCFoldLevels::Files)
    {
        *outLine = inLine;
        return true;
    }

    if (foldLevel != STCFoldLevels::ResultLines)
        return false;

    const int parentFoldLine = stc->GetFoldParent(inLine);
    if (parentFoldLine == -1)
        return false; // This is probably an error and cannot happen!

    const int parentFoldLevel = stc->GetFoldLevel(parentFoldLine) & wxSCI_FOLDLEVELNUMBERMASK;
    if (parentFoldLevel != STCFoldLevels::Files)
        return false;

    *outLine = parentFoldLine;
    return true;
}

static bool FindSearchLineFromLine(int *outLine, wxScintilla *stc, int inLine)
{
    int line = inLine;

    for(;;)
    {
        const int foldLevel = stc->GetFoldLevel(line) & wxSCI_FOLDLEVELNUMBERMASK;
        if (foldLevel == STCFoldLevels::Search)
        {
            *outLine = line;
            return true;
        }

        const int parentFoldLine = stc->GetFoldParent(line);
        if (parentFoldLine == -1)
            return false;
        line = parentFoldLine;
    }
}

static bool FindResultInfoForLine(wxString *outFilepath, int *outLineInFile, wxScintilla *stc, int stcLine)
{
    int parentFoldLine;
    if (!FindFileLineFromLine(&parentFoldLine, stc, stcLine))
        return false;

    // We know both the lines for the result and the file path, so we can extract our information.
    {
        // Extract the line number
        const wxString &lineData = stc->GetLine(stcLine);
        wxString::size_type colonPos = lineData.find(wxT(':'));
        if (colonPos == wxString::npos)
            return false;

        long value;
        wxString lineStr = lineData.substr(0, colonPos);
        lineStr.Trim();
        if (!lineStr.ToLong(&value))
            return false;
        *outLineInFile = value;
    }

    {
        // Extract the file path
        const wxString &filepathData = stc->GetLine(parentFoldLine);
        wxString::size_type lastOpenBracketPos = filepathData.rfind(wxT('('));
        if (lastOpenBracketPos == wxString::npos)
            return false;

        *outFilepath = filepathData.substr(0, lastOpenBracketPos - 1);
    }

    return true;
}

void ThreadSearchLoggerSTC::OnStyleNeeded(wxScintillaEvent &event)
{
    int startPosition = m_stc->GetEndStyled();
    const int startLine = m_stc->LineFromPosition(startPosition);
    startPosition = m_stc->PositionFromLine(startLine);

    StyleItemVector::const_iterator first = std::lower_bound(m_styles.cbegin(), m_styles.cend(),
                                                             StyleItem::make(startPosition));
    const int lastPosition = event.GetPosition();
    if (first != m_styles.end())
    {
        m_stc->StartStyling(startPosition);
        int pos = startPosition;
        while (first != m_styles.end())
        {
            if (first->startPos > pos)
                m_stc->SetStyling(first->startPos - pos, wxSCI_STYLE_DEFAULT);
            m_stc->SetStyling(first->length, first->style);

            pos = first->startPos + first->length;
            if (pos >= lastPosition)
                break;
            ++first;
        }

        if (pos < lastPosition)
            m_stc->SetStyling(lastPosition - pos, wxSCI_STYLE_DEFAULT);
    }
    else
    {
        m_stc->StartStyling(startPosition);
        m_stc->SetStyling(lastPosition - startPosition, wxSCI_STYLE_DEFAULT);
    }

    event.Skip();
}

void ThreadSearchLoggerSTC::OnSTCUpdateUI(wxScintillaEvent &event)
{
    event.Skip();

    if ((event.GetUpdated() & wxSCI_UPDATE_SELECTION) == 0)
        return;

    const int stcLine = m_stc->GetCurrentLine();

    wxString filepath;
    int line;

    if (FindResultInfoForLine(&filepath, &line, m_stc, stcLine))
    {
        m_ThreadSearchView.OnLoggerClick(filepath, line);
    }
}

void ThreadSearchLoggerSTC::OnDoubleClick(wxScintillaEvent &event)
{
    wxString filepath;
    int line;

    if (FindResultInfoForLine(&filepath, &line, m_stc, event.GetLine()))
    {
        m_ThreadSearchView.OnLoggerDoubleClick(filepath, line);
    }

    m_stc->SetEmptySelection(m_stc->GetCurrentPos());

    event.Skip();
}

void ThreadSearchLoggerSTC::OnKeyDown(wxKeyEvent& event)
{
    const int keyCode = event.GetKeyCode();
    if (keyCode == WXK_RETURN || keyCode == WXK_NUMPAD_ENTER)
    {
        wxString filepath;
        int line;

        if (FindResultInfoForLine(&filepath, &line, m_stc, m_stc->GetCurrentLine()))
            m_ThreadSearchView.OnLoggerDoubleClick(filepath, line);
    }
    event.Skip();
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

void ThreadSearchLoggerSTC::OnContextMenu(wxContextMenuEvent &event)
{
    const int stcLine = m_stc->GetCurrentLine();
    int temp;
    const bool isFileLine = FindFileLineFromLine(&temp, m_stc, stcLine);
    const bool isSearchLine = FindSearchLineFromLine(&temp, m_stc, stcLine);
    const bool isEmpty = m_stc->GetLength() == 0;

    wxMenu menu;
    menu.Append(controlIDs.Get(ControlIDs::idMenuCtxCopy), _("Copy contents to clipboard"));
    menu.Enable(controlIDs.Get(ControlIDs::idMenuCtxCopy), !isEmpty);

    menu.Append(controlIDs.Get(ControlIDs::idMenuCtxCopySelection), _("Copy selection to clipboard"));
    menu.Enable(controlIDs.Get(ControlIDs::idMenuCtxCopySelection), !m_stc->GetSelectionEmpty());

    menu.AppendSeparator();

    menu.Append(controlIDs.Get(ControlIDs::idMenuCtxCollapseFile), _("Collapse file"));
    menu.Enable(controlIDs.Get(ControlIDs::idMenuCtxCollapseFile), isFileLine);

    menu.Append(controlIDs.Get(ControlIDs::idMenuCtxCollapseSearch), _("Collapse search"));
    menu.Enable(controlIDs.Get(ControlIDs::idMenuCtxCollapseSearch), isSearchLine);

    menu.Append(controlIDs.Get(ControlIDs::idMenuCtxCollapseAll), _("Collapse all"));
    menu.Enable(controlIDs.Get(ControlIDs::idMenuCtxCollapseAll), !m_stc->GetSelectionEmpty());

    menu.AppendSeparator();

    menu.Append(controlIDs.Get(ControlIDs::idMenuCtxDeleteItem), _("Delete search"));
    menu.Enable(controlIDs.Get(ControlIDs::idMenuCtxDeleteItem), isSearchLine);

    menu.Append(controlIDs.Get(ControlIDs::idMenuCtxDeleteAllItems), _("Delete all"));
    menu.Enable(controlIDs.Get(ControlIDs::idMenuCtxDeleteAllItems), !isEmpty);

    // display menu
    wxPoint clientpos;
    wxPoint position = event.GetPosition();
    if (position==wxDefaultPosition) // "context menu" key
    {
        // obtain the caret point (on the screen) as we assume
        // that the user wants to work with the keyboard
        clientpos = m_stc->PointFromPosition(m_stc->GetCurrentPos());
    }
    else
    {
        clientpos = m_stc->ScreenToClient(position);
    }

    PopupMenu(&menu, clientpos);
}

void ThreadSearchLoggerSTC::OnMenuCopy(cb_unused wxCommandEvent &event)
{
    m_stc->CopyRange(0, m_stc->GetLength());
}

void ThreadSearchLoggerSTC::OnMenuCopySelection(cb_unused wxCommandEvent &event)
{
    m_stc->CopyAllowLine();
}

void ThreadSearchLoggerSTC::OnMenuCollapseFile(cb_unused wxCommandEvent &event)
{
    const int stcLine = m_stc->GetCurrentLine();
    int fileLine;
    if (FindFileLineFromLine(&fileLine, m_stc, stcLine))
    {
        m_stc->FoldLine(fileLine, wxSCI_FOLDACTION_CONTRACT);
    }
}

void ThreadSearchLoggerSTC::OnMenuCollapseSearch(cb_unused wxCommandEvent &event)
{
    const int stcLine = m_stc->GetCurrentLine();
    int searchLine;
    if (FindSearchLineFromLine(&searchLine, m_stc, stcLine))
    {
        // Collapse the found search line.
        m_stc->FoldLine(searchLine, wxSCI_FOLDACTION_CONTRACT);

        // Collapse all lines to the next search line.
        const int count = m_stc->GetLineCount();
        for (int line = searchLine + 1; line < count; ++line)
        {
            const int level = m_stc->GetFoldLevel(line);
            if ((level & wxSCI_FOLDLEVELNUMBERMASK) == STCFoldLevels::Search)
                break;

            if ((level & wxSCI_FOLDLEVELHEADERFLAG) != 0)
            {
                m_stc->FoldLine(line, wxSCI_FOLDACTION_CONTRACT);
            }
        }

        m_stc->SetFirstVisibleLine(searchLine);
    }
}

void ThreadSearchLoggerSTC::OnMenuCollapseAll(cb_unused wxCommandEvent &event)
{
    const int count = m_stc->GetLineCount();
    for (int line = 0; line < count; ++line)
    {
        const int level = m_stc->GetFoldLevel(line);
        if ((level & wxSCI_FOLDLEVELHEADERFLAG) != 0)
        {
            m_stc->FoldLine(line, wxSCI_FOLDACTION_CONTRACT);
        }
    }
}

void ThreadSearchLoggerSTC::OnMenuDelete(cb_unused wxCommandEvent &event)
{
    const int stcLine = m_stc->GetCurrentLine();
    int startLine;
    if (!FindSearchLineFromLine(&startLine, m_stc, stcLine))
        return;

    int endLine = -1;
    // Try to find the start of the next search. This will be our end position.
    const int count = m_stc->GetLineCount();
    for (int line = startLine + 1; line < count; ++line)
    {
        const int level = m_stc->GetFoldLevel(line);
        if ((level & wxSCI_FOLDLEVELNUMBERMASK) == STCFoldLevels::Search)
        {
            endLine = line;
            break;
        }
    }

    const int startPosition = m_stc->PositionFromLine(startLine);
    int endPosition;
    if (endLine == -1)
        endPosition = m_stc->GetLength();
    else
        endPosition = m_stc->PositionFromLine(endLine);

    // We have the range for the search, so delete the whole text for it.
    m_stc->SetReadOnly(false);
    m_stc->Remove(startPosition, endPosition);
    m_stc->SetScrollWidth(100);
    m_stc->SetReadOnly(true);

    {
        // Now we have to delete the style information.

        // Lower bounds work, because we don't have style items which are specifying styles across
        // more than one line.

        // First is pointing to the first item in the deleted search.
        StyleItemVector::iterator first = std::lower_bound(m_styles.begin(), m_styles.end(),
                                                           StyleItem::make(startPosition));
        // Last is pointing to the first item after the deleted search.
        StyleItemVector::iterator last = std::lower_bound(first, m_styles.end(),
                                                          StyleItem::make(endPosition));

        // We have to adjust the start positions of all style items for the text after the deleted
        // block.
        const int delta = endPosition - startPosition;
        for (StyleItemVector::iterator it = last; it != m_styles.end(); ++it)
        {
            it->startPos -= delta;
        }

        m_styles.erase(first, last);
    }
}

void ThreadSearchLoggerSTC::OnMenuDeleteAll(cb_unused wxCommandEvent &event)
{
    Clear();
}

void ThreadSearchLoggerSTC::OnSTCFocus(wxFocusEvent &event)
{
    const wxEventType type = event.GetEventType();
    if (type == wxEVT_KILL_FOCUS)
    {
        m_lastLineMarkerHandle = m_stc->MarkerAdd(m_stc->GetCurrentLine(), MARKER_UNFOCUSED_LINE);
    }
    else if (type == wxEVT_SET_FOCUS)
    {
        m_stc->MarkerDeleteHandle(m_lastLineMarkerHandle);
        m_lastLineMarkerHandle = -1;
    }

    event.Skip();
}

struct WordRange
{
    int start, end;
};

static WordRange FindWordForLineNumber(wxScintilla *stc, int lineStart)
{
    // Use 9 because we know that the end of the number is at position 10. This gives small boost in
    // performance, because the number of calls to WordEndPosition is minimized.
    int start = lineStart + 9;
    int end;
    while (1)
    {
        end = stc->WordEndPosition(start, true);
        if (end == start)
            ++start;
        else
            break;
    }

    WordRange result;
    result.end = end;
    result.start = stc->WordStartPosition(end, true);
    return result;
}

void ThreadSearchLoggerSTC::EditorLinesAddedOrRemoved(cbEditor *editor, int startLine,
                                                      int linesAdded)
{
    m_stc->SetReadOnly(false);

    // Start from the end of the document and jump from file to file until a match is found.
    // When a matching file is found search the lines one by one and adjust them.

    int lastLine = m_stc->LineFromPosition(m_stc->GetLength() - 1);

    while (lastLine > 0)
    {
        while (lastLine > 0)
        {
            const int foldLevel = m_stc->GetFoldLevel(lastLine) & wxSCI_FOLDLEVELNUMBERMASK;
            if (foldLevel == STCFoldLevels::ResultLines)
                break;
            lastLine--;
        }

        int fileLine;
        if (!FindFileLineFromLine(&fileLine, m_stc, lastLine))
        {
            break;
        }

        wxString textFile = m_stc->GetLine(fileLine);

        const wxString::size_type delimiterPosition = textFile.rfind(" (");
        if (delimiterPosition != wxString::npos)
        {
            textFile.RemoveLast(textFile.length() - delimiterPosition);

            if (textFile == editor->GetFilename())
            {
                // Iterate the lines of the file
                for (int line = fileLine + 1; line <= lastLine; ++line)
                {
                    const int lineStartPosition = m_stc->PositionFromLine(line);

                    const WordRange word = FindWordForLineNumber(m_stc, lineStartPosition);
                    const wxString &lineNumberText = m_stc->GetTextRange(word.start,
                                                                         word.end);

                    long value;
                    if (lineNumberText.ToLong(&value))
                    {
                        if (value >= startLine)
                        {
                            int newValue;
                            newValue = value + linesAdded;
                            m_stc->SetTargetRange(lineStartPosition, word.end);
                            m_stc->ReplaceTarget(wxString::Format("%10d", newValue));
                        }
                    }
                }
            }
        }

        lastLine = fileLine - 1;
    }

    // Never return before this line!
    m_stc->SetReadOnly(false);
}
