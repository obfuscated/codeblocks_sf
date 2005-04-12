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

#include <wx/notebook.h>
#include <wx/menu.h>
#include <wx/splitter.h>
#include <wx/imaglist.h>

#include "editormanager.h" // class's header file
#include "configmanager.h"
#include "messagemanager.h"
#include "projectmanager.h"
#include "manager.h"
#include "editorcolorset.h"
#include "editorconfigurationdlg.h"
#include "finddlg.h"
#include "replacedlg.h"
#include "confirmreplacedlg.h"
#include "projectbuildtarget.h"
#include "cbproject.h"
#include "globals.h"
#include "managerproxy.h"
#include "xtra_classes.h"
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(EditorsList);

#define MIN(a,b) (a<b?a:b)
#define MAX(a,b) (a>b?a:b)

//#define DONT_USE_OPENFILES_TREE

int ID_EditorManager = wxNewId();

BEGIN_EVENT_TABLE(EditorManager,wxEvtHandler)
#ifdef USE_OPENFILES_TREE
    EVT_UPDATE_UI(ID_EditorManager,EditorManager::OnUpdateUI)
    EVT_TREE_SEL_CHANGING(ID_EditorManager, EditorManager::OnTreeItemActivated)
    EVT_TREE_ITEM_ACTIVATED(ID_EditorManager, EditorManager::OnTreeItemActivated)
    EVT_TREE_ITEM_RIGHT_CLICK(ID_EditorManager, EditorManager::OnTreeItemRightClick)
#endif
END_EVENT_TABLE()

// static
bool EditorManager::s_CanShutdown = true;

EditorManager* EditorManager::Get(wxWindow* parent)
{
    if(Manager::isappShuttingDown()) // The mother of all sanity checks
        EditorManager::Free();
    else 
    if (!EditorManagerProxy::Get())
	{
		EditorManagerProxy::Set( new EditorManager(parent) );
		Manager::Get()->GetMessageManager()->Log(_("EditorManager initialized"));
	}
    return EditorManagerProxy::Get();
}

void EditorManager::Free()
{
	if (EditorManagerProxy::Get())
	{
		delete EditorManagerProxy::Get();
		EditorManagerProxy::Set( 0L );
	}
}

// class constructor
EditorManager::EditorManager(wxWindow* parent)
    : //wxNotebook(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,  wxNO_FULL_REPAINT_ON_RESIZE | wxNB_MULTILINE | wxCLIP_CHILDREN),
	m_LastFindReplaceData(0L),
	m_IntfType(eitTabbed),
    m_pImages(0L),
    m_pTree(0L),
    m_LastActiveFile(""),
    m_LastModifiedflag(false)
{
	SC_CONSTRUCTOR_BEGIN
	EditorManagerProxy::Set(this);
	m_EditorsList.Clear();
    #ifdef USE_OPENFILES_TREE
	ShowOpenFilesTree(ConfigManager::Get()->Read("/editor/show_opened_files_tree", true));
	#endif
	m_Theme = new EditorColorSet(ConfigManager::Get()->Read("/editor/color_sets/active_color_set", COLORSET_DEFAULT));
	ConfigManager::AddConfiguration(_("Editor"), "/editor");
	Manager::Get()->GetAppWindow()->PushEventHandler(this);
}

// class destructor
EditorManager::~EditorManager()
{
	SC_DESTRUCTOR_BEGIN
	if (m_Theme)
		delete m_Theme;
		
	if (m_LastFindReplaceData)
		delete m_LastFindReplaceData;
    if (m_pTree)
        { 
            delete m_pTree;
            m_pTree = 0L;
        }
    if (m_pImages)
        {
            delete m_pImages;
            m_pImages = 0L;
        }
    // free-up any memory used for editors
    m_EditorsList.DeleteContents(true); // Set this to false to preserve
    m_EditorsList.Clear();              // linked data.

    SC_DESTRUCTOR_END
}

void EditorManager::CreateMenu(wxMenuBar* menuBar)
{
    SANITY_CHECK();
}

void EditorManager::ReleaseMenu(wxMenuBar* menuBar)
{
    SANITY_CHECK();
}

void EditorManager::Configure()
{
    SANITY_CHECK();
	EditorConfigurationDlg dlg(Manager::Get()->GetAppWindow());
    if (dlg.ShowModal() == wxID_OK)
    {
    	// tell all open editors to re-create their styles
		for (EditorsList::Node* node = m_EditorsList.GetFirst(); node; node = node->GetNext())
		{
        	cbEditor* ed = node->GetData();
            ed->SetEditorStyle();
        }
    }
}

cbEditor* EditorManager::IsOpen(const wxString& filename)
{
    SANITY_CHECK(NULL);
	wxString uFilename = UnixFilename(filename);
	for (EditorsList::Node* node = m_EditorsList.GetFirst(); node; node = node->GetNext())
	{
        cbEditor* ed = node->GetData();
        wxString fname = ed->GetFilename();
        if (fname.IsSameAs(uFilename) || fname.IsSameAs(EDITOR_MODIFIED + uFilename))
            return ed;
	}

	return NULL;
}

void EditorManager::SetEditorInterfaceType(const EditorInterfaceType& _type)
{
    SANITY_CHECK();
	m_IntfType = _type;
	// we should re-create all editors here...
}

