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
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(EditorsList);

#define MIN(a,b) (a<b?a:b)
#define MAX(a,b) (a>b?a:b)

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
	m_IntfType(eitTabbed)
{
	SC_CONSTRUCTOR_BEGIN
	m_EditorsList.Clear();
	m_Theme = new EditorColorSet(ConfigManager::Get()->Read("/editor/color_sets/active_color_set", COLORSET_DEFAULT));

	ConfigManager::AddConfiguration(_("Editor"), "/editor");
}

// class destructor
EditorManager::~EditorManager()
{
	SC_DESTRUCTOR_BEGIN
	if (m_Theme)
		delete m_Theme;
		
	if (m_LastFindReplaceData)
		delete m_LastFindReplaceData;
		
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

cbEditor* EditorManager::Open(const wxString& filename, int pos)
{
    SANITY_CHECK(0L);
	wxString fname = UnixFilename(filename);
//	Manager::Get()->GetMessageManager()->DebugLog("Trying to open '%s'", fname.c_str());
    if (!wxFileExists(fname))
        return NULL;
//	Manager::Get()->GetMessageManager()->DebugLog("File exists '%s'", fname.c_str());

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

	if (ed)
		ed->GetControl()->SetFocus();
    
    // check for ProjectFile
    if (ed && !ed->GetProjectFile())
    {
        ProjectsArray* projects = Manager::Get()->GetProjectManager()->GetProjects();
        for (unsigned int i = 0; i < projects->GetCount(); ++i)
        {
            cbProject* prj = projects->Item(i);
            ProjectFile* pf = prj->GetFileByFilename(ed->GetFilename(), false);
            if (pf)
            {
                Manager::Get()->GetMessageManager()->DebugLog("found %s", pf->file.GetFullPath().c_str());
                bool wasModified = ed->GetModified();
                ed->SetProjectFile(pf);
                // don't modify an unmodified file because of ed->SetProjectFile(pf)
                ed->SetModified(wasModified);
                break;
            }
        }
    }
    
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

bool EditorManager::CloseAll()
{
    SANITY_CHECK(false);
	return CloseAllExcept(0L);
}

bool EditorManager::CloseAllExcept(cbEditor* editor)
{
    SANITY_CHECK(false);
    int count = m_EditorsList.GetCount();
	EditorsList::Node* node = m_EditorsList.GetFirst();
    while (node)
	{
        cbEditor* ed = node->GetData();
        EditorsList::Node* next = node->GetNext();
        if (ed && ed != editor && Close(ed))
        {
            node = next;
            --count;
        }
        else
            node = node->GetNext();
    }

    return count == (editor ? 1 : 0);
}

bool EditorManager::CloseActive()
{
    SANITY_CHECK(false);
    return Close(GetActiveEditor());
}

bool EditorManager::Close(const wxString& filename)
{
    SANITY_CHECK(false);
    cbEditor* ed = IsOpen(filename);
    if (ed)
        return Close(ed);
    return true;
}

bool EditorManager::Close(cbEditor* editor)
{
    SANITY_CHECK(false);
    if (editor)
	{
		EditorsList::Node* node = m_EditorsList.Find(editor);
		if (node)
		{
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
			editor->Destroy();
			m_EditorsList.DeleteNode(node);
			return true;
		}
	}
    return true;
}

bool EditorManager::Close(int index)
{
    SANITY_CHECK(false);
	int i = 0;
	for (EditorsList::Node* node = m_EditorsList.GetFirst(); node; node = node->GetNext(), ++i)
	{
		if (i == index)
		{
			cbEditor* ed = node->GetData();
			return Close(ed);
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
	int i = 0;
	for (EditorsList::Node* node = m_EditorsList.GetFirst(); node; node = node->GetNext(), ++i)
	{
		if (i == index)
		{
			cbEditor* ed = node->GetData();
            return ed->SaveAs();
		}
	}
	return false;
}

bool EditorManager::SaveActiveAs()
{
    SANITY_CHECK(false);
	if (GetActiveEditor())
		return GetActiveEditor()->SaveAs();
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
