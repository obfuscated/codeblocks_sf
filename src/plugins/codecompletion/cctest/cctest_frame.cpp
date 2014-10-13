/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/file.h>
#endif

#include "ccdebuginfo.h"

#include "cctest_frame.h"
#include "cclogger.h"
#include "token.h"
#include "tokentree.h"

#include "nativeparser_test.h"

//(*InternalHeaders(CCTestFrame)
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include <wx/arrstr.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/font.h>
#include <wx/progdlg.h>
#include <wx/textdlg.h>
#include <wx/tokenzr.h>

//(*IdInit(CCTestFrame)
const long CCTestFrame::ID_CHK_HIDE = wxNewId();
const long CCTestFrame::wxID_TOKEN = wxNewId();
//*)

namespace CCTestAppGlobal
{
    extern wxArrayString s_includeDirs;
    extern wxArrayString s_fileQueue;
    extern wxArrayString s_filesParsed;
}// CCTestAppGlobal

int idCCLogger   = wxNewId();
int idCCAddToken = wxNewId();

const int C_FOLDING_MARGIN = 3; // as in C::B (fwiw...)

BEGIN_EVENT_TABLE(CCTestFrame, wxFrame)
    //(*EventTable(CCTestFrame)
    //*)
    EVT_FIND            (wxID_ANY,     CCTestFrame::OnFindDialog)
    EVT_FIND_NEXT       (wxID_ANY,     CCTestFrame::OnFindDialog)
    EVT_FIND_REPLACE    (wxID_ANY,     CCTestFrame::OnFindDialog)
    EVT_FIND_REPLACE_ALL(wxID_ANY,     CCTestFrame::OnFindDialog)
    EVT_FIND_CLOSE      (wxID_ANY,     CCTestFrame::OnFindDialog)
    EVT_MENU            (idCCLogger,   CCTestFrame::OnCCLogger  )
    EVT_MENU            (idCCAddToken, CCTestFrame::OnCCAddToken)
END_EVENT_TABLE()

