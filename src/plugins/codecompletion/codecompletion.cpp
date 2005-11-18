/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Id$
* $Date$
*/

#include <sdk.h>
#include "codecompletion.h"
#include <wx/mimetype.h>
#include <wx/filename.h>
#include <wx/regex.h>
#include <wx/xrc/xmlres.h>
#include <wx/fs_zip.h>
#include <wx/msgdlg.h>
#include <manager.h>
#include <configmanager.h>
#include <messagemanager.h>
#include <projectmanager.h>
#include <editormanager.h>
#include <sdk_events.h>
#include <incrementalselectlistdlg.h>
#include "insertclassmethoddlg.h"
#include "ccoptionsdlg.h"
#include "parser/parser.h"
#include "cclist.h"

CB_IMPLEMENT_PLUGIN(CodeCompletion);

// menu IDS
// just because we don't know other plugins' used identifiers,
// we use wxNewId() to generate a guaranteed unique ID ;), instead of enum
// (don't forget that, especially in a plugin)
int idMenuCodeComplete = wxNewId();
int idMenuShowCallTip = wxNewId();
int idMenuGotoFunction = wxNewId();
int idEditorSubMenu = wxNewId();
int idClassMethod = wxNewId();
int idGotoDeclaration = wxNewId();
int idOpenIncludeFile = wxNewId();

BEGIN_EVENT_TABLE(CodeCompletion, cbCodeCompletionPlugin)
	EVT_UPDATE_UI_RANGE(idMenuCodeComplete, idMenuGotoFunction, CodeCompletion::OnUpdateUI)

	EVT_MENU(idMenuCodeComplete, CodeCompletion::OnCodeComplete)
	EVT_MENU(idMenuShowCallTip, CodeCompletion::OnShowCallTip)
	EVT_MENU(idMenuGotoFunction, CodeCompletion::OnGotoFunction)
	EVT_MENU(idClassMethod, CodeCompletion::OnClassMethod)
	EVT_MENU(idGotoDeclaration, CodeCompletion::OnGotoDeclaration)
	EVT_MENU(idOpenIncludeFile, CodeCompletion::OnOpenIncludeFile)

	EVT_EDITOR_AUTOCOMPLETE(CodeCompletion::OnCodeComplete)
	EVT_EDITOR_CALLTIP(CodeCompletion::OnShowCallTip)
	EVT_EDITOR_USERLIST_SELECTION(CodeCompletion::OnUserListSelection)
	EVT_EDITOR_SAVE(CodeCompletion::OnReparseActiveEditor)
	EVT_EDITOR_ACTIVATED(CodeCompletion::OnEditorActivated)

	EVT_PROJECT_OPEN(CodeCompletion::OnProjectOpened)
	EVT_PROJECT_ACTIVATE(CodeCompletion::OnProjectActivated)
	EVT_PROJECT_CLOSE(CodeCompletion::OnProjectClosed)
	EVT_PROJECT_FILE_ADDED(CodeCompletion::OnProjectFileAdded)
	EVT_PROJECT_FILE_REMOVED(CodeCompletion::OnProjectFileRemoved)

	EVT_CCLIST_CODECOMPLETE(CodeCompletion::OnCodeComplete)
END_EVENT_TABLE()

CodeCompletion::CodeCompletion()
{
    wxFileSystem::AddHandler(new wxZipFSHandler);
    wxXmlResource::Get()->InitAllHandlers();
    wxString resPath = ConfigManager::GetDataFolder();
    wxXmlResource::Get()->Load(resPath + _T("/code_completion.zip#zip:*.xrc"));

    m_PluginInfo.name = _T("CodeCompletion");
    m_PluginInfo.title = _("Code completion");
    m_PluginInfo.version = _T("0.1");
    m_PluginInfo.description = _("This plugin provides a class browser for your projects "
                               "and code-completion inside the editor\n\n"
                               "Note: Only C/C++ language is supported by this plugin...");
    m_PluginInfo.author = _T("Yiannis An. Mandravellos");
    m_PluginInfo.authorEmail = _T("info@codeblocks.org");
    m_PluginInfo.authorWebsite = _T("www.codeblocks.org");
    m_PluginInfo.thanksTo = _T("");
	m_PluginInfo.hasConfigure = true;

    m_PageIndex = -1;
    m_EditMenu = 0L;
	m_SearchMenu = 0L;
}

