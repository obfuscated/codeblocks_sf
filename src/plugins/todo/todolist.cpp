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
#include <cbeditor.h>
#include <licenses.h>
#include "todolist.h"
#include "addtododlg.h"
#include "viewtododlg.h"

cbPlugin* GetPlugin()
{
	return new ToDoList;
}

const int idAddTodo = wxNewId();
const int idViewTodo = wxNewId();

BEGIN_EVENT_TABLE(ToDoList, cbPlugin)
	EVT_MENU(idAddTodo, ToDoList::OnAddItem)
	EVT_MENU(idViewTodo, ToDoList::OnViewList)
END_EVENT_TABLE()

ToDoList::ToDoList()
	: m_pMenu(0L)
{
	//ctor
    wxFileSystem::AddHandler(new wxZipFSHandler);
    wxXmlResource::Get()->InitAllHandlers();
    wxString resPath = ConfigManager::Get()->Read("data_path", wxEmptyString);
    wxXmlResource::Get()->Load(resPath + "/todo.zip");

	m_PluginInfo.name = "ToDoList";
	m_PluginInfo.title = "To-Do List";
	m_PluginInfo.version = "0.1";
	m_PluginInfo.description = "Code::Blocks To-Do List plugin";
    m_PluginInfo.author = "Yiannis An. Mandravellos";
    m_PluginInfo.authorEmail = "info@codeblocks.org";
    m_PluginInfo.authorWebsite = "www.codeblocks.org";
	m_PluginInfo.thanksTo = "";
	m_PluginInfo.license = LICENSE_GPL;
	m_PluginInfo.hasConfigure = false;
}

ToDoList::~ToDoList()
{
	//dtor
}

void ToDoList::OnAttach()
{
}

void ToDoList::OnRelease()
{
	if (m_pMenu)
		m_pMenu->Delete(idViewTodo);
}

void ToDoList::BuildMenu(wxMenuBar* menuBar)
{
	if (!menuBar)
		return;

	wxMenu* menu = 0L;
	int idx = menuBar->FindMenu(_("View"));
	if (idx != wxNOT_FOUND)
		menu = menuBar->GetMenu(idx);

	m_pMenu = menu;
	if (!menu)
		return;

	menu->Append(idViewTodo, _("To-Do list"), _("View list of To-Do items"));
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
	return;
}

// events

void ToDoList::OnAddItem(wxCommandEvent& event)
{
	cbEditor* ed = Manager::Get()->GetEditorManager()->GetActiveEditor();
	if (!ed)
		return;

    // display todo dialog
    AddTodoDlg dlg(Manager::Get()->GetAppWindow());
    if (dlg.ShowModal() != wxID_OK)
        return;

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
	switch (dlg.GetType())
	{
		case tdtToDo: buffer << "TODO "; break;
		case tdtFixMe: buffer << "FIXME "; break;
		case tdtNote: buffer << "NOTE "; break;
	}

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
}

void ToDoList::OnViewList(wxCommandEvent& event)
{
    // display todo list
    ViewTodoDlg dlg(Manager::Get()->GetAppWindow());
    if (dlg.ShowModal() != wxID_OK)
        return;
	
	// jump to file/line selected
	cbEditor* ed = Manager::Get()->GetEditorManager()->Open(dlg.GetFilename());
	if (ed)
		ed->GetControl()->GotoLine(dlg.GetLine());
}
