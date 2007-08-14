/***************************************************************
 * Name:      todolist.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    Yiannis Mandravellos <mandrav@codeblocks.org>
 * Created:   11/21/03 14:01:50
 * Copyright: (c) Yiannis Mandravellos
 * License:   GPL
 **************************************************************/
#include "sdk.h"
#ifndef CB_PRECOMP
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
//#include "messagemanager.h"
#include "sdk_events.h"
#endif

#include "addtododlg.h"
#include "asktypedlg.h"
#include "todolist.h"
#include "todolistview.h"
#include "todosettingsdlg.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ToDoItems);

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
	EVT_MENU(idAddTodo, ToDoList::OnAddItem)
END_EVENT_TABLE()

ToDoList::ToDoList() :
m_InitDone(false),
m_ParsePending(false),
m_StandAlone(true)
{
	//ctor
    if(!Manager::LoadResource(_T("todo.zip")))
    {
        NotifyMissingFile(_T("todo.zip"));
    }
}

ToDoList::~ToDoList()
{
	//dtor
}

void ToDoList::OnAttach()
{
	// create ToDo in bottom view
	wxArrayString titles;
	int widths[6] = {64, 320, 64, 48, 48, 640};
	titles.Add(_("Type"));
	titles.Add(_("Text"));
	titles.Add(_("User"));
	titles.Add(_("Prio."));
	titles.Add(_("Line"));
	titles.Add(_("File"));

	m_pListLog = new ToDoListView(6, widths, titles, m_Types);
	m_pListLog->SetSize(wxSize(352,94));
    #if wxCHECK_VERSION(2, 8, 0)
    m_pListLog->SetInitialSize(wxSize(352,94));
    #else
    m_pListLog->SetBestFittingSize(wxSize(352,94));
    #endif

    bool standalone = Manager::Get()->GetConfigManager(_T("todo_list"))->ReadBool(_T("stand_alone"), true);
    m_StandAlone = standalone;

    if(!standalone)
    {
        MessageManager* msgMan = Manager::Get()->GetMessageManager();
        m_ListPageIndex = msgMan->AddLog(m_pListLog, _("To-Do"));
    }
    else
    {
        CodeBlocksDockEvent evt(cbEVT_ADD_DOCK_WINDOW);
        evt.name = _T("TodoListPanev2.0.0");
        evt.title = _("To-Do list");
        evt.pWindow = m_pListLog;
        evt.dockSide = CodeBlocksDockEvent::dsFloating;
        evt.desiredSize.Set(352, 94);
        evt.floatingSize.Set(352, 94);
        evt.minimumSize.Set(352, 94);
        Manager::Get()->ProcessEvent(evt);
    }

    m_AutoRefresh = Manager::Get()->GetConfigManager(_T("todo_list"))->ReadBool(_T("auto_refresh"), true);
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

void ToDoList::OnRelease(bool appShutDown)
{
    if(m_StandAlone)
    {
        CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
        evt.pWindow = m_pListLog;
        Manager::Get()->ProcessEvent(evt);
        m_pListLog->Destroy();
    }
    else
    {
        if (Manager::Get()->GetMessageManager())
            Manager::Get()->GetMessageManager()->RemoveLog(m_pListLog);
    }
    m_pListLog = 0;
}

void ToDoList::BuildMenu(wxMenuBar* menuBar)
{
    int idx = menuBar->FindMenu(_("View"));
    if (idx != wxNOT_FOUND)
    {
        wxMenu* view = menuBar->GetMenu(idx);
        wxMenuItemList& items = view->GetMenuItems();
        // find the first separator and insert before it
        for (size_t i = 0; i < items.GetCount(); ++i)
        {
            if (items[i]->IsSeparator())
            {
                view->InsertCheckItem(i, idViewTodo, _("To-Do list"), _("Toggle displaying the To-Do list"));
                return;
            }
        }
        // not found, just append
        view->AppendCheckItem(idViewTodo, _("To-Do list"), _("Toggle displaying the To-Do list"));
    }
}

void ToDoList::BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data)
{
	if (!menu || !IsAttached())
		return;
	if (type == mtEditorManager)
	{
		menu->AppendSeparator();
		menu->Append(idAddTodo, _("Add To-Do item..."), _("Add new To-Do item..."));
	}
}

bool ToDoList::BuildToolBar(wxToolBar* toolBar)
{
	return false;
}

cbConfigurationPanel* ToDoList::GetConfigurationPanel(wxWindow* parent)
{
    ToDoSettingsDlg* dlg = new ToDoSettingsDlg(parent);
    return dlg;
}

int ToDoList::Configure()
{
    return 0;
//    ToDoSettingsDlg dlg;
//    PlaceWindow(&dlg);
//    if (dlg.ShowModal() == wxID_OK)
//        m_AutoRefresh = Manager::Get()->GetConfigManager(_T("todo_list"))->ReadBool(_T("auto_refresh"), true);
//	return 0;
}

