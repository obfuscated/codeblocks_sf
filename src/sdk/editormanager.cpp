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
#include "cbeditor.h"
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
	
	LoadAutoComplete();
}

// class destructor
EditorManager::~EditorManager()
{
	SC_DESTRUCTOR_BEGIN
	
	SaveAutoComplete();

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
        	cbEditor* ed = InternalGetBuiltinEditor(node);
        	if (ed)
                ed->SetEditorStyle();
        }
    }
}

void EditorManager::LoadAutoComplete()
{
	m_AutoCompleteMap.clear();
	long cookie;
	wxString entry;
	wxConfigBase* conf = ConfigManager::Get();
	wxString oldPath = conf->GetPath();
	conf->SetPath(_("/editor/auto_complete"));
	bool cont = conf->GetFirstEntry(entry, cookie);
	while (cont)
	{
        wxString code = conf->Read(entry, _(""));
        // convert non-printable chars to printable
        code.Replace(_("\\n"), _("\n"));
        code.Replace(_("\\r"), _("\r"));
        code.Replace(_("\\t"), _("\t"));
        m_AutoCompleteMap[entry] = code;
		cont = conf->GetNextEntry(entry, cookie);
	}
	conf->SetPath(oldPath);

    if (m_AutoCompleteMap.size() == 0)
    {
        // default auto-complete items
        m_AutoCompleteMap[_("if")] = _("if (|)\n\t;");
        m_AutoCompleteMap[_("ifb")] = _("if (|)\n{\n\t\n}");
        m_AutoCompleteMap[_("ife")] = _("if (|)\n{\n\t\n}\nelse\n{\n\t\n}");
        m_AutoCompleteMap[_("ifei")] = _("if (|)\n{\n\t\n}\nelse if ()\n{\n\t\n}\nelse\n{\n\t\n}");
        m_AutoCompleteMap[_("while")] = _("while (|)\n\t;");
        m_AutoCompleteMap[_("whileb")] = _("while (|)\n{\n\t\n}");
        m_AutoCompleteMap[_("for")] = _("for (|; ; )\n\t;");
        m_AutoCompleteMap[_("forb")] = _("for (|; ; )\n{\n\t\n}");
        m_AutoCompleteMap[_("class")] = _("class $(Class name)|\n{\n\tpublic:\n\t\t$(Class name)();\n\t\t~$(Class name)();\n\tprotected:\n\t\t\n\tprivate:\n\t\t\n};\n");
        m_AutoCompleteMap[_("struct")] = _("struct |\n{\n\t\n};\n");
    }
}

void EditorManager::SaveAutoComplete()
{
	wxConfigBase* conf = ConfigManager::Get();
	conf->DeleteGroup(_("/editor/auto_complete"));
	wxString oldPath = conf->GetPath();
	conf->SetPath(_("/editor/auto_complete"));
	AutoCompleteMap::iterator it;
	for (it = m_AutoCompleteMap.begin(); it != m_AutoCompleteMap.end(); ++it)
	{
        wxString code = it->second;
        // convert non-printable chars to printable
        code.Replace(_("\n"), _("\\n"));
        code.Replace(_("\r"), _("\\r"));
        code.Replace(_("\t"), _("\\t"));
		conf->Write(it->first, code);
	}
	conf->SetPath(oldPath);
}

cbEditor* EditorManager::InternalGetBuiltinEditor(EditorsList::Node* node)
{
    EditorBase* eb = node->GetData();
    if (eb && eb->IsBuiltinEditor())
        return (cbEditor*)eb;
    return 0;
}

cbEditor* EditorManager::GetBuiltinEditor(EditorBase* eb)
{
    return eb && eb->IsBuiltinEditor() ? (cbEditor*)eb : 0;
}

EditorBase* EditorManager::IsOpen(const wxString& filename)
{
    SANITY_CHECK(NULL);
	wxString uFilename = UnixFilename(filename);
	for (EditorsList::Node* node = m_EditorsList.GetFirst(); node; node = node->GetNext())
	{
        EditorBase* eb = node->GetData();
        wxString fname = eb->GetFilename();
        if (fname.IsSameAs(uFilename) || fname.IsSameAs(EDITOR_MODIFIED + uFilename))
            return eb;
	}

	return NULL;
}

