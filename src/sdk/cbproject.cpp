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
* $Revision$
* $Id$
* $HeadURL$
*/

#include "sdk_precomp.h"

#ifndef wxUSE_CHOICEDLG
    #define wxUSE_CHOICEDLG 1
#endif

#include <wx/choicdlg.h>

#ifndef CB_PRECOMP
    #include "cbproject.h" // class's header file
    #include "sdk_events.h"
    #include "manager.h"
    #include "cbeditor.h"
    #include "globals.h"
    #include "pluginmanager.h"
    #include "projectmanager.h"
    #include "messagemanager.h"
    #include "editormanager.h"
    #include "configmanager.h"
    #include "compilerfactory.h"
#endif

#include <map>
#include "projectoptionsdlg.h"
#include "projectloader.h"
#include "devcpploader.h"
#include "msvcloader.h"
#include "msvc7loader.h"
#include "projectlayoutloader.h"
#include "selecttargetdlg.h"
#include "filegroupsandmasks.h"
#include "filefilters.h"
#include "importers_globals.h"
#include "annoyingdialog.h"

// class constructor
cbProject::cbProject(const wxString& filename)
    : m_ActiveTarget(-1),
    m_LastSavedActiveTarget(-1),
    m_DefaultExecuteTarget(-1),
    m_Makefile(_T("")),
    m_CustomMakefile(false),
    m_Loaded(false),
    m_CurrentlyLoading(false),
    m_BasePath(_T("")),
    m_PCHMode(pchObjectDir),
    m_CurrentlyCompilingTarget(0)
{
    SetCompilerID(CompilerFactory::GetDefaultCompilerID());
    SetModified(false);

	m_Files.Clear();
    if (!filename.IsEmpty() && wxFileExists(filename))
    {
		// existing project
		m_Filename = filename;
        m_BasePath = GetBasePath();
        Open();
    }
    else
    {
		// new project
        SetModified(true);
		if (filename.IsEmpty())
		{
	        m_Filename = CreateUniqueFilename();
    	    m_Loaded = SaveAs();
		}
		else
		{
	        m_Filename = filename;
    	    m_Loaded = Save();
		}
		if (m_Loaded)
		{
            wxFileName fname(m_Filename);
			m_Title = fname.GetName();
            m_BasePath = GetBasePath();
			m_CommonTopLevelPath = GetBasePath() + wxFileName::GetPathSeparator();

            // moved to ProjectManager::LoadProject()
            // see explanation there...
//			NotifyPlugins(cbEVT_PROJECT_OPEN);
		}
    }
}

// class destructor
cbProject::~cbProject()
{
    // moved to ProjectManager::CloseProject()
    // see explanation there...
//	NotifyPlugins(cbEVT_PROJECT_CLOSE);

	ClearAllProperties();
}

void cbProject::NotifyPlugins(wxEventType type)
{
	CodeBlocksEvent event(type);
	event.SetProject(this);
	Manager::Get()->GetPluginManager()->NotifyPlugins(event);
}

void cbProject::SetCompilerID(const wxString& id)
{
// TODO (mandrav##): Is this needed? The project's compiler has nothing to do with the targets' compilers...


    if (!CompilerFactory::GetCompiler(id))
        return; // Invalid compiler

    CompileTargetBase::SetCompilerID(id);
    if (id != GetCompilerID())
    {
        // update object filenames
        for (unsigned int i = 0; i < m_Targets.GetCount(); ++i)
        {
            ProjectBuildTarget* target = m_Targets[i];
            if (target)
            {
                Compiler* compiler = CompilerFactory::GetCompiler(target->GetCompilerID());
                if (!compiler)
                    continue;

                int count = GetFilesCount();
                for (int i = 0; i < count; ++i)
                {
                    ProjectFile* pf = GetFile(i);
                    wxFileName obj(pf->GetObjName());
                    if (FileTypeOf(pf->relativeFilename) != ftResource &&
                        obj.GetExt() == compiler->GetSwitches().objectExtension)
                    {
                        obj.SetExt(compiler->GetSwitches().objectExtension);
                        pf->SetObjName(obj.GetFullName());
                    }
                }
            }
        }
    }
}

