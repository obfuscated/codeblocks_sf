//////////////////////////////////////////////////////////////////////////////
// File:        edit.cpp
// Purpose:     wxScintilla test module
// Maintainer:  Otto Wyss
// Created:     2003-09-01
// RCS-ID:      $Id: edit.cpp 1960 2006-02-07 13:36:51Z thomasdenk $
// Copyright:   (c) 2004wxCode
// Licence:     wxWindows
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// informations
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// headers
//----------------------------------------------------------------------------

// For compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all 'standard' wxWindows headers)
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

//! wxWindows headers
#include <wx/file.h>     // raw file io support
#include <wx/filename.h> // filename support
#include <wx/fileconf.h>
#include <wx/listctrl.h>

//! application headers
#include "defsext.h"     // additional definitions
#include "edit.h"        // edit module
#include "version.h"
#include "snippetsconfig.h"
#include "messagebox.h"



//----------------------------------------------------------------------------
// resources
//----------------------------------------------------------------------------


//============================================================================
// declarations
//============================================================================


//============================================================================
// implementation
//============================================================================
wxString g_statustext;
//----------------------------------------------------------------------------
// Edit
//----------------------------------------------------------------------------

BEGIN_EVENT_TABLE (Edit, wxScintilla)
    // common
    EVT_SIZE (                         Edit::OnSize)
    // edit
    EVT_MENU (wxID_CLEAR,              Edit::OnEditClear)
    EVT_MENU (wxID_CUT,                Edit::OnEditCut)
    EVT_MENU (wxID_COPY,               Edit::OnEditCopy)
    EVT_MENU (wxID_PASTE,              Edit::OnEditPaste)
    EVT_MENU (myID_INDENTINC,          Edit::OnEditIndentInc)
    EVT_MENU (myID_INDENTRED,          Edit::OnEditIndentRed)
    EVT_MENU (wxID_SELECTALL,          Edit::OnEditSelectAll)
    EVT_MENU (myID_SELECTLINE,         Edit::OnEditSelectLine)
    EVT_MENU (wxID_REDO,               Edit::OnEditRedo)
    EVT_MENU (wxID_UNDO,               Edit::OnEditUndo)
    // find
    EVT_MENU     (wxID_FIND,           Edit::OnFind)
    EVT_UPDATE_UI(wxID_FIND,           Edit::OnFindUI)
    EVT_MENU     (myID_FINDNEXT,       Edit::OnFindNext)
    EVT_UPDATE_UI(myID_FINDNEXT,       Edit::OnFindNextUI)
    EVT_MENU     (myID_FINDPREV,       Edit::OnFindPrev)
    EVT_UPDATE_UI(myID_FINDPREV,       Edit::OnFindPrevUI)
    // replace
    EVT_MENU     (myID_REPLACE,        Edit::OnReplace)
    EVT_UPDATE_UI(myID_REPLACE,        Edit::OnReplaceUI)
    EVT_MENU     (myID_REPLACENEXT,    Edit::OnReplaceNext)
    EVT_UPDATE_UI(myID_REPLACENEXT,    Edit::OnReplaceNextUI)
    EVT_MENU     (myID_BRACEMATCH,     Edit::OnBraceMatch)
    EVT_UPDATE_UI(myID_BRACEMATCH,     Edit::OnBraceMatchUI)
    EVT_MENU (myID_GOTO,               Edit::OnGoto)
    EVT_UPDATE_UI (myID_GOTO,          Edit::OnGotoUI)
    // view
    EVT_MENU_RANGE (myID_HILIGHTFIRST, myID_HILIGHTLAST,
                                       Edit::OnHilightLang)
    EVT_MENU (myID_DISPLAYEOL,         Edit::OnDisplayEOL)
    EVT_MENU (myID_INDENTGUIDE,        Edit::OnIndentGuide)
    EVT_MENU (myID_LINENUMBER,         Edit::OnLineNumber)
    EVT_MENU (myID_LONGLINEON,         Edit::OnLongLineOn)
    EVT_MENU (myID_WHITESPACE,         Edit::OnWhiteSpace)
    EVT_MENU (myID_FOLDTOGGLE,         Edit::OnFoldToggle)
    EVT_MENU (myID_OVERTYPE,           Edit::OnSetOverType)
    EVT_MENU (myID_READONLY,           Edit::OnSetReadOnly)
    EVT_MENU (myID_WRAPMODEON,         Edit::OnWrapmodeOn)
    EVT_MENU (myID_CHARSETANSI,        Edit::OnUseCharset)
    EVT_MENU (myID_CHARSETMAC,         Edit::OnUseCharset)
    // extra
    EVT_MENU (myID_CHANGELOWER,        Edit::OnChangeCase)
    EVT_MENU (myID_CHANGEUPPER,        Edit::OnChangeCase)
    EVT_MENU (myID_CONVERTCR,          Edit::OnConvertEOL)
    EVT_MENU (myID_CONVERTCRLF,        Edit::OnConvertEOL)
    EVT_MENU (myID_CONVERTLF,          Edit::OnConvertEOL)
    // scintilla
    EVT_SCI_MARGINCLICK (-1,           Edit::OnMarginClick)
    EVT_SCI_CHARADDED (-1,             Edit::OnCharAdded)

    //-EVT_ENTER_WINDOW( Edit::OnEnterWindow)
    //-EVT_LEAVE_WINDOW( Edit::OnLeaveWindow)
    EVT_SCROLL       (Edit::OnScroll)
    EVT_SCROLLWIN    (Edit::OnScrollWin)

END_EVENT_TABLE()

// ----------------------------------------------------------------------------
Edit::Edit (wxWindow *parent, wxWindowID id,
            const wxPoint &pos,
            const wxSize &size,
            long style)
// ----------------------------------------------------------------------------
    : wxScintilla (parent, id, pos, size, style)
{

    m_SysWinBkgdColour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW); //(pecan 2007/3/27)

    m_filename = _T("");
    m_language = NULL;

    m_LineNrID = 0;
    m_LineNrMargin = TextWidth (wxSCI_STYLE_LINENUMBER, _T("_999999"));
    m_FoldingID = 1;
    m_FoldingMargin = 16;
    m_DividerID = 2;

    // default font for all styles
    SetViewEOL (g_CommonPrefs.displayEOLEnable);
    SetIndentationGuides (g_CommonPrefs.indentGuideEnable);
    SetMarginWidth (m_LineNrID,
                    g_CommonPrefs.lineNumberEnable? m_LineNrMargin: 0);
    SetEdgeMode (g_CommonPrefs.longLineOnEnable?
                 wxSCI_EDGE_LINE: wxSCI_EDGE_NONE);
    SetViewWhiteSpace (g_CommonPrefs.whiteSpaceEnable?
                       wxSCI_WS_VISIBLEALWAYS: wxSCI_WS_INVISIBLE);
    SetOvertype (g_CommonPrefs.overTypeInitial);
    SetReadOnly (g_CommonPrefs.readOnlyInitial);
    SetWrapMode (g_CommonPrefs.wrapModeInitial?
                 wxSCI_WRAP_WORD: wxSCI_WRAP_NONE);

    //wxFont font (10, wxTELETYPE, wxNORMAL, wxNORMAL);

    // use the same font as CodeBlocks editor's //(pecan 2007/4/04)
    wxFont font(8, wxMODERN, wxNORMAL, wxBOLD);
    StyleSetFont (wxSCI_STYLE_DEFAULT, font);

    StyleSetForeground (wxSCI_STYLE_DEFAULT, wxColour (_T("BLACK")));

    //-StyleSetBackground (wxSCI_STYLE_DEFAULT, wxColour (_T("WHITE")));
    StyleSetBackground (wxSCI_STYLE_DEFAULT, m_SysWinBkgdColour); //(pecan 2007/3/27)

    StyleSetForeground (wxSCI_STYLE_LINENUMBER, wxColour (_T("DARK GREY")));

    //-StyleSetBackground (wxSCI_STYLE_LINENUMBER, wxColour (_T("WHITE")));
    StyleSetBackground (wxSCI_STYLE_DEFAULT, m_SysWinBkgdColour); //(pecan 2007/3/27)

    StyleSetForeground(wxSCI_STYLE_INDENTGUIDE, wxColour (_T("DARK GREY")));
    InitializePrefs (DEFAULT_LANGUAGE);

    // set visibility
    SetVisiblePolicy (wxSCI_VISIBLE_STRICT|wxSCI_VISIBLE_SLOP, 1);
    SetXCaretPolicy (wxSCI_CARET_EVEN|wxSCI_VISIBLE_STRICT|wxSCI_CARET_SLOP, 1);
    SetYCaretPolicy (wxSCI_CARET_EVEN|wxSCI_VISIBLE_STRICT|wxSCI_CARET_SLOP, 1);

    // markers
    MarkerDefine (wxSCI_MARKNUM_FOLDER, wxSCI_MARK_BOXPLUS);
    MarkerSetBackground (wxSCI_MARKNUM_FOLDER, wxColour (_T("BLACK")));
    MarkerSetForeground (wxSCI_MARKNUM_FOLDER, wxColour (_T("WHITE")));
    MarkerDefine (wxSCI_MARKNUM_FOLDEROPEN, wxSCI_MARK_BOXMINUS);
    MarkerSetBackground (wxSCI_MARKNUM_FOLDEROPEN, wxColour (_T("BLACK")));
    MarkerSetForeground (wxSCI_MARKNUM_FOLDEROPEN, wxColour (_T("WHITE")));
    MarkerDefine (wxSCI_MARKNUM_FOLDERSUB, wxSCI_MARK_EMPTY);
    MarkerDefine (wxSCI_MARKNUM_FOLDEREND, wxSCI_MARK_SHORTARROW);
    MarkerDefine (wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_ARROWDOWN);
    MarkerDefine (wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_EMPTY);
    MarkerDefine (wxSCI_MARKNUM_FOLDERTAIL, wxSCI_MARK_EMPTY);

    // clear wrong default keys