cbEditor* EditorManager::GetEditor(int index)
{
    SANITY_CHECK(0L);
	EditorsList::Node* node = m_EditorsList.Item(index);
	if (node)
		return node->GetData();
	return 0L;
}

void EditorManager::SetColorSet(EditorColorSet* theme)
{
    SANITY_CHECK();
	if (m_Theme)
		delete m_Theme;
	
	// copy locally
	m_Theme = new EditorColorSet(*theme);

	for (EditorsList::Node* node = m_EditorsList.GetFirst(); node; node = node->GetNext())
	{
        cbEditor* ed = node->GetData();
		ed->SetColorSet(m_Theme);
	}
}

cbEditor* EditorManager::Open(const wxString& filename, int pos,ProjectFile* data)
{
    SANITY_CHECK(0L);
    bool can_updateui = !GetActiveEditor() || !Manager::Get()->GetProjectManager()->IsLoading();
	wxString fname = UnixFilename(filename);
//	Manager::Get()->GetMessageManager()->DebugLog("Trying to open '%s'", fname.c_str());
    if (!wxFileExists(fname))
        return NULL;
//	Manager::Get()->GetMessageManager()->DebugLog("File exists '%s'", fname.c_str());

    // disallow application shutdown while opening files
    // WARNING: remember to set it to true, when exiting this function!!!
    s_CanShutdown = false;

    cbEditor* ed = IsOpen(fname);
    if (!ed)
    {
        ed = new cbEditor(Manager::Get()->GetAppWindow(), fname, m_Theme);
        if (ed->IsOK())
        {
            m_EditorsList.Append(ed);
#if 0
            int pos = ed->GetControl()->PositionFromLine(line);
            pos += col;
            ed->GetControl()->GotoPos(pos);
			ed->GetControl()->MoveCaretInsideView();
			ed->GetControl()->Refresh();
#endif
            ed->Activate();
//            SetSelection(ed->GetPageIndex());
			//ed->GetControl()->GotoPos(pos);
#if 0
			cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
			if (prj)
			{
				ProjectFile* pf = prj->GetFileByFilename(ed->GetFilename(), false);
				if (pf)
				{
					ed->GetControl()->GotoPos(pf->editorPos);
					pf->editorOpen = true;
				}
			}
#endif
        }
        else
        {
			ed->Destroy();
//            DeletePage(ed->GetPageIndex());
            ed = NULL;
        }
    }
    else
		ed->Activate();
//        SetSelection(ed->GetPageIndex());

	
    if(can_updateui)
    {
        if (ed)
            ed->GetControl()->SetFocus();
    }
    
    // check for ProjectFile
    if (ed && !ed->GetProjectFile())
    {
        // First checks if we're already being passed a ProjectFile
        // as a parameter
        if(data) 
        {
            Manager::Get()->GetMessageManager()->DebugLog("project data set for %s", data->file.GetFullPath().c_str());
        }
        else
        {
            ProjectsArray* projects = Manager::Get()->GetProjectManager()->GetProjects();
            for (unsigned int i = 0; i < projects->GetCount(); ++i)
            {
                cbProject* prj = projects->Item(i);
                ProjectFile* pf = prj->GetFileByFilename(ed->GetFilename(), false);
                if (pf)
                {
                    Manager::Get()->GetMessageManager()->DebugLog("found %s", pf->file.GetFullPath().c_str());
                    data = pf;
                    break;
                }
            }
        }
        if(data)
            ed->SetProjectFile(data,true);
    }
    #ifdef USE_OPENFILES_TREE
    if(can_updateui)
        AddFiletoTree(ed);
    #endif

    // we 're done
    s_CanShutdown = true;

    return ed;
}

cbEditor* EditorManager::GetActiveEditor()
{
    SANITY_CHECK(0L);
    wxMDIParentFrame *appwindow =Manager::Get()->GetAppWindow();
    if(!appwindow) return 0; // prevents segfault
    return static_cast<cbEditor*>(appwindow->GetActiveChild());
}

void EditorManager::SetActiveEditor(cbEditor* ed)
{
    SANITY_CHECK();
    if (ed)
        ed->Activate();
}

cbEditor* EditorManager::New()
{
    SANITY_CHECK(0L);
    cbEditor* ed = new cbEditor(Manager::Get()->GetAppWindow(), wxEmptyString);
	if (!ed->SaveAs())
	{
		//DeletePage(ed->GetPageIndex());
		ed->Destroy();
		return 0L;
	}

    // add default text
    wxString key;
    key.Printf("/editor/default_code/%d", (int)FileTypeOf(ed->GetFilename()));
    wxString code = ConfigManager::Get()->Read(key, wxEmptyString);
    ed->GetControl()->SetText(code);

	ed->SetColorSet(m_Theme);
    m_EditorsList.Append(ed);
    #ifdef USE_OPENFILES_TREE
    AddFiletoTree(ed);
    #endif
	ed->Show(true);
    //SetSelection(ed->GetPageIndex());
    return ed;
}