bool cbProject::GetModified()
{
    // check base options
	if (CompileOptionsBase::GetModified())
		return true;

    // check active target
//    if (m_LastSavedActiveTarget != m_ActiveTarget)
//        return true;

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

    if (!modified)
        m_LastSavedActiveTarget = m_ActiveTarget;
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
    tmp << prefix << wxString::Format(_T("%d"), iter);
    while (!ok)
    {
        tmp.Clear();
        tmp << prefix << wxString::Format(_T("%d"), iter);

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
    return tmp << _T(".") << FileFilters::CODEBLOCKS_EXT;
}

void cbProject::ClearAllProperties()
{
    m_Files.DeleteContents(true);
    m_Files.Clear();
    m_Files.DeleteContents(false);
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
        cbMessageBox(msg, _("Error"), wxOK | wxCENTRE | wxICON_ERROR);
        return;
    }

	bool fileUpgraded = false;
	bool fileModified = false;
    wxFileName fname(m_Filename);
	FileType ft = FileTypeOf(m_Filename);
    if (ft == ftCodeBlocksProject)
    {
		Manager::Get()->GetMessageManager()->AppendLog(_("Opening %s: "), m_Filename.c_str());
        m_CurrentlyLoading = true;
        ProjectLoader loader(this);
        m_Loaded = loader.Open(m_Filename);
        fileUpgraded = loader.FileUpgraded();
        fileModified = loader.FileModified();
        m_CurrentlyLoading = false;
    }
    else
    {
        Manager::Get()->GetMessageManager()->AppendLog(_("Importing %s: "), m_Filename.c_str());
        IBaseLoader* loader = 0L;
        switch (ft)
        {
             case ftDevCppProject: loader = new DevCppLoader(this); break;
             case ftMSVC6Project: loader = new MSVCLoader(this); break;
             case ftMSVC7Project: loader = new MSVC7Loader(this); break;
             default:
                 Manager::Get()->GetMessageManager()->Log(_("failed."));
                 return;
        }

        wxString compilerID;
        if (ImportersGlobals::UseDefaultCompiler)
            compilerID = CompilerFactory::GetDefaultCompilerID();
        else
        {
            // select compiler for the imported project
            // need to do it before actual import, because the importer might need
            // project's compiler information (like the object files extension etc).
            Compiler* compiler = CompilerFactory::SelectCompilerUI(_("Select compiler for ") + wxFileName(m_Filename).GetFullName());
            if (compiler)
                compilerID = compiler->GetID();
        }

        if (!compilerID.IsEmpty())
        {
            SetCompilerID(compilerID);

            // actually import project file
            m_CurrentlyLoading = true;
            m_Loaded = loader->Open(m_Filename);
            fname.SetExt(FileFilters::CODEBLOCKS_EXT);
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
		SetModified(ft != ftCodeBlocksProject || fileUpgraded || fileModified);

		// moved to ProjectManager::LoadProject()
		// see explanation there...
//		NotifyPlugins(cbEVT_PROJECT_OPEN);

		if (fileUpgraded)
		{
            wxString msg;
            msg.Printf(_("The project file of \"%s\" needs to be updated to the latest format.\n"
                        "This will happen automatically when you save the project."), m_Title.c_str());
            AnnoyingDialog dlg(_("Project file format will be updated on save"),
                                msg,
                                wxART_INFORMATION,
                                AnnoyingDialog::OK,
                                wxID_OK);
            dlg.ShowModal();
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
    Manager::Get()->GetMessageManager()->DebugLog(_T("Project's base path: %s"), base.GetFullPath().c_str());
    for (FilesList::Node* node = m_Files.GetFirst(); node; node = node->GetNext())
    {
        ProjectFile* f = node->GetData();
        wxString tmp = f->relativeFilename;
        wxFileName tmpbase = GetBasePath() + sep;
        while (tmp.StartsWith(_T("..")))
        {
            tmpbase.AppendDir(_T(".."));
            tmp.Remove(0, 2); // two dots
            // remove separator(s) after dots
            while (!tmp.IsEmpty() &&  (tmp.GetChar(0) == _T('/') || tmp.GetChar(0) == _T('\\')))
                tmp.Remove(0, 1);
        }
        tmpbase.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE);

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
    Manager::Get()->GetMessageManager()->DebugLog(_T("Project's common toplevel path: %s"), m_CommonTopLevelPath.c_str());
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
                    FileFilters::GetFilterString(_T('.') + FileFilters::CODEBLOCKS_EXT),
                    wxSAVE | wxOVERWRITE_PROMPT);

    PlaceWindow(&dlg);
    if (dlg.ShowModal() != wxID_OK)
        return false;
    m_Filename = dlg.GetPath();
    fname.Assign(m_Filename);

    // make sure the project file uses the correct extension
    // we don't use wxFileName::SetExt() because if the user has added a dot
    // in the filename, the part after it would be interpeted as extension
    // (and it might not be)
    // so we just append the correct extension
    if (!fname.GetExt().Matches(FileFilters::CODEBLOCKS_EXT))
        fname.Assign(m_Filename + _T('.') + FileFilters::CODEBLOCKS_EXT);

//    Manager::Get()->GetProjectManager()->GetTree()->SetItemText(m_ProjectNode, fname.GetFullName());
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
	fname.SetExt(_T("layout"));
    ProjectLayoutLoader loader(this);
    return loader.Save(fname.GetFullPath());
}

bool cbProject::LoadLayout()
{
   if (m_Filename.IsEmpty())
        return false;
    int openmode = Manager::Get()->GetConfigManager(_T("project_manager"))->ReadInt(_T("/open_files"), (long int)1);
    bool result = false;

    if(openmode==2)
    {
        // Do not open any files
        result = true;
    }
    else
    {
        Manager::Get()->GetEditorManager()->HideNotebook();
        if(openmode == 0) // Open all files
        {
            FilesList::Node* node = m_Files.GetFirst();
            while(node)
            {
                ProjectFile* f = node->GetData();
                Manager::Get()->GetEditorManager()->Open(f->file.GetFullPath(),0,f);
                node = node->GetNext();
            }
            result = true;
        }
        else if(openmode == 1)// Open last open files
        {
            wxFileName fname(m_Filename);
            fname.SetExt(_T("layout"));
            ProjectLayoutLoader loader(this);
            if (loader.Open(fname.GetFullPath()))
            {
            	typedef std::map<int, ProjectFile*> open_files_map;
            	open_files_map open_files;

            	// Get all files to open and sort them according to their tab-position:
                FilesList::Node* node = m_Files.GetFirst();
                while(node)
                {
                    ProjectFile* f = node->GetData();
                    if (f->editorOpen)
						open_files[f->editorTabPos] = f;
                    node = node->GetNext();
                }

				// Open all requested files:
				for (open_files_map::iterator it = open_files.begin(); it != open_files.end(); ++it)
                {
                    cbEditor* ed = Manager::Get()->GetEditorManager()->Open((*it).second->file.GetFullPath(),0,(*it).second);
                    if (ed)
                        ed->SetProjectFile((*it).second);
                }

                ProjectFile* f = loader.GetTopProjectFile();
                if (f)
                {
                    Manager::Get()->GetMessageManager()->DebugLog(_T("Top Editor: %s"),f->file.GetFullPath().c_str());
                    EditorBase* eb = Manager::Get()->GetEditorManager()->Open(f->file.GetFullPath());
                    if(eb)
                    {
                        Manager::Get()->GetProjectManager()->SetTopEditor(eb);
                        eb->Activate();
                    }
                }
//                Manager::Get()->GetAppWindow()->Thaw();
            }
            result = true;
        }
        else
            result = false;
        Manager::Get()->GetEditorManager()->ShowNotebook();
    }
    return result;
}

ProjectFile* cbProject::AddFile(const wxString& targetName, const wxString& filename, bool compile, bool link, unsigned short int weight)
{
    int idx = IndexOfBuildTargetName(targetName);
    return AddFile(idx, filename, compile, link, weight);
}

ProjectFile* cbProject::AddFile(int targetIndex, const wxString& filename, bool compile, bool link, unsigned short int weight)
{
//  NOTE (Rick#1#): When loading the project, do not search for existing files
//  (Assumming that there are no duplicate entries in the .cbp file)
//  This saves us a lot of processing when loading large projects.
//  Remove the if to do the search anyway

//  NOTE (mandrav#1#): We can't ignore that because even if we can rely on .cbp
//  containing discrete files, we can't do that for imported projects...
//  This means we have to search anyway.
//  NP though, I added a hashmap for fast searches in GetFileByFilename()

/* NOTE (mandrav#1#): Calling GetFileByFilename() twice, is costly.
    Instead of searching for duplicate files when entering here,
    we 'll search before exiting.
    The rationale is that by then, we 'll have the relative filename
    in our own representation and this will make everything quicker
    (check GetFileByFilename implementation to understand why)...
*/
//    f = GetFileByFilename(filename, true, true);
//    if (!f)
//        f = GetFileByFilename(filename, false, true);
//    if (f)
//    {
//        if (targetIndex >= 0 && targetIndex < (int)m_Targets.GetCount())
//            f->AddBuildTarget(m_Targets[targetIndex]->GetTitle());
//        return f;
//    }

    // quick test
    ProjectFile* f = m_ProjectFilesMap[UnixFilename(filename)];
    if (f)
        return f;

    // create file
    f = new ProjectFile(this);
    bool localCompile, localLink;
    wxFileName fname;
    wxString ext;

	f->project = this;
	f->editorOpen = false;
	f->editorPos = 0;
	f->editorTopLine = 0;
    f->weight = weight;

	FileType ft = FileTypeOf(filename);
	fname = filename; //UnixFilename(filename);
	ext = filename.AfterLast(_T('.')).Lower();
	if (ext.Matches(FileFilters::C_EXT) || ext.Matches(FileFilters::CC_EXT))
        f->compilerVar = _T("CC");
#ifdef __WXMSW__
	else if (ext.Matches(FileFilters::RESOURCE_EXT))
        f->compilerVar = _T("WINDRES");
#endif
    else
        f->compilerVar = _T("CPP"); // default

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

    // add the build target
    if (targetIndex >= 0 && targetIndex < (int)m_Targets.GetCount())
        f->AddBuildTarget(m_Targets[targetIndex]->GetTitle());

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

    wxString local_filename = filename;

#ifdef __WXMSW__
    // for windows, make sure the filename is not on another drive...
    if (local_filename.Length() > 1 &&
        local_filename.GetChar(1) == _T(':') && // quick test to avoid the costly wxFileName ctor below
        fname.GetVolume() != wxFileName(m_Filename).GetVolume())
    {
        fname.Assign(filename);
    }
    else
#endif
    {
        // make sure the filename is relative to the project's base path
        if (fname.IsAbsolute())
        {
            fname.MakeRelativeTo(GetBasePath());
            local_filename = fname.GetFullPath();
        }
        fname.Assign(GetBasePath() + wxFILE_SEP_PATH + local_filename);
    }
    NormalizePath(fname, GetBasePath());

    wxString fullFilename = fname.GetFullPath();
    f->file.Assign(fname);
    f->relativeFilename = UnixFilename(local_filename);

    // now check if we have already added this file
    // if we have, return the existing file, but add the specified target
    ProjectFile* existing = GetFileByFilename(f->relativeFilename, true, true);
    if (existing == f)
    {
        delete f;
        if (targetIndex >= 0 && targetIndex < (int)m_Targets.GetCount())
            existing->AddBuildTarget(m_Targets[targetIndex]->GetTitle());
        return existing;
    }

    m_Files.Append(f);
    if (!m_CurrentlyLoading)
    {
        // check if we really need to recalculate the common top-level path for the project
        if (!fullFilename.StartsWith(m_CommonTopLevelPath))
            CalculateCommonTopLevelPath();
        else
        {
            // set f->relativeToCommonTopLevelPath
            f->relativeToCommonTopLevelPath = fullFilename.Right(fullFilename.Length() - m_CommonTopLevelPath.Length());
        }
    }
    SetModified(true);
    m_ProjectFilesMap[UnixFilename(f->relativeFilename)] = f; // add to hashmap

    if (!wxFileExists(fullFilename))
        f->SetFileState(fvsMissing);
    else if (!wxFile::Access(fullFilename.c_str(), wxFile::write)) // readonly
        f->SetFileState(fvsReadOnly);

	return f;
}

bool cbProject::RemoveFile(ProjectFile* pf)
{
    if (!pf)
        return false;
    m_ProjectFilesMap.erase(UnixFilename(pf->relativeFilename)); // remove from hashmap
    Manager::Get()->GetEditorManager()->Close(pf->file.GetFullPath());

    FilesList::Node* node = m_Files.Find(pf);
    if (!node)
    {
        Manager::Get()->GetMessageManager()->DebugLog(_T("Can't locate node for ProjectFile* !"));
    }
    else
    {
        m_Files.DeleteNode(node);
    }

    // remove this file from all targets too
    for (unsigned int i = 0; i < m_Targets.GetCount(); ++i)
    {
        ProjectBuildTarget* target = m_Targets[i];
        if (target)
        {
            target->GetFilesList().DeleteObject(pf);
        }
    }
    delete pf;

    SetModified(true);
	return true;
}

bool cbProject::RemoveFile(int index)
{
    if (index < 0 || index >= (int)m_Files.GetCount())
        return false; // invalid index
    ProjectFile* f = m_Files[index];
    return RemoveFile(f);
}

int filesSort(const ProjectFile** arg1, const ProjectFile** arg2)
{
    return (*arg1)->file.GetFullPath().CompareTo((*arg2)->file.GetFullPath());
}

void cbProject::BuildTree(wxTreeCtrl* tree, const wxTreeItemId& root, bool categorize, bool useFolders, FilesGroupsAndMasks* fgam)
{
    if (!tree)
        return;

    int fldIdx = Manager::Get()->GetProjectManager()->FolderIconIndex();
    int prjIdx = Manager::Get()->GetProjectManager()->ProjectIconIndex();

    //sort list of files
    m_Files.Sort(filesSort);

    FileTreeData* ftd = new FileTreeData(this, FileTreeData::ftdkProject);
    m_ProjectNode = tree->AppendItem(root, GetTitle(), prjIdx, prjIdx, ftd);
    wxTreeItemId others = m_ProjectNode;

    // create file-type categories nodes (if enabled)
    wxTreeItemId* pGroupNodes = 0L;
    if (categorize && fgam)
    {
        pGroupNodes = new wxTreeItemId[fgam->GetGroupsCount()];
        for (unsigned int i = 0; i < fgam->GetGroupsCount(); ++i)
        {
            ftd = new FileTreeData(this, FileTreeData::ftdkVirtualGroup);
            ftd->SetFolder(fgam->GetGroupName(i));
            pGroupNodes[i] = tree->AppendItem(m_ProjectNode, fgam->GetGroupName(i), fldIdx, fldIdx, ftd);
        }
        // add a default category "Others" for all non-matching file-types
        others = tree->AppendItem(m_ProjectNode, _("Others"), fldIdx, fldIdx);
    }

    // iterate all project files and add them to the tree
    int count = 0;
    for (FilesList::Node* node = m_Files.GetFirst(); node; node = node->GetNext())
    {
        ProjectFile* f = node->GetData();
        ftd = new FileTreeData(this, FileTreeData::ftdkFile);
        ftd->SetFileIndex(count++);
        ftd->SetProjectFile(f);
        ftd->SetFolder(f->file.GetFullPath());

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
        f->m_TreeItemId = AddTreeNode(tree, f->relativeToCommonTopLevelPath, parentNode, useFolders, f->compile, (int)f->m_VisualState, ftd);
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

// helper function used by AddTreeNode
static wxString GetRelativeFolderPath(wxTreeCtrl* tree, wxTreeItemId parent)
{
    wxString fld;
    while (parent.IsOk())
    {
        FileTreeData* ftd = (FileTreeData*)tree->GetItemData(parent);
        if (!ftd || ftd->GetKind() != FileTreeData::ftdkFolder)
            break;
        fld.Prepend(tree->GetItemText(parent) + wxFILE_SEP_PATH);
        parent = tree->GetItemParent(parent);
    }
    return fld;
}

wxTreeItemId cbProject::AddTreeNode(wxTreeCtrl* tree, const wxString& text, const wxTreeItemId& parent, bool useFolders, bool compiles, int image, FileTreeData* data)
{
    // see if the text contains any path info, e.g. plugins/compilergcc/compilergcc.cpp
    // in that case, take the first element (plugins in this example), create a sub-folder
    // with the same name and recurse with the result...

    wxTreeItemId ret;

    wxString path = text;
#ifdef __WXMSW__
    // special case for windows and files on a different drive
    if (path.Length() > 1 && path.GetChar(1) == _T(':'))
        path.Remove(1, 1);
#endif
    int pos = path.Find(_T('/'));
    if (pos == -1)
        pos = path.Find(_T('\\'));
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
            int fldIdx = Manager::Get()->GetProjectManager()->FolderIconIndex();
#if (wxMAJOR_VERSION == 2) && (wxMINOR_VERSION < 5)
            long int cookie2 = 0;
#else
            wxTreeItemIdValue cookie2; //2.6.0
#endif
			wxTreeItemId child = tree->GetFirstChild(parent, cookie2);
			wxTreeItemId lastChild;
			while (child)
			{
				if (tree->GetItemImage(child) == fldIdx)
				{
					if (folder.CompareTo(tree->GetItemText(child)) < 0)
					{
					    FileTreeData* ftd = new FileTreeData(*data);
					    ftd->SetKind(FileTreeData::ftdkFolder);
					    ftd->SetFolder(m_CommonTopLevelPath + GetRelativeFolderPath(tree, parent) + folder + wxFILE_SEP_PATH);
					    ftd->SetProjectFile(0);
						newparent = tree->InsertItem(parent, lastChild, folder, fldIdx, fldIdx, ftd);
						break;
					}
				}
				else
				{
                    FileTreeData* ftd = new FileTreeData(*data);
                    ftd->SetKind(FileTreeData::ftdkFolder);
                    ftd->SetFolder(m_CommonTopLevelPath + GetRelativeFolderPath(tree, parent) + folder + wxFILE_SEP_PATH);
                    ftd->SetProjectFile(0);
					newparent = tree->PrependItem(parent, folder, fldIdx, fldIdx, ftd);
					break;
				}
				lastChild = child;
				child = tree->GetNextChild(parent, cookie2);
			}
			if (!newparent)
			{
                FileTreeData* ftd = new FileTreeData(*data);
                ftd->SetKind(FileTreeData::ftdkFolder);
                ftd->SetFolder(m_CommonTopLevelPath + GetRelativeFolderPath(tree, parent) + folder + wxFILE_SEP_PATH);
                ftd->SetProjectFile(0);
				newparent = tree->AppendItem(parent, folder, fldIdx, fldIdx, ftd);
			}
		}
		//tree->SortChildren(parent);
        ret = AddTreeNode(tree, path, newparent, true, compiles, image, data);
    }
    else
	{
        ret = tree->AppendItem(parent, text, image, image, data);
		if (!compiles)
			tree->SetItemTextColour(ret, wxColour(0x80, 0x80, 0x80));
	}
	return ret;
}

void cbProject::RenameInTree(const wxString &newname)
{
    wxTreeCtrl* tree = Manager::Get()->GetProjectManager()->GetTree();
    if(!tree || !m_ProjectNode)
        return;
    tree->SetItemText(m_ProjectNode, newname);
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
    makefile.SetName(_T("Makefile"));
    makefile.SetExt(_T(""));
    makefile.MakeRelativeTo(GetBasePath());

    m_Makefile = makefile.GetFullPath();

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
        if (tmp.StartsWith(_T(".\\")) ||
            tmp.StartsWith(_T("./")))
        {
            tmp.Remove(0, 2);
        }
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
    Manager::Get()->GetEditorManager()->HideNotebook();
    FilesList::Node* node = m_Files.GetFirst();
    while(node)
    {
        ProjectFile* f = node->GetData();
        Manager::Get()->GetEditorManager()->Close(f->file.GetFullPath(),true);
        delete f;
        delete node;
        node = m_Files.GetFirst();
    }
    Manager::Get()->GetEditorManager()->ShowNotebook();
    return true;
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
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        // update file details
        FilesList::Node* node = m_Files.GetFirst();
        while(node)
        {
            ProjectFile* f = node->GetData();
            f->UpdateFileDetails();
            node = node->GetNext();
        }
        return true;
    }
    return false;
}

int cbProject::SelectTarget(int initial, bool evenIfOne)
{
	if (!evenIfOne && GetBuildTargetsCount() == 1)
		return 0;

	SelectTargetDlg dlg(0L, this, initial);
    PlaceWindow(&dlg);
	if (dlg.ShowModal() == wxID_OK)
		return dlg.GetSelection();
	return -1;
}

// Build targets

ProjectBuildTarget* cbProject::AddDefaultBuildTarget()
{
    return AddBuildTarget(_T("default"));
}

ProjectBuildTarget* cbProject::AddBuildTarget(const wxString& targetName)
{
    ProjectBuildTarget* target = new ProjectBuildTarget(this);
    target->m_Filename = m_Filename; // really important
    target->SetTitle(targetName);
    target->SetCompilerID(GetCompilerID()); // same compiler as project's
    target->SetOutputFilename(wxFileName(GetOutputFilename()).GetFullName());
    target->SetWorkingDir(_T("."));
    target->SetObjectOutput(_T(".objs"));
    target->SetDepsOutput(_T(".deps"));
    m_Targets.Add(target);

    SetModified(true);

    NotifyPlugins(cbEVT_PROJECT_TARGETS_MODIFIED);
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
        NotifyPlugins(cbEVT_PROJECT_TARGETS_MODIFIED);
        return true;
    }
    return false;
}