#if !defined(__WXGTK__)
    CmdKeyClear (wxSCI_KEY_TAB, 0);
    CmdKeyClear (wxSCI_KEY_TAB, wxSCI_SCMOD_SHIFT);
#endif
    CmdKeyClear ('A', wxSCI_SCMOD_CTRL);
#if !defined(__WXGTK__)
    CmdKeyClear ('C', wxSCI_SCMOD_CTRL);
#endif
    CmdKeyClear ('D', wxSCI_SCMOD_CTRL);
    CmdKeyClear ('D', wxSCI_SCMOD_SHIFT | wxSCI_SCMOD_CTRL);
    CmdKeyClear ('F', wxSCI_SCMOD_ALT | wxSCI_SCMOD_CTRL);
    CmdKeyClear ('L', wxSCI_SCMOD_CTRL);
    CmdKeyClear ('L', wxSCI_SCMOD_SHIFT | wxSCI_SCMOD_CTRL);
    CmdKeyClear ('T', wxSCI_SCMOD_CTRL);
    CmdKeyClear ('T', wxSCI_SCMOD_SHIFT | wxSCI_SCMOD_CTRL);
    CmdKeyClear ('U', wxSCI_SCMOD_CTRL);
    CmdKeyClear ('U', wxSCI_SCMOD_SHIFT | wxSCI_SCMOD_CTRL);
#if !defined(__WXGTK__)
    CmdKeyClear ('V', wxSCI_SCMOD_CTRL);
    CmdKeyClear ('X', wxSCI_SCMOD_CTRL);
#endif
    CmdKeyClear ('Y', wxSCI_SCMOD_CTRL);
#if !defined(__WXGTK__)
    CmdKeyClear ('Z', wxSCI_SCMOD_CTRL);
#endif

    // miscelaneous
    //UsePopUp (0); //(pecan 2007/4/06)
    UsePopUp (1);
    SetLayoutCache (wxSCI_CACHE_PAGE);
    SetBufferedDraw (1);

    // Initialize mouse drag scrolling
    InitDragScroller();

    // create dialogs
    m_FindReplaceDlg = new myFindReplaceDlg (this);
    m_startpos = 0;
    m_replace = false;
    m_GotoDlg = new myGotoDlg (this);

}

// ----------------------------------------------------------------------------
Edit::~Edit ()
// ----------------------------------------------------------------------------
{
     // remove created objects
    if (m_GotoDlg) delete m_GotoDlg;
    if (m_FindReplaceDlg) delete m_FindReplaceDlg;

}
// ----------------------------------------------------------------------------
int Edit::GetLongestLinePixelWidth(int top_line, int bottom_line)  //(pecan 2007/3/28)
// ----------------------------------------------------------------------------
{
    int ste_ctrlCharLengths[32] = { 3, 3, 3, 3, 3, 3, 3, 3,
                                       2, 2, 2, 2, 2, 2, 2, 2,
                                       3, 3, 3, 3, 3, 3, 3, 3,
                                       3, 2, 3, 3, 2, 2, 2, 2 };

    int longest_len   = 0;
    int first_line    = top_line < 0 ? GetFirstVisibleLine() : top_line;
    int line_count    = GetLineCount();
    int lines_visible = LinesOnScreen();
    int last_line     = bottom_line < 0 ? wxMin(line_count, first_line + lines_visible) : bottom_line;
    int tab_width     = GetTabWidth();
    int ctrl_char_symbol = GetControlCharSymbol();

    if (last_line < first_line)
    {
        int tmp = first_line; first_line = last_line; last_line = tmp;
    }

    // FIXME this is not the best solution, but with some luck it'll work
    //       Scintilla should provide this info from its LayoutCache
    for (int n = first_line; n <= last_line; n++)
    {
        int len = LineLength(n);

        int tabs = 0;
        if ((tab_width > 1) && (len*tab_width > longest_len))
        {
            // need to sum up only how much of the tab is used
            wxCharBuffer buf = GetLineRaw(n);
            const char* c = buf.data();
            for (int i = 0; i < len; i++, c++)
            {
                if (*c == '\t')
                    tabs += tab_width - ((i + tabs) % tab_width);
                else if ((ctrl_char_symbol >= 32) && (size_t(*c) < 32))
                {
                    // scintilla writes name of char
                    tabs += ste_ctrlCharLengths[size_t(*c)] - 1;
                }
            }

            //wxPrintf(wxT("line %d len %d pos %d\n"), n, len, len+tabs); fflush(stdout);
        }
        len += tabs + 3; // add a little extra if showing line endings
        if (longest_len < len) longest_len = len;
    }
    // FIXME:
    //return pEdit->TextWidth(wxSTC_STYLE_DEFAULT, wxString(longest_len, wxT('D')));
    return TextWidth(0, wxString(longest_len, wxT('D')));
}

//----------------------------------------------------------------------------
// common event handlers
// ----------------------------------------------------------------------------
void Edit::OnSize( wxSizeEvent& event )
// ----------------------------------------------------------------------------
{
    int x = GetClientSize().x +
            (g_CommonPrefs.lineNumberEnable? m_LineNrMargin: 0) +
            (g_CommonPrefs.foldEnable? m_FoldingMargin: 0);
    //(pecan 2007/4/05)
    //-if (x > 0) SetScrollWidth (x);
    if (x > 0) SetScrollWidth (x<<1);
    //(pecan 2007/4/05);
    event.Skip();
}

// edit event handlers
void Edit::OnEditRedo (wxCommandEvent &WXUNUSED(event)) {
    if (!CanRedo()) return;
    Redo ();
}

void Edit::OnEditUndo (wxCommandEvent &WXUNUSED(event)) {
    if (!CanUndo()) return;
    Undo ();
}

void Edit::OnEditClear (wxCommandEvent &WXUNUSED(event)) {
    if (GetReadOnly()) return;
    Clear ();
}

void Edit::OnEditCut (wxCommandEvent &WXUNUSED(event)) {
    if (GetReadOnly() || (GetSelectionEnd()-GetSelectionStart() <= 0)) return;
    Cut ();
}

void Edit::OnEditCopy (wxCommandEvent &WXUNUSED(event)) {
    if (GetSelectionEnd()-GetSelectionStart() <= 0) return;
    Copy ();
}

void Edit::OnEditPaste (wxCommandEvent &WXUNUSED(event)) {
    if (!CanPaste()) return;
    Paste ();
}

void Edit::OnFind (wxCommandEvent& event)
{
    m_startpos = GetCurrentPos();
    SetTargetStart (m_startpos);
    SetTargetEnd (m_startpos);
    m_replace = false;
    if (GetSelectionEnd()-GetSelectionStart() > 0)
    {
        m_FindReplaceDlg->SetFindString (GetSelectedText());
    }
    GetConfig()->CenterChildOnParent(m_FindReplaceDlg);
    if (m_FindReplaceDlg->ShowModal () != wxID_OK) return;
    OnFindNext (event);
}

// ----------------------------------------------------------------------------
void Edit::OnFindUI (wxUpdateUIEvent &event)
// ----------------------------------------------------------------------------
{
     //LOGIT( _T("OnFindUI[%d]"),GetLength() );
    event.Enable (GetLength() > 0);
}

// ----------------------------------------------------------------------------
void Edit::OnFindNext (wxCommandEvent &WXUNUSED(event))
// ----------------------------------------------------------------------------
{
    wxString findtext = m_FindReplaceDlg->GetFindString();
    if (findtext.IsEmpty()) {
        //wxMessageBox (_("Can't find anything with empty string"),
        //              _("Find string"), wxOK | wxICON_INFORMATION, this);
        messageBox (_("Can't find anything with empty string"),
                      _("Find string"), wxOK | wxICON_INFORMATION);
        return;
    }
    m_startpos = GetCurrentPos();
    SetTargetStart (m_startpos);
    SetTargetEnd (m_startpos);
    int pos = FindString (findtext, m_FindReplaceDlg->GetFlags());
    if (pos >= 0) {
        EnsureCaretVisible();
        SetSelection (pos, pos + findtext.Length());
        g_statustext = _("Found text: ") + findtext;
    }else{
        //wxMessageBox (_("Can't anymore find the string: \"") + findtext + _("\""),
        //              _("Find string"), wxOK | wxICON_INFORMATION, this);
        messageBox (_("Cannot find the string: \"") + findtext + _("\""),
                      _("Find string"), wxOK | wxICON_INFORMATION);
        g_statustext = _("No more text found!");
    }
    SetSCIFocus(true);
    SetFocus ();
}
// ----------------------------------------------------------------------------
void Edit::OnFindNextUI (wxUpdateUIEvent &event)
// ----------------------------------------------------------------------------
{
    //LOGIT( _T("Edit::OnFindNextUI[%d]"), event.GetId() );
    event.Enable (GetLength() > 0);
}
// ----------------------------------------------------------------------------
void Edit::OnFindPrev (wxCommandEvent &WXUNUSED(event))
// ----------------------------------------------------------------------------
{
    wxString findtext = m_FindReplaceDlg->GetFindString();
    if (findtext.IsEmpty()) {
        //wxMessageBox (_("Can't find anything with empty string"),
        //              _("Find string"), wxOK | wxICON_INFORMATION, this);
        messageBox (_("Can't find anything with empty string"),
                      _("Find string"), wxOK | wxICON_INFORMATION);
        return;
    }
    m_startpos = GetCurrentPos() - findtext.Length();
    if (m_startpos < 0) m_startpos = 0;
    SetTargetStart (m_startpos);
    SetTargetEnd (m_startpos);
    int pos = FindString (findtext, m_FindReplaceDlg->GetFlags() & !myFR_DOWN);
    if (pos >= 0) {
        EnsureCaretVisible();
        SetSelection (pos, pos + findtext.Length());
        g_statustext = _("Found text: ") + findtext;
    }else{
        //wxMessageBox (_("Can't anymore find the string: \"") + findtext + _("\""),
        //              _("Find string"), wxOK | wxICON_INFORMATION, this);
        messageBox (_("Can't anymore find the string: \"") + findtext + _("\""),
                      _("Find string"), wxOK | wxICON_INFORMATION);
        g_statustext = _("No more text found!");
    }
    SetSCIFocus(true);
    SetFocus ();
}

