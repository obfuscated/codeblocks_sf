/***************************************************************
 * Name:      todolist.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    Yiannis Mandravellos <mandrav@codeblocks.org>
 * Created:   11/21/03 14:01:50
 * Copyright: (c) Yiannis Mandravellos
 * License:   GPL
 **************************************************************/

#if defined(__GNUG__) && !defined(__APPLE__)
	#pragma implementation "todolist.h"
#endif

#include <wx/intl.h>
#include <wx/textdlg.h>
#include <wx/xrc/xmlres.h>
#include <wx/fs_zip.h>
#include <manager.h>
#include <configmanager.h>
#include <editormanager.h>
#include <messagemanager.h>
#include <projectmanager.h>
#include <cbeditor.h>
#include <cbproject.h>
#include <licenses.h>
#include "todolist.h"
#include "addtododlg.h"
#include "todosettingsdlg.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ToDoItems);

cbPlugin* GetPlugin()
{
	return new ToDoList;
}

const int idAddTodo = wxNewId();

BEGIN_EVENT_TABLE(ToDoList, cbPlugin)
	EVT_MENU(idAddTodo, ToDoList::OnAddItem)
    EVT_EDITOR_OPEN(ToDoList::OnReparse)
    EVT_EDITOR_SAVE(ToDoList::OnReparse)
    EVT_PROJECT_CLOSE(ToDoList::OnReparse)
    EVT_PROJECT_ACTIVATE(ToDoList::OnReparse)
    EVT_PROJECT_FILE_ADDED(ToDoList::OnReparse)
    EVT_PROJECT_FILE_REMOVED(ToDoList::OnReparse)
END_EVENT_TABLE()

ToDoList::ToDoList()
{
	//ctor
    wxFileSystem::AddHandler(new wxZipFSHandler);
    wxXmlResource::Get()->InitAllHandlers();
    wxString resPath = ConfigManager::Get()->Read("data_path", wxEmptyString);
    wxXmlResource::Get()->Load(resPath + "/todo.zip#zip:*.xrc");

	m_PluginInfo.name = "ToDoList";
	m_PluginInfo.title = "To-Do List";
	m_PluginInfo.version = "0.1";
	m_PluginInfo.description = "Code::Blocks To-Do List plugin";
    m_PluginInfo.author = "Yiannis An. Mandravellos";
    m_PluginInfo.authorEmail = "info@codeblocks.org";
    m_PluginInfo.authorWebsite = "www.codeblocks.org";
	m_PluginInfo.thanksTo = "";
	m_PluginInfo.license = LICENSE_GPL;
	m_PluginInfo.hasConfigure = true;

	ConfigManager::AddConfiguration(m_PluginInfo.title, "/todo_list");
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

    MessageManager* msgMan = Manager::Get()->GetMessageManager();
	m_pListLog = new ToDoListView(msgMan, m_PluginInfo.title, 6, widths, titles, m_Types);
	m_ListPageIndex = msgMan->AddLog(m_pListLog);

    m_AutoRefresh = ConfigManager::Get()->Read("todo_list/auto_refresh", true);
    LoadTypes();
}

void ToDoList::OnRelease(bool appShutDown)
{
    if (Manager::Get()->GetMessageManager())
        Manager::Get()->GetMessageManager()->DeletePage(m_ListPageIndex);
}

void ToDoList::BuildMenu(wxMenuBar* menuBar)
{
    // nothing to be added in menu
}

void ToDoList::BuildModuleMenu(const ModuleType type, wxMenu* menu, const wxString& arg)
{
	if (!menu || !m_IsAttached)
		return;
	if (type == mtEditorManager)
	{
		menu->AppendSeparator();
		menu->Append(idAddTodo, _("Add To-Do item..."), _("Add new To-Do item..."));
	}
}

void ToDoList::BuildToolBar(wxToolBar* toolBar)
{
	//NotImplemented("ToDoList::BuildToolBar()");
}

int ToDoList::Configure()
{
    ToDoSettingsDlg dlg;
    if (dlg.ShowModal() == wxID_OK)
        m_AutoRefresh = ConfigManager::Get()->Read("todo_list/auto_refresh", true);
	return 0;
}

void ToDoList::LoadTypes()
{
    m_Types.Clear();
	long cookie;
	wxString entry;
	wxConfigBase* conf = ConfigManager::Get();
	wxString oldPath = conf->GetPath();
	conf->SetPath("/todo/types");
	bool cont = conf->GetFirstEntry(entry, cookie);
	while (cont)
	{
		m_Types.Add(entry);
		cont = conf->GetNextEntry(entry, cookie);
	}
	conf->SetPath(oldPath);
}

