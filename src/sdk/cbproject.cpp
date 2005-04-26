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

#ifndef wxUSE_CHOICEDLG
#define wxUSE_CHOICEDLG 1
#endif

#include <wx/choicdlg.h>
#include "cbproject.h" // class's header file
#include "sdk_events.h"
#include "manager.h"
#include "projectoptionsdlg.h"
#include "projectloader.h"
#include "devcpploader.h"
#include "msvcloader.h"
#include "msvc7loader.h"
#include "projectlayoutloader.h"
#include "selecttargetdlg.h"
#include "globals.h"
#include "pluginmanager.h"
#include "projectmanager.h"
#include "messagemanager.h"
#include "editormanager.h"
#include "configmanager.h"
#include "filegroupsandmasks.h"
#include "compilerfactory.h"
#include "importers_globals.h"

// class constructor
cbProject::cbProject(const wxString& filename)
    : m_ActiveTarget(-1),
    m_DefaultExecuteTarget(-1),
    m_Makefile(""),
    m_CustomMakefile(false),
    m_Loaded(false),
    m_CurrentlyLoading(false),
    m_BasePath("")
{
    SetCompilerIndex(CompilerFactory::GetDefaultCompilerIndex());

	m_Files.Clear();
    if (!filename.IsEmpty())
    {
		// existing project
		m_Filename = filename;
        Open();
    }
    else
    {
		// new project
        SetModified(true);
        m_Filename = CreateUniqueFilename();
        m_Loaded = SaveAs();
		if (m_Loaded)
		{
            wxFileName fname(m_Filename);
			m_Title = fname.GetName();
			m_CommonTopLevelPath = GetBasePath() + wxFileName::GetPathSeparator();
			NotifyPlugins(cbEVT_PROJECT_OPEN);
		}
    }
}

// class destructor
cbProject::~cbProject()
{
	NotifyPlugins(cbEVT_PROJECT_CLOSE);
	ClearAllProperties();
}

void cbProject::NotifyPlugins(wxEventType type)
{
	CodeBlocksEvent event(type);
	event.SetProject(this);
	Manager::Get()->GetPluginManager()->NotifyPlugins(event);
}

void cbProject::SetCompilerIndex(int compilerIdx)
{
    if (compilerIdx != m_CompilerIdx)
    {
        // update object filenames
        for (unsigned int i = 0; i < m_Targets.GetCount(); ++i)
        {
            ProjectBuildTarget* target = m_Targets[i];
            if (target)
            {
                int count = GetFilesCount();
                for (int i = 0; i < count; ++i)
                {
                    ProjectFile* pf = GetFile(i);
                    wxFileName obj(pf->GetObjName());
                    if (FileTypeOf(pf->relativeFilename) != ftResource &&
                        obj.GetExt() == CompilerFactory::Compilers[m_CompilerIdx]->GetSwitches().objectExtension)
                    {
                        obj.SetExt(CompilerFactory::Compilers[compilerIdx]->GetSwitches().objectExtension);
                        pf->SetObjName(obj.GetFullName());
                    }
                }
            }
        }
        m_CompilerIdx = compilerIdx;
        SetModified(true);
    }
}

bool cbProject::GetModified()
{
	if (CompileOptionsBase::GetModified())
		return true;
	
	// check targets
    for (unsigned int i = 0; i < m_Targets.GetCount(); ++i)
    {
        ProjectBuildTarget* target = m_Targets[i];
        if (target->GetModified())
            return true;
    }
	
	return false;
}

void cbProject::SetModified(bool modified)
{
	CompileOptionsBase::SetModified(modified);
	
	// modify targets
    for (unsigned int i = 0; i < m_Targets.GetCount(); ++i)
    {
        ProjectBuildTarget* target = m_Targets[i];
        target->SetModified(modified);
    }
}

void cbProject::SetMakefileCustom(bool custom)
{
    if (m_CustomMakefile != custom)
    {
        m_CustomMakefile = custom;
        SetModified(true);
    }
}