bool EditorManager::UpdateProjectFiles(cbProject* project)
{
    SANITY_CHECK(false);
	for (EditorsList::Node* node = m_EditorsList.GetFirst(); node; node = node->GetNext())
	{
        cbEditor* ed = node->GetData();
		ProjectFile* pf = ed->GetProjectFile();
		if (!pf)
			continue;
		if (pf->project != project)
			continue;
		pf->editorTopLine = ed->GetControl()->GetFirstVisibleLine();
		pf->editorPos = ed->GetControl()->GetCurrentPos();
		pf->editorOpen = true;
	}
    return true;
}

bool EditorManager::CloseAll(bool dontsave)
{
    SANITY_CHECK(true);
	return CloseAllExcept(0L,dontsave);
}

bool EditorManager::QueryCloseAll()
{
    SANITY_CHECK(true);
	EditorsList::Node* node = m_EditorsList.GetFirst();
    while (node)
	{
        cbEditor* ed = node->GetData();
        if(ed && !QueryClose(ed))
            return false; // aborted
        node = node->GetNext();
    }
    return true;
}

bool EditorManager::CloseAllExcept(cbEditor* editor,bool dontsave)
{
    if(!editor)
        SANITY_CHECK(true);
    SANITY_CHECK(false);
    
    int count = m_EditorsList.GetCount();
	EditorsList::Node* node = m_EditorsList.GetFirst();
    if(!dontsave)
    while (node)
	{
        cbEditor* ed = node->GetData();
        if(ed && ed != editor && !QueryClose(ed))
            return false; // aborted
        node = node->GetNext();
    }
    
    count = m_EditorsList.GetCount();
    node = m_EditorsList.GetFirst();
    while (node)
	{
        cbEditor* ed = node->GetData();
        EditorsList::Node* next = node->GetNext();
        if (ed && ed != editor && Close(ed,true))
        {
            node = next;
            --count;
        }
        else
            node = node->GetNext();
    }
    #ifdef USE_OPENFILES_TREE
    RebuildOpenedFilesTree();
    #endif
    return count == (editor ? 1 : 0);
}

bool EditorManager::CloseActive(bool dontsave)
{
    SANITY_CHECK(false);
    return Close(GetActiveEditor(),dontsave);
}

bool EditorManager::QueryClose(cbEditor *editor)
{
    if(!editor) 
        return true;
    if (editor->GetModified())
    {
        wxString msg;
        msg.Printf(_("File %s is modified...\nDo you want to save the changes?"), editor->GetFilename().c_str());
        switch (wxMessageBox(msg, _("Save file"), wxICON_QUESTION | wxYES_NO | wxCANCEL))
        {
            case wxYES:     if (!editor->Save())
                                return false;
                            break;
            case wxNO:      break;
            case wxCANCEL:  return false;
        }
    }
    return true;
}

bool EditorManager::Close(const wxString& filename,bool dontsave)
{
    SANITY_CHECK(false);
    cbEditor* ed = IsOpen(filename);
    return Close(ed,dontsave);
}

bool EditorManager::Close(cbEditor* editor,bool dontsave)
{
    SANITY_CHECK(false);
    if (editor)
	{
		EditorsList::Node* node = m_EditorsList.Find(editor);
		if (node)
		{
            if(!dontsave)
                if(!QueryClose(editor))
                    return false;
			#ifdef USE_OPENFILES_TREE
			DeleteFilefromTree(editor->GetFilename());
			#endif
			editor->Destroy();
			m_EditorsList.DeleteNode(node);			
			return true;
		}
	}
    return true;
}

bool EditorManager::Close(int index,bool dontsave)
{
    SANITY_CHECK(false);
	int i = 0;
	for (EditorsList::Node* node = m_EditorsList.GetFirst(); node; node = node->GetNext(), ++i)
	{
		if (i == index)
		{
			cbEditor* ed = node->GetData();
			return Close(ed,dontsave);
		}
	}
	return false;
}

bool EditorManager::Save(const wxString& filename)
{
    SANITY_CHECK(false);
    cbEditor* ed = IsOpen(filename);
    if (ed)
        return ed->Save();
    return true;
}

bool EditorManager::Save(int index)
{
    SANITY_CHECK(false);
	int i = 0;
	for (EditorsList::Node* node = m_EditorsList.GetFirst(); node; node = node->GetNext(), ++i)
	{
		if (i == index)
		{
			cbEditor* ed = node->GetData();
            return ed->Save();
		}
	}
	return false;
}

bool EditorManager::SaveActive()
{
    SANITY_CHECK(false);
	if (GetActiveEditor())
		return GetActiveEditor()->Save();
	return true;
}

bool EditorManager::SaveAs(int index)
{
    SANITY_CHECK(false);
	cbEditor *ed = GetEditor(index);
	if(!ed)
        return false;
    wxString oldname=ed->GetFilename();
    if(!ed->SaveAs())
        return false;
    RenameTreeFile(oldname,ed->GetFilename());
    return true;
}

bool EditorManager::SaveActiveAs()
{
    SANITY_CHECK(false);
	if (GetActiveEditor())
    {
        cbEditor *ed=GetActiveEditor();
        wxString oldname=ed->GetFilename();
        if(ed->SaveAs())
            RenameTreeFile(oldname,ed->GetFilename());
    }
	return true;
}