void ToDoList::SaveTypes()
{
	wxConfigBase* conf = ConfigManager::Get();
	conf->DeleteGroup("/todo/types");
	wxString oldPath = conf->GetPath();
	conf->SetPath("/todo/types");
	for (unsigned int i = 0; i < m_Types.GetCount(); ++i)
	{
		conf->Write(m_Types[i], wxEmptyString);
	}
	conf->SetPath(oldPath);
}

// events

void ToDoList::OnAddItem(wxCommandEvent& event)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
	if (!ed)
		return;

    // display todo dialog
    AddTodoDlg dlg(Manager::Get()->GetAppWindow(), m_Types);
    if (dlg.ShowModal() != wxID_OK)
        return;
    SaveTypes();

	cbStyledTextCtrl* control = ed->GetControl();
	
	// calculate insertion point
	int idx = 0;
	int crlfLen = 0; // length of newline chars
	int origPos = control->GetCurrentPos(); // keep current position in the document
	int line = control->GetCurrentLine(); // current line
	if (dlg.GetPosition() == tdpCurrent)
		idx = control->GetCurrentPos(); // current position in the document
	else
	{
		if (dlg.GetPosition() == tdpAbove)
			idx = control->GetLineEndPosition(line - 1); // get previous line's end
		else if (dlg.GetPosition() == tdpBelow)
			idx = control->GetLineEndPosition(line); // get current line's end
		// calculate insertion point by skipping next newline
		switch (control->GetEOLMode())
		{
			case wxSTC_EOL_CR:
			case wxSTC_EOL_LF: crlfLen = 1; break;
			case wxSTC_EOL_CRLF: crlfLen = 2; break;
		}
		idx += crlfLen;
	}
	
	// ok, construct todo line text like this:
    // TODO (mandrav#0#): Implement code to do this and the other...
	wxString buffer;
    
	// start with the comment
    if (dlg.GetCommentType() == tdctCpp && dlg.GetPosition() != tdpCurrent)
		buffer << "// "; // if tdpCurrent we can't use this type of comment...
	else
    {
        if (dlg.GetCommentType() == tdctWarning)
            buffer << "#warning ";
        else if (dlg.GetCommentType() == tdctError)
            buffer << "#error ";
        else
            buffer << "/* ";
    }

    // continue with the type
	buffer << dlg.GetType() << " ";

	// now do the () part
	buffer << "(" << dlg.GetUser() << "#" << dlg.GetPriority() << "#): ";

    wxString text = dlg.GetText();
    if (dlg.GetCommentType() != tdctC)
    {
        // make sure that multi-line notes, don't break the to-do
        if (text.Replace("\r\n", "\\\r\n") == 0)
            text.Replace("\n", "\\\n");
        // now see if there were already a backslash before newline
        if (text.Replace("\\\\\r\n", "\\\r\n") == 0)
            text.Replace("\\\\\n", "\\\n");
    }

	// add the actual text
	buffer << text;
	
    if (dlg.GetCommentType() == tdctWarning || dlg.GetCommentType() == tdctError)
        buffer << "";

    else if (dlg.GetCommentType() == tdctC || dlg.GetPosition() == tdpCurrent)
		buffer << " */";

	// add newline char(s), only if dlg.GetPosition() != tdpCurrent
	if (dlg.GetPosition() != tdpCurrent)
	{
		switch (control->GetEOLMode())
		{
			// NOTE: maybe this switch, should make it in the SDK (maybe as cbStyledTextCtrl::GetEOLString())???
			case wxSTC_EOL_CR: buffer << '\n'; break;
			case wxSTC_EOL_CRLF: buffer << "\r\n"; break;
			case wxSTC_EOL_LF: buffer << '\r'; break;
		}
	}

	// ok, insert the todo line text
	control->InsertText(idx, buffer);
	if (dlg.GetPosition() == tdpAbove)
		origPos += buffer.Length() + crlfLen;
	control->GotoPos(origPos);
	control->EnsureCaretVisible();
	
	m_pListLog->Parse();
}

void ToDoList::OnReparse(CodeBlocksEvent& event)
{
    if (m_AutoRefresh)
        m_pListLog->Parse();
    event.Skip();
}
