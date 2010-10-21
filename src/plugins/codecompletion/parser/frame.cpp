/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#include <wx/file.h>
#endif

#include "frame.h"
#include "finddlg.h"

//(*InternalHeaders(Frame)
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include <wx/filename.h>

//(*IdInit(Frame)
const long Frame::ID_LOGMAIN = wxNewId();
const long Frame::ID_OPEN = wxNewId();
const long Frame::ID_RELOAD = wxNewId();
const long Frame::ID_SAVE = wxNewId();
const long Frame::ID_QUIT = wxNewId();
const long Frame::ID_FIND = wxNewId();
const long Frame::ID_ABOUT = wxNewId();
const long Frame::ID_STATUSBAR = wxNewId();
//*)

BEGIN_EVENT_TABLE(Frame, wxFrame)
    //(*EventTable(Frame)
    //*)
    EVT_FIND(wxID_ANY, Frame::OnFindDialog)
    EVT_FIND_NEXT(wxID_ANY, Frame::OnFindDialog)
    EVT_FIND_REPLACE(wxID_ANY, Frame::OnFindDialog)
    EVT_FIND_REPLACE_ALL(wxID_ANY, Frame::OnFindDialog)
    EVT_FIND_CLOSE(wxID_ANY, Frame::OnFindDialog)
END_EVENT_TABLE()

