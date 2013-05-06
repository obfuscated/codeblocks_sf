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
    #include <wx/checklst.h>
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
    #include "logmanager.h"
#endif

#include <wx/progdlg.h>

#include "cbstyledtextctrl.h"
#include "encodingdetector.h"
#include "editorcolourset.h"

#include "todolistview.h"

namespace
{
    int idList          = wxNewId();
    int idSource        = wxNewId();
    int idUser          = wxNewId();
    int idButtonRefresh = wxNewId();
    int idButtonTypes   = wxNewId();
};

BEGIN_EVENT_TABLE(ToDoListView, wxEvtHandler)
    EVT_COMBOBOX(idSource,        ToDoListView::OnComboChange)
    EVT_COMBOBOX(idUser,          ToDoListView::OnComboChange)
    EVT_BUTTON  (idButtonRefresh, ToDoListView::OnButtonRefresh)
    EVT_BUTTON  (idButtonTypes,   ToDoListView::OnButtonTypes)
END_EVENT_TABLE()

ToDoListView::ToDoListView(const wxArrayString& titles_in, const wxArrayInt& widths_in, const wxArrayString& Types) :
    ListCtrlLogger(titles_in, widths_in, false),
    m_pPanel(0),
    m_pSource(0L),
    m_pUser(0L),
    m_Types(Types),
    m_LastFile(wxEmptyString),
    m_Ignore(false),
    m_SortAscending(false),
    m_SortColumn(-1)
{
    //ctor
}

ToDoListView::~ToDoListView()
{
    //dtor
}

wxWindow* ToDoListView::CreateControl(wxWindow* parent)
{
    m_pPanel = new wxPanel(parent);
    ListCtrlLogger::CreateControl(m_pPanel);

    control->SetId(idList);
    Connect(idList, -1, wxEVT_COMMAND_LIST_ITEM_SELECTED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &ToDoListView::OnListItemSelected);
    Connect(idList, -1, wxEVT_COMMAND_LIST_ITEM_ACTIVATED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &ToDoListView::OnDoubleClick);
    Connect(idList, -1, wxEVT_COMMAND_LIST_COL_CLICK,
            (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction)
            &ToDoListView::OnColClick);

    Manager::Get()->GetAppWindow()->PushEventHandler(this);

    control->SetInitialSize(wxSize(342,56));
    control->SetMinSize(wxSize(342,56));
    wxSizer* bs = new wxBoxSizer(wxVERTICAL);
    bs->Add(control, 1, wxEXPAND);
    wxArrayString choices;
    choices.Add(_("Current file"));
    choices.Add(_("Open files"));
    choices.Add(_("All project files"));
    wxBoxSizer* hbs = new wxBoxSizer(wxHORIZONTAL);

    hbs->Add(new wxStaticText(m_pPanel, wxID_ANY, _("Scope:")), 0, wxTOP, 4);

    m_pSource = new wxComboBox(m_pPanel, idSource, wxEmptyString, wxDefaultPosition, wxDefaultSize, 3, &choices[0], wxCB_READONLY);
    int source = Manager::Get()->GetConfigManager(_T("todo_list"))->ReadInt(_T("source"), 0);
    m_pSource->SetSelection(source);
    hbs->Add(m_pSource, 0, wxLEFT | wxRIGHT, 8);

    hbs->Add(new wxStaticText(m_pPanel, wxID_ANY, _("User:")), 0, wxTOP, 4);

    m_pUser = new wxComboBox(m_pPanel, idUser, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0L, wxCB_READONLY);
    m_pUser->Append(_("<All users>"));
    m_pUser->SetSelection(0);
    hbs->Add(m_pUser, 0, wxLEFT, 4);

    wxButton* pRefresh = new wxButton(m_pPanel, idButtonRefresh, _("Refresh"));
    hbs->Add(pRefresh, 0, wxLEFT, 4);

    wxButton* pAllowedTypes = new wxButton(m_pPanel, idButtonTypes, _("Types"));
    hbs->Add(pAllowedTypes, 0, wxLEFT, 4);

    bs->Add(hbs, 0, wxGROW | wxALL, 4);
    m_pPanel->SetSizer(bs);

    m_pPanel->SetSizer(bs);

    m_pAllowedTypesDlg = new CheckListDialog(m_pPanel);
    return m_pPanel;
}