wxString cbProject::CreateUniqueFilename()
{
    const wxString prefix = _("Untitled");
    wxString tmp;
    ProjectsArray* arr = Manager::Get()->GetProjectManager()->GetProjects();
    int projCount = arr->GetCount();
    int iter = 1;
	bool ok = false;
    tmp << prefix << iter;
    while (!ok)
    {
        tmp.Clear();
        tmp << prefix << iter;
        
		ok = true;
        for (int i = 0; i < projCount; ++i)
        {
            cbProject* prj = arr->Item(i);
            wxFileName fname(prj->GetFilename());

            if (fname.GetName().Matches(tmp))
			{
				ok = false;
                break;
			}
        }
		if (ok)
			break;
        ++iter;
    }
    return tmp << "." << CODEBLOCKS_EXT;
}

void cbProject::ClearAllProperties()
{
    m_Files.Clear();
    m_CompilerOptions.Clear();
    m_LinkerOptions.Clear();
    m_IncludeDirs.Clear();
    m_LibDirs.Clear();
    
    while (m_Targets.GetCount())
    {
        ProjectBuildTarget* target = m_Targets[0];
        delete target;
        m_Targets.RemoveAt(0);
    }
    SetModified(true);
}

void cbProject::Open()
{
    m_Loaded = false;
    m_ProjectFilesMap.clear();

	if (!wxFileName::FileExists(m_Filename))
	{
        wxString msg;
        msg.Printf(_("Project '%s' does not exist..."), m_Filename.c_str());
        wxMessageBox(msg, _("Error"), wxOK | wxCENTRE | wxICON_ERROR);
        return;
    }
	
	bool fileUpgraded = false;
    wxFileName fname(m_Filename);
	FileType ft = FileTypeOf(m_Filename);
    if (ft == ftCodeBlocksProject)
    {
		Manager::Get()->GetMessageManager()->AppendLog(_("Opening %s: "), m_Filename.c_str());    
        m_CurrentlyLoading = true;
        ProjectLoader loader(this);
        m_Loaded = loader.Open(m_Filename);
        fileUpgraded = loader.FileUpgraded();
        m_CurrentlyLoading = false;
    }
    else
    {
        Manager::Get()->GetMessageManager()->AppendLog(_("Importing %s: "), m_Filename.c_str());    
        IBaseLoader* loader = 0L;
        switch (ft)
        {
             case ftDevCppProject: loader = new DevCppLoader(this); break;
             case ftMSVCProject: loader = new MSVCLoader(this); break;
             case ftMSVSProject: loader = new MSVC7Loader(this); break;
             default: return;
        }

        int compilerIdx = -1;
        if (ImportersGlobals::UseDefaultCompiler)
            compilerIdx = CompilerFactory::GetDefaultCompilerIndex();
        else
        {
            // select compiler for the imported project
            // need to do it before actual import, because the importer might need
            // project's compiler information (like the object files extension etc).
    
            // first build a list of available compilers
            wxString* comps = new wxString[CompilerFactory::Compilers.GetCount()];
            for (unsigned int i = 0; i < CompilerFactory::Compilers.GetCount(); ++i)
            {
                comps[i] = CompilerFactory::Compilers[i]->GetName();
            }
            // now display a choice dialog
            wxSingleChoiceDialog dlg(0,
                                _("Select compiler to use for the imported project"),
                                _("Select compiler for ") + wxFileName(m_Filename).GetFullName(),
                                CompilerFactory::Compilers.GetCount(),
                                comps);
            dlg.SetSelection(CompilerFactory::GetDefaultCompilerIndex());
            if (dlg.ShowModal() == wxID_OK)
                compilerIdx = dlg.GetSelection();
        }

        if (compilerIdx != -1)
        {
            SetCompilerIndex(compilerIdx);

            // actually import project file
            m_CurrentlyLoading = true;
            m_Loaded = loader->Open(m_Filename);
            fname.SetExt(CODEBLOCKS_EXT);
            m_Filename = fname.GetFullPath();
            SetModified(true);
            m_CurrentlyLoading = false;
        }
        else
            m_Loaded = false;
        delete loader;
    }
	
    if (m_Loaded)
	{
        CalculateCommonTopLevelPath();
		Manager::Get()->GetMessageManager()->Log(_("done"));    
		if (!m_Targets.GetCount())
			AddDefaultBuildTarget();
		SetModified(ft != ftCodeBlocksProject || fileUpgraded);
		NotifyPlugins(cbEVT_PROJECT_OPEN);
		
		if (fileUpgraded)
		{
            wxString msg;
            msg.Printf(_("The project file of \"%s\" needs to be updated to the latest format.\n"
                        "This will happen automatically when you save the project."), m_Title.c_str());
            wxMessageBox(msg, _("Information"), wxICON_INFORMATION);
        }
	}
	else
		Manager::Get()->GetMessageManager()->Log(_("failed"));    
}