Frame::Frame() : m_logCnt(0), m_dlgFind(NULL)
{
    //(*Initialize(Frame)
    wxMenuItem* MenuItem5;
    wxMenuItem* MenuItem2;
    wxMenu* Menu3;
    wxBoxSizer* sizer;
    wxMenuItem* MenuItem1;
    wxMenuItem* MenuItem4;
    wxMenu* Menu1;
    wxMenuItem* MenuItem3;
    wxMenuItem* MenuItem6;
    wxMenuBar* MenuBar1;
    wxMenu* Menu2;

    Create(0, wxID_ANY, _("Parser Testing"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUBAR));
    sizer = new wxBoxSizer(wxHORIZONTAL);
    m_logCtrl = new wxTextCtrl(this, ID_LOGMAIN, wxEmptyString, wxDefaultPosition, wxSize(900,550), wxTE_MULTILINE|wxTE_READONLY|wxHSCROLL|wxTE_RICH2, wxDefaultValidator, _T("ID_LOGMAIN"));
    sizer->Add(m_logCtrl, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(sizer);
    MenuBar1 = new wxMenuBar();
    Menu2 = new wxMenu();
    MenuItem3 = new wxMenuItem(Menu2, ID_OPEN, _("&Open...\tCtrl+O"), _("Open the source code to be tested"), wxITEM_NORMAL);
    Menu2->Append(MenuItem3);
    MenuItem6 = new wxMenuItem(Menu2, ID_RELOAD, _("&Reload\tF5"), _("Reload test file"), wxITEM_NORMAL);
    Menu2->Append(MenuItem6);
    Menu2->AppendSeparator();
    MenuItem1 = new wxMenuItem(Menu2, ID_SAVE, _("&Save Log...\tCtrl+S"), _("Save log file to hard disk "), wxITEM_NORMAL);
    Menu2->Append(MenuItem1);
    Menu2->AppendSeparator();
    MenuItem2 = new wxMenuItem(Menu2, ID_QUIT, _("&Quit\tCtrl+Q"), wxEmptyString, wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("&File"));
    Menu1 = new wxMenu();
    MenuItem4 = new wxMenuItem(Menu1, ID_FIND, _("&Find...\tCtrl+F"), _("Find keyword"), wxITEM_NORMAL);
    Menu1->Append(MenuItem4);
    MenuBar1->Append(Menu1, _("&Search"));
    Menu3 = new wxMenu();
    MenuItem5 = new wxMenuItem(Menu3, ID_ABOUT, _("&About"), wxEmptyString, wxITEM_NORMAL);
    Menu3->Append(MenuItem5);
    MenuBar1->Append(Menu3, _("&Help"));
    SetMenuBar(MenuBar1);
    m_statuBar = new wxStatusBar(this, ID_STATUSBAR, 0, _T("ID_STATUSBAR"));
    int __wxStatusBarWidths_1[1] = { -10 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    m_statuBar->SetFieldsCount(1,__wxStatusBarWidths_1);
    m_statuBar->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(m_statuBar);
    m_openFile = new wxFileDialog(this, _("Select Test Source File"), _("."), wxEmptyString, _("*.cpp;*.h"), wxFD_DEFAULT_STYLE, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    m_saveFile = new wxFileDialog(this, _("Select file"), _("."), _("log.txt"), _("*.txt"), wxFD_DEFAULT_STYLE|wxFD_SAVE, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    sizer->Fit(this);
    sizer->SetSizeHints(this);
    Center();

    Connect(ID_OPEN,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Frame::OnMenuOpenSelected);
    Connect(ID_RELOAD,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Frame::OnMenuReloadSelected);
    Connect(ID_SAVE,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Frame::OnMenuSaveSelected);
    Connect(ID_QUIT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Frame::OnMenuQuitSelected);
    Connect(ID_FIND,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Frame::OnMenuFindSelected);
    Connect(ID_ABOUT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Frame::OnMenuAboutSelected);
    //*)

    m_statuBar->SetStatusText(_("Ready!"));
}

Frame::~Frame()
{
    //(*Destroy(Frame)
    //*)
    if (m_dlgFind != NULL)
    {
        delete m_dlgFind;
    }
}

void Frame::Start(const wxString& file)
{
    if (!wxFileName::FileExists(file))
        return;

    m_lastFile = file;
    m_logCnt = 0;
    m_log.Clear();
    m_logCtrl->Clear();
    m_parserTest.Clear();
    DoStart();
}

void Frame::DoStart()
{
    m_statuBar->SetStatusText(m_lastFile);
    m_log += _T("--------------M-a-i-n--L-o-g--------------\r\n\r\n");
    m_parserTest.Start(m_lastFile);
    m_log += _T("\r\n\r\n--------------T-r-e-e--L-o-g--------------\r\n\r\n");
    m_parserTest.PrintTree();
    m_log += _T("\r\n\r\n--------------L-i-s-t--L-o-g--------------\r\n\r\n");
    m_parserTest.PrintList();
    ShowLog();
}

void Frame::ShowLog()
{
    m_logCtrl->SetValue(m_log);
    Freeze();
    m_logCtrl->ScrollLines(m_logCtrl->GetLastPosition());
    Thaw();
}

void Frame::Log(const wxString& log)
{
    m_log += wxString::Format(_T("%06d. "), ++m_logCnt);
    m_log += log;
    m_log += _T("\r\n");
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
    m_saveFile->ShowModal();
    wxFile file(m_saveFile->GetPath(), wxFile::write);
    file.Write(m_log);
}

void Frame::OnMenuOpenSelected(wxCommandEvent& event)
{
    if (m_openFile->ShowModal() == wxID_OK)
        Start(m_openFile->GetPath());
}

void Frame::OnMenuFindSelected(wxCommandEvent& event)
{
    if (m_dlgFind != NULL)
    {
        delete m_dlgFind;
        m_dlgFind = NULL;
    }
    else
    {
        m_findData.SetFlags(wxFR_DOWN);
        m_dlgFind = new wxFindReplaceDialog(this, &m_findData, _("Find dialog"));
        m_dlgFind->Show(true);
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
                m_lastIndex = m_logCtrl->GetInsertionPoint();
                wxString tmp = m_logCtrl->GetValue().SubString(m_lastIndex, m_logCtrl->GetLastPosition() - 1);
                int i;
                if (event.GetFlags() & wxFR_MATCHCASE)
                    i = m_log.Find(event.GetFindString().c_str());
                else
                    i = tmp.Upper().Find(event.GetFindString().Upper().c_str());
                if (i >= 0)
                {
                    m_lastIndex += i;
                    m_logCtrl->SetSelection(m_lastIndex, m_lastIndex + event.GetFindString().Length());
                }
            }
            else // find next
            {
                wxString tmp = m_logCtrl->GetValue().SubString(++m_lastIndex, m_logCtrl->GetLastPosition() - 1) ;
                int i;
                if (event.GetFlags() & wxFR_MATCHCASE)
                    i = tmp.Find(event.GetFindString().c_str());
                else
                    i = tmp.Upper().Find(event.GetFindString().Upper().c_str());
                if (i >= 0)
                {
                    m_lastIndex += i;
                    m_logCtrl->SetSelection(m_lastIndex, m_lastIndex + event.GetFindString().Length());
                }
            }
            m_logCtrl->SetFocus();
        }
        else //find up
        {
            if (type == wxEVT_COMMAND_FIND)
            {
                m_lastIndex = m_logCtrl->GetInsertionPoint();
                int i;
                if (event.GetFlags() & wxFR_MATCHCASE)
                    i = m_logCtrl->GetValue().rfind(event.GetFindString().c_str(), m_lastIndex);
                else
                    i = m_logCtrl->GetValue().Upper().rfind(event.GetFindString().Upper().c_str(), m_lastIndex);

                if (i >= 0)
                {
                    m_logCtrl->SetSelection(i, i + event.GetFindString().Length());
                    m_lastIndex = i;
                }
            }
            else
            {
                wxString tmp = m_logCtrl->GetValue().SubString(0, --m_lastIndex) ;
                int i;
                if (event.GetFlags() & wxFR_MATCHCASE)
                    i = tmp.rfind(event.GetFindString().c_str(), m_lastIndex);
                else
                    i = tmp.Upper().rfind(event.GetFindString().Upper().c_str(), m_lastIndex);
                if (i >= 0)
                {
                    m_lastIndex = i;
                    m_logCtrl->SetSelection(m_lastIndex, m_lastIndex + event.GetFindString().Length());
                }
            }
        }

        m_logCtrl->SetFocus();
    }
    else if (type == wxEVT_COMMAND_FIND_CLOSE)
    {
        delete m_dlgFind;
        m_dlgFind = NULL;
    }
}

void Frame::OnMenuReloadSelected(wxCommandEvent& event)
{
    Start(m_lastFile);
}