CodeCompletion::~CodeCompletion()
{
}

int CodeCompletion::Configure()
{
	CCOptionsDlg dlg(Manager::Get()->GetAppWindow());
	if (dlg.ShowModal() == wxID_OK)
	{
		m_NativeParsers.RereadParserOptions();
	}
	return 0;
}

void CodeCompletion::BuildMenu(wxMenuBar* menuBar)
{
    // if not attached, exit
    if (!m_IsAttached)
        return;

//	if (m_EditMenu)
//    	return; // already set-up

    int pos = menuBar->FindMenu(_("&Edit"));
    if (pos != wxNOT_FOUND)
    {
		m_EditMenu = menuBar->GetMenu(pos);
        m_EditMenu->AppendSeparator();
        m_EditMenu->Append(idMenuCodeComplete, _("Complete code\tCtrl-Space"));
        m_EditMenu->Append(idMenuShowCallTip, _("Show call tip\tCtrl-Shift-Space"));
    }
    else
    	Manager::Get()->GetMessageManager()->DebugLog(_("Could not find Edit menu!"));
    pos = menuBar->FindMenu(_("Sea&rch"));
    if (pos != wxNOT_FOUND)
    {
		m_SearchMenu = menuBar->GetMenu(pos);
        m_SearchMenu->Append(idMenuGotoFunction, _("Goto function...\tCtrl-Alt-G"));
    }
    else
    	Manager::Get()->GetMessageManager()->DebugLog(_("Could not find Search menu!"));
}

void CodeCompletion::BuildModuleMenu(const ModuleType type, wxMenu* menu, const wxString& arg)
{
    // if not attached, exit
	if (!menu || !m_IsAttached)
		return;

	if (type == mtEditorManager)
	{
	    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
	    if (ed)
	    {
	        m_LastIncludeFileFrom = ed->GetFilename();
            cbStyledTextCtrl* control = ed->GetControl();
            int pos = control->GetCurrentPos();
            wxString line = control->GetLine(control->LineFromPosition(pos));

            wxRegEx reg(_T("^[ \t]*#[ \t]*include[ \t]+[\"<]([^\">]+)[\">]"));
            wxString inc;
            if (reg.Matches(line))
                inc = reg.GetMatch(line, 1);
            m_LastIncludeFile.Clear();
	        if (!inc.IsEmpty())
	        {
                wxString msg;
                msg.Printf(_("Open #include file: '%s'"), inc.c_str());
                menu->Insert(0, idOpenIncludeFile, msg);
                menu->Insert(1, wxID_SEPARATOR, wxEmptyString);
                m_LastIncludeFile = inc;
	        }
	        else // either #include or keyword-search
	        {
                int ws = control->WordStartPosition(pos, true);
                int we = control->WordEndPosition(pos, true);
                wxString txt = control->GetTextRange(ws, we);
                m_LastKeyword.Clear();
                if (!txt.IsEmpty())
                {
                    wxString msg;
                    msg.Printf(_("Find declaration of: '%s'"), txt.c_str());
                    menu->Insert(0, idGotoDeclaration, msg);
                    menu->Insert(1, wxID_SEPARATOR, wxEmptyString);
                    m_LastKeyword = txt;
                }
	        }
	    }
        int insertId = menu->FindItem(_("Insert..."));
        if (insertId != wxNOT_FOUND)
        {
            wxMenuItem* insertMenu = menu->FindItem(insertId, NULL);
            if (insertMenu)
            {
                wxMenu* subMenu = insertMenu->GetSubMenu();
                if (subMenu)
                {
                    subMenu->Append(idClassMethod, _("Class method declaration/implementation..."));
                }
                else
                    Manager::Get()->GetMessageManager()->DebugLog(_("Could not find Insert menu 3!"));
            }
            else
                Manager::Get()->GetMessageManager()->DebugLog(_("Could not find Insert menu 2!"));
        }
        else
            Manager::Get()->GetMessageManager()->DebugLog(_("Could not find Insert menu!"));
	}
}

