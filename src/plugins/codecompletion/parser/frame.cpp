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

#include "frame.h"
#include "finddlg.h"
#include "cclogger.h"
#include "token.h"
#include "tokenstree.h"

//(*InternalHeaders(Frame)
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include <wx/arrstr.h>
#include <wx/busyinfo.h>
#include <wx/filename.h>
#include <wx/textdlg.h>
#include <wx/tokenzr.h>

//(*IdInit(Frame)
const long Frame::wxID_TOKEN = wxNewId();
//*)

namespace ParserTestAppGlobal
{
    extern wxArrayString s_includeDirs;
    extern wxArrayString s_fileQueue;
    extern wxArrayString s_filesParsed;
}

int idCCLogger = wxNewId();

BEGIN_EVENT_TABLE(Frame, wxFrame)
    //(*EventTable(Frame)
    //*)
    EVT_FIND(wxID_ANY, Frame::OnFindDialog)
    EVT_FIND_NEXT(wxID_ANY, Frame::OnFindDialog)
    EVT_FIND_REPLACE(wxID_ANY, Frame::OnFindDialog)
    EVT_FIND_REPLACE_ALL(wxID_ANY, Frame::OnFindDialog)
    EVT_FIND_CLOSE(wxID_ANY, Frame::OnFindDialog)
    EVT_MENU(idCCLogger, Frame::OnCCLogger)
END_EVENT_TABLE()

Frame::Frame(const wxString& main_file) :
    m_BusyInfo(NULL),
    m_MainFile(main_file),
    m_LogCount(0),
    m_FRDlg(NULL)
{
    //(*Initialize(Frame)
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

    Create(0, wxID_ANY, _("Parser Testing"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUBAR));
    bsz_main = new wxBoxSizer(wxVERTICAL);
    bsz_misc = new wxBoxSizer(wxVERTICAL);
    bsz_include = new wxBoxSizer(wxVERTICAL);
    lbl_include = new wxStaticText(this, wxID_ANY, _("Add include directories to search for files here (one directory per line):"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    bsz_include->Add(lbl_include, 0, wxBOTTOM|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    m_IncludeCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_AUTO_SCROLL|wxTE_MULTILINE, wxDefaultValidator, _T("wxID_ANY"));
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
    m_LogCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_AUTO_SCROLL|wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH2|wxHSCROLL, wxDefaultValidator, _T("wxID_ANY"));
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
    m_TreeCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_AUTO_SCROLL|wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH2|wxHSCROLL, wxDefaultValidator, _T("wxID_ANY"));
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

    Connect(wxID_ANY,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&Frame::OnDoHeadersClick);
    Connect(wxID_OPEN,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Frame::OnMenuOpenSelected);
    Connect(wxID_REFRESH,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Frame::OnMenuReloadSelected);
    Connect(wxID_SAVE,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Frame::OnMenuSaveSelected);
    Connect(wxID_EXIT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Frame::OnMenuQuitSelected);
    Connect(wxID_FIND,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Frame::OnMenuFindSelected);
    Connect(wxID_TOKEN,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Frame::OnMenuTokenSelected);
    Connect(wxID_ABOUT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Frame::OnMenuAboutSelected);
    //*)

    ParserTest::Get()->Init();

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

    m_HeadersCtrl->SetValue(_T("<cstddef>,<w32api.h>,<wx/defs.h>,<wx/dlimpexp.h>,<wx/toplevel.h>,<boost/config.hpp>,<boost/filesystem/config.hpp>,\"pch.h\",\"sdk.h\",\"stdafx.h\""));

    CCLogger::Get()->Init(this, idCCLogger, idCCLogger);
    m_StatuBar->SetStatusText(_("Ready!"));
}

Frame::~Frame()
{
    //(*Destroy(Frame)
    //*)
    delete m_FRDlg;
}

