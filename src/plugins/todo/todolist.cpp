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
  #include <wx/checklst.h>
  #include <wx/event.h>
  #include <wx/frame.h> // GetMenuBar
  #include <wx/fs_zip.h>
  #include <wx/intl.h>
  #include <wx/menu.h>
  #include <wx/menuitem.h>
  #include <wx/string.h>
  #include <wx/utils.h>
  #include <wx/xrc/xmlres.h>
  #include "cbeditor.h"
  #include "configmanager.h"
  #include "editormanager.h"
  #include "manager.h"
  #include "projectmanager.h"
  #include "logmanager.h"
  #include "sdk_events.h"
#endif


#include "addtododlg.h"
#include "asktypedlg.h"
#include "cbstyledtextctrl.h"
#include "editorcolourset.h"
#include "todolist.h"
#include "todolistview.h"
#include "todosettingsdlg.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ToDoItems); // TODO: find out why this causes a shadow warning for 'Item'

namespace
{
    PluginRegistrant<ToDoList> reg(_T("ToDoList"));
}

const int idViewTodo = wxNewId();
const int idAddTodo = wxNewId();
const int idStartParsing = wxNewId();

BEGIN_EVENT_TABLE(ToDoList, cbPlugin)
    EVT_UPDATE_UI(idViewTodo, ToDoList::OnUpdateUI)
    EVT_MENU(idViewTodo, ToDoList::OnViewList)
    EVT_UPDATE_UI(idAddTodo, ToDoList::OnUpdateAdd)
    EVT_MENU(idAddTodo, ToDoList::OnAddItem)
END_EVENT_TABLE()

ToDoList::ToDoList() :
    m_InitDone(false),
    m_ParsePending(false),
    m_StandAlone(true)
{
    //ctor
    if (!Manager::LoadResource(_T("todo.zip")))
    {
        NotifyMissingFile(_T("todo.zip"));
    }
}

ToDoList::~ToDoList()
{
}

void ToDoList::OnAttach()
{
    // create ToDo in bottom view
    wxArrayString titles;
    wxArrayInt widths;
    titles.Add(_("Type")); widths.Add(64);
    titles.Add(_("Text")); widths.Add(320);
    titles.Add(_("User")); widths.Add(64);
    titles.Add(_("Prio")); widths.Add(48);
    titles.Add(_("Line")); widths.Add(48);
    titles.Add(_("File")); widths.Add(640);

    m_pListLog = new ToDoListView(titles, widths, m_Types);

    m_StandAlone = Manager::Get()->GetConfigManager(_T("todo_list"))->ReadBool(_T("stand_alone"), true);
    if (!m_StandAlone)
    {
        LogManager* msgMan = Manager::Get()->GetLogManager();
        m_ListPageIndex = msgMan->SetLog(m_pListLog);
        msgMan->Slot(m_ListPageIndex).title = _("To Do");

        CodeBlocksLogEvent evt(cbEVT_ADD_LOG_WINDOW, m_pListLog, msgMan->Slot(m_ListPageIndex).title, msgMan->Slot(m_ListPageIndex).icon);
        Manager::Get()->ProcessEvent(evt);
    }
    else
    {
        m_pListLog->CreateControl(Manager::Get()->GetAppWindow());
        m_pListLog->GetWindow()->SetSize(wxSize(352,94));
        m_pListLog->GetWindow()->SetInitialSize(wxSize(352,94));

        CodeBlocksDockEvent evt(cbEVT_ADD_DOCK_WINDOW);
        evt.name = _T("TodoListPanev2.0.0");
        evt.title = _("Todo list");
        evt.pWindow = m_pListLog->GetWindow();
        evt.dockSide = CodeBlocksDockEvent::dsFloating;
        evt.desiredSize.Set(352, 94);
        evt.floatingSize.Set(352, 94);
        evt.minimumSize.Set(352, 94);
        Manager::Get()->ProcessEvent(evt);
    }

    m_AutoRefresh = Manager::Get()->GetConfigManager(_T("todo_list"))->ReadBool(_T("auto_refresh"), true);
    LoadUsers();
    LoadTypes();

    // register event sink
    Manager::Get()->RegisterEventSink(cbEVT_APP_STARTUP_DONE, new cbEventFunctor<ToDoList, CodeBlocksEvent>(this, &ToDoList::OnAppDoneStartup));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_OPEN, new cbEventFunctor<ToDoList, CodeBlocksEvent>(this, &ToDoList::OnReparseCurrent));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_SAVE, new cbEventFunctor<ToDoList, CodeBlocksEvent>(this, &ToDoList::OnReparseCurrent));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_ACTIVATED, new cbEventFunctor<ToDoList, CodeBlocksEvent>(this, &ToDoList::OnReparseCurrent));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_CLOSE, new cbEventFunctor<ToDoList, CodeBlocksEvent>(this, &ToDoList::OnReparseCurrent));
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_CLOSE, new cbEventFunctor<ToDoList, CodeBlocksEvent>(this, &ToDoList::OnReparse));
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_ACTIVATE, new cbEventFunctor<ToDoList, CodeBlocksEvent>(this, &ToDoList::OnReparse));
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_FILE_ADDED, new cbEventFunctor<ToDoList, CodeBlocksEvent>(this, &ToDoList::OnReparse));
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_FILE_REMOVED, new cbEventFunctor<ToDoList, CodeBlocksEvent>(this, &ToDoList::OnReparse));
}

