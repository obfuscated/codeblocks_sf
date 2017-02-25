/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/app.h>
    #include <wx/filedlg.h>
    #include <wx/filename.h>
    #include <wx/menu.h>
    #include <wx/notebook.h>
    #include <wx/wfstream.h>

    #include "cbeditor.h" // class's header file

    #include "cbauibook.h"
    #include "cbplugin.h"
    #include "cbproject.h"
    #include "configmanager.h"
    #include "debuggermanager.h"
    #include "editorcolourset.h"
    #include "editormanager.h"
    #include "globals.h"
    #include "infowindow.h"
    #include "logmanager.h"
    #include "macrosmanager.h" // ReplaceMacros
    #include "manager.h"
    #include "pluginmanager.h"
    #include "projectbuildtarget.h"
    #include "projectfile.h"
    #include "projectmanager.h"
    #include "sdk_events.h"
#endif
#include "cbstyledtextctrl.h"
#include "cbcolourmanager.h"

#include <wx/fontutil.h>
#include <wx/splitter.h>

#include "cbeditorprintout.h"
#include "cbdebugger_interfaces.h"
#include "editor_hooks.h"
#include "encodingdetector.h"
#include "filefilters.h"
#include "projectfileoptionsdlg.h"

const wxString g_EditorModified = _T("*");

#define ERROR_STYLE      wxSCI_MARK_SMALLRECT
#define BOOKMARK_STYLE   wxSCI_MARK_ARROW
#define BREAKPOINT_STYLE wxSCI_MARK_CIRCLE
#define DEBUG_STYLE      wxSCI_MARK_ARROW
#define DEBUG_STYLE_HIGHLIGHT wxSCI_MARK_BACKGROUND

#define BREAKPOINT_OTHER_MARKER    1
#define BREAKPOINT_DISABLED_MARKER 2
#define BREAKPOINT_MARKER          3
#define BOOKMARK_MARKER            4
#define ERROR_MARKER               5
#define DEBUG_MARKER               6
#define DEBUG_MARKER_HIGHLIGHT     7

#define C_LINE_MARGIN      0 // Line numbers
#define C_MARKER_MARGIN    1 // Bookmarks, Breakpoints...
#define C_CHANGEBAR_MARGIN 2
#define C_FOLDING_MARGIN   3

/* This struct holds private data for the cbEditor class.
 * It's a paradigm to avoid rebuilding the entire project (as cbEditor is a basic dependency)
 * for just adding a private var or method.
 * What happens is that we 've declared a cbEditorInternalData* private member in cbEditor
 * and define it in the .cpp file (here). Effectively, this means that we can now add/remove
 * elements from cbEditorInternalData without needing to rebuild the project :)
 * The cbEditor::m_pData is the variable to use in code. It's the very first thing
 * constructed and the very last destructed.
 *
 * So, if we want to add a new private member in cbEditor, we add it here instead
 * and access it with m_pData->
 * e.g. m_pData->lastPosForCodeCompletion
 * and of course you can add member functions here ;)
 *
 * cbEditorInternalData also contains a pointer to the owner cbEditor (named m_pOwner).
 * Using m_pOwner the struct's member functions can access cbEditor functions
 * (even private ones - it's a friend).
 *
 * The same logic should be used all around the project's classes, gradually.
 */
struct cbEditorInternalData
{
    cbEditor* m_pOwner;

    cbEditorInternalData(cbEditor* owner, LoaderBase* fileLoader = nullptr)
        : m_pOwner(owner),
        m_strip_trailing_spaces(true),
        m_ensure_final_line_end(false),
        m_ensure_consistent_line_ends(true),
        m_LastMarginMenuLine(-1),
        m_LastDebugLine(-1),
        m_useByteOrderMark(false),
        m_byteOrderMarkLength(0),
        m_lineNumbersWidth(0),
        m_lineNumbersWidth2(0),
        m_pFileLoader(fileLoader)
    {
        m_encoding = wxLocale::GetSystemEncoding();

        if (m_pFileLoader)
        {
#ifdef fileload_measuring
            wxStopWatch sw;
#endif
            EncodingDetector enc(fileLoader);
            if (enc.IsOK())
            {
                m_byteOrderMarkLength = enc.GetBOMSizeInBytes();
                m_useByteOrderMark    = enc.UsesBOM();
                m_encoding            = enc.GetFontEncoding();
            }
#ifdef fileload_measuring
            Manager::Get()->GetLogManager()->DebugLog(F(_T("Encoding via fileloader took : %d ms"),(int)sw.Time()));
#endif
        }
    }

    ~cbEditorInternalData()
    {
        if (m_pFileLoader)
        {
            delete m_pFileLoader;
            m_pFileLoader = nullptr;
        }
    }

    // funcs

    /** Strip trailing blanks before saving */
    void StripTrailingSpaces()
    {
        cbStyledTextCtrl* control = m_pOwner->GetControl();
        // The following code was adapted from the SciTE sourcecode

        int maxLines = control->GetLineCount();
        for (int line = 0; line < maxLines; line++)
        {
            int lineStart = control->PositionFromLine(line);
            int lineEnd = control->GetLineEndPosition(line);
            int i = lineEnd-1;
            wxChar ch = (wxChar)(control->GetCharAt(i));
            if (control->GetLexer() == wxSCI_LEX_DIFF)
                lineStart++;
            while ((i >= lineStart) && ((ch == _T(' ')) || (ch == _T('\t'))))
            {
                i--;
                ch = (wxChar)(control->GetCharAt(i));
            }
            if (i < (lineEnd-1))
            {
                control->SetTargetStart(i+1);
                control->SetTargetEnd(lineEnd);
                control->ReplaceTarget(_T(""));
            }
        }
    }

    /** Add extra blank line to the file */
    void EnsureFinalLineEnd()
    {
        cbStyledTextCtrl* control = m_pOwner->GetControl();
        // The following code was adapted from the SciTE sourcecode
        int maxLines = control->GetLineCount();
        int enddoc = control->PositionFromLine(maxLines);
        if (maxLines <= 1 || enddoc > control->PositionFromLine(maxLines-1))
            control->InsertText(enddoc, GetEOLStr(m_pOwner->GetControl()->GetEOLMode()));
    }

    /** Make sure all the lines end with the same EOL mode */
    void EnsureConsistentLineEnds()
    {
        cbStyledTextCtrl* control = m_pOwner->GetControl();
        // The following code was adapted from the SciTE sourcecode
        control->ConvertEOLs(control->GetEOLMode());
    }

    /** Set line number column width */
    void SetLineNumberColWidth(bool both=true)
    {
        ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));

        if (cfg->ReadBool(_T("/show_line_numbers"), true))
        {
            if (m_pOwner->m_pControl2 && both)
            {
                int pixelWidth = m_pOwner->m_pControl->TextWidth(wxSCI_STYLE_LINENUMBER, _T("9"));
                int pixelWidth2 = m_pOwner->m_pControl2->TextWidth(wxSCI_STYLE_LINENUMBER, _T("9"));

                if (cfg->ReadBool(_T("/margin/dynamic_width"), false))
                {
                    int lineNumChars = 1;
                    int lineCount = m_pOwner->m_pControl->GetLineCount();

                    while (lineCount >= 10)
                    {
                        lineCount /= 10;
                        ++lineNumChars;
                    }

                    int lineNumWidth =  lineNumChars * pixelWidth + pixelWidth * 0.75;

                    if (lineNumWidth != m_lineNumbersWidth)
                    {
                        m_pOwner->m_pControl->SetMarginWidth(C_LINE_MARGIN, lineNumWidth);
                        m_lineNumbersWidth = lineNumWidth;
                    }

                    lineNumWidth =  lineNumChars * pixelWidth2 + pixelWidth2 * 0.75;
                    if (lineNumWidth != m_lineNumbersWidth2)
                    {
                        m_pOwner->m_pControl2->SetMarginWidth(C_LINE_MARGIN, lineNumWidth);
                        m_lineNumbersWidth2 = lineNumWidth;
                    }
                }
                else
                {
                    m_pOwner->m_pControl->SetMarginWidth(C_LINE_MARGIN, pixelWidth * 0.75 + cfg->ReadInt(_T("/margin/width_chars"), 6) * pixelWidth);
                    m_pOwner->m_pControl2->SetMarginWidth(C_LINE_MARGIN, pixelWidth * 0.75 + cfg->ReadInt(_T("/margin/width_chars"), 6) * pixelWidth);
                }
            }
            else
            {
                cbStyledTextCtrl* control = m_pOwner->GetControl();
                int* pLineNumbersWidth = nullptr;
                if (control == m_pOwner->m_pControl)
                    pLineNumbersWidth = &m_lineNumbersWidth;
                else
                    pLineNumbersWidth = &m_lineNumbersWidth2;

                int pixelWidth = control->TextWidth(wxSCI_STYLE_LINENUMBER, _T("9"));

                if (cfg->ReadBool(_T("/margin/dynamic_width"), false))
                {
                    int lineNumChars = 1;
                    int lineCount = control->GetLineCount();

                    while (lineCount >= 10)
                    {
                        lineCount /= 10;
                        ++lineNumChars;
                    }

                    int lineNumWidth =  lineNumChars * pixelWidth + pixelWidth * 0.75;

                    if (lineNumWidth != *pLineNumbersWidth)
                    {
                        control->SetMarginWidth(C_LINE_MARGIN, lineNumWidth);
                        *pLineNumbersWidth = lineNumWidth;
                    }
                }
                else
                {
                    control->SetMarginWidth(C_LINE_MARGIN, pixelWidth * 0.75 + cfg->ReadInt(_T("/margin/width_chars"), 6) * pixelWidth);
                }
            }
        }
        else
        {
            m_pOwner->m_pControl->SetMarginWidth(C_LINE_MARGIN, 0);
            if (m_pOwner->m_pControl2 && both)
                m_pOwner->m_pControl2->SetMarginWidth(C_LINE_MARGIN, 0);
        }
    }

    void SetFoldingColWidth(bool both=true)
    {
        float pointSize = m_pOwner->m_pControl->StyleGetFont(wxSCI_STYLE_DEFAULT).GetPointSize();
        if (both)
        {
            int width = 16 * (pointSize+m_pOwner->m_pControl->GetZoom()) / pointSize;
            if (width < 1)
                width = 1;
            m_pOwner->m_pControl->SetMarginWidth(C_FOLDING_MARGIN, width);
            if(m_pOwner->m_pControl2)
            {
            width = 16 * (pointSize+m_pOwner->m_pControl2->GetZoom()) / pointSize;
            if (width < 1)
                width = 1;
            m_pOwner->m_pControl2->SetMarginWidth(C_FOLDING_MARGIN, width);
            }
        }
        else
        {
            int width = 16 * (pointSize+m_pOwner->GetControl()->GetZoom()) / pointSize;
            if (width < 1)
                width = 1;
            m_pOwner->GetControl()->SetMarginWidth(C_FOLDING_MARGIN, width);
        }
    }

    wxString GetUrl()
    {
        cbStyledTextCtrl* control = m_pOwner->GetControl();
        if (!control)
            return wxEmptyString;

        wxRegEx reUrl(wxT("***:("
                                "((ht|f)tp(s?)\\:\\/\\/)"
                                "|(www\\.)"
                              ")"
                              "("
                                "([\\w\\-]+(\\.[\\w\\-]+)+)"
                                "|localhost"
                              ")"
                              "(\\/?)([\\w\\-\\.\\?\\,\\'\\/\\\\\\+&amp;%\\$#]*)?"
                              "([\\d\\w\\.\\/\\%\\+\\-\\=\\&amp;\\?\\:\\\\\\&quot;\\'\\,\\|\\~\\;]*)"));
        wxString url = control->GetSelectedText();
        // Is the URL selected?
        if (reUrl.Matches(url))
            return reUrl.GetMatch(url);
        // else is there a URL near the cursor?

        // Find out start position
        int startPos = control->GetCurrentPos();
        const wxString space = wxT(" \n\r\t{}");
        wxChar curCh = control->GetCharAt(startPos);
        while ( (startPos > 0) && (space.Find(curCh) == -1) )
        {
            startPos--;
            curCh = control->GetCharAt(startPos);
        }

        // Find out end position
        int endPos = control->GetCurrentPos();
        int maxPos = control->GetLineEndPosition(control->GetLineCount());
        curCh = control->GetCharAt(endPos);
        while ( (endPos < maxPos) && (space.Find(curCh) == -1) )
        {
            endPos++;
            curCh = control->GetCharAt(endPos);
        }

        url = control->GetTextRange(startPos, endPos);
        if (    (control->GetLexer() == wxSCI_LEX_CPP)
            &&  (   (control->GetStyleAt(control->GetCurrentPos()) == wxSCI_C_STRING)
                 || (control->GetStyleAt(control->GetCurrentPos()) == wxSCI_C_STRINGEOL) ) )
        {
            url.Replace(wxT("\\n"), wxT("\n"));
            url.Replace(wxT("\\r"), wxT("\r"));
            url.Replace(wxT("\\t"), wxT("\t"));
        }

        if (reUrl.Matches(url))
        {
            wxString match = reUrl.GetMatch(url);
            if (   (url.Find(match) + startPos                       < control->GetCurrentPos())
                && (url.Find(match) + startPos + (int)match.Length() > control->GetCurrentPos()) )
            {
                url = match(0, match.find_last_not_of(wxT(",.")) + 1); // trim trailing
            }
            else
                url = wxEmptyString; // nope, too far from cursor, return invalid (empty)
        }
        else
            url = wxEmptyString; // nope, return invalid (empty)

        return url;
    }

    static wxString GetLineIndentString(int line, cbStyledTextCtrl* stc)
    {
        int currLine = (line == -1)
                        ? stc->LineFromPosition(stc->GetCurrentPos())
                        : line;
        wxString text = stc->GetLine(currLine);
        unsigned int len = text.Length();
        wxString indent;
        for (unsigned int i = 0; i < len; ++i)
        {
            if (text[i] == _T(' ') || text[i] == _T('\t'))
            {
                indent << text[i];
            }
            else
            {
                break;
            }
        }
        return indent;
    }

    /** Detect the indentation style used in a document.
     * Values for the thresholds are from trial and error over many
     * example files. If this function is not certain on the style
     * used, it will return -1.
     */
    static int DetectIndentStyle(cbStyledTextCtrl* stc)
    {
        int lineCount[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        // lineCount[0] == number of lines with tabs
        // lineCount[1] == number of lines with only spaces
        // lineCount[2 ... 8] == number of lines divisible by that number
        // Scan 1000 lines from the middle of the file to generate statistics
        const int maxLine = std::min(stc->GetLineCount(), stc->GetLineCount() / 2 + 500);
        for (int line = std::max(0, stc->GetLineCount() / 2 - 500); line < maxLine; ++line)
        {
            const wxString& indent = cbEditorInternalData::GetLineIndentString(line, stc);
            if (indent.IsEmpty())
                continue;
            if (indent.Find(wxT('\t')) != wxNOT_FOUND)
                ++lineCount[0];
            else
            {
                ++lineCount[1];
                for (int i = 2; i < 9; ++i)
                {
                    if (indent.Length() % i == 0)
                        ++lineCount[i];
                }
            }
        }

        if (lineCount[0] > 0 && lineCount[1] == 0)
            return 0;  // tabs
        double total = lineCount[0] + lineCount[1];
        if (total < 10)
            return -1; // not sure -> use defaults
        else if (lineCount[0] / total > 0.75)
            return 0;  // tabs
        else if (lineCount[1] / total < 0.75)
            return -1; // not sure -> use defaults

        total = lineCount[1];
        int tabSize = 8;
        for (int i = 2; i < 8; ++i)
        {
            if (lineCount[i] > lineCount[tabSize])
                tabSize = i;
        }
        if (lineCount[tabSize] / total < 0.65)
            return -1; // not sure -> use defaults

        switch (tabSize)
        {
            case 2:
                if ((lineCount[2] - lineCount[6]) / total < 0.1)
                    return 6;
                if (   lineCount[2] > lineCount[4] * 1.8
                    || lineCount[4] / total < 0.5 )
                    return 2;
                // fall through
            case 4:
                if (   lineCount[4] > lineCount[8] * 1.8
                    || lineCount[8] / total < 0.5 )
                    return 4;
                // fall through
            case 8:
                if (lineCount[8] / total < 0.6)
                    return -1; // not sure -> use defaults
                return 8;

            case 3:
                if (   lineCount[3] > lineCount[6] * 1.8
                    || lineCount[6] / total < 0.5 )
                    return 3;
                if (lineCount[6] / total < 0.6)
                    return -1; // not sure -> use defaults
                return 6;

            default:
                if (lineCount[tabSize] / total < 0.7)
                    return -1; // not sure -> use defaults
                return tabSize;
        }
    }

    // vars
    bool m_strip_trailing_spaces;
    bool m_ensure_final_line_end;
    bool m_ensure_consistent_line_ends;

    int m_LastMarginMenuLine;
    int m_LastDebugLine;

    bool mFoldingLimit;
    int mFoldingLimitLevel;

    wxFontEncoding m_encoding;
    bool m_useByteOrderMark;
    int m_byteOrderMarkLength;

    int m_lineNumbersWidth;
    int m_lineNumbersWidth2;

    LoaderBase* m_pFileLoader;
};
////////////////////////////////////////////////////////////////////////////////