EditorBase* EditorManager::GetEditor(int index)
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
        cbEditor* ed = InternalGetBuiltinEditor(node);
        if (ed)
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
    bool domaximize = false;
    if(!Manager::Get()->GetProjectManager()->IsLoading())
    {
        wxMDIChildFrame* frame = Manager::Get()->GetAppWindow()->GetActiveChild();
        if(!frame || frame->IsMaximized()) domaximize = true;
    }

    EditorBase* eb = IsOpen(fname);
    cbEditor* ed = 0;
    if (eb)
    {
        if (eb->IsBuiltinEditor())
            ed = (cbEditor*)eb;
        else
            return 0; // is open but not a builtin editor
    }

    if (!ed)
    {
        ed = new cbEditor(Manager::Get()->GetAppWindow(), fname, m_Theme);
        if (ed->IsOK())
            AddEditorBase(ed);
        else
        {
			ed->Destroy();
            ed = NULL;
        }
    }

    if(can_updateui)
    {
        if (ed)
        {
            ed->Activate();
            if(domaximize)
                ed->Maximize(true);
            ed->GetControl()->SetFocus();
        }
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

EditorBase* EditorManager::GetActiveEditor()
{
    SANITY_CHECK(0L);
    wxMDIParentFrame *appwindow =Manager::Get()->GetAppWindow();
    if(!appwindow) return 0; // prevents segfault
    return static_cast<EditorBase*>(appwindow->GetActiveChild());
}

void EditorManager::SetActiveEditor(EditorBase* ed)
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
    AddEditorBase(ed);
    #ifdef USE_OPENFILES_TREE
    AddFiletoTree(ed);
    #endif
	ed->Show(true);
    //SetSelection(ed->GetPageIndex());
    return ed;
}

void EditorManager::AddCustomEditor(EditorBase* eb)
{
    SANITY_CHECK();
    AddEditorBase(eb);
}

void EditorManager::RemoveCustomEditor(EditorBase* eb)
{
    SANITY_CHECK();
    RemoveEditorBase(eb);
}

void EditorManager::AddEditorBase(EditorBase* eb)
{
    SANITY_CHECK();
    if (!m_EditorsList.Find(eb))
        m_EditorsList.Append(eb);
}

void EditorManager::RemoveEditorBase(EditorBase* eb)
{
    SANITY_CHECK();
    if (m_EditorsList.Find(eb))
        m_EditorsList.DeleteObject(eb);
}

bool EditorManager::UpdateProjectFiles(cbProject* project)
{
    SANITY_CHECK(false);
	for (EditorsList::Node* node = m_EditorsList.GetFirst(); node; node = node->GetNext())
	{
        cbEditor* ed = InternalGetBuiltinEditor(node);
        if (!ed)
            continue;
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
        EditorBase* eb = node->GetData();
        if(eb && !QueryClose(eb))
            return false; // aborted
        node = node->GetNext();
    }
    return true;
}

bool EditorManager::CloseAllExcept(EditorBase* editor,bool dontsave)
{
    if(!editor)
        SANITY_CHECK(true);
    SANITY_CHECK(false);
    
    int count = m_EditorsList.GetCount();
	EditorsList::Node* node = m_EditorsList.GetFirst();
    if(!dontsave)
    {
        while (node)
        {
            EditorBase* eb = node->GetData();
            if(eb && eb != editor && !QueryClose(eb))
                return false; // aborted
            node = node->GetNext();
        }
    }
    
    count = m_EditorsList.GetCount();
    node = m_EditorsList.GetFirst();
    while (node)
	{
        EditorBase* eb = node->GetData();
        EditorsList::Node* next = node->GetNext();
        if (eb && eb != editor && Close(eb, true))
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

bool EditorManager::QueryClose(EditorBase *editor)
{
    if(!editor) 
        return true;
    cbEditor* ed = editor->IsBuiltinEditor() ? (cbEditor*)editor : 0;
    if (ed && ed->GetModified())
    {
// TODO (mandrav#1#): Move this in cbEditor
        wxString msg;
        msg.Printf(_("File %s is modified...\nDo you want to save the changes?"), ed->GetFilename().c_str());
        switch (wxMessageBox(msg, _("Save file"), wxICON_QUESTION | wxYES_NO | wxCANCEL))
        {
            case wxYES:     if (!ed->Save())
                                return false;
                            break;
            case wxNO:      break;
            case wxCANCEL:  return false;
        }
    }
    else
    {
        return editor->QueryClose();
    }
    return true;
}

bool EditorManager::Close(const wxString& filename,bool dontsave)
{
    SANITY_CHECK(false);
    return Close(IsOpen(filename),dontsave);
}

bool EditorManager::Close(EditorBase* editor,bool dontsave)
{
    SANITY_CHECK(false);
    if (editor)
	{
		EditorsList::Node* node = m_EditorsList.Find(editor);
		if (node)
		{
            if (editor->IsBuiltinEditor())
            {
                cbEditor* ed = (cbEditor*)editor;
                if(!dontsave)
                    if(!QueryClose(ed))
                        return false;
                #ifdef USE_OPENFILES_TREE
                DeleteFilefromTree(ed->GetFilename());
                #endif
                ed->Destroy();
                m_EditorsList.DeleteNode(node);			
                return true;
			}
			else
                return editor->Close();
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
            return Close(node->GetData(),dontsave);
		}
	}
	return false;
}

bool EditorManager::Save(const wxString& filename)
{
    SANITY_CHECK(false);
    cbEditor* ed = GetBuiltinEditor(IsOpen(filename));
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
			cbEditor* ed = InternalGetBuiltinEditor(node);
			if (ed)
                return ed->Save();
		}
	}
	return false;
}