void ToDoListView::DestroyControls(bool destr_control)
{
    if (Manager::Get()->IsAppShuttingDown())
        return;
    Manager::Get()->GetAppWindow()->RemoveEventHandler(this);
    if (destr_control)
    {
        m_pPanel->Destroy();
        m_pPanel = nullptr;
    }
}


void ToDoListView::Parse()
{
//    wxBusyCursor busy;
    // based on user prefs, parse files for todo items
    if (m_Ignore || (m_pPanel && !m_pPanel->IsShownOnScreen()) )
        return; // Reentrancy

    Clear();
    m_ItemsMap.clear();
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
            wxProgressDialog pd(_T("Todo Plugin: Processing all files.."),
                                _T("Processing a big project may take large amount of time.\n\n"
                                   "Please be patient!\n"),
                                prj->GetFilesCount(),
                                Manager::Get()->GetAppWindow(),
                                wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_CAN_ABORT);
            int i = 0;
            for (FilesList::iterator it = prj->GetFilesList().begin(); it != prj->GetFilesList().end(); ++it)
            {
                ProjectFile* pf = *it;
                wxString filename = pf->file.GetFullPath();
                cbEditor* ed = Manager::Get()->GetEditorManager()->IsBuiltinOpen(filename);
                if (ed)
                    ParseEditor(ed);
                else
                    ParseFile(filename);
                if (!pd.Update(i++))
                {
                    break;
                }
            }
            break;
        }
        default:
            break;
    }
    FillList();
}

void ToDoListView::ParseCurrent(bool forced)
{
    if (m_Ignore)
        return; // Reentrancy
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(Manager::Get()->GetEditorManager()->GetActiveEditor());
    if (ed)
    {
        wxString filename = ed->GetFilename();
        if (forced || filename != m_LastFile)
        {
            m_LastFile = filename;
            m_Items.Clear();
            ParseEditor(ed);
        }
    }
    FillList();
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
            if (m_pUser->FindString(user, true) == wxNOT_FOUND)
                m_pUser->Append(user);
        }
    }
    int old = m_pUser->FindString(oldStr, true);
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

    if (m_pSource->GetSelection()==0) // Single file
    {
        wxString filename(wxEmptyString);
        cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(Manager::Get()->GetEditorManager()->GetActiveEditor());
        if (ed)
            filename = ed->GetFilename();
        for (unsigned int i = 0; i < m_ItemsMap[filename].size(); i++)
            m_Items.Add(m_ItemsMap[filename][i]);
    }
    else
    {
        for (it = m_ItemsMap.begin();it != m_ItemsMap.end();++it)
        {
            for (unsigned int i = 0; i < it->second.size(); i++)
                m_Items.Add(it->second[i]);
        }
    }

    SortList();
    FillListControl();

    control->Thaw();
    LoadUsers();
}