void cbProject::CalculateCommonTopLevelPath()
{
    // find the common toplevel path
    // for simple projects, this might be the path to the project file
    // for projects where the project file is in a subdir, files will have ..
    // in their paths
    wxString sep = wxFileName::GetPathSeparator();
    wxFileName base = GetBasePath() + sep;
    Manager::Get()->GetMessageManager()->DebugLog("Project's base path: %s", base.GetFullPath().c_str());
    for (FilesList::Node* node = m_Files.GetFirst(); node; node = node->GetNext())
    {
        ProjectFile* f = node->GetData();
        wxString tmp = f->relativeFilename;
        wxFileName tmpbase = GetBasePath() + sep;
        while (tmp.StartsWith(_("..")))
        {
            tmpbase.AppendDir(_(".."));
            tmp.Remove(0, 2); // two dots
            // remove separator(s) after dots
            while (!tmp.IsEmpty() &&  (tmp.GetChar(0) == '/' || tmp.GetChar(0) == '\\'))
                tmp.Remove(0, 1);
        }
        tmpbase.Normalize(wxPATH_NORM_ALL);

        if (tmpbase.GetDirCount() < base.GetDirCount())
            base = tmpbase;
    }

    // update ProjectFiles info
    for (FilesList::Node* node = m_Files.GetFirst(); node; node = node->GetNext())
    {
        ProjectFile* f = node->GetData();
        wxFileName fname = f->file;
        fname.MakeRelativeTo(base.GetFullPath());
        f->relativeToCommonTopLevelPath = fname.GetFullPath();
        f->SetObjName(f->relativeToCommonTopLevelPath);
    }
    m_CommonTopLevelPath = base.GetFullPath();
    Manager::Get()->GetMessageManager()->DebugLog("Project's common toplevel path: %s", m_CommonTopLevelPath.c_str());
}

wxString cbProject::GetCommonTopLevelPath()
{
    return m_CommonTopLevelPath;
}

bool cbProject::SaveAs()
{
    wxFileName fname;
    fname.Assign(m_Filename);
    wxFileDialog dlg(Manager::Get()->GetAppWindow(),
                    _("Save file"),
                    fname.GetPath(),
                    fname.GetFullName(),
                    CODEBLOCKS_FILES_FILTER,
                    wxSAVE | wxOVERWRITE_PROMPT);

    if (dlg.ShowModal() != wxID_OK)
        return false;
    m_Filename = dlg.GetPath();
    fname.Assign(m_Filename);
    Manager::Get()->GetProjectManager()->GetTree()->SetItemText(m_ProjectNode, fname.GetFullName());
    if (!m_Loaded)
        AddDefaultBuildTarget();
    ProjectLoader loader(this);
    if (loader.Save(m_Filename))
	{
		NotifyPlugins(cbEVT_PROJECT_SAVE);
		return true;
	}
	return false;
}

bool cbProject::Save()
{
    if (m_Filename.IsEmpty())
        return SaveAs();
    ProjectLoader loader(this);
    if (loader.Save(m_Filename))
	{
		NotifyPlugins(cbEVT_PROJECT_SAVE);
		return true;
	}
	return false;
}

bool cbProject::SaveLayout()
{
    if (m_Filename.IsEmpty())
        return false;

	wxFileName fname(m_Filename);
	fname.SetExt("layout");
    ProjectLayoutLoader loader(this);
    return loader.Save(fname.GetFullPath());
}