bool EditorManager::SaveActive()
{
    SANITY_CHECK(false);
    cbEditor* ed = GetBuiltinEditor(GetActiveEditor());
	if (ed)
		return ed->Save();
	return true;
}

bool EditorManager::SaveAs(int index)
{
    SANITY_CHECK(false);
    cbEditor* ed = GetBuiltinEditor(GetEditor(index));
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
    cbEditor* ed = GetBuiltinEditor(GetActiveEditor());
	if (ed)
    {
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
        cbEditor* ed = InternalGetBuiltinEditor(node);
        if (ed && !ed->Save())
		{
			wxString msg;
			msg.Printf(_("File %s could not be saved..."), ed->GetFilename().c_str());
			wxMessageBox(msg, _("Error saving file"));
		}
        node = node->GetNext();
    }

    return true;
}

void EditorManager::Print(PrintScope ps, PrintColorMode pcm)
{
    switch (ps)
    {
        case psAllOpenEditors:
        {
            for (EditorsList::Node* node = m_EditorsList.GetFirst(); node; node = node->GetNext())
            {
                cbEditor* ed = InternalGetBuiltinEditor(node);
                if (ed)
                    ed->Print(false, pcm);
                    
            }
            break;
        }
        default:
        {
            cbEditor* ed = GetBuiltinEditor(GetActiveEditor());
            if (ed)
                ed->Print(ps == psSelection, pcm);
            break;
        }
    }
}

void EditorManager::CheckForExternallyModifiedFiles()
{
    SANITY_CHECK();
    bool reloadAll = false; // flag to stop bugging the user
    wxArrayString failedFiles; // list of files failed to reload
	for (EditorsList::Node* node = m_EditorsList.GetFirst(); node; node = node->GetNext())
	{
        cbEditor* ed = InternalGetBuiltinEditor(node);
        if (!ed)
            continue;
        wxFileName fname(ed->GetFilename());
        wxDateTime last = fname.GetModificationTime();
        if (!last.IsEqualTo(ed->GetLastModificationTime()))
        {
            // modified; ask to reload
            int ret = -1;
            if (!reloadAll)
            {
                wxString msg;
                msg.Printf(_("File %s is modified outside the IDE...\nDo you want to reload it (you will lose any unsaved work)?"),
                            ed->GetFilename().c_str());
                ConfirmReplaceDlg dlg(Manager::Get()->GetAppWindow(), msg);
                dlg.SetTitle(_("Reload file?"));
                ret = dlg.ShowModal();
                reloadAll = ret == crAll;
            }
            if (reloadAll || ret == crYes)
            {
                if (!ed->Reload())
                    failedFiles.Add(ed->GetFilename());
            }
            if (ret == crCancel)
                break;
        }
    }
    
    if (failedFiles.GetCount())
    {
        wxString msg;
        msg.Printf(_("Could not reload all files:\n\n%s"), GetStringFromArray(failedFiles, "\n").c_str());
        wxMessageBox(msg, _("Error"), wxICON_ERROR);
    }
}