bool EditorManager::SaveAll()
{
    SANITY_CHECK(false);
	EditorsList::Node* node = m_EditorsList.GetFirst();
    while (node)
	{
        cbEditor* ed = node->GetData();
        if (!ed->Save())
		{
			wxString msg;
			msg.Printf(_("File %s could not be saved..."), ed->GetFilename().c_str());
			wxMessageBox(msg, _("Error saving file"));
		}
        node = node->GetNext();
    }

    return true;
}

void EditorManager::UpdateEditorIndices()
{
    SANITY_CHECK();
#if 0
	for (EditorsList::Node* node = m_EditorsList.GetFirst(); node; node = node->GetNext())
	{
        cbEditor* ed = node->GetData();
        // for each node, find the respective notebook panel
        for (int i = 0; i < GetPageCount(); ++i)
        {
            if (GetPageText(i).IsSameAs(ed->GetShortName()) ||
                GetPageText(i).IsSameAs(EDITOR_MODIFIED + ed->GetShortName()))
            {
                ed->SetPageIndex(i);
                break;
            }
        }
	}
#endif
}

void EditorManager::CheckForExternallyModifiedFiles()
{
    SANITY_CHECK();
	for (EditorsList::Node* node = m_EditorsList.GetFirst(); node; node = node->GetNext())
	{
        cbEditor* ed = node->GetData();
        wxFileName fname(ed->GetFilename());
        wxDateTime last = fname.GetModificationTime();
        if (!last.IsEqualTo(ed->GetLastModificationTime()))
        {
            // modified; ask to reload
            wxString msg;
            msg.Printf(_("File %s is modified outside the IDE...\nDo you want to reload it (you will lose any unsaved work)?"), ed->GetFilename().c_str());
            if (wxMessageBox(msg, _("Reload?"), wxICON_QUESTION | wxYES_NO) == wxYES)
            {
                if (!ed->Reload())
                    wxMessageBox(_("Could not reload file!"), _("Error"), wxICON_ERROR);
            }
        }
    }
}

bool EditorManager::SwapActiveHeaderSource()
{
    SANITY_CHECK(false);
    cbEditor* ed = GetActiveEditor();
    if (!ed)
        return false;

	FileType ft = FileTypeOf(ed->GetFilename());
	if (ft != ftHeader && ft != ftSource)
        return 0L;

    // create a list of search dirs
    wxArrayString dirs;
    
    // get project's include dirs
    cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (project)
    {
        dirs = project->GetIncludeDirs();

        // first add all paths that contain project files
        for (int i = 0; i < project->GetFilesCount(); ++i)
        {
            ProjectFile* pf = project->GetFile(i);
            if (pf)
            {
                wxString dir = pf->file.GetPath(wxPATH_GET_VOLUME);
                if (dirs.Index(dir) == wxNOT_FOUND)
                    dirs.Add(dir);
            }
        }
        
        // get targets include dirs
        for (int i = 0; i < project->GetBuildTargetsCount(); ++i)
        {
            ProjectBuildTarget* target = project->GetBuildTarget(i);
            if (target)
            {
                for (unsigned int ti = 0; ti < target->GetIncludeDirs().GetCount(); ++ti)
                {
                    wxString dir = target->GetIncludeDirs()[ti];
                    if (dirs.Index(dir) == wxNOT_FOUND)
                        dirs.Add(dir);
                }
            }
        }
    }

    wxFileName fname;
    wxFileName fn(ed->GetFilename());
    dirs.Insert(fn.GetPath(wxPATH_GET_VOLUME), 0); // add file's dir

    for (unsigned int i = 0; i < dirs.GetCount(); ++i)
    {
        fname.Assign(dirs[i] + wxFileName::GetPathSeparator() + fn.GetFullName());
        if (!fname.IsAbsolute() && project)
        {
            fname.Normalize(wxPATH_NORM_ALL, project->GetBasePath());
        }
        //Manager::Get()->GetMessageManager()->DebugLog("Looking for '%s'", fname.GetFullPath().c_str());
        if (ft == ftHeader)
        {
            fname.SetExt(CPP_EXT);
            if (fname.FileExists())
                break;
            fname.SetExt(C_EXT);
            if (fname.FileExists())
                break;
            fname.SetExt(CC_EXT);
            if (fname.FileExists())
                break;
            fname.SetExt(CXX_EXT);
            if (fname.FileExists())
                break;
        }
        else if (ft == ftSource)
        {
            fname.SetExt(HPP_EXT);
            if (fname.FileExists())
                break;
            fname.SetExt(H_EXT);
            if (fname.FileExists())
                break;
            fname.SetExt(HH_EXT);
            if (fname.FileExists())
                break;
            fname.SetExt(HXX_EXT);
            if (fname.FileExists())
                break;
        }
    }

    if (fname.FileExists())
    {
        //Manager::Get()->GetMessageManager()->DebugLog("ed=%s, pair=%s", ed->GetFilename().c_str(), pair.c_str());
        cbEditor* newEd = Open(fname.GetFullPath());
        //if (newEd)
        //    newEd->SetProjectFile(ed->GetProjectFile());
        return newEd;
    }
    return 0L;
}