bool cbProject::LoadLayout()
{
    if (m_Filename.IsEmpty())
        return false;
    int openmode = ConfigManager::Get()->Read("/project_manager/open_files", (long int)1);
    switch (openmode)
    {
        case 0: // open all files
            {
                FilesList::Node* node = m_Files.GetFirst();
                while(node)
                {
                    ProjectFile* f = node->GetData();
                    Manager::Get()->GetEditorManager()->Open(f->file.GetFullPath(),0,f);
                    node = node->GetNext();
                }
            }
            break;
        
        case 1: // open last open files
            {
                wxFileName fname(m_Filename);
                fname.SetExt("layout");
                ProjectLayoutLoader loader(this);
                if (loader.Open(fname.GetFullPath()))
                {
                    FilesList::Node* node = m_Files.GetFirst();
                    while(node)
                    {
                        ProjectFile* f = node->GetData();
                        if (f->editorOpen)
                        {
                            cbEditor* ed = Manager::Get()->GetEditorManager()->Open(f->file.GetFullPath(),0,f);
                            if (ed)
                                ed->SetProjectFile(f);
                        }
                        node = node->GetNext();
                    }
                    ProjectFile* f = loader.GetTopProjectFile();
                    if (f)
                    {
                        Manager::Get()->GetMessageManager()->DebugLog(_T("Top Editor: %s"),_T(f->file.GetFullPath().c_str()));
                        EditorBase* eb = Manager::Get()->GetEditorManager()->Open(f->file.GetFullPath());
                        if(eb)
                        {
                            Manager::Get()->GetProjectManager()->SetTopEditor(eb);
                            eb->Activate();
                        }
                    }
                    return true;
                }
            }
            break;

        case 2: // do not open any files
            return true;
        
        default: break;
    }
	return false;
}

ProjectFile* cbProject::AddFile(const wxString& targetName, const wxString& filename, bool compile, bool link, unsigned short int weight)
{
    int idx = IndexOfBuildTargetName(targetName);
    return AddFile(idx, filename, compile, link, weight);
}

ProjectFile* cbProject::AddFile(int targetIndex, const wxString& filename, bool compile, bool link, unsigned short int weight)
{
    // look if the file belongs to the project already. If so, return it
    ProjectFile* f;

//  NOTE (Rick#1#): When loading the project, do not search for existing files
//  (Assumming that there are no duplicate entries in the .cbp file)
//  This saves us a lot of processing when loading large projects.
//  Remove the if to do the search anyway

//  NOTE (mandrav#1#): We can't ignore that because even if we can rely on .cbp
//  containing discrete files, we can't do that for imported projects...
//  This means we have to search anyway.
//  NP though, I added a hashmap for fast searches in GetFileByFilename()

    f = GetFileByFilename(filename, true, true);
    if (f)
        return f;
    f = GetFileByFilename(filename, false, true);
    if (f)
        return f;

    // OK, add file
    f = new ProjectFile;
    bool localCompile, localLink;
    wxFileName fname;
    wxString ext;

	f->project = this;
	f->editorOpen = false;
	f->editorPos = 0;
	f->editorTopLine = 0;
	f->useCustomBuildCommand = false;
	f->autoDeps = true;
    f->weight = weight;

	fname = UnixFilename(filename);
	ext = fname.GetExt().Lower();
	if (ext.Matches(CPP_EXT) ||
		ext.Matches(CXX_EXT))
        f->compilerVar = "CPP";
	else if (ext.Matches(C_EXT) ||
		ext.Matches(CC_EXT))
        f->compilerVar = "CC";
#ifdef __WXMSW__
	else if (ext.Matches(RESOURCE_EXT))
        f->compilerVar = "WINDRES";
#endif

    if (!m_Targets.GetCount())
    {
        // no targets in project; add default
        AddDefaultBuildTarget();
        if (!m_Targets.GetCount())
        {
            delete f;
            return 0L; // if that failed, fail addition of file...
        }
    }

    if (targetIndex >= 0 && targetIndex < (int)m_Targets.GetCount())
        f->AddBuildTarget(m_Targets[targetIndex]->GetTitle());

	FileType ft = FileTypeOf(filename);
    localCompile = compile &&
					(ft == ftSource ||
					ft == ftResource);
    localLink = link &&
				(ft == ftSource ||
				ft == ftResource ||
				ft == ftObject ||
				ft == ftResourceBin ||
				ft == ftStaticLib);

    
    f->compile = localCompile;
    f->link = localLink;
    fname.Assign(UnixFilename(filename));
    if(!m_CurrentlyLoading || m_BasePath.IsEmpty())
        m_BasePath = GetBasePath();
    fname.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE, m_BasePath);
    
    fname.Assign(fname.GetFullPath());
    f->file.Assign(fname);
	//Manager::Get()->GetMessageManager()->Log("Adding %s", f->file.GetFullPath().c_str());
    fname.MakeRelativeTo(m_BasePath);
    f->relativeFilename = fname.GetFullPath();
    
    m_Files.Append(f);
    if (!m_CurrentlyLoading)
    {
        CalculateCommonTopLevelPath();        
    }
    SetModified(true);
    m_ProjectFilesMap[UnixFilename(f->relativeFilename)] = f; // add to hashmap
	return f;
}