CCTestFrame::CCTestFrame(const wxString& main_file) :
    m_ProgDlg(NULL),
    m_MainFile(main_file),
    m_CurrentFile(),
    m_LogCount(0),
    m_FRDlg(NULL)
{
    //(*Initialize(CCTestFrame)
    wxMenuItem* mnu_itm_save_log;
    wxMenuItem* mnu_item_find;
    wxBoxSizer* bsz_parser;
    wxBoxSizer* bszParserInput;
    wxBoxSizer* bsz_search_tree;
    wxPanel* panParserInput;
    wxPanel* panParserOutput;
    wxBoxSizer* bszParserSearchTree;
    wxBoxSizer* bszParserOutput;
    wxMenu* mnu_help;
    wxStaticText* lbl_include;
    wxMenuItem* mnu_itm_quit;
    wxButton* btnParse;
    wxBoxSizer* bsz_include;
    wxBoxSizer* bsz_misc;
    wxMenuItem* mnu_item_about;
    wxMenuBar* mnu_main;
    wxMenuItem* mnu_item_token;
    wxMenu* mnu_search;
    wxPanel* panParserSearchTree;
    wxMenuItem* mnu_itm_open;
    wxMenu* mnu_file;
    wxMenuItem* mnu_itm_reparse;
    wxBoxSizer* bsz_main;

    Create(0, wxID_ANY, _("CC Testing"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUBAR));
    bsz_main = new wxBoxSizer(wxVERTICAL);
    bsz_misc = new wxBoxSizer(wxVERTICAL);
    bsz_include = new wxBoxSizer(wxVERTICAL);
    lbl_include = new wxStaticText(this, wxID_ANY, _("Add include directories to search for files here (one directory per line):"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    bsz_include->Add(lbl_include, 0, wxBOTTOM|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    m_IncludeCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator, _T("wxID_ANY"));
    m_IncludeCtrl->SetMinSize(wxSize(-1,60));
    bsz_include->Add(m_IncludeCtrl, 0, wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    bsz_misc->Add(bsz_include, 0, wxBOTTOM|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    bsz_main->Add(bsz_misc, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    bsz_search_tree = new wxBoxSizer(wxHORIZONTAL);
    m_DoTreeCtrl = new wxCheckBox(this, wxID_ANY, _("Enable creation of parser\'s internal search tree (might get HUGE!!!)"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("wxID_ANY"));
    m_DoTreeCtrl->SetValue(false);
    bsz_search_tree->Add(m_DoTreeCtrl, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    m_DoHideCtrl = new wxCheckBox(this, ID_CHK_HIDE, _("Hide frame"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHK_HIDE"));
    m_DoHideCtrl->SetValue(true);
    bsz_search_tree->Add(m_DoHideCtrl, 0, wxLEFT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    bsz_search_tree->Add(-1,-1,1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    btnParse = new wxButton(this, wxID_ANY, _("Parse"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("wxID_ANY"));
    bsz_search_tree->Add(btnParse, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    btnPrintTree = new wxButton(this, wxID_ANY, _("Print Tree"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("wxID_ANY"));
    bsz_search_tree->Add(btnPrintTree, 1, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    bsz_main->Add(bsz_search_tree, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    bsz_parser = new wxBoxSizer(wxVERTICAL);
    m_ParserCtrl = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    panParserInput = new wxPanel(m_ParserCtrl, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("wxID_ANY"));
    bszParserInput = new wxBoxSizer(wxVERTICAL);
    m_Control = new wxScintilla(panParserInput,wxID_ANY,wxDefaultPosition,wxDefaultSize);
    bszParserInput->Add(m_Control, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    panParserInput->SetSizer(bszParserInput);
    bszParserInput->Fit(panParserInput);
    bszParserInput->SetSizeHints(panParserInput);
    panParserOutput = new wxPanel(m_ParserCtrl, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("wxID_ANY"));
    bszParserOutput = new wxBoxSizer(wxVERTICAL);
    m_LogCtrl = new wxTextCtrl(panParserOutput, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH2|wxHSCROLL, wxDefaultValidator, _T("wxID_ANY"));
    m_LogCtrl->SetMinSize(wxSize(640,250));
    bszParserOutput->Add(m_LogCtrl, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    panParserOutput->SetSizer(bszParserOutput);
    bszParserOutput->Fit(panParserOutput);
    bszParserOutput->SetSizeHints(panParserOutput);
    panParserSearchTree = new wxPanel(m_ParserCtrl, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("wxID_ANY"));
    bszParserSearchTree = new wxBoxSizer(wxVERTICAL);
    m_TreeCtrl = new wxTextCtrl(panParserSearchTree, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH2|wxHSCROLL, wxDefaultValidator, _T("wxID_ANY"));
    m_TreeCtrl->SetMinSize(wxSize(640,150));
    bszParserSearchTree->Add(m_TreeCtrl, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    panParserSearchTree->SetSizer(bszParserSearchTree);
    bszParserSearchTree->Fit(panParserSearchTree);
    bszParserSearchTree->SetSizeHints(panParserSearchTree);
    m_ParserCtrl->AddPage(panParserInput, _("Parser input"), true);
    m_ParserCtrl->AddPage(panParserOutput, _("Parser output"), false);
    m_ParserCtrl->AddPage(panParserSearchTree, _("Parser search tree"), false);
    bsz_parser->Add(m_ParserCtrl, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    bsz_main->Add(bsz_parser, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(bsz_main);
    mnu_main = new wxMenuBar();
    mnu_file = new wxMenu();
    mnu_itm_open = new wxMenuItem(mnu_file, wxID_OPEN, _("&Open...\tCtrl+O"), _("Open the source code to be tested"), wxITEM_NORMAL);
    mnu_file->Append(mnu_itm_open);
    mnu_itm_reparse = new wxMenuItem(mnu_file, wxID_REFRESH, _("&Re-parse\tF5"), _("Re-parse test file / buffer"), wxITEM_NORMAL);
    mnu_file->Append(mnu_itm_reparse);
    mnu_file->AppendSeparator();
    mnu_itm_save_log = new wxMenuItem(mnu_file, wxID_SAVE, _("&Save Log...\tCtrl+S"), _("Save log file to hard disk "), wxITEM_NORMAL);
    mnu_file->Append(mnu_itm_save_log);
    mnu_file->AppendSeparator();
    mnu_itm_quit = new wxMenuItem(mnu_file, wxID_EXIT, _("&Quit\tCtrl+Q"), wxEmptyString, wxITEM_NORMAL);
    mnu_file->Append(mnu_itm_quit);
    mnu_main->Append(mnu_file, _("&File"));
    mnu_search = new wxMenu();
    mnu_item_find = new wxMenuItem(mnu_search, wxID_FIND, _("&Find...\tCtrl+F"), _("Find keyword"), wxITEM_NORMAL);
    mnu_search->Append(mnu_item_find);
    mnu_item_token = new wxMenuItem(mnu_search, wxID_TOKEN, _("&Token...\tCtrl+T"), _("Find token"), wxITEM_NORMAL);
    mnu_search->Append(mnu_item_token);
    mnu_main->Append(mnu_search, _("&Search"));
    mnu_help = new wxMenu();
    mnu_item_about = new wxMenuItem(mnu_help, wxID_ABOUT, _("&About"), wxEmptyString, wxITEM_NORMAL);
    mnu_help->Append(mnu_item_about);
    mnu_main->Append(mnu_help, _("&Help"));
    SetMenuBar(mnu_main);
    m_StatuBar = new wxStatusBar(this, wxID_ANY, 0, _T("wxID_ANY"));
    int __wxStatusBarWidths_1[1] = { -10 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    m_StatuBar->SetFieldsCount(1,__wxStatusBarWidths_1);
    m_StatuBar->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(m_StatuBar);
    m_OpenFile = new wxFileDialog(this, _("Select Test Source File"), _("."), wxEmptyString, _("*.cpp;*.h"), wxFD_DEFAULT_STYLE, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    m_SaveFile = new wxFileDialog(this, _("Select file"), _("."), _("log.txt"), _("*.txt"), wxFD_SAVE, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    bsz_main->Fit(this);
    bsz_main->SetSizeHints(this);
    Center();

    Connect(wxID_ANY,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CCTestFrame::OnParse);
    Connect(wxID_ANY,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CCTestFrame::OnPrintTree);
    Connect(wxID_OPEN,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&CCTestFrame::OnMenuOpenSelected);
    Connect(wxID_REFRESH,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&CCTestFrame::OnMenuReparseSelected);
    Connect(wxID_SAVE,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&CCTestFrame::OnMenuSaveSelected);
    Connect(wxID_EXIT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&CCTestFrame::OnMenuQuitSelected);
    Connect(wxID_FIND,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&CCTestFrame::OnMenuFindSelected);
    Connect(wxID_TOKEN,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&CCTestFrame::OnMenuTokenSelected);
    Connect(wxID_ABOUT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&CCTestFrame::OnMenuAboutSelected);
    //*)

    // redirect the wxLogMessage to the text ctrl of the frame
    wxLogTextCtrl *textLog = new wxLogTextCtrl(m_LogCtrl);
    wxLog::SetActiveTarget(textLog);
#if wxCHECK_VERSION(2,9,0)
    wxLog::DisableTimestamp(); // do not show the time stamp
#else
    wxLog::SetTimestamp(NULL); // do not show the time stamp
#endif

    //Setting the macro replacements
    m_NativeParser.Init();

    // TODO: Make this base folders configurable
    wxString wx_base (_T("E:\\code\\cb\\wx\\wxWidgets-2.8.12\\"));
    wxString gcc_base(_T("E:\\code\\gcc\\pcxmingw463\\" ));
    wxString mingwver(_T("i686-w64-mingw32"));
    wxString gccver(_T("4.6.3"));

    m_IncludeCtrl->SetValue(wx_base  + _T("include")                                          + _T("\n") +
                            wx_base  + _T("lib\\gcc_dll\\mswu")                               + _T("\n") +
                            gcc_base + _T("lib\\gcc\\")+mingwver+_T("\\")+gccver+_T("\\include\\c++")               + _T("\n") +
                            gcc_base + _T("lib\\gcc\\")+mingwver+_T("\\")+gccver+_T("\\include\\c++\\") + mingwver  + _T("\n") +
                            gcc_base + _T("lib\\gcc\\")+mingwver+_T("\\")+gccver+_T("\\include\\c++\\backward")     + _T("\n") +
                            gcc_base + _T("lib\\gcc\\")+mingwver+_T("\\")+gccver+_T("\\include")                    + _T("\n") +
                            gcc_base + _T("include")                                                                + _T("\n") +
                            gcc_base + _T("lib\\gcc\\")+mingwver+_T("\\")+gccver+_T("\\include-fixed")              + _T("\n") +
                            gcc_base + mingwver + _T("\\include")                                                   + _T("\n"));


    CCLogger::Get()->Init(this, idCCLogger, idCCLogger, idCCAddToken);
    m_StatuBar->SetStatusText(_("Ready!"));

    InitControl();
    LoadToControl();
}

CCTestFrame::~CCTestFrame()
{
    //(*Destroy(CCTestFrame)
    //*)
    delete m_FRDlg;
}

void CCTestFrame::Start()
{
    if (m_ParserCtrl)
        m_ParserCtrl->SetSelection(1); // make sure "Output" tab is selected

    CCTestAppGlobal::s_includeDirs.Clear();
    CCTestAppGlobal::s_fileQueue.Clear();
    CCTestAppGlobal::s_filesParsed.Clear();

    // Obtain all include directories
    wxStringTokenizer tkz_inc(m_IncludeCtrl->GetValue(), _T("\r\n"));
    while ( tkz_inc.HasMoreTokens() )
    {
        wxString include = tkz_inc.GetNextToken().Trim(true).Trim(false);
        if (!include.IsEmpty())
            CCTestAppGlobal::s_includeDirs.Add(include);
    }

    if (m_DoHideCtrl && m_DoHideCtrl->IsChecked())
        Hide();

    m_ProgDlg = new wxProgressDialog(_T("Please wait, operating..."),
                                     _("Preparing...\nPlease wait..."), 0, this, wxPD_APP_MODAL);
    m_ProgDlg->SetSize(640,100);
    m_ProgDlg->Layout();
    m_ProgDlg->CenterOnParent();

    m_LogCount = 0;
    m_LogCtrl->Clear();

    m_NativeParser.Clear(); // initial clearance

    if (!m_MainFile.IsEmpty())
    {
        wxFileName fn(m_MainFile);
        fn.Normalize(); // cwd is used
        wxString absFilePath = fn.GetFullPath();
        CCTestAppGlobal::s_fileQueue.Add(absFilePath);
    }

    // scan the files with pattern "cc_*.cpp"
    wxArrayString testFiles;
    // if we only debug a single file, we use a file pattern ccc_*.cpp
    wxDir::GetAllFiles (wxGetCwd(),&testFiles,wxT("ccc_*.cpp"));
    if (testFiles.size() > 0)
        CCTestAppGlobal::s_fileQueue.Add(testFiles[0]);
    else // otherwise, we collect all the files with pattern cc_*.cpp
    {
        wxDir::GetAllFiles 	(wxGetCwd(),&testFiles,wxT("cc_*.cpp"));
        for (size_t i=0; i<testFiles.size(); i++)
            CCTestAppGlobal::s_fileQueue.Add(testFiles[i]);
    }

    AppendToLog(_T("--------------M-a-i-n--L-o-g--------------\r\n\r\n"));

    // parse file from the queue one-by-one
    while (!CCTestAppGlobal::s_fileQueue.IsEmpty())
    {
        wxString file = CCTestAppGlobal::s_fileQueue.Item(0);
        CCTestAppGlobal::s_fileQueue.Remove(file);
        if (file.IsEmpty()) continue;

        AppendToLog(_T("-----------I-n-t-e-r-i-m--L-o-g-----------"));
        m_CurrentFile = file;

        m_ProgDlg->Update(-1, m_CurrentFile);
        m_StatuBar->SetStatusText(m_CurrentFile);

        // parse the file and test the expression solving algorithm
        m_NativeParser.BatchTest(m_CurrentFile);

        CCTestAppGlobal::s_filesParsed.Add(m_CurrentFile); // done
    }

    if (m_ProgDlg) { delete m_ProgDlg; m_ProgDlg = 0; }

    if ( !IsShown() ) Show();
}

void CCTestFrame::AppendToLog(const wxString& log)
{
    Freeze();

    m_LogCtrl->AppendText( wxString::Format(_T("%06d. "), ++m_LogCount) );
    m_LogCtrl->AppendText( log );
    m_LogCtrl->AppendText( _T("\r\n") );

    m_LogCtrl->SetInsertionPoint(m_LogCtrl->GetLastPosition());

    Thaw();
}

void CCTestFrame::InitControl()
{
    m_Control->StyleClearAll();

    m_Control->SetLexer(wxSCI_LEX_CPP);

    m_Control->StyleSetForeground(wxSCI_C_DEFAULT,        wxColour(0, 0, 0));
    m_Control->StyleSetFontAttr  (wxSCI_C_DEFAULT,        10, wxT("Courier New"), false, false, false);

    m_Control->StyleSetForeground(wxSCI_C_COMMENT,        wxColour(160, 160, 160));
    m_Control->StyleSetFontAttr  (wxSCI_C_COMMENT,        10, wxT("Courier New"), false, false, false);

    m_Control->StyleSetForeground(wxSCI_C_COMMENTLINE,    wxColour(190, 190, 230));
    m_Control->StyleSetFontAttr  (wxSCI_C_COMMENTLINE,    10, wxT("Courier New"), false, false, false);

    m_Control->StyleSetForeground(wxSCI_C_COMMENTDOC,     wxColour(128, 128, 255));
    m_Control->StyleSetFontAttr  (wxSCI_C_COMMENTDOC,     10, wxT("Courier New"), false, false, false);

    m_Control->StyleSetForeground(wxSCI_C_NUMBER,         wxColour(240, 0, 240));
    m_Control->StyleSetFontAttr  (wxSCI_C_NUMBER,         10, wxT("Courier New"), false, false, false);

    m_Control->StyleSetForeground(wxSCI_C_WORD,           wxColour(0, 0, 160));
    m_Control->StyleSetFontAttr  (wxSCI_C_WORD,           10, wxT("Courier New"), false, false, false);

    m_Control->StyleSetForeground(wxSCI_C_STRING,         wxColour(0, 0, 255));
    m_Control->StyleSetFontAttr  (wxSCI_C_STRING,         10, wxT("Courier New"), false, false, false);

    m_Control->StyleSetForeground(wxSCI_C_CHARACTER,      wxColour(224, 160, 0));
    m_Control->StyleSetFontAttr  (wxSCI_C_CHARACTER,      10, wxT("Courier New"), false, false, false);

    m_Control->StyleSetForeground(wxSCI_C_UUID,           wxColour(0, 0, 0));
    m_Control->StyleSetFontAttr  (wxSCI_C_UUID,           10, wxT("Courier New"), false, false, false);

    m_Control->StyleSetForeground(wxSCI_C_PREPROCESSOR,   wxColour(0, 128, 0));
    m_Control->StyleSetFontAttr  (wxSCI_C_PREPROCESSOR,   10, wxT("Courier New"), false, false, false);

    m_Control->StyleSetForeground(wxSCI_C_OPERATOR,       wxColour(255, 0, 0));
    m_Control->StyleSetFontAttr  (wxSCI_C_OPERATOR,       10, wxT("Courier New"), false, false, false);

    m_Control->StyleSetForeground(wxSCI_C_IDENTIFIER,     wxColour(0, 0, 0));
    m_Control->StyleSetFontAttr  (wxSCI_C_IDENTIFIER,     10, wxT("Courier New"), false, false, false);

    m_Control->StyleSetForeground(wxSCI_C_STRINGEOL,      wxColour(0, 0, 255));
    m_Control->StyleSetFontAttr  (wxSCI_C_STRINGEOL,      10, wxT("Courier New"), false, false, false);

    m_Control->StyleSetForeground(wxSCI_C_COMMENTLINEDOC, wxColour(128, 128, 255));
    m_Control->StyleSetFontAttr  (wxSCI_C_COMMENTLINEDOC, 10, wxT("Courier New"), false, false, false);

    m_Control->StyleSetForeground(wxSCI_C_WORD2,          wxColour(0, 160, 0));
    m_Control->StyleSetFontAttr  (wxSCI_C_WORD2,          10, wxT("Courier New"), false, false, false);

    m_Control->StyleSetForeground(wxSCI_C_PREPROCESSORCOMMENT, wxColour(160, 160, 160));
    m_Control->StyleSetFontAttr  (wxSCI_C_PREPROCESSORCOMMENT, 10, wxT("Courier New"), false, false, false);

    const wxString kw(wxT(
    "_Char16_t _Char32_t align_union alignof asm auto bool break case catch char class const "
    "const_cast constexpr continue decltype default delete do double dynamic_cast else enum "
    "explicit export extern false final float for friend goto if import inline int long "
    "mutable namespace new nullptr operator override private protected public register "
    "reinterpret_cast return short signed sizeof static static_cast struct switch template "
    "this throw true try typedef typeid typename union unsigned using virtual void volatile "
    "wchar_t while static_assert int8_t uint8_t int16_t uint16_t int32_t uint32_t int64_t "
    "uint64_t int_least8_t uint_least8_t int_least16_t uint_least16_t int_least32_t "
    "uint_least32_t int_least64_t uint_least64_t int_fast8_t uint_fast8_t int_fast16_t "
    "uint_fast16_t int_fast32_t uint_fast32_t int_fast64_t uint_fast64_t intptr_t uintptr_t "
    "intmax_t uintmax_t wint_t wchar_t wctrans_t wctype_t size_t time_t and and_eq bitand "
    "bitor compl not not_eq or or_eq xor xor_eq"));

    m_Control->SetKeyWords(0, kw);

    const wxString kwStl(wxT(
    "basic_string bitset deque hash_map hash_multimap hash_multiset hash_set list map multimap "
    "multiset pair priority_queue queue set stack std string valarray vector"));

    m_Control->SetKeyWords(1, kwStl);

    m_Control->SetProperty(_T("fold"),              _T("1"));
    m_Control->SetProperty(_T("fold.html"),         _T("1"));
    m_Control->SetProperty(_T("fold.comment"),      _T("1"));
    m_Control->SetProperty(_T("fold.compact"),      _T("0"));
    m_Control->SetProperty(_T("fold.preprocessor"), _T("1"));

    m_Control->SetFoldFlags(16);
    m_Control->SetMarginType(C_FOLDING_MARGIN, wxSCI_MARGIN_SYMBOL);
    m_Control->SetMarginWidth(C_FOLDING_MARGIN, 16);
    // use "|" here or we might break plugins that use the margin (none at the moment)
    m_Control->SetMarginMask(C_FOLDING_MARGIN,
                             m_Control->GetMarginMask(C_FOLDING_MARGIN)
                             | (  wxSCI_MASK_FOLDERS
                                - (  (1 << wxSCI_MARKNUM_CHANGEUNSAVED)
                                   | (1 << wxSCI_MARKNUM_CHANGESAVED))) );
    m_Control->SetMarginSensitive(C_FOLDING_MARGIN, 1);

    wxColor f(0xff, 0xff, 0xff); // foreground colour
    wxColor b(0x80, 0x80, 0x80); // background colour
    SetMarkerStyle(wxSCI_MARKNUM_FOLDEROPEN,    wxSCI_MARK_MINUS,      f, b);
    SetMarkerStyle(wxSCI_MARKNUM_FOLDER,        wxSCI_MARK_PLUS,       f, b);
    SetMarkerStyle(wxSCI_MARKNUM_FOLDERSUB,     wxSCI_MARK_BACKGROUND, f, b);
    SetMarkerStyle(wxSCI_MARKNUM_FOLDERTAIL,    wxSCI_MARK_BACKGROUND, f, b);
    SetMarkerStyle(wxSCI_MARKNUM_FOLDEREND,     wxSCI_MARK_PLUS,       f, b);
    SetMarkerStyle(wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_MINUS,      f, b);
    SetMarkerStyle(wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_BACKGROUND, f, b);

    Connect(m_Control->GetId(), -1, wxEVT_SCI_MARGINCLICK,
            (wxObjectEventFunction) (wxEventFunction) (wxScintillaEventFunction)
            &CCTestFrame::OnMarginClick);
}

void CCTestFrame::SetMarkerStyle(int marker, int markerType, wxColor fore, wxColor back)
{
    m_Control->MarkerDefine(marker, markerType);
    m_Control->MarkerSetForeground(marker, fore);
    m_Control->MarkerSetBackground(marker, back);
}

void CCTestFrame::LoadToControl()
{
    if (!m_MainFile.IsEmpty() && !m_Control->LoadFile(m_MainFile))
    {
        // NOTE: comment out the below lines, since we can have m_MainFile empty
        //wxMessageBox(_("Could not load input file."), _("CCTest"),
        //             wxOK | wxICON_EXCLAMATION, this);
    }
}

void CCTestFrame::OnMenuQuitSelected(wxCommandEvent& /*event*/)
{
    Close();
}

void CCTestFrame::OnMenuSaveSelected(wxCommandEvent& /*event*/)
{
    if (m_SaveFile->ShowModal() == wxID_OK)
    {
        wxFile file(m_SaveFile->GetPath(), wxFile::write);
        file.Write(m_LogCtrl->GetValue());
    }
}

// load the select source file to wxScintilla control
void CCTestFrame::OnMenuOpenSelected(wxCommandEvent& /*event*/)
{
    if (m_OpenFile->ShowModal() == wxID_OK)
    {
        m_MainFile = m_OpenFile->GetPath();
        LoadToControl();
    }
}

void CCTestFrame::OnMenuFindSelected(wxCommandEvent& /*event*/)
{
    if (m_FRDlg != NULL)
    {
        delete m_FRDlg;
        m_FRDlg = NULL;
    }
    else
    {
        m_FRData.SetFlags(wxFR_DOWN);
        m_FRDlg = new wxFindReplaceDialog(this, &m_FRData, _("Find dialog"));
        m_FRDlg->Show(true);
    }
}

void CCTestFrame::OnMenuTokenSelected(wxCommandEvent& /*event*/)
{
    ParserBase* pb = &(m_NativeParser.m_Parser);
    TokenTree*  tt = m_NativeParser.m_Parser.GetTokenTree();
    if (!pb || !tt) return;
    wxTextEntryDialog dlg(this, _T("Enter name of token to debug:"), _T("CCTest"));
    if (dlg.ShowModal()==wxID_OK)
    {
        wxString target = dlg.GetValue().Trim(true).Trim(false);
        if (target.IsEmpty()) return;

        // Search over all tokens, no AI whatsoever available
        bool found = false;
        for (size_t i=0; i<tt->size(); ++i)
        {
            Token* t = tt->at(i);
            if (t && t->m_Name.Matches(target))
            {
                CCDebugInfo di(this, pb, t); di.ShowModal();
                found = true;
                break; // found, exit for-loop
            }
        }
        if (!found)
            wxMessageBox(_("Token not found."), _("CCTest"),
                         wxOK | wxICON_INFORMATION, this);
    }
}

void CCTestFrame::OnMenuReparseSelected(wxCommandEvent& /*event*/)
{
    Start();
}

void CCTestFrame::OnMenuAboutSelected(wxCommandEvent& /*event*/)
{
    wxString str;
    str.Printf(_("CCTest build with %s!\nRunning under %s."),
               wxVERSION_STRING, wxGetOsDescription().c_str());
    wxMessageBox(str, _("About CCTest"), wxOK | wxICON_INFORMATION, this);
}

void CCTestFrame::OnParse(wxCommandEvent& WXUNUSED(event))
{
    Start();
}

void CCTestFrame::OnMarginClick(wxScintillaEvent& event)
{
    if (event.GetMargin()==C_FOLDING_MARGIN)
    {
        int lineYpix = event.GetPosition();
        int line     = m_Control->LineFromPosition(lineYpix);

        m_Control->ToggleFold(line);
    }
}

void CCTestFrame::OnFindDialog(wxFindDialogEvent& event)
{
    wxEventType type = event.GetEventType();
    if (type == wxEVT_COMMAND_FIND || type == wxEVT_COMMAND_FIND_NEXT)
    {
        if (event.GetFlags() & wxFR_DOWN)
        {
            if (type == wxEVT_COMMAND_FIND)
            {
                m_LastIndex = m_LogCtrl->GetInsertionPoint();
                wxString tmp = m_LogCtrl->GetValue().SubString(m_LastIndex, m_LogCtrl->GetLastPosition() - 1);
                int i;
                if (event.GetFlags() & wxFR_MATCHCASE)
                    i = m_LogCtrl->GetValue().Find(event.GetFindString().c_str());
                else
                    i = tmp.Upper().Find(event.GetFindString().Upper().c_str());
                if (i >= 0)
                {
                    m_LastIndex += i;
                    m_LogCtrl->SetSelection(m_LastIndex, m_LastIndex + event.GetFindString().Length());
                }
            }
            else // find next
            {
                wxString tmp = m_LogCtrl->GetValue().SubString(++m_LastIndex, m_LogCtrl->GetLastPosition() - 1) ;
                int i;
                if (event.GetFlags() & wxFR_MATCHCASE)
                    i = tmp.Find(event.GetFindString().c_str());
                else
                    i = tmp.Upper().Find(event.GetFindString().Upper().c_str());
                if (i >= 0)
                {
                    m_LastIndex += i;
                    m_LogCtrl->SetSelection(m_LastIndex, m_LastIndex + event.GetFindString().Length());
                }
            }
            m_LogCtrl->SetFocus();
        }
        else //find up
        {
            if (type == wxEVT_COMMAND_FIND)
            {
                m_LastIndex = m_LogCtrl->GetInsertionPoint();
                int i;
                if (event.GetFlags() & wxFR_MATCHCASE)
                    i = m_LogCtrl->GetValue().rfind(event.GetFindString().c_str(), m_LastIndex);
                else
                    i = m_LogCtrl->GetValue().Upper().rfind(event.GetFindString().Upper().c_str(), m_LastIndex);

                if (i >= 0)
                {
                    m_LogCtrl->SetSelection(i, i + event.GetFindString().Length());
                    m_LastIndex = i;
                }
            }
            else
            {
                wxString tmp = m_LogCtrl->GetValue().SubString(0, --m_LastIndex) ;
                int i;
                if (event.GetFlags() & wxFR_MATCHCASE)
                    i = tmp.rfind(event.GetFindString().c_str(), m_LastIndex);
                else
                    i = tmp.Upper().rfind(event.GetFindString().Upper().c_str(), m_LastIndex);
                if (i >= 0)
                {
                    m_LastIndex = i;
                    m_LogCtrl->SetSelection(m_LastIndex, m_LastIndex + event.GetFindString().Length());
                }
            }
        }

        m_LogCtrl->SetFocus();
    }
    else if (type == wxEVT_COMMAND_FIND_CLOSE)
    {
        delete m_FRDlg;
        m_FRDlg = NULL;
    }
}

void CCTestFrame::OnCCLogger(wxCommandEvent& event)
{
    wxString log(event.GetString());

    for (size_t i = 0; i < log.Len(); ++i) // Convert '\r' to "\r", '\n' to "\n"
    {
        if (log.GetChar(i) == _T('\r'))
        {
            log.SetChar(i, _T('\\'));
            log.insert(++i, 1, _T('r'));
        }
        else if (log.GetChar(i) == _T('\n'))
        {
            log.SetChar(i, _T('\\'));
            log.insert(++i, 1, _T('n'));
        }
    }

    AppendToLog(log);
}

void CCTestFrame::OnCCAddToken(wxCommandEvent& event)
{
    wxString log(event.GetString());

    m_ProgDlg->Update(-1, m_CurrentFile + wxT("\n") + log);
}

void CCTestFrame::OnPrintTree(cb_unused wxCommandEvent& event)
{
    m_ProgDlg = new wxProgressDialog(_T("Please wait, operating..."), _("Preparing...\nPlease wait..."), 0, this, wxPD_APP_MODAL);
    m_ProgDlg->SetSize(640,100);
    m_ProgDlg->Layout();
    m_ProgDlg->CenterOnParent();
    // print tree information below

    m_ProgDlg->Update(-1, wxT("Creating tree log..."));
    AppendToLog(_T("--------------T-r-e-e--L-o-g--------------\r\n"));
    m_NativeParser.PrintTree();

    m_ProgDlg->Update(-1, wxT("Creating list log..."));
    AppendToLog(_T("--------------L-i-s-t--L-o-g--------------\r\n"));
    m_NativeParser.PrintList();

    if (m_DoTreeCtrl->IsChecked())
    {
        m_ProgDlg->Update(-1, wxT("Serializing tree..."));

        Freeze();
        m_TreeCtrl->SetValue( m_NativeParser.SerializeTree() );
        Thaw();
    }

    if (m_ProgDlg) { delete m_ProgDlg; m_ProgDlg = 0; }

    if ( !IsShown() ) Show();

    TokenTree* tt = m_NativeParser.m_Parser.GetTokenTree();
    if (tt)
    {
        AppendToLog((wxString::Format(_("The parser contains %lu tokens, found in %lu files."),
                                      static_cast<unsigned long>(tt->size()), static_cast<unsigned long>(tt->m_FileMap.size()))));
    }
}