// ----------------------------------------------------------------------------
void Edit::OnFindPrevUI (wxUpdateUIEvent &event)
// ----------------------------------------------------------------------------
{
    event.Enable (GetLength() > 0);
}

// ----------------------------------------------------------------------------
int Edit::FindString (const wxString &str, int flags)
// ----------------------------------------------------------------------------
{

    // initialize direction
    bool down = (flags & myFR_DOWN) > 0;
    bool first;
    if (down) {
        SetTargetStart (GetTargetEnd());
        first = m_startpos <= GetTargetEnd();
        if (first) {
            SetTargetEnd (GetLength() - 1);
        }else{
            SetTargetEnd (m_startpos);
        }
    }else{
        SetTargetStart (GetTargetStart());
        first = m_startpos >= GetTargetStart();
        if (first) {
            SetTargetEnd (0);
        }else{
            SetTargetEnd (m_startpos);
        }
    }

    // set flags
    int findFlags = 0;
    if ((flags & myFR_MATCHCASE) > 0) findFlags |= wxSCI_FIND_MATCHCASE;
    if ((flags & myFR_WHOLEWORD) > 0) findFlags |= wxSCI_FIND_WHOLEWORD;
    if ((flags & myFR_FINDREGEX) > 0) findFlags |= wxSCI_FIND_REGEXP;
    SetSearchFlags (findFlags);

    // search string
    int pos = SearchInTarget (str);
    if (pos >= 0) return pos;

    // search the rest if any
    if (down && first) {
        SetTargetStart (0);
        SetTargetEnd (m_startpos);
    }else if (!down && first) {
        SetTargetStart (GetLength() - 1);
        SetTargetEnd (m_startpos);
    }else{
        return pos;
    }
    return pos = SearchInTarget (str);
}

// ----------------------------------------------------------------------------
void Edit::OnReplace (wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    m_startpos = GetCurrentPos();
    SetTargetStart (m_startpos);
    SetTargetEnd (m_startpos);
    m_replace = true;
    if (GetSelectionEnd()-GetSelectionStart() > 0) {
        m_FindReplaceDlg->SetFindString (GetSelectedText());
    }
    GetConfig()->CenterChildOnParent(m_FindReplaceDlg);
    int retCode = m_FindReplaceDlg->ShowModal (myFR_REPLACEDIALOG);
    if (retCode == wxID_OK) {
        OnFindNext (event);
    }else if (retCode == myID_REPLACE) {
        OnReplaceNext (event);
    }else if (retCode == myID_REPLACEALL) {
        OnReplaceAll (event);
    }

}
// ----------------------------------------------------------------------------
void Edit::OnReplaceUI (wxUpdateUIEvent& event)
// ----------------------------------------------------------------------------
{
    event.Enable (!GetReadOnly() && (GetLength() > 0));
}

// ----------------------------------------------------------------------------
void Edit::OnReplaceNext (wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    if (!m_replace) return;
    wxString findtext = m_FindReplaceDlg->GetFindString();
    if (findtext.IsEmpty()) {
        //wxMessageBox (_("Can't find anything with empty string"),
        //              _("Find string"), wxOK | wxICON_INFORMATION, this);
        messageBox (_("Can't find anything with empty string"),
                      _("Find string"), wxOK | wxICON_INFORMATION);
        return;
    }
    int pos = FindString (findtext, m_FindReplaceDlg->GetFlags());
    if (pos >= 0) {
        SetSelection (pos, pos + findtext.Length());
        wxString replacetext = m_FindReplaceDlg->GetReplaceString ();
        ReplaceSelection (replacetext);
        EnsureCaretVisible();
        SetSelection (pos, pos + replacetext.Length());
        g_statustext = _("Replaced text: ") + findtext;
    }else{
        //wxMessageBox (_("Can't anymore find the string: \"") + findtext + _("\""),
        //              _("Replace string"), wxOK | wxICON_INFORMATION, this);
        messageBox (_("Can't anymore find the string: \"") + findtext + _("\""),
                      _("Replace string"), wxOK | wxICON_INFORMATION);
        g_statustext = _("No more text found!");

    }
    SetSCIFocus(true);
    SetFocus ();

}

// ----------------------------------------------------------------------------
void Edit::OnReplaceNextUI (wxUpdateUIEvent& event)
// ----------------------------------------------------------------------------
{
    event.Enable (!GetReadOnly() && (GetLength() > 0));
}

// ----------------------------------------------------------------------------
void Edit::OnReplaceAll (wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    if (!m_replace) return;
    wxString findtext = m_FindReplaceDlg->GetFindString();
    if (findtext.IsEmpty()) {
        //wxMessageBox (_("Can't find anything with empty string"),
        //              _("Find string"), wxOK | wxICON_INFORMATION, this);
        messageBox (_("Can't find anything with empty string"),
                      _("Find string"), wxOK | wxICON_INFORMATION);
        return;
    }
    BeginUndoAction();
    while (FindString (findtext, m_FindReplaceDlg->GetFlags()) >= 0) {
        wxString replacetext = m_FindReplaceDlg->GetReplaceString ();
        ReplaceTarget (replacetext);
    }
    EndUndoAction();
    //wxMessageBox (_("All occurence of text \"") + findtext + _("\" have been replaced"),
    //              _("Replace string"), wxOK | wxICON_INFORMATION, this);
    messageBox (_("All occurence of text \"") + findtext + _("\" have been replaced"),
                  _("Replace string"), wxOK | wxICON_INFORMATION);
    g_statustext = _("All occurence have been replaced");
    m_replace = false;
}

// ----------------------------------------------------------------------------
void Edit::OnBraceMatch (wxCommandEvent &WXUNUSED(event))
// ----------------------------------------------------------------------------
{
    int min = GetCurrentPos ();
    int max = BraceMatch (min);
    if (max > (min+1)) {
        BraceHighlight (min+1, max);
        SetSelection (min+1, max);
    }else{
        BraceBadLight (min);
    }
}
// ----------------------------------------------------------------------------
void Edit::OnBraceMatchUI (wxUpdateUIEvent &event)
// ----------------------------------------------------------------------------
{
    event.Enable (GetLength() > 0);
}

// ----------------------------------------------------------------------------
void Edit::OnGoto (wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    GetConfig()->CenterChildOnParent(m_GotoDlg);
    if (m_GotoDlg->ShowModal() != wxID_OK) return;
    int line = m_GotoDlg->GetPosition ();
    if (line > 0) {
        GotoLine (line - 1);
        line = GetCurrentLine() - (GetFirstVisibleLine()+2);
        if (line > 0) LineScroll (0, line);
        g_statustext = _("Went to line: ") + wxString::Format (_T("%d"), line);
    }else{
        GotoLine (0);
        g_statustext = _("Went to first line");
    }
    SetSCIFocus(true);
    SetFocus ();

}
// ----------------------------------------------------------------------------
void Edit::OnGotoUI (wxUpdateUIEvent &event)
// ----------------------------------------------------------------------------
{
    event.Enable (GetLineCount() > 1);
}


void Edit::OnEditIndentInc (wxCommandEvent &WXUNUSED(event))
{
    CmdKeyExecute (wxSCI_CMD_TAB);
}

void Edit::OnEditIndentRed (wxCommandEvent &WXUNUSED(event))
{
    CmdKeyExecute (wxSCI_CMD_DELETEBACK);
}

void Edit::OnEditSelectAll (wxCommandEvent &WXUNUSED(event))
{
    SetSelection (0, GetLength());
}

void Edit::OnEditSelectLine (wxCommandEvent &WXUNUSED(event))
{
    int lineStart = PositionFromLine (GetCurrentLine());
    int lineEnd = PositionFromLine (GetCurrentLine() + 1);
    SetSelection (lineStart, lineEnd);
}

void Edit::OnHilightLang (wxCommandEvent &event)
{
    InitializePrefs (g_LanguagePrefs [event.GetId() - myID_HILIGHTFIRST].name);
}

void Edit::OnDisplayEOL (wxCommandEvent &WXUNUSED(event))
{
    SetViewEOL (!GetViewEOL());
}

void Edit::OnIndentGuide (wxCommandEvent &WXUNUSED(event))
{
    SetIndentationGuides (!GetIndentationGuides());
}

void Edit::OnLineNumber (wxCommandEvent &WXUNUSED(event))
{
    SetMarginWidth (m_LineNrID,
                    GetMarginWidth (m_LineNrID) == 0? m_LineNrMargin: 0);
}

void Edit::OnLongLineOn (wxCommandEvent &WXUNUSED(event))
{
    SetEdgeMode (GetEdgeMode() == 0? wxSCI_EDGE_LINE: wxSCI_EDGE_NONE);
}

void Edit::OnWhiteSpace (wxCommandEvent &WXUNUSED(event))
{
    SetViewWhiteSpace (GetViewWhiteSpace() == 0?
                       wxSCI_WS_VISIBLEALWAYS: wxSCI_WS_INVISIBLE);
}

void Edit::OnFoldToggle (wxCommandEvent &WXUNUSED(event))
{
    ToggleFold (GetFoldParent(GetCurrentLine()));
}

void Edit::OnSetOverType (wxCommandEvent &WXUNUSED(event))
{
    SetOvertype (!GetOvertype());
}

void Edit::OnSetReadOnly (wxCommandEvent &WXUNUSED(event))
{
    SetReadOnly (!GetReadOnly());
}

void Edit::OnWrapmodeOn (wxCommandEvent &WXUNUSED(event))
{
    SetWrapMode (GetWrapMode() == 0? wxSCI_WRAP_WORD: wxSCI_WRAP_NONE);
}

void Edit::OnUseCharset (wxCommandEvent &event)
{
    int Nr;
    int charset = GetCodePage();
    switch (event.GetId()) {
        case myID_CHARSETANSI: {charset = wxSCI_CHARSET_ANSI; break;}
        case myID_CHARSETMAC: {charset = wxSCI_CHARSET_ANSI; break;}
    }
    for (Nr = 0; Nr < wxSCI_STYLE_LASTPREDEFINED; Nr++)
    {
        StyleSetCharacterSet (Nr, charset);
    }
    SetCodePage (charset);
}

