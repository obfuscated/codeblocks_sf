/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 * $Revision$
 * $Id$
 * $HeadURL$
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

Frame::Frame() : m_LogCount(0), m_DlgFind(NULL)
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
    m_LogCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(900,550), wxTE_AUTO_SCROLL|wxTE_MULTILINE|wxTE_READONLY|wxHSCROLL|wxTE_RICH2, wxDefaultValidator, _T("wxID_ANY"));
    sizer->Add(m_LogCtrl, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(sizer);
    MenuBar1 = new wxMenuBar();
    Menu2 = new wxMenu();
    MenuItem3 = new wxMenuItem(Menu2, wxID_OPEN, _("&Open...\tCtrl+O"), _("Open the source code to be tested"), wxITEM_NORMAL);
    Menu2->Append(MenuItem3);
    MenuItem6 = new wxMenuItem(Menu2, wxID_REFRESH, _("&Reload\tF5"), _("Reload test file"), wxITEM_NORMAL);
    Menu2->Append(MenuItem6);
    Menu2->AppendSeparator();
    MenuItem1 = new wxMenuItem(Menu2, wxID_SAVE, _("&Save Log...\tCtrl+S"), _("Save log file to hard disk "), wxITEM_NORMAL);
    Menu2->Append(MenuItem1);
    Menu2->AppendSeparator();
    MenuItem2 = new wxMenuItem(Menu2, wxID_EXIT, _("&Quit\tCtrl+Q"), wxEmptyString, wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("&File"));
    Menu1 = new wxMenu();
    MenuItem4 = new wxMenuItem(Menu1, wxID_FIND, _("&Find...\tCtrl+F"), _("Find keyword"), wxITEM_NORMAL);
    Menu1->Append(MenuItem4);
    MenuBar1->Append(Menu1, _("&Search"));
    Menu3 = new wxMenu();
    MenuItem5 = new wxMenuItem(Menu3, wxID_ABOUT, _("&About"), wxEmptyString, wxITEM_NORMAL);
    Menu3->Append(MenuItem5);
    MenuBar1->Append(Menu3, _("&Help"));
    SetMenuBar(MenuBar1);
    m_StatuBar = new wxStatusBar(this, wxID_ANY, 0, _T("wxID_ANY"));
    int __wxStatusBarWidths_1[1] = { -10 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    m_StatuBar->SetFieldsCount(1,__wxStatusBarWidths_1);
    m_StatuBar->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(m_StatuBar);
    m_OpenFile = new wxFileDialog(this, _("Select Test Source File"), _("."), wxEmptyString, _("*.cpp;*.h"), wxFD_DEFAULT_STYLE, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    m_SaveFile = new wxFileDialog(this, _("Select file"), _("."), _("log.txt"), _("*.txt"), wxFD_DEFAULT_STYLE|wxFD_SAVE, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    sizer->Fit(this);
    sizer->SetSizeHints(this);
    Center();

    Connect(wxID_OPEN,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Frame::OnMenuOpenSelected);
    Connect(wxID_REFRESH,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Frame::OnMenuReloadSelected);
    Connect(wxID_SAVE,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Frame::OnMenuSaveSelected);
    Connect(wxID_EXIT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Frame::OnMenuQuitSelected);
    Connect(wxID_FIND,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Frame::OnMenuFindSelected);
    Connect(wxID_ABOUT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Frame::OnMenuAboutSelected);
    //*)

    m_StatuBar->SetStatusText(_("Ready!"));
}

Frame::~Frame()
{
    //(*Destroy(Frame)
    //*)
    if (m_DlgFind != NULL)
    {
        delete m_DlgFind;
    }
}

void Frame::Start(const wxString& file)
{
    if (!wxFileName::FileExists(file))
        return;

    m_LastFile = file;
    m_LogCount = 0;
    m_Log.Clear();
    m_LogCtrl->Clear();
    m_ParserTest.Clear();
    DoStart();
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
    m_SaveFile->ShowModal();
    wxFile file(m_SaveFile->GetPath(), wxFile::write);
    file.Write(m_Log);
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