void ToDoListView::SortList()
{
    if (m_Items.GetCount()<2)
        return;

    // TODO (morten#5#): Used lame bubble sort here -> use another algo to speed up if required.
    bool swapped = true;
    while (swapped)
    {
        swapped = false;
        // Compare one item with the next one...
        for (unsigned int i=0; i<m_Items.GetCount()-1; ++i)
        {
            int swap = 0; // no swap
            const ToDoItem item1 = m_Items[i];
            const ToDoItem item2 = m_Items[i+1];
            switch (m_SortColumn)
            {
                // The columns are composed with different types...
                case 0: // type
                    swap = item1.type.CmpNoCase(item2.type);
                    break;
                case 1: // text
                    swap = item1.text.CmpNoCase(item2.text);
                    break;
                case 2: // user
                    swap = item1.user.CmpNoCase(item2.user);
                    break;
                case 3: // priority
                    if      (item1.priorityStr > item2.priorityStr)
                        swap =  1;
                    else if (item1.priorityStr < item2.priorityStr)
                        swap = -1;
                    else
                        swap =  0;
                    break;
                case 4: // line
                    if      (item1.line > item2.line)
                        swap =  1;
                    else if (item1.line < item2.line)
                        swap = -1;
                    else
                        swap =  0;
                    break;
                case 5: // date
                    {
                        wxDateTime date1;
                        wxDateTime date2;
                        date1.ParseDate(item1.date.c_str());
                        date2.ParseDate(item2.date.c_str());
                        if      (date1 > date2)
                            swap =  1;
                        else if (date1 < date2)
                            swap = -1;
                        else
                            swap =  0;
                        break;
                    }
                case 6: // filename
                    swap = item1.filename.CmpNoCase(item2.filename);
                    break;
                default:
                    break;
            }// switch

            // Swap items
            if      ((m_SortAscending)  && (swap== 1)) // item1 "greater" than item2
            {
                m_Items[i]   = item2;
                m_Items[i+1] = item1;
                swapped = true;
            }
            else if ((!m_SortAscending) && (swap==-1)) // item1 "smaller" than item2
            {
                m_Items[i]   = item2;
                m_Items[i+1] = item1;
                swapped = true;
            }
        }// for
    }// while
}

void ToDoListView::FillListControl()
{
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
            control->SetItem(idx, 5, item.date);
            control->SetItem(idx, 6, item.filename);
            control->SetItemData(idx, i);
        }
    }
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
        return;

    delete fileBuffer;
}

void ToDoListView::SkipSpaces(const wxString& buffer, size_t &pos)
{
    wxChar c = buffer[pos];
    while ( c == _T(' ') || c == _T('\t') )
        c = buffer[++pos];
}