void Frame::Start()
{
    ParserTestAppGlobal::s_includeDirs.Clear();
    ParserTestAppGlobal::s_fileQueue.Clear();
    ParserTestAppGlobal::s_filesParsed.Clear();

    // Obtain all include directories
    wxStringTokenizer tkz_inc(m_IncludeCtrl->GetValue(), _T("\r\n"));
    while ( tkz_inc.HasMoreTokens() )
    {
        wxString include = tkz_inc.GetNextToken().Trim(true).Trim(false);
        if (!include.IsEmpty())
            ParserTestAppGlobal::s_includeDirs.Add(include);
    }

    if (m_DoHeadersCtrl->IsChecked())
    {
        // Obtain all priority header files
        wxStringTokenizer tkz_hdr(m_HeadersCtrl->GetValue(), _T(","));
        while (tkz_hdr.HasMoreTokens())
        {
            wxString header = tkz_hdr.GetNextToken().Trim(false).Trim(true);
            if (header.Len() <= 2) // error, at least "" or <> is required
                continue;

            header = header.SubString(1, header.Len()-2).Trim(false).Trim(true);
            // Find the header files in include path's as provided
            // (practically the same as ParserBase::FindFileInIncludeDirs())
            for (size_t i=0; i<ParserTestAppGlobal::s_includeDirs.GetCount(); ++i)
            {
                // Normalize the path (as in NormalizePath())
                wxFileName f_header(header);
                f_header.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE, ParserTestAppGlobal::s_includeDirs[i]);
                header = f_header.GetFullPath();
                if ( ::wxFileExists(header) )
                    ParserTestAppGlobal::s_fileQueue.Add(header);
            }
        }
    }

    if (!m_MainFile.IsEmpty() && wxFileExists(m_MainFile))
        ParserTestAppGlobal::s_fileQueue.Add(m_MainFile);

    if (ParserTestAppGlobal::s_fileQueue.IsEmpty())
    {
        wxMessageBox(wxT("Main file not found. Nothing to do."),
                     _("Information"), wxOK | wxICON_INFORMATION, this);
        return;
    }

    m_BusyInfo = new wxBusyInfo(_T("Please wait, operating..."));

    m_LogCount = 0;
    m_LogCtrl->Clear();
    ParserTest::Get()->Clear(); // initial clearance

    AppendToLog(_T("--------------M-a-i-n--L-o-g--------------\r\n\r\n"));

    while (!ParserTestAppGlobal::s_fileQueue.IsEmpty())
    {
      wxString file = ParserTestAppGlobal::s_fileQueue.Item(0);
      ParserTestAppGlobal::s_fileQueue.Remove(file);
      if (file.IsEmpty()) continue;

      AppendToLog(_T("-----------I-n-t-e-r-i-m--L-o-g-----------"));

      if (m_BusyInfo) { delete m_BusyInfo; m_BusyInfo = 0; }
      m_BusyInfo = new wxBusyInfo(wxT("Please wait, operating '")+file+wxT("'..."));
      wxTheApp->Yield();

      m_StatuBar->SetStatusText(file);

      ParserTest::Get()->Start(file);
      ParserTestAppGlobal::s_filesParsed.Add(file); // done
    }

    if (m_BusyInfo) { delete m_BusyInfo; m_BusyInfo = 0; }
    m_BusyInfo = new wxBusyInfo(wxT("Please wait, creating tree log..."));
    wxTheApp->Yield();
    AppendToLog(_T("--------------T-r-e-e--L-o-g--------------\r\n"));
    ParserTest::Get()->PrintTree();

    if (m_BusyInfo) { delete m_BusyInfo; m_BusyInfo = 0; }
    m_BusyInfo = new wxBusyInfo(wxT("Please wait, creating list log..."));
    wxTheApp->Yield();
    AppendToLog(_T("--------------L-i-s-t--L-o-g--------------\r\n"));
    ParserTest::Get()->PrintList();

    if (m_DoTreeCtrl->IsChecked())
    {
      if (m_BusyInfo) { delete m_BusyInfo; m_BusyInfo = 0; }
      m_BusyInfo = new wxBusyInfo(wxT("Please wait, serializing tree..."));
      wxTheApp->Yield();

      Freeze();
      m_TreeCtrl->SetValue( ParserTest::Get()->SerializeTree() );
      Thaw();
    }

    if (m_BusyInfo) { delete m_BusyInfo; m_BusyInfo = 0; }

    if (ParserTest::Get()->GetTokensTree())
    {
        AppendToLog((wxString::Format(_("The parser contains %d tokens, found in %d files."),
                                      ParserTest::Get()->GetTokensTree()->size(),
                                      ParserTest::Get()->GetTokensTree()->m_FilesMap.size())));
    }
}

void Frame::AppendToLog(const wxString& log)
{
    Freeze();

    m_LogCtrl->AppendText( wxString::Format(_T("%06d. "), ++m_LogCount) );
    m_LogCtrl->AppendText( log );
    m_LogCtrl->AppendText( _T("\r\n") );

    m_LogCtrl->SetInsertionPoint(m_LogCtrl->GetLastPosition());

    Thaw();
}

void Frame::OnMenuQuitSelected(wxCommandEvent& event)
{
    Close();
}

void Frame::OnMenuAboutSelected(wxCommandEvent& event)
{
    wxString str;
    str.Printf(_("ParserTest build with %s!\nRunning under %s."),
               wxVERSION_STRING, wxGetOsDescription().c_str());
    wxMessageBox(str, _("About ParserTest"), wxOK | wxICON_INFORMATION, this);
}

void Frame::OnMenuSaveSelected(wxCommandEvent& event)
{
    if (m_SaveFile->ShowModal() == wxID_OK)
    {
        wxFile file(m_SaveFile->GetPath(), wxFile::write);
        file.Write(m_LogCtrl->GetValue());
    }
}

void Frame::OnMenuOpenSelected(wxCommandEvent& event)
{
    if (m_OpenFile->ShowModal() == wxID_OK)
    {
        m_MainFile = m_OpenFile->GetPath();
        Start();
    }
}

void Frame::OnMenuFindSelected(wxCommandEvent& event)
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

void Frame::OnMenuTokenSelected(wxCommandEvent& event)
{
    ParserBase* pb = ParserTest::Get()->GetParser();
    TokensTree* tt = ParserTest::Get()->GetTokensTree();
    if (!pb || !tt) return;

    wxTextEntryDialog dlg(this, _T("Enter name of token to debug:"), _T("ParserTest"));
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
            wxMessageBox(_("Token not found."), _("ParserTest"),
                         wxOK | wxICON_INFORMATION, this);
    }
}

void Frame::OnFindDialog(wxFindDialogEvent& event)
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

void Frame::OnMenuReloadSelected(wxCommandEvent& event)
{
    if (!ParserTestAppGlobal::s_fileQueue.IsEmpty())
        Start();
}

void Frame::OnDoHeadersClick(wxCommandEvent& event)
{
    if (m_HeadersCtrl)
        m_HeadersCtrl->Enable(event.IsChecked());
}

void Frame::OnCCLogger(wxCommandEvent& event)
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