int EditorManager::ShowFindDialog(bool replace)
{
    SANITY_CHECK(-1);
	cbEditor* ed = GetActiveEditor();
	if (!ed)
		return -1;
		
	wxStyledTextCtrl* control = ed->GetControl();

	int wordStart = control->WordStartPosition(control->GetCurrentPos(), true);
	int wordEnd = control->WordEndPosition(control->GetCurrentPos(), true);
	wxString wordAtCursor = control->GetTextRange(wordStart, wordEnd);

	FindReplaceBase* dlg;
	if (!replace)
	{
		dlg = new FindDlg(Manager::Get()->GetAppWindow(), wordAtCursor, control->GetSelectionStart() != control->GetSelectionEnd());
		if (dlg->ShowModal() == wxID_CANCEL)
		{
			delete dlg;
			return -2;
		}
	}
	else
	{
		dlg = new ReplaceDlg(Manager::Get()->GetAppWindow(), wordAtCursor, control->GetSelectionStart() != control->GetSelectionEnd());
		if (dlg->ShowModal() == wxID_CANCEL)
		{
			delete dlg;
			return -2;
		}
	}
	
/* TODO (Rick#1#): Implemente Find in Files and delete this sorry message. */
	
	if(dlg->IsFindInFiles())
	{
        wxMessageBox("Find in Files is not implemented yet.\n"
        "Sorry for the inconveniences.\n"
        "The Code::Blocks team.","Our Apologies...");
        delete dlg;
        return -2;
	}
		
	if (!m_LastFindReplaceData)
		m_LastFindReplaceData = new cbFindReplaceData;
		
	m_LastFindReplaceData->start = 0;
	m_LastFindReplaceData->end = 0;
	m_LastFindReplaceData->findText = dlg->GetFindString();
	m_LastFindReplaceData->replaceText = dlg->GetReplaceString();
	m_LastFindReplaceData->findInFiles = dlg->IsFindInFiles();
	m_LastFindReplaceData->matchWord = dlg->GetMatchWord();
	m_LastFindReplaceData->startWord = dlg->GetStartWord();
	m_LastFindReplaceData->matchCase = dlg->GetMatchCase();
	m_LastFindReplaceData->regEx = dlg->GetRegEx();
	m_LastFindReplaceData->directionDown = dlg->GetDirection() == 1;
	m_LastFindReplaceData->originEntireScope = dlg->GetOrigin() == 1;
	m_LastFindReplaceData->scopeSelectedText = dlg->GetScope() == 1;
	
	delete dlg;
	
	if (!replace)
		return Find(ed, m_LastFindReplaceData);
	else
		return Replace(ed, m_LastFindReplaceData);
}

void EditorManager::CalculateFindReplaceStartEnd(cbEditor* editor, cbFindReplaceData* data)
{
    SANITY_CHECK();
	if (!data)
		return;

	wxStyledTextCtrl* control = editor->GetControl();

	data->start = 0;
	data->end = control->GetLength();
		
	if (!data->findInFiles)
	{
		if (!data->originEntireScope) // from pos
			data->start = control->GetCurrentPos();
		else // entire scope
		{
			if (!data->directionDown) // up
				data->start = control->GetLength();
		}

		if (!data->directionDown) // up
			data->end = 0;

		if (data->scopeSelectedText) // selected text
		{
			if (!data->directionDown) // up
			{
				data->start = MAX(control->GetSelectionStart(), control->GetSelectionEnd());
				data->end = MIN(control->GetSelectionStart(), control->GetSelectionEnd());
			}
			else // down
			{
				data->start = MIN(control->GetSelectionStart(), control->GetSelectionEnd());
				data->end = MAX(control->GetSelectionStart(), control->GetSelectionEnd());
			}
		}
	}
}

int EditorManager::Replace(cbEditor* editor, cbFindReplaceData* data)
{
    SANITY_CHECK(-1);
	if (!editor || !data)
		return -1;

	wxStyledTextCtrl* control = editor->GetControl();

	int flags = 0;
	int start = data->start;
	int end = data->end;
	CalculateFindReplaceStartEnd(editor, data);
	
	if ((data->directionDown && (data->start < start)) ||
		(!data->directionDown && (data->start > start)))
		data->start = start;
	if ((data->directionDown && (data->end < end)) ||
		(!data->directionDown && (data->end > end)))
		data->end = end;
	
	if (data->matchWord)
		flags |= wxSTC_FIND_WHOLEWORD;
	if (data->startWord)
		flags |= wxSTC_FIND_WORDSTART;
	if (data->matchCase)
		flags |= wxSTC_FIND_MATCHCASE;
	if (data->regEx)
		flags |= wxSTC_FIND_REGEXP;
	
	control->BeginUndoAction();
	int pos = -1;
	bool replace = false;
	bool confirm = true;
	bool stop = false;
	while (!stop)
	{
		int lengthFound = 0;
		pos = control->FindText(data->start, data->end, data->findText, flags/*, &lengthFound*/);
		lengthFound = data->findText.Length();
		if (pos == -1)
			break;
		control->GotoPos(pos);
		control->EnsureVisible(control->LineFromPosition(pos));
		control->SetSelection(pos, pos + lengthFound);
		data->start = pos;
		//Manager::Get()->GetMessageManager()->DebugLog("pos=%d, selLen=%d, length=%d", pos, data->end - data->start, lengthFound);
		
		if (confirm)
		{
			ConfirmReplaceDlg dlg(Manager::Get()->GetAppWindow());
			switch (dlg.ShowModal())
			{
				case crYes:
					replace = true;
					break;
				case crNo:
					replace = false;
					break;
				case crAll:
					replace = true;
					confirm = false;
					break;
				case crCancel:
					stop = true;
					break;
			}
		}
		
		if (!stop)
		{
			if (replace)
			{
				if (data->regEx)
				{
					// set target same as selection
					control->SetTargetStart(control->GetSelectionStart());
					control->SetTargetEnd(control->GetSelectionEnd());
					// replace with regEx support
					control->ReplaceTargetRE(data->replaceText);
					// reset target
					control->SetTargetStart(0);
					control->SetTargetEnd(0);
				}
				else
					control->ReplaceSelection(data->replaceText);
				data->start += data->replaceText.Length();
				// adjust end pos by adding the length difference between find and replace strings
				int diff = data->replaceText.Length() - lengthFound;
				if (data->directionDown)
					data->end += diff;
				else
					data->end -= diff;
			}
			else
				data->start += lengthFound;
		}
	}
	control->EndUndoAction();
	
	return pos;
}