void ToDoListView::ParseBuffer(const wxString& buffer, const wxString& filename)
{
    // this is the actual workhorse...

    HighlightLanguage hlang = Manager::Get()->GetEditorManager()->GetColourSet()->GetLanguageForFilename(filename);
    CommentToken cmttoken = Manager::Get()->GetEditorManager()->GetColourSet()->GetCommentToken(hlang);
    wxString langName = Manager::Get()->GetEditorManager()->GetColourSet()->GetLanguageName(hlang);

    m_ItemsMap[filename].clear();

    wxArrayString allowedTypes;
    size_t t = 0;
    while(t < m_Types.Count())
    {
        if(m_pAllowedTypesDlg->IsChecked(m_Types.Item(t)))
            allowedTypes.Add(m_Types.Item(t));
        t++;
    }

    wxArrayString startStrings;
    if (langName == _T("C/C++") )
    {
        startStrings.Add(_T("#warning"));
        startStrings.Add(_T("#error"));
    }
    if (!cmttoken.doxygenLineComment.IsEmpty())
        startStrings.Add(cmttoken.doxygenLineComment);
    if (!cmttoken.doxygenStreamCommentStart.IsEmpty())
        startStrings.Add(cmttoken.doxygenStreamCommentStart);

    if ( !cmttoken.lineComment.IsEmpty() )
        startStrings.Add(cmttoken.lineComment);
    if ( !cmttoken.streamCommentStart.IsEmpty() )
        startStrings.Add(cmttoken.streamCommentStart);

    if ( startStrings.IsEmpty() || allowedTypes.IsEmpty() )
    {
        Manager::Get()->GetLogManager()->Log(_T("ToDoList: Warning: No to-do types or comment symbols selected to search for, nothing to do."));
        return;
    }

    for ( unsigned k = 0; k < startStrings.size(); k++)
    {
        size_t pos = 0;
        int oldline=0, oldlinepos=0;
        while (1)
        {
            pos = buffer.find(startStrings[k], pos);
            if ( pos == (size_t)wxNOT_FOUND )
                break;

            pos += startStrings[k].length();
            SkipSpaces(buffer, pos);

            for (unsigned int i = 0; i < allowedTypes.GetCount(); ++i)
            {
                wxString type = buffer.Mid(pos, allowedTypes[i].length());
                if ( type != allowedTypes[i])
                    continue;
                pos += allowedTypes[i].length();
                SkipSpaces(buffer, pos);

                ToDoItem item;
                item.type = allowedTypes[i];
                item.filename = filename;


                // ok, we look for two basic kinds of todo entries in the text
                // our version...
                // TODO (mandrav#0#): Implement code to do this and the other...
                // and a generic version...
                // TODO: Implement code to do this and the other...

                wxChar c = buffer.GetChar(pos);
                // is it ours or generic todo?
                if (c == _T('('))
                {
                    // it's ours, find user and/or priority
                    ++pos;
                    while(pos < buffer.length() && buffer.GetChar(pos) != _T('\r') && buffer.GetChar(pos) != _T('\n'))
                    {
                        wxChar c1 = buffer.GetChar(pos);
                        if (c1 != _T('#') && c1 != _T(')'))
                        {
                            // a little logic doesn't hurt ;)
                            if (c1 == _T(' ') || c1 == _T('\t'))
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
                            c1 = buffer.GetChar(++pos);
                            const wxString allowedChars = _T("0123456789");
                            if ((int)allowedChars.Index(c1) != wxNOT_FOUND)
                                item.priorityStr << c1;
                            // skip to start of date
                            while (pos < buffer.length() && buffer.GetChar(pos) != _T('\r') && buffer.GetChar(pos) != _T('\n') )
                            {
                                wxChar c2 = buffer.GetChar(pos);
                                if ( c2 == _T('#'))
                                {
                                    pos++;
                                    break;
                                }
                                if ( c2 == _T(')') )
                                    break;
                                pos++;
                            }
                            // look for date
                            while (pos < buffer.length() && buffer.GetChar(pos) != _T('\r') && buffer.GetChar(pos) != _T('\n') )
                            {
                                wxChar c2 = buffer.GetChar(pos++);
                                if (c2 == _T(')'))
                                    break;
                                item.date << c2;
                            }

                            break;
                        }
                        else if (c1 == _T(')'))
                        {
                            ++pos;
                            break;
                        }
                        else
                            break;
                        ++pos;
                    }
                }
                // ok, we 've reached the actual todo text :)
                // take everything up to the end of line
                if( buffer.GetChar(pos) == _T(':'))
                    ++pos;
                size_t idx = pos;
                while (buffer.GetChar(idx) != _T('\r') && buffer.GetChar(idx) != _T('\n'))
                    idx++;
                item.text = buffer.Mid(pos, idx-pos);

                // do some clean-up
                item.text.Trim();
                item.text.Trim(false);
                item.user.Trim();
                item.user.Trim(false);
                wxDateTime date;
                if ( !date.ParseDate(item.date.c_str()) ) item.date.clear(); // not able to parse date so clear the string
                item.line = CalculateLineNumber(buffer, pos, oldline, oldlinepos);
                item.lineStr << wxString::Format(_T("%d"), item.line + 1); // 1-based line number for list
                m_ItemsMap[filename].push_back(item);
                m_Items.Add(item);

                pos = idx;
            }
            pos ++;
        }
    }
}

int ToDoListView::CalculateLineNumber(const wxString& buffer, int upTo, int &oldline, int &oldlinepos )
{
    for (; oldlinepos < upTo; ++oldlinepos)
    {
        if (buffer.GetChar(oldlinepos) == _T('\r') && buffer.GetChar(oldlinepos + 1) == _T('\n')) // CR+LF
            continue; // we 'll count on \n (next loop)
        else if (buffer.GetChar(oldlinepos) == _T('\r') || // CR only
                buffer.GetChar(oldlinepos) == _T('\n')) // lf only
            ++oldline;
    }
    return oldline;
}

void ToDoListView::FocusEntry(size_t index)
{
    if (index < (size_t)control->GetItemCount())
    {
        control->SetItemState(index, wxLIST_STATE_FOCUSED | wxLIST_STATE_SELECTED, wxLIST_STATE_FOCUSED | wxLIST_STATE_SELECTED);
        control->EnsureVisible(index);
    }
}
void ToDoListView::OnComboChange(cb_unused wxCommandEvent& event)
{
    Manager::Get()->GetConfigManager( _T("todo_list"))->Write(_T("source"), m_pSource->GetSelection() );
    Parse();
}