void Edit::OnChangeCase (wxCommandEvent &event)
{
    switch (event.GetId()) {
        case myID_CHANGELOWER: {
            CmdKeyExecute (wxSCI_CMD_LOWERCASE);
            break;
        }
        case myID_CHANGEUPPER: {
            CmdKeyExecute (wxSCI_CMD_UPPERCASE);
            break;
        }
    }
}

void Edit::OnConvertEOL (wxCommandEvent &event)
{
    int eolMode = GetEOLMode();
    switch (event.GetId()) {
        case myID_CONVERTCR: { eolMode = wxSCI_EOL_CR; break;}
        case myID_CONVERTCRLF: { eolMode = wxSCI_EOL_CRLF; break;}
        case myID_CONVERTLF: { eolMode = wxSCI_EOL_LF; break;}
    }
    ConvertEOLs (eolMode);
    SetEOLMode (eolMode);
}

//! misc
void Edit::OnMarginClick (wxScintillaEvent &event)
{
    if (event.GetMargin() == 2) {
        int lineClick = LineFromPosition (event.GetPosition());
        int levelClick = GetFoldLevel (lineClick);
        if ((levelClick & wxSCI_FOLDLEVELHEADERFLAG) > 0) {
            ToggleFold (lineClick);
        }
    }
}

void Edit::OnCharAdded (wxScintillaEvent &event)
{
    char chr = event.GetKey();
    int currentLine = GetCurrentLine();
    // Change this if support for mac files with \r is needed
    if (chr == '\n') {
        int lineInd = 0;
        if (currentLine > 0) {
            lineInd = GetLineIndentation(currentLine - 1);
        }
        if (lineInd == 0) return;
        SetLineIndentation (currentLine, lineInd);
        GotoPos(PositionFromLine (currentLine) + lineInd);
    }
}


//----------------------------------------------------------------------------
// private functions
wxString Edit::DeterminePrefs (const wxString &filename) {

    LanguageInfo const* curInfo;

    // determine language from filepatterns
    int languageNr;
    for (languageNr = 0; languageNr < g_LanguagePrefsSize; languageNr++) {
        curInfo = &g_LanguagePrefs [languageNr];
        wxString filepattern = curInfo->filepattern;
        filepattern.Lower();
        while (!filepattern.IsEmpty()) {
            wxString cur = filepattern.BeforeFirst (';');
            if ((cur == filename) ||
                (cur == (filename.BeforeLast ('.') + _T(".*"))) ||
                (cur == (_T("*.") + filename.AfterLast ('.')))) {
                return curInfo->name;
            }
            filepattern = filepattern.AfterFirst (';');
        }
    }
    return wxEmptyString;

}

bool Edit::InitializePrefs (const wxString &name) {

    // initialize styles
    StyleClearAll();
    LanguageInfo const* curInfo = NULL;

    // determine language
    bool found = false;
    int languageNr;
    for (languageNr = 0; languageNr < g_LanguagePrefsSize; languageNr++) {
        curInfo = &g_LanguagePrefs [languageNr];
        if (curInfo->name == name) {
            found = true;
            break;
        }
    }
    if (!found) return false;

    // set lexer and language
    SetLexer (curInfo->lexer);
    m_language = curInfo;

    // set margin for line numbers
    SetMarginType (m_LineNrID, wxSCI_MARGIN_NUMBER);
    StyleSetForeground (wxSCI_STYLE_LINENUMBER, wxColour (_T("DARK GREY")));
    //-StyleSetBackground (wxSCI_STYLE_LINENUMBER, wxColour (_T("WHITE")));
    StyleSetBackground (wxSCI_STYLE_DEFAULT, m_SysWinBkgdColour); //(pecan 2007/3/27)
    SetMarginWidth (m_LineNrID,
                    g_CommonPrefs.lineNumberEnable? m_LineNrMargin: 0);

    // set common styles
    StyleSetForeground (wxSCI_STYLE_DEFAULT, wxColour (_T("DARK GREY")));
    StyleSetForeground (wxSCI_STYLE_INDENTGUIDE, wxColour (_T("DARK GREY")));

    // initialize settings
    if (g_CommonPrefs.syntaxEnable) {
        int keywordnr = 0;
        int Nr;
        for (Nr = 0; Nr < STYLE_TYPES_COUNT; Nr++) {
            if (curInfo->styles[Nr].type == -1) continue;
            const StyleInfo &curType = g_StylePrefs [curInfo->styles[Nr].type];
            //-wxFont font (curType.fontsize, wxTELETYPE, wxNORMAL, wxNORMAL, false,
            //-             curType.fontname);
            wxFont font ( 8, wxMODERN, wxNORMAL, wxBOLD, false, //(pecan 2007/4/04)
                         curType.fontname);
            StyleSetFont (Nr, font);
            if (curType.foreground) {
                StyleSetForeground (Nr, wxColour (curType.foreground));
            }
            if (curType.background) {
                //-StyleSetBackground (Nr, wxColour (curType.background));
                StyleSetBackground (Nr, wxColour (m_SysWinBkgdColour)); //(pecan 2007/4/04)
            }
            StyleSetBold (Nr, (curType.fontstyle & TOKEN_STYLE_BOLD) > 0);
            StyleSetBold (Nr, true); // set bold //(pecan 2007/4/04)
            StyleSetItalic (Nr, (curType.fontstyle & TOKEN_STYLE_ITALIC) > 0);
            StyleSetUnderline (Nr, (curType.fontstyle & TOKEN_STYLE_UNDERL) > 0);
            StyleSetVisible (Nr, (curType.fontstyle & TOKEN_STYLE_HIDDEN) == 0);
            StyleSetCase (Nr, curType.lettercase);
            const wxChar *pwords = curInfo->styles[Nr].words;
            if (pwords) {
                SetKeyWords (keywordnr, pwords);
                keywordnr += 1;
            }
        }
    }

    // set margin as unused
    SetMarginType (m_DividerID, wxSCI_MARGIN_SYMBOL);
    SetMarginWidth (m_DividerID, 8);
    SetMarginSensitive (m_DividerID, false);

    // folding
    SetMarginType (m_FoldingID, wxSCI_MARGIN_SYMBOL);
    SetMarginMask (m_FoldingID, wxSCI_MASK_FOLDERS);
    StyleSetBackground (m_FoldingID, wxColour (_T("WHITE")));
    SetMarginWidth (m_FoldingID, 0);
    SetMarginSensitive (m_FoldingID, false);
    if (g_CommonPrefs.foldEnable) {
        SetMarginWidth (m_FoldingID, curInfo->folds != 0? m_FoldingMargin: 0);
        SetMarginSensitive (m_FoldingID, curInfo->folds != 0);
        SetProperty (_T("fold"), curInfo->folds != 0? _T("1"): _T("0"));
        SetProperty (_T("fold.comment"),
                     (curInfo->folds & FOLD_TYPE_COMMENT) > 0? _T("1"): _T("0"));
        SetProperty (_T("fold.compact"),
                     (curInfo->folds & FOLD_TYPE_COMPACT) > 0? _T("1"): _T("0"));
        SetProperty (_T("fold.preprocessor"),
                     (curInfo->folds & FOLD_TYPE_PREPROC) > 0? _T("1"): _T("0"));
        SetProperty (_T("fold.html"),
                     (curInfo->folds & FOLD_TYPE_HTML) > 0? _T("1"): _T("0"));
        SetProperty (_T("fold.html.preprocessor"),
                     (curInfo->folds & FOLD_TYPE_HTMLPREP) > 0? _T("1"): _T("0"));
        SetProperty (_T("fold.comment.python"),
                     (curInfo->folds & FOLD_TYPE_COMMENTPY) > 0? _T("1"): _T("0"));
        SetProperty (_T("fold.quotes.python"),
                     (curInfo->folds & FOLD_TYPE_QUOTESPY) > 0? _T("1"): _T("0"));
    }
    SetFoldFlags (wxSCI_FOLDFLAG_LINEBEFORE_CONTRACTED |
                  wxSCI_FOLDFLAG_LINEAFTER_CONTRACTED);

    // set spaces and indention
    SetTabWidth (4);
    SetUseTabs (false);
    SetTabIndents (true);
    SetBackSpaceUnIndents (true);
    SetIndent (g_CommonPrefs.indentEnable? 4: 0);

    // others
    SetViewEOL (g_CommonPrefs.displayEOLEnable);
    SetIndentationGuides (g_CommonPrefs.indentGuideEnable);
    SetEdgeColumn (80);
    SetEdgeMode (g_CommonPrefs.longLineOnEnable? wxSCI_EDGE_LINE: wxSCI_EDGE_NONE);
    SetViewWhiteSpace (g_CommonPrefs.whiteSpaceEnable?
                       wxSCI_WS_VISIBLEALWAYS: wxSCI_WS_INVISIBLE);
    SetOvertype (g_CommonPrefs.overTypeInitial);
    SetReadOnly (g_CommonPrefs.readOnlyInitial);
    SetWrapMode (g_CommonPrefs.wrapModeInitial?
                 wxSCI_WRAP_WORD: wxSCI_WRAP_NONE);

    return true;
}

bool Edit::LoadFile () {

    // get filname
    if (!m_filename) {
        wxFileDialog dlg (this, _T("Open file"), _T(""), _T(""),
                          _T("Any file (*)|*"), wxOPEN | wxFILE_MUST_EXIST | wxCHANGE_DIR);
        if (dlg.ShowModal() != wxID_OK) return false;
        m_filename = dlg.GetPath();
    }

    // load file
    return LoadFile (m_filename);
}

