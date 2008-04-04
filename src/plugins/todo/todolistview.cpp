/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk.h"
#ifndef CB_PRECOMP
  #include <wx/arrstr.h>
  #include <wx/button.h>
  #include <wx/combobox.h>
  #include <wx/event.h>
  #include <wx/file.h>
  #include <wx/intl.h>
  #include <wx/listctrl.h>
  #include <wx/sizer.h>
  #include <wx/stattext.h>
  #include <wx/utils.h>

  #include "cbeditor.h"
  #include "cbproject.h"
  #include "editormanager.h"
  #include "filemanager.h"
  #include "globals.h"
  #include "manager.h"
  #include "projectfile.h"
  #include "projectmanager.h"
  //#include "logmanager.h"
#endif

#include <wx/progdlg.h>

#include "cbstyledtextctrl.h"
#include "encodingdetector.h"

#include "todolistview.h"

namespace
{
    int idList = wxNewId();
    int idSource = wxNewId();
    int idUser = wxNewId();
    int idButtonRefresh = wxNewId();
};

BEGIN_EVENT_TABLE(ToDoListView, wxEvtHandler)
    EVT_COMBOBOX(idSource, ToDoListView::OnComboChange)
    EVT_COMBOBOX(idUser, ToDoListView::OnComboChange)
    EVT_BUTTON(idButtonRefresh, ToDoListView::OnButtonRefresh)
END_EVENT_TABLE()

ToDoListView::ToDoListView(const wxArrayString& titles, const wxArrayInt& widths, const wxArrayString& m_Types)
    : ListCtrlLogger(titles, widths, false),
    panel(0),
    m_pSource(0L),
    m_pUser(0L),
    m_Types(m_Types),
    m_LastFile(wxEmptyString),
    m_ignore(false)
{
    //ctor
}

ToDoListView::~ToDoListView()
{
    //dtor
    Manager::Get()->GetAppWindow()->RemoveEventHandler(this);
}

wxWindow* ToDoListView::CreateControl(wxWindow* parent)
{
    panel = new wxPanel(parent);
    ListCtrlLogger::CreateControl(panel);

    control->SetId(idList);
    Connect(idList, -1, wxEVT_COMMAND_LIST_ITEM_SELECTED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &ToDoListView::OnListItemSelected);
    Connect(idList, -1, wxEVT_COMMAND_LIST_ITEM_ACTIVATED, //pecan 1/2/2006 12PM
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &ToDoListView::OnDoubleClick);

    Manager::Get()->GetAppWindow()->PushEventHandler(this);

    control->SetInitialSize(wxSize(342,56));
    control->SetMinSize(wxSize(342,56));
    wxSizer* bs = new wxBoxSizer(wxVERTICAL);
    bs->Add(control, 1, wxEXPAND);
    if (bs)
    {
        wxArrayString choices;
        choices.Add(_("Current file"));
        choices.Add(_("Open files"));
        choices.Add(_("All project files"));
        wxBoxSizer* hbs = new wxBoxSizer(wxHORIZONTAL);

        hbs->Add(new wxStaticText(panel, wxID_ANY, _("Scope:")), 0, wxTOP, 4);

        m_pSource = new wxComboBox(panel, idSource, wxEmptyString, wxDefaultPosition, wxDefaultSize, 3, &choices[0], wxCB_READONLY);
        int source = Manager::Get()->GetConfigManager(_T("todo_list"))->ReadInt(_T("source"), 0);
        m_pSource->SetSelection(source);
        hbs->Add(m_pSource, 0, wxLEFT | wxRIGHT, 8);

        hbs->Add(new wxStaticText(panel, wxID_ANY, _("User:")), 0, wxTOP, 4);

        m_pUser = new wxComboBox(panel, idUser, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0L, wxCB_READONLY);
        m_pUser->Append(_("<All users>"));
        m_pUser->SetSelection(0);
        hbs->Add(m_pUser, 0, wxLEFT, 4);

        m_pRefresh = new wxButton(panel, idButtonRefresh, _("Refresh"));
        hbs->Add(m_pRefresh, 0, wxLEFT, 4);

        bs->Add(hbs, 0, wxGROW | wxALL, 4);
    }
    panel->SetSizer(bs);

    return panel;
}