bool cbProject::RemoveFile(int index)
{
    ProjectFile* f = m_Files[index];
    m_ProjectFilesMap.erase(UnixFilename(f->relativeFilename)); // remove from hashmap
    Manager::Get()->GetEditorManager()->Close(f->file.GetFullPath());
    
    FilesList::Node* node = m_Files.Item(index);
    m_Files.DeleteNode(node);
	
    SetModified(true);
	return true;
}

int filesSort(const ProjectFile** arg1, const ProjectFile** arg2)
{
    return (*arg1)->file.GetFullPath().CompareTo((*arg2)->file.GetFullPath());
}

void cbProject::BuildTree(wxTreeCtrl* tree, const wxTreeItemId& root, bool categorize, bool useFolders, FilesGroupsAndMasks* fgam)
{
    if (!tree)
        return;
    
    //sort list of files
    m_Files.Sort(filesSort);
    
    FileTreeData* ftd = new FileTreeData(this);
    m_ProjectNode = tree->AppendItem(root, GetTitle(), 1, 1, ftd);
    wxTreeItemId others = m_ProjectNode;
    
    // create file-type categories nodes (if enabled)
    wxTreeItemId* pGroupNodes = 0L;
    if (categorize && fgam)
    {
        pGroupNodes = new wxTreeItemId[fgam->GetGroupsCount()];
        for (unsigned int i = 0; i < fgam->GetGroupsCount(); ++i)
        {
            pGroupNodes[i] = tree->AppendItem(m_ProjectNode, fgam->GetGroupName(i), 3, 3);
        }
        // add a default category "Others" for all non-matching file-types
        others = tree->AppendItem(m_ProjectNode, _("Others"), 3, 3);
    }

    // iterate all project files and add them to the tree
    int count = 0;
    for (FilesList::Node* node = m_Files.GetFirst(); node; node = node->GetNext())
    {
        ProjectFile* f = node->GetData();
        ftd = new FileTreeData(this, count++);

        wxTreeItemId parentNode = m_ProjectNode;
        // check if files grouping is enabled and find the group parent
        if (categorize && pGroupNodes && fgam)
        {
            bool found = false;
            for (unsigned int i = 0; i < fgam->GetGroupsCount(); ++i)
            {
                wxFileName fname(f->relativeToCommonTopLevelPath);
                if (fgam->MatchesMask(fname.GetFullName(), i))
                {
                    parentNode = pGroupNodes[i];
                    found = true;
                    break;
                }
            }
            // if not matched a group, put it in "Others" group
            if (!found)
                parentNode = others;
        }
        // add file in the tree
        AddTreeNode(tree, f->relativeToCommonTopLevelPath, parentNode, useFolders, f->compile, ftd);
    }

	// remove empty tree nodes (like empty groups)
    if (categorize && fgam)
    {
		for (unsigned int i = 0; i < fgam->GetGroupsCount(); ++i)
		{
			if (tree->GetChildrenCount(pGroupNodes[i], false) == 0)
				tree->Delete(pGroupNodes[i]);
		}
		if (tree->GetChildrenCount(others, false) == 0)
			tree->Delete(others);
	}
    delete[] pGroupNodes;

    tree->Expand(m_ProjectNode);
}