bool cbProject::RenameBuildTarget(const wxString& oldTargetName, const wxString& newTargetName)
{
    return RenameBuildTarget(IndexOfBuildTargetName(oldTargetName), newTargetName);
}

ProjectBuildTarget* cbProject::DuplicateBuildTarget(int index, const wxString& newName)
{
    ProjectBuildTarget* newTarget = 0;
    ProjectBuildTarget* target = GetBuildTarget(index);
    if (target)
    {
        newTarget = new ProjectBuildTarget(*target);
        wxString newTargetName = !newName.IsEmpty() ? newName : (_("Copy of ") + target->GetTitle());
        newTarget->SetTitle(newTargetName);
        // just notify the files of this target that they belong to the new target too
        for (FilesList::Node* it = newTarget->GetFilesList().GetFirst(); it; it = it->GetNext())
        {
            ProjectFile* pf = it->GetData();
            pf->AddBuildTarget(newTargetName);
        }
        SetModified(true);
        m_Targets.Add(newTarget);
        NotifyPlugins(cbEVT_PROJECT_TARGETS_MODIFIED);
    }
    return newTarget;
}

ProjectBuildTarget* cbProject::DuplicateBuildTarget(const wxString& targetName, const wxString& newName)
{
    return DuplicateBuildTarget(IndexOfBuildTargetName(targetName), newName);
}