void ToDoListView::LoadUsers()
{
    wxString oldStr = m_pUser->GetStringSelection();
    m_pUser->Clear();
    m_pUser->Append(_("<All users>"));

    // loop through all todos and add distinct users
//    Manager::Get()->GetLogManager()->DebugLog(F(_T("Managing %d items."), m_Items.GetCount()));
    for (unsigned int i = 0; i < m_Items.GetCount(); ++i)
    {
        wxString user = m_Items[i].user;
//        Manager::Get()->GetLogManager()->DebugLog(F(_T("Found user %s."), user.c_str()));
        if (!user.IsEmpty())
        {
            if (m_pUser->FindString(user) == wxNOT_FOUND)
                m_pUser->Append(user);
        }
    }
    int old = m_pUser->FindString(oldStr);
    if (old != wxNOT_FOUND)
        m_pUser->SetSelection(old);
    else
        m_pUser->SetSelection(0); // all users
}

void ToDoListView::FillList()
{
    control->Freeze();
    Clear();
    m_Items.Clear();

    TodoItemsMap::iterator it;

    if(m_pSource->GetSelection()==0) // Single file
    {
        wxString filename(wxEmptyString);
        cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(Manager::Get()->GetEditorManager()->GetActiveEditor());
        if(ed)
            filename = ed->GetFilename();
        for(unsigned int i = 0; i < m_itemsmap[filename].size(); i++)
        {
            m_Items.Add(m_itemsmap[filename][i]);
        }
    }
    else
    {
        for(it = m_itemsmap.begin();it != m_itemsmap.end();++it)
        {
            for(unsigned int i = 0; i < it->second.size(); i++)
            {
                m_Items.Add(it->second[i]);
            }
        }
    }

    for (unsigned int i = 0; i < m_Items.GetCount(); ++i)
    {
        const ToDoItem& item = m_Items[i];
        if (m_pUser->GetSelection() == 0 || // all users
            m_pUser->GetStringSelection().Matches(item.user)) // or matches user
        {
            int idx = control->InsertItem(control->GetItemCount(), item.type);
            control->SetItem(idx, 1, item.text);
            control->SetItem(idx, 2, item.user);
            control->SetItem(idx, 3, item.priorityStr);
            control->SetItem(idx, 4, item.lineStr);
            control->SetItem(idx, 5, item.filename);
            control->SetItemData(idx, i);
        }
    }
    control->Thaw();
    LoadUsers();
}

void ToDoListView::ParseCurrent(bool forced)
{
    if(m_ignore)
        return; // Reentrancy
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(Manager::Get()->GetEditorManager()->GetActiveEditor());
    if(ed)
    {
        wxString filename = ed->GetFilename();
        if(forced || filename != m_LastFile)
        {
            m_LastFile = filename;
            m_Items.Clear();
            ParseEditor(ed);
        }
    }
    FillList();
}

void ToDoListView::Parse()
{
//    wxBusyCursor busy;
    // based on user prefs, parse files for todo items
    if(m_ignore || (panel && !panel->IsShownOnScreen()) )
        return; // Reentrancy
    Clear();
    m_itemsmap.clear();
    m_Items.Clear();

    switch (m_pSource->GetSelection())
    {
        case 0: // current file only
        {
            // this is the easiest selection ;)
            cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(Manager::Get()->GetEditorManager()->GetActiveEditor());
            ParseEditor(ed);
            break;
        }
        case 1: // open files
        {
            // easy too; parse all open editor files...
            for (int i = 0; i < Manager::Get()->GetEditorManager()->GetEditorsCount(); ++i)
            {
                cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(Manager::Get()->GetEditorManager()->GetEditor(i));
                ParseEditor(ed);
            }
            break;
        }
        case 2: // all project files
        {
            // loop all project files
            // but be aware: if a file is opened, use the open file because
            // it might not be the same on the disk...
            cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
            if (!prj)
                return;
            wxProgressDialog pd(_T("To-Do Plugin: Processing all files.."),
                                _T("Processing a big project may take large amount of time.\n\n"
                                   "Please be patient!\n"),
                                prj->GetFilesCount(),
                                Manager::Get()->GetAppWindow(),
                                wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_CAN_ABORT);
            for (int i = 0; i < prj->GetFilesCount(); ++i)
            {
                ProjectFile* pf = prj->GetFile(i);
                wxString filename = pf->file.GetFullPath();
                cbEditor* ed = Manager::Get()->GetEditorManager()->IsBuiltinOpen(filename);
                if (ed)
                    ParseEditor(ed);
                else
                    ParseFile(filename);
                if (!pd.Update(i))
                {
                    break;
                }
            }
            break;
        }
    }
    FillList();
}

