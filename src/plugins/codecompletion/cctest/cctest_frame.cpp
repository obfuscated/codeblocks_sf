/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/file.h>
#endif

#include "ccdebuginfo.h"

#include "cctest_frame.h"
#include "finddlg.h"
#include "cclogger.h"
#include "token.h"
#include "tokenstree.h"

#include "nativeparser_test.h"

//(*InternalHeaders(CCTestFrame)
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include <wx/arrstr.h>
#include <wx/filename.h>
#include <wx/progdlg.h>
#include <wx/textdlg.h>
#include <wx/tokenzr.h>

//(*IdInit(CCTestFrame)
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
    wxStaticText* lbl_search_tree;
    wxMenuItem* mnu_item_find;
    wxBoxSizer* bsz_search_tree;
    wxStaticText* lbl_log;
    wxMenuItem* mnu_itm_reload;
    wxMenu* mnu_help;
    wxBoxSizer* bsz_parserlog;
    wxBoxSizer* bsz_search_tree_opts;
    wxStaticText* lbl_include;
    wxMenuItem* mnu_itm_quit;
    wxBoxSizer* bsz_headers;
    wxBoxSizer* bsz_include;
    wxBoxSizer* bsz_misc;
    wxMenuItem* mnu_item_about;
    wxMenuBar* mnu_main;
    wxMenuItem* mnu_item_token;
    wxMenu* mnu_search;
    wxMenuItem* mnu_itm_open;
    wxMenu* mnu_file;
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
    bsz_headers = new wxBoxSizer(wxVERTICAL);
    m_DoHeadersCtrl = new wxCheckBox(this, wxID_ANY, _("Parse the following priority files/headers first (colon separated):"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("wxID_ANY"));
    m_DoHeadersCtrl->SetValue(false);
    bsz_headers->Add(m_DoHeadersCtrl, 0, wxBOTTOM|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    m_HeadersCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("wxID_ANY"));
    m_HeadersCtrl->Disable();
    bsz_headers->Add(m_HeadersCtrl, 0, wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    bsz_misc->Add(bsz_headers, 0, wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    bsz_main->Add(bsz_misc, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    bsz_parserlog = new wxBoxSizer(wxVERTICAL);
    lbl_log = new wxStaticText(this, wxID_ANY, _("The parser\'s log output:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    bsz_parserlog->Add(lbl_log, 0, wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_LogCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH2|wxHSCROLL, wxDefaultValidator, _T("wxID_ANY"));
    m_LogCtrl->SetMinSize(wxSize(640,250));
    bsz_parserlog->Add(m_LogCtrl, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    bsz_main->Add(bsz_parserlog, 2, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    bsz_search_tree = new wxBoxSizer(wxVERTICAL);
    bsz_search_tree_opts = new wxBoxSizer(wxHORIZONTAL);
    lbl_search_tree = new wxStaticText(this, wxID_ANY, _("The parser\'s internal search tree:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    bsz_search_tree_opts->Add(lbl_search_tree, 1, wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_DoTreeCtrl = new wxCheckBox(this, wxID_ANY, _("Enable creation (careful, might get HUGE!!!)"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("wxID_ANY"));
    m_DoTreeCtrl->SetValue(false);
    bsz_search_tree_opts->Add(m_DoTreeCtrl, 1, wxBOTTOM|wxLEFT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    bsz_search_tree->Add(bsz_search_tree_opts, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_TreeCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH2|wxHSCROLL, wxDefaultValidator, _T("wxID_ANY"));
    m_TreeCtrl->SetMinSize(wxSize(640,150));
    bsz_search_tree->Add(m_TreeCtrl, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    bsz_main->Add(bsz_search_tree, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(bsz_main);
    mnu_main = new wxMenuBar();
    mnu_file = new wxMenu();
    mnu_itm_open = new wxMenuItem(mnu_file, wxID_OPEN, _("&Open...\tCtrl+O"), _("Open the source code to be tested"), wxITEM_NORMAL);
    mnu_file->Append(mnu_itm_open);
    mnu_itm_reload = new wxMenuItem(mnu_file, wxID_REFRESH, _("&Reload\tF5"), _("Reload test file"), wxITEM_NORMAL);
    mnu_file->Append(mnu_itm_reload);
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
    m_SaveFile = new wxFileDialog(this, _("Select file"), _("."), _("log.txt"), _("*.txt"), wxFD_DEFAULT_STYLE|wxFD_SAVE, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    bsz_main->Fit(this);
    bsz_main->SetSizeHints(this);
    Center();

    Connect(wxID_ANY,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CCTestFrame::OnDoHeadersClick);
    Connect(wxID_OPEN,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&CCTestFrame::OnMenuOpenSelected);
    Connect(wxID_REFRESH,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&CCTestFrame::OnMenuReloadSelected);
    Connect(wxID_SAVE,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&CCTestFrame::OnMenuSaveSelected);
    Connect(wxID_EXIT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&CCTestFrame::OnMenuQuitSelected);
    Connect(wxID_FIND,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&CCTestFrame::OnMenuFindSelected);
    Connect(wxID_TOKEN,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&CCTestFrame::OnMenuTokenSelected);
    Connect(wxID_ABOUT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&CCTestFrame::OnMenuAboutSelected);
    //*)

    //Setting the macro replacements
    CCTest::Get()->Init();

    // TODO: Make this base folders configurable
    wxString wx_base (_T("C:\\Devel\\wxWidgets\\"));
    wxString gcc_base(_T("C:\\Devel\\GCC46TDM\\" ));

    m_IncludeCtrl->SetValue(wx_base  + _T("include")                                          + _T("\n") +
                            wx_base  + _T("lib\\gcc_dll\\mswu")                               + _T("\n") +
                            gcc_base + _T("lib\\gcc\\mingw32\\4.6.1\\include\\c++")           + _T("\n") +
                            gcc_base + _T("lib\\gcc\\mingw32\\4.6.1\\include\\c++\\mingw32")  + _T("\n") +
                            gcc_base + _T("lib\\gcc\\mingw32\\4.6.1\\include\\c++\\backward") + _T("\n") +
                            gcc_base + _T("lib\\gcc\\mingw32\\4.6.1\\include")                + _T("\n") +
                            gcc_base + _T("include")                                          + _T("\n") +
                            gcc_base + _T("lib\\gcc\\mingw32\\4.6.1\\include-fixed")          + _T("\n"));

    m_HeadersCtrl->SetValue(_T("<_mingw.h>,<cstddef>,<w32api.h>,<winbase.h>,<wx/defs.h>,<wx/dlimpexp.h>,<wx/toplevel.h>,<boost/config.hpp>,<boost/filesystem/config.hpp>,\"pch.h\",\"sdk.h\",\"stdafx.h\""));

    CCLogger::Get()->Init(this, idCCLogger, idCCLogger, idCCAddToken);
    m_StatuBar->SetStatusText(_("Ready!"));
}

CCTestFrame::~CCTestFrame()
{
    //(*Destroy(CCTestFrame)
    //*)
    delete m_FRDlg;
}

void CCTestFrame::Start()
{
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

    if (m_DoHeadersCtrl->IsChecked())
    {
        // Obtain all priority header files
        wxStringTokenizer tkz_hdr(m_HeadersCtrl->GetValue(), _T(","));
        while (tkz_hdr.HasMoreTokens())
        {
            wxString header = tkz_hdr.GetNextToken().Trim(false).Trim(true);

            // Remove <> (if any)
            int lt = header.Find(wxT('<')); int gt = header.Find(wxT('>'),true);
            if (lt!=wxNOT_FOUND && gt!=wxNOT_FOUND && gt>lt)
                header = header.AfterFirst(wxT('<')).BeforeLast(wxT('>'));
            // Remove "" (if any)
            int oq = header.Find(wxT('"')); int cq = header.Find(wxT('"'),true);
            if (oq!=wxNOT_FOUND && cq!=wxNOT_FOUND && cq>oq)
                header = header.AfterFirst(wxT('"')).BeforeLast(wxT('"'));

            header = header.Trim(false).Trim(true);

            // Find the header files in include path's as provided
            // (practically the same as ParserBase::FindFileInIncludeDirs())
            for (size_t i=0; i<CCTestAppGlobal::s_includeDirs.GetCount(); ++i)
            {
                // Normalize the path (as in C::B's "NormalizePath()")
                wxFileName f_header(header);
                wxString   base_path(CCTestAppGlobal::s_includeDirs[i]);
                if (f_header.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE, base_path))
                {
                    wxString this_header = f_header.GetFullPath();
                    if ( ::wxFileExists(this_header) )
                        CCTestAppGlobal::s_fileQueue.Add(this_header);
                }
            }
        }
    }

    if (!m_MainFile.IsEmpty() && wxFileExists(m_MainFile))
        CCTestAppGlobal::s_fileQueue.Add(m_MainFile);

    if (CCTestAppGlobal::s_fileQueue.IsEmpty())
    {
        wxMessageBox(wxT("Main file not found. Nothing to do."),
                     _("Information"), wxOK | wxICON_INFORMATION, this);
        return;
    }

    Hide();

    m_ProgDlg = new wxProgressDialog(_T("Please wait, operating..."), _("Preparing...\nPlease wait..."), 0, this, wxPD_APP_MODAL);
    m_ProgDlg->SetSize(640,100);
    m_ProgDlg->Layout();
    m_ProgDlg->CenterOnParent();

    m_LogCount = 0;
    m_LogCtrl->Clear();
    CCTest::Get()->Clear(); // initial clearance

    AppendToLog(_T("--------------M-a-i-n--L-o-g--------------\r\n\r\n"));

    while (!CCTestAppGlobal::s_fileQueue.IsEmpty())
    {
      wxString file = CCTestAppGlobal::s_fileQueue.Item(0);
      CCTestAppGlobal::s_fileQueue.Remove(file);
      if (file.IsEmpty()) continue;

      AppendToLog(_T("-----------I-n-t-e-r-i-m--L-o-g-----------"));
      m_CurrentFile = file;

      m_ProgDlg->Update(-1, m_CurrentFile);
      m_StatuBar->SetStatusText(m_CurrentFile);

      // This is the core parse stage
      CCTest::Get()->Start(m_CurrentFile);
      CCTestAppGlobal::s_filesParsed.Add(m_CurrentFile); // done
    }

    m_ProgDlg->Update(-1, wxT("Creating tree log..."));
    AppendToLog(_T("--------------T-r-e-e--L-o-g--------------\r\n"));
    CCTest::Get()->PrintTree();

    m_ProgDlg->Update(-1, wxT("Creating list log..."));
    AppendToLog(_T("--------------L-i-s-t--L-o-g--------------\r\n"));
    CCTest::Get()->PrintList();

    if (m_DoTreeCtrl->IsChecked())
    {
      m_ProgDlg->Update(-1, wxT("Serializing tree..."));

      Freeze();
      m_TreeCtrl->SetValue( CCTest::Get()->SerializeTree() );
      Thaw();
    }

    // Here we are going to test the expression solving algorithm

    NativeParserTest nativeParserTest;

    wxString exp = _T("obj.m_Member1");

    TokenIdxSet searchScope;
    searchScope.insert(-1);

    TokenIdxSet result;

    TokensTree *tree = CCTest::Get()->GetTokensTree();

    nativeParserTest.TestExpression(exp,
                                    tree,
                                    searchScope,
                                    result );

    wxLogMessage(_T("Result have %d matches"), result.size());


    for (TokenIdxSet::iterator it=result.begin(); it!=result.end(); ++it)
    {
        Token* token = tree->at(*it);
        if (token)
        {
            wxString log;
            log << token->GetTokenKindString() << _T(" ")
                << token->DisplayName()        << _T("\t[")
                << token->m_Line               << _T(",")
                << token->m_ImplLine           << _T("]");
            CCLogger::Get()->Log(log);
        }
    }


    if (m_ProgDlg) { delete m_ProgDlg; m_ProgDlg = 0; }

    Show();

    TokensTree* tt = CCTest::Get()->GetTokensTree();
    if (tt)
        AppendToLog((wxString::Format(_("The parser contains %d tokens, found in %d files."),
                                      tt->size(), tt->m_FilesMap.size())));
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

void CCTestFrame::OnMenuQuitSelected(wxCommandEvent& /*event*/)
{
    Close();
}

void CCTestFrame::OnMenuAboutSelected(wxCommandEvent& /*event*/)
{
    wxString str;
    str.Printf(_("CCTest build with %s!\nRunning under %s."),
               wxVERSION_STRING, wxGetOsDescription().c_str());
    wxMessageBox(str, _("About CCTest"), wxOK | wxICON_INFORMATION, this);
}

void CCTestFrame::OnMenuSaveSelected(wxCommandEvent& /*event*/)
{
    if (m_SaveFile->ShowModal() == wxID_OK)
    {
        wxFile file(m_SaveFile->GetPath(), wxFile::write);
        file.Write(m_LogCtrl->GetValue());
    }
}

void CCTestFrame::OnMenuOpenSelected(wxCommandEvent& /*event*/)
{
    if (m_OpenFile->ShowModal() == wxID_OK)
    {
        m_MainFile = m_OpenFile->GetPath();
        Start();
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
    ParserBase* pb = CCTest::Get()->GetParser();
    TokensTree* tt = CCTest::Get()->GetTokensTree();
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

void CCTestFrame::OnMenuReloadSelected(wxCommandEvent& /*event*/)
{
    Start();
}

void CCTestFrame::OnDoHeadersClick(wxCommandEvent& event)
{
    if (m_HeadersCtrl)
        m_HeadersCtrl->Enable(event.IsChecked());
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