bool cbProject::ExportTargetAsProject(int index)
{
    ProjectBuildTarget* target = GetBuildTarget(index);
    if (!target)
        return false;
    return ExportTargetAsProject(target->GetTitle());
}

bool cbProject::ExportTargetAsProject(const wxString& targetName)
{
    ProjectBuildTarget* target = GetBuildTarget(targetName);
    if (!target)
        return false;

    // ask for the new project's name
    wxString newName = wxGetTextFromUser(_("Please enter the new project's name (no path, no extension)."),
                                        _("Export target as new project"),
                                        target->GetTitle());
    if (newName.IsEmpty())
        return false;
    wxFileName fname(GetFilename());
    fname.SetName(newName);

    Save();
    bool alreadyModified = GetModified();
    wxString oldTitle = GetTitle();
    SetTitle(targetName);

    ProjectLoader loader(this);
    bool ret = loader.ExportTargetAsProject(fname.GetFullPath(), target->GetTitle());

    SetTitle(oldTitle);
    if (!alreadyModified)
        SetModified(false);

    return ret;
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
        NotifyPlugins(cbEVT_PROJECT_TARGETS_MODIFIED);
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

bool cbProject::SetActiveBuildTarget(int index)
{
    if (index < -1 || index >= GetBuildTargetsCount())
        return false;
    if (index == m_ActiveTarget)
        return true;
    m_ActiveTarget = index;
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
        msgMan->DebugLog(_T("cbProject::ReOrderTargets() : Count does not match (%d sent, %d had)..."), nameOrder.GetCount(), m_Targets.GetCount());
        return;
    }

    for (unsigned int i = 0; i < nameOrder.GetCount(); ++i)
    {
        ProjectBuildTarget* target = GetBuildTarget(nameOrder[i]);
        if (!target)
        {
            msgMan->DebugLog(_T("cbProject::ReOrderTargets() : Target \"%s\" not found..."), nameOrder[i].c_str());
            break;
        }

        m_Targets.Remove(target);
        m_Targets.Insert(target, i);
    }
    SetModified(true);
}

void cbProject::SetCurrentlyCompilingTarget(ProjectBuildTarget* bt)
{
    m_CurrentlyCompilingTarget = bt;
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