void ToDoList::OnRelease(cb_unused bool appShutDown)
{
    if (m_StandAlone)
    {
        CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
        evt.pWindow = m_pListLog->GetWindow();
        Manager::Get()->ProcessEvent(evt);
        m_pListLog->DestroyControls(true);
        delete m_pListLog;
    }
    else
    {
        CodeBlocksLogEvent evt(cbEVT_REMOVE_LOG_WINDOW, m_pListLog);
        m_pListLog->DestroyControls(false);
        Manager::Get()->ProcessEvent(evt);
    }
    m_pListLog = nullptr;
}

void ToDoList::BuildMenu(wxMenuBar* menuBar)
{
    int idx = menuBar->FindMenu(_("&View"));
    if (idx != wxNOT_FOUND)
    {
        wxMenu* view = menuBar->GetMenu(idx);
        wxMenuItemList& items = view->GetMenuItems();
        // find the first separator and insert before it
        bool done = false;
        for (size_t i = 0; i < items.GetCount(); ++i)
        {
            if (items[i]->IsSeparator())
            {
                view->InsertCheckItem(i, idViewTodo, _("Todo list"), _("Toggle displaying the To-Do list"));
                done = true;
                break;
            }
        }
        // not found, just append
        if ( !done )
            view->AppendCheckItem(idViewTodo, _("Todo list"), _("Toggle displaying the To-Do list"));
    }

    idx = menuBar->FindMenu(_("&Edit"));
    if (idx != wxNOT_FOUND)
    {
        wxMenu* edit = menuBar->GetMenu(idx);
        edit->AppendSeparator();
        edit->Append(idAddTodo, _("Add Todo item..."), _("Add Todo item..."));
    }

}

void ToDoList::BuildModuleMenu(const ModuleType type, wxMenu* menu, cb_unused const FileTreeData* data)
{
    if (!menu || !IsAttached())
        return;
    if (type == mtEditorManager)
    {
        menu->AppendSeparator();
        menu->Append(idAddTodo, _("Add Todo item..."), _("Add new Todo item..."));
    }
}

cbConfigurationPanel* ToDoList::GetConfigurationPanel(wxWindow* parent)
{
    ToDoSettingsDlg* dlg = new ToDoSettingsDlg(parent);
    return dlg;
}

void ToDoList::LoadUsers()
{
    m_Users.Clear();

    Manager::Get()->GetConfigManager(_T("todo_list"))->Read(_T("users"), &m_Users);

    if (m_Users.GetCount() == 0)
        m_Users.Add(wxGetUserId());

    SaveUsers();
}

