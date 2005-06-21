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

#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/fontutil.h>
#include <wx/notebook.h>

#include "cbeditor.h" // class's header file
#include "globals.h"
#include "sdk_events.h"
#include "projectbuildtarget.h" // for ProjectFile*
#include "editorcolorset.h"
#include "manager.h"
#include "configmanager.h"
#include "pluginmanager.h"
#include "editormanager.h"
#include "messagemanager.h"
#include "cbplugin.h"
#include "cbeditorprintout.h"

/* This struct holds private data for the cbEditor class.
 * It's a paradigm to avoid rebuilding the entire project (as cbEditor is a basic dependency)
 * for just adding a private var or method.
 * What happens is that we 've declared a cbEditorInternalData* private member in cbEditor
 * and define it in the .cpp file (here). Effectively, this means that we can now add/remove
 * elements from cbEditorInternalData without needing to rebuild the project :)
 * The cbEditor::m_pData is the variable to use in code. It's the very first thing
 * constructed and the very last destructed.
 *
 * So, if we want to add a new private member in cbEditor, we add it here instead
 * and access it with m_pData->
 * e.g. m_pData->lastPosForCodeCompletion
 * and of course you can add member functions here ;)
 *
 * cbEditorInternalData also contains a pointer to the owner cbEditor (named m_pOwner).
 * Using m_pOwner the struct's member functions can access cbEditor functions
 * (even private ones - it's a friend).
 *
 * The same logic should be used all around the project's classes, gradually.
 */
struct cbEditorInternalData
{
    cbEditorInternalData(cbEditor* owner)
        : m_pOwner(owner),
        lastPosForCodeCompletion(0)
    {}
    cbEditor* m_pOwner;

    // add member vars/funcs below
    
    // funcs
    /** Get the last non-whitespace character before position */
    wxChar GetLastNonWhitespaceChar(int position = -1)
    {
        wxStyledTextCtrl* control = m_pOwner->GetControl();
        if (position == -1)
            position = control->GetCurrentPos();
        while (position)
        {
            wxChar c = control->GetCharAt(position--);
            if (c != ' ' && c != '\t' && c != '\n' && c != '\r')
                return c;
        }
        return 0;
    }

    int FindBlockStart(int position, wxChar blockStart, wxChar blockEnd, bool skipNested = true)
    {
        wxStyledTextCtrl* control = m_pOwner->GetControl();
        int lvl = 0;
        wxChar b = control->GetCharAt(position);
        while (b)
        {
            if (b == blockEnd)
                ++lvl;
            else if (b == blockStart)
            {
                if (lvl == 0)
                    return position;
                --lvl;
            }
            --position;
            b = control->GetCharAt(position);
        }
        return -1;
    }

    //vars
    int lastPosForCodeCompletion;
};
////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(cbStyledTextCtrl, wxStyledTextCtrl)
	EVT_CONTEXT_MENU(cbStyledTextCtrl::OnContextMenu)
END_EVENT_TABLE()

cbStyledTextCtrl::cbStyledTextCtrl(cbEditor* pParent, int id, const wxPoint& pos, const wxSize& size, long style)
	: wxStyledTextCtrl(pParent, id, pos, size, style),
	m_pParent(pParent)
{
	//ctor
}

cbStyledTextCtrl::~cbStyledTextCtrl()
{
	//dtor
}

// events

void cbStyledTextCtrl::OnContextMenu(wxContextMenuEvent& event)
{
	if (m_pParent)
		m_pParent->DisplayContextMenu(event.GetPosition());
}

const int idEmptyMenu = wxNewId();
const int idEdit = wxNewId();
const int idUndo = wxNewId();
const int idRedo = wxNewId();
const int idCut = wxNewId();
const int idCopy = wxNewId();
const int idPaste = wxNewId();
const int idDelete = wxNewId();
const int idSelectAll = wxNewId();
const int idSwapHeaderSource = wxNewId();
const int idBookmarks = wxNewId();
const int idBookmarksToggle = wxNewId();
const int idBookmarksPrevious = wxNewId();
const int idBookmarksNext = wxNewId();
const int idFolding = wxNewId();
const int idFoldingFoldAll = wxNewId();
const int idFoldingUnfoldAll = wxNewId();
const int idFoldingToggleAll = wxNewId();
const int idFoldingFoldCurrent = wxNewId();
const int idFoldingUnfoldCurrent = wxNewId();
const int idFoldingToggleCurrent = wxNewId();
const int idInsert = wxNewId();
const int idConfigureEditor = wxNewId();
const int idProperties = wxNewId();

BEGIN_EVENT_TABLE(cbEditor, EditorBase)
    EVT_CLOSE(cbEditor::OnClose)
    EVT_TIMER(-1, cbEditor::OnTimer)
    // we got dynamic events; look in CreateEditor()
	
	EVT_MENU(idUndo, cbEditor::OnContextMenuEntry)
	EVT_MENU(idRedo, cbEditor::OnContextMenuEntry)
	EVT_MENU(idCut, cbEditor::OnContextMenuEntry)
	EVT_MENU(idCopy, cbEditor::OnContextMenuEntry)
	EVT_MENU(idPaste, cbEditor::OnContextMenuEntry)
	EVT_MENU(idDelete, cbEditor::OnContextMenuEntry)
	EVT_MENU(idSelectAll, cbEditor::OnContextMenuEntry)
	EVT_MENU(idSwapHeaderSource, cbEditor::OnContextMenuEntry)
	EVT_MENU(idBookmarksToggle, cbEditor::OnContextMenuEntry)
	EVT_MENU(idBookmarksPrevious, cbEditor::OnContextMenuEntry)
	EVT_MENU(idBookmarksNext, cbEditor::OnContextMenuEntry)
	EVT_MENU(idFoldingFoldAll, cbEditor::OnContextMenuEntry)
	EVT_MENU(idFoldingUnfoldAll, cbEditor::OnContextMenuEntry)
	EVT_MENU(idFoldingToggleAll, cbEditor::OnContextMenuEntry)
	EVT_MENU(idFoldingFoldCurrent, cbEditor::OnContextMenuEntry)
	EVT_MENU(idFoldingUnfoldCurrent, cbEditor::OnContextMenuEntry)
	EVT_MENU(idFoldingToggleCurrent, cbEditor::OnContextMenuEntry)
	EVT_MENU(idConfigureEditor, cbEditor::OnContextMenuEntry)
	EVT_MENU(idProperties, cbEditor::OnContextMenuEntry)