void cbProject::AddTreeNode(wxTreeCtrl* tree, const wxString& text, const wxTreeItemId& parent, bool useFolders, bool compiles, FileTreeData* data)
{
    // see if the text contains any path info, e.g. plugins/compilergcc/compilergcc.cpp
    // in that case, take the first element (plugins in this example), create a sub-folder
    // with the same name and recurse with the result...
    
    wxString path = text;
    int pos = path.Find('/');
    if (pos == -1)
        pos = path.Find('\\');
    if (useFolders && pos >= 0)
    {
        // ok, we got it. now split it up and recurse
        wxString folder = path.Left(pos);
        path = path.Right(path.Length() - pos - 1);
        
        // see if we already have this path
#if (wxMAJOR_VERSION == 2) && (wxMINOR_VERSION < 5)	
        long int cookie = 0;
#else
        wxTreeItemIdValue cookie; //2.6.0
#endif
        wxTreeItemId newparent = tree->GetFirstChild(parent, cookie);
        while (newparent)
        {
            wxString itemText = tree->GetItemText(newparent);
            if (itemText.Matches(folder))
                break;
            newparent = tree->GetNextChild(parent, cookie);
        }
        
        if (!newparent)
		{
			// in order not to override wxTreeCtrl to sort alphabetically but the
			// folders be always on top, we just search here where to put the new folder...
#if (wxMAJOR_VERSION == 2) && (wxMINOR_VERSION < 5)	
            long int cookie2 = 0;
#else
            wxTreeItemIdValue cookie2; //2.6.0
#endif
			wxTreeItemId child = tree->GetFirstChild(parent, cookie2);
			wxTreeItemId lastChild;
			while (child)
			{
				if (tree->GetItemImage(child) == 3)
				{
					if (folder.CompareTo(tree->GetItemText(child)) < 0)
					{
						newparent = tree->InsertItem(parent, lastChild, folder, 3, 3);
						break;
					}
				}
				else
				{
					newparent = tree->PrependItem(parent, folder, 3, 3);
					break;
				}
				lastChild = child;
				child = tree->GetNextChild(parent, cookie2);
			}
			if (!newparent)
				newparent = tree->AppendItem(parent, folder, 3, 3);
		}
		//tree->SortChildren(parent);
        AddTreeNode(tree, path, newparent, true, compiles, data);
    }
    else
	{
        wxTreeItemId newnode = tree->AppendItem(parent, text, 2, 2, data);
		// the following doesn't seem to work under wxMSW...
		if (!compiles)
			tree->SetItemTextColour(newnode, wxColour(0x80, 0x80, 0x80));
	}
}

void cbProject::SaveTreeState(wxTreeCtrl* tree)
{
	::SaveTreeState(tree, m_ProjectNode, m_ExpandedNodes);
}

void cbProject::RestoreTreeState(wxTreeCtrl* tree)
{
	::RestoreTreeState(tree, m_ProjectNode, m_ExpandedNodes);
}

const wxString& cbProject::GetMakefile()
{
	if (!m_Makefile.IsEmpty())
		return m_Makefile;
		
	wxFileName makefile(m_Makefile);
    makefile.Assign(m_Filename);
    makefile.SetName("Makefile");
    makefile.SetExt("");
    makefile.MakeRelativeTo(GetBasePath());

    m_Makefile = makefile.GetFullPath();
	
	SetModified(true);
	return m_Makefile;
}

ProjectFile* cbProject::GetFile(int index)
{
    FilesList::Node* node = m_Files.Item(index);
    if (node)
        return node->GetData();
        
    return NULL;
}