void ToDoList::SaveUsers()
{
    Manager::Get()->GetConfigManager(_T("todo_list"))->Write(_T("users"), m_Users);
}

void ToDoList::LoadTypes()
{
    m_Types.Clear();

    Manager::Get()->GetConfigManager(_T("todo_list"))->Read(_T("types"), &m_Types);

    if (m_Types.GetCount() == 0)
    {
        m_Types.Add(_T("TODO"));
        m_Types.Add(_T("@todo"));
        m_Types.Add(_T("\\todo"));

        m_Types.Add(_T("FIXME"));
        m_Types.Add(_T("@fixme"));
        m_Types.Add(_T("\\fixme"));

        m_Types.Add(_T("NOTE"));
        m_Types.Add(_T("@note"));
        m_Types.Add(_T("\\note"));
    }
    m_pListLog->m_pAllowedTypesDlg->Clear();
    m_pListLog->m_pAllowedTypesDlg->AddItem(m_Types);

    wxArrayString selectedTypes;
    Manager::Get()->GetConfigManager(_T("todo_list"))->Read(_T("types_selected"), &selectedTypes);

    if (selectedTypes.GetCount()==0)
    {
      // initially, all types are selected
      for (size_t i=0; i<m_Types.GetCount(); i++)
        selectedTypes.Add(m_Types[i]);
    }

    m_pListLog->m_pAllowedTypesDlg->SetChecked(selectedTypes);
    SaveTypes();
}

void ToDoList::SaveTypes()
{
    Manager::Get()->GetConfigManager(_T("todo_list"))->Write(_T("types"), m_Types);
}

// events

void ToDoList::OnAppDoneStartup(CodeBlocksEvent& event)
{
    m_InitDone = true;
    Parse();
    event.Skip();
}

void ToDoList::OnUpdateUI(cb_unused wxUpdateUIEvent& event)
{
    Manager::Get()->GetAppFrame()->GetMenuBar()->Check(idViewTodo, IsWindowReallyShown(m_pListLog->GetWindow()));
}

void ToDoList::OnUpdateAdd(wxUpdateUIEvent& event)
{
    event.Enable(Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor() != NULL);
}