void ToDoListView::OnListItemSelected(cb_unused wxCommandEvent& event)
{
    long index = control->GetNextItem(-1,
                                      wxLIST_NEXT_ALL,
                                      wxLIST_STATE_SELECTED);
    if (index == -1)
        return;
    FocusEntry(index);
}

void ToDoListView::OnButtonTypes(cb_unused wxCommandEvent& event)
{
    m_pAllowedTypesDlg->Show(!m_pAllowedTypesDlg->IsShown());
}

void ToDoListView::OnButtonRefresh(cb_unused wxCommandEvent& event)
{
    Parse();
}

void ToDoListView::OnDoubleClick(cb_unused wxCommandEvent& event)
{    // pecan 1/2/2006 12PM : Switched with OnListItemSelected by Rick 20/07/2007
    long item = control->GetNextItem(-1,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_SELECTED);
    if (item == -1)
        return;
    long idx = control->GetItemData(item);

    wxString file = m_Items[idx].filename;
    long int line = m_Items[idx].line;

    if (file.IsEmpty() || line < 0)
        return;

    // jump to file/line selected
    cbEditor* ed = Manager::Get()->GetEditorManager()->Open(file);
    if (ed)
    {
        bool old_ignore = m_Ignore;
        m_Ignore = true;
        ed->Activate();
        ed->GotoLine(line);
        FocusEntry(idx);
        m_Ignore = old_ignore;
    }
}

void ToDoListView::OnColClick(wxListEvent& event)
{
    if (event.GetColumn() != m_SortColumn)
        m_SortAscending = true;
    else
        m_SortAscending = !m_SortAscending;

    m_SortColumn = event.GetColumn();
    FillList();
}


CheckListDialog::CheckListDialog(wxWindow*       parent,
                                 wxWindowID      id,
                                 const wxString& title,
                                 const wxPoint&  pos,
                                 const wxSize&   size,
                                 long            style)
  : wxDialog(parent, id, title, pos, size, style)
{
    SetSizeHints(wxDefaultSize, wxDefaultSize);

    wxBoxSizer* boxSizer;
    boxSizer = new wxBoxSizer( wxVERTICAL );

    wxArrayString m_checkList1Choices;
    m_checkList = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkList1Choices, 0 );
    boxSizer->Add( m_checkList, 1, wxEXPAND, 5 );

    m_okBtn = new wxButton(this, wxID_ANY, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0);
    boxSizer->Add( m_okBtn, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP|wxBOTTOM, 5 );

    SetSizer( boxSizer );
    Layout();

    // Connect Events
    m_okBtn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CheckListDialog::OkOnButtonClick ), NULL, this);
}

CheckListDialog::~CheckListDialog()
{
    // Disconnect Events
    m_okBtn->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CheckListDialog::OkOnButtonClick ), NULL, this);
}

void CheckListDialog::OkOnButtonClick(cb_unused wxCommandEvent& event)
{
    Show(false);
    Manager::Get()->GetConfigManager(_T("todo_list"))->Write(_T("types_selected"), GetChecked());
}

bool CheckListDialog::IsChecked(wxString item)
{
    int result = m_checkList->FindString(item, true);
    result = (result == wxNOT_FOUND) ? 0 : result;
    return m_checkList->IsChecked(result);
}

wxArrayString CheckListDialog::GetChecked()
{
    size_t item = 0;
    wxArrayString items;
    while(item < m_checkList->GetCount())
    {
        if (m_checkList->IsChecked(item))
            items.Add(m_checkList->GetString(item));
        item++;
    }
    return items;
}

void CheckListDialog::SetChecked(wxArrayString items)
{
    size_t item = 0;
    while(item < items.GetCount())
    {
        m_checkList->Check(m_checkList->FindString(items.Item(item), true));
        item++;
    }
}