// ----------------------------------------------------------------------------
bool Edit::LoadFile (const wxString &filename)
// ----------------------------------------------------------------------------
{

    // load file in edit and clear undo
    if (!filename.IsEmpty()) m_filename = filename;
    if (!wxScintilla::LoadFile (m_filename)) return false;

    // determine lexer language
    wxFileName fname (m_filename);
    InitializePrefs (DeterminePrefs (fname.GetFullName()));

    //(pecan 2007/4/05)
    // calculate the smallest line number width to save space
    int line_count = GetLineCount();
    line_count = wxMax(line_count, 1)*10;
    //-wxString lineStr((int)log10((double)line_count), wxT('5'));
    wxString lineStr(wxString::Format(wxT("%d"), line_count));
    int line_margin_width = TextWidth(wxSCI_STYLE_LINENUMBER, lineStr);
    SetMarginWidth( m_LineNrID, line_margin_width);
    //(pecan 2007/4/05)

    return true;
}

// ----------------------------------------------------------------------------
bool Edit::SaveFile ()
// ----------------------------------------------------------------------------
{

    // return if no change
    if (!Modified()) return true;

    // get filname
    if (!m_filename) {
        wxFileDialog dlg (this, _T("Save file"), _T(""), _T(""), _T("Any file (*)|*"),
                          wxSAVE | wxOVERWRITE_PROMPT);
        if (dlg.ShowModal() != wxID_OK) return false;
        m_filename = dlg.GetPath();
    }

    // save file
    return SaveFile (m_filename);
}

// ----------------------------------------------------------------------------
bool Edit::SaveFile (const wxString &filename)
// ----------------------------------------------------------------------------
{

    // return if no change
    if (!Modified()) return true;

    //(pecan 2007/4/05)
    // calculate the smallest line number width to save space
    int line_count = GetLineCount();
    line_count = wxMax(line_count, 1)*10;
    //-wxString lineStr((int)log10((double)line_count), wxT('5'));
    wxString lineStr(wxString::Format(wxT("%d"), line_count));
    int line_margin_width = TextWidth(wxSCI_STYLE_LINENUMBER, lineStr);
    SetMarginWidth( m_LineNrID, line_margin_width);
    //(pecan 2007/4/05)

    return wxScintilla::SaveFile(filename);

}

// ----------------------------------------------------------------------------
bool Edit::Modified ()
// ----------------------------------------------------------------------------
{

    // return modified state
    return (GetModify() && !GetReadOnly());
}

//----------------------------------------------------------------------------
// EditProperties
//----------------------------------------------------------------------------