bool CodeCompletion::BuildToolBar(wxToolBar* toolBar)
{
	// no need for toolbar items
	return false;
}

void CodeCompletion::OnAttach()
{
	m_NativeParsers.CreateClassBrowser();

	// parse all active projects
	ProjectManager* prjMan = Manager::Get()->GetProjectManager();
	for (unsigned int i = 0; i < prjMan->GetProjects()->GetCount(); ++i)
		m_NativeParsers.AddParser(prjMan->GetProjects()->Item(i));
}

void CodeCompletion::OnRelease(bool appShutDown)
{
	m_NativeParsers.RemoveClassBrowser(appShutDown);
	m_NativeParsers.ClearParsers();
	CCList::Free();

/* TODO (mandrav#1#): Delete separator line too... */
	if (m_EditMenu)
	{
		m_EditMenu->Delete(idMenuCodeComplete);
		m_EditMenu->Delete(idMenuShowCallTip);
	}
	if (m_SearchMenu)
		m_SearchMenu->Delete(idMenuGotoFunction);
}

int CodeCompletion::CodeComplete()
{
	if (!m_IsAttached)
		return -1;

	EditorManager* edMan = Manager::Get()->GetEditorManager();
    if (!edMan)
		return -2;
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
    if (!ed)
		return -3;

	FileType ft = FileTypeOf(ed->GetShortName());
	if (ft != ftHeader && ft != ftSource) // only parse source/header files
		return -4;

	Parser* parser = m_NativeParsers.FindParserFromEditor(ed);
	if (!parser)
	{
		Manager::Get()->GetMessageManager()->DebugLog(_("Active editor has no associated parser ?!?"));
		return -4;
	}

    if (m_NativeParsers.MarkItemsByAI(parser->Options().useSmartSense))
    {
        CCList::Free(); // free any previously open cc list
        CCList::Get(this, ed->GetControl(), parser)->Show();
        return 0;
    }
	return -5;
}

void CodeCompletion::CodeCompleteIncludes()
{
	if (!m_IsAttached)
		return;

    cbProject* pPrj = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (!pPrj)
        return;

	EditorManager* edMan = Manager::Get()->GetEditorManager();
    if (!edMan)
		return;
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
    if (!ed)
		return;

	Parser* parser = m_NativeParsers.FindParserFromActiveEditor();
	bool caseSens = parser ? parser->Options().caseSensitive : false;

	FileType ft = FileTypeOf(ed->GetShortName());
	if ( ft != ftHeader && ft != ftSource) // only parse source/header files
		return;

    int pos = ed->GetControl()->GetCurrentPos();
    int lineStartPos = ed->GetControl()->PositionFromLine(ed->GetControl()->GetCurrentLine());
    wxString line = ed->GetControl()->GetLine(ed->GetControl()->GetCurrentLine());
    //Manager::Get()->GetMessageManager()->DebugLog("#include cc for \"%s\"", line.c_str());
    line.Trim();
    if (line.IsEmpty() || !line.StartsWith(_T("#include")))
        return;

    // find opening quote (" or <)
    int idx = pos - lineStartPos;
    int found = -1;
    wxString filename;
    while (idx > 0)
    {
        wxChar c = line.GetChar(idx);
        if (c == _T('>'))
            return; // the quote is closed already...
        else if (c == _T('"') || c == _T('<'))
        {
            if (found != -1)
                return; // the already found quote was the closing one...
            found = idx + 1;
        }
        else if (c != _T(' ') && c != _T('\t') && !found)
            filename << c;
        --idx;
    }

    // take care: found might point at the end of the string (out of bounds)
    // in this case: #include "(<-code-completion at this point)
    //Manager::Get()->GetMessageManager()->DebugLog("#include using \"%s\" (starting at %d)", filename.c_str(), found);
    if (found == -1)
        return;

    // fill a list of matching project files
    wxArrayString files;
    for (int i = 0; i < pPrj->GetFilesCount(); ++i)
    {
        ProjectFile* pf = pPrj->GetFile(i);
        if (pf && FileTypeOf(pf->relativeFilename) == ftHeader)
        {
            wxFileName fname(pf->relativeFilename);
            files.Add(pf->relativeFilename);
            files.Add(fname.GetFullName());
        }
    }

    if (files.GetCount() != 0)
    {
        files.Sort();
        ed->GetControl()->AutoCompSetIgnoreCase(caseSens);
        ed->GetControl()->AutoCompShow(pos - lineStartPos - found, GetStringFromArray(files, _T(" ")));
    }
}