int EditorManager::Find(cbEditor* editor, cbFindReplaceData* data)
{
    SANITY_CHECK(-1);
	if (!editor || !data)
		return -1;

	wxStyledTextCtrl* control = editor->GetControl();

	int flags = 0;
	int start = data->start;
	int end = data->end;
	CalculateFindReplaceStartEnd(editor, data);
	
	if ((data->directionDown && (data->start < start)) ||
		(!data->directionDown && (data->start > start)))
		data->start = start;
	if ((data->directionDown && (data->end < end)) ||
		(!data->directionDown && (data->end > end)))
		data->end = end;
	
	if (data->matchWord)
		flags |= wxSTC_FIND_WHOLEWORD;
	if (data->startWord)
		flags |= wxSTC_FIND_WORDSTART;
	if (data->matchCase)
		flags |= wxSTC_FIND_MATCHCASE;
	if (data->regEx)
		flags |= wxSTC_FIND_REGEXP;
	
	int lengthFound = 0;
	int pos = control->FindText(data->start, data->end, data->findText, flags/*, &lengthFound*/);
	lengthFound = data->findText.Length();
	if (pos != -1)
	{
		control->GotoPos(pos);
		control->EnsureVisible(control->LineFromPosition(pos));
		control->SetSelection(pos, pos + lengthFound);
//		Manager::Get()->GetMessageManager()->DebugLog("pos=%d, selLen=%d, length=%d", pos, data->end - data->start, lengthFound);
		data->start = pos;
	}
	
	return pos;
}

int EditorManager::FindNext(bool goingDown)
{
    SANITY_CHECK(-1);
	if (!m_LastFindReplaceData)
		return -1;
	
	if (!goingDown && m_LastFindReplaceData->directionDown)
		m_LastFindReplaceData->end = 0;
	else if (goingDown && !m_LastFindReplaceData->directionDown)
		m_LastFindReplaceData->end = m_LastFindReplaceData->start;

	m_LastFindReplaceData->directionDown = goingDown;
	int multi = goingDown ? 1 : -1;
	m_LastFindReplaceData->start = GetActiveEditor()->GetControl()->GetCurrentPos();
	m_LastFindReplaceData->start += multi * (m_LastFindReplaceData->findText.Length() + 1);
	return Find(GetActiveEditor(), m_LastFindReplaceData);
}

#ifdef USE_OPENFILES_TREE
bool EditorManager::OpenFilesTreeSupported()
{
    #if defined(__WXGTK__) || defined(DONT_USE_OPENFILES_TREE)
    return false;
    #else
    return true;
    #endif
}

void EditorManager::ShowOpenFilesTree(bool show)
{
    static int s_SashPosition = 200;

    if (!OpenFilesTreeSupported())
        return;
    if (!m_pTree)
        InitPane();
    if (!m_pTree)
        return;
    if(Manager::isappShuttingDown())
        return;
    wxSplitPanel* mypanel = (wxSplitPanel*)(Manager::Get()->GetNotebookPage("Projects",wxTAB_TRAVERSAL | wxCLIP_CHILDREN,true));
    wxSplitterWindow* mysplitter = mypanel->GetSplitter();
    if (show && !IsOpenFilesTreeVisible())
    {
        m_pTree->Show(true);
        mypanel->RefreshSplitter(ID_EditorManager,ID_ProjectManager,s_SashPosition);
    }
    else if (!show && IsOpenFilesTreeVisible())
    {
        s_SashPosition = mysplitter->GetSashPosition();
        m_pTree->Show(false);
        mypanel->RefreshSplitter(ID_EditorManager,ID_ProjectManager,s_SashPosition);
    }
    // update user prefs
    ConfigManager::Get()->Write("/editor/show_opened_files_tree", show);
}

bool EditorManager::IsOpenFilesTreeVisible()
{
    return m_pTree && m_pTree->IsShown();
}

wxTreeCtrl* EditorManager::GetTree()
{
    SANITY_CHECK(0L);
    return m_pTree;
    // Manager::Get()->GetProjectManager()->GetTree();
}