int ToDoListView::CalculateLineNumber(const wxString& buffer, int upTo)
{
    int line = 0;
    for (int i = 0; i < upTo; ++i)
    {
        if (buffer.GetChar(i) == _T('\r') && buffer.GetChar(i + 1) == _T('\n')) // CR+LF
            continue; // we 'll count on \n (next loop)
        else if (buffer.GetChar(i) == _T('\r') || // CR only
                buffer.GetChar(i) == _T('\n')) // lf only
            ++line;
    }
    return line;
}

void ToDoListView::ParseEditor(cbEditor* pEditor)
{
    if (pEditor)
        ParseBuffer(pEditor->GetControl()->GetText(), pEditor->GetFilename());
}

void ToDoListView::ParseFile(const wxString& filename)
{
    if (!wxFileExists(filename))
        return;

    wxString st;
    LoaderBase* fileBuffer = Manager::Get()->GetFileManager()->Load(filename, true);
    if (fileBuffer)
    {
        EncodingDetector encDetector(fileBuffer);
        if (encDetector.IsOK())
        {
            st = encDetector.GetWxStr();
            ParseBuffer(st, filename);
        }
    }
    else
    {
        return;
    }

    delete fileBuffer;
}

void ToDoListView::ParseBuffer(const wxString& buffer, const wxString& filename)
{
    // this is the actual workhorse...

    // ok, we look for two basic kinds of todo entries in the text
    // our version...
    // TODO (mandrav#0#): Implement code to do this and the other...
    // and a generic version...
    // TODO: Implement code to do this and the other...

    m_itemsmap[filename].clear();

    for (unsigned int i = 0; i < m_Types.GetCount(); ++i)
    {
//Manager::Get()->GetLogManager()->DebugLog("Looking for %s", m_Types[i].c_str());
        int pos = buffer.find(m_Types[i], 0);

        while (pos > 0)
        {
            // ok, start parsing now...
            // keep a temp copy of pos to work with
            int idx = pos;
            bool isValid = false; // found it in a comment?
            bool isC = false; // C or C++ style comment?

//#warning TODO (mandrav#1#): Make viewtododlg understand and display todo notes that are compiler warnings/errors...

            // first check what type of comment we have
            wxString allowedChars = _T(" \t/*");
            wxChar lastChar = _T('\0');
            while (idx >= 0)
            {
                wxChar c = buffer.GetChar(--idx);
                if ((int)allowedChars.Index(c) != wxNOT_FOUND)
                {
                    if (c == _T('/') && (lastChar == _T('/') || lastChar == _T('*')))
                    {
                        isValid = true;
                        isC = lastChar == _T('*');
                        break;
                    }
                }
                else
                    break;
                lastChar = c;
            }

//Manager::Get()->GetLogManager()->DebugLog("Found %s %s style %s at %d", isValid ? "valid" : "invalid", isC ? "C" : "C++", m_Types[i].c_str(), pos);
            if (isValid)
            {
                ToDoItem item;
                item.type = m_Types[i];
                item.filename = filename;

                idx = pos + m_Types[i].Length();
                wxChar c = _T('\0');

//Manager::Get()->GetLogManager()->DebugLog("1");
                // skip to next non-blank char
                while (idx < (int)buffer.Length())
                {
                    c = buffer.GetChar(idx);
                    if (c != _T(' ') && c != _T('\t'))
                        break;
                    ++idx;
                }
//Manager::Get()->GetLogManager()->DebugLog("2");
                // is it ours or generic todo?
                if (c == _T('('))
                {
                    // it's ours, find user and/or priority
                    ++idx; // skip (
                    while (idx < (int)buffer.Length())
                    {
                        wxChar c1 = buffer.GetChar(idx);
                        if (c1 != _T('#') && c1 != _T(')'))
                        {
                            // a little logic doesn't hurt ;)

                            if (c1 == _T(' ') || c1 == _T('\t') || c1 == _T('\r') || c1 == _T('\n'))
                            {
                                // allow one consecutive space
                                if (item.user.Last() != _T(' '))
                                    item.user << _T(' ');
                            }
                            else
                                item.user << c1;
                        }
                        else if (c1 == _T('#'))
                        {
                            // look for priority
                            c1 = buffer.GetChar(++idx);
                            allowedChars = _T("0123456789");
                            if ((int)allowedChars.Index(c1) != wxNOT_FOUND)
                                item.priorityStr << c1;
                            // skip to start of text
                            while (idx < (int)buffer.Length())
                            {
                                wxChar c2 = buffer.GetChar(idx++);
                                if (c2 == _T(')') || c2 == _T('\r') || c2 == _T('\n'))
                                    break;
                            }
                            break;
                        }
                        else
                            break;
                        ++idx;
                    }
                }
//Manager::Get()->GetLogManager()->DebugLog("3");
                // ok, we 've reached the actual todo text :)
                // take everything up to the end of line or end of comment (if isC)
                wxChar lastChar = _T('\0');
                if (buffer.GetChar(idx) == _T(':'))
                    ++idx;
                while (idx < (int)buffer.Length())
                {
                    wxChar c1 = buffer.GetChar(idx++);
                    if (c1 == _T('\r') || c1 == _T('\n'))
                        break;
                    if (isC && c1 == _T('/') && lastChar == _T('*'))
                    {
                        // remove last char '*'
                        item.text.RemoveLast();
                        break;
                    }
                    if (c1 == _T(' ') || c1 == _T('\t'))
                    {
                        // allow one consecutive space
                        if (item.text.IsEmpty() || item.text.Last() != _T(' '))
                            item.text << _T(' ');
                    }
                    else
                        item.text << c1;
                    lastChar = c1;
                }
//Manager::Get()->GetLogManager()->DebugLog("4");
                // do some clean-up
                item.text.Trim();
                item.text.Trim(false);
                item.user.Trim();
                item.user.Trim(false);
                item.line = CalculateLineNumber(buffer, pos);
                item.lineStr << wxString::Format(_T("%d"), item.line + 1); // 1-based line number for list
                m_itemsmap[filename].push_back(item);
                // m_Items.Add(item);
            }
            else
                break; // invalid style...

            pos = buffer.find(m_Types[i], idx);
        }
//        Manager::Get()->GetLogManager()->DebugLog("Found it at %d", pos);
    }
}