wxArrayString CodeCompletion::GetCallTips()
{
	if (!m_IsAttached)
	{
		wxArrayString items;
		return items;
	}
	return m_NativeParsers.GetCallTips();
}

void CodeCompletion::ShowCallTip()
{
	if (!m_IsAttached)
		return;

	if (!Manager::Get()->GetEditorManager())
		return;

    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
	if (!ed)
		return;

	wxArrayString items = GetCallTips();
	wxString definition;
	for (unsigned int i = 0; i < items.GetCount(); ++i)
	{
		if (!items[i].IsEmpty())
		{
			if (i != 0)
				definition << _T('\n'); // add new-line, except for the first line
			definition << items[i];
		}
	}
	if (!definition.IsEmpty())
		ed->GetControl()->CallTipShow(ed->GetControl()->GetCurrentPos(), definition);
}

int CodeCompletion::DoClassMethodDeclImpl()
{
	if (!m_IsAttached)
		return -1;

	EditorManager* edMan = Manager::Get()->GetEditorManager();
    if (!edMan)
		return -2;
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
    if (!ed)
		return -3;

	FileType ft = FileTypeOf(ed->GetShortName());
	if ( ft != ftHeader && ft != ftSource) // only parse source/header files
		return -4;

	Parser* parser = m_NativeParsers.FindParserFromActiveEditor();
	if (!parser)
	{
		Manager::Get()->GetMessageManager()->DebugLog(_("Active editor has no associated parser ?!?"));
		return -4;
	}

    wxString filename = ed->GetFilename();
    InsertClassMethodDlg dlg(Manager::Get()->GetAppWindow(), parser, filename);
    if (dlg.ShowModal() == wxID_OK)
    {
        int pos = ed->GetControl()->GetCurrentPos();
        int line = ed->GetControl()->LineFromPosition(pos);
        ed->GetControl()->GotoPos(ed->GetControl()->PositionFromLine(line));

        wxArrayString result = dlg.GetCode();
        for (unsigned int i = 0; i < result.GetCount(); ++i)
        {
            pos = ed->GetControl()->GetCurrentPos();
            line = ed->GetControl()->LineFromPosition(pos);
            wxString str = ed->GetLineIndentString(line - 1) + result[i];
            ed->GetControl()->SetTargetStart(pos);
            ed->GetControl()->SetTargetEnd(pos);
            ed->GetControl()->ReplaceTarget(str);
            ed->GetControl()->GotoPos(pos + str.Length());// - 3);
        }
        return 0;
    }

	return -5;
}

void CodeCompletion::DoCodeComplete()
{
	EditorManager* edMan = Manager::Get()->GetEditorManager();
    if (!edMan)
    	return;
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
    if (!ed)
    	return;

    int style = ed->GetControl()->GetStyleAt(ed->GetControl()->GetCurrentPos());
//	Manager::Get()->GetMessageManager()->DebugLog(_("Style at %d is %d"), ed->GetControl()->GetCurrentPos(), style);
//	Manager::Get()->GetMessageManager()->DebugLog(_("wxSCI_C_PREPROCESSOR is %d"), wxSCI_C_PREPROCESSOR);
    if (style == wxSCI_C_PREPROCESSOR)
    {
        CodeCompleteIncludes();
        return;
    }

    if (style != wxSCI_C_DEFAULT && style != wxSCI_C_OPERATOR && style != wxSCI_C_IDENTIFIER)
        return;

	CodeComplete();
}