const int idEmptyMenu = wxNewId();
const int idEdit = wxNewId();
const int idUndo = wxNewId();
const int idRedo = wxNewId();
const int idClearHistory = wxNewId();
const int idCut = wxNewId();
const int idCopy = wxNewId();
const int idPaste = wxNewId();
const int idDelete = wxNewId();
const int idUpperCase = wxNewId();
const int idLowerCase = wxNewId();
const int idSelectAll = wxNewId();
const int idSwapHeaderSource = wxNewId();
const int idOpenContainingFolder = wxNewId();
const int idBookmarks = wxNewId();
const int idBookmarksToggle = wxNewId();
const int idBookmarksPrevious = wxNewId();
const int idBookmarksNext = wxNewId();
const int idBookmarksClearAll = wxNewId();
const int idFolding = wxNewId();
const int idFoldingFoldAll = wxNewId();
const int idFoldingUnfoldAll = wxNewId();
const int idFoldingToggleAll = wxNewId();
const int idFoldingFoldCurrent = wxNewId();
const int idFoldingUnfoldCurrent = wxNewId();
const int idFoldingToggleCurrent = wxNewId();
const int idInsert = wxNewId();
const int idSplit = wxNewId();
const int idSplitHorz = wxNewId();
const int idSplitVert = wxNewId();
const int idUnsplit = wxNewId();
const int idProperties = wxNewId();
const int idAddFileToProject = wxNewId();
const int idRemoveFileFromProject = wxNewId();
const int idShowFileInProject = wxNewId();
const int idOpenUrl = wxNewId();

const int idBookmarkAdd = wxNewId();
const int idBookmarkRemove = wxNewId();
const int idBookmarkRemoveAll = wxNewId();

const int idBreakpointAdd = wxNewId();
const int idBreakpointEdit = wxNewId();
const int idBreakpointRemove = wxNewId();
const long idBreakpointEnable = wxNewId();
const long idBreakpointDisable = wxNewId();

BEGIN_EVENT_TABLE(cbEditor, EditorBase)
    EVT_CLOSE(cbEditor::OnClose)
    // we got dynamic events; look in ConnectEvents()

    EVT_MENU(idUndo, cbEditor::OnContextMenuEntry)
    EVT_MENU(idRedo, cbEditor::OnContextMenuEntry)
    EVT_MENU(idClearHistory, cbEditor::OnContextMenuEntry)
    EVT_MENU(idCut, cbEditor::OnContextMenuEntry)
    EVT_MENU(idCopy, cbEditor::OnContextMenuEntry)
    EVT_MENU(idPaste, cbEditor::OnContextMenuEntry)
    EVT_MENU(idDelete, cbEditor::OnContextMenuEntry)
    EVT_MENU(idUpperCase, cbEditor::OnContextMenuEntry)
    EVT_MENU(idLowerCase, cbEditor::OnContextMenuEntry)
    EVT_MENU(idSelectAll, cbEditor::OnContextMenuEntry)
    EVT_MENU(idSwapHeaderSource, cbEditor::OnContextMenuEntry)
    EVT_MENU(idOpenContainingFolder, cbEditor::OnContextMenuEntry)
    EVT_MENU(idBookmarksToggle, cbEditor::OnContextMenuEntry)
    EVT_MENU(idBookmarksPrevious, cbEditor::OnContextMenuEntry)
    EVT_MENU(idBookmarksNext, cbEditor::OnContextMenuEntry)
    EVT_MENU(idBookmarksClearAll, cbEditor::OnContextMenuEntry)
    EVT_MENU(idFoldingFoldAll, cbEditor::OnContextMenuEntry)
    EVT_MENU(idFoldingUnfoldAll, cbEditor::OnContextMenuEntry)
    EVT_MENU(idFoldingToggleAll, cbEditor::OnContextMenuEntry)
    EVT_MENU(idFoldingFoldCurrent, cbEditor::OnContextMenuEntry)
    EVT_MENU(idFoldingUnfoldCurrent, cbEditor::OnContextMenuEntry)
    EVT_MENU(idFoldingToggleCurrent, cbEditor::OnContextMenuEntry)
    EVT_MENU(idProperties, cbEditor::OnContextMenuEntry)
    EVT_MENU(idAddFileToProject, cbEditor::OnContextMenuEntry)
    EVT_MENU(idRemoveFileFromProject, cbEditor::OnContextMenuEntry)
    EVT_MENU(idShowFileInProject, cbEditor::OnContextMenuEntry)
    EVT_MENU(idBookmarkAdd, cbEditor::OnContextMenuEntry)
    EVT_MENU(idBookmarkRemove, cbEditor::OnContextMenuEntry)
    EVT_MENU(idBookmarkRemoveAll, cbEditor::OnContextMenuEntry)
    EVT_MENU(idBreakpointAdd, cbEditor::OnContextMenuEntry)
    EVT_MENU(idBreakpointEdit, cbEditor::OnContextMenuEntry)
    EVT_MENU(idBreakpointRemove, cbEditor::OnContextMenuEntry)
    EVT_MENU(idBreakpointEnable, cbEditor::OnContextMenuEntry)
    EVT_MENU(idBreakpointDisable, cbEditor::OnContextMenuEntry)
    EVT_MENU(idSplitHorz, cbEditor::OnContextMenuEntry)
    EVT_MENU(idSplitVert, cbEditor::OnContextMenuEntry)
    EVT_MENU(idUnsplit, cbEditor::OnContextMenuEntry)
    EVT_MENU(idOpenUrl, cbEditor::OnContextMenuEntry)

    EVT_SCI_ZOOM(-1, cbEditor::OnZoom)
    EVT_SCI_ZOOM(-1, cbEditor::OnZoom)

END_EVENT_TABLE()

// Count lines of EOL style in the opened file
static void CountLineEnds(cbStyledTextCtrl* control, int &linesCR, int &linesLF, int &linesCRLF)
{
    linesCR = 0;
    linesLF = 0;
    linesCRLF = 0;

    int lengthDoc = control->GetLength();
    const int maxLengthDoc = 1000000;
    char chPrev = ' ';
    char chNext = control->GetCharAt(0);
    for (int i = 0; i < lengthDoc; i++)
    {
        char ch = chNext;
        chNext = control->GetCharAt(i + 1);
        if (ch == '\r')
        {
            if (chNext == '\n')
                linesCRLF++;
            else
                linesCR++;
        }
        else if (ch == '\n')
        {
            if (chPrev != '\r')
                linesLF++;
        }
        else if (i > maxLengthDoc)     // stop the loop if the file contains too many characters
            return;

        chPrev = ch;
    }
}

// Detect the EOL mode of the control. If a file has mixed EOLs, we will using the voting
// logic, and give user a InfoWindow notification.
static int DetectLineEnds(cbStyledTextCtrl* control)
{
    int eolMode;
    wxString eolModeStr;
    // initial EOL mode depend on OS
    if (platform::windows)
    {
        eolMode =  wxSCI_EOL_CRLF;
        eolModeStr = _T("\"CR-LF\"");
    }
    else
    {
        eolMode =  wxSCI_EOL_LF;
        eolModeStr = _T("\"LF\"");
    }

    int linesCR;
    int linesLF;
    int linesCRLF;
    // count lines of each EOL style
    CountLineEnds(control, linesCR, linesLF, linesCRLF);

    // voting logic
    // if the file does not contain any line-feed or the most largest counts are equal( e.g.: linesLF=5,
    // linesCRLF=5, linesCR=0 ), then we will use the initial EOL mode
    if ( (linesLF > linesCR) && (linesLF > linesCRLF) )
    {
        eolMode = wxSCI_EOL_LF;
        eolModeStr = _T("\"LF\"");
    }
    else if ( (linesCR > linesLF) && (linesCR > linesCRLF) )
    {
        eolMode = wxSCI_EOL_CR;
        eolModeStr = _T("\"CR\"");
    }
    else if ( (linesCRLF > linesLF) && (linesCRLF > linesCR))
    {
        eolMode = wxSCI_EOL_CRLF;
        eolModeStr = _T("\"CR-LF\"");
    }

    unsigned int delay = 2000;
    if (  ( (linesCR>0) && (linesCRLF>0) )
       || ( (linesLF>0) && (linesCRLF>0) )
       || ( (linesCR>0) && (linesLF>0) ) )
    {
        //In mixed EOL file, give the user a beep and InfoWindow notification.
        wxBell();
        InfoWindow::Display(_("Mixed Line Endings"), _("Mixed line endings found, setting mode ") + eolModeStr, delay);
    }
    return eolMode;
}

// class constructor
cbEditor::cbEditor(wxWindow* parent, const wxString& filename, EditorColourSet* theme)
    : EditorBase(parent, filename),
    m_pSplitter(nullptr),
    m_pSizer(nullptr),
    m_pControl(nullptr),
    m_pControl2(nullptr),
    m_foldBackup(nullptr),
    m_SplitType(stNoSplit),
    m_Modified(false),
    m_Index(-1),
    m_pProjectFile(nullptr),
    m_pTheme(theme),
    m_lang(HL_AUTO)
{
    DoInitializations(filename);
}

// class constructor
cbEditor::cbEditor(wxWindow* parent, LoaderBase* fileLdr, const wxString& filename, EditorColourSet* theme)
    : EditorBase(parent, filename),
    m_pSplitter(nullptr),
    m_pSizer(nullptr),
    m_pControl(nullptr),
    m_pControl2(nullptr),
    m_foldBackup(nullptr),
    m_SplitType(stNoSplit),
    m_Modified(false),
    m_Index(-1),
    m_pProjectFile(nullptr),
    m_pTheme(theme),
    m_lang(HL_AUTO)
{
    DoInitializations(filename, fileLdr);
}

// class destructor
cbEditor::~cbEditor()
{
    SetSizer(nullptr);

    // moved in ~EditorBase
//    NotifyPlugins(cbEVT_EDITOR_CLOSE, 0, m_Filename);

    UpdateProjectFile();
    if (m_pControl)
    {
        if (m_pProjectFile)
            m_pProjectFile->editorOpen = false;
        m_pControl->Destroy();
        m_pControl = nullptr;
    }
    DestroySplitView();

    delete m_pData;
}