wxTreeItemId EditorManager::FindTreeFile(const wxString& filename)
{
    wxTreeItemId item = wxTreeItemId();
    SANITY_CHECK(item);
    do
    {
        if(Manager::isappShuttingDown())
            break;
        if(filename=="")
            break;
        wxTreeCtrl *tree=GetTree();
        if(!tree || !m_TreeOpenedFiles)
            break;
        long int cookie = 0;
        for(item = tree->GetFirstChild(m_TreeOpenedFiles,cookie);
            item;
            item = tree->GetNextChild(m_TreeOpenedFiles, cookie))
        {
            if(GetTreeItemFilename(item)==filename)
                break;
        }
    }while(false);
    return item;
}

wxString EditorManager::GetTreeItemFilename(wxTreeItemId item)
{
    SANITY_CHECK("");
    if(Manager::isappShuttingDown())
        return "";
    wxTreeCtrl *tree=GetTree();
    if(!tree || !m_TreeOpenedFiles || !item)
        return "";
    MiscTreeItemData *data=(MiscTreeItemData*)tree->GetItemData(item);
    if(!data)
        return "";
    if(data->GetOwner()!=this)
        return "";
    return ((EditorTreeData*)data)->GetFullName();
}

void EditorManager::DeleteItemfromTree(wxTreeItemId item)
{
    SANITY_CHECK();
    if(Manager::isappShuttingDown())
        return;
    wxTreeCtrl *tree=GetTree();
    if(!tree || !m_TreeOpenedFiles || !item)
        return;
    wxTreeItemId itemparent=tree->GetItemParent(item);
    if(itemparent!=m_TreeOpenedFiles)
        return;
    tree->Delete(item);    
}

void EditorManager::DeleteFilefromTree(const wxString& filename)
{
    SANITY_CHECK();
    if(Manager::isappShuttingDown())
        return;
    DeleteItemfromTree(FindTreeFile(filename));
    RefreshOpenedFilesTree();    
}

void EditorManager::AddFiletoTree(cbEditor* ed)
{
    SANITY_CHECK();
    if(Manager::isappShuttingDown())
        return;
    if(!ed)
        return;
    wxString shortname=ed->GetShortName();
    wxString filename=ed->GetFilename();
    wxTreeItemId item=FindTreeFile(filename);
    if(item.IsOk())
        return;
    wxTreeCtrl *tree=GetTree();
    if(!tree)
        return;
    if(!m_TreeOpenedFiles)
        return;
    if(ed->GetModified()) shortname=wxString("*")+shortname;
    tree->AppendItem(m_TreeOpenedFiles,shortname,2,2,
        new EditorTreeData(this,filename));
    tree->SortChildren(m_TreeOpenedFiles);
    RefreshOpenedFilesTree(true);
}

bool EditorManager::RenameTreeFile(const wxString& oldname, const wxString& newname)
{
    SANITY_CHECK(false);
    if(Manager::isappShuttingDown())
        return false;
    wxTreeCtrl *tree = GetTree();
    if(!tree)
        return false;
    long int cookie = 0;
    wxTreeItemId item;
    wxString filename,shortname;
    for(item=tree->GetFirstChild(m_TreeOpenedFiles,cookie);
        item;
        item = tree->GetNextChild(m_TreeOpenedFiles, cookie))
    {
        EditorTreeData *data=(EditorTreeData*)tree->GetItemData(item);
        if(!data)
            continue;
        filename=data->GetFullName();
        if(filename!=oldname)
            continue;
        data->SetFullName(newname);
        cbEditor *ed=GetEditor(filename);
        if(ed)
        {
            shortname=ed->GetShortName();
            if(ed->GetModified()) shortname=wxString("*")+shortname;
            if(tree->GetItemText(item)!=shortname)
                tree->SetItemText(item,shortname);
            if(ed==GetActiveEditor())
                tree->SelectItem(item);
        }
        return true;
    }
    return false;
}

void EditorManager::InitPane()
{
    #if defined(__WXGTK__) || defined(DONT_USE_OPENFILES_TREE)
        return; // wxGTK uses Tabs, no need for the tree
    #endif

    SANITY_CHECK();
    if(Manager::isappShuttingDown())
        return;
    if(m_pTree)
        return;
    wxSplitPanel* mypanel = (wxSplitPanel*)(Manager::Get()->GetNotebookPage("Projects",wxTAB_TRAVERSAL | wxCLIP_CHILDREN,true));
    wxSplitterWindow* mysplitter = mypanel->GetSplitter();
    BuildOpenedFilesTree(mysplitter);
    mypanel->SetAutoLayout(true);
    mypanel->RefreshSplitter(ID_EditorManager,ID_ProjectManager,200);
}

void EditorManager::BuildOpenedFilesTree(wxWindow* parent)
{
    #if defined(__WXGTK__) || defined(DONT_USE_OPENFILES_TREE)
        return; // wxGTK uses Tabs, no need for the tree
    #endif
    SANITY_CHECK();
    if(m_pTree)
        return;
    m_pTree = new wxTreeCtrl(parent, ID_EditorManager,wxDefaultPosition,wxDefaultSize,wxTR_HAS_BUTTONS | wxNO_BORDER);

    wxBitmap bmp;
    m_pImages = new wxImageList(16, 16);
    wxString prefix = ConfigManager::Get()->Read("data_path") + "/images/";
    bmp.LoadFile(prefix + "gohome.png", wxBITMAP_TYPE_PNG); // workspace
    m_pImages->Add(bmp);
    bmp.LoadFile(prefix + "codeblocks.png", wxBITMAP_TYPE_PNG); // project
    m_pImages->Add(bmp);
    bmp.LoadFile(prefix + "ascii.png", wxBITMAP_TYPE_PNG); // file
    m_pImages->Add(bmp);
    bmp.LoadFile(prefix + "folder_open.png", wxBITMAP_TYPE_PNG); // folder
    m_pImages->Add(bmp);
    m_pTree->SetImageList(m_pImages);
    m_TreeOpenedFiles=m_pTree->AddRoot("Opened Files", 3, 3);
    m_pTree->SetItemBold(m_TreeOpenedFiles);
    RebuildOpenedFilesTree(m_pTree);
}