void CodeCompletion::DoInsertCodeCompleteToken(wxString tokName)
{
	// remove arguments
	int pos = tokName.Find(_T("("));
	if (pos != wxNOT_FOUND)
		tokName.Remove(pos);

	EditorManager* edMan = Manager::Get()->GetEditorManager();
    if (!edMan)
    	return;
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
    if (!ed)
    	return;

	int end = ed->GetControl()->GetCurrentPos() > m_NativeParsers.GetEditorEndWord() ? ed->GetControl()->GetCurrentPos() : m_NativeParsers.GetEditorEndWord();
	ed->GetControl()->SetSelection(m_NativeParsers.GetEditorStartWord(), end);
	ed->GetControl()->ReplaceSelection(_T(""));
	ed->GetControl()->InsertText(m_NativeParsers.GetEditorStartWord(), tokName);
	ed->GetControl()->GotoPos(m_NativeParsers.GetEditorStartWord() + tokName.Length());
}

// events

void CodeCompletion::OnProjectOpened(CodeBlocksEvent& event)
{
    if (m_IsAttached)
		m_NativeParsers.AddParser(event.GetProject());
    event.Skip();
}

void CodeCompletion::OnProjectActivated(CodeBlocksEvent& event)
{
    if (m_IsAttached)
		m_NativeParsers.SetClassBrowserProject(event.GetProject());
    event.Skip();
}

void CodeCompletion::OnProjectClosed(CodeBlocksEvent& event)
{
    if (m_IsAttached)
		m_NativeParsers.RemoveParser(event.GetProject());
    event.Skip();
}

void CodeCompletion::OnProjectFileAdded(CodeBlocksEvent& event)
{
    if (m_IsAttached)
		m_NativeParsers.AddFileToParser(event.GetProject(), event.GetString());
	event.Skip();
}

void CodeCompletion::OnProjectFileRemoved(CodeBlocksEvent& event)
{
    if (m_IsAttached)
		m_NativeParsers.RemoveFileFromParser(event.GetProject(), event.GetString());
	event.Skip();
}

void CodeCompletion::OnUserListSelection(CodeBlocksEvent& event)
{
    if (m_IsAttached)
    {
		wxString tokName = event.GetString();
		DoInsertCodeCompleteToken(event.GetString());
    }

    event.Skip();
}

void CodeCompletion::OnReparseActiveEditor(CodeBlocksEvent& event)
{
    if (m_IsAttached)
    {
    	EditorBase* ed = event.GetEditor();
    	if (!ed)
    		return;
		Parser* parser = m_NativeParsers.FindParserFromActiveEditor();
		if (!parser || !parser->Done())
			return;

		parser->StartTimer();
		parser->Reparse(ed->GetFilename());
    }

    event.Skip();
}

void CodeCompletion::OnEditorActivated(CodeBlocksEvent& event)
{
    static EditorBase* m_LastActiveEditor = event.GetEditor();
    if (m_LastActiveEditor != event.GetEditor())
    {
        m_LastActiveEditor = event.GetEditor();
        m_NativeParsers.OnEditorActivated(event.GetEditor());
    }
    event.Skip();
}

void CodeCompletion::OnUpdateUI(wxUpdateUIEvent& event)
{
    bool hasEd = Manager::Get()->GetEditorManager()->GetActiveEditor() != 0;
	if (m_EditMenu)
	{
	    m_EditMenu->Enable(idMenuCodeComplete, hasEd);
	    m_EditMenu->Enable(idMenuShowCallTip, hasEd);
	}

	if (m_SearchMenu)
	{
	    m_SearchMenu->Enable(idMenuGotoFunction, hasEd);
    }

    // must do...
    event.Skip();
}