ProjectFile* cbProject::GetFileByFilename(const wxString& filename, bool isRelative, bool isUnixFilename)
{
    // m_ProjectFilesMap keeps UnixFilename(ProjectFile::relativeFilename)
    wxString tmp = filename;
    if (!isRelative)
    {
        // if the search is not relative, make it
        wxFileName fname(filename);
        fname.MakeRelativeTo(GetBasePath());
        tmp = fname.GetFullPath();
    }
    else
    {
        // make sure filename doesn't start with ".\"
        // our own relative files don't have it, so the search would fail
        // this happens when importing MS projects...
        if (tmp.StartsWith(".\\"))
            tmp.Remove(0, 2);
    }

    if (isUnixFilename)
        return m_ProjectFilesMap[tmp];
    return m_ProjectFilesMap[UnixFilename(tmp)];
}

bool cbProject::QueryCloseAllFiles()
{
    FilesList::Node* node;
    node = m_Files.GetFirst();
    while(node)
    {
        ProjectFile* f = node->GetData();
        cbEditor* ed = Manager::Get()->GetEditorManager()->IsBuiltinOpen(f->file.GetFullPath());
        if (ed && ed->GetModified())
        {
            if (!Manager::Get()->GetEditorManager()->QueryClose(ed))
                return false;
        }
        node = node->GetNext();
    }
    return true;
}

bool cbProject::CloseAllFiles(bool dontsave)
{
	// first try to close modified editors
	
    if(!dontsave)
        if(!QueryCloseAllFiles())
            return false;

	// now free the rest of the project files
    int count = m_Files.GetCount();
    FilesList::Node* node = m_Files.GetFirst();
    while(node)
    {
        ProjectFile* f = node->GetData();
        if (Manager::Get()->GetEditorManager()->Close(f->file.GetFullPath(),true))
        {
            FilesList::Node* oldNode = node;
            node = node->GetNext();
            m_Files.DeleteNode(oldNode);
            --count;
        }
        else
            node = node->GetNext();
    }
    return count == 0;
}

bool cbProject::SaveAllFiles()
{
    int count = m_Files.GetCount();
    FilesList::Node* node = m_Files.GetFirst();
    while(node)
    {
        ProjectFile* f = node->GetData();
        if (Manager::Get()->GetEditorManager()->Save(f->file.GetFullPath()))
            --count;
        node = node->GetNext();
    }
    return count == 0;
}

bool cbProject::ShowOptions()
{
    ProjectOptionsDlg dlg(Manager::Get()->GetAppWindow(), this);
    return dlg.ShowModal() == wxID_OK;
}

int cbProject::SelectTarget(int initial, bool evenIfOne)
{
	if (!evenIfOne && GetBuildTargetsCount() == 1)
		return 0;

	SelectTargetDlg dlg(0L, this, initial);
	if (dlg.ShowModal() == wxID_OK)
		return dlg.GetSelection();
	return -1;
}

// Build targets

ProjectBuildTarget* cbProject::AddDefaultBuildTarget()
{
    return AddBuildTarget("default");
}

ProjectBuildTarget* cbProject::AddBuildTarget(const wxString& targetName)
{
    ProjectBuildTarget* target = new ProjectBuildTarget();
    target->m_Filename = m_Filename; // really important
    target->SetTitle(targetName);
    target->SetOutputFilename(GetOutputFilename());
    target->SetWorkingDir(".");
    target->SetObjectOutput(".objs");
    target->SetDepsOutput(".deps");
    m_Targets.Add(target);

    SetModified(true);
    return target;
}

bool cbProject::RenameBuildTarget(int index, const wxString& targetName)
{
    ProjectBuildTarget* target = GetBuildTarget(index);
    if (target)
    {
        int count = GetFilesCount();
        for (int i = 0; i < count; ++i)
        {
            ProjectFile* pf = GetFile(i);
            pf->RenameBuildTarget(target->GetTitle(), targetName);
        }
        target->SetTitle(targetName);
        SetModified(true); 
        return true;
    }
    return false;
}

bool cbProject::RenameBuildTarget(const wxString& oldTargetName, const wxString& newTargetName)
{
    return RenameBuildTarget(IndexOfBuildTargetName(oldTargetName), newTargetName);
}