END_EVENT_TABLE()

// class constructor
cbEditor::cbEditor(wxWindow* parent, const wxString& filename, EditorColorSet* theme)
    : EditorBase(parent, filename),
	m_pControl(0L),
	m_Modified(false),
	m_Index(-1),
	m_pProjectFile(0L),
	m_pTheme(theme),
	m_ActiveCalltipsNest(0)
{
    // first thing to do!
    // if we add more constructors in the future, don't forget to set this!
    m_pData = new cbEditorInternalData(this);
    m_IsBuiltinEditor = true;

    m_timerWait.SetOwner(this);
    
    InitFilename(filename);
//    Manager::Get()->GetMessageManager()->DebugLog("ctor: Filename=%s\nShort=%s", m_Filename.c_str(), m_Shortname.c_str());

	CreateEditor();
	SetEditorStyle();
	m_IsOK = Open();

    // if !m_IsOK then it's a new file, so set the modified flag ON
    if (!m_IsOK && filename.IsEmpty())
    {
        SetModified(true);
    }
}

// class destructor
cbEditor::~cbEditor()
{
//    if (!Manager::Get())
//        return;
//    Manager::Get()->GetMessageManager()->DebugLog("~cbEditor(): " + m_Filename);
	NotifyPlugins(cbEVT_EDITOR_CLOSE, 0, m_Filename);
	UpdateProjectFile();
	if (m_pControl)
	{
		if (m_pProjectFile)
			m_pProjectFile->editorOpen = false;
        m_pControl->Destroy();
        m_pControl = 0;
	}
	delete m_pData;
}

void cbEditor::NotifyPlugins(wxEventType type, int intArg, const wxString& strArg, int xArg, int yArg)
{
    if (!Manager::Get()->GetPluginManager())
        return; // no plugin manager! app shuting down?
	CodeBlocksEvent event(type);
	event.SetEditor(this);
	event.SetInt(intArg);
	event.SetString(strArg);
	event.SetX(xArg);
	event.SetY(yArg);
	//wxPostEvent(Manager::Get()->GetAppWindow(), event);
	Manager::Get()->GetPluginManager()->NotifyPlugins(event);
}

bool cbEditor::GetModified()
{ 
    return m_Modified || m_pControl->GetModify(); 
}

void cbEditor::SetModified(bool modified)
{
    if (modified != m_Modified)
    {
        m_Modified = modified;
        if (m_Modified)
            SetEditorTitle(EDITOR_MODIFIED + m_Shortname);
        else
            SetEditorTitle(m_Shortname);
    }
}

void cbEditor::SetEditorTitle(const wxString& title)
{
    SetTitle(title);
}

void cbEditor::SetProjectFile(ProjectFile* project_file,bool preserve_modified)
{
	if (m_pProjectFile == project_file)
		return; // we 've been here before ;)

	bool wasmodified = false;
	if(preserve_modified)
        wasmodified = GetModified();
		
	m_pProjectFile = project_file;
	if (m_pProjectFile)
	{
        // update our filename
        m_Filename = UnixFilename(project_file->file.GetFullPath());

		m_pControl->GotoPos(m_pProjectFile->editorPos);
		m_pControl->ScrollToLine(m_pProjectFile->editorTopLine);
		m_pControl->ScrollToColumn(0);

		m_pProjectFile->editorOpen = true;
		SetBreakpoints();
		
		if (ConfigManager::Get()->Read("/editor/tab_text_relative", 1) == 1)
            m_Shortname = m_pProjectFile->relativeToCommonTopLevelPath;
        else
            m_Shortname = m_pProjectFile->file.GetFullName();
		SetEditorTitle(m_Shortname);
	}
#if 0
	wxString dbg;
	dbg << "[ed] Filename: " << GetFilename() << '\n';
	dbg << "[ed] Short name: " << GetShortName() << '\n';
	dbg << "[ed] Modified: " << GetModified() << '\n';
	dbg << "[ed] Project: " << ((m_pProjectFile && m_pProjectFile->project) ? m_pProjectFile->project->GetTitle() : "unknown") << '\n';
	dbg << "[ed] Project file: " << (m_pProjectFile ? m_pProjectFile->relativeFilename : "unknown") << '\n';
	Manager::Get()->GetMessageManager()->DebugLog(dbg);
#endif
    if(preserve_modified)
        SetModified(wasmodified);
}

void cbEditor::UpdateProjectFile()
{
	if (m_pControl && m_pProjectFile)
	{
		m_pProjectFile->editorPos = m_pControl->GetCurrentPos();
		m_pProjectFile->editorTopLine = m_pControl->GetFirstVisibleLine();
		m_pProjectFile->editorOpen = true;
	}
}