EditProperties::EditProperties (Edit *edit,
                                long style)
        : wxDialog (edit, -1, wxEmptyString,
                    wxDefaultPosition, wxDefaultSize,
                    style | wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) {

    // sets the application title
    SetTitle (_("Properties"));
    wxString text;

    // fullname
    wxBoxSizer *fullname = new wxBoxSizer (wxHORIZONTAL);
    fullname->Add (10, 0);
    fullname->Add (new wxStaticText (this, -1, _("Full filename"),
                                     wxDefaultPosition, wxSize(80, -1)),
                   0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
    fullname->Add (new wxStaticText (this, -1, edit->GetFilename()),
                   0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

    // text info
    wxGridSizer *textinfo = new wxGridSizer (4, 0, 2);
    textinfo->Add (new wxStaticText (this, -1, _("Language"),
                                     wxDefaultPosition, wxSize(80, -1)),
                   0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 4);
    textinfo->Add (new wxStaticText (this, -1, edit->m_language->name),
                   0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxRIGHT, 4);
    textinfo->Add (new wxStaticText (this, -1, _("Lexer-ID: "),
                                     wxDefaultPosition, wxSize(80, -1)),
                   0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 4);
    text = wxString::Format (_T("%d"), edit->GetLexer());
    textinfo->Add (new wxStaticText (this, -1, text),
                   0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxRIGHT, 4);
    wxString EOLtype = _T("");
    switch (edit->GetEOLMode()) {
        case wxSCI_EOL_CR: {EOLtype = _T("CR (Unix)"); break; }
        case wxSCI_EOL_CRLF: {EOLtype = _T("CRLF (Windows)"); break; }
        case wxSCI_EOL_LF: {EOLtype = _T("CR (Macintosh)"); break; }
    }
    textinfo->Add (new wxStaticText (this, -1, _("Line endings"),
                                     wxDefaultPosition, wxSize(80, -1)),
                   0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 4);
    textinfo->Add (new wxStaticText (this, -1, EOLtype),
                   0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxRIGHT, 4);

    // text info box
    wxStaticBoxSizer *textinfos = new wxStaticBoxSizer (
                     new wxStaticBox (this, -1, _("Informations")),
                     wxVERTICAL);
    textinfos->Add (textinfo, 0, wxEXPAND);
    textinfos->Add (0, 6);

    // statistic
    wxGridSizer *statistic = new wxGridSizer (4, 0, 2);
    statistic->Add (new wxStaticText (this, -1, _("Total lines"),
                                     wxDefaultPosition, wxSize(80, -1)),
                    0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 4);
    text = wxString::Format (_T("%d"), edit->GetLineCount());
    statistic->Add (new wxStaticText (this, -1, text),
                    0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxRIGHT, 4);
    statistic->Add (new wxStaticText (this, -1, _("Total chars"),
                                     wxDefaultPosition, wxSize(80, -1)),
                    0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 4);
    text = wxString::Format (_T("%d"), edit->GetTextLength());
    statistic->Add (new wxStaticText (this, -1, text),
                    0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxRIGHT, 4);
    statistic->Add (new wxStaticText (this, -1, _("Current line"),
                                     wxDefaultPosition, wxSize(80, -1)),
                    0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 4);
    text = wxString::Format (_T("%d"), edit->GetCurrentLine());
    statistic->Add (new wxStaticText (this, -1, text),
                    0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxRIGHT, 4);
    statistic->Add (new wxStaticText (this, -1, _("Current pos"),
                                     wxDefaultPosition, wxSize(80, -1)),
                    0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 4);
    text = wxString::Format (_T("%d"), edit->GetCurrentPos());
    statistic->Add (new wxStaticText (this, -1, text),
                    0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxRIGHT, 4);

    // char/line statistics
    wxStaticBoxSizer *statistics = new wxStaticBoxSizer (
                     new wxStaticBox (this, -1, _("Statistics")),
                     wxVERTICAL);
    statistics->Add (statistic, 0, wxEXPAND);
    statistics->Add (0, 6);

    // total pane
    wxBoxSizer *totalpane = new wxBoxSizer (wxVERTICAL);
    totalpane->Add (fullname, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
    totalpane->Add (0, 6);
    totalpane->Add (textinfos, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
    totalpane->Add (0, 10);
    totalpane->Add (statistics, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
    totalpane->Add (0, 6);
    wxButton *okButton = new wxButton (this, wxID_OK, _("OK"));
    okButton->SetDefault();
    totalpane->Add (okButton, 0, wxALIGN_CENTER | wxALL, 10);

    SetSizerAndFit (totalpane);
    GetConfig()->CenterChildOnParent(this);
    ShowModal();
}

//----------------------------------------------------------------------------
// EditPrint
//----------------------------------------------------------------------------

EditPrint::EditPrint (Edit *edit, wxChar *title)
              : wxPrintout(title) {
    m_edit = edit;
    m_printed = 0;

}

bool EditPrint::OnPrintPage (int page) {

    wxDC *dc = GetDC();
    if (!dc) return false;

    // scale DC
    PrintScaling (dc);

    // print page
    if (page == 1) m_printed = 0;
    m_printed = m_edit->FormatRange (1, m_printed, m_edit->GetLength(),
                                     dc, dc, m_printRect, m_pageRect);

    return true;
}

bool EditPrint::OnBeginDocument (int startPage, int endPage) {

    if (!wxPrintout::OnBeginDocument (startPage, endPage)) {
        return false;
    }

    return true;
}

void EditPrint::GetPageInfo (int *minPage, int *maxPage, int *selPageFrom, int *selPageTo) {

    // initialize values
    *minPage = 0;
    *maxPage = 0;
    *selPageFrom = 0;
    *selPageTo = 0;

    // scale DC if possible
    wxDC *dc = GetDC();
    if (!dc) return;
    PrintScaling (dc);

    // get print page informations and convert to printer pixels
    wxSize ppiScr;
    GetPPIScreen (&ppiScr.x, &ppiScr.y);
    wxSize page = g_pageSetupData->GetPaperSize();
    page.x = static_cast<int> (page.x * ppiScr.x / 25.4);
    page.y = static_cast<int> (page.y * ppiScr.y / 25.4);
    m_pageRect = wxRect (0,
                         0,
                         page.x,
                         page.y);

    // get margins informations and convert to printer pixels
    int  top = 25; // default 25
    int  bottom = 25; // default 25
    int  left = 20; // default 20
    int  right = 20; // default 20
    wxPoint (top, left) = g_pageSetupData->GetMarginTopLeft();
    wxPoint (bottom, right) = g_pageSetupData->GetMarginBottomRight();
    top = static_cast<int> (top * ppiScr.y / 25.4);
    bottom = static_cast<int> (bottom * ppiScr.y / 25.4);
    left = static_cast<int> (left * ppiScr.x / 25.4);
    right = static_cast<int> (right * ppiScr.x / 25.4);
    m_printRect = wxRect (left,
                          top,
                          page.x - (left + right),
                          page.y - (top + bottom));

    // count pages
    while (HasPage (*maxPage)) {
        m_printed = m_edit->FormatRange (0, m_printed, m_edit->GetLength(),
                                       dc, dc, m_printRect, m_pageRect);
        *maxPage += 1;
    }
    if (*maxPage > 0) *minPage = 1;
    *selPageFrom = *minPage;
    *selPageTo = *maxPage;
    m_printed = 0;
}

bool EditPrint::HasPage (int WXUNUSED(page)) {

    return (m_printed < m_edit->GetLength());
}

bool EditPrint::PrintScaling (wxDC *dc){

    // check for dc, return if none
    if (!dc) return false;

    // get printer and screen sizing values
    wxSize ppiScr;
    GetPPIScreen (&ppiScr.x, &ppiScr.y);
    if (ppiScr.x == 0) { // most possible guess 96 dpi
        ppiScr.x = 96;
        ppiScr.y = 96;
    }
    wxSize ppiPrt;
    GetPPIPrinter (&ppiPrt.x, &ppiPrt.y);
    if (ppiPrt.x == 0) { // scaling factor to 1
        ppiPrt.x = ppiScr.x;
        ppiPrt.y = ppiScr.y;
    }
    wxSize dcSize = dc->GetSize();
    wxSize pageSize;
    GetPageSizePixels (&pageSize.x, &pageSize.y);

    // set user scale
    float scale_x = (float)(ppiPrt.x * dcSize.x) /
                    (float)(ppiScr.x * pageSize.x);
    float scale_y = (float)(ppiPrt.y * dcSize.y) /
                    (float)(ppiScr.y * pageSize.y);
    dc->SetUserScale (scale_x, scale_y);

    return true;
}

// ----------------------------------------------------------------------------
void Edit::OnEnterWindow(wxMouseEvent& event)
// ----------------------------------------------------------------------------
{
    event.Skip(); return;
//    // This is really annoying. CodeSnippetsTreeCtrl::ExecuteFrame()
//    // disables its parent to avoid the user closing the parent and leaving this
//    // window orphaned. But doing so causes the Scintilla editor to get focus
//    // with a hidden disabled cursor. So here, we enable both the editor and
//    // its frame.when the mouse enters the window.
//    // *Hack* *Hack* *Hack* *Hack*
//    // Enable this Edit window and its frame
//    LOGIT( _T("EVT_ENTER_WINDOW") );
//    wxWindow* pw = GetParent();
//    LOGIT( _T("parent[%s]Title[%s]"),pw->GetName().c_str(),pw->GetTitle().c_str() );
//    pw->Enable();
//    pw->SetFocus();
//    Enable();
//    SetFocus();
//    event.Skip();
}
// ----------------------------------------------------------------------------
void Edit::OnLeaveWindow(wxMouseEvent& event)
// ----------------------------------------------------------------------------
{
    event.Skip(); return;

//    // Disable the Snippet Tree Ctrl frame when leaving editor
//    // so user can't shut it down and leave us orphaned.
//    // *Hack* *Hack* *Hack* *Hack*
//
//    //LOGIT( _T("EVT_LEAVE_WINDOW") );
//    wxWindow* pw = GetParent();
//    //LOGIT( _T("parent1[%s]Title[%s]"),pw->GetName().c_str(),pw->GetTitle().c_str() );
//    if (pw && pw->GetParent()) //Edit frame
//    {   pw = pw->GetParent();
//        //pw->Disable();
//        //LOGIT( _T("parent2[%s]Title[%s]"),pw->GetName().c_str(),pw->GetTitle().c_str() );
//    }
//    if (pw && pw->GetParent())  // a panel
//    {   pw = pw->GetParent();
//        pw->Disable();          // This disables Snippet frame Whoop!
//        //LOGIT( _T("parent3[%s]Title[%s]"),pw->GetName().c_str(),pw->GetTitle().c_str() );
//    }
//    if (pw && pw->GetParent())  //Snippet tree frame
//    {   pw = pw->GetParent();
//        //pw->Disable(); This disables CodeBlocks
//        //LOGIT( _T("parent4[%s]Title[%s]"),pw->GetName().c_str(),pw->GetTitle().c_str() );
//    }
//    if (pw && pw->GetParent())  //CodeBlocks
//    {   pw = pw->GetParent();
//        //LOGIT( _T("parent5[%s]Title[%s]"),pw->GetName().c_str(),pw->GetTitle().c_str() );
//    }
//
//     event.Skip();
}
// ----------------------------------------------------------------------------
void Edit::OnScroll( wxScrollEvent& event )
// ----------------------------------------------------------------------------
{//(pecan 2007/4/05)
    // this event is from user set wxScrollBars
    event.Skip();
    if (event.GetOrientation() == wxVERTICAL) return;

    wxScrollBar *sb = (wxScrollBar*)event.GetEventObject();
    int pos   = event.GetPosition();
    int thumb = sb->GetThumbSize();
    //int range = sb->GetRange();
    int scroll_width = GetScrollWidth();

    // only check if at scroll end, wxEVT_SCROLL(WIN)_BOTTOM is not reliable
    if (pos + thumb >= scroll_width)
    {
        int longest_len = GetLongestLinePixelWidth();
        if (longest_len > scroll_width)
            SetScrollWidth(longest_len);
            //sb->SetScrollbar(pos, thumb, text_width, true);

        sb->Refresh();
    }
}
// ----------------------------------------------------------------------------
void Edit::OnScrollWin( wxScrollWinEvent& event )
// ----------------------------------------------------------------------------
{//(pecan 2007/4/05)

    // this event is from this window's builtin scrollbars
    event.Skip();
    if (event.GetOrientation() == wxVERTICAL) return;

    int pos   = event.GetPosition(); //GetScrollPos(wxHORIZONTAL);
    int thumb = GetScrollThumb(wxHORIZONTAL);
    //int range = GetScrollRange(wxHORIZONTAL);
    int scroll_width = GetScrollWidth();

    // only check if at scroll end, wxEVT_SCROLL(WIN)_BOTTOM is not reliable
    if (pos + thumb >= scroll_width)
    {
        int longest_len = GetLongestLinePixelWidth();
        if (longest_len > scroll_width)
            SetScrollWidth(longest_len);
            //SetScrollbar(wxHORIZONTAL, pos, thumb, text_width, true);
    }
}

// ----------------------------------------------------------------------------
void Edit::InitDragScroller()
// ----------------------------------------------------------------------------
{//(pecan 2007/4/05)

    pDragScroller = 0;
    wxWindow* p = this->GetParent();
    if (not p) return;
    pDragScroller = new DragScrollEvents( this );
}

// ----------------------------------------------------------------------------
//      MOUSE DRAG and SCROLL Routines
// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(DragScrollEvents, wxEvtHandler)
    // none, using connect/disconnect events instead
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
DragScrollEvents::DragScrollEvents(wxWindow *window)
// ----------------------------------------------------------------------------
{
    m_Window = window;
    m_pMS_Window = window->GetParent();
    m_DragMode       = DRAG_NONE;
    m_MouseHasMoved  = false;
    m_RatioX = 1; m_RatioY = 1;
    m_MouseMoveToLineMoveRatio    = 0.30;
    m_Direction      = -1;

    m_gtkContextDelay = 240 ;
    MouseDragScrollEnabled = true;
    MouseEditorFocusEnabled = true;
    MouseDragDirection      = 0;
    MouseDragKey            = 0;
    MouseDragSensitivity    = 5;
    MouseToLineRatio        = 30;
    //MouseRightKeyCtrl       = 0 ;
    MouseContextDelay       = 192;

    wxString CfgFilenameStr = GetConfig()->SettingsSnippetsCfgFullPath;
    LOGIT(_T("DragScroll Config Filename:[%s]"), CfgFilenameStr.c_str());
    // read configuaton file
    wxFileConfig cfgFile(wxEmptyString,     // appname
                        wxEmptyString,      // vendor
                        CfgFilenameStr,     // local filename
                        wxEmptyString,      // global file
                        wxCONFIG_USE_LOCAL_FILE);

    cfgFile.Read(_T("MouseDragScrollEnabled"),  &MouseDragScrollEnabled ) ;
	cfgFile.Read(_T("MouseEditorFocusEnabled"), &MouseEditorFocusEnabled ) ;
	cfgFile.Read(_T("MouseFocusEnabled"),       &MouseFocusEnabled ) ;
	cfgFile.Read(_T("MouseDragDirection"),      &MouseDragDirection ) ;
	cfgFile.Read(_T("MouseDragKey"),            &MouseDragKey ) ;
	cfgFile.Read(_T("MouseDragSensitivity"),    &MouseDragSensitivity ) ;
	cfgFile.Read(_T("MouseToLineRatio"),        &MouseToLineRatio ) ;
	//cfgFile.Read(_T("MouseRightKeyCtrl"),       &MouseRightKeyCtrl) ;

    #ifdef LOGGING
        LOGIT(_T("MouseDragScrollEnabled:%d"),  MouseDragScrollEnabled ) ;
        LOGIT(_T("MouseEditorFocusEnabled:%d"), MouseEditorFocusEnabled ) ;
        LOGIT(_T("MouseFocusEnabled:%d"),       MouseFocusEnabled ) ;
        LOGIT(_T("MouseDragDirection:%d"),      MouseDragDirection ) ;
        LOGIT(_T("MouseDragKey:%d"),            MouseDragKey ) ;
        LOGIT(_T("MouseDragSensitivity:%d"),    MouseDragSensitivity ) ;
        LOGIT(_T("MouseToLineRatio:%d"),        MouseToLineRatio ) ;
        //LOGIT(_T("MouseRightKeyCtrl:%d"),       MouseRightKeyCtrl ) ;
    #endif //LOGGING


    m_Window->Connect(wxEVT_MIDDLE_DOWN,
                    (wxObjectEventFunction)(wxEventFunction)
                    (wxMouseEventFunction)&DragScrollEvents::OnMouseEvent,
                     NULL, this);
    m_Window->Connect(wxEVT_MIDDLE_UP,
                    (wxObjectEventFunction)(wxEventFunction)
                    (wxMouseEventFunction)&DragScrollEvents::OnMouseEvent,
                     NULL, this);
    m_Window->Connect(wxEVT_RIGHT_DOWN,
                    (wxObjectEventFunction)(wxEventFunction)
                    (wxMouseEventFunction)&DragScrollEvents::OnMouseEvent,
                     NULL, this);
    m_Window->Connect(wxEVT_RIGHT_UP,
                    (wxObjectEventFunction)(wxEventFunction)
                    (wxMouseEventFunction)&DragScrollEvents::OnMouseEvent,
                     NULL, this);
    m_Window->Connect(wxEVT_MOTION,
                    (wxObjectEventFunction)(wxEventFunction)
                    (wxMouseEventFunction)&DragScrollEvents::OnMouseEvent,
                     NULL, this);
    m_Window->Connect(wxEVT_ENTER_WINDOW,
                    (wxObjectEventFunction)(wxEventFunction)
                    (wxMouseEventFunction)&DragScrollEvents::OnMouseEvent,
                     NULL, this);

    #if LOGGING
     LOGIT(_T("Edit:Attach Window[%p] named[%s] "), m_Window, m_Window->GetName().c_str());
    #endif

}

// ----------------------------------------------------------------------------
DragScrollEvents::~DragScrollEvents()
// ----------------------------------------------------------------------------
{
    #if LOGGING
     LOGIT(_T("DragScrollEvents dtor"));
    #endif
    #if LOGGING
    LOGIT(_T("Edit:Disconnecting DragScroller"));
    #endif
    if (m_Window)
    {
        m_Window->Disconnect(wxEVT_MIDDLE_DOWN,
                        (wxObjectEventFunction)(wxEventFunction)
                        (wxMouseEventFunction)&DragScrollEvents::OnMouseEvent,
                         NULL, this);
        m_Window->Disconnect(wxEVT_MIDDLE_UP,
                        (wxObjectEventFunction)(wxEventFunction)
                        (wxMouseEventFunction)&DragScrollEvents::OnMouseEvent,
                         NULL, this);
        m_Window->Disconnect(wxEVT_RIGHT_DOWN,
                        (wxObjectEventFunction)(wxEventFunction)
                        (wxMouseEventFunction)&DragScrollEvents::OnMouseEvent,
                         NULL, this);
        m_Window->Disconnect(wxEVT_RIGHT_UP,
                        (wxObjectEventFunction)(wxEventFunction)
                        (wxMouseEventFunction)&DragScrollEvents::OnMouseEvent,
                         NULL, this);
        m_Window->Disconnect(wxEVT_MOTION,
                        (wxObjectEventFunction)(wxEventFunction)
                        (wxMouseEventFunction)&DragScrollEvents::OnMouseEvent,
                         NULL, this);
        m_Window->Disconnect(wxEVT_ENTER_WINDOW,
                        (wxObjectEventFunction)(wxEventFunction)
                        (wxMouseEventFunction)&DragScrollEvents::OnMouseEvent,
                         NULL, this);
    }//if
    return;
}//dtor

///////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------------
//      MOUSE SCROLLING for __WXMSW__
// ----------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
#ifdef __WXMSW__
// ----------------------------------------------------------------------------
void DragScrollEvents::OnMouseEvent(wxMouseEvent& event)    //MSW
// ----------------------------------------------------------------------------
{
    //remember event window pointer
    m_pEvtObject = event.GetEventObject();
    DragScrollEvents* pDS = this;

    // Why is an event getting in here when this OS window doesnt have the focus
//     LOGIT( _T("ActiveWinow[%p][%s] m_Window[%p][%s]"),
//            ::wxGetActiveWindow(), ::wxGetActiveWindow()->GetName().c_str(),
//            m_Window, m_Window->GetName().c_str() );
    if (::wxGetActiveWindow() != pDS->m_pMS_Window)
        {event.Skip(); return;}
    // For efficiency, skip wheel events right now
    if ( event.GetEventType() ==  wxEVT_MOUSEWHEEL)
        { event.Skip(); return; }

//    //Returns the string form of the class name.
//    const wxChar* pClassName = 0;
//    if (m_pEvtObject)
//        pClassName = m_pEvtObject->GetClassInfo()->GetClassName();
//    #ifdef LOGGING
//     LOGIT( _T("ClassName[%s]"), pClassName );
//    #endif //LOGGING

    // differentialte window types
    Edit* p_cbStyledTextCtrl = 0;
    if ( ((wxWindow*)m_pEvtObject)->GetName().MakeLower() == wxT("sciwindow"))
        p_cbStyledTextCtrl = (Edit*)m_pEvtObject;

    // if "focus follows mouse" enabled, set focus to window
    if (pDS->GetMouseFocusEnabled() )
        if (event.GetEventType() ==  wxEVT_ENTER_WINDOW)
            if (m_pEvtObject) ((wxWindow*)m_pEvtObject)->SetFocus();

//    // set focus to editor window if mouse is in it
//    if (event.GetEventType() ==  wxEVT_ENTER_WINDOW)
//    {
//         //LOGIT( _T("EVT_ENTER_WINDOW:%p Styled:%p LeftSplit:%p RightSplit:%p"), m_pEvtObject, p_cbStyledTextCtrl, pLeftSplitWin, pRightSplitWin );
//        if (pDS->GetMouseEditorFocusEnabled() )
//        {  if (p_cbStyledTextCtrl && (m_pEvtObject ==  pLeftSplitWin))
//           {    pLeftSplitWin->SetFocus();
//                #ifdef LOGGING
//                 //LOGIT( _T("OnMouseEvent:SetFocus %p"), pLeftSplitWin );
//                #endif //LOGGING
//           }
//           if (pRightSplitWin && (m_pEvtObject ==  pRightSplitWin))
//           {    pRightSplitWin->SetFocus();
//                #ifdef LOGGING
//                 //LOGIT( _T("OnMouseEvent:SetFocus %p"), pRightSplitWin );
//                #endif //LOGGING
//           }
//        }
//    }

    int scrollx;
    int scrolly;

    #if LOGGING
    //LOGIT(_T("OnMouseEvent"));
    #endif

    if (KeyDown(event))
     {
            m_Direction = pDS->GetMouseDragDirection() ? 1 : -1 ; //v0.14
            m_MouseMoveToLineMoveRatio = pDS->GetMouseToLineRatio()/100.0;
            #ifdef LOGGING
             //LOGIT( _T("m_MouseMoveToLineMoveRatio %f"),m_MouseMoveToLineMoveRatio );
            #endif //LOGGING
            // We tentatively start dragging, but wait for
            // mouse movement before dragging properly.

            m_MouseHasMoved = false;
            //start position will change for each move
            m_StartY = event.GetPosition().y; m_StartX = event.GetPosition().x;
            //remember initial position for entire drag activity
            m_InitY = m_StartY; m_InitX = m_StartX;

            m_DragMode = DRAG_START;
            m_DragStartPos = event.GetPosition();
            #if LOGGING
             //LOGIT(_T("Down X:%d Y:%d"), m_InitY, m_InitX);
            #endif
//            // If hiding Right mouse keydown from ListCtrls, return v0.22
//            // RightMouseDown is causing an immediate selection in the control
//            // This stops it.
//            if (pDS->GetMouseRightKeyCtrl()) return;
            event.Skip(); //v0.21
            return;
     }

    else if (KeyUp(event) && (m_DragMode != DRAG_NONE) )
     {
        // Finish dragging
        int lastmode = m_DragMode;
        m_DragMode = DRAG_NONE;
        // if our trapped drag, hide event from others, ie. don't event.skip()
        #if LOGGING
         //LOGIT(_T("Up"));
        #endif
        if (lastmode ==  DRAG_DRAGGING) return;
        // allow context menu processing
        event.Skip();
        return;
     }

    else if ( event.Dragging() && (m_DragMode != DRAG_NONE ) )
     {
        //make sure user didnt leave client area and lift mouse key
        if ( not KeyIsDown(event))
         {  m_DragMode = DRAG_NONE;
            return;
         }

       //allow user some slop moves in case this is a "moving" context menu request
       if ( ! m_MouseHasMoved
            && abs(event.GetPosition().x - m_InitX) < 3
            && abs(event.GetPosition().y - m_InitY) < 3)
        {  return;}

        //+v0.6 code moved here to allow sloppier context menu requests
        if (m_DragMode == DRAG_START)
         {
            // Start the drag. This will stop the context popup
            #if LOGGING
            //LOGIT(_T("Drag_Start"));
            #endif
            m_DragMode = DRAG_DRAGGING;
         }

       m_MouseHasMoved = true;
       int dX = event.GetPosition().x - m_StartX;
       int dY = event.GetPosition().y - m_StartY;

      //show some sensitivity to speed of user mouse movements
      m_RatioX = m_RatioY = m_MouseMoveToLineMoveRatio;
      // build up some mouse movements to guarantee ratios won't cancel scrolling
      if ( (abs(dX)*m_RatioX >= 1) || (abs(dY)*m_RatioY >= 1) )
       { m_StartY = event.GetPosition().y; m_StartX = event.GetPosition().x;}

      //add one full mousemove for every x mouse positions beyond start position
      //so scrolling is faster as user is faster
      // slider values 1...2...3...4...5...6...7...8...9...10   //v0.14
      // divisn values 90  80  70  60 50   40  30  20  10   1
      int nThreshold = 1+( 100-(pDS->GetMouseDragSensitivity()*10) );
      m_RatioX += (abs(dX)/nThreshold);
      m_RatioY += (abs(dY)/nThreshold);

      // scroll the client area
      if (abs(dX) > abs(dY))
       {
          scrollx = int(dX * m_RatioX); scrolly = 0;
       }
      else
       {
            scrollx = 0; scrolly = int(dY * m_RatioY);
       }
        #if LOGGING
       //  LOGIT(_T("RatioX:%f RatioY:%f"), m_RatioX, m_RatioY);
       //  LOGIT(_T("Drag: dX:%d dY:%d scrollx:%d scrolly:%d"), dX, dY, scrollx, scrolly);
        #endif

        // Scroll horizontally and vertically.
        // void LineScroll (int columns, int lines);
        if ((scrollx==0) && (scrolly==0)) return;
        scrollx *= m_Direction; scrolly *= m_Direction;

        // if editor window, use scintilla scroll
        if (p_cbStyledTextCtrl && (m_pEvtObject == p_cbStyledTextCtrl))
        {
                p_cbStyledTextCtrl->LineScroll (scrollx,scrolly);
        }
        else //use control scrolling
        {
            //use wxTextCtrl scroll for y scrolling
            if ( scrolly)
                ((wxWindow*)m_pEvtObject)->ScrollLines(scrolly);
            else  // use listCtrl for x scrolling
                ((wxListCtrl*)m_pEvtObject)->ScrollList(scrollx,scrolly);
        }//esle
    }//else if

    // pass the event onward
    event.Skip();

}//OnMouseEvent
#endif //__WXMSW__ scroling

///////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------------
//      MOUSE SCROLLING __WXGTK__
// ----------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
#if defined(__WXGTK__) || defined(__WXMAC__)
// ----------------------------------------------------------------------------
void DragScrollEvents::OnMouseEvent(wxMouseEvent& event)    //GTK
// ----------------------------------------------------------------------------
{
    //remember event window pointer
    m_pEvtObject = event.GetEventObject();
    DragScrollEvents* pDS = this;

    // For efficiency, skip wheel events now
    if ( event.GetEventType() ==  wxEVT_MOUSEWHEEL)
        { event.Skip(); return; }

    #ifdef LOGGING
     //LOGIT( _T("m_MouseMoveToLineMoveRatio %f"),m_MouseMoveToLineMoveRatio );
    #endif //LOGGING

    // differentialte window types
    Edit* p_cbStyledTextCtrl = 0;
    if ( ((wxWindow*)m_pEvtObject)->GetName().MakeLower() == wxT("sciwindow"))
        p_cbStyledTextCtrl = (Edit*)m_pEvtObject;

    // if "focus follows mouse" endabled, set focus to window
    if (pDS->GetMouseFocusEnabled() )
        if (event.GetEventType() ==  wxEVT_ENTER_WINDOW)
            if (m_pEvtObject) ((wxWindow*)m_pEvtObject)->SetFocus();

     // set focus to editor window if mouse is in it
    if (event.GetEventType() ==  wxEVT_ENTER_WINDOW)
        if (pDS->GetMouseEditorFocusEnabled() )
           if (p_cbStyledTextCtrl && (m_pEvtObject == p_cbStyledTextCtrl))
                p_cbStyledTextCtrl->SetFocus();

    int scrollx;
    int scrolly;

    #if LOGGING
     //LOGIT(_T("OnMouseEvent"));
    #endif

    //--------- Key Down ------------------------------------------------------
    if (KeyDown(event))
     {
        m_Direction = pDS->GetMouseDragDirection() ? 1 : -1 ; //v0.14
        m_MouseMoveToLineMoveRatio = pDS->GetMouseToLineRatio()/100.0;

        // We tentatively start dragging, but wait for
        // mouse movement before dragging properly.

        m_MouseHasMoved = false;
        //start position will change for each move
        m_StartY = event.GetPosition().y; m_StartX = event.GetPosition().x;
        //remember initial position for entire drag activity
        m_InitY = m_StartY; m_InitX = m_StartX;

        m_DragMode = DRAG_NONE;
        m_DragStartPos = event.GetPosition();
        #if LOGGING
         LOGIT(_T("Down at  X:%d Y:%d"), m_InitX, m_InitY);
        #endif

        wxPoint mouseXY = ((wxWindow*)m_pEvtObject)->ScreenToClient(wxGetMousePosition());
        LOGIT(_T("Down MoveTo X:%d Y:%d"), mouseXY.x, mouseXY.y);

        // wait for possible mouse moves before poping context menu
        for (int i = 0; i < pDS->GetMouseContextDelay();)
        {
            ::wxMilliSleep(10);    // wait for move (if any)
            mouseXY = ((wxWindow*)m_pEvtObject)->ScreenToClient(wxGetMousePosition());
            scrollx = abs(mouseXY.x - m_InitX) ;
            scrolly = abs(mouseXY.y - m_InitY) ;
            if ( ( scrolly > 1) || (scrollx > 1) ) break;
            i += 10;
        }

        // capture middle mouse key for immediate dragging
        if ( (GetUserDragKey() ==  wxMOUSE_BTN_MIDDLE ) && event.MiddleIsDown() )
        {   m_DragMode = DRAG_START;
            return;
        }
        else // wait for movement if right mouse key; might be context menu request
        {
            #if LOGGING
             LOGIT(_T("Down delta x:%d y:%d"), scrollx, scrolly );
            #endif
            if (p_cbStyledTextCtrl && (m_pEvtObject == p_cbStyledTextCtrl) //v0.21
                && ( ( scrolly > 1) || (scrollx > 1) ))
            {   m_DragMode = DRAG_START;
                return;
            }
            // Since scrolling other types of windows doesnt work on GTK
            // just event.Skip()
            //else {  // listctrl windows ALWAYS report 24 pixel y move
            //        // when just hitting the mouse button.
            //    if ( (scrolly > 24) || (scrollx > 1))
            //    {   m_DragMode = DRAG_START;
            //        return;
            //    }
            //}//endelse
            else {  // listctrl windows ALWAYS report 24 pixel y move
                    // when just hitting the mouse button.
                if ( (scrolly > 1) || (scrollx > 1))
                {   m_DragMode = DRAG_START;
                    return;
                }
            }//endelse
        }//else wait for movement
        // --------------------------------
        // Dont do the following on Linux, it kills all context menus
        // --------------------------------
        //// If hiding Right mouse keydown from ListCtrls, return v0.22
        //// RightMouseDown is causing an immediate selection in the control
        //// This stops it.
        //if (pDS->GetMouseRightKeyCtrl()) return;
        //event.Skip(); //v0.21
        //return;

        //no mouse movements, so pass off to context menu processing
        event.Skip();
        return;
    }//fi (event.RightDown()
    //--------- Key UP -------------------------------------------------------
    else if (KeyUp(event) )
     {
        // Finish dragging
        int lastmode = m_DragMode;
        m_DragMode = DRAG_NONE;
        #if LOGGING
         LOGIT( _T("Up") ) ;
        #endif
        if (lastmode ==  DRAG_DRAGGING) return;
        // allow non-drag processing
        event.Skip();
        return;
     }//fi (event.RighUp)
    //--------- DRAGGING  -----------------------------------------------------
    else if ( (m_DragMode!=DRAG_NONE) && event.Dragging() ) //v0.12
    {

        //-LOGIT( _T("Dragging") ) ;
        //make sure user didnt leave client area and lift mouse key
        if ( not KeyIsDown(event))
         {  m_DragMode = DRAG_NONE;
            return;
         }

        ////allow user some slop moves in case this is a "moving" context menu request
        //if ( ! m_MouseHasMoved
        //    && abs(event.GetPosition().x - m_InitX) < 3
        //    && abs(event.GetPosition().y - m_InitY) < 3)
        //{  return;}
        //else m_DragMode = DRAG_START;//v0.12

        if (m_DragMode == DRAG_START)
         {
            // Start the drag. This will stop the context popup
            #if LOGGING
            LOGIT(_T("Drag_Start"));
            #endif
            m_DragMode = DRAG_DRAGGING;
         }

       m_MouseHasMoved = true;
       int dX = event.GetPosition().x - m_StartX;
       int dY = event.GetPosition().y - m_StartY;

      //set ration of mouse moves to lines scrolled (currently 30 percent)
      m_RatioX = m_RatioY = m_MouseMoveToLineMoveRatio;
      // build up some mouse movements to guarantee ratios won't cancel scrolling
      if ( (abs(dX)*m_RatioX >= 1) || (abs(dY)*m_RatioY >= 1) )
       { m_StartY = event.GetPosition().y; m_StartX = event.GetPosition().x;}

      //add one full mousemove for every x mouse positions beyond start position
      //so scrolling is faster as user is faster
      // slider values 1...2...3...4...5...6...7...8...9...10   //v0.14
      // divisn values 90  80  70  60  50  40  30  20  10   1
      int nThreshold = 1+( 100-(pDS->GetMouseDragSensitivity()*10) );
      m_RatioX += (abs(dX)/nThreshold);
      m_RatioY += (abs(dY)/nThreshold);

      // scroll the client area
      if (abs(dX) > abs(dY))
       {
          scrollx = int(dX * m_RatioX); scrolly = 0;
       }
      else
       {
            scrollx = 0; scrolly = int(dY * m_RatioY);
       }
        #if LOGGING
       //  LOGIT(_T("RatioX:%f RatioY:%f"), m_RatioX, m_RatioY);
       //  LOGIT(_T("Drag: dX:%d dY:%d scrollx:%d scrolly:%d"), dX, dY, scrollx, scrolly);
        #endif

        // Scroll horizontally and vertically.
        // void LineScroll (int columns, int lines);
        if ((scrollx==0) && (scrolly==0)) return;
        scrollx *= m_Direction; scrolly *= m_Direction;


        // if editor window, use scintilla scroll
        if (p_cbStyledTextCtrl && (m_pEvtObject == p_cbStyledTextCtrl))
        {
                p_cbStyledTextCtrl->LineScroll (scrollx,scrolly);
        }
        else //use control scrolling
        {
//            //Returns the string form of the class name.
//            const wxChar* pClassName = 0;
//            wxString classname;
//            if (m_pEvtObject)
//            {   pClassName = m_pEvtObject->GetClassInfo()->GetClassName();
//                classname = wxString(pClassName,10);
//                LOGIT( _T("ClassName[%s]"), classname.c_str() );
//            }
            // ---------------------------------
            //The following works in the BuildLog, but now the SearchResults
            // ---------------------------------
            //use wxTextCtrl scroll for y scrolling
            if ( scrolly )//&& (classname == wxT("wxTextCtrl")) )
            {   //LOGIT(wxT("ScrollText x:%d y:%d"),scrollx, scrolly );
                ((wxWindow*)m_pEvtObject)->ScrollLines(scrolly);
            }
            // Following does not work. GTK does not scroll wxListCtrl
            //else  // use listCtrl for x scrolling
            //{    LOGIT(wxT("ScrollList x:%d y:%d"),scrollx, scrolly );
            //    ((wxListCtrl*)m_pEvtObject)->ScrollList(scrollx,scrolly);
            //}
        }//esle
    }//esle fi (event.Dragging() && m_dragMode != DRAG_NONE)

    // pass on the event
    event.Skip();

}//OnMouseEvent
#endif //__WXGTK__ scrolling
// ----------------------------------------------------------------------------
//   end __WXGTK__ scrolling
// ----------------------------------------------------------------------------