bool cbProject::RemoveBuildTarget(int index)
{
    ProjectBuildTarget* target = GetBuildTarget(index);
    if (target)
    {
        int count = GetFilesCount();
        for (int i = 0; i < count; ++i)
        {
            ProjectFile* pf = GetFile(i);
            pf->RemoveBuildTarget(target->GetTitle());
        }
        delete target;
        m_Targets.RemoveAt(index);
        SetModified(true); 
        return true;
    }
    return false;
}

bool cbProject::RemoveBuildTarget(const wxString& targetName)
{
    return RemoveBuildTarget(IndexOfBuildTargetName(targetName));
}

int cbProject::IndexOfBuildTargetName(const wxString& targetName)
{
    for (unsigned int i = 0; i < m_Targets.GetCount(); ++i)
    {
        ProjectBuildTarget* target = m_Targets[i];
        if (target->GetTitle().Matches(targetName))
            return i;
    }
    return -1;
}

bool cbProject::SetActiveBuildTarget(int target)
{
    if (target == m_ActiveTarget)
        return true;
    m_ActiveTarget = target;
    SetModified(true);
    return true;
}

int cbProject::GetActiveBuildTarget()
{
    if (m_ActiveTarget < -1 || m_ActiveTarget >= (int)m_Targets.GetCount())
        m_ActiveTarget = -1;
    return m_ActiveTarget;
}

void cbProject::SetDefaultExecuteTargetIndex(int index)
{
    if (m_DefaultExecuteTarget != index)
    {
        m_DefaultExecuteTarget = index;
        SetModified(true);
    }
}

int cbProject::GetDefaultExecuteTargetIndex()
{
    if (m_DefaultExecuteTarget == -1)
    {
        // first-time: find the first target that provides executable...
        for (unsigned int i = 0; i < m_Targets.GetCount(); ++i)
        {
            ProjectBuildTarget* target = m_Targets[i];
            if (target->GetTargetType() == ttExecutable ||
                target->GetTargetType() == ttConsoleOnly)
            {
                m_DefaultExecuteTarget = i;
                break;
            }
        }
    }
    return m_DefaultExecuteTarget;
}

ProjectBuildTarget* cbProject::GetBuildTarget(int index)
{
    if (index >= 0 && index < (int)m_Targets.GetCount())
        return m_Targets[index];
    return 0L;
}

ProjectBuildTarget* cbProject::GetBuildTarget(const wxString& targetName)
{
    int idx = IndexOfBuildTargetName(targetName);
    return GetBuildTarget(idx);
}

void cbProject::ReOrderTargets(const wxArrayString& nameOrder)
{
    MessageManager* msgMan = Manager::Get()->GetMessageManager();
    if (nameOrder.GetCount() != m_Targets.GetCount())
    {
        msgMan->DebugLog(_("cbProject::ReOrderTargets() : Count does not match (%d sent, %d had)..."), nameOrder.GetCount(), m_Targets.GetCount());
        return;
    }
    
    for (unsigned int i = 0; i < nameOrder.GetCount(); ++i)
    {
        ProjectBuildTarget* target = GetBuildTarget(nameOrder[i]);
        if (!target)
        {
            msgMan->DebugLog(_("cbProject::ReOrderTargets() : Target \"%s\" not found..."), nameOrder[i].c_str());
            break;
        }
        
        m_Targets.Remove(target);
        m_Targets.Insert(target, i);
    }
    SetModified(true); 
}

#ifdef USE_OPENFILES_TREE
bool MiscTreeItemData::OwnerCheck(wxTreeEvent& event,wxTreeCtrl *tree,wxEvtHandler *handler,bool strict)
{
    if(!tree)   // No tree to get data from - ignore event
        return false;
        
    MiscTreeItemData* data = 
        (MiscTreeItemData*)tree->GetItemData(event.GetItem());
    if(!data)
    {
        if(!strict)
            return true; // On doubt, allow event
        else
        {
            event.Skip();
            return false;
        }         
    }
    wxEvtHandler *h = data->GetOwner();
    if((h && h!=handler) || (strict && !h))
    {   // Tree Item belongs to another handler - skip
        event.Skip(); 
        return false;
    }
    return true;
}
#endif