bool EditorManager::SwapActiveHeaderSource()
{
    SANITY_CHECK(false);
    cbEditor* ed = GetBuiltinEditor(GetActiveEditor());
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
	cbEditor* ed = GetBuiltinEditor(GetActiveEditor());
	if (!ed)
		return -1;
		
	wxStyledTextCtrl* control = ed->GetControl();

	int wordStart = control->WordStartPosition(control->GetCurrentPos(), true);
	int wordEnd = control->WordEndPosition(control->GetCurrentPos(), true);
	wxString wordAtCursor = control->GetTextRange(wordStart, wordEnd);
    bool hasSelection = control->GetSelectionStart() != control->GetSelectionEnd();
    // if selected text is the last searched text, don't suggest "search in selection"
    if ((m_LastFindReplaceData &&
        !control->GetSelectedText().IsEmpty() &&
        control->GetSelectedText() == m_LastFindReplaceData->findText)
        || control->GetSelectedText() == wordAtCursor)
    {
        hasSelection = false;
    }

	FindReplaceBase* dlg;
	if (!replace)
	{
		dlg = new FindDlg(Manager::Get()->GetAppWindow(), wordAtCursor, hasSelection);
		if (dlg->ShowModal() == wxID_CANCEL)
		{
			delete dlg;
			return -2;
		}
	}
	else
	{
		dlg = new ReplaceDlg(Manager::Get()->GetAppWindow(), wordAtCursor, hasSelection);
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
	
	int pos = -1;
	while (true) // loop while not found and user selects to start again from the top
	{
        int lengthFound = 0;
        pos = control->FindText(data->start, data->end, data->findText, flags/*, &lengthFound*/);
        lengthFound = data->findText.Length();
        if (pos != -1)
        {
            control->GotoPos(pos);
            control->EnsureVisible(control->LineFromPosition(pos));
            control->SetSelection(pos, pos + lengthFound);
//            Manager::Get()->GetMessageManager()->DebugLog("pos=%d, selLen=%d, length=%d", pos, data->end - data->start, lengthFound);
            data->start = pos;
            break; // done
        }
        else
        {
            if (!data->scopeSelectedText &&
                ((data->directionDown && start != 0) ||
                (!data->directionDown && start != control->GetLength())))
            {
                wxString msg;
                if (data->directionDown)
                    msg = _("Text not found.\nSearch from the start of the document?");
                else
                    msg = _("Text not found.\nSearch from the end of the document?");
                if (wxMessageBox(msg, _("Result"), wxOK | wxCANCEL | wxICON_QUESTION) == wxOK)
                {
                    if (data->directionDown)
                    {
                        data->start = 0;
                        data->end = control->GetLength();
                    }
                    else
                    {
                        data->start = control->GetLength();
                        data->end = 0;
                    }
                }
                else
                    break; // done
            }
            else
            {
                wxString msg;
                msg.Printf(_("Not found: %s"), data->findText.c_str());
                wxMessageBox(msg, _("Result"), wxICON_INFORMATION);
                break; // done
            }
        }
    }
	
	return pos;
}

int EditorManager::FindNext(bool goingDown)
{
    SANITY_CHECK(-1);
    cbEditor* ed = GetBuiltinEditor(GetActiveEditor());
	if (!m_LastFindReplaceData || !ed)
		return -1;
	
	if (!goingDown && m_LastFindReplaceData->directionDown)
		m_LastFindReplaceData->end = 0;
	else if (goingDown && !m_LastFindReplaceData->directionDown)
		m_LastFindReplaceData->end = m_LastFindReplaceData->start;

	m_LastFindReplaceData->directionDown = goingDown;
	// when going down, no need to add the search-text length, because the cursor
	// is already positioned at the end of the word...
	int multi = goingDown ? 0 : -1;
	m_LastFindReplaceData->start = ed->GetControl()->GetCurrentPos();
	m_LastFindReplaceData->start += multi * (m_LastFindReplaceData->findText.Length() + 1);
	return Find(ed, m_LastFindReplaceData);
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
#if (wxMAJOR_VERSION == 2) && (wxMINOR_VERSION < 5)	
        long int cookie = 0;
#else
        wxTreeItemIdValue cookie; //2.6.0
#endif
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
#if (wxMAJOR_VERSION == 2) && (wxMINOR_VERSION < 5)	
    long int cookie = 0;
#else
    wxTreeItemIdValue cookie; //2.6.0
#endif
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
        cbEditor *ed=GetBuiltinEditor(GetEditor(filename));
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
        cbEditor* ed = InternalGetBuiltinEditor(node);
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
    cbEditor *aed=GetBuiltinEditor(GetActiveEditor());
    if(!aed)
        return;
    bool ismodif=aed->GetModified();
    fname=aed->GetFilename();
    
    if(!force && m_LastActiveFile==fname && m_LastModifiedflag==ismodif)
        return; // Nothing to do
    
    m_LastActiveFile=fname;
    m_LastModifiedflag=ismodif;
    Manager::Get()->GetProjectManager()->FreezeTree();
#if (wxMAJOR_VERSION == 2) && (wxMINOR_VERSION < 5)	
    long int cookie = 0;
#else
    wxTreeItemIdValue cookie; //2.6.0
#endif
    wxTreeItemId item = tree->GetFirstChild(m_TreeOpenedFiles,cookie);
    wxString filename,shortname;
    while (item)
    {
        EditorTreeData *data=(EditorTreeData*)tree->GetItemData(item);
        if(data)
        {
            filename=data->GetFullName();
            cbEditor *ed=GetBuiltinEditor(GetEditor(filename));
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
