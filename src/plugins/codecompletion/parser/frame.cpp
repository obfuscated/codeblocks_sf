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

#include "frame.h"
#include "finddlg.h"
#include "cclogger.h"
#include "token.h"

//(*InternalHeaders(Frame)
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include <wx/arrstr.h>
#include <wx/busyinfo.h>
#include <wx/filename.h>
#include <wx/tokenzr.h>

//(*IdInit(Frame)
//*)

extern wxArrayString     s_includeDirs;
extern wxArrayString     s_filesParsed;
extern wxBusyInfo*       s_busyInfo;

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

Frame::Frame() : m_LogCount(0), m_DlgFind(NULL)
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
    wxBoxSizer* bsz_include;
    wxMenuItem* mnu_item_about;
    wxMenuBar* mnu_main;
    wxMenu* mnu_search;
    wxMenuItem* mnu_itm_open;
    wxMenu* mnu_file;
    wxBoxSizer* bsz_main;

    Create(0, wxID_ANY, _("Parser Testing"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUBAR));
    bsz_main = new wxBoxSizer(wxVERTICAL);
    bsz_include = new wxBoxSizer(wxVERTICAL);
    lbl_include = new wxStaticText(this, wxID_ANY, _("Add include directories to search for files here (one directory per line):"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    bsz_include->Add(lbl_include, 0, wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_IncludeCtrl = new wxTextCtrl(this, wxID_ANY, _("C:\\Devel\\wxWidgets\\lib\\gcc_dll\\mswu\nC:\\Devel\\wxWidgets\\include\n"), wxDefaultPosition, wxSize(-1,60), wxTE_AUTO_SCROLL|wxTE_MULTILINE, wxDefaultValidator, _T("wxID_ANY"));
    bsz_include->Add(m_IncludeCtrl, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    bsz_main->Add(bsz_include, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    bsz_parserlog = new wxBoxSizer(wxVERTICAL);
    lbl_log = new wxStaticText(this, wxID_ANY, _("The parser\'s log output:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    bsz_parserlog->Add(lbl_log, 0, wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_LogCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(640,350), wxTE_AUTO_SCROLL|wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH2|wxHSCROLL, wxDefaultValidator, _T("wxID_ANY"));
    bsz_parserlog->Add(m_LogCtrl, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    bsz_main->Add(bsz_parserlog, 1, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    bsz_search_tree = new wxBoxSizer(wxVERTICAL);
    bsz_search_tree_opts = new wxBoxSizer(wxHORIZONTAL);
    lbl_search_tree = new wxStaticText(this, wxID_ANY, _("The parser\'s internal search tree:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    bsz_search_tree_opts->Add(lbl_search_tree, 1, wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_DoTreeCtrl = new wxCheckBox(this, wxID_ANY, _("Enable creation (careful, might get HUGE!!!)"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("wxID_ANY"));
    m_DoTreeCtrl->SetValue(false);
    bsz_search_tree_opts->Add(m_DoTreeCtrl, 1, wxBOTTOM|wxLEFT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    bsz_search_tree->Add(bsz_search_tree_opts, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_TreeCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(640,200), wxTE_AUTO_SCROLL|wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH2|wxHSCROLL, wxDefaultValidator, _T("wxID_ANY"));
    bsz_search_tree->Add(m_TreeCtrl, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    bsz_main->Add(bsz_search_tree, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
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

    Connect(wxID_OPEN,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Frame::OnMenuOpenSelected);
    Connect(wxID_REFRESH,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Frame::OnMenuReloadSelected);
    Connect(wxID_SAVE,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Frame::OnMenuSaveSelected);
    Connect(wxID_EXIT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Frame::OnMenuQuitSelected);
    Connect(wxID_FIND,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Frame::OnMenuFindSelected);
    Connect(wxID_ABOUT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Frame::OnMenuAboutSelected);
    //*)

    CCLogger::Get()->Init(this, idCCLogger, idCCLogger);
    m_StatuBar->SetStatusText(_("Ready!"));
}

Frame::~Frame()
{
    //(*Destroy(Frame)
    //*)
    delete m_DlgFind;
}

void Frame::Start(const wxString& file)
{
    if (!wxFileName::FileExists(file))
        return;

    s_busyInfo = new wxBusyInfo(wxT("Please wait, operating '")+file+wxT("'..."));

    s_filesParsed.Clear();
    wxString          includes_ctrl = m_IncludeCtrl->GetValue();
    wxStringTokenizer tkz(includes_ctrl, wxT("\r\n"));
    s_includeDirs.Clear();
    while ( tkz.HasMoreTokens() )
    {
        wxString include = tkz.GetNextToken().Trim(true).Trim(false);
        if (!include.IsEmpty())
            s_includeDirs.Add(include);
    }
    m_LastFile = file;
    m_LogCount = 0;
    m_Log.Clear();
    m_LogCtrl->Clear();
    m_ParserTest.Clear();

    DoStart();

    if (s_busyInfo) { delete s_busyInfo; s_busyInfo = 0; }
}

void Frame::DoStart()
{
    m_StatuBar->SetStatusText(m_LastFile);
    m_Log += _T("--------------M-a-i-n--L-o-g--------------\r\n\r\n");
    m_ParserTest.Start(m_LastFile);
    m_Log += _T("\r\n\r\n--------------T-r-e-e--L-o-g--------------\r\n\r\n");
    m_ParserTest.PrintTree();
    m_Log += _T("\r\n\r\n--------------L-i-s-t--L-o-g--------------\r\n\r\n");
    m_ParserTest.PrintList();

    ShowLog();

    if (m_DoTreeCtrl->IsChecked())
    {
      Freeze();
      m_TreeCtrl->SetValue( m_ParserTest.SerializeTree() );
      Thaw();
    }
}

void Frame::ShowLog()
{
    Freeze();
    m_LogCtrl->SetValue(m_Log);
    m_LogCtrl->SetInsertionPoint(m_LogCtrl->GetLastPosition());
    Thaw();
}

void Frame::Log(const wxString& log)
{
    m_Log += wxString::Format(_T("%06d. "), ++m_LogCount);
    m_Log += log;
    m_Log += _T("\r\n");
}

void Frame::OnMenuQuitSelected(wxCommandEvent& event)
{
    Close();
}

void Frame::OnMenuAboutSelected(wxCommandEvent& event)
{
    wxString str;
    str.Printf(_("Parser Testing build with %s!\nRunning under %s."), wxVERSION_STRING,
               wxGetOsDescription().c_str());
    wxMessageBox(str, _("About Parser Testing"), wxOK | wxICON_INFORMATION, this);
}

void Frame::OnMenuSaveSelected(wxCommandEvent& event)
{
    if (m_SaveFile->ShowModal() == wxID_OK)
    {
        wxFile file(m_SaveFile->GetPath(), wxFile::write);
        file.Write(m_Log);
    }
}

void Frame::OnMenuOpenSelected(wxCommandEvent& event)
{
    if (m_OpenFile->ShowModal() == wxID_OK)
        Start(m_OpenFile->GetPath());
}

void Frame::OnMenuFindSelected(wxCommandEvent& event)
{
    if (m_DlgFind != NULL)
    {
        delete m_DlgFind;
        m_DlgFind = NULL;
    }
    else
    {
        m_FindData.SetFlags(wxFR_DOWN);
        m_DlgFind = new wxFindReplaceDialog(this, &m_FindData, _("Find dialog"));
        m_DlgFind->Show(true);
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
                    i = m_Log.Find(event.GetFindString().c_str());
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
        delete m_DlgFind;
        m_DlgFind = NULL;
    }
}

void Frame::OnMenuReloadSelected(wxCommandEvent& event)
{
    Start(m_LastFile);
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

    Log(log);
}