void ToDoListView::OnComboChange(wxCommandEvent& event)
{
    Manager::Get()->GetConfigManager( _T("todo_list"))->Write(_T("source"), m_pSource->GetSelection() );
    Parse();
}

void ToDoListView::OnListItemSelected(wxCommandEvent& event)
{
    long index = control->GetNextItem(-1,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_SELECTED);
    if (index == -1)
        return;
    FocusEntry(index);
}

void ToDoListView::OnButtonRefresh(wxCommandEvent& event)
{
    Parse();
}

void ToDoListView::OnDoubleClick( wxCommandEvent& event )
{    //pecan 1/2/2006 12PM // Switched with OnListItemSelected by Rick 20/07/2007
    long idx = control->GetNextItem(-1,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_SELECTED);
    if (idx == -1)
        return;

    wxString file = m_Items[idx].filename;
    long int line = m_Items[idx].line;

    if (file.IsEmpty() || line < 0)
        return;

    // jump to file/line selected
    cbEditor* ed = Manager::Get()->GetEditorManager()->Open(file);
    if (ed)
    {
        bool old_ignore = m_ignore;
        m_ignore = true;
        ed->Activate();
        ed->GotoLine(line);
        FocusEntry(idx);
        m_ignore = old_ignore;
    }
}

void ToDoListView::FocusEntry(size_t index)                 //pecan 1/2/2006 12PM
{
    if (index >= 0 && index < (size_t)control->GetItemCount())
    {
        control->SetItemState(index, wxLIST_STATE_FOCUSED | wxLIST_STATE_SELECTED, wxLIST_STATE_FOCUSED | wxLIST_STATE_SELECTED);
        control->EnsureVisible(index);
    }
}