void EditorManager::RebuildOpenedFilesTree(wxTreeCtrl *tree)
{    
    #if defined(__WXGTK__) || defined(DONT_USE_OPENFILES_TREE)
        return; // wxGTK uses Tabs, no need for the tree
    #endif
    SANITY_CHECK();

    if(Manager::isappShuttingDown())
        return;
    if(!tree)
        tree=GetTree();
    if(!tree)
        return;
    tree->DeleteChildren(m_TreeOpenedFiles);
    if(!GetEditorsCount())
        return;
    tree->Freeze();
    for (EditorsList::Node* node = m_EditorsList.GetFirst(); node; node = node->GetNext())
    {
        cbEditor* ed = node->GetData();
        if(!ed)
            continue;
        wxString shortname=ed->GetShortName();
        if(ed->GetModified()) shortname=wxString("*")+shortname;
        wxTreeItemId item=tree->AppendItem(m_TreeOpenedFiles,shortname,2,2,
          new EditorTreeData(this,ed->GetFilename()));
        if(GetActiveEditor()==ed)
            tree->SelectItem(item);
    }
    tree->Expand(m_TreeOpenedFiles);    
    tree->Thaw();
}

void EditorManager::RefreshOpenedFilesTree(bool force)
{
    #if defined(__WXGTK__) || defined(DONT_USE_OPENFILES_TREE)
        return; // wxGTK uses Tabs, no need for the tree
    #endif
    SANITY_CHECK();
    if(Manager::isappShuttingDown())
        return;
    wxTreeCtrl *tree=GetTree();
    if(!tree)
        return;
    wxString fname;
    cbEditor *aed=GetActiveEditor();
    if(!aed)
        return;
    bool ismodif=aed->GetModified();
    fname=aed->GetFilename();
    
    if(!force && m_LastActiveFile==fname && m_LastModifiedflag==ismodif)
        return; // Nothing to do
    
    m_LastActiveFile=fname;
    m_LastModifiedflag=ismodif;
    Manager::Get()->GetProjectManager()->FreezeTree();
    long int cookie = 0;
    wxTreeItemId item = tree->GetFirstChild(m_TreeOpenedFiles,cookie);
    wxString filename,shortname;
    while (item)
    {
        EditorTreeData *data=(EditorTreeData*)tree->GetItemData(item);
        if(data)
        {
            filename=data->GetFullName();
            cbEditor *ed=GetEditor(filename);
            if(ed)
            {
                shortname=ed->GetShortName();
                if(ed->GetModified()) shortname=wxString("*")+shortname;
                if(tree->GetItemText(item)!=shortname)
                    tree->SetItemText(item,shortname);
                if(ed==aed)
                    tree->SelectItem(item);
                // tree->SetItemBold(item,(ed==aed));
            }
        }
        item = tree->GetNextChild(m_TreeOpenedFiles, cookie);
    }
    Manager::Get()->GetProjectManager()->UnfreezeTree();
}

void EditorManager::OnTreeItemActivated(wxTreeEvent &event)
{
    SANITY_CHECK();
    if(Manager::isappShuttingDown())
        return;
    if(!MiscTreeItemData::OwnerCheck(event,GetTree(),this,true))
        return;
    wxString filename=GetTreeItemFilename(event.GetItem());
    if(filename=="")
        return;
    Open(filename);
}

void EditorManager::OnTreeItemRightClick(wxTreeEvent &event)
{
    SANITY_CHECK();
    if(Manager::isappShuttingDown())
        return;
    if(!MiscTreeItemData::OwnerCheck(event,GetTree(),this,true))
        return;
    Manager::Get()->GetMessageManager()->DebugLog("(Editor Tree Popup menu not implemented yet)");
}

void EditorManager::OnUpdateUI(wxUpdateUIEvent& event)
{
    // no need for check (happens in RefreshOpenedFilesTree, if called)
//    SANITY_CHECK();
    if(!Manager::isappShuttingDown())
        RefreshOpenedFilesTree();

    // allow other UpdateUI handlers to process this event
    // *very* important! don't forget it...
    event.Skip();
}

#else
void EditorManager::OnTreeItemSelected(wxTreeEvent &event)
{
    event.Skip();
}

void EditorManager::OnTreeItemActivated(wxTreeEvent &event) 
{
    event.Skip();
}
void EditorManager::OnTreeItemRightClick(wxTreeEvent &event) 
{
    event.Skip();
}
void EditorManager::OnUpdateUI(wxUpdateUIEvent& event)
{
    event.Skip();
}

#endif