void ToDoList::LoadTypes()
{
    m_Types.Clear();

	Manager::Get()->GetConfigManager(_T("todo_list"))->Read(_T("types"), &m_Types);

	if(m_Types.GetCount() == 0)
	{
        m_Types.Add(_T("TODO"));
        m_Types.Add(_T("@todo"));
        m_Types.Add(_T("\\todo"));
        m_Types.Add(_T("FIXME"));
        m_Types.Add(_T("NOTE"));
        m_Types.Add(_T("@note"));
        m_Types.Add(_T("\\note"));
	}
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

void ToDoList::OnUpdateUI(wxUpdateUIEvent& event)
{
    Manager::Get()->GetAppFrame()->GetMenuBar()->Check(idViewTodo, IsWindowReallyShown(m_pListLog));
}

void ToDoList::OnViewList(wxCommandEvent& event)
{
    CodeBlocksDockEvent evt(event.IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = m_pListLog;
    Manager::Get()->ProcessEvent(evt);
}

void ToDoList::OnAddItem(wxCommandEvent& event)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
	if (!ed)
		return;

    // display todo dialog
    AddTodoDlg dlg(Manager::Get()->GetAppWindow(), m_Types);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() != wxID_OK)
        return;
    SaveTypes();

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
		if(idx != control->GetLineEndPosition(line))
		{
			// let's ask the user, and present as options
			// keep cpp style at current position, switch to c style, add the todo at the end (keeping cpp style)
			// if user cancels out / do nothing : just return
			// future idea : check if there's any non white space character
			// if yes -> in the middle of code
			// if no -> then only whitespace after the insertion point -> no harm to turn that into comments
			AskTypeDlg dlg(Manager::Get()->GetAppWindow());
			PlaceWindow(&dlg);
			if (dlg.ShowModal() != wxID_OK)
				return;
			switch(dlg.GetTypeCorrection())
			{
				case tcCppStay:
					break; // do nothing, leave things as they are
				case tcCpp2C:
					CmtType = tdctC;
					break;
				case tcCppMove:
				default:
					idx = control->GetLineEndPosition(line);
					break;
			} // end switch
		}
	}
	else
	{
		if (dlg.GetPosition() == tdpAbove)
			idx = control->GetLineEndPosition(line - 1); // get previous line's end
		else if (dlg.GetPosition() == tdpBelow)
			idx = control->GetLineEndPosition(line); // get current line's end
		// calculate insertion point by skipping next newline
		switch (control->GetEOLMode())
		{
			case wxSCI_EOL_CR:
			case wxSCI_EOL_LF: crlfLen = 1; break;
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
		case tdctCpp:
			buffer << _T("// ");
			break;
		case tdctDoxygen:
			buffer << _T("/// ");
			break;
		case tdctWarning:
			buffer << _T("#warning ");
			break;
		case tdctError:
			buffer << _T("#error ");
			break;
		default:
			buffer << _T("/* ");
			break;
	} // end switch

    // continue with the type
	buffer << dlg.GetType() << _T(" ");
	wxString priority = wxString::Format(_T("%d"), dlg.GetPriority()); // do it like this (wx bug with int and streams)

	// now do the () part
	buffer << _T("(") << dlg.GetUser() << _T("#") << priority << _T("#): ");

    wxString text = dlg.GetText();
    if (CmtType != tdctC)
    {
        // make sure that multi-line notes, don't break the to-do
        if (text.Replace(_T("\r\n"), _T("\\\r\n")) == 0)
            text.Replace(_T("\n"), _T("\\\n"));
        // now see if there were already a backslash before newline
        if (text.Replace(_T("\\\\\r\n"), _T("\\\r\n")) == 0)
            text.Replace(_T("\\\\\n"), _T("\\\n"));
    }

	// add the actual text
	buffer << text;

    if (CmtType == tdctWarning || CmtType == tdctError)
        buffer << _T("");

    else if (CmtType == tdctC)
		buffer << _T(" */");

	// add newline char(s), only if dlg.GetPosition() != tdpCurrent
	if (dlg.GetPosition() != tdpCurrent)
	{
		switch (control->GetEOLMode())
		{
			// NOTE: maybe this switch, should make it in the SDK (maybe as cbStyledTextCtrl::GetEOLString())???
			case wxSCI_EOL_CR: buffer << _T("\n"); break;
			case wxSCI_EOL_CRLF: buffer << _T("\r\n"); break;
			case wxSCI_EOL_LF: buffer << _T("\r"); break;
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
    {
        Parse();
    }
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
        if(m_ParsePending)
        {
            m_ParsePending = false;
            Parse();
        }
        else
        {
            ParseCurrent(forced);
        }
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