void ToDoList::OnViewList(wxCommandEvent& event)
{
    if (m_StandAlone)
    {
        CodeBlocksDockEvent evt(event.IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
        evt.pWindow = m_pListLog->GetWindow();
        Manager::Get()->ProcessEvent(evt);
    }
    else
    {
        if (event.IsChecked())
        {
            CodeBlocksLogEvent evtShow(cbEVT_SHOW_LOG_MANAGER);
            Manager::Get()->ProcessEvent(evtShow);
            CodeBlocksLogEvent evt(cbEVT_SWITCH_TO_LOG_WINDOW, m_pListLog);
            Manager::Get()->ProcessEvent(evt);
        }
        else
        {
            CodeBlocksLogEvent evt(cbEVT_HIDE_LOG_WINDOW, m_pListLog);
            Manager::Get()->ProcessEvent(evt);
        }
    }
}

void ToDoList::OnAddItem(cb_unused wxCommandEvent& event)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed)
        return;

    HighlightLanguage hlang = Manager::Get()->GetEditorManager()->GetColourSet()->GetLanguageName(ed->GetLanguage());
    bool edIsCCpp = (hlang == _T("C/C++"));

    CommentToken token = Manager::Get()->GetEditorManager()->GetColourSet()->GetCommentToken(ed->GetLanguage());
    bool hasStreamComments = not token.streamCommentStart.IsEmpty();
    bool hasLineComments = not token.lineComment.IsEmpty();

    if (!(edIsCCpp||hasLineComments||hasStreamComments))
        return;
    std::bitset<(int)tdctError+1> supportedTdcts;
    supportedTdcts[tdctLine] = !(token.lineComment.IsEmpty());
    supportedTdcts[tdctStream] = !(token.streamCommentStart.IsEmpty());
    supportedTdcts[tdctDoxygenLine] = !(token.doxygenLineComment.IsEmpty());
    supportedTdcts[tdctDoxygenStream] = !(token.doxygenStreamCommentStart.IsEmpty());
    supportedTdcts[tdctWarning] = edIsCCpp;
    supportedTdcts[tdctError] = edIsCCpp;
    // display todo dialog
    AddTodoDlg dlg(Manager::Get()->GetAppWindow(), m_Users, m_Types, supportedTdcts );
    PlaceWindow(&dlg);
    if (dlg.ShowModal() != wxID_OK)
        return;
    // Re-load users and types as they might have changed by the user via AddTodoDlg
    LoadUsers();
    LoadTypes();

    cbStyledTextCtrl* control = ed->GetControl();

    // calculate insertion point
    int idx = 0;
    int crlfLen = 0; // length of newline chars
    int origPos = control->GetCurrentPos(); // keep current position in the document
    int line = control->GetCurrentLine(); // current line
    ToDoCommentType CmtType = dlg.GetCommentType();
    if (dlg.GetPosition() == tdpCurrent)
    {
        idx = control->GetCurrentPos(); // current position in the document
        // if the style is cpp comments (// ...), there's the possibility that the current position
        // is somewhere in the middle of a line of code; this would result
        // in everything after the insertion point to turn into comments
        // let's double check this with the user
        if (idx != control->GetLineEndPosition(line) && (CmtType != tdctStream) && (CmtType != tdctDoxygenStream))
        {
            // let's ask the user, and present as options
            // keep cpp style at current position, switch to c style, add the todo at the end (keeping cpp style)
            // if user cancels out / do nothing : just return
            // future idea : check if there's any non white space character
            // if yes -> in the middle of code
            // if no -> then only whitespace after the insertion point -> no harm to turn that into comments
            wxString streamStart = token.streamCommentStart, streamEnd = token.streamCommentEnd;
            if (CmtType == tdctDoxygenLine && !token.doxygenStreamCommentStart.IsEmpty())
            {
                streamStart = token.doxygenStreamCommentStart;
                streamEnd = token.doxygenStreamCommentEnd;
            }
            AskTypeDlg asktype_dlg(Manager::Get()->GetAppWindow(), streamStart, streamEnd);
            PlaceWindow(&asktype_dlg);
            if (asktype_dlg.ShowModal() != wxID_OK)
                return;
            switch(asktype_dlg.GetTypeCorrection())
            {
                case tcStay:
                    break; // do nothing, leave things as they are
                case tcSwitch:
                    if (CmtType == tdctDoxygenLine)
                        CmtType = tdctDoxygenStream;
                    else
                        CmtType = tdctStream;
                    break;
                case tcMove:
                default:
                    idx = control->GetLineEndPosition(line);
                    break;
            } // end switch
        }
    }
    else
    {
        if      (dlg.GetPosition() == tdpAbove)
            idx = control->GetLineEndPosition(line - 1); // get previous line's end
        else if (dlg.GetPosition() == tdpBelow)
            idx = control->GetLineEndPosition(line); // get current line's end
        // calculate insertion point by skipping next newline
        switch (control->GetEOLMode())
        {
            case wxSCI_EOL_CR:
            case wxSCI_EOL_LF:   crlfLen = 1; break;
            case wxSCI_EOL_CRLF: crlfLen = 2; break;
        }
        if (idx > 0)
            idx += crlfLen;
    }
    // make sure insertion point is valid (bug #1300981)
    if (idx > control->GetLength())
        idx = control->GetLength();

    // ok, construct todo line text like this:
    // TODO (mandrav#0#): Implement code to do this and the other...
    wxString buffer;

    // start with the comment
    switch(CmtType)
    {
        default:
        case tdctLine:
            buffer << token.lineComment;
            break;
        case tdctDoxygenLine:
            buffer << token.doxygenLineComment;
            break;
        case tdctDoxygenStream:
            buffer << token.doxygenStreamCommentStart;
            break;
        case tdctWarning:
            buffer << _T("#warning");
            break;
        case tdctError:
            buffer << _T("#error");
            break;
        case tdctStream:
            buffer << token.streamCommentStart;
            break;
    } // end switch
    buffer << _T(" ");

    // continue with the type
    buffer << dlg.GetType() << _T(" ");
    wxString priority = wxString::Format(_T("%d"), dlg.GetPriority()); // do it like this (wx bug with int and streams)

    // now do the () part
    buffer << _T("(") << dlg.GetUser() << _T("#") << priority << _T("#): ");

    wxString text = dlg.GetText();

    // make sure that multi-line notes, don't break the todo
    if ( (CmtType == tdctWarning) || (CmtType == tdctError) )
    {
        if (text.Replace(_T("\r\n"), _T("\\\r\n")) == 0)
            text.Replace(_T("\n"), _T("\\\n"));
        // now see if there were already a backslash before newline
        if (text.Replace(_T("\\\\\r\n"), _T("\\\r\n")) == 0)
            text.Replace(_T("\\\\\n"), _T("\\\n"));
    }
    else if (CmtType == tdctLine || (CmtType == tdctDoxygenLine))
    {
        wxString lc;
        if (CmtType == tdctLine)
            lc = token.lineComment;
        else
            lc = token.doxygenLineComment;
        // comment every line from the todo text
        if ( text.Replace(_T("\r\n"), _T("\r\n")+lc) == 0 )
            text.Replace(_T("\n"), _T("\n")+lc);
        // indicate (on the first line) that there is some more text
        if ( text.Replace(_T("\r\n"), _T(" ...\r\n"),false) == 0 )
            text.Replace(_T("\n"), _T(" ...\n"),false);
    }

    // add the actual text
    buffer << text;

    if ( CmtType == tdctStream )
        buffer << _T(" ") << token.streamCommentEnd;
    if ( CmtType == tdctDoxygenStream )
        buffer << _T(" ") << token.doxygenStreamCommentEnd;

    // add newline char(s), only if dlg.GetPosition() != tdpCurrent
    if (dlg.GetPosition() != tdpCurrent)
    {
        switch (control->GetEOLMode())
        {
            // NOTE: maybe this switch, should make it in the SDK (maybe as cbStyledTextCtrl::GetEOLString())???
            case wxSCI_EOL_CR: buffer << _T("\r"); break;
            case wxSCI_EOL_CRLF: buffer << _T("\r\n"); break;
            case wxSCI_EOL_LF: buffer << _T("\n"); break;
        }
    }

    // ok, insert the todo line text
    control->InsertText(idx, buffer);
    if (dlg.GetPosition() == tdpAbove)
        origPos += buffer.Length() + crlfLen;
    control->GotoPos(origPos);
    control->EnsureCaretVisible();

    ParseCurrent(true);
} // end of OnAddItem

void ToDoList::OnReparse(CodeBlocksEvent& event)
{
    if (m_InitDone && m_AutoRefresh && !(ProjectManager::IsBusy()))
        Parse();
    else
    {
        m_ParsePending = true;
        m_pListLog->Clear();
    }
    event.Skip();
}

void ToDoList::OnReparseCurrent(CodeBlocksEvent& event)
{
    bool forced = (event.GetEventType() == cbEVT_EDITOR_OPEN || event.GetEventType() == cbEVT_EDITOR_SAVE);
    if (m_InitDone && m_AutoRefresh && !(ProjectManager::IsBusy()))
    {
        if (m_ParsePending)
        {
            m_ParsePending = false;
            Parse();
        }
        else
            ParseCurrent(forced);
    }
    event.Skip();
}

void ToDoList::ParseCurrent(bool forced)
{
    m_pListLog->ParseCurrent(forced);
}

void ToDoList::Parse()
{
    m_pListLog->Parse();
}