void cbEditor::CreateEditor()
{
	m_ID = wxNewId();

    wxBoxSizer* bs = new wxBoxSizer(wxVERTICAL);
    m_pControl = new cbStyledTextCtrl(this, m_ID, wxPoint(0, 0), GetSize());
	m_pControl->UsePopUp(false);
    bs->Add(m_pControl, 1, wxEXPAND);
    SetSizer(bs);
    SetAutoLayout(true);

    // dynamic events
    Connect( m_ID,  -1, wxEVT_STC_MARGINCLICK,
                  (wxObjectEventFunction) (wxEventFunction) (wxStyledTextEventFunction)
                  &cbEditor::OnMarginClick );
    Connect( m_ID,  -1, wxEVT_STC_UPDATEUI,
                  (wxObjectEventFunction) (wxEventFunction) (wxStyledTextEventFunction)
                  &cbEditor::OnEditorUpdateUI );
    Connect( m_ID,  -1, wxEVT_STC_CHANGE,
                  (wxObjectEventFunction) (wxEventFunction) (wxStyledTextEventFunction)
                  &cbEditor::OnEditorChange );
    Connect( m_ID,  -1, wxEVT_STC_CHARADDED,
                  (wxObjectEventFunction) (wxEventFunction) (wxStyledTextEventFunction)
                  &cbEditor::OnEditorCharAdded );
    Connect( m_ID,  -1, wxEVT_STC_DWELLSTART,
                  (wxObjectEventFunction) (wxEventFunction) (wxStyledTextEventFunction)
                  &cbEditor::OnEditorDwellStart );
    Connect( m_ID,  -1, wxEVT_STC_DWELLEND,
                  (wxObjectEventFunction) (wxEventFunction) (wxStyledTextEventFunction)
                  &cbEditor::OnEditorDwellEnd );
    Connect( m_ID,  -1, wxEVT_STC_USERLISTSELECTION,
                  (wxObjectEventFunction) (wxEventFunction) (wxStyledTextEventFunction)
                  &cbEditor::OnUserListSelection );
}

wxColour cbEditor::GetOptionColour(const wxString& option, const wxColour _default)
{
    return wxColour (ConfigManager::Get()->Read(option + "/red", _default.Red()),
    				ConfigManager::Get()->Read(option + "/green", _default.Green()),
    				ConfigManager::Get()->Read(option + "/blue", _default.Blue())
	);
}

void cbEditor::SetEditorStyle()
{
    wxFont font(8, wxMODERN, wxNORMAL, wxNORMAL);
    wxString fontstring = ConfigManager::Get()->Read("/editor/font", wxEmptyString);

    if (!fontstring.IsEmpty())
    {
        wxNativeFontInfo nfi;
        nfi.FromString(fontstring);
        font.SetNativeFontInfo(nfi);
    }

    // update the tab text based on preferences
    if (m_pProjectFile)
    {
		if (ConfigManager::Get()->Read("/editor/tab_text_relative", 1) == 1)
            m_Shortname = m_pProjectFile->relativeToCommonTopLevelPath;
        else
            m_Shortname = m_pProjectFile->file.GetFullName();
		SetEditorTitle(m_Shortname);
    }

	m_pControl->SetMouseDwellTime(1000);

	m_pControl->SetCaretLineVisible(ConfigManager::Get()->Read("/editor/highlight_caret_line", 1));
	m_pControl->SetCaretLineBack(GetOptionColour("/editor/highlight_caret_line_color", wxColour(0xFF, 0xFF, 0x00)));

    m_pControl->SetUseTabs(ConfigManager::Get()->Read("/editor/use_tab", 0L));
	m_pControl->SetIndentationGuides(ConfigManager::Get()->Read("/editor/show_indent_guides", 0L));
    m_pControl->SetTabIndents(ConfigManager::Get()->Read("/editor/tab_indents", 1));
    m_pControl->SetBackSpaceUnIndents(ConfigManager::Get()->Read("/editor/backspace_unindents", 1));
    m_pControl->SetWrapMode(ConfigManager::Get()->Read("/editor/word_wrap", 0L));
    m_pControl->SetViewEOL(ConfigManager::Get()->Read("/editor/show_eol", 0L));
    m_pControl->SetViewWhiteSpace(ConfigManager::Get()->Read("/editor/view_whitespace", 0L));
	//gutter
    m_pControl->SetEdgeMode(ConfigManager::Get()->Read("/editor/gutter/mode", 1));
    m_pControl->SetEdgeColour(GetOptionColour("/editor/gutter/color", wxColour(0xC0, 0xC0, 0xC0)));
    m_pControl->SetEdgeColumn(ConfigManager::Get()->Read("/editor/gutter/column", 80));

    m_pControl->StyleSetFont(wxSTC_STYLE_DEFAULT, font);
    m_pControl->StyleClearAll();

    m_pControl->SetTabWidth(ConfigManager::Get()->Read("/editor/tab_size", 4));

    // line numbering
   	if (ConfigManager::Get()->Read("/editor/show_line_numbers", 0L))
    {
	    m_pControl->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    	m_pControl->SetMarginWidth(0, 48);
    }
	else
		m_pControl->SetMarginWidth(0, 0);

    // margin for bookmarks, breakpoints etc.
	// FIXME: how to display a mark with an offset???
	m_pControl->SetMarginWidth(1, 16);
    m_pControl->SetMarginType(1, wxSTC_MARGIN_SYMBOL);
    m_pControl->SetMarginSensitive(1, 1);
    m_pControl->SetMarginMask(1, (1 << BOOKMARK_MARKER) | (1 << BREAKPOINT_MARKER));
	m_pControl->MarkerDefine(BOOKMARK_MARKER, BOOKMARK_STYLE, wxNullColour, wxColour(0xA0, 0xA0, 0xFF));
	m_pControl->MarkerDefine(BREAKPOINT_MARKER, BREAKPOINT_STYLE, wxNullColour, wxColour(0xFF, 0x00, 0x00));

    // folding margin
    m_pControl->SetProperty("fold", ConfigManager::Get()->Read("/editor/folding/show_folds", 1) ? "1" : "0");
    m_pControl->SetProperty("fold.comment", ConfigManager::Get()->Read("/editor/folding/fold_comments", 0L) ? "1" : "0");
    m_pControl->SetProperty("fold.compact", "0");
    m_pControl->SetProperty("fold.preprocessor", ConfigManager::Get()->Read("/editor/folding/fold_preprocessor", 0L) ? "1" : "0");
   	if (ConfigManager::Get()->Read("/editor/folding/show_folds", 1))
    {
        m_pControl->SetFoldFlags(16);
        m_pControl->SetMarginType(2, wxSTC_MARGIN_SYMBOL);
        m_pControl->SetMarginWidth(2, 16);
        m_pControl->SetMarginMask(2, wxSTC_MASK_FOLDERS);
        m_pControl->SetMarginSensitive(2, 1);

        m_pControl->MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS, wxColour(0xff, 0xff, 0xff), wxColour(0x80, 0x80, 0x80));
        m_pControl->MarkerDefine(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS, wxColour(0xff, 0xff, 0xff), wxColour(0x80, 0x80, 0x80));
        m_pControl->MarkerDefine(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_VLINE, wxColour(0xff, 0xff, 0xff), wxColour(0x80, 0x80, 0x80));
        m_pControl->MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_LCORNER, wxColour(0xff, 0xff, 0xff), wxColour(0x80, 0x80, 0x80));
        m_pControl->MarkerDefine(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUSCONNECTED, wxColour(0xff, 0xff, 0xff), wxColour(0x80, 0x80, 0x80));
        m_pControl->MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUSCONNECTED, wxColour(0xff, 0xff, 0xff), wxColour(0x80, 0x80, 0x80));
        m_pControl->MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_TCORNER, wxColour(0xff, 0xff, 0xff), wxColour(0x80, 0x80, 0x80));
	}
	else
		m_pControl->SetMarginWidth(2, 0);

	if (m_pTheme)
		m_pTheme->Apply(this);
}