void CodeCompletion::OnCodeComplete(wxCommandEvent& event)
{
    if (!Manager::Get()->GetConfigManager(_T("code_completion"))->ReadBool(_T("/use_code_completion"), true))
        return;
    if (m_IsAttached)
		DoCodeComplete();
    event.Skip();
}

void CodeCompletion::OnShowCallTip(wxCommandEvent& event)
{
    if (m_IsAttached)
		ShowCallTip();
    event.Skip();
}

void CodeCompletion::OnGotoFunction(wxCommandEvent& event)
{
	EditorManager* edMan = Manager::Get()->GetEditorManager();
   	if (!edMan)
   		return;
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
   	if (!ed)
		return;

	Parser parser(this);
	parser.ParseBufferForFunctions(ed->GetControl()->GetText());

	wxArrayString funcs;
	const TokensArray& tokens = parser.GetTokens();
	for (unsigned int i = 0; i < tokens.GetCount(); ++i)
	{
		funcs.Add(tokens[i]->m_DisplayName);// token->m_Name);
	}
	if (!funcs.GetCount())
	{
		wxMessageBox(_("No functions parsed in this file..."));
		return;
	}
	IncrementalSelectListDlg dlg(Manager::Get()->GetAppWindow(), funcs, _("Select function..."), _("Please select function to go to:"));
	if (dlg.ShowModal() == wxID_OK)
	{
        wxString sel = dlg.GetStringSelection();
        for (unsigned int i = 0; i < tokens.GetCount(); ++i)
        {
            Token* token = tokens[i];
            if (token && token->m_DisplayName.Matches(sel))
            {
                Manager::Get()->GetMessageManager()->DebugLog(_("Token found at line %d"), token->m_Line);
                ed->GotoLine(token->m_Line - 1);
            }
        }
	}
}

void CodeCompletion::OnClassMethod(wxCommandEvent& event)
{
    DoClassMethodDeclImpl();
}

void CodeCompletion::OnGotoDeclaration(wxCommandEvent& event)
{
	EditorManager* edMan = Manager::Get()->GetEditorManager();
   	if (!edMan)
   		return;

    wxString txt = m_LastKeyword;
//    Manager::Get()->GetMessageManager()->DebugLog(_("Go to decl for '%s'"), txt.c_str());

    Parser* parser = m_NativeParsers.FindParserFromActiveEditor();
	if (!parser)
		parser = m_NativeParsers.FindParserFromActiveProject(); // get parser of active project, then
    if (!parser)
        return;

    Token* token = parser->FindTokenByName(txt, false);
    if (token)
    {
        cbEditor* ed = edMan->Open(token->m_Filename);
        if (ed)
        {
            ed->GotoLine(token->m_Line - 1);
            return;
        }
    }
    wxMessageBox(wxString::Format(_("Not found: %s"), txt.c_str()), _("Warning"), wxICON_WARNING);
}

void CodeCompletion::OnOpenIncludeFile(wxCommandEvent& event)
{
    Parser* parser = m_NativeParsers.FindParserFromActiveEditor();
	if (!parser)
		parser = m_NativeParsers.FindParserFromActiveProject(); // get parser of active project, then
    if (!parser)
        return;

    // search in all parser's include dirs
    wxString tmp = parser->FindFileInIncludeDirs(m_LastIncludeFile);
    if (!tmp.IsEmpty())
    {
        EditorManager* edMan = Manager::Get()->GetEditorManager();
        edMan->Open(tmp);
        return;
    }

    // look in the same dir as the source file
    wxFileName fname = m_LastIncludeFile;
    fname.Assign(wxFileName(m_LastIncludeFileFrom).GetPath(wxPATH_GET_SEPARATOR) + m_LastIncludeFile);
    if (wxFileExists(fname.GetFullPath()))
    {
        EditorManager* edMan = Manager::Get()->GetEditorManager();
        edMan->Open(fname.GetFullPath());
        return;
    }

    wxMessageBox(wxString::Format(_("Not found: %s"), m_LastIncludeFile.c_str()), _("Warning"), wxICON_WARNING);
}