void cbEditor::DoInitializations(const wxString& filename, LoaderBase* fileLdr)
{
    // first thing to do!
    // if we add more constructors in the future, don't forget to set this!
    m_pData = new cbEditorInternalData(this);//, fileLdr);
    m_pData->m_pFileLoader = fileLdr;
    m_IsBuiltinEditor = true;

    if (!filename.IsEmpty())
    {
        InitFilename(filename);
        wxFileName fname(m_Filename);
        NormalizePath(fname, wxEmptyString);
        m_Filename = fname.GetFullPath();
    }
    else
    {
        static int untitledCounter = 1;
        wxString f;
        cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
        if (prj)
            f.Printf(_("%sUntitled%d"), prj->GetBasePath().c_str(), untitledCounter++);
        else
            f.Printf(_("Untitled%d"), untitledCounter++);

        InitFilename(f);
    }
//    Manager::Get()->GetLogManager()->DebugLog(_T("ctor: Filename=%s\nShort=%s"), m_Filename.c_str(), m_Shortname.c_str());

    // initialize left control (unsplit state)
    Freeze();
    m_pSizer = new wxBoxSizer(wxVERTICAL);
    m_pControl = CreateEditor();
    m_pSizer->Add(m_pControl, 1, wxEXPAND);
    SetSizer(m_pSizer);

    // the following two lines make the editors behave strangely in linux:
    // when resizing other docked windows, the editors do NOT resize too
    // and they stame the same size...
    // if commenting the following two lines causes problems in other platforms,
    // simply put an "#ifdef __WXGTK__" guard around and uncomment them.
//    m_pSizer->Fit(this);
//    m_pSizer->SetSizeHints(this);

    Thaw();
    m_pSizer->SetItemMinSize(m_pControl, 32, 32);

    // by default we show no markers, marginMasks are set explicitly in "InternalSetEditorStyleBeforeFileOpen()"
    // and/or by plugins, that use markers, like browsemarks-plugin
    m_pControl->SetMarginMask(C_LINE_MARGIN,      0);
    m_pControl->SetMarginMask(C_MARKER_MARGIN,    0);
    m_pControl->SetMarginMask(C_CHANGEBAR_MARGIN, 0);
    m_pControl->SetMarginMask(C_FOLDING_MARGIN,   0);

    SetEditorStyleBeforeFileOpen();
    m_IsOK = Open();
    SetEditorStyleAfterFileOpen();
    if (Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/folding/fold_all_on_open"), false))
        FoldAll();

    // if !m_IsOK then it's a new file, so set the modified flag ON
    if (!m_IsOK || filename.IsEmpty())
    {
        SetModified(true);
        m_IsOK = false;
    }
    ConnectEvents(m_pControl);
}

void cbEditor::NotifyPlugins(wxEventType type, int intArg, const wxString& strArg, int xArg, int yArg)
{
    if (!Manager::Get()->GetPluginManager())
        return; // no plugin manager! app shutting down?
    CodeBlocksEvent event(type);
    event.SetEditor(this);
    event.SetInt(intArg);
    event.SetString(strArg);
    event.SetX(xArg);
    event.SetY(yArg);
    if (m_pProjectFile)
        event.SetProject(m_pProjectFile->GetParentProject());
    //wxPostEvent(Manager::Get()->GetAppWindow(), event);
    Manager::Get()->GetPluginManager()->NotifyPlugins(event);
}

void cbEditor::DestroySplitView()
{
    if (m_pControl2)
    {
        m_pControl2->Destroy();
        m_pControl2 = nullptr;
    }
    if (m_pSplitter)
    {
        m_pSplitter->Destroy();
        m_pSplitter = nullptr;
    }
}

cbStyledTextCtrl* cbEditor::GetControl() const
{
    // return the last focused control (left or right)
    if (m_pControl2)
    {
        // every time a control gets the focus it stores the actual timestamp, the timestamp defaults to 0 so the
        // greater is the timestamp of the control that had the focus last time
        // finding the focused window does not work if another control has the keyboard-focus
        if ( m_pControl2->GetLastFocusTime() > m_pControl->GetLastFocusTime() )
            return m_pControl2;
    }
    return m_pControl;
}

bool cbEditor::GetModified() const
{
    return m_Modified || m_pControl->GetModify();
}

void cbEditor::SetModified(bool modified)
{
    if (modified != m_Modified)
    {
        m_Modified = modified;
        if (!m_Modified)
            m_pControl->SetSavePoint();

        SetEditorTitle(m_Shortname);
        NotifyPlugins(cbEVT_EDITOR_MODIFIED);
        // visual state
        if (m_pProjectFile)
            m_pProjectFile->SetFileState(m_pControl->GetReadOnly() ? fvsReadOnly : (m_Modified ? fvsModified : fvsNormal));
    }
}

void cbEditor::SetEditorTitle(const wxString& title)
{
    if (m_Modified)
        SetTitle(g_EditorModified + title);
    else
        SetTitle(title);
}

void cbEditor::SetProjectFile(ProjectFile* project_file, bool preserve_modified)
{
    if (m_pProjectFile == project_file)
        return; // we 've been here before ;)

    bool wasmodified = false;
    if (preserve_modified)
        wasmodified = GetModified();

    m_pProjectFile = project_file;
    if (m_pProjectFile)
    {
        // update our filename
        m_Filename = UnixFilename(project_file->file.GetFullPath());

        m_pControl->GotoPos(m_pProjectFile->editorPos);
        m_pControl->ScrollToLine(m_pProjectFile->editorTopLine);
        m_pControl->ScrollToColumn(0);
        m_pControl->SetZoom(m_pProjectFile->editorZoom);
        if (m_pProjectFile->editorSplit != (int)stNoSplit)
        {
            Split((SplitType)m_pProjectFile->editorSplit);
            if (m_pControl2)
            {
                m_pSplitter->SetSashPosition(m_pProjectFile->editorSplitPos);
                m_pControl2->GotoPos(m_pProjectFile->editorPos_2);
                m_pControl2->ScrollToLine(m_pProjectFile->editorTopLine_2);
                m_pControl2->ScrollToColumn(0);
                m_pControl2->SetZoom(m_pProjectFile->editorZoom_2);
            }
        }

        m_pData->SetLineNumberColWidth();

        if ( Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/folding/show_folds"), true) )
        {
            for (unsigned int i = 0; i < m_pProjectFile->editorFoldLinesArray.GetCount(); i++)
                m_pControl->ToggleFold(m_pProjectFile->editorFoldLinesArray[i]);
        }

        m_pProjectFile->editorOpen = true;

        if (Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/tab_text_relative"), true))
            m_Shortname = m_pProjectFile->relativeToCommonTopLevelPath;
        else
            m_Shortname = m_pProjectFile->file.GetFullName();
        SetEditorTitle(m_Shortname);

        if (!wxFileExists(m_Filename))
            m_pProjectFile->SetFileState(fvsMissing);
        else if (!wxFile::Access(m_Filename.c_str(), wxFile::write)) // readonly
            m_pProjectFile->SetFileState(fvsReadOnly);
    }

#if 0
    wxString dbg;
    dbg << _T("[ed] Filename: ") << GetFilename() << _T('\n');
    dbg << _T("[ed] Short name: ") << GetShortName() << _T('\n');
    dbg << _T("[ed] Modified: ") << GetModified() << _T('\n');
    dbg << _T("[ed] Project: ") << ((m_pProjectFile && m_pProjectFile->project) ? m_pProjectFile->project->GetTitle() : _T("unknown")) << _T('\n');
    dbg << _T("[ed] Project file: ") << (m_pProjectFile ? m_pProjectFile->relativeFilename : _T("unknown")) << _T('\n');
    Manager::Get()->GetLogManager()->DebugLog(dbg);
#endif
    if (preserve_modified)
        SetModified(wasmodified);
}

void cbEditor::UpdateProjectFile()
{
    if (m_pControl && m_pProjectFile)
    {
        m_pProjectFile->editorOpen = true;
        m_pProjectFile->editorSplit = m_SplitType;
        m_pProjectFile->editorPos = m_pControl->GetCurrentPos();
        m_pProjectFile->editorTopLine = m_pControl->GetFirstVisibleLine();
        m_pProjectFile->editorZoom = m_pControl->GetZoom();
        m_pProjectFile->editorSplitActive = 1;
        if (m_pControl2)
        {
            m_pProjectFile->editorSplitPos = m_pSplitter->GetSashPosition();
            m_pProjectFile->editorPos_2 = m_pControl2->GetCurrentPos();
            m_pProjectFile->editorTopLine_2 = m_pControl2->GetFirstVisibleLine();
            m_pProjectFile->editorZoom_2 = m_pControl2->GetZoom();
            if (GetControl()==m_pControl2)
                m_pProjectFile->editorSplitActive = 2;
        }

        if (m_pProjectFile->editorFoldLinesArray.GetCount() != 0)
            m_pProjectFile->editorFoldLinesArray.Clear();

        int i = 0;
        while ((i = m_pControl->ContractedFoldNext(i)) != -1)
            m_pProjectFile->editorFoldLinesArray.Add(i++);
    }
}

void cbEditor::SetMarkerStyle(int marker, int markerType, wxColor fore, wxColor back)
{
    m_pControl->MarkerDefine(marker, markerType);
    m_pControl->MarkerSetForeground(marker, fore);
    m_pControl->MarkerSetBackground(marker, back);

    if (m_pControl2)
    {
        m_pControl2->MarkerDefine(marker, markerType);
        m_pControl2->MarkerSetForeground(marker, fore);
        m_pControl2->MarkerSetBackground(marker, back);
    }
}

void cbEditor::UnderlineFoldedLines(bool underline)
{
    m_pControl->SetFoldFlags(underline ? 16 : 0);
    if (m_pControl2)
        m_pControl2->SetFoldFlags(underline ? 16 : 0);
}

cbStyledTextCtrl* cbEditor::CreateEditor()
{
    // avoid gtk-critical because of sizes less than -1 (can happen with wxAuiNotebook/cbAuiNotebook)
    wxSize size = m_pControl ? wxDefaultSize : GetSize();
    size.x = std::max(size.x, -1);
    size.y = std::max(size.y, -1);

    cbStyledTextCtrl* control = new cbStyledTextCtrl(this, wxNewId(), wxDefaultPosition, size);
    control->UsePopUp(false);

    m_pData->m_encoding = wxFontMapper::GetEncodingFromName(
        Manager::Get()->GetConfigManager(_T("editor"))->Read(_T("/default_encoding"), wxEmptyString) );

    for (int marker = 0 ; marker <= wxSCI_MARKNUM_LASTUNUSED ; ++marker)
        control->MarkerDefine(marker, wxSCI_MARK_EMPTY);

    return control;
}

void cbEditor::ConnectEvents(cbStyledTextCtrl* stc)
{
    wxWindowID stcID = stc->GetId();
    // dynamic events
    Connect( stcID, wxEVT_SCI_MARGINCLICK,       wxScintillaEventHandler(cbEditor::OnMarginClick)       );
    Connect( stcID, wxEVT_SCI_UPDATEUI,          wxScintillaEventHandler(cbEditor::OnEditorUpdateUI)    );
    Connect( stcID, wxEVT_SCI_CHANGE,            wxScintillaEventHandler(cbEditor::OnEditorChange)      );
    Connect( stcID, wxEVT_SCI_CHARADDED,         wxScintillaEventHandler(cbEditor::OnEditorCharAdded)   );
    Connect( stcID, wxEVT_SCI_DWELLSTART,        wxScintillaEventHandler(cbEditor::OnEditorDwellStart)  );
    Connect( stcID, wxEVT_SCI_DWELLEND,          wxScintillaEventHandler(cbEditor::OnEditorDwellEnd)    );
    Connect( stcID, wxEVT_SCI_USERLISTSELECTION, wxScintillaEventHandler(cbEditor::OnUserListSelection) );
    Connect( stcID, wxEVT_SCI_MODIFIED,          wxScintillaEventHandler(cbEditor::OnEditorModified)    );

    // Now bind all *other* scintilla events to a common function so that editor hooks
    // can be informed for them too.
    // If you implement one of these events using a different function, do the following:
    //  * comment it out here,
    //  * "connect" it in the above block
    //  * and make sure you call OnScintillaEvent() from your new handler function
    // This will make sure that all editor hooks will be called when needed.
    int scintilla_events[] =
    {
//        wxEVT_SCI_CHANGE,
        wxEVT_SCI_STYLENEEDED,
//        wxEVT_SCI_CHARADDED,
        wxEVT_SCI_SAVEPOINTREACHED,
        wxEVT_SCI_SAVEPOINTLEFT,
        wxEVT_SCI_ROMODIFYATTEMPT,
        wxEVT_SCI_KEY,
        wxEVT_SCI_DOUBLECLICK,
//        wxEVT_SCI_UPDATEUI,
//        wxEVT_SCI_MODIFIED,
        wxEVT_SCI_MACRORECORD,
//        wxEVT_SCI_MARGINCLICK,
        wxEVT_SCI_NEEDSHOWN,
        wxEVT_SCI_PAINTED,
//        wxEVT_SCI_USERLISTSELECTION,
        wxEVT_SCI_URIDROPPED,
//        wxEVT_SCI_DWELLSTART,
//        wxEVT_SCI_DWELLEND,
        wxEVT_SCI_START_DRAG,
        wxEVT_SCI_FINISHED_DRAG,
        wxEVT_SCI_DRAG_OVER,
        wxEVT_SCI_DO_DROP,
        wxEVT_SCI_ZOOM,
        wxEVT_SCI_HOTSPOT_CLICK,
        wxEVT_SCI_HOTSPOT_DCLICK,
        wxEVT_SCI_CALLTIP_CLICK,
        wxEVT_SCI_AUTOCOMP_SELECTION,
//        wxEVT_SCI_INDICATOR_CLICK,
//        wxEVT_SCI_INDICATOR_RELEASE,
        wxEVT_SCI_AUTOCOMP_CANCELLED,
        wxEVT_SCI_TAB,
        wxEVT_SCI_ESC,

        -1 // to help enumeration of this array
    };
    int i = 0;
    while (scintilla_events[i] != -1)
    {
        Connect( stcID, scintilla_events[i], wxScintillaEventHandler(cbEditor::OnScintillaEvent) );
        ++i;
    }
}

void cbEditor::Split(cbEditor::SplitType split)
{
    Freeze();

    // unsplit first, if needed
    if (m_pSplitter)
    {
        Unsplit();
        Manager::Yield();
    }
    m_SplitType = split;
    if (m_SplitType == stNoSplit)
    {
        Thaw();
        return;
    }

    // remove the left control from the sizer
    m_pSizer->Detach(m_pControl);

    // create the splitter window
    m_pSplitter = new wxSplitterWindow(this, wxNewId(), wxDefaultPosition, wxDefaultSize, wxSP_NOBORDER | wxSP_LIVE_UPDATE);
    m_pSplitter->SetMinimumPaneSize(32);

    // create the right control
    m_pControl2 = CreateEditor();

    // update controls' look'n'feel
    // do it here (before) document is attached, speeds up syntaxhighlighting
    // we do not call "SetEditorStyleAfterFileOpen" here because it calls SetLanguage for the already loaded text inside
    // the left control and slows down loading of large files a lot.
    InternalSetEditorStyleBeforeFileOpen(m_pControl2);

    // make sure basic settings of indicators (maybe set by plugins) are used for the new control
    for (int i = 0; i < wxSCI_INDIC_MAX; ++i )
    {
        m_pControl2->IndicatorSetStyle(i, m_pControl->IndicatorGetStyle(i));
        m_pControl2->IndicatorSetUnder(i, m_pControl->IndicatorGetUnder(i));
        m_pControl2->IndicatorSetForeground(i, m_pControl->IndicatorGetForeground(i));
    }

    ConfigManager* mgr = Manager::Get()->GetConfigManager(_T("editor"));
    SetFoldingIndicator(mgr->ReadInt(_T("/folding/indicator"), 2));
    UnderlineFoldedLines(mgr->ReadBool(_T("/folding/underline_folded_line"), true));

    if (m_pTheme)
        m_pTheme->Apply(m_lang, m_pControl2);

    // and make it a live copy of left control
    m_pControl2->SetDocPointer(m_pControl->GetDocPointer());

    // on wxGTK > 2.9 we need to thaw before reparent and refreeze the editor here or the whole app stays frozen
    #if defined ( __WXGTK__ ) && wxCHECK_VERSION(3, 0, 0)
    Thaw();
    #endif
    // parent both controls under the splitter
    m_pControl->Reparent(m_pSplitter);
    m_pControl2->Reparent(m_pSplitter);
    #if defined ( __WXGTK__ ) && wxCHECK_VERSION(3, 0, 0)
    Freeze();
    #endif

    // add the splitter in the sizer
    m_pSizer->SetDimension(0, 0, GetSize().x, GetSize().y);
    m_pSizer->Add(m_pSplitter, 1, wxEXPAND);

    m_pSizer->Layout();

    // split as needed
    switch (m_SplitType)
    {
        case stHorizontal:
            m_pSplitter->SplitHorizontally(m_pControl, m_pControl2, 0);
            break;

        case stVertical:
            m_pSplitter->SplitVertically(m_pControl, m_pControl2, 0);
            break;

        case stNoSplit: // fall-trough
        default:
            break;
    }

    SetEditorStyleAfterFileOpen();

    // initial zoom is same as left/top control
    m_pControl2->SetZoom(m_pControl->GetZoom());
    // make sure the line numbers margin is correct for the new control
    m_pControl2->SetMarginWidth(C_LINE_MARGIN, m_pControl->GetMarginWidth(C_LINE_MARGIN));

    ConnectEvents(m_pControl2);

    NotifyPlugins(cbEVT_EDITOR_SPLIT);

    Thaw();
}

void cbEditor::Unsplit()
{
    m_SplitType = stNoSplit;
    if (!m_pSplitter)
        return;

    Freeze();

    // if "unsplit" requested on right control, swap left-right first
    if (GetControl() == m_pControl2)
    {
        cbStyledTextCtrl* tmp = m_pControl;
        m_pControl = m_pControl2;
        m_pControl2 = tmp;
    }

    // remove the splitter from the sizer
    m_pSizer->Detach(m_pSplitter);

    // on wxGTK > 2.9 we need to thaw before reparent and refreeze the editor here or the whole app stays frozen
    #if defined ( __WXGTK__ ) && wxCHECK_VERSION(3, 0, 0)
    Thaw();
    #endif
    // parent the left control under this
    m_pControl->Reparent(this);
    #if defined ( __WXGTK__ ) && wxCHECK_VERSION(3, 0, 0)
    Freeze();
    #endif
    // add it in the sizer
    m_pSizer->Add(m_pControl, 1, wxEXPAND);
    // notify the plugin when the right splitter window is not destroyed and the left window is reparented to cbEditor
    NotifyPlugins(cbEVT_EDITOR_UNSPLIT);
    // destroy the splitter and right control
    DestroySplitView();
    // and layout
    m_pSizer->Layout();

    Thaw();
}

// static
wxColour cbEditor::GetOptionColour(const wxString& option, const wxColour _default)
{
    return Manager::Get()->GetConfigManager(_T("editor"))->ReadColour(option, _default);
}

void cbEditor::SetEditorStyle()
{
    SetEditorStyleBeforeFileOpen();
    SetEditorStyleAfterFileOpen();
}

inline void OverrideUseTabsPerLanguage(cbStyledTextCtrl *control)
{
    if (!control)
        return;
    // override the use tab setting for Python files and Makefiles
    int lexer = control->GetLexer();
    switch (lexer)
    {
        case wxSCI_LEX_PYTHON:
            control->SetUseTabs(false);
            break;
        case wxSCI_LEX_MAKEFILE:
            control->SetUseTabs(true);
            break;
        default:
            break;
    }
}

void cbEditor::SetEditorStyleBeforeFileOpen()
{
    ConfigManager* mgr = Manager::Get()->GetConfigManager(_T("editor"));

    // update the tab text based on preferences
    if (m_pProjectFile)
    {
        if (mgr->ReadBool(_T("/tab_text_relative"), true))
            m_Shortname = m_pProjectFile->relativeToCommonTopLevelPath;
        else
            m_Shortname = m_pProjectFile->file.GetFullName();
        SetEditorTitle(m_Shortname);
    }

    // Folding properties.
    m_pData->mFoldingLimit = mgr->ReadBool(_T("/folding/limit"), false);
    m_pData->mFoldingLimitLevel = mgr->ReadInt(_T("/folding/limit_level"), 1);

    // EOL properties
    m_pData->m_strip_trailing_spaces = mgr->ReadBool(_T("/eol/strip_trailing_spaces"), true);
    m_pData->m_ensure_final_line_end = mgr->ReadBool(_T("/eol/ensure_final_line_end"), true);
    m_pData->m_ensure_consistent_line_ends = mgr->ReadBool(_T("/eol/ensure_consistent_line_ends"), false);

    InternalSetEditorStyleBeforeFileOpen(m_pControl);

    if (m_pControl2)
        InternalSetEditorStyleBeforeFileOpen(m_pControl2);

    SetFoldingIndicator(mgr->ReadInt(_T("/folding/indicator"), 2));

    SetLanguage( HL_AUTO );

    OverrideUseTabsPerLanguage(m_pControl);
    OverrideUseTabsPerLanguage(m_pControl2);
}

void cbEditor::SetEditorStyleAfterFileOpen()
{
    InternalSetEditorStyleAfterFileOpen(m_pControl);
    if (m_pControl2)
        InternalSetEditorStyleAfterFileOpen(m_pControl2);

    ConfigManager* mgr = Manager::Get()->GetConfigManager(_T("editor"));

    UnderlineFoldedLines(mgr->ReadBool(_T("/folding/underline_folded_line"), true));

    // line numbers
    m_pData->SetLineNumberColWidth();
}

// static
// public version of InternalSetEditorStyleBeforeFileOpen
void cbEditor::ApplyStyles(cbStyledTextCtrl* control)
{
    if (!control)
        return;

    InternalSetEditorStyleBeforeFileOpen(control);
    InternalSetEditorStyleAfterFileOpen(control);
}

// static
void cbEditor::InternalSetEditorStyleBeforeFileOpen(cbStyledTextCtrl* control)
{
    if (!control)
        return;

    control->Colourise(0, -1);

    ConfigManager* mgr = Manager::Get()->GetConfigManager(_T("editor"));

    // setting the default editor font size to 10 point
    wxFont font(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

    wxString fontstring = mgr->Read(_T("/font"), wxEmptyString);

    if (!fontstring.IsEmpty())
    {
        wxNativeFontInfo nfi;
        nfi.FromString(fontstring);
        font.SetNativeFontInfo(nfi);
    }

    control->SetMouseDwellTime(1000);

    int caretStyle = mgr->ReadInt(_T("/caret/style"), wxSCI_CARETSTYLE_LINE);
    control->SetCaretStyle(caretStyle);
    if (caretStyle == wxSCI_CARETSTYLE_LINE)
        control->SetCaretWidth(mgr->ReadInt(_T("/caret/width"), 1));
    else
        control->SetCaretWidth(1);

    ColourManager *colours = Manager::Get()->GetColourManager();

    control->SetCaretForeground(colours->GetColour(wxT("editor_caret")));
    control->SetCaretPeriod(mgr->ReadInt(_T("/caret/period"), 500));
    control->SetCaretLineVisible(mgr->ReadBool(_T("/highlight_caret_line"), false));
    control->SetCaretLineBackground(GetOptionColour(_T("/highlight_caret_line_colour"), wxColour(0xFF, 0xFF, 0x00)));

    control->SetFoldMarginColour(true, colours->GetColour(wxT("editor_margin_chrome")));
    control->SetFoldMarginHiColour(true, colours->GetColour(wxT("editor_margin_chrome_highlight")));

    control->SetWhitespaceForeground(true, colours->GetColour(wxT("editor_whitespace")));

    // setup for "CamelCase selection"
    if (mgr->ReadBool(_T("/camel_case"), false))
    {
        // consider CamelCase for both: cursor movement with CTRL and selection with CTRL+SHIFT:
        control->CmdKeyAssign(wxSCI_KEY_LEFT,  wxSCI_SCMOD_CTRL,                   wxSCI_CMD_WORDPARTLEFT);
        control->CmdKeyAssign(wxSCI_KEY_RIGHT, wxSCI_SCMOD_CTRL,                   wxSCI_CMD_WORDPARTRIGHT);
        control->CmdKeyAssign(wxSCI_KEY_LEFT,  wxSCI_SCMOD_CTRL|wxSCI_SCMOD_SHIFT, wxSCI_CMD_WORDPARTLEFTEXTEND);
        control->CmdKeyAssign(wxSCI_KEY_RIGHT, wxSCI_SCMOD_CTRL|wxSCI_SCMOD_SHIFT, wxSCI_CMD_WORDPARTRIGHTEXTEND);
    }
    else // else set default "none CamelCase" key behavior (also default scintilla behaviour, see scintilla docs)
    {
        control->CmdKeyAssign(wxSCI_KEY_LEFT,  wxSCI_SCMOD_CTRL,                   wxSCI_CMD_WORDLEFT);
        control->CmdKeyAssign(wxSCI_KEY_RIGHT, wxSCI_SCMOD_CTRL,                   wxSCI_CMD_WORDRIGHT);
        control->CmdKeyAssign(wxSCI_KEY_LEFT,  wxSCI_SCMOD_CTRL|wxSCI_SCMOD_SHIFT, wxSCI_CMD_WORDLEFTEXTEND);
        control->CmdKeyAssign(wxSCI_KEY_RIGHT, wxSCI_SCMOD_CTRL|wxSCI_SCMOD_SHIFT, wxSCI_CMD_WORDRIGHTEXTEND);
    }

    control->SetUseTabs(mgr->ReadBool(_T("/use_tab"), false));
    control->SetIndentationGuides(mgr->ReadBool(_T("/show_indent_guides"), false)?wxSCI_IV_LOOKBOTH:wxSCI_IV_NONE);
    control->SetTabIndents(mgr->ReadBool(_T("/tab_indents"), true));
    control->SetBackSpaceUnIndents(mgr->ReadBool(_T("/backspace_unindents"), true));
    control->SetWrapMode(mgr->ReadBool(_T("/word_wrap"), false));
    if (mgr->ReadBool(_T("/word_wrap_style_home_end"), true))
    {
        // in word wrap mode, home/end keys goto the wrap point if not already there,
        // otherwise to the start/end of the entire line.
        // alt+home/end go to start/end of the entire line.
        // in unwrapped mode, there is no difference between home/end and alt+home/end
        control->CmdKeyAssign(wxSCI_KEY_END,  wxSCI_SCMOD_NORM,                  wxSCI_CMD_LINEENDWRAP);
        control->CmdKeyAssign(wxSCI_KEY_END,  wxSCI_SCMOD_ALT,                   wxSCI_CMD_LINEEND);
        control->CmdKeyAssign(wxSCI_KEY_END,  wxSCI_SCMOD_SHIFT,                 wxSCI_CMD_LINEENDWRAPEXTEND);
        control->CmdKeyAssign(wxSCI_KEY_END,  wxSCI_SCMOD_SHIFT|wxSCI_SCMOD_ALT, wxSCI_CMD_LINEENDEXTEND);

        // if user wants "Home" key to set cursor to the very beginning of line
        if (mgr->ReadBool(_T("/simplified_home"), false))
        {
            control->CmdKeyAssign(wxSCI_KEY_HOME,wxSCI_SCMOD_NORM,wxSCI_CMD_HOMEWRAP);
            control->CmdKeyAssign(wxSCI_KEY_HOME,wxSCI_SCMOD_ALT,wxSCI_CMD_HOME);
            control->CmdKeyAssign(wxSCI_KEY_HOME,wxSCI_SCMOD_SHIFT,wxSCI_CMD_HOMEWRAPEXTEND);
            control->CmdKeyAssign(wxSCI_KEY_HOME,wxSCI_SCMOD_SHIFT|wxSCI_SCMOD_ALT,wxSCI_CMD_HOMEEXTEND);
        }
        else // else set default "Home" key behaviour
        {
            control->CmdKeyAssign(wxSCI_KEY_HOME,wxSCI_SCMOD_NORM,wxSCI_CMD_VCHOMEWRAP);
            control->CmdKeyAssign(wxSCI_KEY_HOME,wxSCI_SCMOD_ALT,wxSCI_CMD_VCHOME);
            control->CmdKeyAssign(wxSCI_KEY_HOME,wxSCI_SCMOD_SHIFT,wxSCI_CMD_VCHOMEWRAPEXTEND);
            control->CmdKeyAssign(wxSCI_KEY_HOME,wxSCI_SCMOD_SHIFT|wxSCI_SCMOD_ALT,wxSCI_CMD_VCHOMEEXTEND);
        }
    }
    else
    {   // in word wrap mode, home/end keys goto start/end of the entire line. alt+home/end goes to wrap points
        control->CmdKeyAssign(wxSCI_KEY_END,  wxSCI_SCMOD_ALT,                   wxSCI_CMD_LINEENDWRAP);
        control->CmdKeyAssign(wxSCI_KEY_END,  wxSCI_SCMOD_SHIFT|wxSCI_SCMOD_ALT, wxSCI_CMD_LINEENDWRAPEXTEND);

        // if user wants "Home" key to set cursor to the very beginning of line
        if (mgr->ReadBool(_T("/simplified_home"), false))
        {
            control->CmdKeyAssign(wxSCI_KEY_HOME,wxSCI_SCMOD_ALT,wxSCI_CMD_HOMEWRAP);
            control->CmdKeyAssign(wxSCI_KEY_HOME,wxSCI_SCMOD_SHIFT|wxSCI_SCMOD_ALT,wxSCI_CMD_HOMEWRAPEXTEND);
        }
        else // else set default "Home" key behaviour
        {
            control->CmdKeyAssign(wxSCI_KEY_HOME,wxSCI_SCMOD_ALT,wxSCI_CMD_VCHOMEWRAP);
            control->CmdKeyAssign(wxSCI_KEY_HOME,wxSCI_SCMOD_SHIFT|wxSCI_SCMOD_ALT,wxSCI_CMD_VCHOMEWRAPEXTEND);
        }
    }
    control->SetViewEOL(mgr->ReadBool(_T("/show_eol"), false));
    control->SetViewWhiteSpace(mgr->ReadInt(_T("/view_whitespace"), 0));
    // gutter
    control->SetEdgeMode(mgr->ReadInt(_T("/gutter/mode"), 0));
    control->SetEdgeColour(Manager::Get()->GetColourManager()->GetColour(wxT("editor_gutter")));
    control->SetEdgeColumn(mgr->ReadInt(_T("/gutter/column"), 80));

    control->StyleSetFont(wxSCI_STYLE_DEFAULT, font);
    control->StyleClearAll();

    control->SetTabWidth(mgr->ReadInt(_T("/tab_size"), 4));

    // margin for bookmarks, breakpoints etc.
    // FIXME: how to display a mark with an offset???
    control->SetMarginWidth(C_MARKER_MARGIN, 16);
    control->SetMarginType(C_MARKER_MARGIN, wxSCI_MARGIN_SYMBOL);
    control->SetMarginSensitive(C_MARKER_MARGIN, mgr->ReadBool(_T("/margin_1_sensitive"), true));
    // use "|" here or we might break plugins that use the margin (like browsemarks)
    control->SetMarginMask(C_MARKER_MARGIN,
                           control->GetMarginMask(C_MARKER_MARGIN)
                           | (1 << BOOKMARK_MARKER)
                           | (1 << BREAKPOINT_MARKER)
                           | (1 << BREAKPOINT_DISABLED_MARKER)
                           | (1 << BREAKPOINT_OTHER_MARKER)
                           | (1 << DEBUG_MARKER)
                           | (1 << DEBUG_MARKER_HIGHLIGHT)
                           | (1 << ERROR_MARKER) );

    // 1.) Marker for Bookmarks etc...
    control->MarkerDefine(BOOKMARK_MARKER, BOOKMARK_STYLE);
    control->MarkerSetBackground(BOOKMARK_MARKER, wxColour(0xA0, 0xA0, 0xFF));

    // 2.) Marker for Breakpoints etc...
    const wxString &basepath = ConfigManager::GetDataFolder() + wxT("/manager_resources.zip#zip:/images/12x12/");
    bool imageBP = mgr->ReadBool(_T("/margin_1_image_bp"), true);
    if (imageBP)
    {
      wxBitmap iconBP    = cbLoadBitmap(basepath + wxT("breakpoint.png"),          wxBITMAP_TYPE_PNG);
      wxBitmap iconBPDis = cbLoadBitmap(basepath + wxT("breakpoint_disabled.png"), wxBITMAP_TYPE_PNG);
      wxBitmap iconBPOth = cbLoadBitmap(basepath + wxT("breakpoint_other.png"),    wxBITMAP_TYPE_PNG);
      if (iconBP.IsOk() && iconBPDis.IsOk() && iconBPOth.IsOk())
      {
          control->MarkerDefineBitmap(BREAKPOINT_MARKER,          iconBP   );
          control->MarkerDefineBitmap(BREAKPOINT_DISABLED_MARKER, iconBPDis);
          control->MarkerDefineBitmap(BREAKPOINT_OTHER_MARKER,    iconBPOth);
      }
      else
        imageBP = false; // apply default markers
    }
    if (!imageBP)
    {
        control->MarkerDefine(BREAKPOINT_MARKER,                 BREAKPOINT_STYLE);
        control->MarkerSetBackground(BREAKPOINT_MARKER,          wxColour(0xFF, 0x00, 0x00));
        control->MarkerDefine(BREAKPOINT_DISABLED_MARKER,        BREAKPOINT_STYLE);
        control->MarkerSetBackground(BREAKPOINT_DISABLED_MARKER, wxColour(0x90, 0x90, 0x90));
        control->MarkerDefine(BREAKPOINT_OTHER_MARKER,           BREAKPOINT_STYLE);
        control->MarkerSetBackground(BREAKPOINT_OTHER_MARKER,    wxColour(0x59, 0x74, 0x8e));
    }
    // 3.) Marker for Debugging (currently debugged line) etc...
    control->MarkerDefine(DEBUG_MARKER, DEBUG_STYLE);
    control->MarkerSetBackground(DEBUG_MARKER, wxColour(0xFF, 0xFF, 0x00));

    control->MarkerDefine(DEBUG_MARKER_HIGHLIGHT, DEBUG_STYLE_HIGHLIGHT);
    control->MarkerSetBackground(DEBUG_MARKER_HIGHLIGHT, control->GetCaretLineBackground());

    // 4.) Marker for Errors...
    control->MarkerDefine(ERROR_MARKER, ERROR_STYLE);
    control->MarkerSetBackground(ERROR_MARKER, wxColour(0xFF, 0x00, 0x00));

    // changebar margin
    if (mgr->ReadBool(_T("/margin/use_changebar"), true))
    {
        control->SetMarginWidth(C_CHANGEBAR_MARGIN, 4);
        control->SetMarginType(C_CHANGEBAR_MARGIN,  wxSCI_MARGIN_SYMBOL);
        // use "|" here or we might break plugins that use the margin (none at the moment)
        control->SetMarginMask(C_CHANGEBAR_MARGIN,
                               control->GetMarginMask(C_CHANGEBAR_MARGIN)
                               | (1 << wxSCI_MARKNUM_CHANGEUNSAVED)
                               | (1 << wxSCI_MARKNUM_CHANGESAVED) );

        control->MarkerDefine(wxSCI_MARKNUM_CHANGEUNSAVED, wxSCI_MARK_LEFTRECT);
        control->MarkerSetBackground(wxSCI_MARKNUM_CHANGEUNSAVED, wxColour(0xFF, 0xE6, 0x04));
        control->MarkerDefine(wxSCI_MARKNUM_CHANGESAVED, wxSCI_MARK_LEFTRECT);
        control->MarkerSetBackground(wxSCI_MARKNUM_CHANGESAVED,   wxColour(0x04, 0xFF, 0x50));
    }
    else
        control->SetMarginWidth(C_CHANGEBAR_MARGIN, 0);

    // NOTE: duplicate line in editorconfigurationdlg.cpp (ctor)
    control->SetScrollWidthTracking(      mgr->ReadBool(_T("/margin/scroll_width_tracking"), false));
    control->SetMultipleSelection(        mgr->ReadBool(_T("/selection/multi_select"),       false));
    control->SetAdditionalSelectionTyping(mgr->ReadBool(_T("/selection/multi_typing"),       false));

    unsigned virtualSpace = 0;
    if (mgr->ReadBool(_T("/selection/use_rect_vspace"), false))
        virtualSpace |= wxSCI_SCVS_RECTANGULARSELECTION;
    if (mgr->ReadBool(_T("/selection/use_vspace"), false))
        virtualSpace |= wxSCI_SCVS_USERACCESSIBLE;
    if (!virtualSpace)
        virtualSpace = wxSCI_SCVS_NONE; // Just in case wxSCI_SCVS_NONE != 0
    control->SetVirtualSpaceOptions(virtualSpace);
}

// static
void cbEditor::InternalSetEditorStyleAfterFileOpen(cbStyledTextCtrl* control)
{
    if (!control)
        return;

    ConfigManager* mgr = Manager::Get()->GetConfigManager(_T("editor"));

    // set the EOL, fall back value: Windows takes CR+LF, other platforms LF only
    int eolMode = mgr->ReadInt(_T("/eol/eolmode"), platform::windows ? wxSCI_EOL_CRLF : wxSCI_EOL_LF);

    if (eolMode == 3) //auto detect the EOL
        eolMode = DetectLineEnds(control);

    control->SetEOLMode(eolMode);

    // indentation style is already set
    if (mgr->ReadBool(_T("/detect_indent"), false))
    {
        // override style if auto-detection succeeds
        int indentStyle = cbEditorInternalData::DetectIndentStyle(control);
        if (indentStyle == 0)
            control->SetUseTabs(true);
        else if (indentStyle != -1)
        {
            control->SetUseTabs(false);
            control->SetTabWidth(indentStyle);
        }
    }

    // Interpret #if/#else/#endif to grey out code that is not active
    control->SetProperty(_T("lexer.cpp.track.preprocessor"), mgr->ReadBool(_T("/track_preprocessor"), true) ? _T("1") : _T("0"));

    // code folding
    if (mgr->ReadBool(_T("/folding/show_folds"), true))
    {
        control->SetProperty(_T("fold"),              _T("1"));
        control->SetProperty(_T("fold.html"),         mgr->ReadBool(_T("/folding/fold_xml"), true) ? _T("1") : _T("0"));
        control->SetProperty(_T("fold.comment"),      mgr->ReadBool(_T("/folding/fold_comments"), false) ? _T("1") : _T("0"));
        control->SetProperty(_T("fold.compact"),      _T("0"));
        control->SetProperty(_T("fold.preprocessor"), mgr->ReadBool(_T("/folding/fold_preprocessor"), false) ? _T("1") : _T("0"));

        control->SetFoldFlags(16);
        control->SetMarginType(C_FOLDING_MARGIN, wxSCI_MARGIN_SYMBOL);
        control->SetMarginWidth(C_FOLDING_MARGIN, 16);
        // use "|" here or we might break plugins that use the margin (none at the moment)
        control->SetMarginMask(C_FOLDING_MARGIN,
                                 control->GetMarginMask(C_FOLDING_MARGIN)
                               | (  wxSCI_MASK_FOLDERS
                                  - (  (1 << wxSCI_MARKNUM_CHANGEUNSAVED)
                                     | (1 << wxSCI_MARKNUM_CHANGESAVED))) );
        control->SetMarginSensitive(C_FOLDING_MARGIN, 1);
    }
    else
    {
        control->SetProperty(_T("fold"), _T("0"));
        control->SetMarginWidth(C_FOLDING_MARGIN, 0);
    }
    control->SetProperty(_T("highlight.wxsmith"), mgr->ReadBool(_T("/highlight_wxsmith"), true) ? _T("1") : _T("0"));

    // line numbering
    control->SetMarginType(C_LINE_MARGIN, wxSCI_MARGIN_NUMBER);
}

void cbEditor::SetColourSet(EditorColourSet* theme)
{
    m_pTheme = theme;
    SetLanguage( m_lang );
}

wxFontEncoding cbEditor::GetEncoding() const
{
    if (!m_pData)
        return wxFONTENCODING_SYSTEM;

    return m_pData->m_encoding;
}

wxString cbEditor::GetEncodingName( ) const
{
    return wxFontMapper::GetEncodingName(GetEncoding());
}

void cbEditor::SetEncoding(wxFontEncoding encoding)
{
    if (!m_pData)
        return;

    if (encoding == wxFONTENCODING_SYSTEM)
        encoding = wxLocale::GetSystemEncoding();

    if (encoding == m_pData->m_encoding)
        return;

    m_pData->m_encoding = encoding;
    SetModified(true);
}

bool cbEditor::GetUseBom() const
{
    if (!m_pData)
        return false;
    return m_pData->m_useByteOrderMark;
}

void cbEditor::SetUseBom( bool bom )
{
    if (!m_pData)
        return;

    if ( bom == GetUseBom() )
        return;

    m_pData->m_useByteOrderMark = bom;
    SetModified(true);
}

bool cbEditor::Reload(bool detect_encoding)
{
    // keep current pos
    const int pos = m_pControl ? m_pControl->GetCurrentPos() : 0;
    const int pos2 = m_pControl2 ? m_pControl2->GetCurrentPos() : 0;

    // call open
    if (!Open(detect_encoding))
    {
        return false;
    }
    // Re-establish margin styles, width,  etc
    SetEditorStyleAfterFileOpen();

    // return (if possible) to old pos
    if (m_pControl)
    {
        m_pControl->GotoPos(pos);
    }
    if (m_pControl2)
    {
        m_pControl2->GotoPos(pos2);
    }
    return true;
} // end of Reload

void cbEditor::Touch()
{
    m_LastModified = wxDateTime::Now();
}

void cbEditor::SetLanguage(HighlightLanguage lang)
{
    if (m_pTheme)
        m_lang = m_pTheme->Apply(this, lang);
    else
        m_lang = HL_AUTO;
}

bool cbEditor::Open(bool detectEncoding)
{
    if (m_pProjectFile)
    {
        if (!wxFileExists(m_Filename))
            m_pProjectFile->SetFileState(fvsMissing);
        else if (!wxFile::Access(m_Filename.c_str(), wxFile::write)) // readonly
            m_pProjectFile->SetFileState(fvsReadOnly);
    }

    if (!wxFileExists(m_Filename))
        return false;

    // open file
    SetReadOnly(false);

    m_pControl->ClearAll();
    m_pControl->SetModEventMask(0);

    if (!m_pData)
        return false;

    if (!m_pData->m_pFileLoader)
        m_pData->m_pFileLoader = Manager::Get()->GetFileManager()->Load(m_Filename, false);

#ifdef fileload_measuring
    wxStopWatch sw;
#endif
    EncodingDetector enc((wxByte*)m_pData->m_pFileLoader->GetData(), m_pData->m_pFileLoader->GetLength());
    if (detectEncoding)
    {
        m_pData->m_useByteOrderMark    = enc.UsesBOM();
        m_pData->m_byteOrderMarkLength = enc.GetBOMSizeInBytes();
        m_pData->m_encoding            = enc.GetFontEncoding();

        SetEncoding(enc.GetFontEncoding());
        SetUseBom(m_pData->m_byteOrderMarkLength > 0);
    }

    ConfigManager* mgr = Manager::Get()->GetConfigManager(_T("editor"));
#ifdef fileload_measuring
    Manager::Get()->GetLogManager()->DebugLog(F(_T("cbEditor::Open() => Encoding detection and conversion took : %d ms"),(int)sw.Time()));
    sw.Start();
#endif

    m_pControl->InsertText(0, enc.GetWxStr());
    m_pControl->EmptyUndoBuffer(mgr->ReadBool(_T("/margin/use_changebar"), true));
    m_pControl->SetModEventMask(wxSCI_MODEVENTMASKALL);

    // mark the file read-only, if applicable
    bool read_only = !wxFile::Access(m_Filename.c_str(), wxFile::write);
    SetReadOnly(read_only);

    wxFileName fname(m_Filename);
    m_LastModified = fname.GetModificationTime();

    SetModified(false);

    NotifyPlugins(cbEVT_EDITOR_OPEN);

    if (m_pData->m_pFileLoader)
    {
        delete m_pData->m_pFileLoader;
        m_pData->m_pFileLoader = nullptr;
    }
#ifdef fileload_measuring
    Manager::Get()->GetLogManager()->DebugLog(F(_T("loading into editor needs : %d ms"),(int)sw.Time()));
#endif
    return true;
}

bool cbEditor::Save()
{
    if ( !GetModified() )
        return true;

    // remember current column (caret and anchor)
    int columnC = m_pControl->GetColumn(m_pControl->GetCurrentPos());
    int columnA = m_pControl->GetColumn(m_pControl->GetAnchor());

    // one undo action for all modifications in this context
    // (angled braces added for clarity)
    m_pControl->BeginUndoAction();
    {
        if (m_pData->m_strip_trailing_spaces)
            m_pData->StripTrailingSpaces();
        if (m_pData->m_ensure_consistent_line_ends)
            m_pData->EnsureConsistentLineEnds();
        if (m_pData->m_ensure_final_line_end)
            m_pData->EnsureFinalLineEnd();
    }
    m_pControl->EndUndoAction();

    // restore virtual position ( if changed by StripTrailingSpaces() )
    columnC -= m_pControl->GetColumn(m_pControl->GetCurrentPos());
    columnA -= m_pControl->GetColumn(m_pControl->GetAnchor());
    if (columnC > 0)
        m_pControl->SetSelectionNCaretVirtualSpace(0,  columnC);
    if (columnA > 0)
        m_pControl->SetSelectionNAnchorVirtualSpace(0, columnA);

    if (!m_IsOK)
        return SaveAs();

    m_pControl->BeginUndoAction();
    NotifyPlugins(cbEVT_EDITOR_BEFORE_SAVE);
    m_pControl->EndUndoAction();

    if ( !cbSaveToFile(m_Filename, m_pControl->GetText(), GetEncoding(), GetUseBom()) )
    {
        wxString msg;
        msg.Printf(_("File %s could not be saved..."), GetFilename().c_str());
        cbMessageBox(msg, _("Error saving file"), wxICON_ERROR);
        return false; // failed; file is read-only?
    }

    wxFileName fname(m_Filename);
    m_LastModified = fname.GetModificationTime();

    m_IsOK = true;

    m_pControl->SetSavePoint();
    SetModified(false);

    NotifyPlugins(cbEVT_EDITOR_SAVE);
    return true;
} // end of Save

bool cbEditor::SaveAs()
{
    wxFileName fname;
    fname.Assign(m_Filename);
    ConfigManager* mgr = Manager::Get()->GetConfigManager(_T("app"));
    int StoredIndex = 0;
    wxString Filters = FileFilters::GetFilterString();
    wxString Path = fname.GetPath();
    wxString Extension = fname.GetExt();
    wxString Filter;
    if (!Extension.IsEmpty())
    {    // use the current extension as the filter
        // Select filter belonging to this file type:
        Extension.Prepend(_T("."));
        Filter = FileFilters::GetFilterString(Extension);
    }
    else if (mgr)
    {
        // File type is unknown. Select the last used filter:
        Filter = mgr->Read(_T("/file_dialogs/save_file_as/filter"), _T("C/C++ files"));
    }
    if (!Filter.IsEmpty())
    {
        // We found a filter, look up its index:
        int sep = Filter.find(_T("|"));
        if (sep != wxNOT_FOUND)
            Filter.Truncate(sep);
        if (!Filter.IsEmpty())
            FileFilters::GetFilterIndexFromName(Filters, Filter, StoredIndex);
    }
    if (mgr && Path.IsEmpty())
        Path = mgr->Read(_T("/file_dialogs/save_file_as/directory"), Path);

    wxFileDialog dlg(Manager::Get()->GetAppWindow(),
                                         _("Save file"),
                                         Path,
                                         fname.GetFullName(),
                                         Filters,
                                         wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    dlg.SetFilterIndex(StoredIndex);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() != wxID_OK)
    {  // cancelled out
        return false;
    }
    m_Filename = dlg.GetPath();
    Manager::Get()->GetLogManager()->Log(m_Filename);
    fname.Assign(m_Filename);
    m_Shortname = fname.GetFullName();
    SetEditorTitle(m_Shortname);
    // invalidate m_pProjectFile, because if kept, it would point to the ProjectFile with old name and
    // cause ProjectManager::RemoveFileFromProject called via context menu to crash
    SetProjectFile(nullptr);
    //Manager::Get()->GetLogManager()->Log(mltDevDebug, "Filename=%s\nShort=%s", m_Filename.c_str(), m_Shortname.c_str());
    m_IsOK = true;
    SetLanguage( HL_AUTO );
    SetModified(true);
    SetEditorStyleAfterFileOpen();
    OverrideUseTabsPerLanguage(m_pControl);
    OverrideUseTabsPerLanguage(m_pControl2);
    // store the last used filter and directory
    if (mgr)
    {
        int Index = dlg.GetFilterIndex();
        Filter.Empty();
        if (FileFilters::GetFilterNameFromIndex(Filters, Index, Filter))
            mgr->Write(_T("/file_dialogs/save_file_as/filter"), Filter);
        wxString Test = dlg.GetDirectory();
        mgr->Write(_T("/file_dialogs/save_file_as/directory"), dlg.GetDirectory());
    }
    return Save();
} // end of SaveAs

bool cbEditor::SaveFoldState()
{
    bool bRet = false;
    if ((m_foldBackup = CreateEditor()))
    {
        ApplyStyles(m_foldBackup);
        m_foldBackup->SetText(m_pControl->GetText());
        int count = m_pControl->GetLineCount();
        for (int i = 0; i < count; ++i)
            m_foldBackup->SetFoldLevel(i,m_pControl->GetFoldLevel(i));
        bRet = true;
    }
    return bRet;
} // end of SaveFoldState

bool cbEditor::FixFoldState()
{
    bool bRet = false;
    if (m_foldBackup)
    {
        int backupLength = m_foldBackup->GetLineCount();
        int realLength = m_pControl->GetLineCount();
        if (backupLength == realLength) // It is supposed to be the same, but you never know :)
        {
            ConfigManager* mgr = Manager::Get()->GetConfigManager(_T("editor"));
            if (mgr->ReadBool(_T("/folding/show_folds"), true)) // Only fix the folds if the folds are enabled
            {
                int count = m_pControl->GetLineCount();
                for (int i = 0; i < count; ++i)
                {
                    int oldFoldLevel = m_foldBackup->GetFoldLevel(i);
                    int newFoldLevel = m_pControl->GetFoldLevel(i);
                    if (oldFoldLevel != newFoldLevel)
                    {
                        if (m_pControl->GetLineVisible(i) == true)
                            m_pControl->SetFoldExpanded(i, true);
                        else
                        {
                            int parent = m_foldBackup->GetFoldParent(i);
                            while(parent != -1)
                            {
                                m_pControl->ToggleFold(parent);
                                parent = m_foldBackup->GetFoldParent(parent);
                            }
                            m_pControl->ShowLines(i, i);
                            parent = m_foldBackup->GetFoldParent(i);
                            while(parent != -1)
                            {
                                m_pControl->ToggleFold(parent);
                                parent = m_foldBackup->GetFoldParent(parent);
                            }
                        }
                    }
                }
            }
            bRet = true;
        }
        m_foldBackup->Destroy();
        m_foldBackup = nullptr;
    }
    return bRet;
} // end of FixFoldState

void cbEditor::AutoComplete()
{
    Manager::Get()->GetLogManager()->Log(_T("cbEditor::AutoComplete() is obsolete.\nUse AutoComplete(cbEditor &ed) from the Abbreviations plugin instead."));
}

void cbEditor::DoFoldAll(int fold)
{
    cbAssert(m_pControl);
    if (m_SplitType != stNoSplit)
        cbAssert(m_pControl2);
    cbStyledTextCtrl* ctrl = GetControl();
    ctrl->Colourise(0, -1); // the *most* important part!
    int count = ctrl->GetLineCount();
    for (int i = 0; i <= count; ++i)
        DoFoldLine(i, fold);
}

void cbEditor::DoFoldBlockFromLine(int line, int fold)
{
    cbAssert(m_pControl);
    if (m_SplitType != stNoSplit)
        cbAssert(m_pControl2);
    cbStyledTextCtrl* ctrl = GetControl();
    ctrl->Colourise(0, -1); // the *most* important part!
    int i, parent, maxLine, level, UnfoldUpto = line;

    parent = ctrl->GetFoldParent(line);
    level = ctrl->GetFoldLevel(parent);
    /* The following code will check if the child is hidden
    *  under parent before unfolding it
    */
    if (fold == 0)
    {
        do
        {
            if (!ctrl->GetFoldExpanded(parent))
                UnfoldUpto = parent;
            if (wxSCI_FOLDLEVELBASE == (level & wxSCI_FOLDLEVELNUMBERMASK))
                break;
            parent = ctrl->GetFoldParent(parent);
            level = ctrl->GetFoldLevel(parent);
        }
        while (parent != -1);
    }

    maxLine = ctrl->GetLastChild(line, -1);

    for (i = UnfoldUpto; i <= maxLine; ++i)
        DoFoldLine(i, fold);
}

bool cbEditor::DoFoldLine(int line, int fold)
{
    cbAssert(m_pControl);
    if (m_SplitType != stNoSplit)
        cbAssert(m_pControl2);
    cbStyledTextCtrl* ctrl = GetControl();
    int level = ctrl->GetFoldLevel(line);

    // The fold parameter is the type of folding action requested
    // 0 = Unfold; 1 = Fold; 2 = Toggle folding.

    // Check if the line is a header (fold point).
    if (level & wxSCI_FOLDLEVELHEADERFLAG)
    {
        bool IsExpanded = ctrl->GetFoldExpanded(line);

        // If a fold/unfold request is issued when the block is already
        // folded/unfolded, ignore the request.
        if (fold == 0 &&  IsExpanded) return true;
        if (fold == 1 && !IsExpanded) return true;

        // Apply the folding level limit only if the current block will be
        // folded (that means it's currently expanded), folding level limiter
        // must be enabled of course. Unfolding will not be affected.
        if (m_pData->mFoldingLimit && IsExpanded)
        {
            if ((level & wxSCI_FOLDLEVELNUMBERMASK) > (wxSCI_FOLDLEVELBASE + m_pData->mFoldingLimitLevel-1))
                return false;
        }

        ctrl->ToggleFold(line);
        return true;
    }
    return false;
}

void cbEditor::FoldAll()
{
    DoFoldAll(1);
}

void cbEditor::UnfoldAll()
{
    DoFoldAll(0);
}

void cbEditor::ToggleAllFolds()
{
    DoFoldAll(2);
}

void cbEditor::SetFoldingIndicator(int id)
{
    wxColor f(0xff, 0xff, 0xff); // foreground colour
    wxColor b(0x80, 0x80, 0x80); // background colour
    // Arrow
    if (id == 0)
    {
        SetMarkerStyle(wxSCI_MARKNUM_FOLDEROPEN,    wxSCI_MARK_ARROWDOWN,  f, b);
        SetMarkerStyle(wxSCI_MARKNUM_FOLDER,        wxSCI_MARK_ARROW,      f, b);
        SetMarkerStyle(wxSCI_MARKNUM_FOLDERSUB,     wxSCI_MARK_BACKGROUND, f, b);
        SetMarkerStyle(wxSCI_MARKNUM_FOLDERTAIL,    wxSCI_MARK_BACKGROUND, f, b);
        SetMarkerStyle(wxSCI_MARKNUM_FOLDEREND,     wxSCI_MARK_ARROW,      f, b);
        SetMarkerStyle(wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_ARROWDOWN,  f, b);
        SetMarkerStyle(wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_BACKGROUND, f, b);
    }
    // Circle
    else if (id == 1)
    {
        SetMarkerStyle(wxSCI_MARKNUM_FOLDEROPEN,    wxSCI_MARK_CIRCLEMINUS,          f, b);
        SetMarkerStyle(wxSCI_MARKNUM_FOLDER,        wxSCI_MARK_CIRCLEPLUS,           f, b);
        SetMarkerStyle(wxSCI_MARKNUM_FOLDERSUB,     wxSCI_MARK_VLINE,                f, b);
        SetMarkerStyle(wxSCI_MARKNUM_FOLDERTAIL,    wxSCI_MARK_LCORNERCURVE,         f, b);
        SetMarkerStyle(wxSCI_MARKNUM_FOLDEREND,     wxSCI_MARK_CIRCLEPLUSCONNECTED,  f, b);
        SetMarkerStyle(wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_CIRCLEMINUSCONNECTED, f, b);
        SetMarkerStyle(wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_TCORNER,              f, b);
    }
    // Square
    else if (id == 2)
    {
        SetMarkerStyle(wxSCI_MARKNUM_FOLDEROPEN,    wxSCI_MARK_BOXMINUS,          f, b);
        SetMarkerStyle(wxSCI_MARKNUM_FOLDER,        wxSCI_MARK_BOXPLUS,           f, b);
        SetMarkerStyle(wxSCI_MARKNUM_FOLDERSUB,     wxSCI_MARK_VLINE,             f, b);
        SetMarkerStyle(wxSCI_MARKNUM_FOLDERTAIL,    wxSCI_MARK_LCORNER,           f, b);
        SetMarkerStyle(wxSCI_MARKNUM_FOLDEREND,     wxSCI_MARK_BOXPLUSCONNECTED,  f, b);
        SetMarkerStyle(wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_BOXMINUSCONNECTED, f, b);
        SetMarkerStyle(wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_TCORNER,           f, b);
    }
    // Simple
    else if (id == 3)
    {
        SetMarkerStyle(wxSCI_MARKNUM_FOLDEROPEN,    wxSCI_MARK_MINUS,      f, b);
        SetMarkerStyle(wxSCI_MARKNUM_FOLDER,        wxSCI_MARK_PLUS,       f, b);
        SetMarkerStyle(wxSCI_MARKNUM_FOLDERSUB,     wxSCI_MARK_BACKGROUND, f, b);
        SetMarkerStyle(wxSCI_MARKNUM_FOLDERTAIL,    wxSCI_MARK_BACKGROUND, f, b);
        SetMarkerStyle(wxSCI_MARKNUM_FOLDEREND,     wxSCI_MARK_PLUS,       f, b);
        SetMarkerStyle(wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_MINUS,      f, b);
        SetMarkerStyle(wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_BACKGROUND, f, b);
    }
}

void cbEditor::FoldBlockFromLine(int line)
{
    if (line == -1)
        line = GetControl()->GetCurrentLine();
    DoFoldBlockFromLine(line, 1);
}

void cbEditor::UnfoldBlockFromLine(int line)
{
    if (line == -1)
        line = GetControl()->GetCurrentLine();
    DoFoldBlockFromLine(line, 0);
}

void cbEditor::ToggleFoldBlockFromLine(int line)
{
    if (line == -1)
        line = GetControl()->GetCurrentLine();
    DoFoldBlockFromLine(line, 2);
}

void cbEditor::GotoLine(int line, bool centerOnScreen)
{
    cbStyledTextCtrl* control = GetControl();

    // Make sure the line is not folded. This is done before moving to that
    // line because folding may change the lines layout.
    control->EnsureVisible(line);

    // If the line or the following is a fold point it will be unfolded, in this way
    // when the line is a function declaration (or only contains the opening brace of it [yes, that happens sometimes] )
    // the body is shown.
    DoFoldLine(line,0);
    DoFoldLine(line+1,0);

    if (centerOnScreen)
    {
        int linesOnScreen    = control->LinesOnScreen() >> 1;
        int firstVisibleLine = control->GetFirstVisibleLine();
        if (   (line <  firstVisibleLine)
            || (line > (firstVisibleLine + 2*linesOnScreen)) )
        {
            control->GotoLine(line - linesOnScreen);
            control->GotoLine(line + linesOnScreen);
        }
    }
    control->GotoLine(line);
}

bool cbEditor::GotoTokenPosition(int line, const wxString& tokenName)
{
    cbStyledTextCtrl* control = GetControl();
    if (line > control->GetLineCount())
        return false;

    GotoLine(line, true); // center function on screen
    SetFocus();           // ...and set focus to this editor

    // Now highlight the token
    const int startPos = control->GetCurrentPos();
    const int endPos   = startPos + control->LineLength(line);
    if (endPos <= startPos)
        return false;

    int tokenPos = control->FindText(startPos, endPos, tokenName,
                                     wxSCI_FIND_WHOLEWORD | wxSCI_FIND_MATCHCASE, nullptr);
    if (tokenPos != wxSCI_INVALID_POSITION)
        control->SetSelectionInt(tokenPos, tokenPos + tokenName.Len());
    else
        control->GotoPos(startPos); // fall back, point the cursor to it

    return true;
}

void cbEditor::BreakpointMarkerToggle(int line)
{
    int marker = m_pControl->MarkerGet(line);
    if      (marker & (1 << BREAKPOINT_MARKER))
        m_pControl->MarkerDelete(line, BREAKPOINT_MARKER);
    else if (marker & (1 << BREAKPOINT_DISABLED_MARKER))
        m_pControl->MarkerDelete(line, BREAKPOINT_DISABLED_MARKER);
    else
        m_pControl->MarkerAdd(line, BREAKPOINT_MARKER);
}

bool cbEditor::AddBreakpoint(int line, bool notifyDebugger)
{
    if (HasBreakpoint(line))
        return false;

    if (line == -1)
        line = GetControl()->GetCurrentLine();

    if (!notifyDebugger)
    {
        BreakpointMarkerToggle(line);
        return false;
    }

    DebuggerManager *dbgManager = Manager::Get()->GetDebuggerManager();
    if (dbgManager->GetBreakpointDialog()->AddBreakpoint(dbgManager->GetActiveDebugger(), m_Filename, line + 1))
    {
        BreakpointMarkerToggle(line);
        return true;
    }
    return false;
}

bool cbEditor::RemoveBreakpoint(int line, bool notifyDebugger)
{
    if (!HasBreakpoint(line))
        return false;

    if (line == -1)
        line = GetControl()->GetCurrentLine();

    if (!notifyDebugger)
    {
        BreakpointMarkerToggle(line);
        return false;
    }

    DebuggerManager *dbgManager = Manager::Get()->GetDebuggerManager();
    if (dbgManager->GetBreakpointDialog()->RemoveBreakpoint(dbgManager->GetActiveDebugger(), m_Filename, line + 1))
    {
        BreakpointMarkerToggle(line);
        return true;
    }
    return false;
}

void cbEditor::ToggleBreakpoint(int line, bool notifyDebugger)
{
    if (line == -1)
        line = GetControl()->GetCurrentLine();
    if (!notifyDebugger)
    {
        BreakpointMarkerToggle(line);
        return;
    }

    DebuggerManager *dbgManager = Manager::Get()->GetDebuggerManager();
    cbBreakpointsDlg *dialog = dbgManager->GetBreakpointDialog();
    cbDebuggerPlugin *plugin = dbgManager->GetActiveDebugger();
    if (!plugin || !plugin->SupportsFeature(cbDebuggerFeature::Breakpoints))
        return;

    bool toggle = false;
    if (HasBreakpoint(line))
    {
        if (dialog->RemoveBreakpoint(plugin, m_Filename, line + 1))
            toggle = true;
    }
    else
    {
        if (dialog->AddBreakpoint(plugin, m_Filename, line + 1))
            toggle = true;
    }

    if (toggle)
    {
        BreakpointMarkerToggle(line);
        dialog->Reload();
    }
}

bool cbEditor::HasBreakpoint(int line) const
{
    if (line == -1)
        line = GetControl()->GetCurrentLine();
    return LineHasMarker(BREAKPOINT_MARKER, line) || LineHasMarker(BREAKPOINT_DISABLED_MARKER, line);
}

void cbEditor::GotoNextBreakpoint()
{
    MarkerNext(BREAKPOINT_MARKER);
}

void cbEditor::GotoPreviousBreakpoint()
{
    MarkerPrevious(BREAKPOINT_MARKER);
}

void cbEditor::ToggleBookmark(int line)
{
    MarkerToggle(BOOKMARK_MARKER, line);
}

void cbEditor::RefreshBreakpointMarkers()
{
    // First remove all breakpoint markers, then add the markers for the active debugger
    cbStyledTextCtrl *c = GetControl();
    int line = 0;
    while ((line = c->MarkerNext(line, (1 << BREAKPOINT_MARKER))) != -1)
        MarkerToggle(BREAKPOINT_MARKER, line);

    line = 0;
    while ((line = c->MarkerNext(line, (1 << BREAKPOINT_DISABLED_MARKER))) != -1)
        MarkerToggle(BREAKPOINT_DISABLED_MARKER, line);

    line = 0;
    while ((line = c->MarkerNext(line, (1 << BREAKPOINT_OTHER_MARKER))) != -1)
        MarkerToggle(BREAKPOINT_OTHER_MARKER, line);

    const DebuggerManager::RegisteredPlugins &plugins = Manager::Get()->GetDebuggerManager()->GetAllDebuggers();
    for (DebuggerManager::RegisteredPlugins::const_iterator it = plugins.begin(); it != plugins.end(); ++it)
    {
        const cbDebuggerPlugin *debugger = it->first;
        if (debugger == Manager::Get()->GetDebuggerManager()->GetActiveDebugger())
        {
            for (int ii = 0; ii < debugger->GetBreakpointsCount(); ++ii)
            {
                cb::shared_ptr<const cbBreakpoint> bp = debugger->GetBreakpoint(ii);
                if (bp->GetLocation() == GetFilename())
                {
                    if (bp->IsEnabled())
                        MarkerToggle(BREAKPOINT_MARKER,          bp->GetLine() - 1);
                    else
                        MarkerToggle(BREAKPOINT_DISABLED_MARKER, bp->GetLine() - 1);
                }
            }
        }
        else
        {
            // all breakpoints for the non active debugger use the other breakpoint marker
            for (int ii = 0; ii < debugger->GetBreakpointsCount(); ++ii)
            {
                cb::shared_ptr<const cbBreakpoint> bp = debugger->GetBreakpoint(ii);
                if (bp->GetLocation() == GetFilename())
                    MarkerToggle(BREAKPOINT_OTHER_MARKER, bp->GetLine() - 1);
            }
        }
    }
}

bool cbEditor::HasBookmark(int line) const
{
    return LineHasMarker(BOOKMARK_MARKER, line);
}

void cbEditor::GotoNextBookmark()
{
    MarkerNext(BOOKMARK_MARKER);
}

void cbEditor::GotoPreviousBookmark()
{
    MarkerPrevious(BOOKMARK_MARKER);
}

void cbEditor::ClearAllBookmarks()
{
    cbStyledTextCtrl* control = GetControl();
    control->MarkerDeleteAll(BOOKMARK_MARKER);
}

void cbEditor::SetDebugLine(int line)
{
    MarkLine(DEBUG_MARKER, line);
    if (GetControl()->GetCaretLineVisible())
        MarkLine(DEBUG_MARKER_HIGHLIGHT, line);
    m_pData->m_LastDebugLine = line;
}

void cbEditor::SetErrorLine(int line)
{
    MarkLine(ERROR_MARKER, line);
}

void cbEditor::Undo()
{
    cbAssert(GetControl());
    GetControl()->Undo();
}

void cbEditor::Redo()
{
    cbAssert(GetControl());
    GetControl()->Redo();
}

void cbEditor::ClearHistory()
{
    cbAssert(GetControl());
    GetControl()->EmptyUndoBuffer(Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/margin/use_changebar"), true));
}

void cbEditor::GotoNextChanged()
{
    cbAssert(GetControl());
    cbStyledTextCtrl* p_Control = GetControl();
    int fromLine = p_Control->LineFromPosition(p_Control->GetCurrentPos());
    int toLine = p_Control->GetLineCount() - 1;
    if (fromLine == toLine)
        fromLine = 0;
    else
        fromLine++;

    int newLine = p_Control->FindChangedLine(fromLine, toLine);
    if (newLine != wxSCI_INVALID_POSITION)
    {
        p_Control->GotoLine(newLine);
        p_Control->MakeNearbyLinesVisible(p_Control->GetCurrentLine());
    }
}

void cbEditor::GotoPreviousChanged()
{
    cbAssert(GetControl());
    cbStyledTextCtrl* p_Control = GetControl();
    int fromLine = p_Control->LineFromPosition(p_Control->GetCurrentPos());
    int toLine = 0;
    if (fromLine == toLine)
        fromLine = p_Control->GetLineCount() - 1;
    else
        fromLine--;

    int newLine = p_Control->FindChangedLine(fromLine, toLine);
    if (newLine != wxSCI_INVALID_POSITION)
    {
        p_Control->GotoLine(newLine);
        p_Control->MakeNearbyLinesVisible(p_Control->GetCurrentLine());
    }
}

void cbEditor::SetChangeCollection(bool collectChange)
{
    cbAssert(GetControl());
    GetControl()->SetChangeCollection(collectChange);
}

void cbEditor::Cut()
{
    cbAssert(GetControl());
    GetControl()->Cut();
}

void cbEditor::Copy()
{
    cbAssert(GetControl());
    GetControl()->Copy();
}

void cbEditor::Paste()
{
    cbAssert(GetControl());
    GetControl()->Paste();
}

bool cbEditor::CanUndo() const
{
    cbAssert(GetControl());
    return !IsReadOnly() && GetControl()->CanUndo();
}

bool cbEditor::CanRedo() const
{
    cbAssert(GetControl());
    return !IsReadOnly() && GetControl()->CanRedo();
}

bool cbEditor::HasSelection() const
{
    cbAssert(GetControl());
    cbStyledTextCtrl* control = GetControl();
    return control->GetSelectionStart() != control->GetSelectionEnd();
}

bool cbEditor::CanPaste() const
{
    cbAssert(GetControl());
    if (platform::gtk)
        return !IsReadOnly();

    return GetControl()->CanPaste() && !IsReadOnly();
}

bool cbEditor::IsReadOnly() const
{
    cbAssert(GetControl());
    return GetControl()->GetReadOnly();
}

void cbEditor::SetReadOnly(bool readonly)
{
    cbAssert(GetControl());
    GetControl()->SetReadOnly(readonly);
}

bool cbEditor::LineHasMarker(int marker, int line) const
{
    if (line == -1)
        line = GetControl()->GetCurrentLine();
    return m_pControl->MarkerGet(line) & (1 << marker);
}

void cbEditor::MarkerToggle(int marker, int line)
{
    if (line == -1)
        line = GetControl()->GetCurrentLine();
    if (LineHasMarker(marker, line))
        GetControl()->MarkerDelete(line, marker);
    else
        GetControl()->MarkerAdd(line, marker);
}

void cbEditor::MarkerNext(int marker)
{
    int line = GetControl()->GetCurrentLine() + 1;
    int newLine = GetControl()->MarkerNext(line, 1 << marker);

    // Start from beginning if at last marker.
    if (newLine == -1)
    {
        line = 0;
        newLine = GetControl()->MarkerNext(line, 1 << marker);

        if (newLine != -1)
            InfoWindow::Display(_("Find bookmark action"), _("Reached the end of the document"), 1000);
    }

    if (newLine != -1)
        GotoLine(newLine);
}

void cbEditor::MarkerPrevious(int marker)
{
    int line = GetControl()->GetCurrentLine() - 1;
    int newLine = GetControl()->MarkerPrevious(line, 1 << marker);

    // Back to last one if at first marker.
    if (newLine == -1)
    {
        line = GetControl()->GetLineCount();
        newLine = GetControl()->MarkerPrevious(line, 1 << marker);

        if (newLine != -1)
            InfoWindow::Display(_("Find bookmark action"), _("Reached the end of the document"), 1000);
    }

    if (newLine != -1)
        GotoLine(newLine);
}

void cbEditor::MarkLine(int marker, int line)
{
    if (line == -1)
        GetControl()->MarkerDeleteAll(marker);
    else
        GetControl()->MarkerAdd(line, marker);
}

void cbEditor::GotoMatchingBrace()
{
    cbStyledTextCtrl* control = GetControl();

    // this works only when the caret is *before* the brace
    int matchingBrace = control->BraceMatch(control->GetCurrentPos());

    // if we haven't found it, we 'll search at pos-1 too
    if (matchingBrace == wxSCI_INVALID_POSITION)
        matchingBrace = control->BraceMatch(control->GetCurrentPos() - 1);
    else
        ++matchingBrace; // to keep the caret on the same side of the brace

    // else look for a matching preprocessor command
    if (matchingBrace == wxSCI_INVALID_POSITION)
    {
        wxRegEx ppIf(wxT("^[ \t]*#[ \t]*if"));
        wxRegEx ppElse(wxT("^[ \t]*#[ \t]*el"));
        wxRegEx ppEnd(wxT("^[ \t]*#[ \t]*endif"));
        wxRegEx pp(wxT("^[ \t]*#[ \t]*[a-z]*")); // generic match to get length
        if (ppIf.Matches(control->GetCurLine()) || ppElse.Matches(control->GetCurLine()))
        {
            int depth = 1; // search forwards
            for (int i = control->GetCurrentLine() + 1; i < control->GetLineCount(); ++i)
            {
                if (control->GetLine(i).Find(wxT('#')) != wxNOT_FOUND) // limit testing due to performance cost
                {
                    if (ppIf.Matches(control->GetLine(i))) // ignore else's, elif's, ...
                        ++depth;
                    else if (ppEnd.Matches(control->GetLine(i)))
                        --depth;
                }
                if (depth == 0)
                {
                    pp.Matches(control->GetLine(i));
                    matchingBrace = control->PositionFromLine(i) + pp.GetMatch(control->GetLine(i)).Length();
                    break;
                }
            }
        }
        else if (ppEnd.Matches(control->GetCurLine()))
        {
            int depth = -1; // search backwards
            for (int i = control->GetCurrentLine() - 1; i >= 0; --i)
            {
                if (control->GetLine(i).Find(wxT('#')) != wxNOT_FOUND) // limit testing due to performance cost
                {
                    if (ppIf.Matches(control->GetLine(i))) // ignore else's, elif's, ...
                        ++depth;
                    else if (ppEnd.Matches(control->GetLine(i)))
                        --depth;
                }
                if (depth == 0)
                {
                    pp.Matches(control->GetLine(i));
                    matchingBrace = control->PositionFromLine(i) + pp.GetMatch(control->GetLine(i)).Length();
                    break;
                }
            }
        }
    }

    // now, we either found it or not
    if (matchingBrace != wxSCI_INVALID_POSITION)
    {
        // move to the actual position
        control->GotoPos(matchingBrace);
        control->ChooseCaretX();
        // make nearby lines visible
        control->MakeNearbyLinesVisible(control->GetCurrentLine());
    }
}

void cbEditor::HighlightBraces()
{
    cbStyledTextCtrl* control = GetControl();

    ////// BRACES HIGHLIGHTING ///////
    int currPos = control->GetCurrentPos();
    int newPos = control->BraceMatch(currPos);
    if (newPos == wxSCI_INVALID_POSITION)
    {
        if (currPos > 0)
            currPos--;
        newPos = control->BraceMatch(currPos);
    }
    wxChar ch = control->GetCharAt(currPos);
    if (ch == _T('{') || ch == _T('[') || ch == _T('(') ||
        ch == _T('}') || ch == _T(']') || ch == _T(')'))
    {
        if (newPos != wxSCI_INVALID_POSITION)
        {
            control->BraceHighlight(currPos, newPos);
            const int currColum = control->GetColumn(currPos);
            const int newColum = control->GetColumn(newPos);
            control->SetHighlightGuide((currColum < newColum) ? currColum :newColum);
        }
        else
        {
            control->BraceBadLight(currPos);
        }
    }
    else
    {
        control->BraceHighlight(-1, -1);
    }
}

int cbEditor::GetLineIndentInSpaces(int line) const
{
    cbStyledTextCtrl* control = GetControl();
    int currLine = (line == -1)
                    ? control->LineFromPosition(control->GetCurrentPos())
                    : line;
    wxString text = control->GetLine(currLine);
    unsigned int len = text.Length();
    int spaceCount = 0;
    for (unsigned int i = 0; i < len; ++i)
    {
        if (text[i] == _T(' '))
        {
            ++spaceCount;
        }
        else if (text[i] == _T('\t'))
        {
            spaceCount += control->GetTabWidth();
        }
        else
        {
            break;
        }
    }
    return spaceCount;
}

wxString cbEditor::GetLineIndentString(int line) const
{
    return cbEditorInternalData::GetLineIndentString(line, GetControl());
}

// Creates a submenu for a Context Menu based on the submenu's specific Id
wxMenu* cbEditor::CreateContextSubMenu(long id)
{
    cbStyledTextCtrl* control = GetControl();
    wxMenu* menu = nullptr;
    if (id == idInsert)
    {
        menu = new wxMenu;
        menu->Append(idEmptyMenu, _("Empty"));
        menu->Enable(idEmptyMenu, false);
    }
    else if (id == idEdit)
    {
        menu = new wxMenu;
        menu->Append(idUndo, _("Undo"));
        menu->Append(idRedo, _("Redo"));
        menu->Append(idClearHistory, _("Clear changes history"));
        menu->AppendSeparator();
        menu->Append(idCut, _("Cut"));
        menu->Append(idCopy, _("Copy"));
        menu->Append(idPaste, _("Paste"));
        menu->Append(idDelete, _("Delete"));
        menu->AppendSeparator();
        menu->Append(idUpperCase, _("UPPERCASE"));
        menu->Append(idLowerCase, _("lowercase"));
        menu->AppendSeparator();
        menu->Append(idSelectAll, _("Select all"));

        bool hasSel = control->GetSelectionEnd() - control->GetSelectionStart() != 0;

        menu->Enable(idUndo, control->CanUndo());
        menu->Enable(idRedo, control->CanRedo());
        menu->Enable(idClearHistory, control->CanUndo() || control->CanRedo());
        menu->Enable(idCut, !control->GetReadOnly() && hasSel);
        menu->Enable(idCopy, hasSel);

        if (platform::gtk) // a wxGTK bug causes the triggering of unexpected events
            menu->Enable(idPaste, !control->GetReadOnly());
        else
            menu->Enable(idPaste, !control->GetReadOnly() && control->CanPaste());

        menu->Enable(idDelete, !control->GetReadOnly() && hasSel);
        menu->Enable(idUpperCase, !control->GetReadOnly() && hasSel);
        menu->Enable(idLowerCase, !control->GetReadOnly() && hasSel);
    }
    else if (id == idBookmarks)
    {
        menu = new wxMenu;
        menu->Append(idBookmarksToggle, _("Toggle bookmark"));
        menu->Append(idBookmarksPrevious, _("Goto previous bookmark"));
        menu->Append(idBookmarksNext, _("Goto next bookmark"));
        menu->Append(idBookmarksClearAll, _("Clear all bookmarks"));
    }
    else if (id == idFolding)
    {
        menu = new wxMenu;
        menu->Append(idFoldingFoldAll, _("Fold all"));
        menu->Append(idFoldingUnfoldAll, _("Unfold all"));
        menu->Append(idFoldingToggleAll, _("Toggle all"));
        menu->AppendSeparator();
        menu->Append(idFoldingFoldCurrent, _("Fold current block"));
        menu->Append(idFoldingUnfoldCurrent, _("Unfold current block"));
        menu->Append(idFoldingToggleCurrent, _("Toggle current block"));
    }
    else
        menu = EditorBase::CreateContextSubMenu(id);

    return menu;
}

// Adds menu items to context menu (both before and after loading plugins' items)
void cbEditor::AddToContextMenu(wxMenu* popup,ModuleType type,bool pluginsdone)
{
    bool noeditor = (type != mtEditorManager);
    if (!pluginsdone)
    {
        wxMenu *bookmarks = nullptr, *folding = nullptr, *editsubmenu = nullptr, *insert = nullptr;
        if (!noeditor)
        {
            insert = CreateContextSubMenu(idInsert);
            editsubmenu = CreateContextSubMenu(idEdit);
            bookmarks = CreateContextSubMenu(idBookmarks);
            if (Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/folding/show_folds"), false))
                folding = CreateContextSubMenu(idFolding);
        }
        if (insert)
        {
            popup->Append(idInsert, _("Insert"), insert);
            popup->AppendSeparator();
        }
        popup->Append(idSwapHeaderSource, _("Swap header/source"));
        popup->Append(idOpenContainingFolder, _("Open containing folder"));
        if (!noeditor)
            popup->AppendSeparator();

        if (editsubmenu)
            popup->Append(idEdit, _("Edit"), editsubmenu);
        if (bookmarks)
            popup->Append(idBookmarks, _("Bookmarks"), bookmarks);
        if (folding)
            popup->Append(idFolding, _("Folding"), folding);
    }
    else
    {
        if (!noeditor && !m_pData->GetUrl().IsEmpty())
        {
            popup->InsertSeparator(0);
            popup->Insert(0, idOpenUrl, _("Open link in browser"));
        }

        wxMenu* splitMenu = new wxMenu;
        splitMenu->Append(idSplitHorz, _("Horizontally (top-bottom)"));
        splitMenu->Append(idSplitVert, _("Vertically (left-right)"));
        splitMenu->AppendSeparator();
        splitMenu->Append(idUnsplit, _("Unsplit"));
        // enable/disable entries accordingly
        bool isSplitHorz = m_pSplitter && m_pSplitter->GetSplitMode() == wxSPLIT_HORIZONTAL;
        bool isSplitVert = m_pSplitter && m_pSplitter->GetSplitMode() == wxSPLIT_VERTICAL;
        splitMenu->Enable(idSplitHorz, !isSplitHorz);
        splitMenu->Enable(idSplitVert, !isSplitVert);
        splitMenu->Enable(idUnsplit, isSplitHorz || isSplitVert);
        popup->Append(idSplit, _("Split view"), splitMenu);

        popup->Append(idProperties, _("Properties..."));

        if (Manager::Get()->GetProjectManager()->GetActiveProject()) // project must be open
        {
            bool isAddRemoveEnabled = true;
            isAddRemoveEnabled = Manager::Get()->GetProjectManager()->GetActiveProject()->GetCurrentlyCompilingTarget() == nullptr;
            popup->AppendSeparator();

            if (m_pProjectFile)
            {
                popup->Append(idRemoveFileFromProject, _("Remove file from project"));
                popup->Enable(idRemoveFileFromProject, isAddRemoveEnabled);
                popup->Append(idShowFileInProject,     _("Show file in the project tree"));
            }
            else
            {
                popup->Append(idAddFileToProject, _("Add file to active project"));
                popup->Enable(idAddFileToProject, isAddRemoveEnabled);
            }
        }
        // remove "Insert/Empty" if more than one entry
        wxMenu* insert = nullptr;
        wxMenuItem* insertitem = popup->FindItem(idInsert);
        if (insertitem)
            insert = insertitem->GetSubMenu();
        if (insert)
        {
            if (insert->GetMenuItemCount() > 1)
                insert->Delete(idEmptyMenu);
        }
    }
}

bool cbEditor::OnBeforeBuildContextMenu(const wxPoint& position, ModuleType type)
{
    bool noeditor = (type != mtEditorManager);
    if (!noeditor && position!=wxDefaultPosition)
    {
        // right mouse click inside the editor

        // because here the focus has not switched yet (i.e. the left control has the focus,
        // but the user right-clicked inside the right control), we find out the active control differently...
        wxPoint clientpos(ScreenToClient(position));
        const int margin = m_pControl->GetMarginWidth(C_LINE_MARGIN) +     // numbers, if present
                           m_pControl->GetMarginWidth(C_MARKER_MARGIN) +   // breakpoints, bookmarks... if present
                           m_pControl->GetMarginWidth(C_FOLDING_MARGIN) +  // folding, if present
                           m_pControl->GetMarginWidth(C_CHANGEBAR_MARGIN); // changebar, if present
        wxRect r = m_pControl->GetRect();

        bool inside1 = r.Contains(clientpos);

        cbStyledTextCtrl* control = !m_pControl2 || inside1 ? m_pControl : m_pControl2;
//        control->SetFocus();

        clientpos = control->ScreenToClient(position);
        if (clientpos.x < margin)
        {
            // keep the line
            int pos = control->PositionFromPoint(clientpos);
            m_pData->m_LastMarginMenuLine = control->LineFromPosition(pos);

            // create special menu
            wxMenu* popup = new wxMenu;

            cbDebuggerPlugin *plugin = Manager::Get()->GetDebuggerManager()->GetActiveDebugger();
            if (plugin && plugin->SupportsFeature(cbDebuggerFeature::Breakpoints))
            {
                bool hasBreak = LineHasMarker(BREAKPOINT_MARKER, m_pData->m_LastMarginMenuLine);
                bool hasBreakDisabled = LineHasMarker(BREAKPOINT_DISABLED_MARKER, m_pData->m_LastMarginMenuLine);

                if (hasBreak || hasBreakDisabled)
                {
                    popup->Append(idBreakpointEdit, _("Edit breakpoint"));
                    popup->Append(idBreakpointRemove, _("Remove breakpoint"));
                    if (hasBreak)
                        popup->Append(idBreakpointDisable, _("Disable breakpoint"));
                    if (hasBreakDisabled)
                        popup->Append(idBreakpointEnable, _("Enable breakpoint"));
                }
                else
                    popup->Append(idBreakpointAdd, _("Add breakpoint"));
                popup->AppendSeparator();
            }

            if (LineHasMarker(BOOKMARK_MARKER, m_pData->m_LastMarginMenuLine))
            {
                popup->Append(idBookmarkRemove, _("Remove bookmark"));
            }
            else
            {
                popup->Append(idBookmarkAdd, _("Add bookmark"));
            }

            popup->Append(idBookmarkRemoveAll, _("Remove all bookmark"));

            // display menu... wxWindows help says not to force the position
            PopupMenu(popup);

            delete popup;
            return false;
        }

        // before the context menu creation, move the caret to where mouse is

        // get caret position and line from mouse cursor
        const int pos = control->PositionFromPoint(control->ScreenToClient(wxGetMousePosition()));

        // this re-enables 1-click "Find declaration of..."
        // but avoids losing selection for cut/copy
        if (control->GetSelectionStart() > pos ||
           control->GetSelectionEnd() < pos)
        {
            control->GotoPos(pos);
        }
    }

    // follow default strategy
    return EditorBase::OnBeforeBuildContextMenu(position, type);
}

void cbEditor::OnAfterBuildContextMenu(cb_unused ModuleType type)
{
    // we don't care
}

void cbEditor::Print(bool selectionOnly, PrintColourMode pcm, bool line_numbers)
{
    cbStyledTextCtrl * control = GetControl();
    if (!control)
        return;

    // Remember same settings, so we can restore them.
    int oldMarginWidth = control->GetMarginWidth(C_LINE_MARGIN);
    int oldMarginType = control->GetMarginType(C_LINE_MARGIN);
    int oldEdgeMode = control->GetEdgeMode();

    // print line numbers?
    control->SetMarginType(C_LINE_MARGIN, wxSCI_MARGIN_NUMBER);
    if (!line_numbers)
    {
        control->SetPrintMagnification(-1);
        control->SetMarginWidth(C_LINE_MARGIN, 0);
    }
    else
    {
        control->SetPrintMagnification(-2);
        control->SetMarginWidth(C_LINE_MARGIN, 1);
    }
    // never print the gutter line
    control->SetEdgeMode(wxSCI_EDGE_NONE);

    switch (pcm)
    {
        case pcmAsIs:
            control->SetPrintColourMode(wxSCI_PRINT_NORMAL);
            break;
        case pcmBlackAndWhite:
            control->SetPrintColourMode(wxSCI_PRINT_BLACKONWHITE);
            break;
        case pcmColourOnWhite:
            control->SetPrintColourMode(wxSCI_PRINT_COLOURONWHITE);
            break;
        case pcmInvertColours:
            control->SetPrintColourMode(wxSCI_PRINT_INVERTLIGHT);
            break;
        default:
            break;
    }
    InitPrinting();
    wxPrintout* printout = new cbEditorPrintout(m_Filename, control, selectionOnly);
    if (!g_printer->Print(this, printout, true))
    {
        if (wxPrinter::GetLastError() == wxPRINTER_ERROR)
        {
            cbMessageBox(_("There was a problem printing.\n"
                            "Perhaps your current printer is not set correctly?"), _("Printing"), wxICON_ERROR);
            DeInitPrinting();
        }
    }
    else
    {
        wxPrintData* ppd = &(g_printer->GetPrintDialogData().GetPrintData());
        Manager::Get()->GetConfigManager(_T("app"))->Write(_T("/printerdialog/paperid"), (int)ppd->GetPaperId());
        Manager::Get()->GetConfigManager(_T("app"))->Write(_T("/printerdialog/paperorientation"), (int)ppd->GetOrientation());
    }
    delete printout;

    // revert line number settings
    control->SetMarginType(C_LINE_MARGIN, oldMarginType);
    control->SetMarginWidth(C_LINE_MARGIN, oldMarginWidth);

    // revert gutter settings
    control->SetEdgeMode(oldEdgeMode);

    // restore line numbers if needed
    m_pData->SetLineNumberColWidth(m_pControl && m_pControl2);
}

// events

void cbEditor::OnContextMenuEntry(wxCommandEvent& event)
{
    cbStyledTextCtrl* control = GetControl();

    // we have a single event handler for all popup menu entries,
    // so that we can add/remove options without the need to recompile
    // the whole project (almost) but more importantly, to
    // *not* break cbEditor's interface for such a trivial task...
    const int id = event.GetId();

    if (id == idUndo)
        control->Undo();
    else if (id == idRedo)
        control->Redo();
    else if (id == idClearHistory)
        control->EmptyUndoBuffer(Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/margin/use_changebar"), true));
    else if (id == idCut)
        control->Cut();
    else if (id == idCopy)
        control->Copy();
    else if (id == idPaste)
        control->Paste();
    else if (id == idDelete)
        control->ReplaceSelection(wxEmptyString);
    else if (id == idUpperCase)
        control->UpperCase();
    else if (id == idLowerCase)
        control->LowerCase();
    else if (id == idSelectAll)
        control->SelectAll();
    else if (id == idSwapHeaderSource)
        Manager::Get()->GetEditorManager()->SwapActiveHeaderSource();
    else if (id == idOpenContainingFolder)
        Manager::Get()->GetEditorManager()->OpenContainingFolder();
    else if (id == idBookmarkAdd)
        control->MarkerAdd(m_pData->m_LastMarginMenuLine, BOOKMARK_MARKER);
    else if (id == idBookmarkRemove)
        control->MarkerDelete(m_pData->m_LastMarginMenuLine, BOOKMARK_MARKER);
    else if (id == idBookmarkRemoveAll)
        control->MarkerDeleteAll(BOOKMARK_MARKER);
    else if (id == idBookmarksToggle)
        MarkerToggle(BOOKMARK_MARKER);
    else if (id == idBookmarksNext)
        MarkerNext(BOOKMARK_MARKER);
    else if (id == idBookmarksPrevious)
        MarkerPrevious(BOOKMARK_MARKER);
    else if (id == idBookmarksClearAll)
        control->MarkerDeleteAll(BOOKMARK_MARKER);
    else if (id == idFoldingFoldAll)
        FoldAll();
    else if (id == idFoldingUnfoldAll)
        UnfoldAll();
    else if (id == idFoldingToggleAll)
        ToggleAllFolds();
    else if (id == idFoldingFoldCurrent)
        FoldBlockFromLine();
    else if (id == idFoldingUnfoldCurrent)
        UnfoldBlockFromLine();
    else if (id == idFoldingToggleCurrent)
        ToggleFoldBlockFromLine();
    else if (id == idOpenUrl)
        wxLaunchDefaultBrowser(m_pData->GetUrl());
    else if (id == idSplitHorz)
        Split(stHorizontal);
    else if (id == idSplitVert)
        Split(stVertical);
    else if (id == idUnsplit)
        Unsplit();
    else if (id == idProperties)
    {
        if (m_pProjectFile)
            m_pProjectFile->ShowOptions(this);
        else
        {
            // active editor not-in-project
            ProjectFileOptionsDlg dlg(this, GetFilename());
            PlaceWindow(&dlg);
            dlg.ShowModal();
        }
    }
    else if (id == idAddFileToProject)
    {
        cbProject *prj = Manager::Get()->GetProjectManager()->GetActiveProject();

        wxArrayInt targets;
        if (Manager::Get()->GetProjectManager()->AddFileToProject(m_Filename, prj, targets) != 0)
        {
            ProjectFile* pf = prj->GetFileByFilename(m_Filename, false);
            SetProjectFile(pf);
            Manager::Get()->GetProjectManager()->GetUI().RebuildTree();
        }
    }
    else if (id == idRemoveFileFromProject)
    {
        if (m_pProjectFile)
        {
            cbProject *prj = m_pProjectFile->GetParentProject();
            Manager::Get()->GetProjectManager()->RemoveFileFromProject(m_pProjectFile, prj);
            Manager::Get()->GetProjectManager()->GetUI().RebuildTree();
        }
    }
    else if (id == idShowFileInProject)
    {
        cbProjectManagerUI& ui = Manager::Get()->GetProjectManager()->GetUI();
        ui.SwitchToProjectsPage();
        ui.ShowFileInTree(*m_pProjectFile);
    }
    else if (id == idBreakpointAdd)
        AddBreakpoint(m_pData->m_LastMarginMenuLine);
    else if (id == idBreakpointEdit)
    {
        cbBreakpointsDlg *dialog = Manager::Get()->GetDebuggerManager()->GetBreakpointDialog();
        dialog->EditBreakpoint(m_Filename, m_pData->m_LastMarginMenuLine + 1);
    }
    else if (id == idBreakpointRemove)
        RemoveBreakpoint(m_pData->m_LastMarginMenuLine);
    else if (id == idBreakpointEnable)
    {
        cbBreakpointsDlg *dialog = Manager::Get()->GetDebuggerManager()->GetBreakpointDialog();
        dialog->EnableBreakpoint(m_Filename, m_pData->m_LastMarginMenuLine + 1, true);
    }
    else if (id == idBreakpointDisable)
    {
        cbBreakpointsDlg *dialog = Manager::Get()->GetDebuggerManager()->GetBreakpointDialog();
        dialog->EnableBreakpoint(m_Filename, m_pData->m_LastMarginMenuLine + 1, false);
    }
    else
        event.Skip();
    //Manager::Get()->GetLogManager()->DebugLog(_T("Leaving OnContextMenuEntry"));
}

void cbEditor::OnMarginClick(wxScintillaEvent& event)
{
    switch (event.GetMargin())
    {
        case C_MARKER_MARGIN: // bookmarks and breakpoints margin
        {
            int lineYpix = event.GetPosition();
            int line = GetControl()->LineFromPosition(lineYpix);

            ToggleBreakpoint(line);
            break;
        }
        case C_FOLDING_MARGIN: // folding margin
        {
            int lineYpix = event.GetPosition();
            int line = GetControl()->LineFromPosition(lineYpix);

            GetControl()->ToggleFold(line);
            break;
        }
        default:
            break;
    }
    OnScintillaEvent(event);
}

void cbEditor::OnEditorUpdateUI(wxScintillaEvent& event)
{
    EditorManager* edMgr = Manager::Get()->GetEditorManager();
    if (edMgr->GetActiveEditor() == this)
    {
        NotifyPlugins(cbEVT_EDITOR_UPDATE_UI);
        HighlightBraces(); // brace highlighting
        if (event.GetUpdated() & wxSCI_UPDATE_SELECTION)
        {
            // emulate ScintillaWX::ClaimSelection()
            cbStyledTextCtrl* stc = GetControl();
            if (stc->GetSelectionStart() != stc->GetSelectionEnd())
                edMgr->SetSelectionClipboard(stc->GetSelectedText());
        }
    }
    OnScintillaEvent(event);
}

void cbEditor::OnEditorChange(wxScintillaEvent& event)
{
    SetModified(m_pControl->GetModify());
    OnScintillaEvent(event);
}

void cbEditor::OnEditorCharAdded(wxScintillaEvent& event)
{
    // if message manager is auto-hiding, this will close it if not needed open
//    Manager::Get()->GetLogManager()->Close();

    m_autoIndentDone = false;
    OnScintillaEvent(event); // smart indent plugins will be called here
    if (!m_autoIndentDone)
    {
        const wxChar ch = event.GetKey();
        cbStyledTextCtrl* control = GetControl();
        // auto indent
        if ( (ch == _T('\n')) || ( (control->GetEOLMode() == wxSCI_EOL_CR) && (ch == _T('\r')) ) )
        {
            const int pos = control->GetCurrentPos();
            const int currLine = control->LineFromPosition(pos);
            const bool autoIndent = Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/auto_indent"), true);
            if (autoIndent && currLine > 0)
            {
                wxString indent;
                if (control->GetCurLine().Trim().IsEmpty())
                {
                    // copy the indentation of the last non-empty line
                    for (int i = currLine - 1; i >= 0; --i)
                    {
                        const wxString& prevLineStr = control->GetLine(i);
                        if (!(prevLineStr.IsEmpty() || prevLineStr[0] == _T('\n') || prevLineStr[0] == _T('\r')))
                        {
                            indent = GetLineIndentString(i);
                            break;
                        }
                    }
                }
                else
                    indent = GetLineIndentString(currLine - 1);
                if (!indent.IsEmpty())
                {
                    control->BeginUndoAction();

                    control->InsertText(pos, indent);
                    control->GotoPos(pos + indent.Length());
                    control->ChooseCaretX();

                    control->EndUndoAction();
                }
            }
        }

        // selection brace completion
        bool braceCompleted = false;
        if (   Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/selection_brace_completion"), false)
            || control->IsBraceShortcutActive() )
        {
            braceCompleted = control->DoSelectionBraceCompletion(ch);
        }

        // brace completion
        if (  !braceCompleted
            && Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/brace_completion"), true) )
        {
            control->DoBraceCompletion(ch);
        }
    }
}

void cbEditor::AutoIndentDone()
{
    m_autoIndentDone = true;
}

void cbEditor::OnEditorDwellStart(wxScintillaEvent& event)
{
    if ( !wxTheApp->IsActive() )
        return;

    cbStyledTextCtrl* control = GetControl();
    if (!control)
        return;

    wxRect screenRect = control->GetScreenRect();
    wxPoint ptEvent(event.GetX(), event.GetY());
    ptEvent = control->ClientToScreen(ptEvent);
    wxPoint ptScreen = wxGetMousePosition();
    wxPoint ptClient = control->ScreenToClient(ptScreen);

    double distance = sqrt(  (ptScreen.x - ptEvent.x) * (ptScreen.x - ptEvent.x)
                           + (ptScreen.y - ptEvent.y) * (ptScreen.y - ptEvent.y) );
    if (!screenRect.Contains(ptScreen) || distance > 10)
        return;

    int pos = control->PositionFromPoint(ptClient);
    int style = control->GetStyleAt(pos);
    NotifyPlugins(cbEVT_EDITOR_TOOLTIP, style, wxEmptyString, ptClient.x, ptClient.y);
    wxScintillaEvent newEvent(event);
    newEvent.SetX(ptClient.x);
    newEvent.SetY(ptClient.y);
    OnScintillaEvent(event);
}

void cbEditor::OnEditorDwellEnd(wxScintillaEvent& event)
{
    NotifyPlugins(cbEVT_EDITOR_TOOLTIP_CANCEL);
    OnScintillaEvent(event);
}

void cbEditor::OnEditorModified(wxScintillaEvent& event)
{
//    wxString txt = _T("OnEditorModified(): ");
//    int flags = event.GetModificationType();
//    if (flags & wxSCI_MOD_CHANGEMARKER) txt << _T("wxSCI_MOD_CHANGEMARKER, ");
//    if (flags & wxSCI_MOD_INSERTTEXT) txt << _T("wxSCI_MOD_INSERTTEXT, ");
//    if (flags & wxSCI_MOD_DELETETEXT) txt << _T("wxSCI_MOD_DELETETEXT, ");
//    if (flags & wxSCI_MOD_CHANGEFOLD) txt << _T("wxSCI_MOD_CHANGEFOLD, ");
//    if (flags & wxSCI_PERFORMED_USER) txt << _T("wxSCI_PERFORMED_USER, ");
//    if (flags & wxSCI_MOD_BEFOREINSERT) txt << _T("wxSCI_MOD_BEFOREINSERT, ");
//    if (flags & wxSCI_MOD_BEFOREDELETE) txt << _T("wxSCI_MOD_BEFOREDELETE, ");
//    txt << _T("pos=")
//        << wxString::Format(_T("%d"), event.GetPosition())
//        << _T(", line=")
//        << wxString::Format(_T("%d"), event.GetLine())
//        << _T(", linesAdded=")
//        << wxString::Format(_T("%d"), event.GetLinesAdded());
//    Manager::Get()->GetLogManager()->DebugLog(txt);

    // whenever event.GetLinesAdded() != 0, we must re-set breakpoints for lines greater
    // than LineFromPosition(event.GetPosition())
    int linesAdded = event.GetLinesAdded();
    bool isAdd = event.GetModificationType() & wxSCI_MOD_INSERTTEXT;
    bool isDel = event.GetModificationType() & wxSCI_MOD_DELETETEXT;
    if ((isAdd || isDel) && linesAdded != 0)
    {
        // whether to show line-numbers or not is handled in SetLineNumberColWidth() now
        m_pData->SetLineNumberColWidth();

        // NB: I don't think polling for each debugger every time will slow things down enough
        // to worry about unless there are automated tasks that call this routine regularly
        //
        // well, scintilla events happen regularly
        // although we only reach this part of the code only if a line has been added/removed
        // so, yes, it might not be that bad after all
        int startline = m_pControl->LineFromPosition(event.GetPosition());
        if (m_pControl == event.GetEventObject())
        {
            const DebuggerManager::RegisteredPlugins &plugins = Manager::Get()->GetDebuggerManager()->GetAllDebuggers();
            cbDebuggerPlugin *active = Manager::Get()->GetDebuggerManager()->GetActiveDebugger();
            for (DebuggerManager::RegisteredPlugins::const_iterator it = plugins.begin(); it != plugins.end(); ++it)
            {
                if (it->first != active)
                    it->first->EditorLinesAddedOrRemoved(this, startline + 1, linesAdded);
            }
            if (active)
                active->EditorLinesAddedOrRemoved(this, startline + 1, linesAdded);

            cbBreakpointsDlg *dlg = Manager::Get()->GetDebuggerManager()->GetBreakpointDialog();
            if (dlg)
                dlg->Reload();
            RefreshBreakpointMarkers();
        }
    }
    // If we remove the folding-point (the brace or whatever) from a folded block,
    // we have to make the hidden lines visible, otherwise, they
    // will no longer be reachable, until the editor is closed and reopened again
    if (   (event.GetModificationType() & wxSCI_MOD_CHANGEFOLD)
        && (event.GetFoldLevelPrev() & wxSCI_FOLDLEVELHEADERFLAG) )
    {
        cbStyledTextCtrl* control = GetControl();
        int line = event.GetLine();
        if (! control->GetFoldExpanded(line))
        {
            control->SetFoldExpanded(line, true);
            control->ShowLines(line, control->GetLastChild(line, -1));
        }
    }
    OnScintillaEvent(event);
} // end of OnEditorModified

void cbEditor::OnUserListSelection(wxScintillaEvent& event)
{
    OnScintillaEvent(event);
}

void cbEditor::OnClose(cb_unused wxCloseEvent& event)
{
    Manager::Get()->GetEditorManager()->Close(this);
}

void cbEditor::DoIndent()
{
    cbStyledTextCtrl* control = GetControl();
    if (control)
        control->SendMsg(wxSCI_CMD_TAB);
}

void cbEditor::DoUnIndent()
{
    cbStyledTextCtrl* control = GetControl();
    if (control)
        control->SendMsg(wxSCI_CMD_BACKTAB);
}

void cbEditor::OnZoom(wxScintillaEvent& event)
{
    ConfigManager* mgr =  Manager::Get()->GetConfigManager(_T("editor"));

    int zoom = GetControl()->GetZoom();
    Manager::Get()->GetEditorManager()->SetZoom(zoom);
    // if all editors should be zoomed, we call cbAuiNotebooks SetZoom()
    bool both = Manager::Get()->GetConfigManager(_T("editor"))->ReadBool(_T("/zoom_all"));
    if (both)
        Manager::Get()->GetEditorManager()->GetNotebook()->SetZoom(zoom);

    m_pData->SetLineNumberColWidth(both);

    if (mgr->ReadBool(_T("/folding/show_folds"), true))
        m_pData->SetFoldingColWidth(both);

    OnScintillaEvent(event);
}

// used to set zoom for both (if splitted) or just the last active control,
// called from cbAuiNotebook
void cbEditor::SetZoom(int zoom, bool both)
{
    if (both)
    {
        if (m_pControl->GetZoom() != zoom)
            m_pControl->SetZoom(zoom);
        if (m_pControl2 && (m_pControl2->GetZoom() != zoom))
            m_pControl2->SetZoom(zoom);
    }
    else
    {
        if (GetControl()->GetZoom() != zoom)
            GetControl()->SetZoom(zoom);
    }
}

// generic scintilla event handler
void cbEditor::OnScintillaEvent(wxScintillaEvent& event)
{
//  wxString txt;
//    wxEventType type = event.GetEventType();
//  if (type == wxEVT_SCI_CHANGE) txt << _T("wxEVT_SCI_CHANGE");
//  else if (type == wxEVT_SCI_STYLENEEDED) txt << _T("wxEVT_SCI_STYLENEEDED");
//  else if (type == wxEVT_SCI_CHARADDED) txt << _T("wxEVT_SCI_CHARADDED");
//  else if (type == wxEVT_SCI_SAVEPOINTREACHED) txt << _T("wxEVT_SCI_SAVEPOINTREACHED");
//  else if (type == wxEVT_SCI_SAVEPOINTLEFT) txt << _T("wxEVT_SCI_SAVEPOINTLEFT");
//  else if (type == wxEVT_SCI_ROMODIFYATTEMPT) txt << _T("wxEVT_SCI_ROMODIFYATTEMPT");
//  else if (type == wxEVT_SCI_KEY) txt << _T("wxEVT_SCI_KEY");
//  else if (type == wxEVT_SCI_DOUBLECLICK) txt << _T("wxEVT_SCI_DOUBLECLICK");
//  else if (type == wxEVT_SCI_UPDATEUI) txt << _T("wxEVT_SCI_UPDATEUI");
//  else if (type == wxEVT_SCI_MODIFIED) txt << _T("wxEVT_SCI_MODIFIED");
//  else if (type == wxEVT_SCI_MACRORECORD) txt << _T("wxEVT_SCI_MACRORECORD");
//  else if (type == wxEVT_SCI_MARGINCLICK) txt << _T("wxEVT_SCI_MARGINCLICK");
//  else if (type == wxEVT_SCI_NEEDSHOWN) txt << _T("wxEVT_SCI_NEEDSHOWN");
//  else if (type == wxEVT_SCI_PAINTED) txt << _T("wxEVT_SCI_PAINTED");
//  else if (type == wxEVT_SCI_USERLISTSELECTION) txt << _T("wxEVT_SCI_USERLISTSELECTION");
//  else if (type == wxEVT_SCI_URIDROPPED) txt << _T("wxEVT_SCI_URIDROPPED");
//  else if (type == wxEVT_SCI_DWELLSTART) txt << _T("wxEVT_SCI_DWELLSTART");
//  else if (type == wxEVT_SCI_DWELLEND) txt << _T("wxEVT_SCI_DWELLEND");
//  else if (type == wxEVT_SCI_START_DRAG) txt << _T("wxEVT_SCI_START_DRAG");
//  else if (type == wxEVT_SCI_DRAG_OVER) txt << _T("wxEVT_SCI_DRAG_OVER");
//  else if (type == wxEVT_SCI_DO_DROP) txt << _T("wxEVT_SCI_DO_DROP");
//  else if (type == wxEVT_SCI_ZOOM) txt << _T("wxEVT_SCI_ZOOM");
//  else if (type == wxEVT_SCI_HOTSPOT_CLICK) txt << _T("wxEVT_SCI_HOTSPOT_CLICK");
//  else if (type == wxEVT_SCI_HOTSPOT_DCLICK) txt << _T("wxEVT_SCI_HOTSPOT_DCLICK");
//  else if (type == wxEVT_SCI_CALLTIP_CLICK) txt << _T("wxEVT_SCI_CALLTIP_CLICK");
//  else if (type == wxEVT_SCI_AUTOCOMP_SELECTION) txt << _T("wxEVT_SCI_AUTOCOMP_SELECTION");
//  else if (type == wxEVT_SCI_INDICATOR_CLICK) txt << _T("wxEVT_SCI_INDICATOR_CLICK");
//  else if (type == wxEVT_SCI_INDICATOR_RELEASE) txt << _T("wxEVT_SCI_INDICATOR_RELEASE");
//    Manager::Get()->GetLogManager()->DebugLog(txt);

    // call any hooked functors
    if (!ProjectManager::IsBusy() && EditorHooks::HasRegisteredHooks())
    {
        EditorHooks::CallHooks(this, event);
    }
}

bool cbEditor::CanSelectAll() const
{
    int res = 0;
    cbStyledTextCtrl* control = GetControl();
    if (control)
        res = control->GetLength();
    return res > 0;
}

void cbEditor::SelectAll()
{
    cbStyledTextCtrl* control = GetControl();
    if (control)
        control->SelectAll();
}