void cbEditor::SetColorSet(EditorColorSet* theme)
{
	m_pTheme = theme;
	m_pTheme->Apply(this);
}

bool cbEditor::Reload()
{
    // keep current pos
    int pos = m_pControl ? m_pControl->GetCurrentPos() : 0;

    // call open
    if (!Open())
        return false;
    
    // return (if possible) to old pos
    if (m_pControl)
        m_pControl->GotoPos(pos);

    return true;
}

bool cbEditor::Open()
{
    if (!wxFileExists(m_Filename))
        return false;

    // open file
    m_pControl->SetReadOnly(false);
    wxString st;

    m_pControl->ClearAll();
    wxFile file(m_Filename);

    if (!file.IsOpened())
        return false;

    char* buff = st.GetWriteBuf(file.Length());
    file.Read(buff, file.Length());
    file.Close();
    st.UngetWriteBuf();

    m_pControl->InsertText(0, st);
    m_pControl->EmptyUndoBuffer();

    // mark the file read-only, if applicable
    bool read_only = !wxFile::Access(m_Filename.c_str(), wxFile::write);
    m_pControl->SetReadOnly(read_only);
    // if editor is read-only, override bg color for *all* styles...
    if (read_only)
    {
        for (int i = 0; i < wxSTC_STYLE_MAX; ++i)
            m_pControl->StyleSetBackground(i, wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    }

	if (ConfigManager::Get()->Read("/editor/fold_all_on_open", 0L))
		FoldAll();
    
    wxFileName fname(m_Filename);
    m_LastModified = fname.GetModificationTime();

    SetModified(false);
	NotifyPlugins(cbEVT_EDITOR_OPEN);
    return true;
}

bool cbEditor::Save()
{
	if (!GetModified())
		return true;
		
    if (!m_IsOK)
    {
        return SaveAs();
    }
    
    wxFile file(m_Filename, wxFile::write);
    if (file.Write(m_pControl->GetText().c_str(), m_pControl->GetTextLength()) == 0 &&
        m_pControl->GetTextLength() != 0)
    {
        return false; // failed; file is read-only?
    }
    file.Flush();
    file.Close();
    
    wxFileName fname(m_Filename);
    m_LastModified = fname.GetModificationTime();

    m_IsOK = true;

    m_pControl->SetSavePoint();
    SetModified(false);
    
	NotifyPlugins(cbEVT_EDITOR_SAVE);
    return true;
}

bool cbEditor::SaveAs()
{
    wxFileName fname;
    fname.Assign(m_Filename);
    wxFileDialog* dlg = new wxFileDialog(this,
                            _("Save file"),
                            fname.GetPath(),
                            fname.GetFullName(),
                            SOURCE_FILES_DIALOG_FILTER,
                            wxSAVE | wxOVERWRITE_PROMPT);
    dlg->SetFilterIndex(SOURCE_FILES_FILTER_INDEX);

    if (dlg->ShowModal() != wxID_OK)
        return false;
    m_Filename = dlg->GetPath();
    fname.Assign(m_Filename);
    m_Shortname = fname.GetFullName();
	SetEditorTitle(m_Shortname);
    //Manager::Get()->GetMessageManager()->Log(mltDevDebug, "Filename=%s\nShort=%s", m_Filename.c_str(), m_Shortname.c_str());
    m_IsOK = true;
    SetModified(true);
    return Save();
}

bool cbEditor::RenameTo(const wxString& filename, bool deleteOldFromDisk)
{
    wxLogWarning(_("Not implemented..."));
	//NotifyPlugins(cbEVT_EDITOR_RENAME);
	return false;
}

void cbEditor::DoAskForCodeCompletion()
{
    m_timerWait.Stop();
	NotifyPlugins(cbEVT_EDITOR_AUTOCOMPLETE);
}

void cbEditor::AutoComplete()
{
    MessageManager* msgMan = Manager::Get()->GetMessageManager();
    AutoCompleteMap& map = Manager::Get()->GetEditorManager()->GetAutoCompleteMap();
    int curPos = m_pControl->GetCurrentPos();
    int wordStartPos = m_pControl->WordStartPosition(curPos, true);
    wxString keyword = m_pControl->GetTextRange(wordStartPos, curPos);
    wxString lineIndent = GetLineIndentString(m_pControl->GetCurrentLine());
    msgMan->DebugLog("Auto-complete keyword: %s", keyword.c_str());
    
    AutoCompleteMap::iterator it;
    for (it = map.begin(); it != map.end(); ++it)
    {
        if (keyword == it->first)
        {
            // found; auto-complete it
            msgMan->DebugLog("Match found");
            m_pControl->BeginUndoAction();
            
            // indent code accordingly
            wxString code = it->second;
            code.Replace("\n", '\n' + lineIndent);

            // look for and replace macros
            int macroPos = code.Find("$(");
            while (macroPos != -1)
            {
                // locate ending parenthesis
                int macroPosEnd = macroPos + 2;
                int len = (int)code.Length();
                while (macroPosEnd < len && code.GetChar(macroPosEnd) != ')')
                    ++macroPosEnd;
                if (macroPosEnd == len)
                    break; // no ending parenthesis
                
                wxString macroName = code.SubString(macroPos + 2, macroPosEnd - 1);
                msgMan->DebugLog("Found macro: %s", macroName.c_str());
                wxString macro = wxGetTextFromUser(_("Please enter the text for \"" + macroName + "\":"), _("Macro substitution"));
                code.Replace(_("$(") + macroName + _(")"), macro);
                macroPos = code.Find("$(");
            }

            // delete keyword
            m_pControl->SetSelection(wordStartPos, curPos);
            m_pControl->ReplaceSelection("");
            curPos = wordStartPos;

            // add the text
            m_pControl->InsertText(curPos, code);

            // put cursor where "|" appears in code (if it appears)
            int caretPos = code.Find('|');
            if (caretPos != -1)
            {
                m_pControl->SetCurrentPos(curPos + caretPos);
                m_pControl->SetSelection(curPos + caretPos, curPos + caretPos + 1);
                m_pControl->ReplaceSelection("");
            }

            m_pControl->EndUndoAction();
            break;
        }
    }
}

void cbEditor::DoFoldAll(int fold)
{
    m_pControl->Colourise(0, -1); // the *most* important part!
    int count = m_pControl->GetLineCount();
    for (int i = 0; i < count; ++i)
		DoFoldLine(i, fold);
}

void cbEditor::DoFoldBlockFromLine(int line, int fold)
{
    m_pControl->Colourise(0, -1); // the *most* important part!
	int i = line;
    while (i != 0)
    {
		if (DoFoldLine(i, fold))
			return;
		--i;
    }
}

bool cbEditor::DoFoldLine(int line, int fold)
{
	int level = m_pControl->GetFoldLevel(line);
	if ((level & wxSTC_FOLDLEVELHEADERFLAG) &&
		(wxSTC_FOLDLEVELBASE == (level & wxSTC_FOLDLEVELNUMBERMASK)))
	{
		bool expand = false;
		int maxLine = m_pControl->GetLastChild(line, -1);
		if (fold == 2) // toggle
			expand = !m_pControl->GetFoldExpanded(line);
		else
			expand = fold == 0;
		m_pControl->SetFoldExpanded(line, expand);
		if (maxLine > line)
		{
			if (expand)
				m_pControl->ShowLines(line + 1, maxLine);
			else
				m_pControl->HideLines(line + 1, maxLine);
		}
		return true;
	}
	return false;
}

void cbEditor::FoldAll()
{
	DoFoldAll(1);
}

void cbEditor::UnfoldAll()
{
	DoFoldAll(0);
}

void cbEditor::ToggleAllFolds()
{
	DoFoldAll(2);
}

void cbEditor::FoldBlockFromLine(int line)
{
	if (line == -1)
		line = m_pControl->GetCurrentLine();
	DoFoldBlockFromLine(line, 1);
}

void cbEditor::UnfoldBlockFromLine(int line)
{
	if (line == -1)
		line = m_pControl->GetCurrentLine();
	DoFoldBlockFromLine(line, 0);
}

void cbEditor::ToggleFoldBlockFromLine(int line)
{
	if (line == -1)
		line = m_pControl->GetCurrentLine();
	DoFoldBlockFromLine(line, 2);
}

bool cbEditor::LineHasMarker(int marker, int line)
{
	if (line == -1)
		line = m_pControl->GetCurrentLine();
	return m_pControl->MarkerGet(line) & (1 << marker);
}

void cbEditor::MarkerToggle(int marker, int line)
{
	if (line == -1)
		line = m_pControl->GetCurrentLine();
	if (LineHasMarker(marker, line))
		m_pControl->MarkerDelete(line, marker);
	else
		m_pControl->MarkerAdd(line, marker);
		
	if (marker == BREAKPOINT_MARKER) // more to do with breakpoints
	{
		ProjectFile* pf = GetProjectFile();
		if (!pf)
			return;
		pf->ToggleBreakpoint(line);
		DebuggerBreakpoint* bp = pf->HasBreakpoint(line);
		if (bp)
		{
			//Manager::Get()->GetMessageManager()->Log(mltDevDebug, "ToggleBreakpoint add");
			m_pControl->MarkerAdd(line, BREAKPOINT_MARKER);
			m_pControl->MarkerAdd(line, BREAKPOINT_LINE);
			NotifyPlugins(cbEVT_EDITOR_BREAKPOINT_ADDED, line);
		}
		else
		{
			//Manager::Get()->GetMessageManager()->Log(mltDevDebug, "ToggleBreakpoint delete");
			m_pControl->MarkerDelete(line, BREAKPOINT_MARKER);
			m_pControl->MarkerDelete(line, BREAKPOINT_LINE);
			NotifyPlugins(cbEVT_EDITOR_BREAKPOINT_DELETED, line);
		}
	}
}

void cbEditor::MarkerNext(int marker)
{
	int line = m_pControl->GetCurrentLine() + 1;
	int newLine = m_pControl->MarkerNext(line, 1 << marker);
	if (newLine != -1)
		m_pControl->GotoLine(newLine);
}

void cbEditor::MarkerPrevious(int marker)
{
	int line = m_pControl->GetCurrentLine() - 1;
	int newLine = m_pControl->MarkerPrevious(line, 1 << marker);
	if (newLine != -1)
		m_pControl->GotoLine(newLine);
}

void cbEditor::SetBreakpoints()
{
	ProjectFile* pf = GetProjectFile();
	if (!pf)
		return;
		
	m_pControl->MarkerDeleteAll(BREAKPOINT_MARKER);
	m_pControl->MarkerDeleteAll(BREAKPOINT_LINE);
	for (unsigned int i = 0; i < pf->breakpoints.GetCount(); ++i)
	{
		DebuggerBreakpoint* bp = pf->breakpoints[i];
		pf->SetBreakpoint(bp->line);
		m_pControl->MarkerAdd(bp->line, BREAKPOINT_MARKER);
		m_pControl->MarkerAdd(bp->line, BREAKPOINT_LINE);
		//Manager::Get()->GetMessageManager()->Log(mltDevDebug, "SetBreakpoint line %d", bp->line);
	}
}

void cbEditor::MarkLine(int marker, int line)
{
	m_pControl->MarkerDeleteAll(marker);
	if (line != -1)
	{
		m_pControl->MarkerAdd(line, marker);
		m_pControl->SetCaretLineVisible(false);
	}
	else
		m_pControl->SetCaretLineVisible(ConfigManager::Get()->Read("/editor/highlight_caret_line", 1));
}

void cbEditor::HighlightBraces()
{
    ////// BRACES HIGHLIGHTING ///////
    int currPos = m_pControl->GetCurrentPos();
    int newPos = m_pControl->BraceMatch(currPos);
    if (newPos == wxSTC_INVALID_POSITION)
    {
        if(currPos > 0) currPos--;
        newPos = m_pControl->BraceMatch(currPos);
    }
    if (newPos != wxSTC_INVALID_POSITION)
        m_pControl->BraceHighlight(currPos, newPos);
    else
    {
        char ch = m_pControl->GetCharAt(currPos);
        if (ch == '{' || ch == '[' || ch == '(' || ch == '<' ||
            ch == '}' || ch == ']' || ch == ')' || ch == '>')
            m_pControl->BraceBadLight(currPos);
        else
            m_pControl->BraceHighlight(-1, -1);
    }
    m_pControl->Refresh(FALSE);
}

int cbEditor::GetLineIndentInSpaces(int line)
{
    int currLine = (line == -1)
                    ? m_pControl->LineFromPosition(m_pControl->GetCurrentPos())
                    : line;
    wxString text = m_pControl->GetLine(currLine);
    unsigned int len = text.Length();
    int spaceCount = 0;
    for (unsigned int i = 0; i < len; ++i)
    {
        if (text[i] == ' ')
            ++spaceCount;
        else if (text[i] == '\t')
            spaceCount += m_pControl->GetTabWidth();
        else 
            break;
    }
    return spaceCount;
}

wxString cbEditor::GetLineIndentString(int line)
{
    int currLine = (line == -1)
                    ? m_pControl->LineFromPosition(m_pControl->GetCurrentPos())
                    : line;
    wxString text = m_pControl->GetLine(currLine);
    unsigned int len = text.Length();
    wxString indent;
    for (unsigned int i = 0; i < len; ++i)
    {
        if (text[i] == ' ' || text[i] == '\t')
            indent << text[i];
        else 
            break;
    }
    return indent;
}

// Creates a submenu for a Context Menu based on the submenu's specific Id
wxMenu* cbEditor::CreateContextSubMenu(long id)
{
    wxMenu* menu = 0;
    if(id == idInsert)
    {
        menu = new wxMenu;
        menu->Append(idEmptyMenu, _("Empty"));
        menu->Enable(idEmptyMenu, false);
    }
    else if(id == idEdit)
    {
        menu = new wxMenu;
        menu->Append(idUndo, _("Undo"));
        menu->Append(idRedo, _("Redo"));
        menu->AppendSeparator();
        menu->Append(idCut, _("Cut"));
        menu->Append(idCopy, _("Copy"));
        menu->Append(idPaste, _("Paste"));
        menu->Append(idDelete, _("Delete"));
        menu->AppendSeparator();
        menu->Append(idSelectAll, _("Select All"));
    
        bool hasSel = m_pControl->GetSelectionEnd() - m_pControl->GetSelectionStart() != 0;
    
        menu->Enable(idUndo, m_pControl->CanUndo());
        menu->Enable(idRedo, m_pControl->CanRedo());
        menu->Enable(idCut, hasSel);
        menu->Enable(idCopy, hasSel);
        menu->Enable(idPaste, m_pControl->CanPaste());
        menu->Enable(idDelete, hasSel);
    }
    else if(id == idBookmarks)
    {
        menu = new wxMenu;
        menu->Append(idBookmarksToggle, _("Toggle bookmark"));
        menu->Append(idBookmarksPrevious, _("Previous bookmark"));
        menu->Append(idBookmarksNext, _("Next bookmark"));
    }
    else if(id == idFolding)
    {
        menu = new wxMenu;
        menu->Append(idFoldingFoldAll, _("Fold all"));
        menu->Append(idFoldingUnfoldAll, _("Unfold all"));
        menu->Append(idFoldingToggleAll, _("Toggle all folds"));
        menu->AppendSeparator();
        menu->Append(idFoldingFoldCurrent, _("Fold current block"));
        menu->Append(idFoldingUnfoldCurrent, _("Unfold current block"));
        menu->Append(idFoldingToggleCurrent, _("Toggle current block"));
    }
    else
        menu = EditorBase::CreateContextSubMenu(id);
    
    return menu;
}

// Adds menu items to context menu (both before and after loading plugins' items)
void cbEditor::AddToContextMenu(wxMenu* popup,bool noeditor,bool pluginsdone)
{
    if(!pluginsdone)
    {
        wxMenu *bookmarks = 0, *folding = 0, *editsubmenu = 0, *insert = 0;
        if(!noeditor)
        {
            insert = CreateContextSubMenu(idInsert);
            editsubmenu = CreateContextSubMenu(idEdit);
            bookmarks = CreateContextSubMenu(idBookmarks);
            folding = CreateContextSubMenu(idFolding);
        }
        if(insert)
        {
            popup->Append(idInsert, _("Insert..."), insert);
            popup->AppendSeparator();
        }
        popup->Append(idSwapHeaderSource, _("Swap header/source"));
        if(!noeditor)
            popup->AppendSeparator();

        if(editsubmenu)
            popup->Append(idEdit, _("Edit"), editsubmenu);
        if(bookmarks)
            popup->Append(idBookmarks, _("Bookmarks"), bookmarks);
        if(folding)
            popup->Append(idFolding, _("Folding"), folding);
    }
    else
    {
        if(!noeditor)
            popup->Append(idConfigureEditor, _("Configure editor"));
        popup->Append(idProperties, _("Properties"));
        
        // remove "Insert/Empty" if more than one entry
        wxMenu* insert = 0;
        wxMenuItem* insertitem = popup->FindItem(idInsert);
        if(insertitem)
            insert = insertitem->GetSubMenu();
        if(insert)
        {
            if (insert->GetMenuItemCount() > 1)
                insert->Delete(idEmptyMenu);
        }
    }
}

void cbEditor::Print(bool selectionOnly, PrintColorMode pcm)
{
    switch (pcm)
    {
        case pcmAsIs:
            m_pControl->SetPrintColourMode(wxSTC_PRINT_NORMAL);
            break;
        case pcmBlackAndWhite:
            m_pControl->SetPrintColourMode(wxSTC_PRINT_BLACKONWHITE);
            break;
        case pcmColorOnWhite:
            m_pControl->SetPrintColourMode(wxSTC_PRINT_COLOURONWHITE);
            break;
        case pcmInvertColors:
            m_pControl->SetPrintColourMode(wxSTC_PRINT_INVERTLIGHT);
            break;
    }
    wxPrinter printer;
    wxPrintout* printout = new cbEditorPrintout(m_Filename, m_pControl, selectionOnly);
    if (!printer.Print(this, printout, false))
    {
        if (wxPrinter::GetLastError() == wxPRINTER_ERROR)
        {
            wxMessageBox(_("There was a problem printing.\n"
                            "Perhaps your current printer is not set correctly?"), _("Printing"), wxICON_ERROR);
        }
    }
    delete printout;
}

// events

void cbEditor::OnContextMenuEntry(wxCommandEvent& event)
{
	// we have a single event handler for all popup menu entries,
	// so that we can add/remove options without the need to recompile
	// the whole project (almost) but more importantly, to
	// *not* break cbEditor's interface for such a trivial task...
	const int id = event.GetId();
	
	if (id == idUndo)
		m_pControl->Undo();
	else if (id == idRedo)
		m_pControl->Redo();
	else if (id == idCut)
		m_pControl->Cut();
	else if (id == idCopy)
		m_pControl->Copy();
	else if (id == idPaste)
		m_pControl->Paste();
	else if (id == idDelete)
		m_pControl->ReplaceSelection(wxEmptyString);
	else if (id == idSelectAll)
		m_pControl->SelectAll();
	else if (id == idSwapHeaderSource)
		Manager::Get()->GetEditorManager()->SwapActiveHeaderSource();
	else if (id == idBookmarksToggle)
		MarkerToggle(BOOKMARK_MARKER);
	else if (id == idBookmarksNext)
		MarkerNext(BOOKMARK_MARKER);
	else if (id == idBookmarksPrevious)
		MarkerPrevious(BOOKMARK_MARKER);
	else if (id == idFoldingFoldAll)
		FoldAll();
	else if (id == idFoldingUnfoldAll)
		UnfoldAll();
	else if (id == idFoldingToggleAll)
		ToggleAllFolds();
	else if (id == idFoldingFoldCurrent)
		FoldBlockFromLine();
	else if (id == idFoldingUnfoldCurrent)
		UnfoldBlockFromLine();
	else if (id == idFoldingToggleCurrent)
		ToggleFoldBlockFromLine();
	else if (id == idConfigureEditor)
		Manager::Get()->GetEditorManager()->Configure();
	else if (id == idProperties)
	{
        if (m_pProjectFile)
            m_pProjectFile->ShowOptions(this);
    }
    else
        event.Skip();
	//Manager::Get()->GetMessageManager()->DebugLog("Leaving OnContextMenuEntry");
}

void cbEditor::OnMarginClick(wxStyledTextEvent& event)
{
    switch (event.GetMargin())
	{
		case 1: // bookmarks and breakpoints margin
			{
				int lineYpix = event.GetPosition();
				int line = m_pControl->LineFromPosition(lineYpix);
		
				MarkerToggle(BREAKPOINT_MARKER, line);
				break;
			}
		case 2: // folding margin
			{
				int lineYpix = event.GetPosition();
				int line = m_pControl->LineFromPosition(lineYpix);
		
				m_pControl->ToggleFold(line);
				break;
			}
	}
}

void cbEditor::OnEditorUpdateUI(wxStyledTextEvent& event)
{
	if (Manager::Get()->GetEditorManager()->GetActiveEditor() == this)
	{
        NotifyPlugins(cbEVT_EDITOR_UPDATE_UI);
		HighlightBraces(); // brace highlighting
    }
}

void cbEditor::OnEditorChange(wxStyledTextEvent& event)
{
    SetModified(m_pControl->GetModify());
}

void cbEditor::OnEditorCharAdded(wxStyledTextEvent& event)
{
    // if message manager is auto-hiding, this will close it if not needed open
    Manager::Get()->GetMessageManager()->Close();

    m_timerWait.Stop();
	int pos = m_pControl->GetCurrentPos();
	char ch = event.GetKey();
	if (ch == '(')
	{
		if (m_pControl->CallTipActive())
			++m_ActiveCalltipsNest;
		NotifyPlugins(cbEVT_EDITOR_CALLTIP);
	}
	if (ch == ')')
	{
		// cancel any active calltip
		m_pControl->CallTipCancel();
		if (m_ActiveCalltipsNest > 0)
		{
			--m_ActiveCalltipsNest;
			NotifyPlugins(cbEVT_EDITOR_CALLTIP);
		}
	}
	else if (ch == '\n')
	{
        m_pControl->BeginUndoAction();
		// new-line: adjust indentation
		bool autoIndent = ConfigManager::Get()->Read("/editor/auto_indent", true);
		bool smartIndent = ConfigManager::Get()->Read("/editor/smart_indent", true);
		int currLine = m_pControl->LineFromPosition(pos);
		if (autoIndent && currLine > 0)
		{
            wxString indent = GetLineIndentString(currLine - 1);
            if (smartIndent)
            {
                // if the last entered char before newline was an opening curly brace,
                // increase indentation level (the closing brace is handled in another block)
                wxChar b = m_pData->GetLastNonWhitespaceChar();
                if (b == '{')
                    indent << '\t'; // TODO: decide between spaces/tabs
            }
			m_pControl->InsertText(pos, indent);
			m_pControl->GotoPos(pos + indent.Length());
		}
		m_pControl->EndUndoAction();
	}
	else if (ch == '}')
	{
		bool smartIndent = ConfigManager::Get()->Read("/editor/smart_indent", true);
		if (smartIndent)
		{
            m_pControl->BeginUndoAction();
            // undo block indentation, if needed
            wxString str = m_pControl->GetLine(m_pControl->GetCurrentLine());
            str.Trim(false);
            str.Trim(true);
            if (str.Matches(_("}")))
            {
                // just the brace here; unindent
                // find opening brace (skipping nested blocks)
                int pos = m_pControl->GetCurrentPos() - 2;
                pos = m_pData->FindBlockStart(pos, '{', '}');
                if (pos != -1)
                {
                    wxString indent = GetLineIndentString(m_pControl->LineFromPosition(pos));
                    indent << '}';
                    m_pControl->DelLineLeft();
                    m_pControl->DelLineRight();
                    pos = m_pControl->GetCurrentPos();
                    m_pControl->InsertText(pos, indent);
                    m_pControl->GotoPos(pos + indent.Length());
                }
            }
            m_pControl->EndUndoAction();
        }
	}
	// we use -2 because the char has already been added and Pos is ahead of it...
	else if ((ch == '"') || // this and the next one are for #include's completion
            (ch == '<') ||
            (ch == '.') ||
			((ch == '>') && (m_pControl->GetCharAt(pos - 2) == '-')) ||
			((ch == ':') && (m_pControl->GetCharAt(pos - 2) == ':')))
	{
        int style = m_pControl->GetStyleAt(pos);
		//Manager::Get()->GetMessageManager()->DebugLog(_("Style at %d is %d (char '%c')"), pos, style, ch);
        if (ch == '"' || ch == '<')
        {
             if (style != wxSTC_C_PREPROCESSOR)
                return;
        }
        else
        {
            if (style != wxSTC_C_DEFAULT && style != wxSTC_C_OPERATOR && style != wxSTC_C_IDENTIFIER)
                return;
        }

		int timerDelay = ConfigManager::Get()->Read("/editor/cc_delay", 500);
		if (timerDelay == 0)
			DoAskForCodeCompletion();
		else
		{
            m_pData->lastPosForCodeCompletion = pos;
			m_timerWait.Start(timerDelay);
        }
    }
}

void cbEditor::OnEditorDwellStart(wxStyledTextEvent& event)
{
	int pos = m_pControl->PositionFromPoint(wxPoint(event.GetX(), event.GetY()));
    int style = m_pControl->GetStyleAt(pos);
    NotifyPlugins(cbEVT_EDITOR_TOOLTIP, style, wxEmptyString, event.GetX(), event.GetY());
}

void cbEditor::OnEditorDwellEnd(wxStyledTextEvent& event)
{
	NotifyPlugins(cbEVT_EDITOR_TOOLTIP_CANCEL);
}

void cbEditor::OnUserListSelection(wxStyledTextEvent& event)
{
	NotifyPlugins(cbEVT_EDITOR_USERLIST_SELECTION, 0, event.GetText());
}

void cbEditor::OnTimer(wxTimerEvent& event)
{
    // ask for code-completion *only* if the editor is still after the "." or "->" operator
    if (m_pControl->GetCurrentPos() == m_pData->lastPosForCodeCompletion)
        DoAskForCodeCompletion();
}

void cbEditor::OnClose(wxCloseEvent& event)
{
	Manager::Get()->GetEditorManager()->Close(this);
}

void cbEditor::DoIndent()
{
    if(m_pControl)
        m_pControl->SendMsg(2327); // wxSTC_CMD_TAB
}

void cbEditor::DoUnIndent()
{
    if(m_pControl)
        m_pControl->SendMsg(2328); // wxSTC_CMD_BACKTAB
}
